#pragma once

namespace DX
{
	template<typename vert> struct VertexBuffer: Buffer<vert>
	{
		static uint VertexSize()
		{
			return sizeof(vert);
		}

		VertexBuffer(uint vertCount, vert *data = null, BufferUsage usage = DynamicUsage, ReadWriteOption rwOption = Writeable)
		{
			DXT(Create(vertCount, data, usage, rwOption));
		}

		HRESULT Create(uint count, vert *data = null, BufferUsage usage = DynamicUsage, ReadWriteOption rwOption = Writeable)
		{
			return Buffer<vert>::Create(VertexBufferType, count, data, usage, rwOption);
		}

		void Activate(ID3D11DeviceContext *context)
		{
			uint stride = sizeof(vert);
			uint offset = 0;
			context->IASetVertexBuffers(0, 1, &mBuffer, &stride, &offset);
		}
	};
}
