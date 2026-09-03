#include <map>
#include <unordered_map>

#include "Dispatcher.h"
#include "Renderer.h"

#define UNITY_INTERFACE_API __stdcall
#define UNITY_INTERFACE_EXPORT __declspec(dllexport)

using namespace UnityRuntimeShader;

namespace
{
	IUnityInterfaces* g_unity = nullptr;
	std::unordered_map<int, Renderer*> g_renderers = {};
	int g_rendererCount = 0;
	std::unordered_map<int, Dispatcher*> g_dispatchers = {};
	int g_dispatcherCount = 0;

	Renderer* FindRenderer(int id)
	{
		auto it = g_renderers.find(id);
		return it == g_renderers.end() ? nullptr : it->second;
	}

	Dispatcher* FindDispatcher(int id)
	{
		auto it = g_dispatchers.find(id);
		return it == g_dispatchers.end() ? nullptr : it->second;
	}
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
		auto renderer = FindRenderer(eventId);
		if (renderer != nullptr) renderer->Update();
	}

	UNITY_INTERFACE_EXPORT UnityRenderingEvent UNITY_INTERFACE_API GetRenderEventFunc()
	{
		return OnRenderEvent;
	}

	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API Render(int id)
	{
		auto renderer = FindRenderer(id);
		if (renderer != nullptr) renderer->Update();
	}

	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API SetOutputTexture(int id, void* texture, int width, int height, int format)
	{
		auto renderer = FindRenderer(id);
		if (renderer != nullptr) renderer->SetOutputTexture(texture, width, height, format);
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
		auto renderer = FindRenderer(id);
		if (renderer == nullptr) return;
		delete renderer;
		g_renderers.erase(id);
	}

	UNITY_INTERFACE_EXPORT const char* UNITY_INTERFACE_API CompilePixelShaderFromString(int id, const char* source)
	{
		static thread_local std::string result;
		auto renderer = FindRenderer(id);
		if (renderer == nullptr)
		{
			result.clear();
			return result.c_str();
		}
		// Keep the returned buffer alive after the temporary std::string is destroyed.
		result = renderer->CompilePixelShaderFromString(source);
		return result.c_str();
	}

	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API SetConstantBuffer(int id, int slot, void* buffer, int size)
	{
		auto renderer = FindRenderer(id);
		if (renderer != nullptr) renderer->SetConstantBuffer(slot, buffer, size);
	}

	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API SetTexture(int id, int slot, void* texture, int format)
	{
		auto renderer = FindRenderer(id);
		if (renderer != nullptr) renderer->SetTexture(slot, texture, format);
	}

	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API SetBuffer(int id, int slot, void* buffer, int count, int stride)
	{
		auto renderer = FindRenderer(id);
		if (renderer != nullptr) renderer->SetBuffer(slot, buffer, count, stride);
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
		auto dispatcher = FindDispatcher(id);
		if (dispatcher == nullptr) return;
		delete dispatcher;
		g_dispatchers.erase(id);
	}

	UNITY_INTERFACE_EXPORT const char* UNITY_INTERFACE_API CompileComputeShaderFromString(int id, const char* source)
	{
		static thread_local std::string result;
		auto dispatcher = FindDispatcher(id);
		if (dispatcher == nullptr)
		{
			result.clear();
			return result.c_str();
		}
		result = dispatcher->CompileComputeShaderFromString(source);
		return result.c_str();
	}

	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API Dispatch(int id, int x, int y, int z)
	{
		auto dispatcher = FindDispatcher(id);
		if (dispatcher != nullptr) dispatcher->Dispatch(x, y, z);
	}

	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API SetConstantBufferToCs(int id, int slot, void* buffer, int size)
	{
		auto dispatcher = FindDispatcher(id);
		if (dispatcher != nullptr) dispatcher->SetConstantBuffer(slot, buffer, size);
	}

	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API SetBufferToCs(int id, int slot, void* buffer, int count, int stride)
	{
		auto dispatcher = FindDispatcher(id);
		if (dispatcher != nullptr) dispatcher->SetBuffer(slot, buffer, count, stride);
	}

	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API SetRwBufferToCs(int id, int slot, void* buffer, int count, int stride)
	{
		auto dispatcher = FindDispatcher(id);
		if (dispatcher != nullptr) dispatcher->SetRwBuffer(slot, buffer, count, stride);
	} 

	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API SetTextureToCs(int id, int slot, void* texture, int format)
	{
		auto dispatcher = FindDispatcher(id);
		if (dispatcher != nullptr) dispatcher->SetTexture(slot, texture, format);
	}
#pragma endregion
	
}
