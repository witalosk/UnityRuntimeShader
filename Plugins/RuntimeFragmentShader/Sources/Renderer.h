#pragma once

#include <unordered_map>

#include "Common.h"
#include "ShaderExecutorBase.h"
#include "Resources/Buffer.h"
#include "Resources/Texture2D.h"

class Renderer : public ShaderExecutorBase
{
	ID3D11Texture2D* _texture = nullptr;
	ID3D11RenderTargetView* _frameBufferView = nullptr;

	ID3D11Buffer* _vertexBuffer;
	ID3D11VertexShader* _vertexShader;
	ID3D11PixelShader* _pixelShader;
	ID3D11InputLayout* _inputLayout;
	ID3D11RasterizerState* _rasterState;
	ID3D11BlendState* _blendState;
	ID3D11DepthStencilState* _depthState;
	
	bool _isRunning = false;
	int _width;
	int _height;

public:
	Renderer(IUnityInterfaces* unity);
	~Renderer();
	void Update();
	void SetOutputTexture(void* ptr, int width, int height, int format);
	void CreateResources();
	std::string CompilePixelShaderFromString(const std::string& source);

private:
	ID3DBlob* CompileVertexShader();
	
};
