//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

#include "..\zlib-1.2.8\inftrees.h"
#include "..\zlib-1.2.8\inflate.h"

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

	private:

		static int InflateBackOutputCallback(void *context, unsigned char *data, unsigned length);
		static uint InflateBackInputCallback(void *context, unsigned char **buffer);

		// Merge this into File!

	private:
	
		enum CompressionMethod: uint16
		{
			None = 0,
			Deflate = 8,
			Deflate64 = 9
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

	public:

	public:

		struct Assistant
		{
			int Init(FileBase *inputFile, FileHeader &f);
			int Read(byte *buffer, uint64 bytesToRead, uint64 *got = null);

		private:

			Ptr<inflateBackState> IBState;			// context
			Ptr<byte> fileBuffer;					// file read buffer
			uint32 fileBufferSize;					// size of the file read buffer
			FileBase *file;							// file we're reading compressed data from
			byte *outputBuffer;						// base address of where the outputdata is going to be stored
			byte *currentOutputPointer;				// where the next bit of data is going to be written to
			byte *currentDataPointer;				// where the next bit of uncompressed data is
			uint64 outputBufferSize;				// total size of the outputBuffer
			uint64 bytesRequired;					// uncompressed size of the uncompress request
			uint64 totalGot;						// uncompressed bytes read so far from this file
			uint64 cachedBytesRemaining;			// from the last time - flush these out before going into inflateBack
			uint64 bufferSize;						// size of the decompression window buffer
			z_stream zstream;
			inflate_state inflateState;
			Ptr<byte>		mWindow;

			uint64			mUncompressedSize;
			uint64			mUncompressedDataRemaining;
			uint64			mCompressedSize;
			uint64			mCompressedDataRemaining;
			uint64			mLocationInZipFile;
			LocalFileHeader	mHeader;

			int InflateBackOutput(unsigned char *data, unsigned length);
			int InflateBackInput(unsigned char **buffer);

			friend struct Archive;
		};

		// A file within a Zip file

		struct File
		{
			File();
			~File();

			// Read compressed file data
			int Read(byte *buffer, size_t *got = null, uint32 bufferSize = 65536);

			// Release everything
			void Close();

			// Size of uncompressed data
			uint64 Size() const;
		
		private:

			friend struct Archive;
			friend uint32 getData(void *in_desc, byte **buf);
			friend int32 putData(void *out_desc, byte *buf, uint32 len);

			uint64			mUncompressedSize;
			uint64			mUncompressedDataRemaining;
			uint64			mCompressedSize;
			uint64			mCompressedDataRemaining;
			uint64			mLocationInZipFile;
			LocalFileHeader	mHeader;
			byte *			mOutputPtr;
			size_t			mOutputSize;
			Ptr<byte>		mFileBuffer;
			uint32			mFileBufferSize;
			Ptr<byte>		mWindow;
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

		int Archive::Locate2(char const *name, Archive::Assistant &file);
		
		// Find a file by name
		int Locate(tchar const *name, File &file);

		// Got a file by index
		int Goto(uint32 fileIndex, File &file);

	private:

		static int ProcessExtraInfo(FileBase *file, size_t extraSize, std::function<int(uint16, uint16, void *)> callback);

		int GetCDLocation(uint64 &offset);
		int GetCD64Location(uint64 &offset, uint64 const endcentraloffset);

		friend struct Assistant;

		static int GetExtraInfo(FileBase *file, ExtraInfo64 &extra, FileHeader &f);

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