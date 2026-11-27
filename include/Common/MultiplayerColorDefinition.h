// MultiplayerColorDefinition: one multiplayer colour entry (0x40 bytes, no vtable). Layout from the
// constructor 0x00380C4E (defaults: the colour words and RGB triples set to -1 through
// RGBColor::setFromInt, the night RGB copied from the day RGB), setColor 0x00380C1E and
// setNightColor 0x00380C36 (the colour word stored with alpha 0xFF).
#pragma once
#ifndef BFME2_MULTIPLAYERCOLORDEFINITION_H
#define BFME2_MULTIPLAYERCOLORDEFINITION_H

#include <stddef.h>
#include "Common/GameType.h"
#include "Common/AsciiString.h"
#include "Common/RGBColor.h"

struct FieldParse;

class MultiplayerColorDefinition
{
public:
	MultiplayerColorDefinition();														// 0x00380C4E
	MultiplayerColorDefinition(const MultiplayerColorDefinition &that);					// 0x00380D30
	MultiplayerColorDefinition *operator=(const MultiplayerColorDefinition &that);		// 0x00380CAE
	void setColor(RGBColor color);														// 0x00380C1E
	void setNightColor(RGBColor color);													// 0x00380C36
	RGBColor getRGBValue() const;	// 0x001EF34E: +0x04 by value
	RGBColor getRGBNightValue() const;	// 0x00098516: +0x14 by value
	Int getColor() const { return m_color; }
	Int getNightColor() const { return m_nightColor; }
	AsciiString getTooltipName() const;												// 0x002E4336: a copy of the tooltip name

	static const FieldParse m_fieldParseTable[];	// 0x00C18F40: the INI block's keys

	AsciiString m_tooltipName;					// +0x00
	RGBColor m_rgbColor;						// +0x04
	Int m_color;								// +0x10, the colour word (setColor)
	RGBColor m_rgbNightColor;					// +0x14
	Int m_nightColor;							// +0x20, the night colour word (setNightColor)
	RGBColor m_livingWorldColor;				// +0x24
	RGBColor m_livingWorldBannerColor;			// +0x30
	Bool m_availableInWotR;						// +0x3C
};
// SIZE 0x40
typedef char MultiplayerColorDefinitionSize[(sizeof(MultiplayerColorDefinition) == 0x40) ? 1 : -1];

#endif
