//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	struct ConstBufferOffset
	{
		char const *name;		// name of this variable
		uint32 offset;			// offset in the constbuffer
		uint32 type;			// type (eg float, int etc)
		uint32 elements;		// 1 for a single var, or >1 for an array (a float4 is a single var)

		bool operator == (ConstBufferOffset const &o) const
		{
			return strcmp(name, o.name) == 0 &&
				offset == o.offset &&
				type == o.type &&
				elements == o.elements;
		}
	};

	template<typename definition> struct ConstBuffer: definition, Buffer<definition>	// definition MUST be POD and Aligned16
	{
		ConstBuffer()
			: Buffer<definition>()
			, mOffsetCount(0)
			, mOffsets(null)
			, mDefaults(null)
		{
		}

		//////////////////////////////////////////////////////////////////////

		bool GetOffset(char const *name, int &offset) const
		{
			for(int i = 0; i < mOffsetCount; ++i)
			{
				if(strcmp(mOffsets[i].name, name) == 0)
				{
					offset = mOffsets[i].offset;
					return true;
				}
			}
			return false;
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Update(ID3D11DeviceContext *context)
		{
			definition *d;
			DXR(Map(context, d));
			memcpy(d, (definition *)this, sizeof(definition));
			UnMap(context);
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

		HRESULT Update()
		{
			definition *d;
			DXR(Map(DX::Context, d));
			memcpy(d, (definition *)this, sizeof(definition));
			UnMap(DX::Context);
			return S_OK;
		}

		//////////////////////////////////////////////////////////////////////

	private:

		template<typename T> friend HRESULT CreateConstBuffer(char const *name, ConstBuffer<T> **constBuffer, uint32 OffsetCount, ConstBufferOffset const Offsets[], uint32 *Defaults, Shader *parent, uint bindPoint);

		HRESULT Init(char const *name, uint32 OffsetCount, ConstBufferOffset const Offsets[], uint32 *Defaults, uint bindPoint, uint ID)
		{
			mName = name;
			mOffsetCount = OffsetCount;
			mOffsets = Offsets;
			mDefaults = Defaults;
			mBindPoint = bindPoint;
			mID = (uint32)gConstBuffers.size();
			if(mDefaults != null)
			{
				memcpy(this, mDefaults, sizeof(definition)); // definition must be most-derived and *** THERE CAN BE NO VIRTUALS ***!
			}
			else
			{
				memset(this, 0, sizeof(definition));
			}
			DXR(CreateBuffer(BufferType::ConstantBufferType, 1, this, DynamicUsage, Writeable));
			return S_OK;
		}

		char const *					mName;
		uint32							mBindPoint;
		uint32							mID;				// where in the global constbuffer list this one is
		uint32							mOffsetCount;
		ConstBufferOffset const *		mOffsets;
		uint32 *			 			mDefaults;
	};

	//////////////////////////////////////////////////////////////////////

	extern vector<TypelessBuffer *> gConstBuffers;
	extern int gBindings[64];

	template<typename definition> HRESULT CreateConstBuffer(char const *name, ConstBuffer<definition> **constBuffer, uint32 OffsetCount, ConstBufferOffset const Offsets[], uint32 *Defaults, Shader *parent, uint bindPoint)
	{
		ConstBuffer<definition> *cp = null;

		// try and find one that's identical in every way
		for(auto c : gConstBuffers)
		{
			ConstBuffer<definition> *b = (ConstBuffer<definition> *)c;

			// name, offsetcount & bindpoint the same?
			if(strcmp(b->mName, name) == 0 && OffsetCount == b->mOffsetCount && bindPoint == b->mBindPoint)
			{
				// all offsets the same?
				uint i = 0;
				for(; i < OffsetCount; ++i)
				{
					if(!(Offsets[i] == b->mOffsets[i]))
					{
						break;
					}
				}
				if(i == OffsetCount)
				{
					// yes, found a match
					cp = b;
					break;
				}
			}
		}

		// found one?
		if(cp == null)
		{
			// no, make a fresh one
			cp = new ConstBuffer<definition>();

			// and add it to the global constbuffer list
			gConstBuffers.push_back(cp);

			DXR(cp->Init(name, OffsetCount, Offsets, Defaults, bindPoint, (uint32)gConstBuffers.size()));
		}

		// tack it onto the shader
		parent->AddConstBuffer(cp, bindPoint);
		assert(parent->mConstBuffers.size() <= parent->mNumConstBuffers);

		// give it back, whatever it is
		*constBuffer = cp;
		return S_OK;
	}

	void DeleteAllConstBuffers();
}