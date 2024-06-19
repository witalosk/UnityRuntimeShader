#pragma once
#include "../Common.h"

class Texture2D
{
    ComPtr<ID3D11ShaderResourceView> _shaderResourceView;
    D3D11_SHADER_RESOURCE_VIEW_DESC _desc;
public:
    Texture2D();
    ~Texture2D();

    HRESULT UpdateTexture(ID3D11Device* device, void* tex, int format);
    ID3D11ShaderResourceView* GetShaderResourceView() const { return _shaderResourceView.Get(); }
};
