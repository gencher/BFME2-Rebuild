// Portions derived from Command & Conquer Generals and Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
// CommandLine.h: the command-line parser (Zero Hour's declaration) and the block of switch
// settings that parseCommandLine checksums.
#pragma once
#ifndef _COMMAND_LINE_H_
#define _COMMAND_LINE_H_

void parseCommandLine(int argc, char *argv[]);	///< Parse command-line parameters

// The settings block retail keeps in .data at 0x00DC1170..0x00DC1187: parseCommandLine 0x003B9FD3 ends
// with a CRC of exactly these 0x18 bytes (push 0; push 0x18; push 0xDC1170; call 0x003EC8F7) and stores
// the result in TheWritableGlobalData (+0xB38). Its zero members sit in .data among the -1 ones,
// which only an aggregate gives (separate zero-initialised globals go to .bss).
struct CommandLineCRCSettings
{
	unsigned int m_flags;			// +0x00 0x00DC1170: one bit per switch (-noMusic 0x2, -noAudio ...)
	int m_spareA;					// +0x04 0x00DC1174: 0, no reader or writer in retail
	int m_debugCRCFrame;			// +0x08 0x00DC1178: the frame both debug-CRC switches store
	int m_spareB;					// +0x0C 0x00DC117C: -1, no reader or writer in retail
	int m_netCRCInterval;			// +0x10 0x00DC1180: copy of -netCRCInterval
	int m_crcValue;					// +0x14 0x00DC1184: the second word of the CRC value pair
};
typedef char CommandLineCRCSettings_size[(sizeof(CommandLineCRCSettings) == 0x18) ? 1 : -1];

extern CommandLineCRCSettings TheCommandLineCRCSettings;

// The CRC debugging switches (the game report lists the ones that are set).
extern int TheDebugCRCFromFrame;
extern int TheDebugCRCUntilFrame;
extern bool TheXObjectCRC;
extern bool TheXPartitionCRC;
extern bool TheXCollisionCRC;
extern bool TheXShroudCRC;
extern bool TheXTaintCRC;
extern bool TheXTerrainLogicCRC;
extern bool TheXPlayerCRC;
extern bool TheXAICRC;
extern unsigned char BfmeClientCRCCheckEnabled;
extern bool TheDeepCRC;
extern bool TheLiteCRC;
extern bool TheXLWCRC;
extern bool TheBinaryDeepCRC;

#endif // _COMMAND_LINE_H_
