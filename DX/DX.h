
#pragma once

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

#include "Types.h"
#include "linked_list.h"
#include "Half.h"
#include "Color.h"
#include "Util.h"
#include "Vec2.h"
#include "Point.h"
#include "Size.h"
#include "Rect.h"
#include "Random.h"
#include "Timer.h"
#include "D3D.h"

#include "Texture.h"
#include "Sampler.h"
#include "Shader.h"
#include "ConstBuffer.h"
#include "VertexBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
