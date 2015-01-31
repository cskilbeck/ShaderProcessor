
#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <d3d11.h>
#pragma warning(push)
#pragma warning(disable:4838)
#include <DirectXMath.h>
#include <d3dcompiler.h>
#pragma warning(pop)

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

#define ALIGNED(x) __declspec(align(x))
#define WEAKSYM __declspec(selectany)

#include "DX.h"

#include "printer.h"
#include "WinResource.h"
#include "TypeDefinition.h"
#include "Binding.h"
#include "TBuffer.h"
#include "Texture.h"
#include "SamplerState.h"
#include "hlsltypes.h"
#include "HLSLShader.h"
#include "CBuffer.h"
#include "output.h"
#include "optionparser.h"
#include "Args.h"

// DX

