//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	struct MaterialOptions
	{
		MaterialDepthOption depth;
		MaterialDepthWriteOption depthWrite;
		MaterialFillModeOption fillMode;
		MaterialBlendOption blend;
		MaterialCullingOption culling;
		MaterialCullingOrderOption cullingOrder;

		MaterialOptions(MaterialDepthOption depth = DepthDefault,
						MaterialDepthWriteOption depthWrite = DepthWriteDefault,
						MaterialBlendOption blend = BlendDefault,
						MaterialCullingOption culling = CullingDefault,
						MaterialFillModeOption fillMode  = FillModeDefault,
						MaterialCullingOrderOption cullingOrder = CullOrderDefault)
			: depth(depth)
			, depthWrite(depthWrite)
			, fillMode(fillMode)
			, blend(blend)
			, culling(culling)
			, cullingOrder(cullingOrder)
		{
		}
	};

	//////////////////////////////////////////////////////////////////////

	struct Material
	{
		HRESULT Create(MaterialOptions const &options)
		{
			CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
			CD3D11_DEPTH_STENCIL_DESC depthstencilDesc(D3D11_DEFAULT);
			CD3D11_BLEND_DESC blendDesc(D3D11_DEFAULT);

			rasterizerDesc.FillMode = (D3D11_FILL_MODE)options.fillMode;
			rasterizerDesc.CullMode = (D3D11_CULL_MODE)options.culling;
			depthstencilDesc.DepthEnable = options.depth;
			depthstencilDesc.DepthWriteMask = (D3D11_DEPTH_WRITE_MASK)options.depthWrite;
			rasterizerDesc.FrontCounterClockwise = options.cullingOrder;
			blendDesc.RenderTarget[0].BlendEnable = options.blend;
			blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;

			DXR(Device->CreateRasterizerState(&rasterizerDesc, &mRasterizerState));
			DXR(Device->CreateDepthStencilState(&depthstencilDesc, &mDepthStencilState));
			DXR(Device->CreateBlendState(&blendDesc, &mBlendState));

			return S_OK;
		}

		void Activate(ID3D11DeviceContext *context)
		{
			context->RSSetState(mRasterizerState);
			context->OMSetDepthStencilState(mDepthStencilState, 0);
			context->OMSetBlendState(mBlendState, null, (uint32)-1);
		}

		void Release()
		{
			mRasterizerState.Release();
			mBlendState.Release();
			mDepthStencilState.Release();
		}

		DXPtr<ID3D11RasterizerState> mRasterizerState;
		DXPtr<ID3D11DepthStencilState> mDepthStencilState;
		DXPtr<ID3D11BlendState> mBlendState;
	};

}