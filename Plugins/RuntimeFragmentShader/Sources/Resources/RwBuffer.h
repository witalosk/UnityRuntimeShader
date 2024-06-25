#pragma once
#include "../Common.h"

class RwBuffer
{
    ComPtr<ID3D11UnorderedAccessView> _unorderedAccessView;
    D3D11_UNORDERED_ACCESS_VIEW_DESC _desc;
    int _count;
    int _stride;
public:
    RwBuffer();
    ~RwBuffer();

    HRESULT UpdateBuffer(ID3D11Device* device, void* buffer, int count, int stride);
    ID3D11UnorderedAccessView* GetUnorderedAccessView() const { return _unorderedAccessView.Get(); }
};