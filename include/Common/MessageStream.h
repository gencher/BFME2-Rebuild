// MessageStream.h: SAGE game messages and the lists that carry them
// (GameMessageList, MessageStream, CommandList). BFME2's message numbering
// differs from Zero Hour's and message types keep their numbers.
#pragma once
#ifndef BFME2_MESSAGESTREAM_H
#define BFME2_MESSAGESTREAM_H

#include "Common/GameType.h"
#include "Common/GameMessageArgument.h"
#include "Common/Player.h"
#include "Common/PlayerList.h"
#include "Common/SubsystemInterface.h"
class AsciiString;
class GameMessageList;

class GameMessage
{
public:
	enum Type
	{
		// Values and names: retail GameMessage::getCommandTypeAsAsciiString
		// 0x0030FA44, a switch from each value to its MSG_ name string, read
		// value by value. All 360 named values are
		// those of that function. Values the
		// switch does not name keep a numeric spelling, with the code that uses
		// them. BFME2 renumbered Zero Hour's list, so no entry is placed by its
		// donor position.
		MSG_INVALID = 0x0,
		MSG_RAW_MOUSE_BEGIN = 0x2,
		MSG_RAW_MOUSE_POSITION = 0x3,
		MSG_RAW_MOUSE_LEFT_BUTTON_DOWN = 0x4,
		MSG_RAW_MOUSE_LEFT_DOUBLE_CLICK = 0x5,
		MSG_RAW_MOUSE_LEFT_BUTTON_UP = 0x6,
		MSG_RAW_MOUSE_LEFT_CLICK = 0x7,
		MSG_RAW_MOUSE_LEFT_DRAG = 0x8,
		MSG_RAW_MOUSE_MIDDLE_BUTTON_DOWN = 0xA,
		MSG_RAW_MOUSE_MIDDLE_DOUBLE_CLICK = 0xB,
		MSG_RAW_MOUSE_MIDDLE_BUTTON_UP = 0xC,
		MSG_RAW_MOUSE_MIDDLE_DRAG = 0xD,
		MSG_RAW_MOUSE_RIGHT_BUTTON_DOWN = 0xE,
		MSG_RAW_MOUSE_RIGHT_DOUBLE_CLICK = 0xF,
		MSG_RAW_MOUSE_RIGHT_BUTTON_UP = 0x10,
		MSG_RAW_MOUSE_RIGHT_DRAG = 0x12,
		MSG_RAW_MOUSE_WHEEL = 0x13,
		MSG_RAW_MOUSE_END = 0x14,
		MSG_RAW_KEY_DOWN = 0x15,
		MSG_RAW_KEY_UP = 0x16,
		MSG_MOUSE_LEFT_CLICK = 0x17,
		MSG_MOUSE_LEFT_DOUBLE_CLICK = 0x18,
		MSG_MOUSE_MIDDLE_CLICK = 0x19,
		MSG_MOUSE_MIDDLE_DOUBLE_CLICK = 0x1A,
		MSG_MOUSE_RIGHT_CLICK = 0x1B,
		MSG_MOUSE_RIGHT_DOUBLE_CLICK = 0x1C,
		MSG_CLEAR_GAME_DATA = 0x1D,
		MSG_NEW_GAME = 0x1E,
		MSG_START_LIVING_WORLD_LOGIC = 0x1F,
		MSG_START_LINEAR_CAMPAIGN = 0x20,
		MSG_CURRENT_LINEAR_CAMPAIGN_MISSION_COMPLETE = 0x21,
		MSG_EXIT_LINEAR_CAMPAIGN = 0x22,
		MSG_BEGIN_META_MESSAGES = 0x23,
		MSG_META_SAVE_VIEW1 = 0x24,
		MSG_META_SAVE_VIEW2 = 0x25,
		MSG_META_SAVE_VIEW3 = 0x26,
		MSG_META_SAVE_VIEW4 = 0x27,
		MSG_META_SAVE_VIEW5 = 0x28,
		MSG_META_SAVE_VIEW6 = 0x29,
		MSG_META_SAVE_VIEW7 = 0x2A,
		MSG_META_SAVE_VIEW8 = 0x2B,
		MSG_META_VIEW_VIEW1 = 0x2C,
		MSG_META_VIEW_VIEW2 = 0x2D,
		MSG_META_VIEW_VIEW3 = 0x2E,
		MSG_META_VIEW_VIEW4 = 0x2F,
		MSG_META_VIEW_VIEW5 = 0x30,
		MSG_META_VIEW_VIEW6 = 0x31,
		MSG_META_VIEW_VIEW7 = 0x32,
		MSG_META_VIEW_VIEW8 = 0x33,
		MSG_META_CREATE_TEAM0 = 0x34,
		MSG_META_CREATE_TEAM1 = 0x35,
		MSG_META_CREATE_TEAM2 = 0x36,
		MSG_META_CREATE_TEAM3 = 0x37,
		MSG_META_CREATE_TEAM4 = 0x38,
		MSG_META_CREATE_TEAM5 = 0x39,
		MSG_META_CREATE_TEAM6 = 0x3A,
		MSG_META_CREATE_TEAM7 = 0x3B,
		MSG_META_CREATE_TEAM8 = 0x3C,
		MSG_META_CREATE_TEAM9 = 0x3D,
		MSG_META_SELECT_TEAM0 = 0x3E,
		MSG_META_SELECT_TEAM1 = 0x3F,
		MSG_META_SELECT_TEAM2 = 0x40,
		MSG_META_SELECT_TEAM3 = 0x41,
		MSG_META_SELECT_TEAM4 = 0x42,
		MSG_META_SELECT_TEAM5 = 0x43,
		MSG_META_SELECT_TEAM6 = 0x44,
		MSG_META_SELECT_TEAM7 = 0x45,
		MSG_META_SELECT_TEAM8 = 0x46,
		MSG_META_SELECT_TEAM9 = 0x47,
		MSG_META_ADD_TEAM0 = 0x48,
		MSG_META_ADD_TEAM1 = 0x49,
		MSG_META_ADD_TEAM2 = 0x4A,
		MSG_META_ADD_TEAM3 = 0x4B,
		MSG_META_ADD_TEAM4 = 0x4C,
		MSG_META_ADD_TEAM5 = 0x4D,
		MSG_META_ADD_TEAM6 = 0x4E,
		MSG_META_ADD_TEAM7 = 0x4F,
		MSG_META_ADD_TEAM8 = 0x50,
		MSG_META_ADD_TEAM9 = 0x51,
		MSG_META_VIEW_TEAM0 = 0x52,
		MSG_META_VIEW_TEAM1 = 0x53,
		MSG_META_VIEW_TEAM2 = 0x54,
		MSG_META_VIEW_TEAM3 = 0x55,
		MSG_META_VIEW_TEAM4 = 0x56,
		MSG_META_VIEW_TEAM5 = 0x57,
		MSG_META_VIEW_TEAM6 = 0x58,
		MSG_META_VIEW_TEAM7 = 0x59,
		MSG_META_VIEW_TEAM8 = 0x5A,
		MSG_META_VIEW_TEAM9 = 0x5B,
		MSG_META_SELECT_MATCHING_UNITS = 0x5C,
		MSG_META_SELECT_NEXT_UNIT = 0x5D,
		MSG_META_SELECT_PREV_UNIT = 0x5E,
		MSG_META_SELECT_NEXT_WORKER = 0x5F,
		MSG_META_SELECT_PREV_WORKER = 0x60,
		MSG_META_VIEW_HOME_BASE = 0x61,
		MSG_META_VIEW_LAST_RADAR_EVENT = 0x62,
		MSG_META_SELECT_HERO = 0x63,
		MSG_META_SELECT_ALL = 0x64,
		MSG_META_SCATTER = 0x65,
		MSG_META_STOP = 0x66,
		MSG_META_DEPLOY = 0x67,
		MSG_META_CREATE_FORMATION = 0x68,
		MSG_META_AUTO_SAVE = 0x69,
		MSG_META_FOLLOW = 0x6A,
		MSG_META_CHAT_PLAYERS = 0x6B,
		MSG_META_CHAT_BUDDIES = 0x6C,
		MSG_META_CHAT_ALLIES = 0x6D,
		MSG_META_CHAT_EVERYONE = 0x6E,
		MSG_META_DIPLOMACY = 0x6F,
		MSG_META_OPTIONS = 0x70,
		MSG_META_TOGGLE_CONTROL_BAR = 0x71,
		MSG_META_BEGIN_PATH_BUILD = 0x72,
		MSG_META_END_PATH_BUILD = 0x73,
		MSG_META_BEGIN_FORCEATTACK = 0x74,
		MSG_META_END_FORCEATTACK = 0x75,
		MSG_META_BEGIN_FORCEMOVE = 0x76,
		MSG_META_END_FORCEMOVE = 0x77,
		MSG_META_BEGIN_WAYPOINTS = 0x78,
		MSG_META_END_WAYPOINTS = 0x79,
		MSG_META_BEGIN_PREFER_SELECTION = 0x7A,
		MSG_META_END_PREFER_SELECTION = 0x7B,
		MSG_META_TAKE_SCREENSHOT = 0x7C,
		MSG_META_ALL_CHEER = 0x7D,
		MSG_META_TOGGLE_ATTACKMOVE = 0x7E,
		MSG_META_BEGIN_CAMERA_ROTATE_LEFT = 0x7F,
		MSG_META_END_CAMERA_ROTATE_LEFT = 0x80,
		MSG_META_BEGIN_CAMERA_ROTATE_RIGHT = 0x81,
		MSG_META_END_CAMERA_ROTATE_RIGHT = 0x82,
		MSG_META_BEGIN_CAMERA_ZOOM_IN = 0x83,
		MSG_META_END_CAMERA_ZOOM_IN = 0x84,
		MSG_META_BEGIN_CAMERA_ZOOM_OUT = 0x85,
		MSG_META_END_CAMERA_ZOOM_OUT = 0x86,
		MSG_META_CAMERA_RESET = 0x87,
		MSG_META_BEGIN_CAMERA_SCROLL_LEFT = 0x88,
		MSG_META_END_CAMERA_SCROLL_LEFT = 0x89,
		MSG_META_BEGIN_CAMERA_SCROLL_RIGHT = 0x8A,
		MSG_META_END_CAMERA_SCROLL_RIGHT = 0x8B,
		MSG_META_BEGIN_CAMERA_SCROLL_UP = 0x8C,
		MSG_META_END_CAMERA_SCROLL_UP = 0x8D,
		MSG_META_BEGIN_CAMERA_SCROLL_DOWN = 0x8E,
		MSG_META_END_CAMERA_SCROLL_DOWN = 0x8F,
		MSG_META_PLACE_BEACON = 0x90,
		MSG_META_DELETE_BEACON = 0x91,
		MSG_META_SPELL_STORE = 0x92,
		MSG_META_STANCE_AGGRESSIVE = 0x93,
		MSG_META_STANCE_HOLDGROUND = 0x94,
		MSG_META_STANCE_BATTLE = 0x95,
		MSG_META_ORDERMODE_IMMEDIATE = 0x96,
		MSG_META_ORDERMODE_WAYPOINT = 0x97,
		MSG_META_ORDER_SYNCHRONIZE = 0x98,
		MSG_META_TOGGLE_PLANNING_MODE = 0x99,
		MSG_META_SELL = 0x9A,
		MSG_META_DEMO_PERFORM_STATISTICAL_DUMP = 0x9B,
		MSG_META_TOGGLE_FAST_FORWARD_MODE = 0x9C,
		MSG_META_RELOAD_RAPID_ITERATION_FEATURE = 0x9D,
		MSG_META_REFRESH_RAPID_ITERATION_FEATURE = 0x9E,
		MSG_META_DEBUG_TOGGLE_SHOWVISIONEGGS = 0x9F,
		MSG_META_DEBUG_TOGGLE_STRINGTAGS = 0xA0,
		MSG_END_META_MESSAGES = 0xA1,
		MSG_MOUSEOVER_DRAWABLE_HINT = 0xA2,
		MSG_MOUSEOVER_LOCATION_HINT = 0xA3,
		MSG_VALID_GUICOMMAND_HINT = 0xA4,
		MSG_INVALID_GUICOMMAND_HINT = 0xA5,
		MSG_AREA_SELECTION_HINT = 0xA6,
		MSG_DO_ATTACK_OBJECT_HINT = 0xA7,
		MSG_IMPOSSIBLE_ATTACK_HINT = 0xA8,	// no name string; Zero Hour's slot between the two attack hints
		MSG_DO_FORCE_ATTACK_OBJECT_HINT = 0xA9,
		MSG_DO_FORCE_ATTACK_GROUND_HINT = 0xAA,
		MSG_GET_REPAIRED_HINT = 0xAB,
		MSG_GET_HEALED_HINT = 0xAC,
		MSG_DO_REPAIR_HINT = 0xAD,
		MSG_RESUME_CONSTRUCTION_HINT = 0xAE,
		MSG_ENTER_HINT = 0xAF,
		MSG_CONTEST_HINT = 0xB0,
		MSG_DOCK_HINT = 0xB1,
		MSG_HARVEST_HINT = 0xB2,
		MSG_DO_MOVETO_HINT = 0xB3,
		MSG_DO_ATTACKMOVETO_HINT = 0xB4,
		MSG_ADD_WAYPOINT_HINT = 0xB5,
		MSG_HIJACK_HINT = 0xB6,
		MSG_FIREBOMB_HINT = 0xB7,
		MSG_CONVERT_TO_CARBOMB_HINT = 0xB8,
		MSG_CAPTUREBUILDING_HINT = 0xB9,
		MSG_SNIPE_VEHICLE_HINT = 0xBA,
		MSG_DEFECTOR_HINT = 0xBB,
		MSG_SET_RALLY_POINT_HINT = 0xBC,
		MSG_DO_SALVAGE_HINT = 0xBE,
		MSG_DO_INVALID_HINT = 0xBF,
		MSG_DO_ATTACK_OBJECT_AFTER_MOVING_HINT = 0xC0,
		MSG_0xC1 = 0xC1,	// no name string; HintSpyTranslator::translateGameMessage 0x00428C43 handles it with the command hints
		MSG_0xC2 = 0xC2,	// no name string; HintSpyTranslator::translateGameMessage 0x00428C43 handles it with the command hints
		MSG_APT_SKIP_NEXT_MSG = 0xC3,
		MSG_BEGIN_NETWORK_MESSAGES = 0x3E8,
		MSG_CREATE_SELECTED_GROUP = 0x3E9,
		MSG_CREATE_SELECTED_GROUP_NO_SOUND = 0x3EA,
		MSG_CREATE_SELECTED_GROUP_IDLE_WORKER_VOICE = 0x3EB,
		MSG_DESTROY_SELECTED_GROUP = 0x3EC,
		MSG_REMOVE_FROM_SELECTED_GROUP = 0x3ED,
		MSG_CREATE_TEAM0 = 0x3EE,
		MSG_CREATE_TEAM1 = 0x3EF,
		MSG_CREATE_TEAM2 = 0x3F0,
		MSG_CREATE_TEAM3 = 0x3F1,
		MSG_CREATE_TEAM4 = 0x3F2,
		MSG_CREATE_TEAM5 = 0x3F3,
		MSG_CREATE_TEAM6 = 0x3F4,
		MSG_CREATE_TEAM7 = 0x3F5,
		MSG_CREATE_TEAM8 = 0x3F6,
		MSG_CREATE_TEAM9 = 0x3F7,
		MSG_SELECT_TEAM0 = 0x3F8,
		MSG_SELECT_TEAM1 = 0x3F9,
		MSG_SELECT_TEAM2 = 0x3FA,
		MSG_SELECT_TEAM3 = 0x3FB,
		MSG_SELECT_TEAM4 = 0x3FC,
		MSG_SELECT_TEAM5 = 0x3FD,
		MSG_SELECT_TEAM6 = 0x3FE,
		MSG_SELECT_TEAM7 = 0x3FF,
		MSG_SELECT_TEAM8 = 0x400,
		MSG_SELECT_TEAM9 = 0x401,
		MSG_ADD_TEAM0 = 0x402,
		MSG_ADD_TEAM1 = 0x403,
		MSG_ADD_TEAM2 = 0x404,
		MSG_ADD_TEAM3 = 0x405,
		MSG_ADD_TEAM4 = 0x406,
		MSG_ADD_TEAM5 = 0x407,
		MSG_ADD_TEAM6 = 0x408,
		MSG_ADD_TEAM7 = 0x409,
		MSG_ADD_TEAM8 = 0x40A,
		MSG_ADD_TEAM9 = 0x40B,
		MSG_DO_ATTACKSQUAD = 0x40C,
		MSG_DO_WEAPON = 0x40D,
		MSG_DO_WEAPON_AT_LOCATION = 0x40E,
		MSG_DO_WEAPON_AT_OBJECT = 0x40F,
		MSG_DO_SPECIAL_POWER = 0x410,
		MSG_DO_SPECIAL_POWER_AT_LOCATION = 0x411,
		MSG_DO_SPECIAL_POWER_AT_OBJECT = 0x412,
		MSG_SET_RALLY_POINT = 0x413,
		MSG_PURCHASE_SCIENCE = 0x414,
		MSG_QUEUE_UPGRADE = 0x415,
		MSG_CANCEL_UPGRADE = 0x416,
		MSG_QUEUE_UNIT_CREATE = 0x417,
		MSG_CANCEL_UNIT_CREATE = 0x418,
		MSG_FOUNDATION_CONSTRUCT = 0x419,
		MSG_DOZER_CONSTRUCT = 0x41A,
		MSG_DOZER_CANCEL_CONSTRUCT = 0x41B,
		MSG_SELL = 0x41C,
		MSG_EXIT = 0x41D,
		MSG_EVACUATE = 0x41E,
		MSG_EVACUATE_CONTESTERS = 0x41F,
		MSG_SACRIFICE = 0x420,
		MSG_COMBATDROP_AT_LOCATION = 0x421,
		MSG_COMBATDROP_AT_OBJECT = 0x422,
		MSG_COMBINE_HORDES_WITH_OBJECT = 0x423,
		MSG_AREA_SELECTION = 0x424,
		MSG_DO_ATTACK_OBJECT = 0x425,
		MSG_DO_FORCE_ATTACK_OBJECT = 0x426,
		MSG_DO_FORCE_ATTACK_GROUND = 0x427,
		MSG_GET_REPAIRED = 0x428,
		MSG_GET_HEALED = 0x429,
		MSG_DO_REPAIR = 0x42A,
		MSG_RESUME_CONSTRUCTION = 0x42B,
		MSG_ENTER = 0x42C,
		MSG_DOCK = 0x42D,
		MSG_HARVEST = 0x42E,
		MSG_DO_MOVETO = 0x42F,
		MSG_DO_ATTACKMOVETO = 0x430,
		MSG_DO_FORCEMOVETO = 0x431,
		MSG_ADD_WAYPOINT = 0x432,
		MSG_DO_GUARD_POSITION = 0x433,
		MSG_DO_GUARD_OBJECT = 0x434,
		MSG_DO_STOP = 0x435,
		MSG_DO_SCATTER = 0x436,
		MSG_OPEN_GATE = 0x437,
		MSG_DO_CHEER = 0x438,
		MSG_CLOSE_GATE = 0x439,
		MSG_SWITCH_WEAPONS = 0x43A,
		MSG_CONVERT_TO_CARBOMB = 0x43B,
		MSG_CAPTUREBUILDING = 0x43C,
		MSG_CASTLE_UNPACK = 0x43D,
		MSG_CASTLE_PACK = 0x43E,
		MSG_CASTLE_UNPACK_EXPLICIT_OBJECT = 0x43F,
		MSG_SNIPE_VEHICLE = 0x440,
		MSG_DO_SPECIAL_POWER_OVERRIDE_DESTINATION = 0x441,
		MSG_DO_SALVAGE = 0x442,
		MSG_CLEAR_INGAME_POPUP_MESSAGE = 0x443,
		MSG_PLACE_BEACON = 0x444,
		MSG_REMOVE_BEACON = 0x445,
		MSG_SET_BEACON_TEXT = 0x446,
		MSG_SET_REPLAY_CAMERA = 0x447,
		MSG_SELF_DESTRUCT = 0x448,
		MSG_CREATE_FORMATION = 0x449,
		MSG_LOGIC_CRC = 0x44A,
		MSG_SET_MINE_CLEARING_DETAIL = 0x44B,
		MSG_DO_USER1 = 0x44C,
		MSG_DO_USER2 = 0x44D,
		MSG_DO_USER3 = 0x44E,
		MSG_DO_USER4 = 0x44F,
		MSG_MOVE_ARMY_TO_POSITION = 0x450,
		MSG_AUTO_SAVE = 0x451,
		MSG_CHANGE_CAMERA_ARRIVED_AT_WAYPOINTID = 0x452,
		MSG_HORDE_TOGGLE_FORMATION = 0x453,
		MSG_ONE_RING = 0x454,
		MSG_CREW_EVACUATE = 0x455,
		MSG_DO_SPELLBOOK_SPECIAL_POWER = 0x456,
		MSG_WEAPONSET_TOGGLE = 0x457,
		MSG_DO_AUTO_ABILITY = 0x458,
		MSG_DO_AUTO_ABILITY_WEAPON = 0x459,
		MSG_REVIVE = 0x45A,
		MSG_TOGGLE_NO_AUTO_ACQUIRE = 0x45B,
		MSG_WAKE_AUTO_PICKUP = 0x45C,
		MSG_START_SELF_REPAIR = 0x45D,
		MSG_SUMMON_REINFORCEMENTS = 0x45E,
		MSG_CALL_IN_REINFORCEMENTS = 0x45F,
		MSG_HORDE_SET_FORMATION = 0x460,
		MSG_CREATE_SELECT_ALL_GROUP = 0x461,
		MSG_ENABLE_RETALIATION_MODE = 0x462,
		MSG_WALL_HUB_CONSTRUCT_SPAN = 0x463,
		MSG_DO_MOVETO_FORMATION = 0x464,
		MSG_DO_MOVE_AND_ORIENTATE_OBJECTTO = 0x465,
		MSG_GIVE_MONEY = 0x466,
		MSG_DO_ROTATE_FIRINGARC = 0x467,
		MSG_CHANGE_STANCE = 0x468,
		MSG_CHANGE_ORDERMODE = 0x469,
		MSG_ACTIONQUEUE_EXECUTE_PLANNED = 0x46A,
		MSG_ACTIONQUEUE_CLEAR_ALL = 0x46B,
		MSG_ACTIONQUEUE_CLEAR_PLANNED = 0x46C,
		MSG_ACTIONQUEUE_CLEAR_LAST = 0x46D,
		MSG_START_NEIGHBORHOOD_REPAIR = 0x46E,
		MSG_CANCEL_NEIGHBORHOOD = 0x46F,
		MSG_ORDER_SYNCHRONIZE = 0x470,
		MSG_ADD_ALL_FACTION_UPGRADE = 0x471,
		MSG_LW_DONE_SESSION = 0x6A5,
		MSG_LW_DONE_PHASE = 0x6A6,
		MSG_LW_SET_CONFLICT_RESOLUTION_METHOD = 0x6A7,
		MSG_LW_ARMY_MOVE_REQUEST = 0x6A8,
		MSG_LW_CANCEL_ARMY_MOVE_REQUEST = 0x6A9,
		MSG_LW_BUILD_BUILDING = 0x6AA,
		MSG_LW_DESTROY_BUILDING = 0x6AB,
		MSG_LW_QUEUE_UNIT = 0x6AC,
		MSG_LW_DEQUEUE_UNIT = 0x6AD,
		MSG_LW_DEQUEUE_UNIT_AT_INDEX = 0x6AE,
		MSG_LW_SWAP_ARMY_MEMBERS = 0x6AF,
		MSG_LW_CANCEL_ARMY_MEMBER_MOVE = 0x6B0,
		MSG_LW_DISBAND_ARMY_MEMBER = 0x6B1,
		MSG_LW_DISBAND_ARMY = 0x6B2,
		MSG_LW_CANCEL_ARMY_MEMBER_DISBAND = 0x6B3,
		MSG_LW_SWAP_ARMIES_IN_BATTLE = 0x6B4,
		MSG_LW_FORCE_ENTER_REALTIME_BATTLE = 0x6B5,
		MSG_LW_DEBUG_QUICKWIN_BATTLE = 0x6B6,
		MSG_LW_RESOLVE_REGION_AWARD_DISPUTE = 0x6B7,
		MSG_LW_PLAYER_SURRENDERED = 0x6B8,
		MSG_LW_REQUEST_ENDGAME = 0x6B9,
		MSG_LW_TRANSITION_TO_TACTICAL_COMPLETE = 0x6BA,
		MSG_LW_TRANSITION_TO_STRATEGIC_COMPLETE = 0x6BB,
		MSG_LW_SCORESCREEN_DONE = 0x6BC,
		MSG_END_NETWORK_MESSAGES = 0x7CF,
		MSG_TIMESTAMP = 0x7D0,
		MSG_OBJECT_CREATED = 0x7D1,
		MSG_OBJECT_DESTROYED = 0x7D2,
		MSG_OBJECT_POSITION = 0x7D3,
		MSG_OBJECT_ORIENTATION = 0x7D4,
		MSG_OBJECT_JOINED_TEAM = 0x7D5,
		MSG_GIVE_UPGRADE_HINT = 0x7D6,
		MSG_JOIN_HORDE_HINT = 0x7D7,
		MSG_CAN_GRAB_HINT = 0x7D8,
		MSG_0x7D9 = 0x7D9,	// no name string; GameClientMessageDispatcher::translateGameMessage 0x00428D66 tests it
		MSG_0x7E9 = 0x7E9,	// no name string; the sacrifice voice response (BloodthirstyUpdate::sacrificeHorde)
		MSG_0x7ED = 0x7ED	// no name string; GameClientMessageDispatcher::translateGameMessage 0x00428D66 tests it
	};
    // Retail getArgument/getArgumentDataType prove head at +0x1C,
    // count at +0x18; constructors prove links/type/player at +4..+0x14.
    // Native 0x0030F47A reads local player +0x10, then index +0x54,
    // and initializes only scalar/link fields. The shared headers expose
    // those exact members; keeping this definition visible also models the
    // non-throwing construction seen by the stream allocation wrappers.
    GameMessage(Type type) {
        m_playerIndex = ThePlayerList->getLocalPlayer()->getPlayerIndex();
        m_type = type;
        m_argList = 0;
        m_argTail = 0;
        m_argCount = 0;
        m_list = 0;
        m_next = 0;
        m_prev = 0;
    }
    GameMessage(const GameMessage &source);
    void copyFrom(const GameMessage &source);
    virtual ~GameMessage();
    // Out of line: retail's callers (Keyboard::createStreamMessages 0x002326FB)
    // call 0x0030F936, which sits between allocArg and appendRealArgument.
    void appendIntegerArgument(int arg);
    GameMessage *next() const { return m_next; }
    GameMessage *prev() const { return m_prev; }
    Type getType() const { return m_type; }
    unsigned char getArgumentCount() const { return m_argCount; } // NetGameCommandMsg 0x004D5CBE reads the byte at +0x18 inline
    int getPlayerIndex() const { return m_playerIndex; } // StatsCollector437607 reads+14
    void friend_setNext(GameMessage *msg) { m_next = msg; }
    void friend_setPrev(GameMessage *msg) { m_prev = msg; }
    void friend_setList(GameMessageList *list) { m_list = list; }
    void friend_setPlayerIndex(int index) { m_playerIndex = index; }
    const GameMessageArgumentType *getArgument(int index) const;
    GameMessageArgumentDataType getArgumentDataType(int index);
    void appendRealArgument(float);
    void appendBooleanArgument(bool);
    void appendObjectIDArgument(ObjectID);
    void appendDrawableIDArgument(DrawableID);
    void appendTeamIDArgument(unsigned int);
    void appendLocationArgument(const Coord3D &);
    void appendPixelArgument(const ICoord2D &);
    void appendPixelRegionArgument(const IRegion2D &);
    void appendTimestampArgument(unsigned int);
    void appendWideCharArgument(const unsigned short &);
    AsciiString getCommandAsAsciiString();
    static AsciiString getCommandTypeAsAsciiString(Type);
private:
    friend class GameMessageList;
    friend class MessageStream;
    friend class CommandList;
    GameMessage *m_next;
    GameMessage *m_prev;
    GameMessageList *m_list;
    Type m_type;
    int m_playerIndex;
    unsigned char m_argCount;
    GameMessageArgument *m_argList;
    GameMessageArgument *m_argTail;
    GameMessageArgument *allocArg();
};

// GameMessageList: a subsystem holding a doubly linked list of GameMessages.
// Target facts:
//  - Table 0x00C09848 (18 slots): SubsystemInterface's 14, then appendMessage
//    0x0030F5D2, insertMessage 0x0030F604, removeMessage 0x0030F642 and
//    containsMessageOfType 0x0030F679. Slots 1, 9 and 10 (init, reset, update;
//    pure in SubsystemInterface) hold the shared empty body 0x000B3FD0, so they
//    are empty here; the other base slots are SubsystemInterface's own.
//  - ctor 0x0030F562 runs SubsystemInterface() and clears +0x0C and +0x10;
//    dtor 0x0030F57C deletes every message still on the list.
//  - removeMessage and containsMessageOfType carry map placeholder names; their
//    bodies (unlink and update both ends; walk comparing GameMessage::m_type)
//    and their slots give the names.
class GameMessageList : public SubsystemInterface
{
public:
	GameMessageList();											// 0x0030F562
	virtual ~GameMessageList();									// 0x0030F57C; deleting 0x00310F85

	virtual void init() {}										// slot 1
	virtual void reset() {}										// slot 9
	virtual void update() {}									// slot 10

	virtual void appendMessage(GameMessage *msg);				// slot 14, 0x0030F5D2
	virtual void insertMessage(GameMessage *msg, GameMessage *position);	// slot 15, 0x0030F604
	virtual void removeMessage(GameMessage *msg);				// slot 16, 0x0030F642
	virtual Bool containsMessageOfType(GameMessage::Type type);	// slot 17, 0x0030F679

	GameMessage *getFirstMessage() { return m_firstMessage; }

protected:
	GameMessage *m_firstMessage;								// +0x0C
	GameMessage *m_lastMessage;									// +0x10
};
// SIZE 0x14

class GameMessageTranslator;

// MessageStream: the GameMessageList that runs its messages through a chain of
// translators. Target facts: table 0x00C09890 (20 slots) adds appendMessage(Type)
// 0x0030F6D4 and insertMessage(Type, GameMessage *) 0x0030F704 as slots 18/19;
// ctor 0x0030F697 clears +0x14/+0x18 and sets +0x1C to 1; attachTranslator
// 0x0030F738, findTranslator 0x0030F7D7 and removeTranslator 0x0030F7F3 walk
// the 0x14-byte TranslatorData nodes; dtor 0x00310FA1 deletes them.
class MessageStream : public GameMessageList
{
public:
	MessageStream();											// 0x0030F697
	virtual ~MessageStream();									// 0x00310FA1; deleting 0x003113F9

	virtual GameMessage *appendMessage(GameMessage::Type type);	// slot 18, 0x0030F6D4
	virtual GameMessage *insertMessage(GameMessage::Type type, GameMessage *position);	// slot 19, 0x0030F704

	UnsignedInt attachTranslator(GameMessageTranslator *translator, UnsignedInt priority);	// 0x0030F738
	GameMessageTranslator *findTranslator(UnsignedInt id);		// 0x0030F7D7
	void removeTranslator(UnsignedInt id);						// 0x0030F7F3
	void propagateMessages();									// 0x00310FF5

	// One translator in the chain, ordered by priority.
	struct TranslatorData
	{
		TranslatorData() : m_next(0), m_prev(0), m_id(0), m_translator(0), m_priority(0) {}
		~TranslatorData();
		TranslatorData *m_next;									// +0x00
		TranslatorData *m_prev;									// +0x04
		UnsignedInt m_id;										// +0x08
		GameMessageTranslator *m_translator;					// +0x0C
		UnsignedInt m_priority;									// +0x10
	};

	TranslatorData *m_firstTranslator;							// +0x14
	TranslatorData *m_lastTranslator;							// +0x18
	UnsignedInt m_nextTranslatorID;								// +0x1C
};
// SIZE 0x20

// CommandList: the GameMessageList the logic executes. Target facts: table
// 0x00C098E0 (18 slots) differs from GameMessageList's only in slot 9, reset
// 0x00311092; ctor 0x0030F842, dtor 0x0031105A (deleting 0x00311415);
// destroyAllMessages 0x0030F854 and appendMessageList 0x0030F880 are its own.
class CommandList : public GameMessageList
{
public:
	CommandList();												// 0x0030F842
	virtual ~CommandList();										// 0x0031105A; deleting 0x00311415

	virtual void reset();										// slot 9, 0x00311092

	void destroyAllMessages();									// 0x0030F854
	void appendMessageList(GameMessage *list);					// 0x0030F880
};
// SIZE 0x14

extern MessageStream *TheMessageStream;
extern CommandList *TheCommandList;

#endif





