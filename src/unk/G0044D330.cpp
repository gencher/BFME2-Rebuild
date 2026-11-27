// cl: /O1 /D_BFME_RETAIL_TREE_INSERT_LAYOUT /D_STLP_USE_MALLOC /D_STLP_USE_STATIC_LIB /D_STLP_BFME_COPY_TRIVIAL_EXTERN /D_STLP_NO_EXCEPTIONS
// stlport46
// bfmealloc_alloconly
#include "Common/GameModePreferences.h"
// Generated from the ledger; map snapshot c591f7146f88c8a8248ebb8ae22f0c978c22aed463db64e05ef57ff0dfae99cc
// object=G0044D330 name-evidence=invented
// Extents and edges are hypotheses; byte verification alone does not establish original TU boundaries.
// FUNCTION: BFME2 0x0044D330 size=335 name=?rva0044D330@GameModePreferences@@QAE?AVUnicodeString@@XZ evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x0044D47F size=142 name=?setUserName@LanLobbyUserNamePrefs@@QAEXVUnicodeString@@@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x0044D512 size=57 name=?makeKey@GameModePreferences@@ABEABVAsciiString@@PBD@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
const AsciiString &GameModePreferences::makeKey(const char *key) const
{
	const char *modeName = "";
	switch (m_mode)
	{
	case 0:
		modeName = "Rts";
		break;
	case 1:
		modeName = "Strat";
		break;
	}
	m_keyBuffer.format("%s:%s", modeName, key);
	return m_keyBuffer;
}
// FUNCTION: BFME2 0x0044D54B size=31 name=??0GameModePreferences@@QAE@H@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
GameModePreferences::GameModePreferences(int mode) : m_mode(mode)
{
}
// FUNCTION: BFME2 0x0044D56A size=59 name=??1GameModePreferences@@UAE@XZ evidence=N:ledger tier=B hypothesis:mm1-gap-unit
GameModePreferences::~GameModePreferences()
{
}
// FUNCTION: BFME2 0x0044D5A5 size=73 name=?getStrategicScenario@GameModePreferences@@QAEHXZ evidence=N:ledger tier=B hypothesis:mm1-gap-unit
int GameModePreferences::getStrategicScenario()
{
	return getInt(AsciiString("StrategicScenario"), -1);
}
// FUNCTION: BFME2 0x0044D5EE size=72 name=?setStrategicScenario@GameModePreferences@@QAEXH@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
void GameModePreferences::setStrategicScenario(int scenario)
{
	setInt(AsciiString("StrategicScenario"), scenario);
}
// FUNCTION: BFME2 0x0044D636 size=47 name=?setBool@GameModePreferences@@UAEXABVAsciiString@@_N@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
void GameModePreferences::setBool(const AsciiString &key, bool val)
{
	UserPreferences::setBool(makeKey(key.str()), val);
}
// FUNCTION: BFME2 0x0044D665 size=51 name=?setReal@GameModePreferences@@UAEXABVAsciiString@@M@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
void GameModePreferences::setReal(const AsciiString &key, float val)
{
	UserPreferences::setReal(makeKey(key.str()), val);
}
// FUNCTION: BFME2 0x0044D698 size=47 name=?setInt@GameModePreferences@@UAEXABVAsciiString@@H@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
void GameModePreferences::setInt(const AsciiString &key, int val)
{
	UserPreferences::setInt(makeKey(key.str()), val);
}
// FUNCTION: BFME2 0x0044D6C7 size=47 name=?getBool@GameModePreferences@@UBE_NABVAsciiString@@_N@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
bool GameModePreferences::getBool(const AsciiString &key, bool defaultValue) const
{
	return UserPreferences::getBool(makeKey(key.str()), defaultValue);
}
// FUNCTION: BFME2 0x0044D6F6 size=51 name=?getReal@GameModePreferences@@UBEMABVAsciiString@@M@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
float GameModePreferences::getReal(const AsciiString &key, float defaultValue) const
{
	return UserPreferences::getReal(makeKey(key.str()), defaultValue);
}
// FUNCTION: BFME2 0x0044D729 size=47 name=?getInt@GameModePreferences@@UBEHABVAsciiString@@H@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
int GameModePreferences::getInt(const AsciiString &key, int defaultValue) const
{
	return UserPreferences::getInt(makeKey(key.str()), defaultValue);
}
// FUNCTION: BFME2 0x0044D774 size=194 name=?rva0044D774@GameModePreferences@@QAE_NPAVGameSlot@@@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x0044D836 size=86 name=?rva0044D836@GameModePreferences@@QAEHXZ evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x0044D88C size=180 name=?rva0044D88C@GameModePreferences@@QAEHXZ evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x0044D940 size=70 name=?rva0044D940@GameModePreferences@@QAE_NXZ evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x0044D986 size=290 name=?rva0044D986@GameModePreferences@@QAE?AVAsciiString@@XZ evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x0044DAA8 size=172 name=?rva0044DAA8@GameModePreferences@@QAE?AVAsciiString@@ABV2@@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x0044DB54 size=81 name=?rva0044DB54@GameModePreferences@@QAE_NPAH@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x0044DBA5 size=175 name=?rva0044DBA5@GameModePreferences@@QAE?AVAsciiString@@XZ evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x0044DC54 size=101 name=?rva0044DC54@GameModePreferences@@QAEXH@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
void GameModePreferences::rva0044DC54(int value)
{
	AsciiString text;
	text.format("%d", value);
	(*this)[makeKey("Hero")] = text;
}
// FUNCTION: BFME2 0x0044DCB9 size=101 name=?rva0044DCB9@GameModePreferences@@QAEXH@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
void GameModePreferences::rva0044DCB9(int value)
{
	AsciiString text;
	text.format("%d", value);
	(*this)[makeKey("Color")] = text;
}
// FUNCTION: BFME2 0x0044DD1E size=101 name=?rva0044DD1E@GameModePreferences@@QAEXH@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
void GameModePreferences::rva0044DD1E(int value)
{
	AsciiString text;
	text.format("%d", value);
	(*this)[makeKey("PlayerTemplate")] = text;
}
// FUNCTION: BFME2 0x0044DD83 size=120 name=?rva0044DD83@GameModePreferences@@QAEXVAsciiString@@@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x0044DDFB size=95 name=?rva0044DDFB@GameModePreferences@@QAEXPAH@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x0044DE5A size=103 name=?rva0044DE5A@GameModePreferences@@QAEXABVAsciiString@@@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x0044DEC1 size=103 name=?rva0044DEC1@GameModePreferences@@QAEXABVAsciiString@@@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
