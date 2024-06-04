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
	// CreateResources();
	UNITY_LOG(_logger, "[ShaderRenderer] Started");

}

void Renderer::Update()
{
	if (!_isRunning || _unity == nullptr || _texture == nullptr) return;
	ID3D11DeviceContext* context;
	_device->GetImmediateContext(&context);



	// context->OMSetDepthStencilState(_depthState, 0);
	// context->RSSetState(_rasterState);
	// context->OMSetBlendState(_blendState, nullptr, 0xFFFFFFFF);
	
	// Update constant buffer - just the world matrix in our case
	// context->UpdateSubresource(_constantBuffer, 0, nullptr, worldMatrix, 64, 0);

	// Set shaders
	// context->VSSetConstantBuffers(0, 1, &m_CB);
	// context->VSSetShader(m_VertexShader, nullptr, 0);
	// context->PSSetShader(m_PixelShader, nullptr, 0);

	// Update vertex buffer
	// const int kVertexSize = 12 + 4;
	// context->UpdateSubresource(m_VB, 0, nullptr, verticesFloat3Byte4, triangleCount * 3 * kVertexSize, 0);

	// set input assembler data and draw
	// context->IASetInputLayout(_inputLayout);
	// context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// UINT stride = kVertexSize;
	// UINT offset = 0;
	// context->IASetVertexBuffers(0, 1, &m_VB, &stride, &offset);
	// context->Draw(triangleCount * 3, 0);

	context->OMSetRenderTargets(1, &_frameBufferView, nullptr);
	FLOAT backgroundColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	context->ClearRenderTargetView(_frameBufferView, backgroundColor);
	context->Release();

	_renderCount++;
}

void Renderer::Stop()
{
	SAFE_RELEASE(_frameBufferView);
	// SAFE_RELEASE(_vertexBuffer);
	// SAFE_RELEASE(_constantBuffer);
	// SAFE_RELEASE(_vertexShader);
	// SAFE_RELEASE(_pixelShader);
	// SAFE_RELEASE(_inputLayout);
	// SAFE_RELEASE(_rasterState);
	// SAFE_RELEASE(_blendState);
	// SAFE_RELEASE(_depthState);
	_isRunning = false;
	
	UNITY_LOG(_logger, "[ShaderRenderer] Stopped");

}

void Renderer::SetTexturePtr(void* ptr, int format)
{
	_texture = static_cast<ID3D11Texture2D*>(ptr);

	if (_frameBufferView != nullptr)
	{
		_frameBufferView->Release();
		_frameBufferView = nullptr;
	}
	_device->CreateRenderTargetView(_texture, new CD3D11_RENDER_TARGET_VIEW_DESC(D3D11_RTV_DIMENSION_TEXTURE2D, static_cast<DXGI_FORMAT>(format)), &_frameBufferView);
}

void Renderer::CreateResources()
{
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));

	// vertex buffer
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = 1024;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	_device->CreateBuffer(&desc, NULL, &_vertexBuffer);

	// constant buffer
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = 64; // hold 1 matrix
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = 0;
	_device->CreateBuffer(&desc, NULL, &_constantBuffer);

	// shaders
	// HRESULT hr;
	// hr = _device->CreateVertexShader(kVertexShaderCode, sizeof(kVertexShaderCode), nullptr, &m_VertexShader);
	// if (FAILED(hr))
	// 	OutputDebugStringA("Failed to create vertex shader.\n");
	// hr = _device->CreatePixelShader(kPixelShaderCode, sizeof(kPixelShaderCode), nullptr, &m_PixelShader);
	// if (FAILED(hr))
	// 	OutputDebugStringA("Failed to create pixel shader.\n");

	// // input layout
	// if (m_VertexShader)
	// {
	// 	D3D11_INPUT_ELEMENT_DESC s_DX11InputElementDesc[] =
	// 	{
	// 		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	// 		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	// 	};
	// 	_device->CreateInputLayout(s_DX11InputElementDesc, 2, kVertexShaderCode, sizeof(kVertexShaderCode), &m_InputLayout);
	// }

	// render states
	D3D11_RASTERIZER_DESC rsdesc;
	memset(&rsdesc, 0, sizeof(rsdesc));
	rsdesc.FillMode = D3D11_FILL_SOLID;
	rsdesc.CullMode = D3D11_CULL_NONE;
	rsdesc.DepthClipEnable = TRUE;
	_device->CreateRasterizerState(&rsdesc, &_rasterState);

	D3D11_DEPTH_STENCIL_DESC dsdesc;
	memset(&dsdesc, 0, sizeof(dsdesc));
	dsdesc.DepthEnable = TRUE;
	dsdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	// dsdesc.DepthFunc = GetUsesReverseZ() ? D3D11_COMPARISON_GREATER_EQUAL : D3D11_COMPARISON_LESS_EQUAL;
	dsdesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	_device->CreateDepthStencilState(&dsdesc, &_depthState);

	D3D11_BLEND_DESC bdesc;
	memset(&bdesc, 0, sizeof(bdesc));
	bdesc.RenderTarget[0].BlendEnable = FALSE;
	bdesc.RenderTarget[0].RenderTargetWriteMask = 0xF;
	_device->CreateBlendState(&bdesc, &_blendState);
}
