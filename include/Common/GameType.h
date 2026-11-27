// GameType.h: SAGE basic types and the small enums every module unit shares.
#pragma once
#ifndef BFME2_GAMETYPE_H
#define BFME2_GAMETYPE_H

typedef bool Bool;
typedef char Char;
typedef unsigned char Byte;
typedef unsigned char UnsignedByte;
typedef short Short;
typedef unsigned short UnsignedShort;
typedef int Int;
typedef unsigned int UnsignedInt;
typedef __int64 Int64;
typedef float Real;
typedef unsigned short WideChar;

#ifndef TRUE
#define TRUE true
#endif
#ifndef FALSE
#define FALSE false
#endif

#ifndef NULL
#define NULL 0
#endif

// Zero Hour GameType.h / GameCommon.h spellings.
// The game's TimeOfDay name table (0x009BE958) lists NONE, MORNING, AFTERNOON,
// EVENING, NIGHT and INTERPOLATE at values 0..5; GlobalData's lighting tables
// have six rows (+0x140, +0x3C8, stride 0x6C).
enum TimeOfDay
{
	TIME_OF_DAY_INVALID = 0,
	TIME_OF_DAY_FIRST = 1,
	TIME_OF_DAY_MORNING = TIME_OF_DAY_FIRST,
	TIME_OF_DAY_AFTERNOON,
	TIME_OF_DAY_EVENING,
	TIME_OF_DAY_NIGHT,
	TIME_OF_DAY_INTERPOLATE,
	TIME_OF_DAY_COUNT
};

// GlobalData parses three lights per time of day (TerrainLighting<Time>Ambient,
// ...Ambient2, ...Ambient3).
enum { MAX_GLOBAL_LIGHTS = 3 };

enum ObjectID
{
	INVALID_ID = 0,
	FORCE_OBJECTID_TO_LONG_SIZE = 0x7ffffff
};

enum DrawableID
{
	INVALID_DRAWABLE_ID = 0,
	FORCE_DRAWABLEID_TO_LONG_SIZE = 0x7ffffff
};

enum NameKeyType
{
	NAMEKEY_INVALID = 0,
	NK_UNKNOWN = 0,
	NAMEKEY_MAX = 1 << 23,
	FORCE_NAMEKEY_LONG = 0x7fffffff
};

// UpdateModule.h (Zero Hour): sleep lengths returned by update().
enum UpdateSleepTime
{
	UPDATE_SLEEP_INVALID = 0,
	UPDATE_SLEEP_NONE = 1,
	UPDATE_SLEEP_FOREVER = 0x3fffffff
};

// BFME2's UPDATE_SLEEP is the identity: 0x0048C50D returns the frame delta
// itself, where Zero Hour's macro added one.
#define UPDATE_SLEEP(frames)	((UpdateSleepTime)(frames))

enum SleepyUpdatePhase
{
	PHASE_INITIAL = 0,
	PHASE_PHYSICS = 1,
	PHASE_NORMAL = 2,
	PHASE_FINAL = 3
};

// Which turret of a multi-turret object a query addresses; initLaser asks for
// the main turret's fire bone.
enum WhichTurretType
{
	TURRET_INVALID = -1,
	TURRET_MAIN = 0
};

// Zero Hour GameCommon.h GameMode, only the evidenced values (ControlBar::setControlBarSchemeByPlayer 0x0031C40E tests 1 and 5; the GameLogic constructor stores 9).
enum GameMode { GAME_SINGLE_PLAYER = 0, GAME_LAN = 1, GAME_INTERNET = 5 };

// Zero Hour GameCommon.h: who issued a command.
enum CommandSourceType
{
	CMD_FROM_PLAYER = 0,
	CMD_FROM_SCRIPT = 1,
	CMD_FROM_AI = 2,
	CMD_FROM_DOZER = 3,
	CMD_DEFAULT_SWITCH_WEAPON = 4
};

// Whether a template may be built: ThingTemplate's Buildable index list (+0x5F2) and
// GameLogic's per-template overrides. Values and order are retail's name table at
// VA 0x00DBEB20: Yes, Ignore_Prerequisites, No, Only_By_AI.
enum BuildableStatus
{
	BSTATUS_YES = 0,
	BSTATUS_IGNORE_PREREQUISITES = 1,
	BSTATUS_NO = 2,
	BSTATUS_ONLY_BY_AI = 3
};

// Object status bits (BitFlags<101> at Object+0x94). Values and spellings are retail's
// INI name table for BitFlags<101> (s_bitNameList, VA 0x00DA5F30, 101 entries), with
// the OBJECT_STATUS_ prefix. BFME2's numbering differs from Zero Hour's (no NONE entry).
enum ObjectStatusTypes
{
	OBJECT_STATUS_NONE = 0,	// kept for existing callers; table entry 0 is DESTROYED
	OBJECT_STATUS_DESTROYED = 0,
	OBJECT_STATUS_CAN_ATTACK = 1,
	OBJECT_STATUS_UNDER_CONSTRUCTION = 2,	// CaveContain::onDie 0x00466724 tests it
	OBJECT_STATUS_UNSELECTABLE = 3,
	OBJECT_STATUS_NO_COLLISIONS = 4,
	OBJECT_STATUS_NO_ATTACK = 5,
	OBJECT_STATUS_AIRBORNE_TARGET = 6,
	OBJECT_STATUS_PARACHUTING = 7,
	OBJECT_STATUS_REPULSOR = 8,
	OBJECT_STATUS_HIJACKED = 9,
	OBJECT_STATUS_AFLAME = 10,
	OBJECT_STATUS_BURNED = 11,
	OBJECT_STATUS_WET = 12,
	OBJECT_STATUS_IS_FIRING_WEAPON = 13,
	OBJECT_STATUS_IS_BRAKING = 14,
	OBJECT_STATUS_STEALTHED = 15,
	OBJECT_STATUS_HIDDEN = 16,
	OBJECT_STATUS_DETECTED = 17,
	OBJECT_STATUS_CAN_STEALTH = 18,
	OBJECT_STATUS_SOLD = 19,
	OBJECT_STATUS_UNDERGOING_REPAIR = 20,
	OBJECT_STATUS_RECONSTRUCTING = 21,
	OBJECT_STATUS_IS_ATTACKING = 22,	// RadiusDecalUpdate::update 0x0039157B tests it
	OBJECT_STATUS_NO_AUTO_ACQUIRE = 23,
	OBJECT_STATUS_USING_ABILITY = 24,
	OBJECT_STATUS_IS_AIMING_WEAPON = 25,
	OBJECT_STATUS_NO_ATTACK_FROM_AI = 26,
	OBJECT_STATUS_IGNORING_STEALTH = 27,
	OBJECT_STATUS_IS_MELEE_ATTACKING = 28,
	OBJECT_STATUS_GUARD_SELECTION = 29,
	OBJECT_STATUS_LEASHED_RETURNING = 30,
	OBJECT_STATUS_DEATH_1 = 31,
	OBJECT_STATUS_DEATH_2 = 32,
	OBJECT_STATUS_DEATH_3 = 33,
	OBJECT_STATUS_DEATH_4 = 34,
	OBJECT_STATUS_DEATH_5 = 35,
	OBJECT_STATUS_CONTESTED = 36,
	OBJECT_STATUS_CONTESTING_BUILDING = 37,
	OBJECT_STATUS_HORDE_MEMBER = 38,
	OBJECT_STATUS_RIDERLESS = 39,
	OBJECT_STATUS_RIDER_IS_PILOT = 40,
	OBJECT_STATUS_RIDER1 = 41,
	OBJECT_STATUS_RIDER2 = 42,
	OBJECT_STATUS_RIDER3 = 43,
	OBJECT_STATUS_RIDER4 = 44,
	OBJECT_STATUS_RIDER5 = 45,
	OBJECT_STATUS_RIDER6 = 46,
	OBJECT_STATUS_RIDER7 = 47,
	OBJECT_STATUS_RIDER8 = 48,
	OBJECT_STATUS_IMMOBILE = 49,
	OBJECT_STATUS_FLEE_OFF_MAP = 50,
	OBJECT_STATUS_NOT_IN_WORLD = 51,
	OBJECT_STATUS_INAUDIBLE = 52,
	OBJECT_STATUS_CHANTING = 53,
	OBJECT_STATUS_ENRAGED = 54,
	OBJECT_STATUS_CREATE_DRAWABLE_WITH_LOW_DETAIL = 55,
	OBJECT_STATUS_SINKING = 56,
	OBJECT_STATUS_RAMPAGING = 57,
	OBJECT_STATUS_INSIDE_GARRISON = 58,	// WeaponTemplate 0x002C98E2 applies GarrisonedRangeMultiplier
	OBJECT_STATUS_DEPLOYED = 59,
	OBJECT_STATUS_UNATTACKABLE = 60,
	OBJECT_STATUS_ENCLOSED = 61,
	OBJECT_STATUS_TEMPORARILY_DEFECTED = 62,
	OBJECT_STATUS_TAGGED = 63,
	OBJECT_STATUS_DEPLOYING = 64,
	OBJECT_STATUS_BLOODTHIRSTY = 65,
	OBJECT_STATUS_PORTER_TAGGED = 66,
	OBJECT_STATUS_GRAB_AND_DROP = 67,
	OBJECT_STATUS_STAND_GROUND = 68,
	OBJECT_STATUS_UNCONTROLLABLY_SCARED = 69,
	OBJECT_STATUS_SPECIAL_ABILITY_PACKING_UNPACKING_OR_USING = 70,
	OBJECT_STATUS_PLEASE_EAT_ME = 71,
	OBJECT_STATUS_UPDATING_AI = 72,
	OBJECT_STATUS_HUNT_WHEN_IDLE = 73,
	OBJECT_STATUS_IGNORE_AI_COMMAND = 74,
	OBJECT_STATUS_RUNNING_DOWN_FROM_BEHIND = 75,
	OBJECT_STATUS_DO_NOT_SCORE = 76,
	OBJECT_STATUS_CAN_NOT_WALK_ON = 77,
	OBJECT_STATUS_MARCH_OF_DEATH = 78,
	OBJECT_STATUS_DO_NOT_PICK_ME = 79,
	OBJECT_STATUS_INHERITED_FROM_ALLY_TEAM = 80,
	OBJECT_STATUS_SWITCHED_WEAPONS = 81,
	OBJECT_STATUS_END_FIRE_STATE = 82,
	OBJECT_STATUS_BOOKENDING = 83,
	OBJECT_STATUS_ELVISH_EXPRESSLY = 84,
	OBJECT_STATUS_INSIDE_CASTLE = 85,
	OBJECT_STATUS_BUILD_BEING_CANCELED = 86,
	OBJECT_STATUS_PENDING_CONSTRUCTION = 87,
	OBJECT_STATUS_PHANTOM_STRUCTURE = 88,
	OBJECT_STATUS_IN_FORMATION_TEMPLATE = 89,
	OBJECT_STATUS_IS_LEAVING_FACTORY = 90,
	OBJECT_STATUS_MOVING_TO_DISMOUNT = 91,
	OBJECT_STATUS_NO_HERO_PROPERTIES = 92,
	OBJECT_STATUS_CAN_ENTER_ANYTHING = 93,
	OBJECT_STATUS_HOLDING_THE_RING = 94,
	OBJECT_STATUS_INVISIBLE_DETECTED_BY_FRIEND = 95,
	OBJECT_STATUS_INVISIBLE_DETECTED = 96,
	OBJECT_STATUS_WORKER_REPAIRING = 97,
	OBJECT_STATUS_ATTACHED = 98,
	OBJECT_STATUS_WONT_RIDE_WITH_YOU = 99,
	OBJECT_STATUS_COMMAND_BUTTON_TOGGLED = 100,

	OBJECT_STATUS_COUNT = 101,

	// Numeric spellings used before the table was read (same values):
	OBJECT_STATUS_41 = OBJECT_STATUS_BLOODTHIRSTY,	// set on a unit while it claims a sacrifice (BloodthirstyUpdate)
	OBJECT_STATUS_46 = OBJECT_STATUS_SPECIAL_ABILITY_PACKING_UNPACKING_OR_USING,	// CanBeActivatedFor 0x004DD018 rejects the object carrying it
	OBJECT_STATUS_4A = OBJECT_STATUS_IGNORE_AI_COMMAND	// set on the sacrifice victim (BloodthirstyUpdate)
};

// Object::getRelationship 0x0028D156 returns 0 on its enemies path, 1 when this
// object is an undetected defector and 2 when the other one is; Player::
// getRelationship 0x002AC3E0 returns 1 when no relation is recorded.
enum Relationship
{
	ENEMIES = 0,
	NEUTRAL = 1,
	ALLIES = 2
};

// Model condition bits; named by number until recovered.
enum ModelConditionFlagType
{
	MODELCONDITION_INVALID = -1,
	// The game's ModelConditionFlag name table (0x009BAA98) has GARRISONED at 10;
	// CaveContain::onRemoving 0x004668DC clears it on the object it lets out.
	MODELCONDITION_GARRISONED = 10,
    MODELCONDITION_RUBBLE_RISE_START=59,
    MODELCONDITION_RUBBLE_RISE_DONE=60,
	MODELCONDITION_40 = 0x40,	// set on a horde with a sacrifice kill (BloodthirstyUpdate)
	MODELCONDITION_RIDERLESS = 297,	// DetachableRiderUpdate sets it when the rider is gone
	MODELCONDITION_DISGUISED = 300,	// set while SpecialDisguiseUpdate disguises the unit
	// WeaponFireSpecialAbilityUpdate's per-weapon conditions (WhichSpecialWeapon 1-6).
	MODELCONDITION_244 = 7 * 32 + 20,
	MODELCONDITION_245 = 7 * 32 + 21,
	MODELCONDITION_246 = 7 * 32 + 22,
	MODELCONDITION_588 = 18 * 32 + 12,
	MODELCONDITION_589 = 18 * 32 + 13,
	MODELCONDITION_590 = 18 * 32 + 14,
	// FireWeaponUpdate's HeroModeTrigger and ChargingModeTrigger tests (0x0048BEE8).
	MODELCONDITION_132 = 4 * 32 + 4,
	MODELCONDITION_220 = 6 * 32 + 28,
	// PhysicsBehavior's flight-completion conditions (0x00390FAF).
	MODELCONDITION_62 = 62,
	MODELCONDITION_72 = 72,
	MODELCONDITION_122 = 122,
	MODELCONDITION_127 = 127,
	MODELCONDITION_128 = 128,
	MODELCONDITION_163 = 163,
	// The name table's 144..148: WeaponSet::setWeaponLock 0x002C8AAE clears all five and sets
	// the locked slot's.
	MODELCONDITION_WEAPONLOCK_PRIMARY = 144,
	MODELCONDITION_WEAPONLOCK_SECONDARY,
	MODELCONDITION_WEAPONLOCK_TERTIARY,
	MODELCONDITION_WEAPONLOCK_QUATERNARY,
	MODELCONDITION_WEAPONLOCK_QUINARY
};

// Zero Hour BodyModule.h: damage states.
enum BodyDamageType
{
	BODY_PRISTINE = 0,
	BODY_DAMAGED,
	BODY_REALLYDAMAGED,
	BODY_RUBBLE,
	BODYDAMAGETYPE_COUNT
};

// Zero Hour ParticleSys.h.
enum ParticleSystemID
{
	INVALID_PARTICLE_SYSTEM_ID = 0,
	FORCE_PARTICLESYSTEMID_TO_LONG_SIZE = 0x7ffffff
};

#endif
