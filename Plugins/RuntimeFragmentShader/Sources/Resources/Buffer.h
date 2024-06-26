#pragma once
#include "../Common.h"

namespace UnityRuntimeShader
{
    class Buffer
    {
        ComPtr<ID3D11ShaderResourceView> _shaderResourceView;
        D3D11_SHADER_RESOURCE_VIEW_DESC _desc;
        int _count;
        int _stride;
    public:
        Buffer();
        ~Buffer();

        HRESULT UpdateBuffer(ID3D11Device* device, void* buffer, int count, int stride);
        ID3D11ShaderResourceView* GetShaderResourceView() const { return _shaderResourceView.Get(); }
    };
}