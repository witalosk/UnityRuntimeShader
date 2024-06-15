#include "Dispatcher.h"

Dispatcher::Dispatcher(IUnityInterfaces* unity)
{
    _unity = unity;
    _device = _unity->Get<IUnityGraphicsD3D11>()->GetDevice();
    _logger = _unity->Get<IUnityLog>();

    _constantBuffer = nullptr;
    _constantBufferPtr = nullptr;
    _constantBufferSize = 0;
}

Dispatcher::~Dispatcher()
{
    SAFE_RELEASE(_computeShader);
    SAFE_RELEASE(_constantBuffer);

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

    // Set Additional Resources
    if (_constantBufferSize > 0 && _constantBuffer != nullptr)
    {
        context->UpdateSubresource(_constantBuffer, 0, nullptr, _constantBufferPtr, 0, 0);
        context->PSSetConstantBuffers(0, 1, &_constantBuffer);
    }

    for (auto buf : _rwBuffers)
    {
        buf.second->SetToComputeShader(context, buf.first);
    }
    
    context->CSSetShader(_computeShader, nullptr, 0);
    context->Dispatch(x, y, z);
}

void Dispatcher::SetConstantBuffer(void* buffer, int size)
{
    if (size == 0) return;
    if (size == _constantBufferSize) return;
    _constantBufferPtr = buffer;
	
    if (_constantBuffer != nullptr)
    {
        _constantBuffer->Release();
        _constantBuffer = nullptr;
    }
	
    D3D11_SUBRESOURCE_DATA sr = { 0 };
    sr.pSysMem = buffer;
	
    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = size + (size % 16 == 0 ? 0 : 16 - size % 16);
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;
    HRESULT hr = _device->CreateBuffer(&desc, &sr, &_constantBuffer);
    if (FAILED(hr))
    {
        UNITY_LOG_ERROR(_logger, "[ShaderRenderer] Failed to create constant buffer");
    }

    _constantBufferSize = size;
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
    HRESULT hr = D3DCompile(source.c_str(), source.size(), nullptr, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "Main", "cs_5_0", 0, 0, &compiledShader, &errorMessage);
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

    hr = _device->CreateComputeShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), nullptr, &_computeShader);

    if (FAILED(hr))
    {
        UNITY_LOG_ERROR(_logger, "[ShaderRenderer] Failed to compile compute shader");
        UNITY_LOG(_logger, std::to_string(hr).c_str());
        return "Failed to compile compute shader";
    }
	
    return "";
}
