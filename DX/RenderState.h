//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	template<typename VShader, typename GShader, typename PShader> struct RenderState
	{
		Material mMaterial;
		Ptr<VShader> mVertexShader;
		Ptr<GShader> mGeometryShader;
		Ptr<PShader> mPixelShader;

		RenderState()
		{
		}

		RenderState(MaterialOptions &materialOptions)
		{
			DXT(Create(materialOptions, new VShader(), new GShader(), new PShader()));
		}

		HRESULT Create(MaterialOptions &materialOptions,
					VShader *vertexShader,
					GShader *geometryShader,
					PShader *pixelShader)
		{
			DXR(mMaterial.Create(materialOptions));
			mVertexShader.reset(vertexShader);
			mGeometryShader.reset(geometryShader);
			mPixelShader.reset(pixelShader);
			return S_OK;
		}

		void Release()
		{
			mMaterial.Release();
			mVertexShader.reset();
			mGeometryShader.reset();
			mPixelShader.reset();
		}

		void Activate(ID3D11DeviceContext *context)
		{
			mMaterial.Activate(context);

			if(mVertexShader != null)
			{
				mVertexShader->Activate(context);
			}
			else
			{
				context->VSSetShader(null, null, 0);
			}

			if(mGeometryShader != null)
			{
				mGeometryShader->Activate(context);
			}
			else
			{
				context->GSSetShader(null, null, 0);
			}

			if(mPixelShader != null)
			{
				mPixelShader->Activate(context);
			}
			else
			{
				context->PSSetShader(null, null, 0);
			}
		}
	};
}