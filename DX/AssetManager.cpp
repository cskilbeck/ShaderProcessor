#include "DX.h"

namespace DX
{
	struct AssetSource
	{
		virtual bool Create(tchar const *name) = 0;
		virtual bool Open(tchar const *name, FileBase **file) = 0;
	};

	struct FolderSource: AssetSource
	{
		bool Create(tchar const *name) override
		{
			folder = name;
			return FolderExists(name);
		}

		bool Open(tchar const *name, FileBase **file) override
		{
			Ptr<DiskFile> diskFile(new DiskFile());
			if(!diskFile->Open((folder + TEXT("/") + name).c_str(), DiskFile::ForReading))
			{
				return false;
			}
			*file = diskFile.release();
			return true;
		}
		tstring folder;
	};

	struct ArchiveSource: AssetSource
	{
		bool Create(tchar const *name) override
		{
			if(!file.Open(name, DiskFile::ForReading))
			{
				return false;
			}
			return archive.Open(&file) == Archive::ok;
		}

		bool Open(tchar const *name, FileBase **file) override
		{
			Ptr<Archive::File> f(new Archive::File());
			if(archive.Locate(name, *f) != Archive::ok)
			{
				return false;
			}
			*file = f.release();
			return true;
		}

		DiskFile file;
		Archive archive;
	};

	AssetManager::AssetManager()
	{
	}

	AssetManager::~AssetManager()
	{
	}

	bool AssetManager::AddFolder(tchar const *folderName)
	{
		Ptr<FolderSource> f(new FolderSource());
		if(!f->Create(folderName))
		{
			return false;
		}
		sources.push_back(f.release());
		return true;
	}

	bool AssetManager::AddArchive(tchar const *archiveName)
	{
		Ptr<ArchiveSource> a(new ArchiveSource());
		if(!a->Create(archiveName))
		{
			return false;
		}
		sources.push_back(a.release());
		return true;
	}

	bool AssetManager::Open(tchar const *filename, FileBase **file)
	{
		for(auto p : sources)
		{
			if(p->Open(filename, file))
			{
				return true;
			}
		}
		return false;
	}
}
