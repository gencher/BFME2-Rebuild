// AsciiStringRef.h: the pieces of a string concatenation expression that know how to write themselves out.
// Target facts: AsciiStringRef is one pointer to an AsciiString; write copies the string's characters to the
// buffer and returns the length (INI.cpp). AsciiStringPlusString is such a reference followed by a second one
// (its write appends the second after the first).
#pragma once
#ifndef BFME2_ASCIISTRINGREF_H
#define BFME2_ASCIISTRINGREF_H

#include "Common/AsciiString.h"

struct AsciiStringRef
{
	int write(char *dst);			// 0x0002C5B1

	const AsciiString *m_string;	// 0x00
};

struct AsciiStringPlusString : AsciiStringRef
{
	int write(char *dst);			// 0x000BBD1C

	AsciiStringRef m_second;		// 0x04
};

#endif
