//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	struct RegistryKey
	{
		int Open(HKEY key, tchar const *subkey, REGSAM access = KEY_READ);
		void Close();
		int GetStringValue(tchar const *name, tstring &value);
		int SetStringValue(tchar const *name, tstring &value);
		int GetDWORDValue(tchar const *name, uint32 &value);
		int SetDWORDValue(tchar const *name, uint32 value);

		static int GetString(HKEY key, tchar const *keyname, tchar const *valuename, tstring &value);
		static int GetDWORD(HKEY key, tchar const *keyname, tchar const *valuename, uint32 &value);

		RegKey mKey;
	};
}
