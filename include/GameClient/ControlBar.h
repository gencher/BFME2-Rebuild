#pragma once
#include <stddef.h>
#include "Common/SubsystemInterface.h"
#include "Common/ICoord2D.h"
#include "Common/STLTypedefs.h"
#include <hash_map>
#include <vector>

// Portions derived from Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
// The command option bit names (retail VA 0x00DBFB68, 32 names and NULL; one copy, in
// CommandButton's object, before TheGuiCommandNames). Zero Hour's name and guard.
#ifdef DEFINE_COMMAND_OPTION_NAMES
static const char *TheCommandOptionNames[] =
{
	"NEED_TARGET_ENEMY_OBJECT",
	"NEED_TARGET_NEUTRAL_OBJECT",
	"NEED_TARGET_ALLY_OBJECT",
	"NO_PLAY_UNIT_SPECIFIC_SOUND_FOR_AUTO_ABILITY",
	"ALLOW_SHRUBBERY_TARGET",
	"NEED_TARGET_POS",
	"NEED_UPGRADE",
	"NEED_SPECIAL_POWER_SCIENCE",
	"OK_FOR_MULTI_SELECT",
	"CONTEXTMODE_COMMAND",
	"CHECK_LIKE",
	"NEEDS_CASTLE_KINDOF",
	"ATTACK_OBJECTS_POSITION",
	"OPTION_ONE",
	"OPTION_TWO",
	"OPTION_THREE",
	"NOT_QUEUEABLE",
	"SINGLE_USE_COMMAND",
	"---DO-NOT-USE---",
	"SCRIPT_ONLY",
	"OK_FOR_MULTI_EXECUTE",
	"ALLOW_ROCK_TARGET",
	"HIDE_WHILE_DISABLED",
	"TOGGLE_IMAGE_ON_WEAPON",
	"TOGGLE_IMAGE_ON_WEAPONSET",
	"TOGGLE_IMAGE_ON_FORMATION",
	"MOUNTED_ONLY",
	"UNMOUNTED_ONLY",
	"NONPRESSABLE",
	"AUTO_ABILITY_TRIGGERED",
	"ON_GROUND_ONLY",
	"CANCELABLE",
	NULL
};
#endif

// The INI names of the button commands (retail VA 0x00DBFBF0, 61 names and NULL; one copy,
// in CommandButton's object). Zero Hour's name and guard.
#ifdef DEFINE_GUI_COMMMAND_NAMES
static const char *TheGuiCommandNames[] =
{
	"NONE", "FOUNDATION_CONSTRUCT", "FOUNDATION_CONSTRUCT_CANCEL", "UNIT_BUILD",
	"SECONDARY_UNIT_BUILD", "CANCEL_UNIT_BUILD", "PLAYER_UPGRADE", "OBJECT_UPGRADE",
	"CASTLE_UPGRADE", "CANCEL_UPGRADE", "ATTACK_MOVE", "GUARD", "GUARD_WITHOUT_PURSUIT",
	"GUARD_FLYING_UNITS_ONLY", "STOP", "WAYPOINTS", "EXIT_CONTAINER", "EVACUATE",
	"EVACUATE_CONTESTED", "EXECUTE_UNUSED_01", "BEACON_DELETE", "SET_RALLY_POINT", "SELL",
	"FIRE_WEAPON", "SPECIAL_POWER", "PURCHASE_SCIENCE", "COMBATDROP", "SWITCH_WEAPON",
	"AUTOCAST_WEAPON", "HIJACK_VEHICLE", "CONVERT_TO_CARBOMB", "PLACE_BEACON",
	"SPECIAL_POWER_FROM_COMMAND_CENTER", "CASTLE_UNPACK", "CASTLE_PACK", "TOGGLE_WEAPON",
	"HORDE_TOGGLE_FORMATION", "SPECIAL_POWER_TOGGLE", "SPELL_BOOK", "ONE_RING",
	"CREW_EVACUATE", "BLOODTHIRSTY", "CLOSE_GATE", "OPEN_GATE", "TOGGLE_GATE",
	"TOGGLE_WEAPONSET", "REVIVE", "MONSTERDOCK", "TOGGLE_NO_AUTO_ACQUIRE", "WAKE_AUTO_PICKUP",
	"CASTLE_UNPACK_EXPLICIT_OBJECT", "START_SELF_REPAIR", "HORDE_SET_FORMATION",
	"DOZER_CONSTRUCT", "DOZER_CONSTRUCT_CANCEL", "PUSH_VISIBLE_COMMAND_RANGE",
	"POP_VISIBLE_COMMAND_RANGE", "TOGGLE_STANCE", "SET_STANCE", "START_NEIGHBORHOOD_REPAIR",
	"CANCEL_NEIGHBORHOOD",
	NULL
};
#endif
// Target allocation29E5B9 and ctor31E94C: size2B4, SubsystemInterface base0..B.
// Primary table80CC88 has14 slots. Scheme consumer1DB68C proves+44.
// Callers9DE6C/35D87A establish border+22C and arrow+26C.
// Neutral storage retains unresolved record/container identities.
class AnimateWindowManager;
class CommandButton;
class CommandSet;
class ControlBarSchemeManager;
class Drawable;
class GameWindow;
class Image;
class INI;
class Object;
class Player;
class PlayerTemplate;

class WindowLayout;
class WindowVideoManager;

class Coord3D;
// Values 0 and 1 are what processCommandTransitionUI 0x00539997 returns; names are Zero Hour's.
enum CBCommandStatus { CBC_COMMAND_NOT_USED = 0, CBC_COMMAND_USED = 1 };
enum CommandAvailability;
enum CommandButtonMappedBorderType;
// switchToContext 0x0031BF64 contexts (its first argument; the switch's case labels). Names are Zero Hour's where it has the context;
// 3 (the contested structure inventory) and 4 (a selected object whose contents the local player may not command) are role names.
enum ControlBarContext { CB_CONTEXT_NONE = 0, CB_CONTEXT_COMMAND = 1, CB_CONTEXT_STRUCTURE_INVENTORY = 2, CB_CONTEXT_CONTESTED_STRUCTURE_INVENTORY = 3, CB_CONTEXT_NO_COMMANDS = 4, CB_CONTEXT_BEACON = 5, CB_CONTEXT_UNDER_CONSTRUCTION = 6, CB_CONTEXT_MULTI_SELECT = 7, CB_CONTEXT_OBSERVER_INFO = 8, CB_CONTEXT_OBSERVER_LIST = 9, CB_CONTEXT_OCL_TIMER = 10 };
// switchControlBarStage 0x0031B99A: 0 -> default config, 2 -> low config, 3 -> hidden, 1 does nothing; anything outside 0..3 is ignored.
// Names are Zero Hour's.
enum ControlBarStages { CONTROL_BAR_STAGE_DEFAULT = 0, CONTROL_BAR_STAGE_SQUISHED = 1, CONTROL_BAR_STAGE_LOW = 2, CONTROL_BAR_STAGE_HIDDEN = 3, CONTROL_BAR_STAGE_MAX = 4 };
enum GadgetGameMessage;

// One inventory button of the structure panel and the object it stands for (retail's 32-entry table at VA 0x00E05E20).
struct ControlBarInventoryEntry {
    GameWindow *control;
    ObjectID objectID;
};
extern ControlBarInventoryEntry ControlBarInventoryEntries[32];

// populateStructureInventory's argument to iterateContained's callback (populateButtonProc).
// doTransportInventoryUI 0x0053BB16's argument to iterateContained's callback (populateInvDataCallback 0x0053B98C); names are Zero Hour's.
struct PopulateInvButtonData {
    int currIndex;
    int maxIndex;
    GameWindow **controls;
    Object *transport;
};
class ControlBar;
// The command sets by name: find 0x0041534B, operator[] 0x0031DB83 and erase 0x003A37DC are its members.
typedef _STL::hash_map<AsciiString, CommandSet *, rts::hash<AsciiString>, _STL::equal_to<AsciiString> > CommandSetMap;
struct PopulateButtonInfo {
    Object *source;
    int buttonIndex;
    ControlBar *self;
    GameWindow **inventoryButtons;
};

class ControlBar : public SubsystemInterface {
public:
    ControlBar();
    virtual ~ControlBar();
    virtual void init();
    virtual bool loadIniFilesFromLegend();	// slot 2, override 0x0031B53D: CommandButton and CommandSet INI files (the reload flag selects INI_LOAD_RELOAD)
    virtual Bool attemptReload(Bool *needsRestart); // SubsystemInterface slot 4; override 0x0031E7F0
    virtual void reset();
    virtual void update();

    const CommandButton *findCommandButton(const AsciiString &name);
    const CommandSet *findCommandSet(const AsciiString &name);
    void hideSpecialPowerShortcut();
    void showSpecialPowerShortcut();
    void hidePurchaseScience();
    void showPurchaseScience();
    void postProcessCommands();
    void updateRightHUDImage(const Image *image);
    void updateConstructionTextDisplay(Object *object);
    void switchControlBarStage(ControlBarStages stage);
    void switchToContext(int context, void *object);
    void setControlBarScheme(const AsciiString &name);
    void setControlBarSchemeByPlayer(Player *player);
    void setControlBarSchemeByPlayerTemplate(const PlayerTemplate *playerTemplate);
    void setCommandBarBorder(GameWindow *window, CommandButtonMappedBorderType type);
    void setControlCommand(GameWindow *window, const CommandButton *command);
    void setControlCommand(const AsciiString &name, GameWindow *parent, const CommandButton *command);
    void setPortraitByImage(const Image *image);
    void setPortraitByObject(Object *object);
    void showRallyPoint(const Coord3D *position);
    void GetPurchaseScienceStatus(Player *, int, const CommandButton **, bool *, bool *, bool *);
    void forgetStaleCommandSet(const AsciiString &name);
    void initObserverControls();
    void populateCommand(Object *, bool);
    void populateBeacon(Object *);
    void populateOCLTimer(Object *);
    void populateObserverInfoWindow();
    void populateObserverList();
    void doTransportInventoryUI(Object *, CommandSet *);
    void updateContextContestedStructureInventory();
    CommandAvailability getCommandAvailability(const CommandButton *, GameWindow *, Object *, float *, bool) const;
    CBCommandStatus processCommandUI(const CommandButton *, bool, bool);
    CBCommandStatus processContextSensitiveButtonClick(GameWindow *, GadgetGameMessage);
    CBCommandStatus processContextSensitiveButtonTransition(GameWindow *, GadgetGameMessage);
    CBCommandStatus rva0053B1CF(GameWindow *, unsigned int);
    static void parseCommandButtonDefinition(INI *);
    static void parseCommandSetDefinition(INI *);

    void onDrawableSelected(Drawable *);
    void onDrawableDeselected(Drawable *);
    void updateRadarAttackGlow();	// 0x0031AEFF
    int toggleControlBarVisibility();	// 0x0031AFDE (role name): flips field29C between 0 and 1 for ShowControlBar and ToggleControlBar, then drops the selection
    unsigned char loadCommandIniFiles(INI *ini, int loadType);	// 0x0031B4D6 (role name): the three INI files in Data\INI
    void updateSlotExitImage(const Image *image);	// 0x0031BE58 (Zero Hour's name: sets the exit-slot image on Command_StructureExit and Command_TransportExit)
    const CommandSet *getPurchaseScienceCommandSet(Player *player);	// 0x0031DF89 (role name): the player template's PurchaseScienceCommandSet, or the MP one in a multiplayer game
    void animateSpecialPowerShortcut(bool isOn);	// 0x0031AF54 (Zero Hour's name)
    int updateGenStarFlash();	// 0x0031AA86 (role name; always returns 0): flashes the General button's star while purchase points are unspent; ControlBar::update calls it first
    void rva0031AB77SetFlag();
    void rva0031AD14(int, int, int, int, int);
    void rva0031AD48(bool);
    void togglePurchaseScience();
    void rva0031ADAE();
    void rva0031AE13();
    void rva0031AE93(int *, int *);
    void rva0031AEAE(int *, int *);
    void rva0031AEC9();
    CommandSet *newCommandSetOverride(CommandSet *setToOverride);
    void rva0031B1BC(const Image *, const Image *, const Image *, const Image *, const Image *, const Image *, const Image *, const Image *);
    void rva0031B210();
    void onPlayerRankChanged(const Player *);	// 0x0031B5A3 (Zero Hour's name)
    void rva0031BAB1();
    void rva0031BAC3(Player *);
    void rva0031D230();
    void rva0031D61E(const AsciiString *);
    void rva0031D64F();
    bool rva0031E068(Player *, int);
    CommandSet *newCommandSet(const AsciiString &name, int mode);
    void rva00405D77();
    CommandSet *rva00406D95(const AsciiString &);
    void rva004C1B60(GameWindow *, void *);
    int selectMatchingOnDoubleClick(Object *);	// 0x005399D0: a double click on one of the local player's selectable non-structure objects selects all of its type on screen (role name; CommandTranslator::rva0042AF97 0x0042AF97 calls it)
    int rva0053BA87(const CommandButton *, Object *) const;
    void rva0053E4B1();
    void rva0053E6E1();
    Object *rva0053E754(GameWindow *);

    void markUIDirty() { m_uiDirty = true; }
    void setObserverLookAtPlayer(Player *player) { m_observerLookAtPlayer = player; }
    Player *getObserverLookAtPlayer() { return m_observerLookAtPlayer; }
    ControlBarSchemeManager *getControlBarSchemeManager() { return m_schemeManager; }
    int getBorderColor() { return field22C; }
    const Image *getArrowImage() { return field26C; }

    // Member evidence, in order of strength: the constructor 0x0031E94C (stores, the
    // rep stosd counts 9 at +0x48 and 0x20 at +0xDC, the five-entry loop at +0xA8/+0xBC,
    // 0x64000000 at +0x208/+0x22C); the allocation (0x2B4). Names are Zero Hour's
    // ControlBar.h spellings where it has the member (donor facts); the other members
    // stay neutral. Offsets are checked at the end of this file.
    WindowVideoManager *m_videoManager;                 // +0x0C
    AnimateWindowManager *m_animateWindowManager;       // +0x10
    AnimateWindowManager *m_animateWindowManagerForGenShortcuts; // +0x14
    AnimateWindowManager *m_generalsScreenAnimate;      // +0x18
    ICoord2D m_defaultControlBarPosition;               // +0x1C
    int m_currentControlBarStage;                       // +0x24 (ControlBarStages)
    bool m_uiDirty;                                     // +0x28 (UIDirty)
    unsigned char field29[0x3];
    CommandButton *m_commandButtons;                    // +0x2C list head (links at CommandButton +0x18)
    CommandSetMap m_commandSets;                        // +0x30 (findCommandSet 0x0031D5F8, newCommandSet 0x0031E8D5)
    ControlBarSchemeManager *m_schemeManager;           // +0x44
    GameWindow *m_contextParent[9];                     // +0x48
    Drawable *m_currentSelectedDrawable;                // +0x6C
    int m_currContext;                                  // +0x70
    int m_rallyPointDrawableID;                         // +0x74
    float m_displayedConstructPercent;                  // +0x78
    unsigned int m_displayedOCLTimerSeconds;            // +0x7C
    int m_lastRecordedInventoryCount;                   // +0x80
    GameWindow *m_rightHUDWindow;                       // +0x84
    GameWindow *m_rightHUDCameoWindow;                  // +0x88
    GameWindow *m_rightHUDUpgradeCameos[5];             // +0x8C
    GameWindow *m_rightHUDUnitSelectParent;             // +0xA0
    GameWindow *m_communicatorButton;                   // +0xA4
    GameWindow *m_specialPowerShortcutButtons[5];       // +0xA8
    GameWindow *m_specialPowerShortcutButtonParents[5]; // +0xBC
    int m_currentlyUsedSpecialPowersButtons;            // +0xD0
    WindowLayout *m_specialPowerLayout;                 // +0xD4
    GameWindow *m_specialPowerShortcutParent;           // +0xD8
    GameWindow *m_commandWindows[32];                   // +0xDC
    const CommandButton *m_commonCommands[32];         // +0x15C the multi-select common command set (resetCommonCommandData 0x0053DB02 clears it; Zero Hour's name); the constructor clears only its first 9 words
    bool m_sideSelectAnimateDown;                       // +0x1DC
    unsigned char field1DD[0x3];
    int m_unk0x1E0[8];                                  // +0x1E0..+0x1FC, cleared by the constructor
    GameWindow *m_animateDownWindow;                    // +0x200
    int m_unk0x204;                                     // +0x204
    unsigned int m_buildUpClockColor;                   // +0x208 (0x64000000 after construction)
    bool m_isObserverCommandBar;                        // +0x20C
    unsigned char field20D[0x3];
    Player *m_observerLookAtPlayer;                                   // +0x210 observer look-at player (getObserverLookAtPlayer)
    bool m_showBuildToolTipLayout;                      // +0x214
    unsigned char field215[0x3];
    int m_commandButtonBorderBuildColor;                // +0x218 (set by 0x0031AD14)
    int m_commandButtonBorderActionColor;               // +0x21C
    int m_commandButtonBorderUpgradeColor;              // +0x220
    int m_commandButtonBorderSystemColor;               // +0x224
    int m_commandButtonBorderAlteredColor;              // +0x228 (a fifth colour)
    int field22C;                                       // +0x22C command bar border colour (getBorderColor; 0x64000000 after construction)
    _STL::vector<CommandSet *> m_retiredCommandSets;      // +0x230 (forgetStaleCommandSet pushes the replaced set)
    _STL::vector<CommandButton *> m_retiredCommandButtons; // +0x23C (removeCommandButton pushes the replaced button)
    const Image *m_genStarOn;                           // +0x248
    const Image *m_genStarOff;                          // +0x24C
    const Image *m_unk0x250[6];                         // +0x250..+0x264, cleared by the constructor
    unsigned char field268[0x4];
    const Image *field26C;                              // +0x26C arrow image (getArrowImage)
    const Image *m_unk0x270[2];                         // +0x270
    bool m_genStarFlash;                                // +0x278
    unsigned char field279[0x3];
    int m_lastFlashedAtPointValue;                      // +0x27C
    ICoord2D m_controlBarForegroundMarkerPos;           // +0x280
    ICoord2D m_controlBarBackgroundMarkerPos;           // +0x288
    bool m_radarAttackGlowOn;                           // +0x290 (updateRadarAttackGlow 0x0031AEFF, Zero Hour's name)
    unsigned char field291[0x3];
    int m_remainingRadarAttackGlowFrames;               // +0x294 (set to 150 by the trigger 0x0031AEC9; Zero Hour's name)
    GameWindow *m_radarAttackGlowWindow;                // +0x298
    int field29C;                                       // +0x29C
    void *m_unknown2A0; // +0x2A0 pointee identity unresolved (the constructor news 0x5C bytes via 0x0053ED1A)
    void *m_unknown2A4; // +0x2A4 owning holder, pointee unresolved
    unsigned char m_unk0x2A8[0xC];                      // +0x2A8 vector (allocator-tagged constructor 0x00211E58)

protected:
    // The command-button registry is a singly linked list headed at +0x2C (links at
    // CommandButton +0x18). The four operations below are 0x0031BE07 (name search,
    // returns the final override), 0x0031AB1E (allocate a 0x2CC node, set its name,
    // push it on the list), 0x0031EB96 (copy into an override node and chain it) and
    // 0x0031BF1B (unlink and queue the node in the vector at +0x23C). Names follow
    // Zero Hour's ControlBar except removeCommandButton, which is role-inferred.
    CommandButton *findNonConstCommandButton(const AsciiString &name);
    CommandSet *findNonConstCommandSet(const AsciiString &name);	// Zero Hour's name; retail folds its body into findCommandSet 0x0031D5F8
    CommandButton *newCommandButton(const AsciiString &name);
    CommandButton *newCommandButtonOverride(CommandButton *buttonToOverride);
    void removeCommandButton(CommandButton *button);
    void setHiddenControlBar();
    void setLowControlBarConfig();
    void setDefaultControlBarConfig();
    void addCommonCommands(Drawable *, bool);
    void resetCommonCommandData();	// 0x0053DB02 (Zero Hour's name)
    void populateMultiSelect();
    void populateSpecialPowerShortcut(Player *);
    void populateStructureInventory(Object *, bool);
    void populateUnderConstruction(Object *);
    void updateContextCommand();
    void updateContextMultiSelect();
    void updateContextOCLTimer();
    void updateContextStructureInventory();
    void updateOCLTimerTextDisplay(unsigned int, float);
    CBCommandStatus processCommandTransitionUI(GameWindow *, GadgetGameMessage);
    static void populateButtonProc(Object *, void *);
    static void populateInvDataCallback(Object *, void *);
    static const Image *calculateVeterancyOverlayForObject(const Object *);
};

extern ControlBar *TheControlBar;

// 0x00402A8B (map: ?ShowControlBar@@YAX_N@Z): PlayerList::refreshObservedPlayerView 0x002A7E42 calls it with false.
void ShowControlBar(Bool show);
// 0x00402960: the matching hide, called with true by ScriptActions::doLetterBoxMode 0x003BBE04.
void HideControlBar(Bool hide);


typedef char ControlBarSize[sizeof(ControlBar)==0x2B4 ? 1 : -1];
typedef char ControlBarSchemeOffset[offsetof(ControlBar,m_schemeManager)==0x44 ? 1 : -1];
typedef char ControlBarOffsetContextParent[offsetof(ControlBar,m_contextParent)==0x48 ? 1 : -1];
typedef char ControlBarOffsetSelected[offsetof(ControlBar,m_currentSelectedDrawable)==0x6C ? 1 : -1];
typedef char ControlBarOffsetRightHUD[offsetof(ControlBar,m_rightHUDWindow)==0x84 ? 1 : -1];
typedef char ControlBarOffsetShortcuts[offsetof(ControlBar,m_specialPowerShortcutButtons)==0xA8 ? 1 : -1];
typedef char ControlBarOffsetCommandWindows[offsetof(ControlBar,m_commandWindows)==0xDC ? 1 : -1];
typedef char ControlBarOffsetBuildUpClock[offsetof(ControlBar,m_buildUpClockColor)==0x208 ? 1 : -1];
typedef char ControlBarOffsetBorderColors[offsetof(ControlBar,m_commandButtonBorderBuildColor)==0x218 ? 1 : -1];
typedef char ControlBarOffsetBorder[offsetof(ControlBar,field22C)==0x22C ? 1 : -1];
typedef char ControlBarOffsetGenStar[offsetof(ControlBar,m_genStarOn)==0x248 ? 1 : -1];
typedef char ControlBarOffsetArrow[offsetof(ControlBar,field26C)==0x26C ? 1 : -1];
typedef char ControlBarOffsetMarkers[offsetof(ControlBar,m_controlBarForegroundMarkerPos)==0x280 ? 1 : -1];
typedef char ControlBarOffsetTail[offsetof(ControlBar,m_unknown2A0)==0x2A0 ? 1 : -1];
