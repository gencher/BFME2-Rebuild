// W3DTornadoDrawModuleData.h: the INI data of W3DTornadoDraw. SIZE 0x44.
// Target facts: ModuleFactory's friend_newModuleData allocates 0x44 bytes and runs the constructor, which builds the
// RadiusDecalTemplate at +0x8 and sets DecalCount to 0 and DecalMaxRadius to 200.0; the destructor destroys the template.
// The table overrides only slot 0 of ModuleData's. buildFieldParse adds the three-entry table (INI tokens below).
#pragma once
#ifndef BFME2_W3D_TORNADO_DRAW_MODULE_DATA_H
#define BFME2_W3D_TORNADO_DRAW_MODULE_DATA_H

#include <stddef.h>
#include "Common/GameType.h"
#include "Common/RadiusDecalTemplate.h"
#include "GameLogic/Module/ModuleData.h"

class MultiIniFieldParse;

class W3DTornadoDrawModuleData : public ModuleData
{
public:
	W3DTornadoDrawModuleData();
	virtual ~W3DTornadoDrawModuleData();
	static void buildFieldParse(MultiIniFieldParse &parse);

	RadiusDecalTemplate m_decalTemplate;		// 0x08 DecalTemplate (RadiusDecalTemplate::parseRadiusDecalTemplate)
	UnsignedInt m_decalCount;					// 0x3C DecalCount (parseUnsignedInt)
	Real m_decalMaxRadius;						// 0x40 DecalMaxRadius (parseReal)
};
// SIZE 0x44
typedef char W3DTornadoDrawModuleData_size[(sizeof(W3DTornadoDrawModuleData) == 0x44) ? 1 : -1];
typedef char W3DTornadoDrawModuleData_off_decalTemplate[(offsetof(W3DTornadoDrawModuleData, m_decalTemplate) == 0x08) ? 1 : -1];
typedef char W3DTornadoDrawModuleData_off_decalCount[(offsetof(W3DTornadoDrawModuleData, m_decalCount) == 0x3C) ? 1 : -1];
typedef char W3DTornadoDrawModuleData_off_decalMaxRadius[(offsetof(W3DTornadoDrawModuleData, m_decalMaxRadius) == 0x40) ? 1 : -1];

#endif
