//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	struct Texture
	{
		Texture(tchar const *name);
		Texture(int w, int h, Color color);
		Texture(int w, int h, DXGI_FORMAT format, byte *pixels, bool isRenderTarget = false);
		virtual ~Texture();

		static void FlushAll();
		static Texture *Grid(int w, int h, int gridWidth, int gridHeight, Color color1, Color color2);

		void Update(ID3D11DeviceContext *sContext, byte *pixels);
		int Width() const;
		int Height() const;
		uint BitsPerPixel() const;
		float FWidth() const;
		float FHeight() const;
		Vec2f FSize() const;
		bool IsValid() const;
		tstring const &GetName() const;

		list_node<Texture> mListNode;

	protected:

		friend struct Shader;

		void InitFromPixelBuffer(byte *buffer, DXGI_FORMAT pixelFormat, int width, int height, bool renderTarget);

		tstring							mName;
		DXPtr<ID3D11Texture2D>			mTexture2D;
		DXPtr<ID3D11ShaderResourceView>	mShaderResourceView;
		CD3D11_TEXTURE2D_DESC			mTextureDesc;
	};

	//////////////////////////////////////////////////////////////////////

	struct RenderTarget: Texture
	{
		enum RenderTargetDepthOption
		{
			WithDepthBuffer = 0,
			WithoutDepthBuffer = 1
		};

		RenderTarget(int w, int h, RenderTargetDepthOption depthOption);
		virtual ~RenderTarget();

		void Activate(ID3D11DeviceContext *context);
		void Clear(ID3D11DeviceContext *context, Color color);
		void ClearDepth(ID3D11DeviceContext *context, DepthClearOption option, float z = 1.0f, byte stencil = 0);

	protected:

		HRESULT CreateRenderTargetView();
		HRESULT CreateDepthBuffer();
		HRESULT CreateDepthStencilView();

		DXPtr<ID3D11RenderTargetView> mRenderTargetView;
		DXPtr<ID3D11Texture2D> mDepthBuffer;
		DXPtr<ID3D11DepthStencilView> mDepthStencilView;
	};

	//////////////////////////////////////////////////////////////////////

	inline int Texture::Width() const
	{
		return mTextureDesc.Width;
	}

	inline int Texture::Height() const
	{
		return mTextureDesc.Height;
	}

	inline float Texture::FWidth() const
	{
		return (float)mTextureDesc.Width;
	}

	inline float Texture::FHeight() const
	{
		return (float)mTextureDesc.Height;
	}

	inline Vec2f Texture::FSize() const
	{
		return Vec2f((float)Width(), (float)Height());
	}

	inline bool Texture::IsValid() const
	{
		return mTexture2D != null;
	}

	inline tstring const &Texture::GetName() const
	{
		return mName;
	}
}