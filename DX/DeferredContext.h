//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	struct DeferredContext
	{
		DeferredContext(ID3D11Device *device)
		{
			DXT(Create(device));
		}

		HRESULT Create(ID3D11Device *device)
		{
			DXR(device->CreateDeferredContext(false, &mContext));
		}

		void Finish()
		{
			mContext->FinishCommandList(false, &mCommandList);
		}

		void Execute(ID3D11DeviceContext *context)
		{
			context->ExecuteCommandList(mCommandList, false);
		}

		DXPtr<ID3D11DeviceContext> mContext;
		DXPtr<ID3D11CommandList> mCommandList;
	};
}