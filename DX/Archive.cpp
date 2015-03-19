//////////////////////////////////////////////////////////////////////

#include "DX.h"

//////////////////////////////////////////////////////////////////////

namespace
{
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
		ZPOS64_T number_entry;      /* total number of entries in the central dir on this disk */
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
		ZPOS64_T compressed_size;   /* compressed size                 8 bytes */
		ZPOS64_T uncompressed_size; /* uncompressed size               8 bytes */
		uLong size_filename;        /* filename length                 2 bytes */
		uLong size_file_extra;      /* extra field length              2 bytes */
		uLong size_file_comment;    /* file comment length             2 bytes */

		uLong disk_num_start;       /* disk number start               2 bytes */
		uLong internal_fa;          /* internal file attributes        2 bytes */
		uLong external_fa;          /* external file attributes        4 bytes */

		tm_unz tmu_date;
		ZPOS64_T disk_offset;
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

		ZPOS64_T pos_in_zipfile;            /* position in byte on the zipfile, for fseek */
		uLong stream_initialised;           /* flag set if stream structure is initialised */

		ZPOS64_T offset_local_extrafield;   /* offset of the local extra field */
		uInt size_local_extrafield;         /* size of the local extra field */
		ZPOS64_T pos_local_extrafield;      /* position in the local extra field in read */
		ZPOS64_T total_out_64;

		uLong crc32;                        /* crc32 of all data uncompressed */
		uLong crc32_wait;                   /* crc32 we must obtain after decompress all */
		ZPOS64_T rest_read_compressed;      /* number of byte to be decompressed */
		ZPOS64_T rest_read_uncompressed;    /* number of byte to be obtained after decomp */

		//zlib_filefunc64_32_def z_filefunc;

		voidpf filestream;                  /* io structore of the zipfile */
		uLong compression_method;           /* compression method (0==store) */
		ZPOS64_T byte_before_the_zipfile;   /* byte before the zipfile, (>0 for sfx) */
		int raw;
	} file_in_zip64_read_info_s;

	//////////////////////////////////////////////////////////////////////
	/* unz_file_info_interntal contain internal info about a file in zipfile*/

	typedef struct unz_file_info64_internal_s
	{
		ZPOS64_T offset_curfile;            /* relative offset of local header 8 bytes */
		ZPOS64_T byte_before_the_zipfile;   /* byte before the zipfile, (>0 for sfx) */
#ifdef HAVE_AES
		uLong aes_encryption_mode;
		uLong aes_compression_method;
		uLong aes_version;
#endif
	} unz_file_info64_internal;

	struct unz64_s
	{
		Ptr<DiskFile> fileStream;
		Ptr<DiskFile> fileStreamWithCD;

		unz_global_info64 gi;               /* public global information */
		ZPOS64_T byte_before_the_zipfile;   /* byte before the zipfile, (>0 for sfx)*/
		ZPOS64_T num_file;                  /* number of the current file in the zipfile*/
		ZPOS64_T pos_in_central_dir;        /* pos of the current file in the central dir*/
		ZPOS64_T current_file_ok;           /* flag about the usability of the current file*/
		ZPOS64_T central_pos;               /* position of the beginning of the central dir*/
		uLong number_disk;                  /* number of the current disk, used for spanning ZIP*/
		ZPOS64_T size_central_dir;          /* size of the central directory  */
		ZPOS64_T offset_central_dir;        /* offset of start of central directory with
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
		: mInfo(null)
		, mFile(null)
		, mLastError(ok)
	{
	}

	Archive::~Archive()
	{
	}

	uint64 Archive::GetCD(DiskFile &file)
	{
		ZPOS64_T file_size;
		ZPOS64_T back_read = 4;
		ZPOS64_T max_back = 0xffff; /* maximum size of global comment */
		ZPOS64_T pos_found = 0;
		uLong read_size;
		ZPOS64_T read_pos;
		int i;

		Ptr<byte> buf((byte *)ALLOC(BUFREADCOMMENT + 4));
		if(buf == NULL)
		{
			return 0;
		}

		file_size = file.Size();

		if(max_back > file_size)
		{
			max_back = file_size;
		}

		while(back_read < max_back)
		{
			if(back_read + BUFREADCOMMENT > max_back)
			{
				back_read = max_back;
			}
			else
			{
				back_read += BUFREADCOMMENT;
			}

			read_pos = file_size - back_read;
			read_size = ((BUFREADCOMMENT + 4) < (file_size - read_pos)) ? (BUFREADCOMMENT + 4) : (uLong)(file_size - read_pos);

			if(!file.Seek(read_pos, SEEK_SET))
			{
				break;
			}
			uint32 got;
			if(!file.Read(buf.get(), read_size, &got) || got != read_size)
			{
				break;
			}

			byte *p = buf.get();
			for(i = (int)read_size - 3; (i--) > 0;)
			{
				if(((*(p + i + 0)) == (ENDHEADERMAGIC >>  0 & 0xff)) &&
				   ((*(p + i + 1)) == (ENDHEADERMAGIC >>  8 & 0xff)) &&
				   ((*(p + i + 2)) == (ENDHEADERMAGIC >> 16 & 0xff)) &&
				   ((*(p + i + 3)) == (ENDHEADERMAGIC >> 24 & 0xff)))
				{
					pos_found = read_pos + i;
					break;
				}
			}

			if(pos_found != 0)
			{
				break;
			}
		}
		return pos_found;
	}

	uint64 Archive::GetCD64(DiskFile &file, ZPOS64_T const endcentraloffset)
	{
		ZPOS64_T offset;
		uLong uL;

		/* Zip64 end of central directory locator */
		if(!file.Seek(endcentraloffset - SIZECENTRALHEADERLOCATOR, SEEK_SET))
		{
			return 0;
		}

		/* read locator signature */
		if(!file.Get(uL) || uL != ZIP64ENDLOCHEADERMAGIC)
		{
			return 0;
		}

		/* number of the disk with the start of the zip64 end of  central directory */
		if(!file.Get(uL))
		{
			return 0;
		}

		/* relative offset of the zip64 end of central directory record */
		if(!file.Get(offset))
		{
			return 0;
		}

		/* total number of disks */
		if(!file.Get(uL))
		{
			return 0;
		}

		/* Goto end of central directory record */
		if(!file.Seek(offset, SEEK_SET))
		{
			return 0;
		}

		/* the signature */
		if(!file.Get(uL) || uL != ZIP64ENDHEADERMAGIC)
		{
			return 0;
		}

		return offset;
	}

	bool Archive::Open(tchar const *name)
	{
		unz64_s us;
		unz64_s *s;
		ZPOS64_T central_pos;
		uLong uL;
		uint16 uS;
		voidpf filestream = NULL;
		ZPOS64_T number_entry_CD;
		int err = UNZ_OK;

		if(unz_copyright[0] != ' ')
			return NULL;

		us.fileStreamWithCD.reset();
		us.fileStream.reset(new DiskFile());
		us.fileStream->Open(name, DiskFile::ForReading);

		if(!us.fileStream->h.IsValid())
			return NULL;

		us.fileStreamWithCD.reset(us.fileStream.get());
		us.isZip64 = 0;

		/* Use unz64local_SearchCentralDir first. Only based on the result
		is it necessary to locate the unz64local_SearchCentralDir64 */
		central_pos = GetCD(*us.fileStream.get());
		if(central_pos)
		{
			if(!us.fileStream->Seek(central_pos, SEEK_SET))
				err = UNZ_ERRNO;

			/* the signature, already checked */
			if(!us.fileStream->Get(uL))
				err = UNZ_ERRNO;

			/* number of this disk */
			if(!us.fileStream->Get(uS))
				err = UNZ_ERRNO;
			us.number_disk = uS;

			/* number of the disk with the start of the central directory */
			if(!us.fileStream->Get(uS))
				err = UNZ_ERRNO;
			us.gi.number_disk_with_CD = uS;

			/* total number of entries in the central directory on this disk */
			if(!us.fileStream->Get(uS))
				err = UNZ_ERRNO;
			us.gi.number_entry = uS;

			/* total number of entries in the central directory */
			if(!us.fileStream->Get(uS))
				err = UNZ_ERRNO;
			number_entry_CD = uS;

			if(number_entry_CD != us.gi.number_entry)
				err = UNZ_BADZIPFILE;

			/* size of the central directory */
			if(!us.fileStream->Get(uL))
				err = UNZ_ERRNO;
			us.size_central_dir = uL;

			/* offset of start of central directory with respect to the starting disk number */
			if(!us.fileStream->Get(uL))
				err = UNZ_ERRNO;
			us.offset_central_dir = uL;

			/* zipfile comment length */
			if(!us.fileStream->Get(uS))
				err = UNZ_ERRNO;
			us.gi.size_comment = uS;

			if(err == UNZ_OK && (us.gi.number_entry == 0xffff || us.size_central_dir == 0xffff || us.offset_central_dir == 0xffffffff))
			{
				/* Format should be Zip64, as the central directory or file size is too large */
				central_pos = GetCD64(*us.fileStream.get(), central_pos);
				if(central_pos != 0)
				{
					ZPOS64_T uL64;

					us.isZip64 = 1;

					if(!us.fileStream->Seek(central_pos, SEEK_SET))
						err = UNZ_ERRNO;

					/* the signature, already checked */
					if(!us.fileStream->Get(uL))
						err = UNZ_ERRNO;

					/* size of zip64 end of central directory record */
					if(!us.fileStream->Get(uL64))
						err = UNZ_ERRNO;

					/* version made by */
					if(!us.fileStream->Get(uS))
						err = UNZ_ERRNO;

					/* version needed to extract */
					if(!us.fileStream->Get(uS))
						err = UNZ_ERRNO;

					/* number of this disk */
					if(!us.fileStream->Get(us.number_disk))
						err = UNZ_ERRNO;

					/* number of the disk with the start of the central directory */
					if(!us.fileStream->Get(us.gi.number_disk_with_CD))
						err = UNZ_ERRNO;

					/* total number of entries in the central directory on this disk */
					if(!us.fileStream->Get(us.gi.number_entry))
						err = UNZ_ERRNO;

					/* total number of entries in the central directory */
					if(!us.fileStream->Get(number_entry_CD))
						err = UNZ_ERRNO;
					if(number_entry_CD != us.gi.number_entry)
						err = UNZ_BADZIPFILE;

					/* size of the central directory */
					if(!us.fileStream->Get(us.size_central_dir))
						err = UNZ_ERRNO;

					/* offset of start of central directory with respect to the starting disk number */
					if(!us.fileStream->Get(us.offset_central_dir))
						err = UNZ_ERRNO;
				}
				else
				{
					err = UNZ_BADZIPFILE;
				}
			}
		}
		else
		{
			err = UNZ_ERRNO;
		}

		if((err == UNZ_OK) && (central_pos < us.offset_central_dir + us.size_central_dir))
		{
			err = UNZ_BADZIPFILE;
		}

		if(err != UNZ_OK)
		{
			us.fileStream->Close();
			return NULL;
		}

		if(us.gi.number_disk_with_CD == 0)
		{
			/* If there is only one disk open another stream so we don't have to seek between the CD
			and the file headers constantly */
			filestream =  ZOPEN64(us.z_filefunc, path, ZLIB_FILEFUNC_MODE_READ | ZLIB_FILEFUNC_MODE_EXISTING);
			if(filestream != NULL)
				us.filestream = filestream;
		}

		/* Hack for zip files that have no respect for zip64
		if ((central_pos > 0xffffffff) && (us.offset_central_dir < 0xffffffff))
		us.offset_central_dir = central_pos - us.size_central_dir;*/

		us.byte_before_the_zipfile = central_pos - (us.offset_central_dir + us.size_central_dir);
		us.central_pos = central_pos;
		us.pfile_in_zip_read = NULL;

		s = (unz64_s*)ALLOC(sizeof(unz64_s));
		if(s != NULL)
		{
			*s = us;
			unzGoToFirstFile((unzFile)s);
		}
		return (unzFile)s;
	}

	void Archive::Close()
	{
	}
}