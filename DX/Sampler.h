//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	struct Sampler
	{
		struct Options
		{
			TextureFilter Filter;
			TextureAddressingMode U;
			TextureAddressingMode V;
			TextureAddressingMode W;
			float MipLodBias;
			uint32 MaxAnisotropy;
			ComparisonFunc comparisonFunc;
			Color BorderColor;
			float MinLOD;
			float MaxLOD;

			Options(TextureFilter filter = TextureFilter::default_filter,
					TextureAddressingMode u = TextureAddressDefault,
					TextureAddressingMode v = TextureAddressDefault,
					TextureAddressingMode w = TextureAddressDefault,
					Color BorderColor = Color::Transparent,
					float mipLodBias = 0.0f,
					uint32 maxAnisotropy = 0,
					ComparisonFunc comparisonFunc = ComparisonFunc::never,
					float minLOD = -FLT_MAX,
					float maxLOD = FLT_MAX)
				: Filter(filter)
				, U(u), V(v), W(w)
				, MipLodBias(mipLodBias)
				, MaxAnisotropy(maxAnisotropy)
				, comparisonFunc(comparisonFunc)
				, BorderColor(BorderColor)
				, MinLOD(minLOD)
				, MaxLOD(maxLOD)
			{
			}
		};

		Sampler()
		{
			DXT(Create(CD3D11_SAMPLER_DESC(D3D11_DEFAULT)));
		}

		Sampler(Options &options)
		{
			D3D11_SAMPLER_DESC desc;
			desc.Filter = (D3D11_FILTER)options.Filter;
			desc.AddressU = (D3D11_TEXTURE_ADDRESS_MODE)options.U;
			desc.AddressU = (D3D11_TEXTURE_ADDRESS_MODE)options.V;
			desc.AddressU = (D3D11_TEXTURE_ADDRESS_MODE)options.W;
			options.BorderColor.GetFloatsRGBA(desc.BorderColor);
			desc.ComparisonFunc = (D3D11_COMPARISON_FUNC)options.comparisonFunc;
			desc.MaxAnisotropy = options.MaxAnisotropy;
			desc.MaxLOD = options.MaxLOD;
			desc.MinLOD = options.MinLOD;
			Create(desc);
		}

		HRESULT Create(D3D11_SAMPLER_DESC &desc)
		{
			DXR(DX::Device->CreateSamplerState(&desc, &mSamplerState));
			return S_OK;
		}

		DXPtr<ID3D11SamplerState> mSamplerState;
	};
}