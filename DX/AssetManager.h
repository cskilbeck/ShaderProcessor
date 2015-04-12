#pragma once

namespace DX
{
	struct AssetSource;

	// Only one AssetManager for now

	namespace AssetManager
	{
		int AddFolder(tchar const *folderName);
		int AddArchive(tchar const *archiveName);

		void Close();

		int Open(tchar const *filename, FileBase **file);
		bool FileExists(tchar const *filename);
		int LoadFile(tchar const *filename, FileResource &data);
	};
}
