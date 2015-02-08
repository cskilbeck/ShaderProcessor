//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{

	template<typename vert> struct VertexBuffer: Buffer < vert >
	{
		VertexBuffer(uint vertCount, vert *data = null)
		{
			DXT(Create(vertCount, data));
		}

		HRESULT Create(uint count, vert *data = null, BufferUsage usage = DefaultUsage, ReadWriteOption rwOption = NotCPUAccessible)
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