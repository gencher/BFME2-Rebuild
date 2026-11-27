// OpenContainModuleData.h: the INI data of OpenContain. SIZE 0x98.
// Target facts: ModuleFactory registers the module with operator new 0x98 and
// constructor RVA 0x00465124; buildFieldParse RVA 0x0046523D adds VA 0x00C438B0 at +0x0 and DieMuxData at +0x8.
// Offsets are the table entries'; each type is what the entry's parse function stores
// (INI::parseReal -> Real, parseDurationUnsignedInt -> UnsignedInt, ...);
// names are the INI tokens. Byte gaps are bytes no table entry writes.
// Unresolved (byte storage up to the next member, named after the token):
//   m_enterSound +0x38: INI::parseAudioEventRTS 0x00339900
//   m_exitSound +0x3C: INI::parseAudioEventRTS 0x00339900
#pragma once
#ifndef BFME2_OPEN_CONTAIN_MODULE_DATA_H
#define BFME2_OPEN_CONTAIN_MODULE_DATA_H

#include <stddef.h>
#include <list>
#include <map>
#include <vector>
#include "Common/AsciiString.h"
#include "Common/BitFlags.h"
#include "Common/ModelConditionFlags.h"
#include "Common/GameType.h"
#include "GameLogic/Module/ModuleData.h"
#include "GameLogic/Module/DieMuxData.h"
#include "GameLogic/ObjectFilter.h"
#include "Common/OpaqueRefElement4.h"

class INI;
class MultiIniFieldParse;

// One PassengerBonePrefix entry: the PassengerBone name and the KindOf mask it serves.
struct PassengerBonePrefix
{
	KindOfMaskType m_kindOf;
	AsciiString m_passengerBone;
};

class OpenContainModuleData : public ModuleData
{
public:
	OpenContainModuleData();
	static void buildFieldParse(MultiIniFieldParse &parse);
	static ModuleData *friend_newModuleData(INI *ini);
	virtual Bool slot08() const { return true; }	// retail table 0x00843658 slot 8 is the shared `mov al,1; ret` stub

	DieMuxData m_dieMuxData;                               // +0x8; added by buildFieldParse at +0x8
	OpaqueRefElement4 m_enterSound;                        // +0x38 EnterSound (INI::parseAudioEventRTS 0x00339900); doLoadSound 0x0046279C passes its address to AudioEventRTS's constructor
	OpaqueRefElement4 m_exitSound;                         // +0x3C ExitSound; doUnloadSound 0x0046282B
	ObjectFilter m_passengerFilter;                        // +0x40 PassengerFilter
	ObjectFilter m_manualPickUpFilter;                     // +0x44 ManualPickUpFilter
	std::list<PassengerBonePrefix> m_passengerBonePrefixes;	// PassengerBonePrefix
	std::map<UnsignedInt, ModelConditionFlags> m_boneSpecificConditionStates;	// BoneSpecificConditionState: bone index to condition state
	ObjectStatusMaskType m_objectStatusOfContained;	// The containment INI parser parses ObjectStatusTypes into this mask.
	Real m_passengersTestCollisionHeight;                  // +0x68 PassengersTestCollisionHeight
	Real m_damagePercentToUnits;                           // +0x6C DamagePercentToUnits; percent
	Int m_containMax;                                      // +0x70 ContainMax
	Int m_numberOfExitPaths;                               // +0x74 NumberOfExitPaths
	UnsignedInt m_doorOpenTime;                            // +0x78 DoorOpenTime; duration, frames
	Bool m_allowOwnPlayerInsideOverride;                   // +0x7C AllowOwnPlayerInsideOverride
	Bool m_allowAlliesInside;                              // +0x7D AllowAlliesInside
	Bool m_allowEnemiesInside;                             // +0x7E AllowEnemiesInside
	Bool m_allowNeutralInside;                             // +0x7F AllowNeutralInside
	Bool m_showPips;                                       // +0x80 ShowPips
	Bool m_collidePickup;                                  // +0x81 CollidePickup
	Bool m_ejectPassengersOnDeath;                         // +0x82 EjectPassengersOnDeath
	Bool m_killPassengersOnDeath;                          // +0x83 KillPassengersOnDeath
	Bool m_enabled;                                        // +0x84 Enabled
	Bool m_passengersInTurret;                             // +0x85 PassengersInTurret
	Bool m_hasObjectStatusOfContained;                     // +0x86 cleared by the constructor 0x00465124, set by the ObjectStatusOfContained parse 0x00464081
	UnsignedByte m_unk0x87;
	_STL::vector<AsciiString> m_modifierToGiveOnExit;	// INI appends the named modifiers one at a time.
	UnsignedInt m_modifierRequiredTime;                    // +0x94 ModifierRequiredTime; duration, frames
};
// SIZE 0x98
typedef char OpenContainModuleData_size[(sizeof(OpenContainModuleData) == 0x98) ? 1 : -1];
typedef char OpenContainModuleData_off_m_dieMuxData[(offsetof(OpenContainModuleData, m_dieMuxData) == 0x8) ? 1 : -1];
typedef char OpenContainModuleData_off_m_enterSound[(offsetof(OpenContainModuleData, m_enterSound) == 0x38) ? 1 : -1];
typedef char OpenContainModuleData_off_m_exitSound[(offsetof(OpenContainModuleData, m_exitSound) == 0x3C) ? 1 : -1];
typedef char OpenContainModuleData_off_m_passengerFilter[(offsetof(OpenContainModuleData, m_passengerFilter) == 0x40) ? 1 : -1];
typedef char OpenContainModuleData_off_m_manualPickUpFilter[(offsetof(OpenContainModuleData, m_manualPickUpFilter) == 0x44) ? 1 : -1];
typedef char OpenContainModuleData_off_m_passengerBonePrefixes[(offsetof(OpenContainModuleData, m_passengerBonePrefixes) == 0x48) ? 1 : -1];
typedef char OpenContainModuleData_off_m_boneSpecificConditionStates[(offsetof(OpenContainModuleData, m_boneSpecificConditionStates) == 0x4C) ? 1 : -1];
typedef char OpenContainModuleData_off_m_objectStatusOfContained[(offsetof(OpenContainModuleData, m_objectStatusOfContained) == 0x58) ? 1 : -1];
typedef char OpenContainModuleData_off_m_passengersTestCollisionHeight[(offsetof(OpenContainModuleData, m_passengersTestCollisionHeight) == 0x68) ? 1 : -1];
typedef char OpenContainModuleData_off_m_damagePercentToUnits[(offsetof(OpenContainModuleData, m_damagePercentToUnits) == 0x6C) ? 1 : -1];
typedef char OpenContainModuleData_off_m_containMax[(offsetof(OpenContainModuleData, m_containMax) == 0x70) ? 1 : -1];
typedef char OpenContainModuleData_off_m_numberOfExitPaths[(offsetof(OpenContainModuleData, m_numberOfExitPaths) == 0x74) ? 1 : -1];
typedef char OpenContainModuleData_off_m_doorOpenTime[(offsetof(OpenContainModuleData, m_doorOpenTime) == 0x78) ? 1 : -1];
typedef char OpenContainModuleData_off_m_allowOwnPlayerInsideOverride[(offsetof(OpenContainModuleData, m_allowOwnPlayerInsideOverride) == 0x7C) ? 1 : -1];
typedef char OpenContainModuleData_off_m_allowAlliesInside[(offsetof(OpenContainModuleData, m_allowAlliesInside) == 0x7D) ? 1 : -1];
typedef char OpenContainModuleData_off_m_allowEnemiesInside[(offsetof(OpenContainModuleData, m_allowEnemiesInside) == 0x7E) ? 1 : -1];
typedef char OpenContainModuleData_off_m_allowNeutralInside[(offsetof(OpenContainModuleData, m_allowNeutralInside) == 0x7F) ? 1 : -1];
typedef char OpenContainModuleData_off_m_showPips[(offsetof(OpenContainModuleData, m_showPips) == 0x80) ? 1 : -1];
typedef char OpenContainModuleData_off_m_collidePickup[(offsetof(OpenContainModuleData, m_collidePickup) == 0x81) ? 1 : -1];
typedef char OpenContainModuleData_off_m_ejectPassengersOnDeath[(offsetof(OpenContainModuleData, m_ejectPassengersOnDeath) == 0x82) ? 1 : -1];
typedef char OpenContainModuleData_off_m_killPassengersOnDeath[(offsetof(OpenContainModuleData, m_killPassengersOnDeath) == 0x83) ? 1 : -1];
typedef char OpenContainModuleData_off_m_enabled[(offsetof(OpenContainModuleData, m_enabled) == 0x84) ? 1 : -1];
typedef char OpenContainModuleData_off_m_passengersInTurret[(offsetof(OpenContainModuleData, m_passengersInTurret) == 0x85) ? 1 : -1];
typedef char OpenContainModuleData_off_m_modifierToGiveOnExit[(offsetof(OpenContainModuleData, m_modifierToGiveOnExit) == 0x88) ? 1 : -1];
typedef char OpenContainModuleData_off_m_modifierRequiredTime[(offsetof(OpenContainModuleData, m_modifierRequiredTime) == 0x94) ? 1 : -1];

#endif
