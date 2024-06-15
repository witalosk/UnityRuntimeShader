#include "Renderer.h"

Renderer::Renderer(IUnityInterfaces* unity)
{
	_unity = unity;
	Start();
}

Renderer::~Renderer()
{
	Stop();
}

void Renderer::Start()
{
	_isRunning = true;
	_device = _unity->Get<IUnityGraphicsD3D11>()->GetDevice();
	_logger = _unity->Get<IUnityLog>();

	CreateResources();

	// Compile Vertex Shader
	auto compiledVs = CompileVertexShader();
	if (compiledVs == nullptr)
	{
		UNITY_LOG(_logger, "[ShaderRenderer] Failed to compile vertex shader");
		return;
	}

	// input layout
	D3D11_INPUT_ELEMENT_DESC _dx11InputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	HRESULT hr = _device->CreateInputLayout(_dx11InputElementDesc, 1, compiledVs->GetBufferPointer(), compiledVs->GetBufferSize(), &_inputLayout);
	if (FAILED(hr))
	{
		UNITY_LOG_ERROR(_logger, "[ShaderRenderer] Failed to create input layout");
	}
}

void Renderer::Update()
{
	if (!_isRunning || _pixelShader == nullptr || _unity == nullptr || _texture == nullptr) return;
	
	ID3D11DeviceContext* context;
	_device->GetImmediateContext(&context);

	// IA: set input assembler data and draw
	context->IASetInputLayout(_inputLayout);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT stride = 4 * sizeof(float);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);

	// VS: set vertex shader
	context->VSSetShader(_vertexShader, nullptr, 0);
	
	// RS: set rasterizer stage
	const D3D11_VIEWPORT* vp = new D3D11_VIEWPORT{0.0f, 0.0f, static_cast<float>(_width), static_cast<float>(_height), 0.0f, 1.0f};
	context->RSSetViewports(1, vp);
	context->RSSetState(_rasterState);
	
	// PS: set pixel shader
	context->PSSetShader(_pixelShader, nullptr, 0);

	// Set Additional Resources
	if (_constantBufferSize > 0 && _constantBuffer != nullptr)
	{
		context->UpdateSubresource(_constantBuffer, 0, nullptr, _constantBufferPtr, 0, 0);
		context->PSSetConstantBuffers(0, 1, &_constantBuffer);
	}

	for (auto tex : _textures)
	{
		tex.second->SetToFragmentShader(context, tex.first);
	}

	// OM: set output merger stage
	context->OMSetRenderTargets(1, &_frameBufferView, nullptr);
	context->OMSetDepthStencilState(_depthState, 0);
	context->OMSetBlendState(_blendState, nullptr, 0xFFFFFFFF);
	
	FLOAT backgroundColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	context->ClearRenderTargetView(_frameBufferView, backgroundColor);
	
	context->Draw(2 * 3, 0);
	context->Release();

	_renderCount++;
}

void Renderer::Stop()
{
	SAFE_RELEASE(_frameBufferView);
	SAFE_RELEASE(_vertexBuffer);
	SAFE_RELEASE(_constantBuffer);
	SAFE_RELEASE(_vertexShader);
	SAFE_RELEASE(_pixelShader);
	SAFE_RELEASE(_inputLayout);
	SAFE_RELEASE(_rasterState);
	SAFE_RELEASE(_blendState);
	SAFE_RELEASE(_depthState);
	
	for (auto tex : _textures)
	{
		tex.second->~Texture2D();
	}
	
	_isRunning = false;
}

void Renderer::SetOutputTexture(void* ptr, int width, int height, int format)
{
	_texture = static_cast<ID3D11Texture2D*>(ptr);
	_width = width;
	_height = height;

	if (_frameBufferView != nullptr)
	{
		_frameBufferView->Release();
		_frameBufferView = nullptr;
	}
	_device->CreateRenderTargetView(_texture, new CD3D11_RENDER_TARGET_VIEW_DESC(D3D11_RTV_DIMENSION_TEXTURE2D, static_cast<DXGI_FORMAT>(format)), &_frameBufferView);
}

void Renderer::SetConstantBuffer(void* buffer, int size)
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

void Renderer::CreateResources()
{
	// x, y, u, v
	float vertexData[] = {
		-1.,  1., 0.f, 0.f,
		1., -1., 1.f, 1.f,
		-1., -1., 0.f, 1.f,
		-1.,  1., 0.f, 0.f,
		1.,  1., 1.f, 0.f,
		1., -1., 1.f, 1.f
	};

	_vertexBuffer = nullptr;
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = sizeof(vertexData);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	
	D3D11_SUBRESOURCE_DATA sr = { 0 };
	sr.pSysMem = vertexData;
	HRESULT hr = _device->CreateBuffer(&desc, &sr, &_vertexBuffer);
	if (FAILED(hr))
	{
		UNITY_LOG_ERROR(_logger, "[ShaderRenderer] Failed to create vertex buffer");
	}

	// render states
	D3D11_RASTERIZER_DESC rsdesc;
	memset(&rsdesc, 0, sizeof(rsdesc));
	rsdesc.FillMode = D3D11_FILL_SOLID;
	rsdesc.CullMode = D3D11_CULL_NONE;
	rsdesc.DepthClipEnable = TRUE;
	hr = _device->CreateRasterizerState(&rsdesc, &_rasterState);
	if (FAILED(hr))
	{
		UNITY_LOG_ERROR(_logger, "[ShaderRenderer] Failed to create rasterizer state");
	}

	D3D11_DEPTH_STENCIL_DESC dsdesc;
	memset(&dsdesc, 0, sizeof(dsdesc));
	dsdesc.DepthEnable = TRUE;
	dsdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	// dsdesc.DepthFunc = GetUsesReverseZ() ? D3D11_COMPARISON_GREATER_EQUAL : D3D11_COMPARISON_LESS_EQUAL;
	dsdesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = _device->CreateDepthStencilState(&dsdesc, &_depthState);
	if (FAILED(hr))
	{
		UNITY_LOG_ERROR(_logger, "[ShaderRenderer] Failed to create depth stencil state");
	}

	D3D11_BLEND_DESC bdesc;
	memset(&bdesc, 0, sizeof(bdesc));
	bdesc.RenderTarget[0].BlendEnable = FALSE;
	bdesc.RenderTarget[0].RenderTargetWriteMask = 0xF;
	hr = _device->CreateBlendState(&bdesc, &_blendState);
	if (FAILED(hr))
	{
		UNITY_LOG_ERROR(_logger, "[ShaderRenderer] Failed to create blend state");
	}
}

std::string Renderer::CompilePixelShaderFromString(const std::string& source)
{
	ID3DBlob* compiledShader;
	ID3DBlob* errorMessage;
	HRESULT hr = D3DCompile(source.c_str(), source.size(), nullptr, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "Frag", "ps_4_0", 0, 0, &compiledShader, &errorMessage);
	if (FAILED(hr))
	{
		UNITY_LOG_ERROR(_logger, "[ShaderRenderer] Failed to compile pixel shader");

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

	hr = _device->CreatePixelShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), nullptr, &_pixelShader);

	if (FAILED(hr))
	{
		UNITY_LOG_ERROR(_logger, "[ShaderRenderer] Failed to compile pixel shader");
		return "Failed to compile fragment shader";
	}
	
	return "";
}

void Renderer::SetTexture(int slot, void* ptr, int format)
{
	if (_textures.count(slot) == 0)
	{
		_textures[slot] = new Texture2D();
	}

	HRESULT hr = _textures[slot]->UpdateTexture(_device, ptr, format);
	if (FAILED(hr))
	{
		UNITY_LOG_ERROR(_logger, ("[ShaderRenderer] Failed to update texture: " + std::to_string(hr)).c_str());
		return;
	}
}

ID3DBlob* Renderer::CompileVertexShader()
{
	std::string vs =
		"struct VsOutput { float4 pos : SV_POSITION; float2 uv : TEXCOORD0; };"
		"VsOutput Vert( float4 pos : POSITION ) { VsOutput vsOut = (VsOutput)0; vsOut.pos = float4(pos.xy, 0, 1); vsOut.uv = pos.zw; return vsOut; }";

	ID3DBlob* compiledShader;
	HRESULT hr = D3DCompile(vs.c_str(), vs.size(), nullptr, nullptr, nullptr, "Vert", "vs_4_0", 0, 0, &compiledShader, nullptr);
	if (FAILED(hr))
	{
		UNITY_LOG_ERROR(_logger, "[ShaderRenderer] Failed to compile vertex shader");
		return nullptr;
	}

	hr = _device->CreateVertexShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), nullptr, &_vertexShader);
	
	if (FAILED(hr))
	{
		UNITY_LOG(_logger, "[ShaderRenderer] Failed to compile vertex shader");
	}
	
	return compiledShader;
}
