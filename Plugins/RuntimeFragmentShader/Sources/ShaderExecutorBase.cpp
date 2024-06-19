#include "ShaderExecutorBase.h"

ShaderExecutorBase::ShaderExecutorBase(IUnityInterfaces* unity)
{
    _unity = unity;
    _device = _unity->Get<IUnityGraphicsD3D11>()->GetDevice();
    _logger = _unity->Get<IUnityLog>();

    _constantBuffer = nullptr;
    _constantBufferPtr = nullptr;
    _constantBufferSize = 0;
}

ShaderExecutorBase::~ShaderExecutorBase()
{
    SAFE_RELEASE(_constantBuffer);

    for (auto buf : _buffers)
    {
        buf.second->~Buffer();
    }

    for (auto tex : _textures)
    {
        tex.second->~Texture2D();
    }
}

void ShaderExecutorBase::SetConstantBuffer(void* buffer, int size)
{
    if (size == 0) return;
    if (size == _constantBufferSize) return;
    _constantBufferPtr = buffer;
	
    if (_constantBuffer != nullptr)
    {
        _constantBuffer->Release();
        _constantBuffer = nullptr;
    }
	
    D3D11_SUBRESOURCE_DATA sr = { 0 };
    sr.pSysMem = buffer;
	
    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = size + (size % 16 == 0 ? 0 : 16 - size % 16);
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;
    HRESULT hr = _device->CreateBuffer(&desc, &sr, &_constantBuffer);
    if (FAILED(hr))
    {
        UNITY_LOG_ERROR(_logger, "[RuntimeShader] Failed to create constant buffer");
    }

    _constantBufferSize = size;
}

void ShaderExecutorBase::SetBuffer(int slot, void* buffer, int count, int stride)
{
    if (_buffers.count(slot) == 0)
    {
        _buffers[slot] = new Buffer();
    }

    HRESULT hr = _buffers[slot]->UpdateBuffer(_device, buffer, count, stride);
    if (FAILED(hr))
    {
        UNITY_LOG_ERROR(_logger, ("[KernelDispatcher] Failed to update buffer: " + std::to_string(hr)).c_str());
        return;
    }
}

void ShaderExecutorBase::SetTexture(int slot, void* ptr, int format)
{
    if (_textures.count(slot) == 0)
    {
        _textures[slot] = new Texture2D();
    }

    HRESULT hr = _textures[slot]->UpdateTexture(_device, ptr, format);
    if (FAILED(hr))
    {
        UNITY_LOG_ERROR(_logger, ("[ShaderRenderer] Failed to update texture: " + std::to_string(hr)).c_str());
        return;
    }
}
