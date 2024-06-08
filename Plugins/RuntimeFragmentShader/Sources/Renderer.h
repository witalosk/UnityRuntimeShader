#pragma once

#include <d3d11.h>
#include <thread>
#include <mutex>
#include <d3dcompiler.h>
#include <string>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "Unity/IUnityInterface.h"
#include "Unity/IUnityGraphics.h"
#include "Unity/IUnityGraphicsD3D11.h"
#include "Unity/IUnityLog.h"

#ifndef SAFE_RELEASE
	#define SAFE_RELEASE(a) if (a) { a->Release(); a = NULL; }
#endif

class Renderer
{

private:
	IUnityInterfaces* _unity;
	ID3D11Texture2D* _texture = nullptr;
	ID3D11Device* _device = nullptr;
	ID3D11RenderTargetView* _frameBufferView = nullptr;
	IUnityLog* _logger = nullptr;

	ID3D11Buffer* _vertexBuffer; // vertex buffer
	ID3D11Buffer* _constantBuffer; // constant buffer
	ID3D11VertexShader* _vertexShader;
	ID3D11PixelShader* _pixelShader;
	ID3D11InputLayout* _inputLayout;
	ID3D11RasterizerState* _rasterState;
	ID3D11BlendState* _blendState;
	ID3D11DepthStencilState* _depthState;

	std::thread _thread;
	std::mutex _mutex;
	bool _isRunning = false;
	int _renderCount = 0;
	int _width;
	int _height;
	void* _constantBufferPtr;
	int _constantBufferSize;
	

public:
	Renderer(IUnityInterfaces* unity);
	~Renderer();
	void Update();
	void SetTexture(void* ptr, int width, int height, int format);
	void SetConstantBuffer(void* buffer, int size);
	void CreateResources();
	std::string CompilePixelShaderFromString(const std::string& source);

private:
	void Start();
	void Stop();
	ID3DBlob* CompileVertexShader();
	
};