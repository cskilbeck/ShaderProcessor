//////////////////////////////////////////////////////////////////////
// Very basic Zip file loader
// Doesn't support:
// - Encryption/passwords
// - Spanned Zip files (multi disc)
// - Any compression method except Deflate or Deflate64
// - Data Descriptors after the compressed data

#include "DX.h"
#include "../zlib-1.2.8/inflate.h"
#include "../zlib-1.2.8/inftrees.h"
#include "../zlib-1.2.8/infback9.h"

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
	int Archive::File::Write(void const *buffer, uint64 size, uint64 *wrote)
	{
		return ERROR_NOT_SUPPORTED;
	}

	int Archive::File::Seek(size_t offset, int seekType, intptr *newPosition)
	{
		return ERROR_NOT_SUPPORTED;
	}

	int Archive::File::Reopen(FileBase **other)
	{
		return ERROR_NOT_SUPPORTED;
	}

	int Archive::File::GetPosition(uint64 &position)
	{
		return ERROR_NOT_SUPPORTED;
	}

	tstring Archive::File::Name()
	{
		return "";
	}

	//////////////////////////////////////////////////////////////////////

	int Archive::File::Init(FileBase *inputFile, FileHeader &f)
	{
		mFileBufferSize = 65336;

		ExtraInfo64 e;
		int r = Archive::GetExtraInfo(inputFile, e, f);					// get the extra info about sizes etc
		if(r != ok)
		{
			return r;
		}
		if(!inputFile->Reopen(&mFile))									// get a new file handle for reading data
		{
			return Archive::error_fileerror;
		}

		if(!mFile->Seek(e.LocalHeaderOffset, SEEK_SET))					// goto localfileheader
		{
			return error_fileerror;
		}
		if(!mFile->Get(mHeader))											// read localfileheader
		{
			return error_fileerror;
		}
		if(mHeader.Signature != LocalHeaderSignature)					// check signature
		{
			return error_badzipfile;
		}
		if(mHeader.Info.CompressionMethod != Deflate &&					// check compression method is supported
		   mHeader.Info.CompressionMethod != Deflate64 &&
		   mHeader.Info.CompressionMethod != None)
		{
			return error_notsupported;
		}
		if(!mFile->Seek(mHeader.Info.FilenameLength, SEEK_CUR))			// tee up the file pointer to the data (or localextrainfo)
		{
			return error_fileerror;
		}
		mCompressedSize = mHeader.Info.CompressedSize;					// snarf some details
		mUncompressedSize = mHeader.Info.UncompressedSize;

		// process any extra info that there is

		Archive::ProcessExtraInfo(mFile, mHeader.Info.ExtraFieldLength, [this] (uint16 tag, uint16 len, void *data)
		{
			if(tag == 0x0001)											// 0x0001 means Zip64 extra info
			{
				LocalExtraInfo64 ei;
				memcpy(&ei, data, len);
				if(mUncompressedSize == 0xffffffff)						// get 64bit uncompressed size if necessary
				{
					if(len < 8)
					{
						return error_badzipfile;
					}
					mUncompressedSize = ei.UnCompressedSize;
				}
				if(mCompressedSize == 0xffffffff)						// get 64bit compressed size if necessary
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
		mCompressedDataRemaining = mCompressedSize;						// init remainders
		mUncompressedDataRemaining = mUncompressedSize;
		return ok;
	}

	//////////////////////////////////////////////////////////////////////
	// the decompressor wants some data

	int Archive::File::InflateBackInput(unsigned char **buffer)
	{
		// tell it where the buffer is
		*buffer = mFileBuffer.get();

		// got any left from last time?
		if(mCachedBytesRemaining > 0)
		{
			// some compressed data was left over, just pass that back
			auto c = mCachedBytesRemaining;
			*buffer = mFileBuffer.get() + mFileBufferSize - c;
			mCachedBytesRemaining = 0;

			// _could_ memmove the remainder back to the beginning of the buffer and
			// fill the rest up from the file, but can't be arsed and it would only
			// help a tiny bit - during large decompression operations it would make
			// almost no difference
			return (int)c;
		}

		// fill input buffer from file
		uint64 got;
		uint64 get = (uint32)min(mCompressedDataRemaining, mFileBufferSize);
		if(mFile->Read(mFileBuffer.get(), get, &got) != S_OK)
		{
			return 0;
		}
		mCompressedDataRemaining -= got;
		return (int)got;
	}

	//////////////////////////////////////////////////////////////////////
	// the decompressor has delivered some uncompressed data

	int Archive::File::InflateBackOutput(unsigned char *data, uint32 length)
	{
		uint32 c = (uint32)min(length, mBytesRequired);
		memcpy(mCurrentOutputPointer, data, c);
		mCurrentDataPointer = data + c;		// in case we need to come back for more in a separate DoInflateBack call
		mBytesRequired -= c;
		mTotalGot += c;
		mCurrentOutputPointer += c;
		mCachedBytesRemaining = mBufferSize - c;
		return mBytesRequired == 0; // true means no more output please, inflateBack will return Z_BUF_ERROR, but that's ok
	}

	//////////////////////////////////////////////////////////////////////

	Archive::File::File()
		: mZStream({ 0 })
		, mFile(null)
		, mOutputBuffer(null)
		, mCurrentOutputPointer(null)
		, mCurrentDataPointer(null)
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
		switch(mHeader.Info.CompressionMethod)
		{
			case Deflate64:
				inflateBack9End(&mZStream);
				break;
			case Deflate:
				inflateBackEnd(&mZStream);
				break;
		}
		mFileBuffer.reset();
		mInflateBackState.reset();
	}

	//////////////////////////////////////////////////////////////////////

	int Archive::File::GetSize(uint64 &size)
	{
		size = mUncompressedSize;
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////
	// read some data from compressed file

	int Archive::File::Read(void *buffer, uint64 bytesToRead, uint64 *got)
	{
		if(mFileBuffer == null)
		{
			mFileBuffer.reset(new byte[mFileBufferSize]);
			mInflateBackState.reset(new inflateBackState());
			memset(mInflateBackState.get(), 0, sizeof(inflateBackState));
			mCachedBytesRemaining = 0;
			mBufferSize = 1 << MAX_WBITS;
			mFileBuffer.reset(new byte[mFileBufferSize]);
			memset(&mZStream, 0, sizeof(mZStream));
			mWindow.reset(new byte[65536]);

			int r = error_notsupported;
			if(mHeader.Info.CompressionMethod == Deflate)
			{
				r = inflateBackInit(&mZStream, MAX_WBITS, mWindow.get());
			}
			else if(mHeader.Info.CompressionMethod == Deflate64)
			{
				r = inflateBack9Init(&mZStream, mWindow.get());
			}
			else if(mHeader.Info.CompressionMethod == None)
			{
				r = ok;
			}
			if(r != ok)
			{
				return r;
			}
		}

		mCurrentOutputPointer = (byte *)buffer;
		mBytesRequired = min(mUncompressedDataRemaining, bytesToRead);
		mTotalGot = 0;

		// is there any left over in the buffer?
		if(mCachedBytesRemaining > 0)
		{
			// yes, copy as much as possible and decrement bytesRequired
			uint32 c = (uint32)min(mCachedBytesRemaining, mBytesRequired);
			memcpy(mCurrentOutputPointer, mCurrentDataPointer, c);
			mCachedBytesRemaining -= c;
			mCurrentOutputPointer += c;
			mCurrentDataPointer += c;
			mBytesRequired -= c;
			mTotalGot += c;
		}

		int r = ok;
		if(mBytesRequired > 0)
		{
			// get the rest (callbacks might be called)
			if(mHeader.Info.CompressionMethod == Deflate)
			{
				r = inflateBack(&mZStream, &Archive::InflateBackInputCallback, this, &Archive::InflateBackOutputCallback, this, mInflateBackState.get());
			}
			else
			{
				r = inflateBack9(&mZStream, &Archive::InflateBackInputCallback, this, &Archive::InflateBackOutputCallback, this, mInflateBackState.get());
			}
		}

		if(got != null)
		{
			*got = mTotalGot;
		}
		mUncompressedDataRemaining -= mTotalGot;

		// there may be data left to be uncompressed, that's ok
		// there may be uncompressed data left in the output buffer, that's ok too (we'll flush it next time)
		switch(r)
		{
			case ok:
			case Z_STREAM_END:		// all uncompressed data was read (but not necessarily streamed out)
			case Z_BUF_ERROR:		// we got enough data to satisfy this read
				return S_OK;

			default:				// anything else is an error (inflateBack never returns Z_OK)
				return r;
		}
	}

	//////////////////////////////////////////////////////////////////////

	int Archive::InflateBackOutputCallback(void *context, unsigned char *data, unsigned length)
	{
		return ((File *)context)->InflateBackOutput(data, length);
	}

	//////////////////////////////////////////////////////////////////////

	uint Archive::InflateBackInputCallback(void *context, unsigned char **buffer)
	{
		return ((File *)context)->InflateBackInput(buffer);
	}

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
		uint64 file_size;
		DXR(mFile->GetSize(file_size));
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

			uint64 got;
			if(mFile->Read(buf.get(), read_size, &got) != S_OK || got != read_size)
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
	// Scan ExtraInfo blocks following a FileHeader or LocalFileHeader

	int Archive::ProcessExtraInfo(FileBase *file, size_t extraSize, std::function<int(uint16, uint16, void *)> callback)
	{
		Ptr<byte> buffer(new byte[65536]);								// Harumph, max size...
		uint64 currentPosition;
		DXR(file->GetPosition(currentPosition));
		while(extraSize > 0)
		{
			uint16 tag;
			uint16 len;
			if(!(file->Get(tag) && file->Get(len)))						// get tag and len of extrainfo block
			{
				return error_fileerror;
			}
			uint64 got;
			if(!file->Read(buffer.get(), len, &got) || got != len)		// read the extrainfo block itself
			{
				return error_fileerror;
			}
			int e = callback(tag, len, buffer.get());					// send it to the callback
			if(e != ok)
			{
				return e;
			}
			len += 4;
			currentPosition += len;
			if(!file->Seek(currentPosition, SEEK_SET))					// go to the next one
			{
				return error_fileerror;
			}
			extraSize -= len;
		}
		return ok;
	}

	//////////////////////////////////////////////////////////////////////

	int Archive::GetExtraInfo(FileBase *file, ExtraInfo64 &extra, FileHeader &f)
	{
		extra.CompressedSize = MAXUINT64;
		extra.UnCompressedSize = MAXUINT64;
		extra.LocalHeaderOffset = MAXUINT64;

		ProcessExtraInfo(file, f.Info.ExtraFieldLength, [&] (uint16 tag, uint16 len, void *data)
		{
			if(tag == 0x0001)
			{
				memcpy(&extra, data, len);
			}
			return ok;
		});

		if(f.Info.UncompressedSize != 0xffffffff)
		{
			extra.UnCompressedSize = f.Info.UncompressedSize;
		}
		if(f.Info.CompressedSize != 0xffffffff)
		{
			extra.CompressedSize = f.Info.CompressedSize;
		}
		if(f.LocalHeaderOffset != 0xffffffff)
		{
			extra.LocalHeaderOffset = f.LocalHeaderOffset;
		}

		if(extra.CompressedSize == MAXUINT64 ||
		   extra.UnCompressedSize == MAXUINT64 ||
		   extra.LocalHeaderOffset == MAXUINT64)
		{
			return error_badzipfile;
		}
		return ok;
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
			uint64 got;
			char filename[256];
			if(mFile->Read(filename, f.Info.FilenameLength, &got) != S_OK || got != f.Info.FilenameLength)
			{
				return error_fileerror;
			}
			if(_strnicmp(filename, name, f.Info.FilenameLength) == 0)
			{
				return file.Init(mFile, f);
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

	int Archive::Goto(uint32 index, File &file)
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
				return file.Init(mFile, f);
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