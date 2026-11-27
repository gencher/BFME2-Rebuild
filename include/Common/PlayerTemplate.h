// PlayerTemplate.h: one PlayerTemplate INI block (a faction: its starting units, sciences,
// UI images and production modifiers) and the PlayerTemplateStore subsystem that owns them.
// SIZE of PlayerTemplate 0x1DC, of PlayerTemplateStore 0x28.
//
// Target facts (retail game.dat):
//  - Layout: field tables 0x007E2070 (61 entries, base 0) and 0x0081DB18 (3 entries, base
//    +0x154), both walked by 0x001FE70F through MultiIniFieldParse; every token's offset is
//    below. The constructor (0x001FEB8B, map name BfmeObject476, ICF-folded with another
//    class that stores the same vtable) shows the default of each member and the container
//    types; its destructor 0x001FE06A shows the member destruction order.
//  - Vtable 0x007E1BE8 has one slot, the deleting destructor 0x001FE772; the first 0x10
//    bytes are Overridable (store keeps every template by value in a vector and chains
//    overrides through +4: findPlayerTemplate 0x001FD31B).
//  - +0x10 is the name key (parsePlayerTemplateDefinition 0x001FEFEC writes it, find reads it).
//  - +0x1C: four floats, each 1.0f in the constructor (helper 0x003B0E83); not in a table.
//  - +0x2C: a Money (vtable 0x00BE1AA0, StartMoney's parser 0x001FD1B7 deposits into it).
//  - +0x154..+0x164: the second table's record (0x003B0E5D: string, then +4 = 0,
//    +8 = 0x3C, +0xC = 0x64); parsePlayerTemplateDefinition's helper copies the side name
//    into its string at the end of the parse (0x001FE768).
//  - The three Production* parsers (0x001FDF26, 0x001FDF73, 0x001FDFDB) fill the maps at
//    +0xF4 (name key -> float), +0x100 (hash map keyed by name string, float values) and
//    +0x114 (name key -> int).
#pragma once
#ifndef BFME2_PLAYERTEMPLATE_H
#define BFME2_PLAYERTEMPLATE_H

#include "Common/GameType.h"
#include "Common/AsciiString.h"
#include "Common/UnicodeString.h"
#include "Common/Overridable.h"
#include "Common/Money.h"
#include "Common/RGBColor.h"
#include "Common/Coord3D.h"
#include "Common/SubsystemInterface.h"
#include "Common/Vector.h"
#include <set>
#include "Common/Science.h"
#include "Common/StlMap.h"
#include "Common/StlHashMap.h"
#include "Common/STLTypedefs.h"
#include "Common/INI.h"
#include "GameClient/Image.h"
#include "Common/NameKeyGenerator.h"

class OpaqueRefCounted;

class PlayerTemplate : public Overridable
{
public:
	PlayerTemplate();												// 0x001FEB8B (map: ??0BfmeObject476)
	PlayerTemplate(const PlayerTemplate &);							// 0x001FE352 (map: ??0BfmeObject476, copy)
	virtual ~PlayerTemplate();										// 0x001FE06A (map: ??1BfmeObject476)
	PlayerTemplate &operator=(const PlayerTemplate &);				// 0x001FE78E (map: ??4BfmeObject476)

	static void parseStartMoney(INI *ini, void *instance, void *store, const void *userData);		// 0x001FD1B7

	NameKeyType getNameKey() const { return m_nameKey; }
	AsciiString getName() const;							// 0x0023E95E: the name behind m_nameKey (NameKeyGenerator::keyToName)
	UnicodeString getDisplayName() const;							// 0x00449B8F (an out-of-line copy of m_displayName; the map folds it into the LANAPI region)
	const Image *getHeadWaterMarkImage() const;						// 0x001FD1E8 (map: Rva001FD1E8)
	Int rva001FD234() const;										// 0x001FD234: looks m_side up through Rva0033A3F4Lookup
	const Image *rva001FD23F() const;								// 0x001FD23F: the image named by m_multiSelectionPortrait
	const Image *getFlagWaterMarkImage() const;						// 0x001FD1FB
	const Image *getSideIconImage() const;							// 0x001FD20E (map: Rva001FD1E8)
	const Image *getEnabledImage() const;							// 0x001FD221
	Coord3D *getStartingUnitOffset(Coord3D *offset, Int index) const;	// 0x001FD2D9 (map: Rva001FD2D9): StartingUnitOffset0..9 at +0x70, zero outside

protected:
	// Protected in retail (the map spells them K): only the field table reaches them.
	static void parseProductionCostChange(INI *ini, void *instance, void *store, const void *userData);		// 0x001FDF26
	static void parseProductionTimeChange(INI *ini, void *instance, void *store, const void *userData);		// 0x001FDF73
	static void parseProductionVeterancyLevel(INI *ini, void *instance, void *store, const void *userData);	// 0x001FDFDB

public:
	NameKeyType m_nameKey;											// 0x10 (written by parsePlayerTemplateDefinition)
	UnicodeString m_displayName;									// 0x14 DisplayName
	AsciiString m_side;												// 0x18 Side
	Real m_unk0x1C[4];												// 0x1C 1.0f each (0x003B0E83), no table entry
	Money m_startMoney;												// 0x2C StartMoney
	RGBColor m_preferredColor;										// 0x38 PreferredColor
	AsciiString m_startingBuilding;									// 0x44 StartingBuilding
	AsciiString m_startingUnit[10];									// 0x48 StartingUnit0..9
	Coord3D m_startingUnitOffset[10];								// 0x70 StartingUnitOffset0..9
	_STL::vector<AsciiString> m_startingUnitTacticalWOTR;			// 0xE8 StartingUnitTacticalWOTR
	_STL::map<Int, Real> m_productionCostChange;					// 0xF4 ProductionCostChange: name key -> cost multiplier
	_STL::hash_map<AsciiString, Real, rts::hash<AsciiString>, _STL::equal_to<AsciiString> > m_productionTimeChange;	// 0x100 ProductionTimeChange: name -> time multiplier (0x001FDE3F is its operator[])
	_STL::map<Int, Int> m_productionVeterancyLevel;					// 0x114 ProductionVeterancyLevel: name key -> veterancy level
	_STL::vector<ScienceType> m_intrinsicSciences;					// 0x120 IntrinsicSciences
	_STL::vector<ScienceType> m_intrinsicSciencesMP;				// 0x12C IntrinsicSciencesMP
	AsciiString m_purchaseScienceCommandSet;						// 0x138 PurchaseScienceCommandSet
	AsciiString m_purchaseScienceCommandSetMP;						// 0x13C PurchaseScienceCommandSetMP
	AsciiString m_specialPowerShortcutCommandSet;					// 0x140 SpecialPowerShortcutCommandSet
	AsciiString m_specialPowerShortcutWinName;						// 0x144 SpecialPowerShortcutWinName
	Int m_specialPowerShortcutButtonCount;							// 0x148 SpecialPowerShortcutButtonCount
	AsciiString m_loadScreenMusic;									// 0x14C LoadScreenMusic
	Bool m_isObserver;												// 0x150 IsObserver
	Bool m_playableSide;											// 0x151 PlayableSide
	AsciiString m_sideCopy;											// 0x154 first member of the second table's record: a copy of m_side
	Int m_intrinsicSciencePurchasePoints;							// 0x158 IntrinsicSciencePurchasePoints (default 0)
	Int m_maxLevelMP;												// 0x15C MaxLevelMP (default 0x3C)
	Int m_maxLevelSP;												// 0x160 MaxLevelSP (default 0x64)
	AsciiString m_scoreScreenImage;									// 0x164 ScoreScreenImage
	AsciiString m_loadScreenImage;									// 0x168 LoadScreenImage
	AsciiString m_headWaterMark;									// 0x16C HeadWaterMark
	AsciiString m_flagWaterMark;									// 0x170 FlagWaterMark
	AsciiString m_enabledImage;										// 0x174 EnabledImage
	AsciiString m_sideIconImage;									// 0x178 SideIconImage
	AsciiString m_beaconName;										// 0x17C BeaconName
	_STL::vector<AsciiString> m_initialUpgrades;					// 0x180 InitialUpgrades
	_STL::vector<AsciiString> m_buildableHeroesMP;					// 0x18C BuildableHeroesMP
	_STL::vector<AsciiString> m_buildableRingHeroesMP;				// 0x198 BuildableRingHeroesMP
	OpaqueRefCounted *m_lightPointsUpSound;							// 0x1A4 LightPointsUpSound (AudioEventRTS handle)
	OpaqueRefCounted *m_objectiveAddedSound;						// 0x1A8 ObjectiveAddedSound
	OpaqueRefCounted *m_objectiveCompletedSound;					// 0x1AC ObjectiveCompletedSound
	AsciiString m_defaultPlayerAIType;								// 0x1B0 DefaultPlayerAIType
	AsciiString m_spellBook;										// 0x1B4 SpellBook
	AsciiString m_spellBookMp;										// 0x1B8 SpellBookMp
	Bool m_evil;													// 0x1BC Evil
	AsciiString m_spellStoreCurrentPowerLabel;						// 0x1C0 SpellStoreCurrentPowerLabel
	AsciiString m_spellStoreMaximumPowerLabel;						// 0x1C4 SpellStoreMaximumPowerLabel
	UnsignedByte m_resourceModifierObjectFilter[4];					// 0x1C8 ResourceModifierObjectFilter (a refcounted filter handle)
	UnsignedByte m_resourceModifierValues[0xC];						// 0x1CC ResourceModifierValues (a vector of 32-bit values)
	AsciiString m_multiSelectionPortrait;							// 0x1D8 MultiSelectionPortrait
};
// SIZE 0x1DC

// The subsystem behind ThePlayerTemplateStore (VA 0x00DFE0D0).
//  - constructor 0x001FDB55: SubsystemInterface base, vtable 0x007E1BB0 (0x00BE1BB0), the template vector
//    at +0xC, a set at +0x18, then +0x24 = -1; destructor 0x001FDBA1.
//  - vtable slots: 0 deleting destructor 0x001FDD61, 1 init 0x001FF223, 4 attemptReload
//    0x001FD57A, 5 attemptRefresh 0x001FD5B2, 9 reset 0x001FD4A2, 10 update (empty);
//    the rest are the base's own bodies.
//  - +0x24: parsePlayerTemplateDefinition stores the index of the observer template there.
class PlayerTemplateStore : public SubsystemInterface
{
public:
	PlayerTemplateStore();											// 0x001FDB55
	virtual ~PlayerTemplateStore();									// 0x001FDBA1 (scalar-deleting 0x001FDD61)

	virtual void init();											// slot 1, 0x001FF223
	virtual Bool attemptReload(Bool *needsRestart);					// slot 4, 0x001FD57A
	virtual Bool attemptRefresh();									// slot 5, 0x001FD5B2
	virtual void reset();											// slot 9, 0x001FD4A2
	virtual void update() {}										// slot 10, empty (folded)

	Int getPlayerTemplateCount();									// 0x001FD2CA
	const PlayerTemplate *findPlayerTemplate(NameKeyType key) const;		// 0x001FD31B
	PlayerTemplate *findPlayerTemplateRecord(NameKeyType key);				// 0x001FD348 (map: Rva001FD348): the template whose key matches, without the final-override step
	Bool rva001FD367(Int sideId, Int *outIndex) const;				// 0x001FD367: index of the template whose side looks up to sideId
	Bool rva001FD4E1(NameKeyType key, Int *outIndex);				// 0x001FD4E1: index of the template named by key
	const PlayerTemplate *getNthPlayerTemplate(Int index) const;	// 0x001FD3C6

	const _STL::set<Int> &getPlayableIndices() const { return m_playableIndices; }
	static void parsePlayerTemplateDefinition(INI *ini);			// 0x001FEFEC

private:
	_STL::vector<PlayerTemplate> m_templates;						// 0x0C, by value (stride 0x1DC)
	_STL::set<Int> m_playableIndices;								// 0x18 indices of the playable templates (parsePlayerTemplateDefinition inserts into it)
	Int m_observerIndex;											// 0x24, -1 until a template with IsObserver is parsed
};
// SIZE 0x28

extern PlayerTemplateStore *ThePlayerTemplateStore;					// VA 0x00DFE0D0
// Set by a successful attemptReload and cleared by attemptRefresh (VA 0x00DFE0D4, no map name).
extern Bool g_playerTemplatesReloaded;

#endif
