#include "Renderer.h"

#define UNITY_INTERFACE_API __stdcall
#define UNITY_INTERFACE_EXPORT __declspec(dllexport)

namespace
{
	IUnityInterfaces* g_unity    = nullptr;
	Renderer* g_renderer = nullptr;
}

extern "C"
{
	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
	{
		g_unity = unityInterfaces;
	}

	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API UnityPluginUnload()
	{
		// Nothing to do here
	}
	
	void UNITY_INTERFACE_API OnRenderEvent(int eventId)
	{
		if (g_renderer) g_renderer->Update();
	}

	UNITY_INTERFACE_EXPORT UnityRenderingEvent UNITY_INTERFACE_API GetRenderEventFunc()
	{
		return OnRenderEvent;
	}

	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API SetTexturePtr(void* ptr, void* texture, int width, int height, int format)
	{
		auto renderer = reinterpret_cast<Renderer*>(ptr);
		renderer->SetTexture(texture, width, height, format);
	}

	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API SetConstantBuffer(void* ptr, void* buffer, int size)
	{
		auto renderer = reinterpret_cast<Renderer*>(ptr);
		renderer->SetConstantBuffer(buffer, size);
	}

	UNITY_INTERFACE_EXPORT void* UNITY_INTERFACE_API CreateRenderer()
	{
		g_renderer = new Renderer(g_unity);
		return g_renderer;
	}

	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API ReleaseRenderer(void* ptr)
	{
		auto renderer = reinterpret_cast<Renderer*>(ptr);
		if (g_renderer == renderer) g_renderer = nullptr;
		delete renderer;
	}

	UNITY_INTERFACE_EXPORT const char* UNITY_INTERFACE_API CompilePixelShaderFromString(void* ptr, const char* source)
	{
		auto renderer = reinterpret_cast<Renderer*>(ptr);
		const char* result;
		result = renderer->CompilePixelShaderFromString(source).c_str();
		return result;
	}

}