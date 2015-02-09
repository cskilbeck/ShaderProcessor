//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	enum DepthClearOption
	{
		Nothing = 0,
		DepthOnly = D3D11_CLEAR_DEPTH,
		StencilOnly = D3D11_CLEAR_STENCIL,
		DepthAndStencil = DepthOnly + StencilOnly
	};

	//////////////////////////////////////////////////////////////////////

	enum MaterialFillModeOption
	{
		FillModeSolid = D3D11_FILL_SOLID,
		FillModeWireFrame = D3D11_FILL_WIREFRAME,

		FillModeDefault = FillModeSolid,
	};

	//////////////////////////////////////////////////////////////////////

	enum MaterialDepthWriteOption
	{
		DepthWriteEnabled = D3D11_DEPTH_WRITE_MASK_ALL,
		DepthWriteDisabled = D3D11_DEPTH_WRITE_MASK_ZERO,

		DepthWriteDefault = DepthWriteEnabled,
	};

	//////////////////////////////////////////////////////////////////////

	enum MaterialDepthOption
	{
		DepthEnabled = TRUE,
		DepthDisabled = FALSE,

		DepthDefault = DepthEnabled,
	};

	//////////////////////////////////////////////////////////////////////

	enum MaterialBlendOption
	{
		BlendDisabled = FALSE,
		BlendEnabled = TRUE,

		BlendDefault = BlendDisabled,
	};

	//////////////////////////////////////////////////////////////////////

	enum MaterialCullingOption
	{
		CullingBackFaces = D3D11_CULL_BACK,
		CullingFrontFaces = D3D11_CULL_FRONT,
		CullingNone = D3D11_CULL_NONE,

		CullingDefault = CullingBackFaces,
	};

	//////////////////////////////////////////////////////////////////////

	enum MaterialCullingOrderOption
	{
		CullClockwise = FALSE,
		CullAntiClockwise = TRUE,

		CullOrderDefault = CullClockwise,
	};

	//////////////////////////////////////////////////////////////////////

	enum BufferType
	{
		IndexBufferType = D3D11_BIND_INDEX_BUFFER,
		VertexBufferType = D3D11_BIND_VERTEX_BUFFER,
		ConstantBufferType = D3D11_BIND_CONSTANT_BUFFER
	};

	//////////////////////////////////////////////////////////////////////

	enum BufferUsage
	{
		DefaultUsage = D3D11_USAGE_DEFAULT,
		StaticUsage = D3D11_USAGE_IMMUTABLE,
		DynamicUsage = D3D11_USAGE_DYNAMIC
	};

	//////////////////////////////////////////////////////////////////////

	enum ReadWriteOption
	{
		NotCPUAccessible = 0,
		Readable = D3D11_CPU_ACCESS_READ,
		Writeable = D3D11_CPU_ACCESS_WRITE,
		ReadWriteable = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ
	};

	//////////////////////////////////////////////////////////////////////

	enum MapWaitOption
	{
		WaitForGPU = 0,
		DontWaitForGPU = D3D11_MAP_FLAG_DO_NOT_WAIT
	};

	//////////////////////////////////////////////////////////////////////

	enum DepthBufferOption
	{
		DepthBufferEnabled = 0,
		DepthBufferDisabled = 1
	};

	//////////////////////////////////////////////////////////////////////

	enum FullScreenOption
	{
		Windowed = 0,
		FullScreen = 1
	};

	//////////////////////////////////////////////////////////////////////

	enum CreateSwapChainOption
	{
		WithSwapChain = 0,
		WithoutSwapChain = 1
	};

}