//////////////////////////////////////////////////////////////////////

#include "DXBase.h"

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	int RegistryKey::Open(HKEY key, tchar const *subkey, REGSAM access /* = KEY_READ */)
	{
		int r = (int)RegOpenKeyEx(key, subkey, 0, access, &mKey);
		return r;
	}

	//////////////////////////////////////////////////////////////////////

	void RegistryKey::Close()
	{
		mKey.Close();
	}

	//////////////////////////////////////////////////////////////////////

	int RegistryKey::GetStringValue(tchar const *name, tstring &value)
	{
		DWORD type;
		DWORD size;
		SXR(RegQueryValueEx(mKey, name, null, &type, null, &size));
		if(type != REG_SZ)
		{
			return ERROR_BAD_FORMAT;
		}
		Ptr<tchar> buffer;
		buffer.reset(new tchar[size]);
		SXR(RegQueryValueEx(mKey, name, null, null, (byte *)buffer.get(), &size));
		value = buffer.get();
		return ERROR_SUCCESS;
	}

	//////////////////////////////////////////////////////////////////////

	int RegistryKey::SetStringValue(tchar const *name, tstring &value)
	{
		SXR(RegSetValueEx(mKey, name, 0, REG_SZ, (byte *)value.data(), (DWORD)value.size()));
		return ERROR_SUCCESS;
	}

	//////////////////////////////////////////////////////////////////////

	int RegistryKey::GetDWORDValue(tchar const *name, uint32 &value)
	{
		DWORD type;
		SXR(RegQueryValueEx(mKey, name, null, &type, null, null));
		if(type != REG_DWORD)
		{
			return ERROR_BAD_FORMAT;
		}
		SXR(RegQueryValueEx(mKey, name, null, null, (byte *)&value, null));
		return ERROR_SUCCESS;
	}

	//////////////////////////////////////////////////////////////////////

	int RegistryKey::SetDWORDValue(tchar const *name, uint32 value)
	{
		SXR(RegSetValueEx(mKey, name, 0, REG_SZ, (byte *)&value, sizeof(value)));
		return ERROR_SUCCESS;
	}

	//////////////////////////////////////////////////////////////////////

	int RegistryKey::GetString(HKEY key, tchar const *keyname, tchar const *valuename, tstring &value)
	{
		RegistryKey bob;
		SXR(bob.Open(key, keyname));
		tstring path;
		SXR(bob.GetStringValue(valuename, value));
		return ERROR_SUCCESS;
	}

	//////////////////////////////////////////////////////////////////////

	int RegistryKey::GetDWORD(HKEY key, tchar const *keyname, tchar const *valuename, uint32 &value)
	{
		RegistryKey bob;
		SXR(bob.Open(key, keyname));
		tstring path;
		SXR(bob.GetDWORDValue(valuename, value));
		return ERROR_SUCCESS;
	}
}
