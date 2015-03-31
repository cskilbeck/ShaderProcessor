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
#include "../zlib-1.2.8/inflate.h"
#include "../zlib-1.2.8/inftrees.h"

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

	int Archive::Assistant::Init(FileBase *inputFile, FileHeader &f)
	{
		fileBufferSize = 1024;

		ExtraInfo64 e;
		int r = Archive::GetExtraInfo(inputFile, e, f);					// get the extra info about sizes etc
		if(r != ok)
		{
			return r;
		}
		if(!inputFile->Reopen(&file))									// get a new file handle for reading data
		{
			return Archive::error_fileerror;
		}

		if(!file->Seek(e.LocalHeaderOffset, SEEK_SET))					// goto localfileheader
		{
			return error_fileerror;
		}
		if(!file->Get(mHeader))											// read localfileheader
		{
			return error_fileerror;
		}
		if(mHeader.Signature != LocalHeaderSignature)					// check signature
		{
			return error_badzipfile;
		}
		if(mHeader.Info.CompressionMethod != Deflate &&					// check compression method is supported
		   //mHeader.Info.CompressionMethod != Deflate64 &&
		   mHeader.Info.CompressionMethod != None)
		{
			return error_notsupported;
		}
		if(!file->Seek(mHeader.Info.FilenameLength, SEEK_CUR))			// tee up the file pointer to the data (or localextrainfo)
		{
			return error_fileerror;
		}
		mCompressedSize = mHeader.Info.CompressedSize;					// snarf some details
		mUncompressedSize = mHeader.Info.UncompressedSize;

		// process any extra info that there is

		Archive::ProcessExtraInfo(file, mHeader.Info.ExtraFieldLength, [this] (uint16 tag, uint16 len, void *data)
		{
			if(tag == 0x0001)
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

	int Archive::Assistant::InflateBackInput(unsigned char **buffer)
	{
		// tell it where the buffer is
		*buffer = fileBuffer.get();

		// got any left from last time?
		if(cachedBytesRemaining > 0)
		{
			// some compressed data was left over, just pass that back
			auto c = cachedBytesRemaining;
			*buffer = fileBuffer.get() + fileBufferSize - c;
			cachedBytesRemaining = 0;

			// _could_ memmove the remainder back to the beginning of the buffer and
			// fill the rest up from the file, but can't be arsed and it would only
			// help a tiny bit - during large decompression operations it would make
			// almost no difference
			return (int)c;
		}

		// fill input buffer from file
		uint32 got;
		if(!file->Read(fileBuffer.get(), fileBufferSize, &got))
		{
			return 0;
		}
		return (int)got;
	}

	//////////////////////////////////////////////////////////////////////
	// the decompressor has delivered some uncompressed data

	int Archive::Assistant::InflateBackOutput(unsigned char *data, uint32 length)
	{
		uint32 c = (uint32)min(length, bytesRequired);
		memcpy(currentOutputPointer, data, c);
		currentDataPointer = data + c;		// in case we need to come back for more in a separate DoInflateBack call
		bytesRequired -= c;
		totalGot += c;
		currentOutputPointer += c;
		cachedBytesRemaining = bufferSize - c;
		return bytesRequired == 0; // true means no more output please, inflateBack will return Z_BUF_ERROR, but that's ok
	}

	//////////////////////////////////////////////////////////////////////
	// read some data from compressed file

	int Archive::Assistant::Read(byte *buffer, uint64 bytesToRead, uint64 *got)
	{
		if(fileBuffer == null)
		{
			fileBuffer.reset(new byte[fileBufferSize]);
			IBState.reset(new inflateBackState());
			memset(IBState.get(), 0, sizeof(inflateBackState));
			cachedBytesRemaining = 0;
			bufferSize = 1 << MAX_WBITS;
			fileBuffer.reset(new byte[fileBufferSize]);
			memset(&zstream, 0, sizeof(zstream));
			mWindow.reset(new byte[65536]);
			int r = inflateBackInit(&zstream, MAX_WBITS, mWindow.get());
			if(r != ok)
			{
				return r;
			}
		}

		currentOutputPointer = buffer;
		bytesRequired = min(mUncompressedDataRemaining, bytesToRead);
		totalGot = 0;

		// is there any left over in the buffer?
		if(cachedBytesRemaining > 0)
		{
			// yes, copy as much as possible and decrement bytesRequired
			uint32 c = (uint32)min(cachedBytesRemaining, bytesRequired);
			memcpy(currentOutputPointer, currentDataPointer, c);
			cachedBytesRemaining -= c;
			currentOutputPointer += c;
			currentDataPointer += c;
			bytesRequired -= c;
			totalGot += c;
		}

		int r = ok;
		if(bytesRequired > 0)
		{
			// get the rest (callbacks might be called)
			r = inflateBack(&zstream, &Archive::InflateBackInputCallback, this, &Archive::InflateBackOutputCallback, this, IBState.get());
		}

		if(got != null)
		{
			*got = totalGot;
		}
		mUncompressedDataRemaining -= totalGot;

		// there may be data left to be uncompressed, that's ok
		// there may be uncompressed data left in the output buffer, that's ok too (we'll flush it next time)
		switch(r)
		{
			case ok:
			case Z_STREAM_END:		// all uncompressed data was read (but not necessarily streamed out)
			case Z_BUF_ERROR:		// we got enough data to satisfy this read
				return Z_OK;

			default:				// anything else is an error (inflateBack never returns Z_OK)
				return r;
		}
	}

	//////////////////////////////////////////////////////////////////////

	int Archive::InflateBackOutputCallback(void *context, unsigned char *data, unsigned length)
	{
		return ((Assistant *)context)->InflateBackOutput(data, length);
	}

	//////////////////////////////////////////////////////////////////////

	uint Archive::InflateBackInputCallback(void *context, unsigned char **buffer)
	{
		return ((Assistant *)context)->InflateBackInput(buffer);
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
			inflateBack9End(&mZStream);
		}
	}

	//////////////////////////////////////////////////////////////////////
	// Get an Archive::File ready for reading

	int Archive::File::Init(FileBase *file, ExtraInfo64 &e)
	{
		if(!file->Reopen(&mFile))										// reopen handle for reading the actual data
		{
			return error_fileerror;
		}
		if(!mFile->Seek(e.LocalHeaderOffset, SEEK_SET))					// goto localfileheader
		{
			return error_fileerror;
		}
		if(!mFile->Get(mHeader))										// read localfileheader
		{
			return error_fileerror;
		}
		if(mHeader.Signature != LocalHeaderSignature)					// check signature
		{
			return error_badzipfile;
		}
		if(mHeader.Info.CompressionMethod != Deflate &&					// check compression method is supported
//		   mHeader.Info.CompressionMethod != Deflate64 &&
		   mHeader.Info.CompressionMethod != None)
		{
			return error_notsupported;
		}
		if(!mFile->Seek(mHeader.Info.FilenameLength, SEEK_CUR))			// tee up the file pointer to the data
		{
			return error_fileerror;
		}
		mCompressedSize = mHeader.Info.CompressedSize;					// snarf some details
		mUncompressedSize = mHeader.Info.UncompressedSize;

		Archive::ProcessExtraInfo(mFile, mHeader.Info.ExtraFieldLength, [this] (uint16 tag, uint16 len, void *data)
		{
			if(tag == 0x0001)
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
	// Scan ExtraInfo blocks following a FileHeader or LocalFileHeader

	int Archive::ProcessExtraInfo(FileBase *file, size_t extraSize, std::function<int(uint16, uint16, void *)> callback)
	{
		Ptr<byte> buffer(new byte[65536]);								// Harumph, max size...
		uint64 currentPosition = file->Position();
		while(extraSize > 0)
		{
			uint16 tag;
			uint16 len;
			if(!(file->Get(tag) && file->Get(len)))						// get tag and len of extrainfo block
			{
				return error_fileerror;
			}
			uint32 got;
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
	// Read data from an Archive::File

	// Supply compressed data to the decompressor

	uint32 getData(void *in_desc, byte **buf)
	{
		Archive::File *f = (Archive::File *)in_desc;
		byte *p = f->mFileBuffer.get();
		*buf = p;
		uint32 len;
		uint32 get = (uint32)min(f->mCompressedDataRemaining, f->mFileBufferSize); 
		if(!f->mFile->Read(p, get, &len))
		{
			return 0;
		}
		f->mCompressedDataRemaining -= len;
		return len;
	}

	// Copy uncompressed data to the output stream

	int32 putData(void *out_desc, byte *buf, uint32 len)
	{
		Archive::File *f = (Archive::File *)out_desc;
		memcpy(f->mOutputPtr, buf, len);
		f->mOutputPtr += len;
		f->mOutputSize += len;
		return 0;
	}

	// main reader

	int Archive::File::Read(byte *buffer, size_t *got /* = null */, uint32 bufferSize)
	{
		mFileBufferSize = bufferSize;

		size_t totalGot = 0;
		// setup if necessary
		if(mFileBuffer == null && GetCompressionMethod() != None)
		{
			mFileBuffer.reset(new byte[mFileBufferSize]);
			mWindow.reset(new byte[1 << MAX_WBITS]);

			memset(&mZStream, 0, sizeof(mZStream));
			mZLibInitialized = true;
			int error = inflateBack9Init(&mZStream, mWindow.get());
			if(error != Z_OK)
			{
				return error;
			}
		}

		mOutputSize = 0;
		mOutputPtr = buffer;

		// decompress or just copy some data
		while(mOutputSize < mUncompressedSize && mCompressedDataRemaining > 0)
		{
			if(GetCompressionMethod() == None)
			{
				uint32 localGot;
				uint32 remainder = (uint32)min(MAXUINT32, mCompressedDataRemaining);
				if(!mFile->Read(mOutputPtr, remainder, &localGot) || localGot != remainder)
				{
					return error_fileerror;
				}
				mOutputPtr += (size_t)localGot;
				mOutputSize += (size_t)localGot;
				totalGot += localGot;
				mUncompressedDataRemaining -= localGot;
				mCompressedDataRemaining -= localGot;
			}
			else
			{
				int e = inflateBack9(&mZStream, &getData, this, &putData, this);
				if(e < 0)
				{
					return e;
				}
			}
		}
		if(got != null)
		{
			*got = mOutputSize;
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

		return ok;// file.Init(mFile, Extra);
	}


	// Prepare an Archive::File for reading

	int Archive::InitFile(File &file, FileHeader &f)
	{
		ExtraInfo64 Extra;
		int r = GetExtraInfo(mFile, Extra, f);
		return r == ok ? file.Init(mFile, Extra) : r;
	}

	//////////////////////////////////////////////////////////////////////
	// Find a file by name in an Archive

	int Archive::Locate2(char const *name, Archive::Assistant &file)
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