//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	struct Archive
	{
		enum Error : int
		{
			ok = 0,
			end_of_list_of_file = -100,
			errnum = Z_ERRNO,
			eof = 0,
			cantopenfile = -101,
			paramerror = -102,
			badzipfile = -103,
			internalerror = -104,
			crcerror = -105,
			fileerror = -106,
			notsupported = -107,
			filenotfound = -108
		};

		Archive();
		~Archive();

		Error Open(FileBase *zipFile);
		void Close();

		uint64 FileCount() const;

		Error Locate(tchar const *name);
		Error Goto(uint32 fileIndex);

		Error Read(byte *buffer, size_t amount, size_t *got = null);

		uint64 CurrentFileSize() const;
		uint32 CurrentFileIndex() const;

	private:

		Error GetCD(uint64 &offset);
		Error GetCD64(uint64 &offset, uint64 const endcentraloffset);
		Error OpenCurrentFile();

		enum
		{
			FileBufferSize = 65536
		};

		uint64				mCurrentFileSize;
		uint64				mCurrentFileOffset;
		uint64				mCentralDirectoryLocation;
		uint64				mEntriesInCentralDirectory;
		uint64				mSizeOfCentralDirectory;
		uint64				mFilePosition;
		uint64				mCompressedDataRemaining;
		uint64				mUncompressedDataRemaining;
		FileBase *			mFile;
		FileBase *			mCDFile;
		uint32				mCompressionMethod;
		uint32				mCurrentFileIndex;
		uint32				mCRC;
		uint32				mCRCSoFar;
		Ptr<byte>			mFileBuffer;
		z_stream			mZStream;
		bool				mIsZip64;
		bool				mIsOpen;
	};

	inline uint64 Archive::FileCount() const
	{
		return mEntriesInCentralDirectory;
	}

	inline uint64 Archive::CurrentFileSize() const
	{
		return mCurrentFileSize;
	}

	inline uint32 Archive::CurrentFileIndex() const
	{
		return mCurrentFileIndex;
	}
}
