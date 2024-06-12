#pragma once

#include <unordered_map>

#include "Common.h"
#include "Resources/Texture2D.h"

class Renderer
{

private:
	IUnityInterfaces* _unity;
	ID3D11Texture2D* _texture = nullptr;
	ID3D11Device* _device = nullptr;
	ID3D11RenderTargetView* _frameBufferView = nullptr;
	IUnityLog* _logger = nullptr;

	ID3D11Buffer* _vertexBuffer;
	ID3D11VertexShader* _vertexShader;
	ID3D11PixelShader* _pixelShader;
	ID3D11InputLayout* _inputLayout;
	ID3D11RasterizerState* _rasterState;
	ID3D11BlendState* _blendState;
	ID3D11DepthStencilState* _depthState;

	// Additional resources
	ID3D11Buffer* _constantBuffer;
	std::unordered_map<int, Texture2D*> _textures = {};

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
	void SetOutputTexture(void* ptr, int width, int height, int format);
	void SetConstantBuffer(void* buffer, int size);
	void CreateResources();
	std::string CompilePixelShaderFromString(const std::string& source);
	void SetTexture(int slot, void* ptr, int format);

private:
	void Start();
	void Stop();
	ID3DBlob* CompileVertexShader();
	
};
