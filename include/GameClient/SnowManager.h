// SnowManager: the weather subsystem both the 3D snow and the (absent) dummy factory derive from. Target facts (retail game.dat): the constructor
// 0x0020109B runs SubsystemInterface's constructor 0x001B4E63, stores the table VA 0x00BE300C and seeds +0x0C..+0x73 (a buffer pointer at +0x0C that init
// 0x00201064 allocates with 0x4000 bytes, floats at +0x10..+0x3C with 1.0f at +0x14, a byte 1 at +0x40, byte 0 at +0x41 and +0x50, ints at +0x44/+0x54/+0x58,
// floats at +0x48/+0x4C/+0x5C..+0x70). Slots: 0 the deleting destructor 0x002015FC, 1 init 0x00201064, 9 reset 0x00201134, 10 update (0x0003B810, the shared
// __purecall), 14 updateIniSettings 0x00201165 (both init and reset tail-jump through it). SIZE 0x74. The members are not recovered beyond that.
#pragma once
#ifndef BFME2_SNOWMANAGER_H
#define BFME2_SNOWMANAGER_H

#include "Common/SubsystemInterface.h"
#include "Common/Coord2D.h"

class SnowManager : public SubsystemInterface
{
public:
	SnowManager();
	virtual ~SnowManager();													// slot 0, 0x002015FC
	virtual void init();													// slot 1, 0x00201064
	virtual void reset();													// slot 9, 0x00201134
	virtual void update() = 0;												// slot 10 (the shared __purecall)
	virtual void updateIniSettings();										// slot 14, 0x00201165

	Real *m_startingHeights;												// +0x0C the 64 by 64 float height field init allocates (0x4000 bytes; Zero Hour names it m_startingHeights)
	Real m_time;															// +0x10 zeroed by init (Zero Hour: m_time)
	Real m_unk0x14;															// +0x14 (1.0f)
	Real m_unk0x18[10];														// +0x18..0x3F
	Bool m_unk0x40;															// +0x40 (1)
	Bool m_unk0x41;															// +0x41
	Int m_unk0x44;															// +0x44
	Coord2D m_xySpeed;														// copied from WeatherSetting::m_snowXYSpeed
	Bool m_unk0x50;															// +0x50
	Int m_unk0x54;															// +0x54
	Int m_unk0x58;															// +0x58
	Real m_unk0x5C[6];														// +0x5C..0x73
};
typedef char SnowManagerSize[(sizeof(SnowManager) == 0x74) ? 1 : -1];

extern SnowManager *TheSnowManager;

#endif
