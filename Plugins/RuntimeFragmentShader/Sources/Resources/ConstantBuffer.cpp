#include "ConstantBuffer.h"

using namespace UnityRuntimeShader;

ConstantBuffer::ConstantBuffer()
{
    _constantBufferPointer = nullptr;
    _constantBufferSize = 0;
    _constantBuffer = nullptr;
    _subresourceData = { 0 };
    _desc = {};
}

ConstantBuffer::~ConstantBuffer()
= default;

HRESULT ConstantBuffer::UpdateBuffer(ID3D11Device* device, void* buffer, int size)
{
    if (size == 0) return S_OK;
    if (size == _constantBufferSize) return S_OK;
    _constantBufferPointer = buffer;
	
    if (_constantBuffer != nullptr)
    {
        _constantBuffer->Release();
        _constantBuffer = nullptr;
    }
	
    _subresourceData = { 0 };
    _subresourceData.pSysMem = buffer;
	
    _desc = {};
    _desc.Usage = D3D11_USAGE_DEFAULT;
    _desc.ByteWidth = size + (size % 16 == 0 ? 0 : 16 - size % 16);
    _desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    _desc.CPUAccessFlags = 0;
    HRESULT hr = device->CreateBuffer(&_desc, &_subresourceData, &_constantBuffer);
    if (FAILED(hr))
    {
        return hr;
    }

    _constantBufferSize = size;
    return S_OK;
}
