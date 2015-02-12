//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	void AsciiToWide(char const *input, wchar *output);
	bool UTF8ToWide(uint8 *input, size_t inputSize, size_t *decodedLength, wchar *decodeBuffer);
	wstring WideStringFromUTF8(char const *input);
	string AsciiStringFromWide(wchar const *input);

	//////////////////////////////////////////////////////////////////////

	//extern const uint8 utf8d[];

	struct UTF8Decoder
	{
		wchar			unic;
		char const *	ptr;
		char const *	currentPtr;
		size_t			inputSize;
		uint8			stat;

		UTF8Decoder(char const *input, size_t inputBytesLength = -1)
		{
			stat = 9;
			unic = 0;
			ptr = input;
			inputSize = inputBytesLength;
		}

		// returns: 0 = end of stream, 0xffffffff = decode error, else decoded unicode character
		// calling Next() after it has returned 0 or -1 is undefined (crash)
		wchar Next();
	};
}