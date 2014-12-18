
#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <memory.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <memory>
#include <unordered_map>
#include <map>

#include "Types.h"
#include "Util.h"
#include "WinResource.h"
#include "D3D.h"
#include "Reportable.h"
#include "TBuffer.h"
#include "CBuffer.h"
#include "Texture.h"
#include "SamplerState.h"

#include "hlsltypes.h"
#include "Shader.h"

#define Print printf
