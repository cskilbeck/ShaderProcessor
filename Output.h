
namespace HLSL
{
#pragma pack(push, 4)

	struct UntexturedVertexShader_t
	{
		struct VertConstants_t
		{
			Float4x4 ProjectionMatrix;
			Float1 bob;			byte pad0[12];
		};

		VertConstants_t VertConstants;

		// VertConstants Offsets
		DECLSPEC_SELECTANY extern CBufferOffset const VertConstants_Offsets[2] = 
		{
			{ "ProjectionMatrix", 0 },
			{ "bob", 64 }
		};

		// VertConstants Defaults
		DECLSPEC_SELECTANY extern uint32 const VertConstants_Defaults[20] = 
		{
			// ProjectionMatrix
			0x00000000,0x00000000,0x00000000,0x00000000,
			0x00000000,0x00000000,0x00000000,0x00000000,
			0x00000000,0x00000000,0x00000000,0x00000000,
			0x00000000,0x00000000,0x00000000,0x00000000,
			// bob
			0x40000000,0x00000000,0x00000000,0x00000000
		};

		DECLSPEC_SELECTANY InputElements[2] =
		{
			{ "HALF_Pos", 0, DXGI_FORMAT_R16G16_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BYTE_Color", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		struct Input
		{
			Half2 Pos;
			Byte4 Color;
		};

		// Constructor
		UntexturedVertexShader_t()
			: VertConstants(VertConstants_Defaults)
		{
		}
	};

	DECLSPEC_SELECTANY UntexturedVertexShader_t UntexturedVertexShader;

#pragma pack(pop)

} // HLSL
