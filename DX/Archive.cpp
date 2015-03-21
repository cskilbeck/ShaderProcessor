//////////////////////////////////////////////////////////////////////

#include "DX.h"

//////////////////////////////////////////////////////////////////////

namespace
{
#pragma pack(push, 1)

	// Follows the compressed data IF bit 3 of FileInfo.Flags is set
	struct DataDescriptor
	{
		//optionally
		//uint32		Signature	// 0x08074b50
		uint32			CRC;
		uint32			CompressedSize;
		uint32			UncompressedSize;
	};

	struct DataDescriptor64
	{
		//optionally
		//uint32		Signature	// 0x08074b50
		uint32			CRC;
		uint64			CompressedSize;
		uint64			UncompressedSize;
	};

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

	struct LocalFileHeader
	{
		uint32			Signature;	// 0x04034b50
		uint16			VersionRequired;
		FileInfo		Info;
	};

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

	struct EndOfCentralDirectory64Locator
	{
		uint32			Signature;
		uint32			DiskWhichContainsEOCD64;
		uint64			CDOffset;
		uint32			DiskCount;
	};

	enum CompressionMethods: uint16
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

	enum VersionMadeBy : uint16
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

#pragma pack(pop)

	//////////////////////////////////////////////////////////////////////
	/* tm_unz contain date/time info */
	typedef struct tm_unz_s
	{
		uInt tm_sec;                /* seconds after the minute - [0,59] */
		uInt tm_min;                /* minutes after the hour - [0,59] */
		uInt tm_hour;               /* hours since midnight - [0,23] */
		uInt tm_mday;               /* day of the month - [1,31] */
		uInt tm_mon;                /* months since January - [0,11] */
		uInt tm_year;               /* years - [1980..2044] */
	} tm_unz;

	//////////////////////////////////////////////////////////////////////
	/* unz_global_info structure contain global data about the ZIPfile
	These data comes from the end of central dir */

	typedef struct unz_global_info64_s
	{
		uint64 number_entry;      /* total number of entries in the central dir on this disk */
		uLong number_disk_with_CD;  /* number the the disk with central dir, used for spanning ZIP*/
		uLong size_comment;         /* size of the global comment of the zipfile */
	} unz_global_info64;

	//////////////////////////////////////////////////////////////////////

	typedef struct unz_global_info_s
	{
		uLong number_entry;         /* total number of entries in the central dir on this disk */
		uLong number_disk_with_CD;  /* number the the disk with central dir, used for spanning ZIP*/
		uLong size_comment;         /* size of the global comment of the zipfile */
	} unz_global_info;

	//////////////////////////////////////////////////////////////////////
	/* unz_file_info contain information about a file in the zipfile */

	typedef struct unz_file_info64_s
	{
		uLong version;              /* version made by                 2 bytes */
		uLong version_needed;       /* version needed to extract       2 bytes */
		uLong flag;                 /* general purpose bit flag        2 bytes */
		uLong compression_method;   /* compression method              2 bytes */
		uLong dosDate;              /* last mod file date in Dos fmt   4 bytes */
		uLong crc;                  /* crc-32                          4 bytes */
		uint64 compressed_size;   /* compressed size                 8 bytes */
		uint64 uncompressed_size; /* uncompressed size               8 bytes */
		uLong size_filename;        /* filename length                 2 bytes */
		uLong size_file_extra;      /* extra field length              2 bytes */
		uLong size_file_comment;    /* file comment length             2 bytes */

		uLong disk_num_start;       /* disk number start               2 bytes */
		uLong internal_fa;          /* internal file attributes        2 bytes */
		uLong external_fa;          /* external file attributes        4 bytes */

		tm_unz tmu_date;
		uint64 disk_offset;
		uLong size_file_extra_internal;
	} unz_file_info64;

	//////////////////////////////////////////////////////////////////////

	typedef struct unz_file_info_s
	{
		uLong version;              /* version made by                 2 bytes */
		uLong version_needed;       /* version needed to extract       2 bytes */
		uLong flag;                 /* general purpose bit flag        2 bytes */
		uLong compression_method;   /* compression method              2 bytes */
		uLong dosDate;              /* last mod file date in Dos fmt   4 bytes */
		uLong crc;                  /* crc-32                          4 bytes */
		uLong compressed_size;      /* compressed size                 4 bytes */
		uLong uncompressed_size;    /* uncompressed size               4 bytes */
		uLong size_filename;        /* filename length                 2 bytes */
		uLong size_file_extra;      /* extra field length              2 bytes */
		uLong size_file_comment;    /* file comment length             2 bytes */

		uLong disk_num_start;       /* disk number start               2 bytes */
		uLong internal_fa;          /* internal file attributes        2 bytes */
		uLong external_fa;          /* external file attributes        4 bytes */

		tm_unz tmu_date;
		uLong disk_offset;
	} unz_file_info;

	//////////////////////////////////////////////////////////////////////

	typedef struct
	{
		Bytef *read_buffer;                 /* internal buffer for compressed data */
		z_stream stream;                    /* zLib stream structure for inflate */

#ifdef HAVE_BZIP2
		bz_stream bstream;                  /* bzLib stream structure for bziped */
#endif
#ifdef HAVE_AES
		fcrypt_ctx aes_ctx;
#endif

		uint64 pos_in_zipfile;            /* position in byte on the zipfile, for fseek */
		uLong stream_initialised;           /* flag set if stream structure is initialised */

		uint64 offset_local_extrafield;   /* offset of the local extra field */
		uInt size_local_extrafield;         /* size of the local extra field */
		uint64 pos_local_extrafield;      /* position in the local extra field in read */
		uint64 total_out_64;

		uLong crc32;                        /* crc32 of all data uncompressed */
		uLong crc32_wait;                   /* crc32 we must obtain after decompress all */
		uint64 rest_read_compressed;      /* number of byte to be decompressed */
		uint64 rest_read_uncompressed;    /* number of byte to be obtained after decomp */

		//zlib_filefunc64_32_def z_filefunc;

		voidpf filestream;                  /* io structore of the zipfile */
		uLong compression_method;           /* compression method (0==store) */
		uint64 byte_before_the_zipfile;   /* byte before the zipfile, (>0 for sfx) */
		int raw;
	} file_in_zip64_read_info_s;

	//////////////////////////////////////////////////////////////////////
	/* unz_file_info_interntal contain internal info about a file in zipfile*/

	typedef struct unz_file_info64_internal_s
	{
		uint64 offset_curfile;            /* relative offset of local header 8 bytes */
		uint64 byte_before_the_zipfile;   /* byte before the zipfile, (>0 for sfx) */
#ifdef HAVE_AES
		uLong aes_encryption_mode;
		uLong aes_compression_method;
		uLong aes_version;
#endif
	} unz_file_info64_internal;

	//////////////////////////////////////////////////////////////////////

	struct unz64_s
	{
		DiskFile fileStream;
		DiskFile fileStreamWithCD;

		unz_global_info64 gi;               /* public global information */
		uint64 byte_before_the_zipfile;   /* byte before the zipfile, (>0 for sfx)*/
		uint64 num_file;                  /* number of the current file in the zipfile*/
		uint64 pos_in_central_dir;        /* pos of the current file in the central dir*/
		uint64 current_file_ok;           /* flag about the usability of the current file*/
		uint64 central_pos;               /* position of the beginning of the central dir*/
		uLong number_disk;                  /* number of the current disk, used for spanning ZIP*/
		uint64 size_central_dir;          /* size of the central directory  */
		uint64 offset_central_dir;        /* offset of start of central directory with
											respect to the starting disk number */

		unz_file_info64 cur_file_info;      /* public info about the current file in zip*/
		unz_file_info64_internal cur_file_info_internal;
		/* private info about it*/
		file_in_zip64_read_info_s* pfile_in_zip_read;
		/* structure about the current file if we are decompressing it */
		int isZip64;                        /* is the current file zip64 */
#ifndef NOUNCRYPT
		unsigned long keys[3];              /* keys defining the pseudo-random sequence */
		const unsigned long* pcrc_32_tab;
#endif
	};

	//////////////////////////////////////////////////////////////////////

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
		, mCDFile(null)
		, mIsZip64(false)
		, mCentralDirectoryLocation(0)
		, mEntriesInCentralDirectory(0)
		, mSizeOfCentralDirectory(0)
		, mIsOpen(false)
	{
	}

	Archive::~Archive()
	{
	}

	Archive::Error Archive::GetCD(uint64 &offset)
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

	Archive::Error Archive::GetCD64(uint64 &offset, uint64 const endcentraloffset)
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

	Archive::Error Archive::Open(FileBase *zipFile)
	{
		mFile = zipFile;
		mCDFile = new DiskFile();

		uint64 central_pos;

		int err = ok;

		if(unz_copyright[0] != ' ')
		{
			return internalerror;
		}

		mIsZip64 = false;

		Error e = GetCD(central_pos);
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

		if(!mCDFile->Clone(mFile))
		{
			return fileerror;
		}
		return ok;
	}

	Archive::Error Archive::Locate(char const *name)
	{
		if(!mFile->Seek(mCentralDirectoryLocation, SEEK_SET))
		{
			return fileerror;
		}

		char filename[1024];

		for(uint i = 0; i < mEntriesInCentralDirectory; ++i)
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
				mCurrentFileIndex = i;
				mCurrentFileOffset = f.LocalHeaderOffset;
				mCurrentFileSize = f.Info.UncompressedSize;
				return ok;
			}
			if(!mFile->Seek(f.FileCommentLength, SEEK_CUR))
			{
				return fileerror;
			}
		}
		return filenotfound;
	}

	Archive::Error Archive::Goto(uint32 index)
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
				mCurrentFileIndex = i;
				mCurrentFileOffset = f.LocalHeaderOffset;
				mCurrentFileSize = f.Info.UncompressedSize;
				return ok;
			}
			if(!mFile->Seek(f.Info.FilenameLength + f.FileCommentLength, SEEK_CUR))
			{
				return fileerror;
			}
		}
		return filenotfound;
	}

	Archive::Error Archive::OpenCurrentFile()
	{
		if(!mCDFile->Seek(mCurrentFileSize, SEEK_SET))
		{
			return fileerror;
		}
		LocalFileHeader f;
		if(!mCDFile->Get(f))
		{
			return fileerror;
		}
		if(f.Signature != LOCALHEADERMAGIC)
		{
			return badzipfile;
		}
		if(f.Info.CompressionMethod != Deflate)
		{
			return notsupported;
		}
		memset(&mZStream, 0, sizeof(mZStream));
		int error = inflateInit2(&mZStream, -MAX_WBITS);
		if(error != Z_OK)
		{
			return badzipfile;
		}
		mUncompressedDataRemaining = f.Info.UncompressedSize;
		mCompressedDataRemaining = f.Info.CompressedSize;
		mFilePosition = mFile->Position() + f.Info.ExtraFieldLength;
		mFileBuffer.reset(new byte[FileBufferSize]);
		mIsOpen = true;
	}

	Archive::Error Archive::Read(byte *buffer, size_t amount, size_t *got)
	{
		if(!mIsOpen)
		{
			Error e = OpenCurrentFile();
			if(e != ok)
			{
				return e;
			}
		}
		// decompress in a loop until amount has been reached
	}

	void Archive::Close()
	{
	}
}