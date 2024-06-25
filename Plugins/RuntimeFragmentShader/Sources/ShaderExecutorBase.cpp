#include "ShaderExecutorBase.h"

ShaderExecutorBase::ShaderExecutorBase(IUnityInterfaces* unity)
{
    _unity = unity;
    _device = _unity->Get<IUnityGraphicsD3D11>()->GetDevice();
    _logger = _unity->Get<IUnityLog>();
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

    HRESULT hr = _buffers[slot]->UpdateBuffer(_device, buffer, count, stride);
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

    HRESULT hr = _textures[slot]->UpdateTexture(_device, ptr, format);
    if (FAILED(hr))
    {
        UNITY_LOG_ERROR(_logger, ("[ShaderRenderer] Failed to update texture: " + std::to_string(hr)).c_str());
    }
}
