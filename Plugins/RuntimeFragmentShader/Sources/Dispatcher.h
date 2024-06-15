#pragma once

#include <unordered_map>

#include "Common.h"
#include "Resources/RwBuffer.h"

class Dispatcher
{
private:
    IUnityInterfaces* _unity;
    IUnityLog* _logger = nullptr;
    ID3D11Device* _device = nullptr;
    ID3D11ComputeShader *_computeShader = nullptr;

    // Additional resources
    ID3D11Buffer* _constantBuffer;
    std::unordered_map<int, RwBuffer*> _rwBuffers = {};

    void* _constantBufferPtr;
    int _constantBufferSize;
    
public:
    Dispatcher(IUnityInterfaces* unity);
    ~Dispatcher();
    void Dispatch(int x, int y, int z);
    void SetConstantBuffer(void* buffer, int size);
    void SetRwBuffer(int slot, void* buffer, int count, int stride);
    std::string CompileComputeShaderFromString(const std::string& source);
    
};
