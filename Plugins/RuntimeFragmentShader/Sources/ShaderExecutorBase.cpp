#include "ShaderExecutorBase.h"

using namespace UnityRuntimeShader;

ShaderExecutorBase::ShaderExecutorBase(IUnityInterfaces* unity)
{
    _unity = unity;
    _logger = _unity->Get<IUnityLog>();

    auto graphics = _unity->Get<IUnityGraphics>();
    if (graphics != nullptr && graphics->GetRenderer() == kUnityGfxRendererD3D12)
    {
        auto d3d12 = _unity->Get<IUnityGraphicsD3D12>();
        ID3D12Device* d3d12Device = d3d12 != nullptr ? d3d12->GetDevice() : nullptr;
        ID3D12CommandQueue* queue = d3d12 != nullptr ? d3d12->GetCommandQueue() : nullptr;
        if (d3d12Device != nullptr && queue != nullptr)
        {
            D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0 };
            ID3D11DeviceContext* context = nullptr;
            D3D_FEATURE_LEVEL achievedLevel = D3D_FEATURE_LEVEL_11_0;
            IUnknown* queues[] = { queue };
            HRESULT hr = D3D11On12CreateDevice(
                d3d12Device, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                levels, ARRAYSIZE(levels),
                queues, 1, 0,
                &_device, &context, &achievedLevel);
            if (SUCCEEDED(hr))
            {
                context->Release();
                _device->QueryInterface(IID_PPV_ARGS(&_d3d11On12));
                _isD3D12 = _d3d11On12 != nullptr;
                _ownsDevice = _isD3D12;
            }
        }
    }

    if (!_isD3D12)
        _device = _unity->Get<IUnityGraphicsD3D11>()->GetDevice();
}

ShaderExecutorBase::~ShaderExecutorBase()
{
    for (auto cbuf : _constantBuffers)
    {
        cbuf.second->~ConstantBuffer();
    }

    for (auto buf : _buffers)
    {
        buf.second->~Buffer();
    }

    for (auto tex : _textures)
    {
        tex.second->~Texture2D();
    }

    if (_ownsDevice && _device != nullptr)
        _device->Release();
}

void ShaderExecutorBase::SetConstantBuffer(int slot, void* buffer, int size)
{
    if (_constantBuffers.count(slot) == 0)
    {
        _constantBuffers[slot] = new ConstantBuffer();
    }

    HRESULT hr = _constantBuffers[slot]->UpdateBuffer(_device, buffer, size);
    if (FAILED(hr))
    {
        UNITY_LOG_ERROR(_logger, ("[KernelDispatcher] Failed to update constant buffer: " + std::to_string(hr)).c_str());
    }
}

void ShaderExecutorBase::SetBuffer(int slot, void* buffer, int count, int stride)
{
    if (_buffers.count(slot) == 0)
    {
        _buffers[slot] = new Buffer();
    }

    void* resource = _isD3D12
        ? WrapResource(buffer, D3D11_BIND_SHADER_RESOURCE,
            D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
            D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)
        : buffer;
    HRESULT hr = resource != nullptr
        ? _buffers[slot]->UpdateBuffer(_device, resource, count, stride)
        : E_INVALIDARG;
    if (FAILED(hr))
    {
        UNITY_LOG_ERROR(_logger, ("[KernelDispatcher] Failed to update buffer: " + std::to_string(hr)).c_str());
    }
}

void ShaderExecutorBase::SetTexture(int slot, void* ptr, int format)
{
    if (_textures.count(slot) == 0)
    {
        _textures[slot] = new Texture2D();
    }

    void* resource = _isD3D12
        ? WrapResource(ptr, D3D11_BIND_SHADER_RESOURCE,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
        : ptr;
    HRESULT hr = resource != nullptr
        ? _textures[slot]->UpdateTexture(_device, resource, format)
        : E_INVALIDARG;
    if (FAILED(hr))
    {
        UNITY_LOG_ERROR(_logger, ("[ShaderRenderer] Failed to update texture: " + std::to_string(hr)).c_str());
    }
}

ID3D11Resource* ShaderExecutorBase::WrapResource(void* resource, UINT bindFlags,
    D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState)
{
    if (!_isD3D12 || resource == nullptr)
        return static_cast<ID3D11Resource*>(resource);

    auto key = std::make_pair(resource, bindFlags);
    auto existing = _wrappedResources.find(key);
    if (existing != _wrappedResources.end()) return existing->second.Get();

    D3D11_RESOURCE_FLAGS flags = {};
    flags.BindFlags = bindFlags;
    ComPtr<ID3D11Resource> wrapped;
    HRESULT hr = _d3d11On12->CreateWrappedResource(
        static_cast<ID3D12Resource*>(resource), &flags,
        beforeState, afterState, IID_PPV_ARGS(&wrapped));
    if (FAILED(hr))
    {
        UNITY_LOG_ERROR(_logger, "[ShaderExecutorBase] Failed to wrap D3D12 resource.");
        return nullptr;
    }

    auto inserted = _wrappedResources.emplace(key, wrapped);
    return inserted.first->second.Get();
}

void ShaderExecutorBase::AcquireWrappedResources()
{
    if (!_isD3D12 || _wrappedResources.empty()) return;
    std::vector<ID3D11Resource*> resources;
    resources.reserve(_wrappedResources.size());
    for (auto& pair : _wrappedResources) resources.push_back(pair.second.Get());
    _d3d11On12->AcquireWrappedResources(resources.data(), static_cast<UINT>(resources.size()));
}

void ShaderExecutorBase::ReleaseWrappedResources()
{
    if (!_isD3D12 || _wrappedResources.empty()) return;
    std::vector<ID3D11Resource*> resources;
    resources.reserve(_wrappedResources.size());
    for (auto& pair : _wrappedResources) resources.push_back(pair.second.Get());
    _d3d11On12->ReleaseWrappedResources(resources.data(), static_cast<UINT>(resources.size()));
    ID3D11DeviceContext* context = nullptr;
    _device->GetImmediateContext(&context);
    if (context != nullptr)
    {
        context->Flush();
        context->Release();
    }
}
