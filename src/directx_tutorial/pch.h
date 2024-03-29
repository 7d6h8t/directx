#pragma once

#include "types.h"
#include "values.h"
#include "struct.h"

// stl
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <string>

// win
#include <Windows.h>
#include <assert.h>

// dx
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <DirectXMath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dCompiler.lib")

// using vcpkg
#include <DirectXTex.h>
#include <DirectXTex.inl>


#define CHECK(p) assert(SUCCEEDED(p))