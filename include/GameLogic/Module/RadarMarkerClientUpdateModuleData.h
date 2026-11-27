// RadarMarkerClientUpdateModuleData.h: the INI data of RadarMarkerClientUpdate. SIZE 0xC.
// Target facts: ModuleFactory registers the module with operator new 0xC and
// constructor RVA 0x004C9C98; buildFieldParse RVA 0x004C9BBC adds VA 0x00C5ED98 at +0x0.
// Offsets are the table entries'; each type is what the entry's parse function stores
// (INI::parseReal -> Real, parseDurationUnsignedInt -> UnsignedInt, ...);
// names are the INI tokens. Byte gaps are bytes no table entry writes.
#pragma once
#ifndef BFME2_RADAR_MARKER_CLIENT_UPDATE_MODULE_DATA_H
#define BFME2_RADAR_MARKER_CLIENT_UPDATE_MODULE_DATA_H

#include <stddef.h>
#include "Common/GameType.h"
#include "GameLogic/Module/ModuleData.h"
#include "Common/AsciiString.h"

class INI;
class MultiIniFieldParse;

class RadarMarkerClientUpdateModuleData : public ModuleData
{
public:
	RadarMarkerClientUpdateModuleData();
	virtual ~RadarMarkerClientUpdateModuleData();
	static void buildFieldParse(MultiIniFieldParse &parse);
	static ModuleData *friend_newModuleData(INI *ini);

	AsciiString m_markerType;                              // +0x8 MarkerType
};
// SIZE 0xC
typedef char RadarMarkerClientUpdateModuleData_size[(sizeof(RadarMarkerClientUpdateModuleData) == 0xC) ? 1 : -1];
typedef char RadarMarkerClientUpdateModuleData_off_m_markerType[(offsetof(RadarMarkerClientUpdateModuleData, m_markerType) == 0x8) ? 1 : -1];

#endif
