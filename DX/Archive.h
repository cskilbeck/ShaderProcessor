//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	struct Archive
	{
		struct FileInfo
		{
			size_t	size;		// uncompressed size in bytes
			uint32	index;		// file index within the archive
		};

		enum Error
		{
			ok = 0,
			end_of_list_of_file = -100,
			errnum = Z_ERRNO,
			eof = 0,
			paramerror = -102,
			badzipfile = -103,
			internalerror = -104,
			crcerror = -105
		};

		Archive();
		~Archive();
		bool Open(tchar const *name);
		void Close();
		bool Locate(tchar const *name, FileInfo &info);
		bool Read(byte *buffer, size_t amount, size_t *got = null);
		tstring GetGlobalComment();

		static byte *Test(tchar const *zipFile, tchar const *filename, size_t &size)
		{
			Archive a;
			if(a.Open(zipFile))
			{
				Archive::FileInfo info;
				if(a.Locate(filename, info))
				{
					Ptr<byte> b(new byte[info.size]);
					if(a.Read(b.get(), info.size))
					{
						size = info.size;
						return b.release();
					}
				}
			}
			return null;
		}
	
	private:

		uint64 GetCD(DiskFile &file);
		uint64 GetCD64(DiskFile &file, ZPOS64_T const endcentraloffset);

		void *				mInfo;		// unz_global_info64
		voidp				mFile;		// unzFile
		Error				mLastError;
	};
}
