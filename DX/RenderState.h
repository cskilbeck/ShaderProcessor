//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	template<typename shader> struct RenderState
	{
		Material mMaterial;
		Ptr<shader> mShader;

		RenderState()
		{
		}

		RenderState(MaterialOptions &materialOptions)
		{
			DXT(Create(materialOptions, new shader()));
		}

		HRESULT Create(MaterialOptions &materialOptions, shader *s)
		{
			DXR(mMaterial.Create(materialOptions));
			mShader.reset(s);
			return S_OK;
		}

		void Release()
		{
			mMaterial.Release();
			mShader.reset();
		}

		void Activate(ID3D11DeviceContext *context)
		{
			mMaterial.Activate(context);
			mShader->Activate(context);
		}
	};
}