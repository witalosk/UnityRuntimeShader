#pragma once

#include <unordered_map>
#include <map>

#include "Common.h"
#include "Resources/Buffer.h"
#include "Resources/ConstantBuffer.h"
#include "Resources/Texture2D.h"

namespace UnityRuntimeShader
{
    class ShaderExecutorBase
    {
    protected:
        IUnityInterfaces* _unity;
        IUnityLog* _logger = nullptr;
        ID3D11Device* _device = nullptr;
        ComPtr<ID3D11On12Device> _d3d11On12;
        bool _isD3D12 = false;
        bool _ownsDevice = false;
        std::map<std::pair<void*, UINT>, ComPtr<ID3D11Resource>> _wrappedResources;

        // Additional resources
        std::unordered_map<int, ConstantBuffer*> _constantBuffers = {};
        std::unordered_map<int, Buffer*> _buffers = {};
        std::unordered_map<int, Texture2D*> _textures = {};
    
    public:
        ShaderExecutorBase(IUnityInterfaces* unity);
        virtual ~ShaderExecutorBase();
        void SetConstantBuffer(int slot, void* buffer, int size);
        void SetBuffer(int slot, void* buffer, int count, int stride);
        void SetTexture(int slot, void* ptr, int format);

    protected:
        ID3D11Resource* WrapResource(void* resource, UINT bindFlags,
            D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState);
        void AcquireWrappedResources();
        void ReleaseWrappedResources();
    };
}
