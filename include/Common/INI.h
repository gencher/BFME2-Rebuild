// INI.h: SAGE INI reader, field-parse tables and the shared field parsers.
// Target facts: a module data's buildFieldParse passes its FieldParse table and
// offset 0 to MultiIniFieldParse::add (FIELD_PROC);
// FieldParse entries are 0x10 bytes {token, parse, userData, offset}.
// Parser names are
// Zero Hour's INI.h spellings.
#pragma once
#ifndef BFME2_INI_H
#define BFME2_INI_H

#include "Common/GameType.h"
#include "Common/AsciiString.h"
#include "Common/Vector.h"
#include "Common/Science.h"
#include "Common/INIFileTable.h"

enum INILoadType { INI_LOAD_INVALID = 0, INI_LOAD_OVERWRITE = 1, INI_LOAD_CREATE_OVERRIDES = 2, INI_LOAD_MULTIFILE = 3, INI_LOAD_RELOAD = 5 /* parser 0x001DAF81 compares the load type with 5 on an existing button; role-inferred name */ };
class INI;
class File;
class Xfer;
struct LookupListRec { const char *name; int value; };	// 8 bytes: retail's {name, value} tables end with {0, 0} (0x007DD0B4 shake types, 0x007DD1D0 scorch types, 0x00800698 weapon slots)

#include "Common/INI/FieldParse.h"

class INI
{
public:
    INI(); //2CDB0, full allocation87C at541DB
    ~INI();
    void initFromINIMulti(void *,const MultiIniFieldParse &);

    void load(AsciiString,INILoadType,Xfer *,void (*)(INI *)=0);
    unsigned char loadFile(AsciiString,INILoadType,Xfer *);
    bool loadBlock(AsciiString,AsciiString,AsciiString,INILoadType,Xfer *);
    bool loadDirectory(AsciiString,bool,INILoadType,Xfer *,int);
    AsciiString getNextQuotedAsciiString();
    const char *getNextSubToken(const char *);
    int scanLookupList(const char *,const LookupListRec *);
    const char *getSeps() const { return m_seps; }
    const char *getSepsPercent() const { return m_sepsPercent; }
    const char *getSepsQuote() const { return m_sepsQuote; }
    bool isEOF() const { return m_endOfFile; }
    static void parseMappedImageDefinition(INI *);
    static void parseMouseDefinition(INI *);        // 0x001EE050: initFromINI(TheMouse, TheMouseFieldParseTable)
    static void parseMouseCursorDefinition(INI *);  // 0x001EF2CB: names one CursorInfo of TheMouse and parses its table
    static void Rva001FDD33_ParseResourceModifierValues(INI *ini, void *instance, void *store, const void *userData); // 0x001FDD33: appends every integer on the line to the vector at store
    static void parseInGameUIDefinition(INI *); // 0x0029A627: initFromINI(TheInGameUI, TheInGameUI->getInGameUIFieldParse())
    static void parseAnim2DDefinition(INI *); // 0x002D7554
    static void parseDynamicGameLODLevel(INI *, void *, void *, const void *); // 0x002025AC: a LOD name token to its level index
    static void parseBenchProfile(INI *); // 0x002026D4
    static void parseLODPreset(INI *); // 0x00202E23
    static void parseStaticGameLODDefinition(INI *); // 0x00202F05
    static void parseAudioLODDefinition(INI *); // 0x00202F8A
    static void parseLanguageDefinition(INI *); // 0x001EA3F4: initFromINI(TheGlobalLanguageData, GlobalLanguage::m_languageParseTable)
    static void parseMiscEvaData(INI *);
    static void parseWaterSettingDefinition(INI *);
    static void parseWaterTransparencyDefinition(INI *);
    static void parseHeaderTemplateDefinition(INI *);
    static void parseDrawGroupNumberDefinition(INI *);
    static void parseOnlineChatColorDefinition(INI *); // 0x001EF471, GameNetwork/GameSpy/Chat.cpp
    static void parseMultiplayerSettingsDefinition(INI *);
    static void parseMultiplayerColorDefinition(INI *);
    static void parseTerrainRoadDefinition(INI *);
    static void parseTerrainBridgeDefinition(INI *);
    static void parseControlBarSchemeDefinition(INI *);
    static void parseMiscAudio(INI *);
    static void parseVideoDefinition(INI *); // 0x0075A4DC
    static void parseTerrainDefinition(INI *); // 0x00200A65
    // Retail block callbacks 0x001EFFC6 and 0x001F0016.
    static void parseObjectDefinition(INI *);
    static void parseObjectReskinDefinition(INI *);
    static void parseChildObjectDefinition(INI *);
    void initFromINI(void *,const FieldParse *);
    // 0x0002DEB5: builds a MultiIniFieldParse through proc, then initFromINIMulti.
    // Every ModuleFactory friend_newModuleData factory calls this on a non-null ini.
    void initFromINIMultiProc(void *what, void (__cdecl *proc)(MultiIniFieldParse &));
    AsciiString getFilename() const; // target2C005
    AsciiString getNextAsciiString(); // target2EA4F, BridgeBehavior parser45679B
    int getLineNum() const; // target2BBED
    int getPreviousLineNum() const; // 0x0002BBDE (map ?rva0002BBDE@INI@@QBEHXZ, placeholder): the line number of the file index at +0x0C
    AsciiString getPreviousFilename() const; // 0x0002BFE5 (map ?rva0002BFE5@INI@@QBE?AVAsciiString@@XZ, placeholder): the file name of the file index at +0x0C
    int scanIndexList(const char *,const char *const *);
	void parseLine(AsciiString &line);	// 0x0002C0F5: parses one line of the file
	const char *getNextToken(const char *seps = 0);
    File *m_file;
    AsciiString m_filename;
    INILoadType m_loadType; // +8
    unsigned int m_previousLine,m_currentLine; //getLineNum2BBED/getPreviousLineNum2BC29
    char m_buffer[0x404]; //14
    const char *m_seps,*m_sepsPercent; //418/41C
    INILoadType getLoadType() const { return m_loadType; }
    const char *m_sepsColon; //420
    const char *m_sepsQuote,*m_blockEndToken,*m_endScriptToken; //424/428/42C
    bool m_endOfFile; //430
    char m_curBlockStart[0x404]; //431
    // Target ctor 0x0002CDB0 / dtor 0x0002CE5B: the per-file record table at +0x838
    // (getFilename 0x0002C005 and getLineNum 0x0002BBED forward to its getName/getLine).
    INIFileTable m_fileTable;
    AsciiString m_unknownString86C;
    _STL::vector<AsciiString> m_unknownStrings870;
	const char *getSepsColon() const { return m_sepsColon; }
	const char *getNextTokenOrNull(const char *seps = 0);
	const char *rva0002DFE2(const char *seps, bool *quotedList);	// 0x0002DFE2: next token; *quotedList set when the token holds a list to split (BitFlags<N>::parse 0x003B1017 proves it)
	// 0x0002D0A9: a token that names a macro comes back as its expansion; any other token is returned as is.
	static const char *preprocessMacro(const char *token);

	Int scanInt(const char *token); // 0x0002ECCF, QAEHPBD: nonstatic __thiscall (the map spells it so; parseFontSizeMapping 0x00218E96 passes the INI in ecx)
	UnsignedInt scanUnsignedInt(const char *token); // 0x0002ED3A, QAEIPBD: nonstatic __thiscall, as scanReal
	Real scanReal(const char *token); // 0x0002EDA5, QAEMPBD: nonstatic __thiscall
	Real dup_002EE10(const char *token); // 0x0002EE10, QAEMPBD: scanReal(token) * 0.01f (percent scanner; retail name pending)
	Bool scanBool(const char *token); // QAE_NPBD in the map: nonstatic __thiscall
	void prependTokenText(const AsciiString &text, char separator);	// 0x0002CBCC (map ?rva0002CBCC@INI@@QAEXABVAsciiString@@D@Z, ?:placeholder): puts text and the separator in front of the pending token source, so the next getNextToken calls read the text's words
	Int parseIntMathExpression(const char *token, Int (INI::*scan)(const char *));	// 0x0002E0C9, QAEHPBDP81@AEH0@Z@Z: the '#' math-expression form of scanInt
	UnsignedInt parseUnsignedIntMathExpression(const char *token, UnsignedInt (INI::*scan)(const char *));	// 0x0002E299
	Real parseMathExpression(const char *token, Real (INI::*scan)(const char *));	// 0x0002E46A

	static void parseByteSizedIndexList(INI *ini, void *instance, void *store, const void *userData);	// 0x0002EC6E: scanIndexList into a byte
	static void parseUnsignedByte(INI *ini, void *instance, void *store, const void *userData);	// 0x0002EE6D
	static void parseShort(INI *ini, void *instance, void *store, const void *userData);	// 0x0002EEB9
	static void parseUnsignedShort(INI *ini, void *instance, void *store, const void *userData);	// 0x0002EF09
	static void parseQuotedAsciiString(INI *ini, void *instance, void *store, const void *userData);	// 0x0002F15A
	static void parseBitString8(INI *ini, void *instance, void *store, const void *userData);	// 0x0002F1DC
	static void parseBitString16(INI *ini, void *instance, void *store, const void *userData);	// 0x0002F21A
	static void parseBitInInt32(INI *ini, void *instance, void *store, const void *userData);	// 0x0002E86C: sets or clears the mask in userData in the word at store
	static Bool isEndOfBlock(char *bufferToCheck);	// 0x0002BDF2: the line is just "End"
	static Bool isDeclarationOfType(AsciiString blockType, AsciiString blockName, char *bufferToCheck);	// 0x0002C2C7: the line is "<type> <name>"
	const char *getNextExpandedTokenOrNull(const char *seps, bool *expanded);	// 0x0002E03D (map ?rva0002E03D@INI@@QAEPBDPBDPA_N@Z, placeholder): the token, run through preprocessMacro; *expanded says a macro was expanded; no throw when the line has no token
	static void parseBool(INI *ini, void *instance, void *store, const void *userData);
	static void parseColorInt(INI *ini, void *instance, void *store, const void *userData);
	static void parseByte(INI *ini, void *instance, void *store, const void *userData);
	static void parseInt(INI *ini, void *instance, void *store, const void *userData);
	static void parseUnsignedInt(INI *ini, void *instance, void *store, const void *userData);
	static void parseReal(INI *ini, void *instance, void *store, const void *userData);
	static void dup_002F02F(INI *ini, void *instance, void *store, const void *userData);	// 0x0002F02F: a real, stored, then refused if negative (the map's placeholder name)
	static void dup_002F082(INI *ini, void *instance, void *store, const void *userData);	// 0x0002F082: a real, stored, refused if positive
	static void dup_002F0F7(INI *ini, void *instance, void *store, const void *userData);	// 0x0002F0F7: (Int)(real * 1000.0f)
	static void parsePositiveNonZeroReal(INI *ini, void *instance, void *store, const void *userData);
	static void Rva004C2918_ParseUpgradeMoneyAmount(INI *ini, void *instance, void *store, const void *userData);	// 0x004C2918: UpgradeMoneyAmount of CashHackSpecialPowerModuleData (placeholder name)
	static void Rva004C3F48_ParseNamedReal(INI *ini, void *instance, void *store, const void *userData);	// 0x004C3F48: ElvenWoodObject of ElvenWoodSpecialPowerModuleData (placeholder name)
	static void Rva004B2B64_ParseReplaceObject(INI *ini, void *instance, void *store, const void *userData);	// 0x004B2B64: ReplaceObject of ReplaceObjectUpdateModuleData (placeholder name)
	static void Rva0045A22E_ParseQuery(INI *ini, void *instance, void *store, const void *userData);	// 0x0045A22E: Query of AutoAbilityBehaviorModuleData (placeholder name)
	static void Rva004C8F94_ParseSpawnObject(INI *ini, void *instance, void *store, const void *userData);	// 0x004C8F94: SpawnObject of CritterEmitterUpdateModuleData (placeholder name)
	static void parsePercentToReal(INI *ini, void *instance, void *store, const void *userData);
	static void Rva004B60D5_ParsePercentage(INI *ini, void *instance, void *store, const void *userData);	// 0x004B60D5: parsePercentToReal into a Real, push_back into the vector<Real> at store (CostModifierUpgrade's Percentage)
	static void parseAngleReal(INI *ini, void *instance, void *store, const void *userData);
	static void parseAngularVelocityReal(INI *ini, void *instance, void *store, const void *userData);
	static void parseAsciiString(INI *ini, void *instance, void *store, const void *userData);
	static void parseAsciiStringVector(INI *ini, void *instance, void *store, const void *userData);
	static void parseAsciiStringVectorAppend(INI *ini, void *instance, void *store, const void *userData);	// 0x0002E896
	static void Rva002F592_ParseArmyPlacementPos(INI *ini, void *instance, void *store, const void *userData);	// 0x0002F592, placeholder name: an army placement position pair
	static void parseICoord2D(INI *ini, void *instance, void *store, const void *userData);	// 0x0002F5BA (ShellMenuSchemeManager::parseImagePart 0x00200842)
	static void parseShellMenuSchemeDefinition(INI *);	// 0x00200984
	static void parseAndTranslateLabel(INI *ini, void *instance, void *store, const void *userData);	// 0x0033987B
	static void parseCoord3D(INI *ini, void *instance, void *store, const void *userData);
	static void parseCoord2D(INI *ini, void *instance, void *store, const void *userData);
	static void parseRGBColor(INI *ini, void *instance, void *store, const void *userData);	// 0x0002F259
	static void parseRGBAColorInt(INI *ini, void *instance, void *store, const void *userData);	// 0x0002F2FF
	static void parseDurationReal(INI *ini, void *instance, void *store, const void *userData);
	static void parseDurationUnsignedInt(INI *ini, void *instance, void *store, const void *userData);
	static void Rva00338E52_ParseEnvelope(INI *ini, void *instance, void *store, const void *userData);	// 0x00338E52: the Envelope token of W3DLaserDrawModuleData (+0x4C); placeholder name
	static void parseDurationUnsignedShort(INI *ini, void *instance, void *store, const void *userData); // 0x00338B76
	static void Rva00339761_ParseDurationUnsignedByte(INI *ini, void *instance, void *store, const void *userData); // 0x00339761
	static void parseVelocityReal(INI *ini, void *instance, void *store, const void *userData);
	static void parseAccelerationReal(INI *ini, void *instance, void *store, const void *userData);
	static void parseBitString32(INI *ini, void *instance, void *store, const void *userData);
	static void parseIndexList(INI *ini, void *instance, void *store, const void *userData);
	static void parseLookupList(INI *ini, void *instance, void *store, const void *userData);
	static void parseFXList(INI *ini, void *instance, void *store, const void *userData);
	static void parseObjectCreationList(INI *ini, void *instance, void *store, const void *userData);
	static void parseParticleSystemTemplate(INI *ini, void *instance, void *store, const void *userData);
	static void parseWeaponTemplate(INI *ini, void *instance, void *store, const void *userData);
	static void parseThingTemplate(INI *ini, void *instance, void *store, const void *userData);
	static void parseAudioEventRTS(INI *ini, void *instance, void *store, const void *userData);
	static void parseSpecialPowerTemplate(INI *ini, void *instance, void *store, const void *userData);
	static void parseUpgradeTemplate(INI *ini, void *instance, void *store, const void *userData);
	static void parseScience(INI *ini, void *instance, void *store, const void *userData);
	static void parseDamageTypeFlags(INI *ini, void *instance, void *store, const void *userData);
	static void parseVeterancyLevelFlags(INI *ini, void *instance, void *store, const void *userData); // 0x00338C0F
	static void parseScienceVector(INI *ini, void *instance, void *store, const void *userData); // 0x00339AF1
	static void parseDeathTypeFlags(INI *ini, void *instance, void *store, const void *userData); // 0x00338D91
	static void parseGameClientRandomVariable(INI *ini, void *instance, void *store, const void *userData); // 0x00338A9D
	static void parseMappedImage(INI *ini, void *instance, void *store, const void *userData); // 0x003390B2
	static void parseDamageFX(INI *ini, void *instance, void *store, const void *userData); // 0x0033962F
	static void parseArmorTemplate(INI *ini, void *instance, void *store, const void *userData); // 0x003394FB
	static void parseAnim2DTemplate(INI *ini, void *instance, void *store, const void *userData); // 0x00339109
	static void parseSpecialPowerIdList(INI *ini, void *instance, void *store, const void *userData); // 0x00339B50
	static void parseIndexListVector(INI *ini, void *instance, void *store, const void *userData); // 0x00339E1C
	static ScienceType scanScience(const char *token); // 0x00338A8D
	static void iniParseAnimAndDuration(INI *ini, void *instance, void *store, const void *userData); // 0x0044E6CD
	static void iniParseAnim(INI *ini, void *instance, void *store, const void *userData); // 0x004B6481 (ModelConditionState token parser) (bfp.json parse_name ?iniParseAnimAndDuration@INI@@SAXPAV1@PAX1PBX@Z)
protected:
	bool prepFile(const AsciiString &filename, INILoadType loadType);	// 0x0002D2C1
	void readLine();	// 0x0002D669
	void unPrepFile();	// 0x0002BF4A
	static Bool isValidINIFilename(const char *filename);	// 0x0002BB7F (map ?isValidINIFilename@INI@@KA_NPBD@Z: protected static)
};

typedef char INISizeCheck[sizeof(INI)==0x87C ? 1 : -1];

// Free field parsers.
void iniParseObjectFilter(INI *ini, void *instance, void *store, const void *userData);

#endif
