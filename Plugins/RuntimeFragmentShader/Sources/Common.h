#pragma once

#include <d3d11.h>
#include <d3d11_3.h>
#include <thread>
#include <d3dcompiler.h>
#include <string>
#include <vector>
#include <wrl.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "Unity/IUnityInterface.h"
#include "Unity/IUnityGraphics.h"
#include "Unity/IUnityGraphicsD3D11.h"
#include "Unity/IUnityLog.h"

using Microsoft::WRL::ComPtr;