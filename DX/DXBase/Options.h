//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX {
//////////////////////////////////////////////////////////////////////

enum TextureFilter
{
    min_mag_mip_point = D3D11_FILTER_MIN_MAG_MIP_POINT,
    min_mag_point_mip_linear = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR,
    min_point_mag_linear_mip_point = D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT,
    min_point_mag_mip_linear = D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR,
    min_linear_mag_mip_point = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT,
    min_linear_mag_point_mip_linear = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
    min_mag_linear_mip_point = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,
    min_mag_mip_linear = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
    anisotropic = D3D11_FILTER_ANISOTROPIC,
    comparison_min_mag_mip_point = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT,
    comparison_min_mag_point_mip_linear = D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR,
    comparison_min_point_mag_linear_mip_point = D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT,
    comparison_min_point_mag_mip_linear = D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR,
    comparison_min_linear_mag_mip_point = D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT,
    comparison_min_linear_mag_point_mip_linear = D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
    comparison_min_mag_linear_mip_point = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
    comparison_min_mag_mip_linear = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR,
    comparison_anisotropic = D3D11_FILTER_COMPARISON_ANISOTROPIC,
    minimum_min_mag_mip_point = D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT,
    minimum_min_mag_point_mip_linear = D3D11_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR,
    minimum_min_point_mag_linear_mip_point = D3D11_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT,
    minimum_min_point_mag_mip_linear = D3D11_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR,
    minimum_min_linear_mag_mip_point = D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT,
    minimum_min_linear_mag_point_mip_linear = D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
    minimum_min_mag_linear_mip_point = D3D11_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT,
    minimum_min_mag_mip_linear = D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR,
    minimum_anisotropic = D3D11_FILTER_MINIMUM_ANISOTROPIC,
    maximum_min_mag_mip_point = D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT,
    maximum_min_mag_point_mip_linear = D3D11_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR,
    maximum_min_point_mag_linear_mip_point = D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT,
    maximum_min_point_mag_mip_linear = D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR,
    maximum_min_linear_mag_mip_point = D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT,
    maximum_min_linear_mag_point_mip_linear = D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
    maximum_min_mag_linear_mip_point = D3D11_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT,
    maximum_min_mag_mip_linear = D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR,
    maximum_anisotropic = D3D11_FILTER_MAXIMUM_ANISOTROPIC,

    default_filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR
};


enum ComparisonFunc
{
    never = D3D11_COMPARISON_NEVER,
    less = D3D11_COMPARISON_LESS,
    equal = D3D11_COMPARISON_EQUAL,
    less_equal = D3D11_COMPARISON_LESS_EQUAL,
    greater = D3D11_COMPARISON_GREATER,
    not_equal = D3D11_COMPARISON_NOT_EQUAL,
    greater_equal = D3D11_COMPARISON_GREATER_EQUAL,
    always = D3D11_COMPARISON_ALWAYS
};

//////////////////////////////////////////////////////////////////////

enum TextureAddressingMode
{
    TextureAddressWrap = D3D11_TEXTURE_ADDRESS_WRAP,
    TextureAddressMirror = D3D11_TEXTURE_ADDRESS_MIRROR,
    TextureAddressClamp = D3D11_TEXTURE_ADDRESS_CLAMP,
    TextureAddressBorder = D3D11_TEXTURE_ADDRESS_BORDER,
    TextureAddressMirrorOnce = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE,
    TextureAddressDefault = D3D11_TEXTURE_ADDRESS_CLAMP
};

//////////////////////////////////////////////////////////////////////

enum DepthClearOption
{
    Nothing = 0,
    DepthOnly = D3D11_CLEAR_DEPTH,
    StencilOnly = D3D11_CLEAR_STENCIL,
    DepthAndStencil = DepthOnly + StencilOnly
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

//////////////////////////////////////////////////////////////////////

enum WindowBorderOption
{
    WithWindowBorder = 0,
    WithoutWindowBorder = 1
};

}    // namespace DX