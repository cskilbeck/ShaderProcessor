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

		enum CompressionMethod: uint16
		{
			None = 0,
			Shrink = 1,
			Reduce1 = 2,
			Reduce2 = 3,
			Reduce3 = 4,
			Reduce4 = 5,
			Implode = 6,
			Tokenize = 7,
			Deflate = 8,
			Deflate64 = 9,
			Reserved1 = 10,
			Reserved2 = 11,
			BZIP2 = 12,
			Reserved3 = 13,
			LZMA = 14,
			Reserved4 = 15,
			Reserved5 = 16,
			Reserved6 = 17,
			IBM_TERSE = 18,
			IBMLZ77 = 19,
			WavPack = 97,
			PPMd1 = 98
		};

		enum VersionMadeBy: uint16
		{
			MSDOS = 0,
			Amiga = 1,
			OpenVMS = 2,
			UNIX = 3,
			VMCMS = 4,
			AtariST = 5,
			OS2 = 6,
			Macintosh = 7,
			ZSystem = 8,
			CPM = 9,
			WindowsNTFS = 10,
			MVS = 11,
			VSE = 12,
			AcornRisc = 13,
			VFAT = 14,
			alternateMVS = 15,
			BeOS = 16,
			Tandem = 17,
			OS400 = 18,
			OSX = 19
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

		// 64 bit version if Zip64

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
			// ExtraField
		};

		// This precedes each file

		struct LocalFileHeader
		{
			uint32			Signature;	// 0x04034b50
			uint16			VersionRequired;
			FileInfo		Info;
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
			uint64			DirectoryWRTStartingDiskNumber;
		};

		// Haven't worked out how this fits in yet

		struct EndOfCentralDirectory64Locator
		{
			uint32			Signature;
			uint32			DiskWhichContainsEOCD64;
			uint64			CDOffset;
			uint32			DiskCount;
		};

#		pragma pack(pop)

		struct File
		{
			enum
			{
				FileBufferSize = 4096
			};

			uint64			mUncompressedDataRemaining;
			uint64			mCompressedDataRemaining;
			LocalFileHeader	mHeader;
			Ptr<byte>		mFileBuffer;
			FileBase *		mFile;
			z_stream		mZStream;
			uint32			mCRCSoFar;

			File();
			~File();

			int Init(FileBase *file, FileHeader &f);
			int Read(byte *buffer, size_t amount, size_t *got = null);
			void Close();

			uint64 CompressedSize() const
			{
				return mHeader.Info.CompressedSize;
			}

			uint64 UncompressedSize() const
			{
				return mHeader.Info.UncompressedSize;
			}

			CompressionMethod GetCompressionMethod() const
			{
				return (CompressionMethod)mHeader.Info.CompressionMethod;
			}
		};

		Archive();
		~Archive();

		int Open(FileBase *zipFile);
		void Close();

		uint64 FileCount() const;

		int Locate(tchar const *name, File &file);
		int Goto(uint32 fileIndex, File &file);

	private:

		int GetCD(uint64 &offset);
		int GetCD64(uint64 &offset, uint64 const endcentraloffset);

		uint64		mCentralDirectoryLocation;
		uint64		mEntriesInCentralDirectory;
		uint64		mSizeOfCentralDirectory;
		FileBase *	mFile;
		bool		mIsZip64;
	};

	inline uint64 Archive::FileCount() const
	{
		return mEntriesInCentralDirectory;
	}
}