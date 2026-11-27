// DynamicShroudClearingRangeUpdateModuleData.h: the INI data of the shroud
// clearing module. SIZE 0x5C.
// Target facts: constructor 0x0048B5C6 (vtable 0x00C4BEF8, then the decal template
// at +0x28, then the six interval words and the final-vision float) and destructor
// 0x0048B634 (tears +0x28 down and restores the Snapshot table). The factory
// news 0x5C with that constructor as sole caller.
#pragma once
#ifndef BFME2_DYNAMICSHROUDCLEARINGRANGEUPDATEMODULEDATA_H
#define BFME2_DYNAMICSHROUDCLEARINGRANGEUPDATEMODULEDATA_H

#include "Common/GameType.h"
#include "GameLogic/Module/ModuleData.h"
#include "Common/RadiusDecalTemplate.h"

class DynamicShroudClearingRangeUpdateModuleData : public ModuleData
{
public:
	DynamicShroudClearingRangeUpdateModuleData();

	const RadiusDecalTemplate &getGridDecalTemplate() const { return m_gridDecalTemplate; }
	static void buildFieldParse(MultiIniFieldParse &parse);
	static ModuleData *friend_newModuleData(INI *ini);

	UnsignedInt m_shrinkDelay;			// 0x08
	UnsignedInt m_shrinkTime;			// 0x0C
	UnsignedInt m_growDelay;			// 0x10
	UnsignedInt m_growTime;				// 0x14
	Real m_finalVision;					// 0x18
	UnsignedInt m_changeInterval;		// 0x1C
	UnsignedInt m_growInterval;			// 0x20
	Bool m_doSpySatFX;					// 0x24
	unsigned char m_pad0x25[3];			// 0x25
	RadiusDecalTemplate m_gridDecalTemplate;	// 0x28
};
// SIZE 0x5C

#endif
