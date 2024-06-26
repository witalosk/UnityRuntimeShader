#pragma once

#include "Common.h"
#include "ShaderExecutorBase.h"

namespace UnityRuntimeShader
{
	class Renderer : public ShaderExecutorBase
	{
		ID3D11Texture2D* _texture = nullptr;
		ComPtr<ID3D11RenderTargetView> _frameBufferView = nullptr;
		ComPtr<ID3D11RenderTargetView> _nextFrameBufferView = nullptr;

		ComPtr<ID3D11Buffer> _vertexBuffer;
		ComPtr<ID3D11VertexShader> _vertexShader;
		ComPtr<ID3D11PixelShader> _pixelShader;
		ComPtr<ID3D11InputLayout> _inputLayout;
		ComPtr<ID3D11RasterizerState> _rasterState;
		ComPtr<ID3D11BlendState> _blendState;
		ComPtr<ID3D11DepthStencilState> _depthState;
	
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
}