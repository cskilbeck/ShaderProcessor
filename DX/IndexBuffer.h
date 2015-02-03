//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

template<typename T> DXGI_FORMAT GetIndexBufferFormat()
{
	return DXGI_FORMAT_UNKNOWN;
}

template<> inline DXGI_FORMAT GetIndexBufferFormat<uint16>()
{
	return DXGI_FORMAT_R16_UINT;
}

template<> inline DXGI_FORMAT GetIndexBufferFormat<uint32>()
{
	return DXGI_FORMAT_R32_UINT;
}

template <typename T> struct IndexBuffer: Buffer <T>
{
	IndexBuffer()
	{
	}

	IndexBuffer(uint count, T *data = null, BufferUsage usage = StaticUsage, ReadWriteOption rwOption = NotCPUAccessible)
	{
		Create(count, data, usage, rwOption);
	}

	HRESULT Create(uint count, T *data = null, BufferUsage usage = StaticUsage, ReadWriteOption rwOption = NotCPUAccessible)
	{
		return Buffer<T>::Create(IndexBufferType, count, data, usage, rwOption);
	}

	void Activate(ID3D11DeviceContext *context, uint offset = 0)
	{
		context->IASetIndexBuffer(mBuffer, GetIndexBufferFormat<T>(), offset);
	}
};

