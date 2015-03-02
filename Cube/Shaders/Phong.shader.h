//////////////////////////////////////////////////////////////////////
// Phong.h - auto generated file, do not edit

#pragma once
#pragma pack(push, 4)

namespace Shaders
{
	using namespace DX;

	// D3D11_DEPTH_STENCIL_DESC
	uint32 WEAKSYM Phong_DepthStencilDesc[] = 
	{
		0x00000001,0x00000001,0x00000002,0x00000000,0x0000ffff,0x00000001,0x00000001,0x00000001,
		0x00000008,0x00000001,0x00000001,0x00000001,0x00000008
	};

	// D3D11_RASTERIZER_DESC
	uint32 WEAKSYM Phong_RasterizerDesc[] = 
	{
		0x00000003,0x00000003,0x00000000,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,
		0x00000000,0x00000000
	};

	// D3D11_BLEND_DESC
	uint32 WEAKSYM Phong_BlendDesc[] = 
	{
		0x00000000,0x00000000,0x00000000,0x00000002,0x00000001,0x00000001,0x00000002,0x00000001,
		0x00000001,0xcccccc0f,0x00000000,0x00000002,0x00000001,0x00000001,0x00000002,0x00000001,
		0x00000001,0xcccccc0f,0x00000000,0x00000002,0x00000001,0x00000001,0x00000002,0x00000001,
		0x00000001,0xcccccc0f,0x00000000,0x00000002,0x00000001,0x00000001,0x00000002,0x00000001,
		0x00000001,0xcccccc0f,0x00000000,0x00000002,0x00000001,0x00000001,0x00000002,0x00000001,
		0x00000001,0xcccccc0f,0x00000000,0x00000002,0x00000001,0x00000001,0x00000002,0x00000001,
		0x00000001,0xcccccc0f,0x00000000,0x00000002,0x00000001,0x00000001,0x00000002,0x00000001,
		0x00000001,0xcccccc0f,0x00000000,0x00000002,0x00000001,0x00000001,0x00000002,0x00000001,
		0x00000001,0xcccccc0f
	};

	//////////////////////////////////////////////////////////////////////
	// offsets and defaults

	//////////////////////////////////////////////////////////////////////
	// VertConstants offsets

	extern ConstBufferOffset const WEAKSYM Phong_VS_VertConstants_Offsets[2] =
	{
		{ "ModelMatrix", 0 },
		{ "TransformMatrix", 64 }
	};

	// no defaults for VertConstants

	//////////////////////////////////////////////////////////////////////
	// const buffer names table

	extern char const WEAKSYM *Phong_VS_ConstBufferNames[] =
	{
		"VertConstants"
	};

	//////////////////////////////////////////////////////////////////////
	// offsets and defaults

	//////////////////////////////////////////////////////////////////////
	// Camera offsets

	extern ConstBufferOffset const WEAKSYM Phong_PS_Camera_Offsets[1] =
	{
		{ "cameraPos", 0 }
	};

	// no defaults for Camera

	//////////////////////////////////////////////////////////////////////
	// Light offsets

	extern ConstBufferOffset const WEAKSYM Phong_PS_Light_Offsets[4] =
	{
		{ "lightPos", 0 },
		{ "ambientColor", 16 },
		{ "diffuseColor", 32 },
		{ "specColor", 48 }
	};

	// no defaults for Light

	//////////////////////////////////////////////////////////////////////
	// const buffer names table

	extern char const WEAKSYM *Phong_PS_ConstBufferNames[] =
	{
		"Camera",
		"Light"
	};

	//////////////////////////////////////////////////////////////////////
	// Sampler names

	extern char const WEAKSYM * Phong_PS_SamplerNames[] =
	{
		"tex1Sampler"
	};

	//////////////////////////////////////////////////////////////////////
	// Texture names

	extern char const WEAKSYM * Phong_PS_TextureNames[] =
	{
		"picTexture"
	};

	//////////////////////////////////////////////////////////////////////
	// Input Element descs

	extern D3D11_INPUT_ELEMENT_DESC const WEAKSYM Phong_VS_InputElements[4] =
	{
		{ "float_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "half_TexCoord", 0, DXGI_FORMAT_R16G16_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "byte_Color", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "float_Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	//////////////////////////////////////////////////////////////////////
	// Binding runs

	extern Shader::BindingState WEAKSYM Phong_Bindings[] = {
		{{}, {}, {{{0,1}}}},
		{},
		{},
		{},
		{{{{4,1}}}, {{{1,1}}}, {{{1,2}}}},
		{}
	};

	//////////////////////////////////////////////////////////////////////
	// Shader struct

	struct Phong : ShaderState
	{
		//////////////////////////////////////////////////////////////////////
		// InputVertex

		struct InputVertex
		{
			Float3 Position;
			Half2 TexCoord;
			Byte4 Color;
			Float3 Normal;
		};

		using VertBuffer = VertexBuffer<InputVertex>;

		//////////////////////////////////////////////////////////////////////
		// Vertex Shader

		struct VS : VertexShader
		{
			// Const Buffers

			struct VertConstants_t
			{
				Float4x4 ModelMatrix;
				Float4x4 TransformMatrix;
			};

			ConstBuffer<VertConstants_t> VertConstants;
			enum { VertConstants_index = 0 };

			// Constructor

			VS()
				: VertexShader(1, Phong_VS_ConstBufferNames, 0, null, 0, null, null, null, Phong_Bindings[ShaderType::Vertex])
				, VertConstants(2, Phong_VS_VertConstants_Offsets, null, this, 0, 0)
			{
			}
		};

		//////////////////////////////////////////////////////////////////////
		// Pixel Shader

		struct PS : PixelShader
		{
			// Const Buffers

			struct Light_t
			{
				Float3 lightPos;				private: byte pad0[4]; public:
				Float3 ambientColor;				private: byte pad1[4]; public:
				Float3 diffuseColor;				private: byte pad2[4]; public:
				Float3 specColor;				private: byte pad3[4]; public:
			};

			ConstBuffer<Light_t> Light;
			enum { Light_index = 1 };

			struct Camera_t
			{
				Float3 cameraPos;				private: byte pad0[4]; public:
			};

			ConstBuffer<Camera_t> Camera;
			enum { Camera_index = 0 };

			// Samplers

			union
			{
				struct
				{
					Sampler *tex1Sampler;
				};
				Sampler *samplers[1];
			};

			// Textures

			union
			{
				struct
				{
					Texture *picTexture;
				};
				Texture *textures[1];
			};

			// Constructor

			PS()
				: PixelShader(2, Phong_PS_ConstBufferNames, 1, Phong_PS_SamplerNames, 1, Phong_PS_TextureNames, textures, samplers, Phong_Bindings[ShaderType::Pixel])
				, tex1Sampler(null)
				, picTexture(null)
				, Light(4, Phong_PS_Light_Offsets, null, this, 1, 1)
				, Camera(1, Phong_PS_Camera_Offsets, null, this, 0, 2)
			{
			}
		};

		//////////////////////////////////////////////////////////////////////
		// Members

		VS vs;
		PS ps;

		//////////////////////////////////////////////////////////////////////
		// Constructor

		Phong(): ShaderState(Phong_BlendDesc, Phong_DepthStencilDesc, Phong_RasterizerDesc)
		{
			FileResource f(TEXT("Data\\Phong.sob"));
			vs.Load(f, Phong_VS_InputElements, _countof(Phong_VS_InputElements));
			ps.Load(f);
			Shaders[Vertex] = &vs;
			Shaders[Hull] = null;
			Shaders[Domain] = null;
			Shaders[Geometry] = null;
			Shaders[Pixel] = &ps;
			Shaders[Compute] = null;
		}

		//////////////////////////////////////////////////////////////////////
		// Activate

		void Activate(ID3D11DeviceContext *context)
		{
			ShaderState::SetState(context);
			vs.Activate(context);
			context->HSSetShader(null, null, 0);
			context->DSSetShader(null, null, 0);
			context->GSSetShader(null, null, 0);
			ps.Activate(context);
			context->CSSetShader(null, null, 0);
		}

		void Activate_V(ID3D11DeviceContext *context) override
		{
			Activate(context);
		}
	};
}

#pragma pack(pop)
