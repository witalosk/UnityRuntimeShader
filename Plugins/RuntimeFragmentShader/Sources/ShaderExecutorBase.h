#pragma once

#include <unordered_map>

#include "Common.h"
#include "Resources/Buffer.h"
#include "Resources/Texture2D.h"


class ShaderExecutorBase
{
protected:
    IUnityInterfaces* _unity;
    IUnityLog* _logger = nullptr;
    ID3D11Device* _device = nullptr;

    // Additional resources
    ID3D11Buffer* _constantBuffer;
    std::unordered_map<int, Buffer*> _buffers = {};
    std::unordered_map<int, Texture2D*> _textures = {};

    void* _constantBufferPtr;
    int _constantBufferSize;
    
public:
    ShaderExecutorBase(IUnityInterfaces* unity);
    ~ShaderExecutorBase();
    void SetConstantBuffer(void* buffer, int size);
    void SetBuffer(int slot, void* buffer, int count, int stride);
    void SetTexture(int slot, void* ptr, int format);
};
