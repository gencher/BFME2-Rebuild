// BfmeStringTailRecord180.h: a 180-byte record the scripted model draw module keeps in a vector.
// Target facts (BfmeStringTailRecord180 constructor): a string at +0x00, an int vector at +0x10 and two
// ModelConditionFlags at +0x1C and +0x68. Its destructor releases the vector and then the string.
#pragma once
#ifndef BFME2_BFMESTRINGTAILRECORD180_H
#define BFME2_BFMESTRINGTAILRECORD180_H

#include <vector>
#include "Common/GameType.h"
#include "Common/AsciiString.h"
#include "Common/ModelConditionFlags.h"

struct BfmeStringTailRecord180
{
	BfmeStringTailRecord180();

	AsciiString m_name;						// 0x00
	Byte m_unk0x04[0xC];					// 0x04
	std::vector<Int> m_values;				// 0x10
	ModelConditionFlags m_conditionFlags;	// 0x1C
	ModelConditionFlags m_unk0x68;			// 0x68
};
typedef char BfmeStringTailRecord180Size[(sizeof(BfmeStringTailRecord180) == 0xB4) ? 1 : -1];

#endif
