#pragma once
#include "../Common.h"

namespace UnityRuntimeShader
{
    class ConstantBuffer
    {
        ComPtr<ID3D11Buffer> _constantBuffer;
        D3D11_SUBRESOURCE_DATA _subresourceData;
        D3D11_BUFFER_DESC _desc;
        void* _constantBufferPointer;
        int _constantBufferSize;
    public:
        ConstantBuffer();
        ~ConstantBuffer();

        HRESULT UpdateBuffer(ID3D11Device* device, void* buffer, int size);
        ID3D11Buffer* GetConstantBuffer() const { return _constantBuffer.Get(); }
        void* GetConstantBufferPointer() const { return _constantBufferPointer; }
    };
}
