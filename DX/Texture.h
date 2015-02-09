//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	struct Texture
	{
		Texture();
		Texture(tchar const *name);
		Texture(int w, int h, Color color);
		Texture(int w, int h, DXGI_FORMAT format, byte *pixels);
		~Texture();

		void Update(ID3D11DeviceContext *sContext, byte *pixels);
		int Width() const;
		int Height() const;
		uint BitsPerPixel() const;
		float FWidth() const;
		float FHeight() const;
		Vec2f FSize() const;
		bool IsValid() const;
		tstring const &GetName() const;

		void InitFromPixelBuffer(byte *buffer, DXGI_FORMAT pixelFormat, int width, int height);

		tstring							mName;
		DXPtr<ID3D11Texture2D>			mTexture2D;
		DXPtr<ID3D11ShaderResourceView>	mShaderResourceView;
		CD3D11_TEXTURE2D_DESC			mTextureDesc;
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