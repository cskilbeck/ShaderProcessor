//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	struct Archive
	{
		enum Status: int
		{
			ok = 0,
			error_badzipfile = -200,	// Zip format parser error
			error_fileerror,			// File IO error
			error_notsupported,			// Only unencrypted, single disc, deflate/store supported
			error_filenotfound			// Can't find a file within the zip
		};

		enum CompressionMethod: uint16
		{
			None = 0,
			Deflate = 8
		};

#		pragma pack(push, 1)

		// Follows the compressed data IF bit 3 of FileInfo.Flags is set

		struct DataDescriptor
		{
			//optionally
			//uint32		Signature	// 0x08074b50
			uint32			CRC;
			uint32			CompressedSize;
			uint32			UncompressedSize;
		};

		// 64 bit version if file is in Zip64 format

		struct DataDescriptor64
		{
			//optionally
			//uint32		Signature	// 0x08074b50
			uint32			CRC;
			uint64			CompressedSize;
			uint64			UncompressedSize;
		};

		// Common to LocalFileHeader and FileHeader

		struct FileInfo
		{
			uint16			Flags;
			uint16			CompressionMethod;
			uint16			FileTime;
			uint16			FileDate;
			uint32			CRC;
			uint32			CompressedSize;
			uint32			UncompressedSize;
			uint16			FilenameLength;
			uint16			ExtraFieldLength;
			// Filename
			// ExtraField (used if it's a Zip64 format file)
		};

		// This precedes each file stored in the Zip

		struct LocalFileHeader
		{
			uint32			Signature;	// 0x04034b50
			uint16			VersionRequired;
			FileInfo		Info;
		};

		// A LocalFileHeader has one of these after it if it's Zip64

		struct LocalExtraInfo64
		{
			uint64			UnCompressedSize;
			uint64			CompressedSize;
		};

		// A FileHeader has one of these after it if it's Zip64

		struct ExtraInfo64
		{
			uint64			UnCompressedSize;
			uint64			CompressedSize;
			uint64			LocalHeaderOffset;			// optional, usually missing
			uint32			DiskOnWhichThisFileStarts;
		};

		// The Central Directory consists of these:

		struct FileHeader
		{
			uint32			Signature;	// 0x02014b50
			uint16			VersionCreatedBy;
			uint16			VersionRequired;
			FileInfo		Info;
			uint16			FileCommentLength;
			uint16			DiskNumberStart;
			uint16			InternalFileAttributes;
			uint32			ExternalFileAttributes;
			uint32			LocalHeaderOffset;
			// File Comment
		};

		// The Central Directory ends with one of these

		struct EndOfCentralDirectory
		{
			uint32			Signature;
			uint16			DiskNumber;
			uint16			DiskWithCD;
			uint16			EntriesInCDOnThisDisk;
			uint16			EntriesInCD;
			uint32			SizeOfCD;
			uint32			CDOffset;
			uint16			CommentLength;
			// Comment
		};

		// And maybe one of these

		struct EndOfCentralDirectory64
		{
			uint32			Signature;
			uint64			SizeOfRecord;	// -12 (sizeof this and previous field)
			uint16			Version;
			uint16			VersionRequired;
			uint32			DiskNumber;
			uint32			DiskWithCD;
			uint64			EntriesInCDOnThisDisk;
			uint64			EntriesInCD;
			uint64			SizeOfCD;
			uint64			CDOffset;
		};

		// This is used to find the Central Directory if it's a Zip64 file

		struct EndOfCentralDirectory64Locator
		{
			uint32			Signature;
			uint32			DiskWhichContainsEOCD64;
			uint64			CDOffset;
			uint32			DiskCount;
		};

#		pragma pack(pop)

		// A file within a Zip file

		struct File
		{
			File();
			~File();

			// Read compressed file data
			int Read(byte *buffer, size_t amount, size_t *got = null, uint32 bufferSize = 65536);
			int Read2(byte *buffer, size_t amount, size_t *got = null, uint32 bufferSize = 65536);

			// Release everything
			void Close();

			// Size of uncompressed data
			uint64 Size() const;
		
		private:

			friend struct Archive;

			uint64			mUncompressedSize;
			uint64			mUncompressedDataRemaining;
			uint64			mCompressedSize;
			uint64			mCompressedDataRemaining;
			uint64			mLocationInZipFile;
			LocalFileHeader	mHeader;
			Ptr<byte>		mFileBuffer;
			FileBase *		mFile;
			z_stream		mZStream;
			bool			mZLibInitialized;

			CompressionMethod GetCompressionMethod() const;
			uint64 CompressedSize() const;
			int Init(FileBase *file, ExtraInfo64 &e);
		};

		Archive();
		~Archive();

		// Open an archive from an existing file
		int Open(FileBase *zipFile);

		// Release everything
		void Close();

		// How many files in the archive
		uint64 FileCount() const;

		// Find a file by name
		int Locate(tchar const *name, File &file);

		// Got a file by index
		int Goto(uint32 fileIndex, File &file);

	private:

		static int ProcessExtraInfo(FileBase *file, size_t extraSize, std::function<int(uint16, uint16, void *)> callback);

		int GetCDLocation(uint64 &offset);
		int GetCD64Location(uint64 &offset, uint64 const endcentraloffset);

		int InitFile(File &file, FileHeader &f);

		uint64		mCentralDirectoryLocation;
		uint64		mEntriesInCentralDirectory;
		uint64		mSizeOfCentralDirectory;
		FileBase *	mFile;
		bool		mIsZip64;
	};

	inline uint64 Archive::File::CompressedSize() const
	{
		return mCompressedSize;
	}

	inline uint64 Archive::File::Size() const
	{
		return mUncompressedSize;
	}

	inline Archive::CompressionMethod Archive::File::GetCompressionMethod() const
	{
		return (CompressionMethod)mHeader.Info.CompressionMethod;
	}

	inline uint64 Archive::FileCount() const
	{
		return mEntriesInCentralDirectory;
	}
}