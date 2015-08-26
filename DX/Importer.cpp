#include "DX.h"

#include "assimp/include/assimp/IOSystem.hpp"
#include "assimp/include/assimp/IOStream.hpp"

using namespace Assimp;

namespace 
{
	class MyIOStream: public Assimp::IOStream
	{
	public:
		~MyIOStream()
		{
			Delete(file);
		}

		size_t Read(void* pvBuffer, size_t pSize, size_t pCount) override
		{
			uint64 g;
			if(file->Read(pvBuffer, pSize * pCount, &g) != S_OK)
			{
				return 0;
			}
			return g / pSize;
		}

		size_t Write(const void* pvBuffer, size_t pSize, size_t pCount) override
		{
			return 0;
		}

		aiReturn Seek(size_t pOffset, aiOrigin pOrigin) override
		{
			return aiReturn_FAILURE;
		}

		size_t Tell() const override
		{
			return 0;
		}

		size_t FileSize() const override
		{
			uint64 size;
			if(file->GetSize(size) == S_OK)
			{
				return size;
			}
			return 0;
		}

		void Flush() override
		{
		}
	private:
		friend class MyIOSystem;

		MyIOStream()
			: file(null)
		{
		}

		FileBase *file;
	};

	class MyIOSystem: public Assimp::IOSystem
	{
	public:
		MyIOSystem()
		{
		}

		~MyIOSystem()
		{
		}

		void Close(Assimp::IOStream *stream) override
		{
			MyIOStream *s = (MyIOStream *)stream;
			FileBase * &f = s->file;
			Delete(f);
		}

		bool ComparePaths(const char *one, const char *second) const override
		{
			return _stricmp(one, second) == 0;
		}

		bool ComparePaths(std::string const &one, std::string const &two) const
		{
			return ComparePaths(one.c_str(), two.c_str());
		}

		bool Exists(char const *pFile) const override
		{
			tstring s = DX::TString(pFile);
			return DX::AssetManager::FileExists(s.c_str());
		}

		bool Exists(string const &str) const
		{
			return Exists(str.c_str());
		}

		char getOsSeparator() const override
		{
			return '/';
		}

		Assimp::IOStream *Open(const char *f, const char *mode = "rb") override
		{
			if(_stricmp(mode, "rb") != 0)
			{
				return null;
			}
			MyIOStream *s = new MyIOStream();
			if(AssetManager::Open(f, &s->file) != S_OK)
			{
				Delete(s);
				return null;
			}
			return s;
		}

		Assimp::IOStream *Open(string const &f, string const &mode)
		{
			return Open(f.c_str(), mode.c_str());
		}
	};

	aiNode *FindTheNode(aiNode *cur, char const *name)
	{
		if(_stricmp(cur->mName.C_Str(), name) == 0)
		{
			return cur;
		}
		for(uint i = 0; i < cur->mNumChildren; ++i)
		{
			aiNode *s = FindTheNode(cur->mChildren[i], name);
			if(s != null)
			{
				return s;
			}
		}
		return null;
	}

}

namespace DX
{
	int LoadScene(tchar const *filename, aiScene const **scene)
	{
		if(scene == null || filename == null)
		{
			return E_POINTER;
		}

		Assimp::Importer *importer = new Assimp::Importer();
		importer->SetIOHandler(new MyIOSystem());
		importer->SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT);

#if defined(_DEBUG)
		DefaultLogger::create("", Logger::VERBOSE, aiDefaultLogStream_DEBUGGER);
#endif
		importer->ReadFile(filename, aiProcess_Triangulate | aiProcess_SortByPType);

#if defined(_DEBUG)
		DefaultLogger::kill();
#endif
		if(importer->GetScene() == null)
		{
			TRACE("Error loading %s: %s\n", filename, importer->GetErrorString());
			return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
		}
		*scene = importer->GetScene();
		return S_OK;
	}

	aiNode *FindNode(aiScene const *scene, char const *name)
	{
		return FindTheNode(scene->mRootNode, name);
	}
}
