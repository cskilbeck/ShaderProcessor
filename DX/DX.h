//////////////////////////////////////////////////////////////////////

#pragma once

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif

//////////////////////////////////////////////////////////////////////
// Assimp

#include "assimp/include/assimp/Importer.hpp"
#include "assimp/include/assimp/postprocess.h"
#include "assimp/include/assimp/scene.h"
#include "assimp/include/assimp/mesh.h"
#include "assimp/include/assimp/DefaultLogger.hpp"
#include "assimp/code/Win32DebugLogStream.h"
#include "assimp/include/assimp/cimport.h"

//////////////////////////////////////////////////////////////////////
// DXBase

#include "DXBase\DXBase.h"

//////////////////////////////////////////////////////////////////////
// Bullet

#include "Bullet\src\btBulletCollisionCommon.h"
#include "Bullet\src\btBulletDynamicsCommon.h"
#include "Bullet\src\linearmath\btIDebugDraw.h"

//////////////////////////////////////////////////////////////////////
// zlib

#include "..\zlib-1.2.8\zlib.h"
#include "..\zlib-1.2.8\infback9.h"

//////////////////////////////////////////////////////////////////////
// For IStream (for ZipStream) (for loading textures from Archives)

#include <Objidl.h>

//////////////////////////////////////////////////////////////////////
// DX

#include "DDSTextureLoader.h"

#include "Device.h"
#include "Event.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Timer.h"
#include "Camera.h"
#include "DeferredContext.h"
#include "Buffer.h"
#include "Texture.h"
#include "Sampler.h"
#include "Shader.h"
#include "DrawList.h"
#include "Viewport.h"
#include "LoadImage.h"
#include "AssetManager.h"
#include "Archive.h"
#include "FileStream.h"
#include "Importer.h"
#include "IndexBuffer.h"
#include "ConstBuffer.h"
#include "VertexBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "GeometryShader.h"
#include "Font.h"
#include "Debug.h"
#include "Sprite.h"
#include "Window.h"
#include "DXWindow.h"
#include "Shaders/Default.shader.h"
#include "Mesh.h"
#include "Physics.h"
#include "PhysicsDebug.h"
#include "UI.h"
