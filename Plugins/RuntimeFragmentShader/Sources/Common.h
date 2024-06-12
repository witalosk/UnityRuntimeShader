#pragma once

#include <d3d11.h>
#include <thread>
#include <mutex>
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

#ifndef SAFE_RELEASE
    #define SAFE_RELEASE(a) if (a) { a->Release(); a = NULL; }
#endif