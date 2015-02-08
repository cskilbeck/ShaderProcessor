//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{

	struct Sampler
	{
		Sampler()
		{
			CD3D11_SAMPLER_DESC desc(D3D11_DEFAULT);
			DXT(DX::Device->CreateSamplerState(&desc, &mSamplerState));
		}

		DXPtr<ID3D11SamplerState> mSamplerState;
	};

}