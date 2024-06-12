#pragma once
#include "../Common.h"

class Texture2D
{
    ID3D11ShaderResourceView* _shaderResourceView;
    D3D11_SHADER_RESOURCE_VIEW_DESC _desc;
public:
    Texture2D(ID3D11Device* device, void* tex, int format);
    Texture2D();
    ~Texture2D();

    HRESULT UpdateTexture(ID3D11Device* device, void* tex, int format);
    void SetToFragmentShader(ID3D11DeviceContext* context, int slot);
};
