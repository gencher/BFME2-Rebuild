// W3DQuadrupedDrawModuleData.h: the INI data of W3DQuadrupedDraw. SIZE 0x198.
// Target facts: friend_newModuleData allocates 0x198 bytes and runs the constructor; buildFieldParse calls
// W3DModelDrawModuleData::buildFieldParse, then adds the four-entry table of foot bone names (parseAsciiString).
// The constructor and destructor are comdat rows (inline here): the constructor calls W3DModelDrawModuleData's and
// builds the four AsciiStrings with the eh vector constructor iterator (count 4, size 4); the destructor runs the eh vector
// destructor iterator over them before W3DModelDrawModuleData's destructor. That is one array, in INI token order.
#pragma once
#ifndef BFME2_W3D_QUADRUPED_DRAW_MODULE_DATA_H
#define BFME2_W3D_QUADRUPED_DRAW_MODULE_DATA_H

#include <stddef.h>
#include "Common/GameType.h"
#include "Common/AsciiString.h"
#include "GameLogic/Module/W3DModelDrawModuleData.h"

class MultiIniFieldParse;

// Base W3DModelDrawModuleData ends at 0x188; this class's members follow it.
typedef char W3DQuadrupedDrawModuleData_base_size[(sizeof(W3DModelDrawModuleData) == 0x188) ? 1 : -1];

class W3DQuadrupedDrawModuleData : public W3DModelDrawModuleData
{
public:
	W3DQuadrupedDrawModuleData() {}
	virtual ~W3DQuadrupedDrawModuleData() {}
	static void buildFieldParse(MultiIniFieldParse &parse);

	// 0x188 LeftFrontFootBone, 0x18C RightFrontFootBone, 0x190 LeftRearFootBone, 0x194 RightRearFootBone
	AsciiString m_footBones[4];
};
// SIZE 0x198
typedef char W3DQuadrupedDrawModuleData_size[(sizeof(W3DQuadrupedDrawModuleData) == 0x198) ? 1 : -1];
typedef char W3DQuadrupedDrawModuleData_off_m_footBones[(offsetof(W3DQuadrupedDrawModuleData, m_footBones) == 0x188) ? 1 : -1];

#endif
