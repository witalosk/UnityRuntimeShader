#include "Renderer.h"

using namespace UnityRuntimeShader;

Renderer::Renderer(IUnityInterfaces* unity): ShaderExecutorBase(unity)
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

Renderer::~Renderer()
{
	_frameBufferView = nullptr;
	_nextFrameBufferView = nullptr;
	_vertexBuffer = nullptr;
	_vertexShader = nullptr;
	_pixelShader = nullptr;
	_inputLayout = nullptr;
	_rasterState = nullptr;
	_blendState = nullptr;
	_depthState = nullptr;
	for (auto tex : _textures)
	{
		tex.second->~Texture2D();
	}
	_isRunning = false;
}


void Renderer::Update()
{
	if (!_isRunning || _pixelShader == nullptr || _unity == nullptr || _texture == nullptr) return;

	if (_nextFrameBufferView != nullptr)
	{
		_frameBufferView = nullptr;
		_frameBufferView = _nextFrameBufferView;
		_nextFrameBufferView = nullptr;
	}
	
	ID3D11DeviceContext* context;
	_device->GetImmediateContext(&context);

	// IA: set input assembler data and draw
	context->IASetInputLayout(_inputLayout.Get());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT stride = 4 * sizeof(float);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);

	// VS: set vertex shader
	context->VSSetShader(_vertexShader.Get(), nullptr, 0);
	
	// RS: set rasterizer stage
	const D3D11_VIEWPORT* vp = new D3D11_VIEWPORT{0.0f, 0.0f, static_cast<float>(_width), static_cast<float>(_height), 0.0f, 1.0f};
	context->RSSetViewports(1, vp);
	context->RSSetState(_rasterState.Get());
	
	// PS: set pixel shader
	context->PSSetShader(_pixelShader.Get(), nullptr, 0);

	// Set Additional Resources
	for (auto cbuf : _constantBuffers)
	{
		auto buf = cbuf.second->GetConstantBuffer();
		context->UpdateSubresource(buf, 0, nullptr, cbuf.second->GetConstantBufferPointer(), 0, 0);
		context->PSSetConstantBuffers(cbuf.first, 1, &buf);
	}

	for (auto tex : _textures)
	{
		ID3D11ShaderResourceView* s = tex.second->GetShaderResourceView();
		context->PSSetShaderResources(tex.first, 1, &s);
	}
	
	// OM: set output merger stage
	context->OMSetRenderTargets(1, _frameBufferView.GetAddressOf(), nullptr);
	context->OMSetDepthStencilState(_depthState.Get(), 0);
	context->OMSetBlendState(_blendState.Get(), nullptr, 0xFFFFFFFF);
	
	FLOAT backgroundColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	context->ClearRenderTargetView(_frameBufferView.Get(), backgroundColor);
	
	context->Draw(2 * 3, 0);
	context->Release();
}

void Renderer::SetOutputTexture(void* ptr, int width, int height, int format)
{
	_texture = static_cast<ID3D11Texture2D*>(ptr);
	_width = width;
	_height = height;

	if (_nextFrameBufferView != nullptr)
	{
		_nextFrameBufferView = nullptr;
	}
	_device->CreateRenderTargetView(_texture, new CD3D11_RENDER_TARGET_VIEW_DESC(D3D11_RTV_DIMENSION_TEXTURE2D, static_cast<DXGI_FORMAT>(format)), _nextFrameBufferView.GetAddressOf());
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
	HRESULT hr = _device->CreateBuffer(&desc, &sr, _vertexBuffer.GetAddressOf());
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
	hr = _device->CreateRasterizerState(&rsdesc, _rasterState.GetAddressOf());
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
	hr = _device->CreateDepthStencilState(&dsdesc, _depthState.GetAddressOf());
	if (FAILED(hr))
	{
		UNITY_LOG_ERROR(_logger, "[ShaderRenderer] Failed to create depth stencil state");
	}

	D3D11_BLEND_DESC bdesc;
	memset(&bdesc, 0, sizeof(bdesc));
	bdesc.RenderTarget[0].BlendEnable = FALSE;
	bdesc.RenderTarget[0].RenderTargetWriteMask = 0xF;
	hr = _device->CreateBlendState(&bdesc, _blendState.GetAddressOf());
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

	hr = _device->CreatePixelShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), nullptr, _pixelShader.GetAddressOf());

	if (FAILED(hr))
	{
		UNITY_LOG_ERROR(_logger, "[ShaderRenderer] Failed to compile pixel shader");
		return "Failed to compile fragment shader";
	}
	
	return "";
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

	hr = _device->CreateVertexShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), nullptr, _vertexShader.GetAddressOf());
	
	if (FAILED(hr))
	{
		UNITY_LOG(_logger, "[ShaderRenderer] Failed to compile vertex shader");
	}
	
	return compiledShader;
}
