#pragma once

#include <unordered_map>

#include "Common.h"
#include "ShaderExecutorBase.h"
#include "Resources/RwBuffer.h"

class Dispatcher : public ShaderExecutorBase
{
    ID3D11ComputeShader *_computeShader = nullptr;
    std::unordered_map<int, RwBuffer*> _rwBuffers = {};

public:
    Dispatcher(IUnityInterfaces* unity);
    ~Dispatcher();
    void Dispatch(int x, int y, int z);
    void SetRwBuffer(int slot, void* buffer, int count, int stride);
    std::string CompileComputeShaderFromString(const std::string& source);
};
