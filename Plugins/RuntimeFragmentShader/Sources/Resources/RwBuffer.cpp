#include "RwBuffer.h"

RwBuffer::RwBuffer()
{
    _unorderedAccessView = nullptr;
    _desc = {};
}

RwBuffer::~RwBuffer()
{
    SAFE_RELEASE(_unorderedAccessView);
}

HRESULT RwBuffer::UpdateBuffer(ID3D11Device* device, void* buffer, int count, int stride)
{
    if (_unorderedAccessView != nullptr)
    {
        _unorderedAccessView->Release();
        _unorderedAccessView = nullptr;
    }

    ID3D11Buffer* bufferResource = static_cast<ID3D11Buffer*>(buffer);
    
    _desc = {};
    _desc.Format = DXGI_FORMAT_UNKNOWN;
    _desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    _desc.Buffer.FirstElement = 0;
    _desc.Buffer.NumElements = count;
    _desc.Buffer.Flags = 0;
    
    return device->CreateUnorderedAccessView(bufferResource, &_desc, &_unorderedAccessView);
}

void RwBuffer::SetToComputeShader(ID3D11DeviceContext* context, int slot)
{
    context->CSSetUnorderedAccessViews(slot, 1, &_unorderedAccessView, nullptr);
}

