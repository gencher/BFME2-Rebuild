// cl: /O1
// dxsdk
// Portions derived from Command & Conquer Generals and Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
#include <stdlib.h>
#include "Common/GlobalData.h"
#include "WW3D2/dxwrapper.h"
// Command-line option handlers. Each takes the argument vector and its length and returns how many arguments it
// consumed: 1 for a flag, 2 for an option that carries a value. They set members of the writable GlobalData
// (TheWritableGlobalData, absent before the data is built, hence the null test) or the BFME2 debug globals. A
// member the header knows only by its offset is reached through its m_unk array, and a handler whose ledger name
// is still a placeholder (Rva...) keeps that spelling because the option it parses is not named in the original.
extern bool BFME2WatchdogEnabled;
extern bool BFME2ScriptDebugLiteMode;
extern bool BFME2UseDebugWindowLite;
extern unsigned int BFME2CommandFlags;
extern bool BfmeFormationPreviewUseDecals;
// Generated from the ledger; map snapshot c591f7146f88c8a8248ebb8ae22f0c978c22aed463db64e05ef57ff0dfae99cc
// object=G003B94CD name-evidence=invented
// Extents and edges are hypotheses; byte verification alone does not establish original TU boundaries.
// FUNCTION: BFME2 0x003B94CD size=9 name=?parseWatchdog@@YAHQAPADH@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
int parseWatchdog(char *args[], int num)
{
	BFME2WatchdogEnabled = true;
	return 1;
}
// FUNCTION: BFME2 0x003B94D6 size=27 name=?parseRif@@YAHQAPADH@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x003B94F1 size=11 name=?parseNoWatchdog@@YAHQAPADH@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
int parseNoWatchdog(char *args[], int num)
{
	BFME2WatchdogEnabled = false;
	return 1;
}
// FUNCTION: BFME2 0x003B94FC size=9 name=?Rva003B94FCParse@@YAHQAPADH@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x003B9505 size=17 name=?parseSkipMapUnroll@@YAHQAPADH@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
int parseSkipMapUnroll(char *args[], int num)
{
	if (TheWritableGlobalData)
		TheWritableGlobalData->m_skipMapUnroll = true;
	return 1;
}
// FUNCTION: BFME2 0x003B9516 size=11 name=?Rva003B9516@@YAHQAPADH@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
int Rva003B9516(char *args[], int num)
{
	BfmeFormationPreviewUseDecals = false;
	return 1;
}
// FUNCTION: BFME2 0x003B9521 size=20 name=?parseNoPlanningMode@@YAHQAPADH@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
int parseNoPlanningMode(char *args[], int num)
{
	if (TheWritableGlobalData)
		TheWritableGlobalData->m_planningModeEnabled = false;
	return 1;
}
// FUNCTION: BFME2 0x003B9535 size=20 name=?parsePlanningMode@@YAHQAPADH@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
int parsePlanningMode(char *args[], int num)
{
	if (TheWritableGlobalData)
		TheWritableGlobalData->m_planningModeEnabled = true;
	return 1;
}
// FUNCTION: BFME2 0x003B9549 size=20 name=?Rva003B9549Parse@@YAHQAPADH@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x003B955D size=17 name=?parseWin@@YAHQAPADH@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
int parseWin(char *args[], int num)
{
	if (TheWritableGlobalData)
		TheWritableGlobalData->m_windowed = true;
	return 1;
}
// FUNCTION: BFME2 0x003B956E size=20 name=?Rva003B956E@@YAHQAPADH@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
int Rva003B956E(char *args[], int num)
{
	if (TheWritableGlobalData)
		TheWritableGlobalData->m_useHelpTextSystem = true;
	return 1;
}
// FUNCTION: BFME2 0x003B9582 size=27 name=?parseNoMusic@@YAHQAPADH@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
int parseNoMusic(char *args[], int num)
{
	BFME2CommandFlags |= 4;
	if (TheWritableGlobalData)
		TheWritableGlobalData->m_musicOn = false;
	return 1;
}
// FUNCTION: BFME2 0x003B959D size=30 name=?parseFPUPreserve@@YAHQAPADH@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
int parseFPUPreserve(char *args[], int num)
{
	if (num > 1)
		DX8Wrapper_PreserveFPU = atoi(args[1]);
	return 2;
}
// FUNCTION: BFME2 0x003B95BB size=83 name=?parseNoAudio@@YAHQAPADH@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
int parseNoAudio(char *args[], int num)
{
	BFME2CommandFlags |= 2;
	if (TheWritableGlobalData)
	{
		TheWritableGlobalData->m_audioOn = false;
		TheWritableGlobalData->m_speechOn = false;
		TheWritableGlobalData->m_soundsOn = false;
		TheWritableGlobalData->m_sounds3DOn = false;
		TheWritableGlobalData->m_musicOn = false;
		TheWritableGlobalData->m_ambientStreamsOn = false;
	}
	return 1;
}
// FUNCTION: BFME2 0x003B960E size=20 name=?Rva003B960E@@YAHQAPADH@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
int Rva003B960E(char *args[], int num)
{
	if (TheWritableGlobalData)
		TheWritableGlobalData->m_unk0x9AD = true;
	return 1;
}
// FUNCTION: BFME2 0x003B9622 size=20 name=?Rva003B9622@@YAHQAPADH@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
int Rva003B9622(char *args[], int num)
{
	if (TheWritableGlobalData)
		TheWritableGlobalData->m_unk0x9AE[0] = true;
	return 1;
}
// FUNCTION: BFME2 0x003B9636 size=17 name=?parseNoWin@@YAHQAPADH@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
int parseNoWin(char *args[], int num)
{
	if (TheWritableGlobalData)
		TheWritableGlobalData->m_windowed = false;
	return 1;
}
// FUNCTION: BFME2 0x003B9647 size=50 name=?parseFullVersion@@YAHQAPADH@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x003B9679 size=34 name=?Rva003B9679Parse@@YAHQAPADH@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
int Rva003B9679Parse(char *args[], int num)
{
	if (TheWritableGlobalData)
	{
		TheWritableGlobalData->m_useShadowMapping = false;
		TheWritableGlobalData->m_useShadowVolumes = false;
		TheWritableGlobalData->m_useShadowDecals = false;
	}
	return 1;
}
// FUNCTION: BFME2 0x003B969B size=45 name=?parseXRes@@YAHQAPADH@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
int parseXRes(char *args[], int num)
{
	if (TheWritableGlobalData && num > 1)
	{
		TheWritableGlobalData->m_xResolution = atoi(args[1]);
		return 2;
	}
	return 1;
}
// FUNCTION: BFME2 0x003B96C8 size=45 name=?parseYRes@@YAHQAPADH@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
int parseYRes(char *args[], int num)
{
	if (TheWritableGlobalData && num > 1)
	{
		TheWritableGlobalData->m_yResolution = atoi(args[1]);
		return 2;
	}
	return 1;
}
// FUNCTION: BFME2 0x003B96F5 size=20 name=?Rva003B96F5Parse@@YAHQAPADH@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
int Rva003B96F5Parse(char *args[], int num)
{
	if (TheWritableGlobalData)
		TheWritableGlobalData->m_unk0xE9D[8] = false;
	return 1;
}
// FUNCTION: BFME2 0x003B9709 size=46 name=?Rva003B9709Parse@@YAHQAPADH@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x003B9737 size=20 name=?Rva003B9737Parse@@YAHQAPADH@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
int Rva003B9737Parse(char *args[], int num)
{
	if (TheWritableGlobalData)
		TheWritableGlobalData->m_unk0xE9D[7] = false;
	return 1;
}
// FUNCTION: BFME2 0x003B974B size=32 name=?Rva003B974BParse@@YAHQAPADH@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
int Rva003B974BParse(char *args[], int num)
{
	if (TheWritableGlobalData)
	{
		TheWritableGlobalData->m_unk0x9C1[0] = true;
		TheWritableGlobalData->m_winCursors = true;
	}
	return 1;
}
// FUNCTION: BFME2 0x003B976B size=39 name=?parseScriptDebug2@@YAHQAPADH@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
int parseScriptDebug2(char *args[], int num)
{
	if (TheWritableGlobalData)
	{
		TheWritableGlobalData->m_unk0x9C1[0] = true;
		TheWritableGlobalData->m_winCursors = true;
		BFME2UseDebugWindowLite = true;
	}
	return 1;
}
// FUNCTION: BFME2 0x003B9792 size=46 name=?parseScriptDebugLite@@YAHQAPADH@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
int parseScriptDebugLite(char *args[], int num)
{
	if (TheWritableGlobalData)
	{
		TheWritableGlobalData->m_unk0x9C1[0] = true;
		TheWritableGlobalData->m_winCursors = true;
		BFME2UseDebugWindowLite = true;
		BFME2ScriptDebugLiteMode = true;
	}
	return 1;
}
// FUNCTION: BFME2 0x003B97C0 size=41 name=?parseParticleEdit@@YAHQAPADH@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
int parseParticleEdit(char *args[], int num)
{
	if (TheWritableGlobalData)
	{
		TheWritableGlobalData->m_unk0x9C1[1] = true;
		TheWritableGlobalData->m_winCursors = true;
		TheWritableGlobalData->m_windowed = true;
	}
	return 1;
}
// FUNCTION: BFME2 0x003B97E9 size=20 name=?parseBuildMapCache@@YAHQAPADH@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
int parseBuildMapCache(char *args[], int num)
{
	if (TheWritableGlobalData)
		TheWritableGlobalData->m_unk0xAB5[0] = true;
	return 1;
}
// FUNCTION: BFME2 0x003B97FD size=20 name=?parseNoFX@@YAHQAPADH@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
int parseNoFX(char *args[], int num)
{
	if (TheWritableGlobalData)
		TheWritableGlobalData->m_unk0x9AE[1] = false;
	return 1;
}
// FUNCTION: BFME2 0x003B9811 size=20 name=?parseForceBenchmark@@YAHQAPADH@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
int parseForceBenchmark(char *args[], int num)
{
	if (TheWritableGlobalData)
		TheWritableGlobalData->m_unk0x9E4[0] = true;
	return 1;
}
// FUNCTION: BFME2 0x003B9825 size=20 name=?parseNoMoveCamera@@YAHQAPADH@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
int parseNoMoveCamera(char *args[], int num)
{
	if (TheWritableGlobalData)
		TheWritableGlobalData->m_disableCameraMovements = true;
	return 1;
}
// FUNCTION: BFME2 0x003B9839 size=20 name=?parseWinCursors@@YAHQAPADH@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
int parseWinCursors(char *args[], int num)
{
	if (TheWritableGlobalData)
		TheWritableGlobalData->m_winCursors = true;
	return 1;
}
// FUNCTION: BFME2 0x003B984D size=20 name=?parseShowTeamDot@@YAHQAPADH@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
int parseShowTeamDot(char *args[], int num)
{
	if (TheWritableGlobalData)
		TheWritableGlobalData->m_unk0x9C7[1] = true;
	return 1;
}
// FUNCTION: BFME2 0x003B9861 size=20 name=?Rva003B9861Parse@@YAHQAPADH@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
int Rva003B9861Parse(char *args[], int num)
{
	if (TheWritableGlobalData)
		TheWritableGlobalData->m_unk0x9D0 = true;
	return 1;
}
// FUNCTION: BFME2 0x003B9875 size=20 name=?Rva003B9875Parse@@YAHQAPADH@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
int Rva003B9875Parse(char *args[], int num)
{
	if (TheWritableGlobalData)
		TheWritableGlobalData->m_unk0x9D1 = true;
	return 1;
}
// FUNCTION: BFME2 0x003B9889 size=20 name=?Rva003B9889Parse@@YAHQAPADH@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
int Rva003B9889Parse(char *args[], int num)
{
	if (TheWritableGlobalData)
		TheWritableGlobalData->m_unk0x9D2[0] = true;
	return 1;
}
// FUNCTION: BFME2 0x003B989D size=20 name=?Rva003B989DParse@@YAHQAPADH@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
int Rva003B989DParse(char *args[], int num)
{
	if (TheWritableGlobalData)
		TheWritableGlobalData->m_unk0x9D8[0] = true;
	return 1;
}
// FUNCTION: BFME2 0x003B98B1 size=46 name=?parseSeed@@YAHQAPADH@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
int parseSeed(char *args[], int num)
{
	if (TheWritableGlobalData && num > 1)
		TheWritableGlobalData->m_fixedSeed = atoi(args[1]);
	return 2;
}
// FUNCTION: BFME2 0x003B98DF size=20 name=?parseIncrAGPBuf@@YAHQAPADH@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
int parseIncrAGPBuf(char *args[], int num)
{
	if (TheWritableGlobalData)
		TheWritableGlobalData->m_incrementalAGPBuf = true;
	return 1;
}
// FUNCTION: BFME2 0x003B98F3 size=46 name=?parseNetMinPlayers@@YAHQAPADH@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x003B9921 size=20 name=?Rva003B9921Parse@@YAHQAPADH@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
int Rva003B9921Parse(char *args[], int num)
{
	if (TheWritableGlobalData)
		TheWritableGlobalData->m_unk0x9AC = true;
	return 1;
}
// FUNCTION: BFME2 0x003B9935 size=46 name=?parsePlayStats@@YAHQAPADH@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
int parsePlayStats(char *args[], int num)
{
	if (TheWritableGlobalData && num > 1)
		TheWritableGlobalData->m_playStats = atoi(args[1]);
	return 2;
}
// FUNCTION: BFME2 0x003B9963 size=20 name=?parseDemoLoadScreen@@YAHQAPADH@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
int parseDemoLoadScreen(char *args[], int num)
{
	if (TheWritableGlobalData)
		TheWritableGlobalData->m_unk0xB6E[0] = true;
	return 1;
}
