//////////////////////////////////////////////////////////////////////

#include "DX.h"

//////////////////////////////////////////////////////////////////////

namespace
{
	const char unz_copyright[] = " unzip 1.01 Copyright 1998-2004 Gilles Vollant - http://www.winimage.com/zLibDll";
}

//////////////////////////////////////////////////////////////////////

#define DISKHEADERMAGIC          (0x08074b50)
#define LOCALHEADERMAGIC         (0x04034b50)
#define CENTRALHEADERMAGIC       (0x02014b50)
#define ENDHEADERMAGIC           (0x06054b50)
#define ZIP64ENDHEADERMAGIC      (0x06064b50)
#define ZIP64ENDLOCHEADERMAGIC   (0x07064b50)

#define SIZECENTRALDIRITEM       (0x2e)
#define SIZECENTRALHEADERLOCATOR (0x14) /* 20 */
#define SIZEZIPLOCALHEADER       (0x1e)

#ifndef BUFREADCOMMENT
#  define BUFREADCOMMENT (0x400)
#endif

#ifndef UNZ_BUFSIZE
#  define UNZ_BUFSIZE (64 * 1024)
#endif
#ifndef UNZ_MAXFILENAMEINZIP
#  define UNZ_MAXFILENAMEINZIP (256)
#endif

#ifndef ALLOC
#  define ALLOC(size) (malloc(size))
#endif
#ifndef TRYFREE
#  define TRYFREE(p) {if (p) free(p);}
#endif

namespace DX
{
	Archive::Archive()
		: mFile(null)
		, mIsZip64(false)
		, mCentralDirectoryLocation(0)
		, mEntriesInCentralDirectory(0)
		, mSizeOfCentralDirectory(0)
	{
	}

	Archive::~Archive()
	{
	}

	int Archive::GetCD(uint64 &offset)
	{
		uint64 file_size = mFile->Size();
		uint64 back_read = 4;
		uint64 max_back = 0xffff; /* maximum size of global comment */

		int const bufreadcomment = 0x400;

		Ptr<byte> buf(new byte[bufreadcomment + 4]);

		if(max_back > file_size)
		{
			max_back = file_size;
		}

		while(back_read < max_back)
		{
			if(back_read + bufreadcomment > max_back)
			{
				back_read = max_back;
			}
			else
			{
				back_read += bufreadcomment;
			}

			uint64 read_pos = file_size - back_read;
			int32 read_size = min(bufreadcomment + 4, (int32)(file_size - read_pos));

			if(!mFile->Seek(read_pos, SEEK_SET))
			{
				return fileerror;
			}

			uint32 got;
			if(!mFile->Read(buf.get(), read_size, &got) || got != read_size)
			{
				return fileerror;
			}

			byte *p = buf.get();
			for(int32 i = read_size - 4; i > 0; --i)
			{
				byte *q = p + i;
				uint32 c = q[0] | (q[1] << 8) | (q[2] << 16) | (q[3] << 24);
				if(c == ENDHEADERMAGIC)
				{
					offset = read_pos + i;
					return ok;
				}
			}
		}
		return badzipfile;
	}

	int Archive::GetCD64(uint64 &offset, uint64 const endcentraloffset)
	{
		if(!mFile->Seek(endcentraloffset - sizeof(EndOfCentralDirectory64Locator), SEEK_SET))
		{
			return fileerror;
		}

		EndOfCentralDirectory64Locator eocd;
		if(!mFile->Get(eocd))
		{
			return fileerror;
		}
		if(eocd.Signature != ZIP64ENDLOCHEADERMAGIC)
		{
			return badzipfile;
		}

		if(!mFile->Seek(eocd.CDOffset, SEEK_SET))
		{
			return fileerror;
		}

		uint32 uL;
		if(!mFile->GetUInt32(uL))
		{
			return fileerror;
		}
		if(uL != ZIP64ENDHEADERMAGIC)
		{
			return badzipfile;
		}
		offset = eocd.CDOffset;
		return ok;
	}

	int Archive::Open(FileBase *zipFile)
	{
		mFile = zipFile;

		uint64 central_pos;

		int err = ok;

		if(unz_copyright[0] != ' ')
		{
			return internalerror;
		}

		mIsZip64 = false;

		int e = GetCD(central_pos);
		if(e != ok)
		{
			return e;
		}

		if(!mFile->Seek(central_pos, SEEK_SET))
		{
			return fileerror;
		}

		EndOfCentralDirectory eocd;
		if(!mFile->Get(eocd))
		{
			return fileerror;
		}

		if(eocd.EntriesInCD != eocd.EntriesInCDOnThisDisk)	// HUH?
		{
			return badzipfile;
		}

		mEntriesInCentralDirectory = eocd.EntriesInCD;
		mSizeOfCentralDirectory = eocd.SizeOfCD;
		mCentralDirectoryLocation = eocd.CDOffset;

		if(eocd.EntriesInCD == 0xffff || eocd.SizeOfCD == 0xffff || eocd.CDOffset == 0xffffffff)
		{
			e = GetCD64(central_pos, central_pos);
			if(e != ok)
			{
				return e;
			}

			if(!mFile->Seek(central_pos, SEEK_SET))
			{
				return fileerror;
			}

			EndOfCentralDirectory64 eocd64;
			if(!mFile->Get(eocd64))
			{
				return fileerror;
			}

			if(eocd64.EntriesInCD != eocd64.EntriesInCDOnThisDisk)
			{
				return badzipfile;
			}
			mIsZip64 = true;
			mEntriesInCentralDirectory = eocd64.EntriesInCD;
			mSizeOfCentralDirectory = eocd64.SizeOfCD;
			mCentralDirectoryLocation = central_pos;
		}

		//if(central_pos < us.offset_central_dir + us.size_central_dir)
		//{
		//	return badzipfile;
		//}

		return ok;
	}

	Archive::File::File()
		: mFile(null)
		, mZLibInitialized(false)
	{
	}

	Archive::File::~File()
	{
		Close();
	}

	void Archive::File::Close()
	{
		Delete(mFile);
		if(mZLibInitialized)
		{
			inflateEnd(&mZStream);
		}
	}

	int Archive::File::Init(FileBase *file, FileHeader &f)
	{
		// reopen handle for reading the actual data
		if(!file->Reopen(&mFile))
		{
			return fileerror;
		}
		// read the localfileheader
		if(!mFile->Seek(f.LocalHeaderOffset, SEEK_SET))
		{
			return fileerror;
		}
		if(!mFile->Get(mHeader))
		{
			return fileerror;
		}
		if(mHeader.Signature != LOCALHEADERMAGIC)
		{
			return badzipfile;
		}
		if(mHeader.Info.CompressionMethod != Deflate && mHeader.Info.CompressionMethod != None)
		{
			return notsupported;
		}
		// tee up the file pointer
		if(!mFile->Seek(mHeader.Info.FilenameLength + mHeader.Info.ExtraFieldLength, SEEK_CUR))
		{
			return fileerror;
		}
		mCompressedDataRemaining = mHeader.Info.CompressedSize;
		mUncompressedDataRemaining = mHeader.Info.UncompressedSize;
		return ok;
	}

	int Archive::File::Read(byte *buffer, size_t amount, size_t *got /* = null */)
	{
		size_t totalGot = 0;
		// setup if necessary
		if(GetCompressionMethod() == Deflate && mFileBuffer == null)
		{
			mFileBuffer.reset(new byte[FileBufferSize]);

			memset(&mZStream, 0, sizeof(mZStream));
			mZLibInitialized = true;
			int error = inflateInit2(&mZStream, -MAX_WBITS);
			if(error != Z_OK)
			{
				return error;
			}
		}

		// decompress or just copy some data
		size_t len = 0;
		byte *currentPtr = buffer;
		while(len < amount && mCompressedDataRemaining > 0)
		{
			if(GetCompressionMethod() == None)
			{
				uint32 localGot;
				uint32 remainder = (uint32)min(MAXUINT32, min(amount, mUncompressedDataRemaining));
				if(!mFile->Read(currentPtr, remainder, &localGot) || localGot != remainder)
				{
					return fileerror;
				}
				currentPtr += (size_t)localGot;
				len += (size_t)localGot;
				totalGot += localGot;
				mUncompressedDataRemaining -= localGot;
				mCompressedDataRemaining -= localGot;
			}
			else
			{
				// exhausted the buffer?
				if(mZStream.avail_in == 0)
				{
					// yes, refill it
					uint32 remainder = (uint32)min(FileBufferSize, mUncompressedDataRemaining);
					uint32 localGot;
					if(!mFile->Read(mFileBuffer.get(), (uint32)remainder, &localGot) || localGot != remainder)
					{
						return fileerror;
					}
					mZStream.next_in = mFileBuffer.get();
					mZStream.avail_in = localGot;
					mZStream.next_out = currentPtr;
					mZStream.avail_out = amount - len;
					mUncompressedDataRemaining -= localGot;
				}

				// decompress some
				uint64 d = mZStream.total_out;
				int e = inflate(&mZStream, Z_SYNC_FLUSH);
				if(e < 0)
				{
					return e;
				}

				d = mZStream.total_out - d;
				totalGot += d;
				currentPtr += d;
				len += d;
				mCompressedDataRemaining -= d;
			}
		}
		if(got != null)
		{
			*got = totalGot;
		}
		return ok;
	}

	int Archive::Locate(char const *name, Archive::File &file)
	{
		if(!mFile->Seek(mCentralDirectoryLocation, SEEK_SET))
		{
			return fileerror;
		}

		char filename[1024];

		for(uint i = 0; i < mEntriesInCentralDirectory; ++i)
		{
			if(mIsZip64)
			{
			}
			else
			{
				FileHeader f;
				if(!mFile->Get(f) || f.Signature != CENTRALHEADERMAGIC)
				{
					return fileerror;
				}
				uint32 got;
				if(!mFile->Read(filename, f.Info.FilenameLength, &got))
				{
					return fileerror;
				}
				if(got != f.Info.FilenameLength)
				{
					return badzipfile;
				}
				if(_strnicmp(filename, name, f.Info.FilenameLength) == 0)
				{
					return file.Init(mFile, f);
				}
				if(!mFile->Seek(f.FileCommentLength, SEEK_CUR))
				{
					return fileerror;
				}
			}
		}
		return filenotfound;
	}

	int Archive::Goto(uint32 index, Archive::File &file)
	{
		if(index >= mEntriesInCentralDirectory)
		{
			return filenotfound;
		}
		if(!mFile->Seek(mCentralDirectoryLocation, SEEK_SET))
		{
			return fileerror;
		}
		for(uint i = 0; i < mEntriesInCentralDirectory; ++i)
		{
			FileHeader f;
			if(!mFile->Get(f))
			{
				return fileerror;
			}
			if(f.Signature != CENTRALHEADERMAGIC)
			{
				return badzipfile;
			}
			if(i == index)
			{
				return file.Init(mFile, f);
			}
			if(!mFile->Seek(f.Info.FilenameLength + f.FileCommentLength, SEEK_CUR))
			{
				return fileerror;
			}
		}
		return filenotfound;
	}

	void Archive::Close()
	{
	}
}