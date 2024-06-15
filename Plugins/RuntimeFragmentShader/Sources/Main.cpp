#include <map>
#include <unordered_map>

#include "Dispatcher.h"
#include "Renderer.h"

#define UNITY_INTERFACE_API __stdcall
#define UNITY_INTERFACE_EXPORT __declspec(dllexport)

namespace
{
	IUnityInterfaces* g_unity = nullptr;
	std::unordered_map<int, Renderer*> g_renderers = {};
	int g_rendererCount = 0;
	std::unordered_map<int, Dispatcher*> g_dispatchers = {};
	int g_dispatcherCount = 0;
}

extern "C"
{

#pragma region UnityPlugin
	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
	{
		g_unity = unityInterfaces;
	}

	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API UnityPluginUnload()
	{
		// Nothing to do here
	}
#pragma endregion

#pragma region Renderer
	void UNITY_INTERFACE_API OnRenderEvent(int eventId)
	{
		g_renderers[eventId]->Update();
	}

	UNITY_INTERFACE_EXPORT UnityRenderingEvent UNITY_INTERFACE_API GetRenderEventFunc()
	{
		return OnRenderEvent;
	}

	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API Render(int id)
	{
		auto renderer = g_renderers[id];
		renderer->Update();
	}

	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API SetOutputTexture(int id, void* texture, int width, int height, int format)
	{
		auto renderer = g_renderers[id];
		renderer->SetOutputTexture(texture, width, height, format);
	}

	UNITY_INTERFACE_EXPORT int UNITY_INTERFACE_API CreateRenderer()
	{
		g_rendererCount++;
		auto renderer = new Renderer(g_unity);
		g_renderers[g_rendererCount] = renderer;
		return g_rendererCount;
	}

	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API ReleaseRenderer(int id)
	{
		auto renderer = g_renderers[id];
		delete renderer;
		g_renderers.erase(id);
	}

	UNITY_INTERFACE_EXPORT const char* UNITY_INTERFACE_API CompilePixelShaderFromString(int id, const char* source)
	{
		auto renderer = g_renderers[id];
		const char* result;
		result = renderer->CompilePixelShaderFromString(source).c_str();
		return result;
	}

	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API SetConstantBuffer(int id, void* buffer, int size)
	{
		auto renderer = g_renderers[id];
		renderer->SetConstantBuffer(buffer, size);
	}

	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API SetTexture(int id, int slot, void* texture, int format)
	{
		auto renderer = g_renderers[id];
		renderer->SetTexture(slot, texture, format);
	}
	
#pragma endregion

#pragma region Dispatcher

	UNITY_INTERFACE_EXPORT int UNITY_INTERFACE_API CreateDispatcher()
	{
		g_dispatcherCount++;
		auto dispatcher = new Dispatcher(g_unity);
		g_dispatchers[g_dispatcherCount] = dispatcher;
		return g_dispatcherCount;
	}

	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API ReleaseDispatcher(int id)
	{
		auto dispacher = g_dispatchers[id];
		delete dispacher;
		g_renderers.erase(id);
	}

	UNITY_INTERFACE_EXPORT const char* UNITY_INTERFACE_API CompileComputeShaderFromString(int id, const char* source)
	{
		auto dispatcher = g_dispatchers[id];
		const char* result;
		result = dispatcher->CompileComputeShaderFromString(source).c_str();
		return result;
	}

	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API Dispatch(int id, int x, int y, int z)
	{
		auto dispatcher = g_dispatchers[id];
		dispatcher->Dispatch(x, y, z);
	}

	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API SetConstantBufferToCs(int id, void* buffer, int size)
	{
		auto dispatcher = g_dispatchers[id];
		dispatcher->SetConstantBuffer(buffer, size);
	}

	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API SetRwBufferToCs(int id, int slot, void* buffer, int count, int stride)
	{
		auto dispatcher = g_dispatchers[id];
		dispatcher->SetRwBuffer(slot, buffer, count, stride);
	}
#pragma endregion
	
}