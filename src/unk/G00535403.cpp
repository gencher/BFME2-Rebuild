// cl: /O1 /EHsc
// Portions derived from Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
#include "Common/AsciiString.h"
#include "Common/UnicodeString.h"
namespace Msvcr71 {
	extern "C" __declspec(dllimport) __declspec(nothrow) int __cdecl isalnum(int);
}
// Generated from the ledger; map snapshot c591f7146f88c8a8248ebb8ae22f0c978c22aed463db64e05ef57ff0dfae99cc
// object=G00535403 name-evidence=invented
// Extents and edges are hypotheses; byte verification alone does not establish original TU boundaries.
// FUNCTION: BFME2 0x00535403 size=25 name=?intToHexDigit@@YADH@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
static char intToHexDigit(int num)
{
	if (num < 0 || num > 15)
		return '\0';
	if (num < 10)
		return (char)('0' + num);
	return (char)('A' + (num - 10));
}
// FUNCTION: BFME2 0x0053541C size=48 name=?hexDigitToInt@@YAHD@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
static int hexDigitToInt(char c)
{
	if (c <= '9' && c >= '0')
		return (c - '0');
	if (c <= 'f' && c >= 'a')
		return (c - 'a' + 10);
	if (c <= 'F' && c >= 'A')
		return (c - 'A' + 10);
	return 0;
}
// FUNCTION: BFME2 0x0053544C size=250 name=?UnicodeStringToQuotedPrintable@@YA?AVAsciiString@@VUnicodeString@@@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
AsciiString UnicodeStringToQuotedPrintable(UnicodeString original)
{
	static char dest[1024];
	const char *src = (const char *)original.str();
	int i = 0;
	while (!(src[0] == '\0' && src[1] == '\0') && i < 1021)
	{
		if (!Msvcr71::isalnum(*src))
		{
			dest[i++] = '_';
			dest[i++] = intToHexDigit(((unsigned char)*src) >> 4);
			dest[i++] = intToHexDigit((*src) & 0xf);
		}
		else
		{
			dest[i++] = *src;
		}
		src++;
		if (!Msvcr71::isalnum(*src))
		{
			dest[i++] = '_';
			dest[i++] = intToHexDigit(((unsigned char)*src) >> 4);
			dest[i++] = intToHexDigit((*src) & 0xf);
		}
		else
		{
			dest[i++] = *src;
		}
		src++;
	}
	dest[i] = '\0';
	return dest;
}
// FUNCTION: BFME2 0x00535546 size=172 name=?AsciiStringToQuotedPrintable@@YA?AVAsciiString@@V1@@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
AsciiString AsciiStringToQuotedPrintable(AsciiString original)
{
	static char dest[1024];
	const char *src = original.str();
	int i = 0;
	while (*src != '\0' && i < 1021)
	{
		if (!Msvcr71::isalnum(*src))
		{
			dest[i++] = '_';
			dest[i++] = intToHexDigit(((unsigned char)*src) >> 4);
			dest[i++] = intToHexDigit((*src) & 0xf);
		}
		else
		{
			dest[i++] = *src;
		}
		src++;
	}
	dest[i] = '\0';
	return dest;
}
// FUNCTION: BFME2 0x005355F2 size=205 name=?QuotedPrintableToUnicodeString@@YA?AVUnicodeString@@VAsciiString@@@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x005356BF size=163 name=?QuotedPrintableToAsciiString@@YA?AVAsciiString@@V1@@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
