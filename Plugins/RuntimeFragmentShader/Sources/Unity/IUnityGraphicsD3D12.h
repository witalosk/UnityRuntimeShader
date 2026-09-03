#pragma once

#include "IUnityInterface.h"

// Minimal Unity D3D12 plugin interface used by the D3D11On12 backend.
UNITY_DECLARE_INTERFACE(IUnityGraphicsD3D12)
{
    ID3D12Device* (UNITY_INTERFACE_API * GetDevice)();
    ID3D12CommandQueue* (UNITY_INTERFACE_API * GetCommandQueue)();
    ID3D12Fence* (UNITY_INTERFACE_API * GetFrameFence)();
    UINT64(UNITY_INTERFACE_API * GetNextFrameFenceValue)();
    bool(UNITY_INTERFACE_API * GetResourceState)(ID3D12Resource* resource, D3D12_RESOURCE_STATES* outState);
    void(UNITY_INTERFACE_API * SetResourceState)(ID3D12Resource* resource, D3D12_RESOURCE_STATES state);
};
UNITY_REGISTER_INTERFACE_GUID(0xEF4CEC88A45F4C4CULL, 0xBD295B6F2A38D9DEULL, IUnityGraphicsD3D12)
