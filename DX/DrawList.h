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
		void SetShader(ShaderState *shader, TypelessBuffer *vb, uint vertexSize);
		void SetMaterial(Material &m);
		void SetPSTexture(Texture &t, uint index = 0);
		void SetPSSampler(Sampler &s, uint index = 0);
		template <typename T> void SetVSConstantData(T &data, uint index);
		template <typename T> void SetGSConstantData(T &data, uint index);
		template <typename T> void SetPSConstantData(T &data, uint index);
		void BeginPointList();
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

		void SetVSConsts(byte *data, uint size, uint index);
		void SetGSConsts(byte *data, uint size, uint index);
		void SetPSConsts(byte *data, uint size, uint index);

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

	//////////////////////////////////////////////////////////////////////

	template <typename T> inline void DrawList::SetVSConstantData(T &data, uint index)
	{
		SetVSConsts((byte *)&data, sizeof(T), index);
	}

	//////////////////////////////////////////////////////////////////////

	template <typename T> inline void DrawList::SetGSConstantData(T &data, uint index)
	{
		SetGSConsts((byte *)&data, sizeof(T), index);
	}

	//////////////////////////////////////////////////////////////////////

	template <typename T> inline void DrawList::SetPSConstantData(T &data, uint index)
	{
		SetPSConsts((byte *)&data, sizeof(T), index);
	}
}
