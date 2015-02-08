//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{

	struct File
	{
		bool Open(tchar const *filename);
		void Close();
		bool IsOpen() const;
		int64 Size();
		uint32 Read(uint32 bytes, void *buffer);
		uint32 Write(uint32 bytes, void *buffer);

		tstring name;
		Handle h;
		long error;
	};

	bool LoadFile(tchar const *filename, void **data, uint32 *size = null);

}