//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

template<typename vert> struct VertexBuffer
{
	VertexBuffer(uint vertCount)
	{
		Create(new vert[vertCount], true);
	}

	VertexBuffer(uint vertCount, vert *data)
	{
		Create(data, false);
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
		context->UpdateSubresource(mD3DBuffer, 0, null, mBuffer.data(), 0, 0);
	}

	void Activate(ID3D11DeviceContext *context)
	{
		context->IASetVertexBuffers(0, 1, &mD3DBuffer, null, null);
	}

	vert &operator[](uint index)
	{
		return mBuffer[index];
	}

	vert *operator &()
	{
		return mBuffer.data();
	}

private:

	void Create(vert *data, bool own)
	{
		mOwnBuffer = own;
		mBuffer = data;
		CD3D11_BUFFER_DESC desc(sizeof(vert) * vertCount, D3D11_BIND_VERTEX_BUFFER);
		DXT(gDevice->CreateBuffer(&desc, mBuffer, &(t->mD3DBuffer)));
	}

	bool				mOwnBuffer;
	vert *				mBuffer;
	DXPtr<ID3D11Buffer> mD3DBuffer;
};

