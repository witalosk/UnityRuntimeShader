#include "Dispatcher.h"

using namespace UnityRuntimeShader;

Dispatcher::Dispatcher(IUnityInterfaces* unity): ShaderExecutorBase(unity)
{
    
}

Dispatcher::~Dispatcher()
{
    _computeShader = nullptr;

    for (auto buf : _rwBuffers)
    {
        buf.second->~RwBuffer();
    }
}

void Dispatcher::Dispatch(int x, int y, int z)
{
    if (_computeShader == nullptr || _unity == nullptr) return;

    ID3D11DeviceContext* context;
    _device->GetImmediateContext(&context);

    // CS: Set the shader
    context->CSSetShader(_computeShader.Get(), nullptr, 0);

    // Set Additional Resources
    for (auto cbuf : _constantBuffers)
    {
        auto buf = cbuf.second->GetConstantBuffer();
        context->UpdateSubresource(buf, 0, nullptr, cbuf.second->GetConstantBufferPointer(), 0, 0);
        context->CSSetConstantBuffers(cbuf.first, 1, &buf);
    }

    for (auto buf : _buffers)
    {
        auto srv = buf.second->GetShaderResourceView();
        context->CSSetShaderResources(buf.first, 1, &srv);
    }

    for (auto buf : _rwBuffers)
    {
        auto uav = buf.second->GetUnorderedAccessView();
        context->CSSetUnorderedAccessViews(buf.first, 1, &uav, nullptr);
    }

    for (auto tex : _textures)
    {
        auto srv = tex.second->GetShaderResourceView();
        context->CSSetShaderResources(tex.first, 1, &srv);
    }
    
    context->Dispatch(x, y, z);

    // Unbind resources after dispatch to avoid unexpected behaviors
    ID3D11Buffer* nullBuf = nullptr;
    ID3D11ShaderResourceView* nullSRV = nullptr;
    ID3D11UnorderedAccessView* nullUAV = nullptr;

    for (auto& cbuf : _constantBuffers)
    {
        context->CSSetConstantBuffers(cbuf.first, 1, &nullBuf);
    }

    for (auto& tex : _textures)
    {
        context->CSSetShaderResources(tex.first, 1, &nullSRV);
    }

    for (auto& buf : _buffers)
    {
        context->CSSetShaderResources(buf.first, 1, &nullSRV);
    }
    
    for (auto& buf : _rwBuffers)
    {
        context->CSSetUnorderedAccessViews(buf.first, 1, &nullUAV, nullptr);
    }
    
    context->CSSetShader(nullptr, nullptr, 0);
    
    context->Release();
}

void Dispatcher::SetRwBuffer(int slot, void* buffer, int count, int stride)
{
    if (_rwBuffers.count(slot) == 0)
    {
        _rwBuffers[slot] = new RwBuffer();
    }

    HRESULT hr = _rwBuffers[slot]->UpdateBuffer(_device, buffer, count, stride);
    if (FAILED(hr))
    {
        UNITY_LOG_ERROR(_logger, ("[KernelDispatcher] Failed to update rw buffer: " + std::to_string(hr)).c_str());
        return;
    }
}

std::string Dispatcher::CompileComputeShaderFromString(const std::string& source)
{
    ID3DBlob* compiledShader;
    ID3DBlob* errorMessage;
    HRESULT hr = D3DCompile(source.c_str(), source.size(), nullptr, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "Main", "cs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_WARNINGS_ARE_ERRORS, 0, &compiledShader, &errorMessage);
    if (FAILED(hr))
    {
        UNITY_LOG_ERROR(_logger, "[KernelDispatcher] Failed to compile compute shader");

        if (errorMessage)
        {
            LPVOID errMsg = errorMessage->GetBufferPointer();
            size_t errMsgSize = errorMessage->GetBufferSize();

            std::string strErrMsg(static_cast<const char*>(errMsg), errMsgSize);
            errorMessage->Release();
			
            return strErrMsg;
        }

        return "Unknown compile error";
    }

    hr = _device->CreateComputeShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), nullptr, _computeShader.GetAddressOf());

    if (FAILED(hr))
    {
        UNITY_LOG_ERROR(_logger, "[ShaderRenderer] Failed to compile compute shader");
        UNITY_LOG(_logger, std::to_string(hr).c_str());
        return "Failed to compile compute shader";
    }
	
    return "";
}
