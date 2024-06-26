#include "Buffer.h"

using namespace UnityRuntimeShader;

Buffer::Buffer()
{
    _shaderResourceView = nullptr;
    _desc = {};
}

Buffer::~Buffer()
{
    _shaderResourceView = nullptr;
}

HRESULT Buffer::UpdateBuffer(ID3D11Device* device, void* buffer, int count, int stride)
{
    if (_count == count && _stride == stride && _shaderResourceView != nullptr)
    {
        return S_OK;
    }
    
    if (_shaderResourceView != nullptr)
    {
        _shaderResourceView.Get()->Release();
        _shaderResourceView = nullptr;
    }
    
    _count = count;
    _stride = stride;
    
    ID3D11Buffer* bufferResource = static_cast<ID3D11Buffer*>(buffer);

    _desc = {};
    _desc.Format = DXGI_FORMAT_UNKNOWN;
    _desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    _desc.Buffer.FirstElement = 0;
    _desc.Buffer.NumElements = count;

    return device->CreateShaderResourceView(bufferResource, &_desc, _shaderResourceView.GetAddressOf());
}

