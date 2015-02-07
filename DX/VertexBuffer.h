//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

template<typename vert> struct VertexBuffer
{
	VertexBuffer(uint vertCount)
	{
		Create(new vert[vertCount], vertCount, true);
	}

	VertexBuffer(uint vertCount, vert *data)
	{
		Create(data, vertCount, false);
	}

	~VertexBuffer()
	{
		if(mOwnBuffer)
		{
			Delete(mBuffer);
		}
	}

	void Commit(ID3D11DeviceContext *context)
	{
		context->UpdateSubresource(mD3DBuffer, 0, null, mBuffer, 0, 0);
	}

	void Activate(ID3D11DeviceContext *context)
	{
		uint stride = sizeof(vert);
		uint offset = 0;
		context->IASetVertexBuffers(0, 1, &mD3DBuffer, &stride, &offset);
	}

	vert &operator[](uint index)
	{
		return mBuffer[index];
	}

	vert *operator &()
	{
		return mBuffer;
	}

	uint32 VertexCount() const
	{
		return mVertexCount;
	}

private:

	void Create(vert *data, uint32 vertCount, bool own)
	{
		mVertexCount = vertCount;
		mOwnBuffer = own;
		mBuffer = data;
		CD3D11_BUFFER_DESC desc(sizeof(vert) * vertCount, D3D11_BIND_VERTEX_BUFFER);
		D3D11_SUBRESOURCE_DATA srd = { data, 0, 0 };
		DXT(D3D::Device->CreateBuffer(&desc, &srd, &mD3DBuffer));
	}

	uint32				mVertexCount;
	bool				mOwnBuffer;
	vert *				mBuffer;
	DXPtr<ID3D11Buffer> mD3DBuffer;
};

