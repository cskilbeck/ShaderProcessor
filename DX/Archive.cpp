//////////////////////////////////////////////////////////////////////
// Very basic Zip file loader
// Doesn't support:
// - Encryption/passwords
// - Spanned Zip files (multi disc)
// - Any compression method except Deflate
// - Data Descriptors after the compressed data
// - CRC checking
// Does support:
// - Zip64 format (version 2 only)
// 

#include "DX.h"

//////////////////////////////////////////////////////////////////////

namespace
{
	enum
	{
		LocalHeaderSignature = 0x04034b50,
		CentralHeaderSignature = 0x02014b50,
		EndHeaderSignature = 0x06054b50,
		EndHeader64Signature = 0x06064b50,
		EndLocalHeader64Signature = 0x07064b50
	};
}

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	Archive::Archive()
		: mFile(null)
		, mIsZip64(false)
		, mCentralDirectoryLocation(0)
		, mEntriesInCentralDirectory(0)
		, mSizeOfCentralDirectory(0)
	{
	}

	//////////////////////////////////////////////////////////////////////

	Archive::~Archive()
	{
	}

	//////////////////////////////////////////////////////////////////////
	// Get the Central Directory

	int Archive::GetCDLocation(uint64 &offset)
	{
		uint64 file_size = mFile->Size();
		uint64 back_read = 4;
		uint64 max_back = 0xffff; /* maximum size of global comment */

		int const CommentBufferSize = 0x400;

		Ptr<byte> buf(new byte[CommentBufferSize + 4]);

		if(max_back > file_size)
		{
			max_back = file_size;
		}

		while(back_read < max_back)
		{
			if(back_read + CommentBufferSize > max_back)
			{
				back_read = max_back;
			}
			else
			{
				back_read += CommentBufferSize;
			}

			uint64 read_pos = file_size - back_read;
			int32 read_size = min(CommentBufferSize + 4, (int32)(file_size - read_pos));

			if(!mFile->Seek(read_pos, SEEK_SET))
			{
				return error_fileerror;
			}

			uint32 got;
			if(!mFile->Read(buf.get(), read_size, &got) || got != read_size)
			{
				return error_fileerror;
			}

			byte *p = buf.get();
			for(int32 i = read_size - 4; i > 0; --i)
			{
				byte *q = p + i;
				uint32 c = q[0] | (q[1] << 8) | (q[2] << 16) | (q[3] << 24);
				if(c == EndHeaderSignature)
				{
					offset = read_pos + i;
					return ok;
				}
			}
		}
		return error_badzipfile;
	}

	//////////////////////////////////////////////////////////////////////
	// Get the 64 bit Central Directory

	int Archive::GetCD64Location(uint64 &offset, uint64 const oldCDOffset)
	{
		if(!mFile->Seek(oldCDOffset - sizeof(EndOfCentralDirectory64Locator), SEEK_SET))
		{
			return error_fileerror;
		}

		EndOfCentralDirectory64Locator eocd;
		if(!mFile->Get(eocd))
		{
			return error_fileerror;
		}
		if(eocd.Signature != EndLocalHeader64Signature)
		{
			return error_badzipfile;
		}
		if(!mFile->Seek(eocd.CDOffset, SEEK_SET))
		{
			return error_fileerror;
		}

		uint32 signature;
		if(!mFile->GetUInt32(signature))
		{
			return error_fileerror;
		}
		if(signature != EndHeader64Signature)
		{
			return error_badzipfile;
		}
		offset = eocd.CDOffset;
		return ok;
	}

	//////////////////////////////////////////////////////////////////////
	// Open an Archive

	int Archive::Open(FileBase *zipFile)
	{
		uint64 CDLocation;
		int err = ok;

		mFile = zipFile;
		mIsZip64 = false;

		int e = GetCDLocation(CDLocation);
		if(e != ok)
		{
			return e;
		}
		if(!mFile->Seek(CDLocation, SEEK_SET))
		{
			return error_fileerror;
		}

		EndOfCentralDirectory eocd;
		if(!mFile->Get(eocd))
		{
			return error_fileerror;
		}
		if(eocd.EntriesInCD != eocd.EntriesInCDOnThisDisk)	// Spanned zips not supported
		{
			return error_notsupported;
		}

		mEntriesInCentralDirectory = eocd.EntriesInCD;
		mSizeOfCentralDirectory = eocd.SizeOfCD;
		mCentralDirectoryLocation = eocd.CDOffset;

		if(eocd.EntriesInCD == 0xffff || eocd.SizeOfCD == 0xffff || eocd.CDOffset == 0xffffffff)
		{
			e = GetCD64Location(CDLocation, CDLocation);
			if(e != ok)
			{
				return e;
			}
			if(!mFile->Seek(CDLocation, SEEK_SET))
			{
				return error_fileerror;
			}

			EndOfCentralDirectory64 eocd64;
			if(!mFile->Get(eocd64))
			{
				return error_fileerror;
			}
			if(eocd64.EntriesInCD != eocd64.EntriesInCDOnThisDisk)
			{
				return error_notsupported;
			}
			mIsZip64 = true;
			mEntriesInCentralDirectory = eocd64.EntriesInCD;
			mSizeOfCentralDirectory = eocd64.SizeOfCD;
			mCentralDirectoryLocation = eocd64.CDOffset;
		}
		return ok;
	}

	//////////////////////////////////////////////////////////////////////

	Archive::File::File()
		: mFile(null)
		, mZLibInitialized(false)
	{
	}

	//////////////////////////////////////////////////////////////////////

	Archive::File::~File()
	{
		Close();
	}

	//////////////////////////////////////////////////////////////////////

	void Archive::File::Close()
	{
		Delete(mFile);
		if(mZLibInitialized)
		{
			inflateEnd(&mZStream);
		}
	}

	//////////////////////////////////////////////////////////////////////
	// Get an Archive::File ready for reading

	int Archive::File::Init(FileBase *file, ExtraInfo64 &e)
	{
		// reopen handle for reading the actual data
		if(!file->Reopen(&mFile))
		{
			return error_fileerror;
		}
		// read the localfileheader
		if(!mFile->Seek(e.LocalHeaderOffset, SEEK_SET))
		{
			return error_fileerror;
		}
		if(!mFile->Get(mHeader))
		{
			return error_fileerror;
		}
		if(mHeader.Signature != LocalHeaderSignature)
		{
			return error_badzipfile;
		}
		if(mHeader.Info.CompressionMethod != Deflate && mHeader.Info.CompressionMethod != None)
		{
			return error_notsupported;
		}
		// tee up the file pointer
		if(!mFile->Seek(mHeader.Info.FilenameLength, SEEK_CUR))
		{
			return error_fileerror;
		}

		mCompressedSize = mHeader.Info.CompressedSize;
		mUncompressedSize = mHeader.Info.UncompressedSize;

		Archive::ProcessExtraInfo(mFile, mHeader.Info.ExtraFieldLength, [this] (uint16 tag, uint16 len, void *data)
		{
			if(tag == 0x0001)
			{
				LocalExtraInfo64 ei;
				memcpy(&ei, data, len);
				if(mUncompressedSize == 0xffffffff)
				{
					if(len < 8)
					{
						return error_badzipfile;
					}
					mUncompressedSize = ei.UnCompressedSize;
				}
				if(mCompressedSize == 0xffffffff)
				{
					if(len < 16)
					{
						return error_badzipfile;
					}
					mCompressedSize = ei.CompressedSize;
				}
			}
			return ok;
		});
		mCompressedDataRemaining = mCompressedSize;
		mUncompressedDataRemaining = mUncompressedSize;
		return ok;
	}

	//////////////////////////////////////////////////////////////////////
	// Scan ExtraInfo blocks following a FileHeader or LocalFileHeader

	int Archive::ProcessExtraInfo(FileBase *file, size_t extraSize, std::function<int(uint16, uint16, void *)> callback)
	{
		Ptr<byte> buffer(new byte[65536]);			// Harumph, max size...
		uint64 currentPosition = file->Position();
		while(extraSize > 0)
		{
			uint16 tag;
			uint16 len;
			if(!(file->Get(tag) && file->Get(len)))
			{
				return error_fileerror;
			}
			uint32 got;
			if(!file->Read(buffer.get(), len, &got) || got != len)
			{
				return error_fileerror;
			}
			int e = callback(tag, len, buffer.get());
			if(e != ok)
			{
				return e;
			}
			len += 4;
			currentPosition += len;
			if(!file->Seek(currentPosition, SEEK_SET))
			{
				return error_fileerror;
			}
			extraSize -= len;
		}
		return ok;
	}

	//////////////////////////////////////////////////////////////////////
	// Read data from an Archive::File

	int Archive::File::Read2(byte *buffer, size_t amount, size_t *got /* = null */, uint32 bufferSize /* = 65536 */)
	{
		// use inflateBack functions (or inflateBack9 to support Deflate64)
	}

	//////////////////////////////////////////////////////////////////////
	// Read data from an Archive::File

	int Archive::File::Read(byte *buffer, size_t amount, size_t *got /* = null */, uint32 bufferSize)
	{
		uint32 FileBufferSize = bufferSize;

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
				uint32 remainder = (uint32)min(MAXUINT32, min(amount, mCompressedDataRemaining));
				if(!mFile->Read(currentPtr, remainder, &localGot) || localGot != remainder)
				{
					return error_fileerror;
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
					uint32 remainder = (uint32)min(FileBufferSize, mCompressedDataRemaining);
					uint32 localGot;
					if(!mFile->Read(mFileBuffer.get(), (uint32)remainder, &localGot) || localGot != remainder)
					{
						return error_fileerror;
					}
					mZStream.next_in = mFileBuffer.get();
					mZStream.avail_in = localGot;
					mZStream.next_out = currentPtr;
					mZStream.avail_out = (uint)(amount - len);
					mCompressedDataRemaining -= localGot;
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
				mUncompressedDataRemaining -= d;
			}
		}
		if(got != null)
		{
			*got = totalGot;
		}
		return ok;
	}

	//////////////////////////////////////////////////////////////////////
	// Prepare an Archive::File for reading

	int Archive::InitFile(File &file, FileHeader &f)
	{
		ExtraInfo64 Extra, *pExtra = null;

		Extra.CompressedSize = MAXUINT64;
		Extra.UnCompressedSize = MAXUINT64;
		Extra.LocalHeaderOffset = MAXUINT64;

		ProcessExtraInfo(mFile, f.Info.ExtraFieldLength, [&] (uint16 tag, uint16 len, void *data)
		{
			if(tag == 0x0001)
			{
				memcpy(&Extra, data, len);
			}
			return ok;
		});

		if(f.Info.UncompressedSize != 0xffffffff)
		{
			Extra.UnCompressedSize = f.Info.UncompressedSize;
		}
		if(f.Info.CompressedSize != 0xffffffff)
		{
			Extra.CompressedSize = f.Info.CompressedSize;
		}
		if(f.LocalHeaderOffset != 0xffffffff)
		{
			Extra.LocalHeaderOffset = f.LocalHeaderOffset;
		}

		if(Extra.CompressedSize == MAXUINT64 ||
		   Extra.UnCompressedSize == MAXUINT64 ||
		   Extra.LocalHeaderOffset == MAXUINT64)
		{
			return error_badzipfile;
		}

		return file.Init(mFile, Extra);
	}

	//////////////////////////////////////////////////////////////////////
	// Find a file by name in an Archive

	int Archive::Locate(char const *name, Archive::File &file)
	{
		if(!mFile->Seek(mCentralDirectoryLocation, SEEK_SET))
		{
			return error_fileerror;
		}
		for(uint i = 0; i < mEntriesInCentralDirectory; ++i)
		{
			FileHeader f;
			if(!mFile->Get(f) || f.Signature != CentralHeaderSignature)
			{
				return error_fileerror;
			}
			uint32 got;
			char filename[256];
			if(!mFile->Read(filename, f.Info.FilenameLength, &got) || got != f.Info.FilenameLength)
			{
				return error_fileerror;
			}
			if(_strnicmp(filename, name, f.Info.FilenameLength) == 0)
			{
				return InitFile(file, f);
			}
			if(!mFile->Seek(f.FileCommentLength, SEEK_CUR))
			{
				return error_fileerror;
			}
		}
		return error_filenotfound;
	}

	//////////////////////////////////////////////////////////////////////
	// Goto a file by index

	int Archive::Goto(uint32 index, Archive::File &file)
	{
		if(index >= mEntriesInCentralDirectory)
		{
			return error_filenotfound;
		}
		if(!mFile->Seek(mCentralDirectoryLocation, SEEK_SET))
		{
			return error_fileerror;
		}
		for(uint i = 0; i < mEntriesInCentralDirectory; ++i)
		{
			FileHeader f;
			if(!mFile->Get(f))
			{
				return error_fileerror;
			}
			if(f.Signature != CentralHeaderSignature)
			{
				return error_badzipfile;
			}
			if(!mFile->Seek(f.Info.FilenameLength, SEEK_CUR))
			{
				return error_fileerror;
			}
			if(i == index)
			{
				return InitFile(file, f);
			}
			if(!mFile->Seek(f.FileCommentLength, SEEK_CUR))
			{
				return error_fileerror;
			}
		}
		return error_filenotfound;
	}

	void Archive::Close()
	{
	}
}