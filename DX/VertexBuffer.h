#pragma once

namespace DX
{
	template<typename vert> struct VertexBuffer: Buffer<vert>
	{
		VertexBuffer()
			: Buffer<vert>()
		{
		}

		HRESULT Create(uint count, vert *data = null, BufferUsage usage = DynamicUsage, ReadWriteOption rwOption = Writeable)
		{
			DXR(Buffer<vert>::CreateBuffer(VertexBufferType, count, data, usage, rwOption));
			return S_OK;
		}

		void Activate(ID3D11DeviceContext *context)
		{
			uint stride = SizeOf();
			uint offset = 0;
			context->IASetVertexBuffers(0, 1, &mBuffer, &stride, &offset);
		}
	};
}
