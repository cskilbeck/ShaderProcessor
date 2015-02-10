//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	struct DrawList
	{
		DrawList();
		~DrawList();

		void Reset();
		void SetShader(VertexShader *vs, PixelShader *ps, TypelessBuffer *vb, uint vertexSize);
		void SetMaterial(Material &m);
		void SetPSTexture(Texture &t, uint index = 0);
		void SetPSSampler(Sampler &s, uint index = 0);
		void SetVSConstantData(byte *data, uint size, uint index);
		void SetPSConstantData(byte *data, uint size, uint index);
		void BeginTriangleList();
		void BeginTriangleStrip();
		void BeginLineList();
		void BeginLineStrip();
		template <typename T> void AddVertex(T const &vertex);
		void End();
		void Execute(ID3D11DeviceContext *context);

	private:

		template<typename T> T *Add();
		byte *AddData(byte const *data, uint size);
		void BeginDrawCall(uint32 topology);

		byte *mItemBuffer;
		byte *mItemPointer;
		uint32 mVertBase;
		uint32 mVertCount;
		byte *mVertPointer;
		void *mCurrentDrawCallItem;
	};

	//////////////////////////////////////////////////////////////////////

	template<typename T> inline T *DrawList::Add()
	{
		T *p = (T *)mItemPointer;
		mItemPointer += sizeof(T);
		p->mType = (uint)T::eType;
		return p;
	}

	//////////////////////////////////////////////////////////////////////

	template <typename T> inline void DrawList::AddVertex(T const &vertex)
	{
		*((T *)mVertPointer) = vertex;
		mVertPointer += sizeof(T);
		++mVertCount;
	}
}
