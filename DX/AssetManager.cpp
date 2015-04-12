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
		virtual ~AssetSource()
		{
		}
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
			if(file.Open(name, DiskFile::ForReading) != S_OK)
			{
				return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
			}
			DXR(archive.Open(&file));
			return S_OK;
		}

		~ArchiveSource()
		{
			archive.Close();
			file.Close();
		}

		int Open(tchar const *name, FileBase **file) override
		{
			Ptr<Archive::File> f(new Archive::File());
			if(archive.Locate(name, *f) != Archive::ok)
			{
				return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
			}
			*file = f.release();
			return S_OK;
		}

		DiskFile file;
		Archive archive;
	};

	namespace AssetManager
	{
		void Close()
		{
			while(!sources.empty())
			{
				AssetSource *s = sources.back();
				sources.pop_back();
				delete s;
			}
		}

		int AddFolder(tchar const *folderName)
		{
			Ptr<FolderSource> f(new FolderSource());
			DXR(f->Create(folderName));
			sources.push_back(f.release());
			return S_OK;
		}

		int AssetManager::AddArchive(tchar const *archiveName)
		{
			ArchiveSource *a = new ArchiveSource();
			int r = a->Create(archiveName);
			if(r == S_OK)
			{
				sources.push_back(a);
			}
			else
			{
				delete a;
			}
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
			return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
		}

		int LoadFile(tchar const *filename, FileResource &data)
		{
			FileBase *file;
			DXR(Open(filename, &file));
			Ptr<FileBase> filep(file); // To make it close it
			DXR(file->GetSize(data.Size()));
			Ptr<byte> mem(new byte[data.Size()]);
			size_t got;
			DXR(file->Read(mem.get(), data.Size(), &got));
			if(got != data.Size())
			{
				return HRESULT_FROM_WIN32(ERROR_READ_FAULT);
			}
			data.Data() = mem.release();
			return S_OK;
		}
	}
}
