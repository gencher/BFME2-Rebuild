// Target: ctor35B8AA / allocation31AB2B establish size0x2CC and Overridable
// at0. Sole vslot is deleting destructor35BCBB in table816204. Parser table
// 815468 establishes named member offsets. header_synth: seven views,84 offsets;
// disagreements resolved against ctor and parser, not majority names.
// ZH ControlBar.h supplies accessor names; BFME2-only names use parser tokens.
// Remaining byte arrays have proven widths but unresolved owning value types.
#ifndef BFME2_COMMANDBUTTON_H
#define BFME2_COMMANDBUTTON_H

#include "Common/Overridable.h"
#include "Common/AsciiString.h"
#include "Common/ModelConditionFlags.h"
#include "Common/Vector.h"
#include <stddef.h>

class INI;
struct FieldParse;
class Object;
class Player;
class Coord3D;
class Drawable;
class Image;
class ThingTemplate;
class UpgradeTemplate;
class SpecialPowerTemplate;
enum ObjectID;
enum Relationship;
enum CommandSourceType;
enum GUICommandType;
enum RadiusCursorType;
enum WeaponSlotType;
enum CommandButtonMappedBorderType;
enum ScienceType;

class CommandButton : public Overridable
{
public:
    CommandButton();
    virtual ~CommandButton();

    static void parseCommand(INI *, void *, void *, const void *);
    // Parse table 0x00815468: 54 entries, every offset below is one of its targets.
    static const FieldParse m_commandButtonFieldParseTable[];
    const FieldParse *getFieldParse() const { return m_commandButtonFieldParseTable; }
    bool isValidRelationshipTarget(Relationship relationship) const;	// the target-relationship option bits (Zero Hour's name)
    bool isValidObjectTarget(const Player *sourcePlayer, const Object *targetObj) const;
    int getNextToggleSlot(int weaponSlot) const;	// the weapon slot after weaponSlot in the toggle cycle
    const Image *getButtonImageAt(int index) const;
    const AsciiString *getTextLabelAt(int index) const;
    const AsciiString *getDescriptionLabelAt(int index) const;
    void selectVariant(int index);	// sets fieldFC when index addresses an image
    bool isValidObjectTarget(const Object *, const Object *) const;
    bool isValidObjectTarget(const Drawable *, const Drawable *) const;
    bool isReady(const Object *) const;
    bool rva0035B2C3(const Object *source, const Object *target, const Coord3D *position, CommandSourceType cmdSource) const;	// 0x0035B2C3 (ScriptActions::doTeamUseCommandButtonOnNamed asks it before groupDoCommandButtonAtObject)
    float getSpecialPowerPercentReady(const Object *) const;
    bool isContextCommand() const;
    const Image *rva0035B19E() const;
    const AsciiString &rva0035B1E9() const;
    const AsciiString &rva0035B26F() const;
    const ThingTemplate *rva0035B570() const;
    void rva0035B5C2(Object *, bool);
    int getStance(int);
    void rva0035B6FA(ObjectID, bool);
    void cacheButtonImage();
    void preloadImages() const;	// 0x0035B3E7 (role name): for a button with +0x101 or +0x102 set, or command 25, calls Image 0x002D937E on every image in m_images; ControlBar 0x0031D61E calls it for each button of a command set
    void setButtonImage(const Image *image);	// 0x0035B750 (Zero Hour's name; ControlBar::updateSlotExitImage 0x0031BE58 calls it)

    const AsciiString &getName() const { return m_name; }
    GUICommandType getCommandType() const { return (GUICommandType)m_command; }
    unsigned int getOptions() const { return m_options; }
    const UpgradeTemplate *getUpgradeTemplate() const { return m_upgradeTemplate; }
    const SpecialPowerTemplate *getSpecialPowerTemplate() const { return m_specialPower; }
    RadiusCursorType getRadiusCursorType() const { return (RadiusCursorType)m_radiusCursor; }
    WeaponSlotType getWeaponSlot() const { return (WeaponSlotType)m_weaponSlot; }
    int getMaxShotsToFire() const { return m_maxShotsToFire; }
    CommandButtonMappedBorderType getCommandButtonMappedBorderType() const { return (CommandButtonMappedBorderType)m_commandButtonBorder; }
    const AsciiString &getCursorName() const { return m_cursorName; }
    const AsciiString &getInvalidCursorName() const { return m_invalidCursorName; }
    const AsciiString &getPurchasedLabel() const { return m_purchasedLabel; }
    const AsciiString &getConflictingLabel() const { return m_conflictingLabel; }
    const CommandButton *getNext() const { return m_next; }
    CommandButton *friend_getNext() { return m_next; }
    void friend_addToList(CommandButton **list) { m_next = *list; *list = this; }
    int getFlashCount() const { return m_flashCount; }
    void setFlashCount(int value) const { m_flashCount = value; }
    const _STL::vector<ScienceType> &getScienceVec() const { return m_sciences; }

    AsciiString m_name;                                      // +0x10
    int m_command;                                           // +0x14
    CommandButton *m_next;                                    // +0x18
    unsigned int m_options;                                   // +0x1C
    const ThingTemplate *field20;                             // +0x20
    const UpgradeTemplate *m_upgradeTemplate;                 // +0x24
    _STL::vector<const UpgradeTemplate *> m_neededUpgrades; // +0x28
    bool m_neededUpgradeAny;                                  // +0x34
    unsigned char pad35[3];
    _STL::vector<AsciiString> m_buildUpgrades; // +0x38
    const SpecialPowerTemplate *m_specialPower;               // +0x44
    AsciiString m_toggleButtonName;                          // +0x48
    int m_radiusCursor;                                      // +0x4C
    AsciiString m_cursorName;                                // +0x50
    AsciiString m_invalidCursorName;                         // +0x54
    _STL::vector<AsciiString> m_textLabel; // +0x58
    _STL::vector<AsciiString> m_description; // +0x64
    AsciiString m_purchasedLabel;                            // +0x70
    AsciiString m_conflictingLabel;                          // +0x74
    AsciiString m_lacksPrerequisiteLabel;                    // +0x78
    AsciiString field7C;                                     // +0x7C
    int m_weaponSlot;                                        // +0x80
    int m_weaponSlotToggle1;                                 // +0x84
    int m_weaponSlotToggle2;                                 // +0x88
    int m_weaponSlotToggle3;                                 // +0x8C
    unsigned char m_flagsUsedForToggle[16];                  // +0x90
    int m_maxShotsToFire;                                    // +0xA0
    _STL::vector<ScienceType> m_sciences; // +0xA4
    int m_commandButtonBorder;                               // +0xB0
    _STL::vector<AsciiString> m_imageNames; // +0xB4
    int fieldC0;                                             // +0xC0
    unsigned int fieldC4;                                    // +0xC4
    unsigned char fieldC8[0x24];                             // +0xC8
    _STL::vector<const Image *> m_images; // +0xEC
    mutable int m_flashCount;                                // +0xF8
    int fieldFC;                                             // +0xFC
    bool m_doubleClick;                                      // +0x100
    bool m_radial;                                           // +0x101
    bool m_inPalantir;                                       // +0x102
    bool field103;                                           // +0x103
    bool m_showProductionCount;                              // +0x104
    bool m_isClickable;                                      // +0x105
    bool m_showButton;                                       // +0x106
    bool m_requiresValidContainer;                           // +0x107
    int m_requireLevel;                                      // +0x108
    bool m_autoAbility;                                      // +0x10C
    unsigned char pad10D[3];
    unsigned char m_affectsKindOf[28];                       // +0x110
    bool m_triggerWhenReady;                                 // +0x12C
    unsigned char pad12D[3];
    float m_presetRange;                                     // +0x130
    float m_autoDelay;                                       // +0x134
    bool m_needDamagedTarget;                                // +0x138
    unsigned char pad139[3];
    ModelConditionFlags m_autoAbilityDisallowedOnModelCondition; // +0x13C
    _STL::vector<AsciiString> m_commandTrigger; // +0x188
    ModelConditionFlags m_enableOnModelCondition;              // +0x194
    ModelConditionFlags m_disableOnModelCondition;             // +0x1E0
    int m_commandRangeStart;                                 // +0x22C
    int m_commandRangeCount;                                 // +0x230
    _STL::vector<int> m_stances; // +0x234
    AsciiString m_createAHeroUIPrerequisiteButtonName;        // +0x240
    int m_createAHeroUIMinimumLevel;                          // +0x244
    AsciiString m_createAHeroUIIconImageName;                 // +0x248
    unsigned char m_createAHeroUIAllowableUpgrades[128];      // +0x24C
};

typedef char CommandButtonOptionOffset[(offsetof(CommandButton,m_options)==0x1C)?1:-1];
typedef char CommandButtonModelFlagsOffset[(offsetof(CommandButton,m_enableOnModelCondition)==0x194)?1:-1];
typedef char CommandButtonTailOffset[(offsetof(CommandButton,m_createAHeroUIAllowableUpgrades)==0x24C)?1:-1];
typedef char CommandButtonSizeCheck[sizeof(CommandButton) == 0x2CC ? 1 : -1];

#endif
