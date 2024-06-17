#include "Texture2D.h"

Texture2D::Texture2D()
{
    _desc = {};
    _shaderResourceView = nullptr;
}

Texture2D::~Texture2D()
{
    SAFE_RELEASE(_shaderResourceView);
}

HRESULT Texture2D::UpdateTexture(ID3D11Device* device, void* tex, int format)
{
    if (_shaderResourceView != nullptr)
    {
        _shaderResourceView->Release();
        _shaderResourceView = nullptr;
    }
    
    ID3D11Texture2D* texture = static_cast<ID3D11Texture2D*>(tex);
    _desc = {};
    _desc.Format = static_cast<DXGI_FORMAT>(format);
    _desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    _desc.Texture2D.MipLevels = 1;
    
    return device->CreateShaderResourceView(texture, &_desc, &_shaderResourceView);
}

void Texture2D::SetToFragmentShader(ID3D11DeviceContext* context, int slot)
{
    context->PSSetShaderResources(slot, 1, &_shaderResourceView);
}

void Texture2D::SetToComputeShader(ID3D11DeviceContext* context, int slot)
{
    context->CSSetShaderResources(slot, 1, &_shaderResourceView);
}
