//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{

	template<typename T> struct IndexBufferFormat
	{
		enum
		{
			format = DXGI_FORMAT_UNKNOWN
		};
	};

	template<> struct IndexBufferFormat<uint16>
	{
		enum
		{
			format = DXGI_FORMAT_R16_UINT
		};
	};

	template<> struct IndexBufferFormat<uint32>
	{
		enum
		{
			format = DXGI_FORMAT_R32_UINT
		};
	};

	//////////////////////////////////////////////////////////////////////

	template <typename T> struct IndexBuffer: Buffer<T>
	{
		static_assert(IndexBufferFormat<T>::format != DXGI_FORMAT_UNKNOWN, "Only uint16 or uint32 index buffers are supported");

		IndexBuffer(T count, T *data = null, BufferUsage usage = DynamicUsage, ReadWriteOption rwOption = Writeable)
		{
			DXT(Create(count, data, usage, rwOption));
		}

		HRESULT Create(T count, T *data = null, BufferUsage usage = DynamicUsage, ReadWriteOption rwOption = Writeable)
		{
			return Buffer<T>::Create(IndexBufferType, count, data, usage, rwOption);
		}

		void Activate(ID3D11DeviceContext *context, uint offset = 0)
		{
			context->IASetIndexBuffer(mBuffer, (DXGI_FORMAT)IndexBufferFormat<T>::format, offset);
		}
	};

}