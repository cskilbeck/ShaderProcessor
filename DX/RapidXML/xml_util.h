//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace
{
	//////////////////////////////////////////////////////////////////////

	using namespace rapidxml;
	using namespace DX;

	//////////////////////////////////////////////////////////////////////

	typedef xml_document<wchar> xml_doc;
	typedef xml_node<wchar> node;
	typedef xml_attribute<wchar> attribute;

	//////////////////////////////////////////////////////////////////////

	inline wchar const *GetValue(node *node, wchar const *name)
	{
		attribute *a = node->first_attribute(name, 0, false);
		return (a != null) ? a->value() : null;
	}

	//////////////////////////////////////////////////////////////////////

	inline float GetFloat(node *node, wchar const *name)
	{
		wchar const *value = GetValue(node, name);
		wchar *stop;
		return value != null ? (float)wcstod(value, &stop) : 0;
	}

	//////////////////////////////////////////////////////////////////////

	inline int GetInt(node *node, wchar const *name)
	{
		wchar const *value = GetValue(node, name);
		wchar *stop;
		return value != null ? (int)wcstol(value, &stop, 10) : 0;
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec2f GetVec2f(node *node, wchar const *x, wchar const *y)
	{
		return Vec2f(GetFloat(node, x), GetFloat(node, y));
	}

	//////////////////////////////////////////////////////////////////////

	inline uint32 GetHex32(node *node, wchar const *name)
	{
		uint32 number = 0;
		wchar const *value = GetValue(node, name);
		if(value != null)
		{
			swscanf_s(value, L"%8x", &number);
		}
		return number;
	}

	//////////////////////////////////////////////////////////////////////

	bool LoadUTF8File(TCHAR const *filename, wstring &str)
	{
		bool rc = false;
		FileResource f;
		if(AssetManager::LoadFile(filename, f) == S_OK)
		{
			size_t len;
			if(UTF8ToWide((byte *)f.Data(), (size_t)f.Size(), &len, null) && len > 0)
			{
				str.resize(len + 1);
				UTF8ToWide((byte *)f.Data(), (size_t)f.Size(), &len, &str[0]);
				str[len] = '\0';
				rc = true;
			}
			else
			{
				assert(false);
			}
		}
		return rc;
	}

	//////////////////////////////////////////////////////////////////////

	xml_doc *LoadUTF8XMLFile(TCHAR const *filename, wstring &str)
	{
		xml_doc *doc = null;
		if(LoadUTF8File(filename, str))
		{
			doc = new xml_doc();
			try
			{
				doc->parse<0>(&str[0]);
			}
			catch(parse_error e)
			{
				Delete(doc);
			}
		}
		return doc;
	}

} // namespace

