#pragma once
#include "../Common.h"

class RwBuffer
{
    ID3D11UnorderedAccessView* _unorderedAccessView;
    D3D11_UNORDERED_ACCESS_VIEW_DESC _desc;
public:
    RwBuffer();
    ~RwBuffer();

    HRESULT UpdateBuffer(ID3D11Device* device, void* buffer, int count, int stride);
    void SetToComputeShader(ID3D11DeviceContext* context, int slot);
};
