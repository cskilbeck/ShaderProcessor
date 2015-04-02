#pragma once

namespace DX
{
	struct AssetSource;

	struct AssetManager
	{
		AssetManager();
		~AssetManager();

		bool AddFolder(tchar const *folderName);
		bool AddArchive(tchar const *archiveName);

		bool Open(tchar const *filename, FileBase **file);

	private:

		std::list<AssetSource *> sources;
	};
}
