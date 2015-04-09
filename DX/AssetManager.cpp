#include "DX.h"

namespace
{
	std::list<AssetSource *> sources;
}

namespace DX
{
	struct AssetSource
	{
		virtual int Create(tchar const *name) = 0;
		virtual int Open(tchar const *name, FileBase **file) = 0;
	};

	struct FolderSource: AssetSource
	{
		int Create(tchar const *name) override
		{
			folder = name;
			return FolderExists(name) ? S_OK : ERROR_FILE_NOT_FOUND;
		}

		int Open(tchar const *name, FileBase **file) override
		{
			Ptr<DiskFile> diskFile(new DiskFile());
			DXR(diskFile->Open((folder + TEXT("/") + name).c_str(), DiskFile::ForReading));
			*file = diskFile.release();
			return S_OK;
		}
		tstring folder;
	};

	struct ArchiveSource: AssetSource
	{
		int Create(tchar const *name) override
		{
			if(!file.Open(name, DiskFile::ForReading))
			{
				return ERROR_FILE_NOT_FOUND;
			}
			return (archive.Open(&file) == Archive::ok) ? S_OK : ERROR_FILE_NOT_FOUND;
		}

		int Open(tchar const *name, FileBase **file) override
		{
			Ptr<Archive::File> f(new Archive::File());
			if(archive.Locate(name, *f) != Archive::ok)
			{
				return ERROR_FILE_NOT_FOUND;
			}
			*file = f.release();
			return S_OK;
		}

		DiskFile file;
		Archive archive;
	};

	namespace AssetManager
	{
		int AddFolder(tchar const *folderName)
		{
			Ptr<FolderSource> f(new FolderSource());
			DXR(f->Create(folderName));
			sources.push_back(f.release());
			return S_OK;
		}

		int AssetManager::AddArchive(tchar const *archiveName)
		{
			Ptr<ArchiveSource> a(new ArchiveSource());
			DXR(a->Create(archiveName));
			sources.push_back(a.release());
			return S_OK;
		}

		bool FileExists(tchar const *filename)
		{
			for(auto p : sources)
			{
				FileBase *f;
				if(p->Open(filename, &f) == S_OK)
				{
					f->Close();
					return true;
				}
			}
			return false;
		}

		int AssetManager::Open(tchar const *filename, FileBase **file)
		{
			for(auto p : sources)
			{
				if(p->Open(filename, file) == S_OK)
				{
					return S_OK;
				}
			}
			return ERROR_FILE_NOT_FOUND;
		}

		int LoadFile(tchar const *filename, FileResource &data)
		{
			FileBase *file;
			DXR(Open(filename, &file));
			Ptr<FileBase> filep(file);
			file->GetSize(data.Size());
			data.Data() = new byte[data.Size()];
			size_t got;
			if(file->Read(data.Data(), data.Size(), &got) != S_OK || got != data.Size())
			{
				data.Data() = null;
				data.Size() = 0;
				return ERROR_FILE_INVALID;
			}
			filep.release();	// data owns it now...
			return S_OK;
		}
	}
}
