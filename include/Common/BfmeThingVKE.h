// BfmeThingVKE.h: a 0x58-byte record: a string, a flag, four words and a 4x4 float block.
// Target facts: bfmeInitVKE constructs the string from the other record's, copies the flag (+0x04) and the
// four words (+0x08..+0x14), then block-copies 0x40 bytes from +0x18.
#pragma once
#ifndef BFME2_BFMETHINGVKE_H
#define BFME2_BFMETHINGVKE_H

#include "Common/GameType.h"
#include "Common/AsciiString.h"

struct BfmeThingVKEBlock
{
	Real m_values[16];
};

class BfmeThingVKE
{
public:
	BfmeThingVKE(const BfmeThingVKE &other);

	AsciiString m_name;		// 0x00
	Bool m_flag;			// 0x04
	Int m_word08;			// 0x08
	Int m_word0C;			// 0x0C
	Int m_word10;			// 0x10
	Int m_word14;			// 0x14
	BfmeThingVKEBlock m_block;	// 0x18
};
typedef char BfmeThingVKESize[(sizeof(BfmeThingVKE) == 0x58) ? 1 : -1];

#endif
