//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////
// Windows

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif

struct IUnknown;

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

//////////////////////////////////////////////////////////////////////
// D3D

#include <mmintrin.h>
#include <d3d11.h>
#include <dxgi.h>
#pragma warning(push)
#pragma warning(disable : 4838)
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <Xinput.h>
#include <dwrite_2.h>
#include <d2d1_2.h>
#pragma warning(pop)

//////////////////////////////////////////////////////////////////////
// c libs

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

//////////////////////////////////////////////////////////////////////
// c++ std libs

#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <memory>
#include <unordered_map>
#include <map>
#include <exception>
#include <cstdarg>
#include <fstream>
#include <iostream>

//////////////////////////////////////////////////////////////////////
// DX

#include <immintrin.h>

#include "Aligned.h"
#include "Types.h"
#include "Log.h"
#include "Macros.h"
#include "Flags.h"
#include "Win32.h"
#include "linked_list.h"

using chs::linked_list;
using chs::list_node;

#include "Pool.h"
#include "LinearAlloc.h"
#include "RefCounted.h"
#include "Half.h"
#include "Vec4.h"
#include "HLSLTypes.h"
#include "Vec2.h"
#include "Vec4.inl"
#include "Matrix.h"
#include "Random.h"
#include "Color.h"
#include "UTF8.h"
#include "Util.h"
#include "Resource.h"
#include "Options.h"
#include "File.h"
#include "Point.h"
#include "Size.h"
#include "Rect.h"
#include "D3D.h"
#include "D2D.h"
#include "Registry.h"
