#include "RwBuffer.h"

RwBuffer::RwBuffer()
{
    _unorderedAccessView = nullptr;
    _desc = {};
}

RwBuffer::~RwBuffer()
{
    _unorderedAccessView = nullptr;
}

HRESULT RwBuffer::UpdateBuffer(ID3D11Device* device, void* buffer, int count, int stride)
{
    if (_count == count && _stride == stride && _unorderedAccessView != nullptr)
    {
        return S_OK;
    }
    
    if (_unorderedAccessView != nullptr)
    {
        _unorderedAccessView.Get()->Release();
        _unorderedAccessView = nullptr;
    }

    _count = count;
    _stride = stride;

    ID3D11Buffer* bufferResource = static_cast<ID3D11Buffer*>(buffer);
    
    _desc = {};
    _desc.Format = DXGI_FORMAT_UNKNOWN;
    _desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    _desc.Buffer.FirstElement = 0;
    _desc.Buffer.NumElements = count;
    _desc.Buffer.Flags = 0;
    
    return device->CreateUnorderedAccessView(bufferResource, &_desc, _unorderedAccessView.GetAddressOf());
}

