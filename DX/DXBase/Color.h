//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	struct Color
	{
		uint32 mColor;

		enum
		{
			kAlphaOffset = 24,
			kBlueOffset = 16,
			kGreenOffset = 8,
			kRedOffset = 0
		};

		enum
		{
			kAlphaMask = 0xff << kAlphaOffset,
			kRedMask = 0xff << kRedOffset,
			kGreenMask = 0xff << kGreenOffset,
			kBlueMask = 0xff << kBlueOffset,
		};

		Color()
		{
		}

		Color(uint32 const c)
			: mColor(c)
		{
		}

		operator bool() const
		{
			return mColor != 0;
		}

		operator uint32() const
		{
			return mColor;
		}

		operator Byte4() const
		{
			return (Byte4)mColor;
		}

		Color(Color const &other)
			: mColor(other.mColor)
		{
		}

		Color(Byte r, Byte g, Byte b, Byte a = 0xff)
		{
			mColor =
				(uint32)a << kAlphaOffset |
				(uint32)r << kRedOffset |
				(uint32)g << kGreenOffset |
				(uint32)b << kBlueOffset;
		}

		Color(float *bgra)
		{
			uint32 b = (uint32)(bgra[0] * 255.0f);
			uint32 g = (uint32)(bgra[1] * 255.0f);
			uint32 r = (uint32)(bgra[2] * 255.0f);
			uint32 a = (uint32)(bgra[3] * 255.0f);
			mColor =
				(uint32)a << kAlphaOffset |
				(uint32)r << kRedOffset |
				(uint32)g << kGreenOffset |
				(uint32)b << kBlueOffset;
		}

		static inline Color Random()
		{
			static DX::Random r;
			return Color(r.Next() & 0xff, r.Next() & 0xff, r.Next() & 0xff);
		}

		float *GetFloatsRGBA(float *f) const
		{
			f[0] = Red() / 255.0f;
			f[1] = Green() / 255.0f;
			f[2] = Blue() / 255.0f;
			f[3] = Alpha() / 255.0f;
			return f;
		}

		Color &operator = (Color o)
		{
			mColor = o.mColor;
			return *this;
		}

		Color &operator = (uint32 const c)
		{
			mColor = c;
			return *this;
		}

		Color &operator = (int const c)
		{
			mColor = (uint32)c;
			return *this;
		}

		uint Alpha() const
		{
			return (mColor >> kAlphaOffset) & 0xff;
		}

		uint Red() const
		{
			return (mColor >> kRedOffset) & 0xff;
		}

		uint Green() const
		{
			return (mColor >> kGreenOffset) & 0xff;
		}

		uint Blue() const
		{
			return (mColor >> kBlueOffset) & 0xff;
		}

		Color &SetAlpha(Byte a)
		{
			mColor = (mColor & ~kAlphaMask) | ((uint32)a << kAlphaOffset);
			return *this;
		}

		Color &SetRed(Byte r)
		{
			mColor = (mColor & ~kRedMask) | ((uint32)r << kRedOffset);
			return *this;
		}

		Color &SetGreen(Byte g)
		{
			mColor = (mColor & ~kGreenMask) | ((uint32)g << kGreenOffset);
			return *this;
		}

		Color &SetBlue(Byte b)
		{
			mColor = (mColor & ~kBlueMask) | ((uint32)b << kBlueOffset);
			return *this;
		}

		inline Color Lerp(Color const &other, uint lerp) const		// 0 = this, 256 = other, 128 = 50:50
		{
			int inv = 256 - lerp;
			return Color((Alpha()	* inv + other.Alpha()	* lerp) >> 8,
						 (Red()		* inv + other.Red()		* lerp) >> 8,
						 (Green()	* inv + other.Green()	* lerp) >> 8,
						 (Blue()	* inv + other.Blue()	* lerp) >> 8);
		}

		inline Color Lerp(Color const &other, float lerp) const
		{
			return Lerp(other, (uint)(lerp * 256.0f));
		}

		Color operator * (Color const &o)
		{
			return Color(Alpha()	* o.Alpha() >> 8,
						 Red()		* o.Red() >> 8,
						 Green()	* o.Green() >> 8,
						 Blue()		* o.Blue() >> 8);
		}

		enum: uint32
		{
			AliceBlue = 0xFFFFF8F0,
			AntiqueWhite = 0xFFD7EBFA,
			Aqua = 0xFFFFFF00,
			Aquamarine = 0xFFD4FF7F,
			Azure = 0xFFFFFFF0,
			Beige = 0xFFDCF5F5,
			Bisque = 0xFFC4E4FF,
			Black = 0xFF000000,
			BlanchedAlmond = 0xFFCDEBFF,
			BrightBlue = 0xFFFF0000,
			BrightGreen = 0xFF00FF00,
			BrightRed = 0xFF0000FF,
			BlueViolet = 0xFFE22B8A,
			Brown = 0xFF2A2AA5,
			BurlyWood = 0xFF87B8DE,
			CadetBlue = 0xFFA09E5F,
			Chartreuse = 0xFF00FF7F,
			Chocolate = 0xFF1E69D2,
			Coral = 0xFF507FFF,
			CornflowerBlue = 0xFFED9564,
			Cornsilk = 0xFFDCF8FF,
			Crimson = 0xFF3C14DC,
			Cyan = 0xFFFFFF00,
			DarkBlue = 0xFF8B0000,
			DarkCyan = 0xFF8B8B00,
			DarkGoldenrod = 0xFF0B86B8,
			DarkGray = 0xFFA9A9A9,
			DarkGreen = 0xFF006400,
			DarkKhaki = 0xFF6BB7BD,
			DarkMagenta = 0xFF8B008B,
			DarkOliveGreen = 0xFF2F6B55,
			DarkOrange = 0xFF008CFF,
			DarkOrchid = 0xFFCC3299,
			DarkRed = 0xFF00008B,
			DarkSalmon = 0xFF7A96E9,
			DarkSeaGreen = 0xFF8FBC8F,
			DarkSlateBlue = 0xFF8B3D48,
			DarkSlateGray = 0xFF4F4F2F,
			DarkTurquoise = 0xFFD1CE00,
			DarkViolet = 0xFFD30094,
			DeepPink = 0xFF9314FF,
			DeepSkyBlue = 0xFFFFBF00,
			DimGray = 0xFF696969,
			DodgerBlue = 0xFFFF901E,
			Firebrick = 0xFF2222B2,
			FloralWhite = 0xFFF0FAFF,
			ForestGreen = 0xFF228B22,
			Fuchsia = 0xFFFF00FF,
			Gainsboro = 0xFFDCDCDC,
			GhostWhite = 0xFFFFF8F8,
			Gold = 0xFF00D7FF,
			Goldenrod = 0xFF20A5DA,
			Gray = 0xFF808080,
			GreenYellow = 0xFF2FFFAD,
			Honeydew = 0xFFF0FFF0,
			HotPink = 0xFFB469FF,
			IndianRed = 0xFF5C5CCD,
			Indigo = 0xFF82004B,
			Ivory = 0xFFF0FFFF,
			Khaki = 0xFF8CE6F0,
			Lavender = 0xFFFAE6E6,
			LavenderBlush = 0xFFF5F0FF,
			LawnGreen = 0xFF00FC7C,
			LemonChiffon = 0xFFCDFAFF,
			LightBlue = 0xFFE6D8AD,
			LightCoral = 0xFF8080F0,
			LightCyan = 0xFFFFFFE0,
			LightGoldenrodYellow = 0xFFD2FAFA,
			LightGreen = 0xFF90EE90,
			LightGray = 0xFFD3D3D3,
			LightPink = 0xFFC1B6FF,
			LightSalmon = 0xFF7AA0FF,
			LightSeaGreen = 0xFFAAB220,
			LightSkyBlue = 0xFFFACE87,
			LightSlateGray = 0xFF998877,
			LightSteelBlue = 0xFFDEC4B0,
			LightYellow = 0xFFE0FFFF,
			Lime = 0xFF00FF00,
			LimeGreen = 0xFF32CD32,
			Linen = 0xFFE6F0FA,
			Magenta = 0xFFFF00FF,
			Maroon = 0xFF000080,
			MediumAquamarine = 0xFFAACD66,
			MediumBlue = 0xFFCD0000,
			MediumOrchid = 0xFFD355BA,
			MediumPurple = 0xFFDB7093,
			MediumSeaGreen = 0xFF71B33C,
			MediumSlateBlue = 0xFFEE687B,
			MediumSpringGreen = 0xFF9AFA00,
			MediumTurquoise = 0xFFCCD148,
			MediumVioletRed = 0xFF8515C7,
			MidnightBlue = 0xFF701919,
			MintCream = 0xFFFAFFF5,
			MistyRose = 0xFFE1E4FF,
			Moccasin = 0xFFB5E4FF,
			NavajoWhite = 0xFFADDEFF,
			Navy = 0xFF800000,
			OldLace = 0xFFE6F5FD,
			Olive = 0xFF008080,
			OliveDrab = 0xFF238E6B,
			Orange = 0xFF00A5FF,
			OrangeRed = 0xFF0045FF,
			Orchid = 0xFFD670DA,
			PaleGoldenrod = 0xFFAAE8EE,
			PaleGreen = 0xFF98FB98,
			PaleTurquoise = 0xFFEEEEAF,
			PaleVioletRed = 0xFF9370DB,
			PapayaWhip = 0xFFD5EFFF,
			PeachPuff = 0xFFB9DAFF,
			Peru = 0xFF3F85CD,
			Pink = 0xFFCBC0FF,
			Plum = 0xFFDDA0DD,
			PowderBlue = 0xFFE6E0B0,
			Purple = 0xFF800080,
			RosyBrown = 0xFF8F8FBC,
			RoyalBlue = 0xFFE16941,
			SaddleBrown = 0xFF13458B,
			Salmon = 0xFF7280FA,
			SandyBrown = 0xFF60A4F4,
			SeaGreen = 0xFF578B2E,
			SeaShell = 0xFFEEF5FF,
			Sienna = 0xFF2D52A0,
			Silver = 0xFFC0C0C0,
			SkyBlue = 0xFFEBCE87,
			SlateBlue = 0xFFCD5A6A,
			SlateGray = 0xFF908070,
			Snow = 0xFFFAFAFF,
			SpringGreen = 0xFF7FFF00,
			SteelBlue = 0xFFB48246,
			Tan = 0xFF8CB4D2,
			Teal = 0xFF808000,
			Thistle = 0xFFD8BFD8,
			Tomato = 0xFF4763FF,
			Transparent = 0x00000000,
			Turquoise = 0xFFD0E040,
			Violet = 0xFFEE82EE,
			Wheat = 0xFFB3DEF5,
			White = 0xFFFFFFFF,
			WhiteSmoke = 0xFFF5F5F5,
			Yellow = 0xFF00FFFF,
			YellowGreen = 0xFF32CD9A
		};

	};
}