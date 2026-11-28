// Upgrade.h: upgrade definitions and their subsystem. Target facts:
// newUpgrade26F952 allocates9C, calls ctor26F5B3, then sets name8/keyC;
// ctor/dtor store tableBFABF0 with one deleting-destructor slot. No distinct
// retail MemoryPoolObject base is established, so the donor base is not used.
// Constructor, assignment26F767 and destructor26F684 establish string widths,
// links64/68, mask index38 and image name/pointer pairs6C/70 and8C/90.
// UpgradeCenter ctor26F419 builds SubsystemInterface, installs BFABB8 (14
// slots), and initializes headC, next index10, flag14 and a vector header18.
// Unknown vector/record types retain measured gaps. Names are target API names
// and ZH name leads where their accessed roles agree with retail.
#pragma once
#ifndef BFME2_UPGRADE_H
#define BFME2_UPGRADE_H
#include <stddef.h>
#include "Common/AsciiString.h"
#include "Common/SubsystemInterface.h"
#include <vector>
#include "GameLogic/ObjectFilter.h"
class Player;
struct UpgradeMaskType;
class Object;
class Image;
class INI;
enum VeterancyLevel;
enum UpgradeType { UPGRADE_TYPE_PLAYER=0, UPGRADE_TYPE_OBJECT=1 };
enum EvaEventID;
class UpgradeTemplate
{
public:
    UpgradeTemplate();
    virtual ~UpgradeTemplate();
    UpgradeTemplate &operator=(const UpgradeTemplate &);
    unsigned int calcCostToBuild(Player *,unsigned int) const;
    void friend_makeVeterancyUpgrade(VeterancyLevel);
    void friend_resolveSubUpgradeNames();
    void cacheButtonImage();
    const AsciiString &getUpgradeName() const { return m_name; }
    void setUpgradeName(const AsciiString &name) { m_name=name; }
    NameKeyType getUpgradeNameKey() const { return m_nameKey; }
    void setUpgradeNameKey(NameKeyType key) { m_nameKey=key; }
    const AsciiString &getDisplayNameLabel() const { return m_displayNameLabel; }
    UpgradeType getUpgradeType() const { return static_cast<UpgradeType>(m_type); }
    int getMaskIndex() const { return m_maskIndex; }
    void setMaskIndex(int index) { m_maskIndex=index; }
    const Image *getButtonImage() const { return m_buttonImage; }
    UpgradeTemplate *friend_getNext() { return m_next; }
    const UpgradeTemplate *friend_getNext() const { return m_next; }
    UpgradeTemplate *friend_getPrev() { return m_prev; }
    const UpgradeTemplate *friend_getPrev() const { return m_prev; }
    void friend_setNext(UpgradeTemplate *next) { m_next=next; }
    void friend_setPrev(UpgradeTemplate *prev) { m_prev=prev; }

    int m_type; //04
    AsciiString m_name; //08
    NameKeyType m_nameKey; //0C
    // 0x10 SubUpgradeTemplateNames (INI::parseAsciiStringVector, table 0x007FA6E8);
    // friend_resolveSubUpgradeNames 0x0026F8D6 reserves +0x1C to its size, pushes each
    // TheUpgradeCenter->findUpgrade hit, then erases the names.
    std::vector<AsciiString> m_subUpgradeTemplateNames; //10
    std::vector<const UpgradeTemplate *> m_subUpgradeTemplates; //1C
    AsciiString m_displayNameLabel, m_tooltip; //28 DisplayName / 2C Tooltip
    float m_buildTime; //30
    int m_cost, m_maskIndex; //34/38
    AsciiString m_upgradeFX; //3C UpgradeFX
    // 0x40..0x63 from the field table: the audio entries are stored by INI::parseAudioEventRTS
    // and the Eva entries by the Eva-event parser 0x001DEE74; neither store type is resolved
    // yet (OpenContainModuleData and ThingTemplate keep the same four bytes).
    unsigned char m_researchSound[4]; //40 ResearchSound
    unsigned char m_researchCompleteEvaEvent[4]; //44 ResearchCompleteEvaEvent
    unsigned char m_unitSpecificSound[4]; //48 UnitSpecificSound
    // 0x4C..0x60: Eva event ids (Player::onUpgradeCompleted 0x002AD93A and onUpgradeRemoved 0x002AD9FD pass them to Eva::reportEvaEvent).
    EvaEventID m_localPlayerGainsUpgradeEvaEvent; //4C
    EvaEventID m_alliedPlayerGainsUpgradeEvaEvent; //50
    EvaEventID m_enemyPlayerGainsUpgradeEvaEvent; //54
    EvaEventID m_localPlayerLosesUpgradeEvaEvent; //58
    EvaEventID m_alliedPlayerLosesUpgradeEvaEvent; //5C
    EvaEventID m_enemyPlayerLosesUpgradeEvaEvent; //60
    UpgradeTemplate *m_next, *m_prev; //64/68
    AsciiString m_buttonImageName; //6C
    const Image *m_buttonImage; //70
    AsciiString m_cursor; //74 Cursor
    bool m_persistsInCampaign, m_scaleCostWithBuildCost; //78 PersistsInCampaign / 79 NoUpgradeDiscount
    AsciiString m_buildCostTemplateName; //7C UseObjectTemplateForCostDiscount
    ObjectFilter m_requiredObjectFilter; //80 RequiredObjectFilter (iniParseObjectFilter)
    AsciiString m_groupName; //84 GroupName
    int m_groupOrder; //88 GroupOrder
    AsciiString m_highlightImageName; //8C StrategicIcon
    const Image *m_highlightImage; //90
    bool m_isOverride; //94, parseUpgradeDefinition sets it before replacing a definition
    int m_unk0x98; //98
};
// SIZE 0x9C
typedef char UpgradeTemplateSize[sizeof(UpgradeTemplate)==0x9C?1:-1];
typedef char UpgradeTemplateNextOffset[offsetof(UpgradeTemplate,m_next)==0x64?1:-1];
typedef char UpgradeTemplateSubOffset[offsetof(UpgradeTemplate,m_subUpgradeTemplates)==0x1C?1:-1];
typedef char UpgradeTemplateFilterOffset[offsetof(UpgradeTemplate,m_requiredObjectFilter)==0x80?1:-1];
typedef char UpgradeTemplatePrevOffset[offsetof(UpgradeTemplate,m_prev)==0x68?1:-1];

class UpgradeCenter : public SubsystemInterface
{
public:
    UpgradeCenter();
    virtual ~UpgradeCenter();
    virtual void init();
    virtual bool attemptReload(bool *needsRestart);
    virtual bool attemptRefresh();
    virtual void reset();
    virtual void update() {}
    const UpgradeTemplate *findUpgradeByKey(NameKeyType) const;
    const UpgradeTemplate *findUpgrade(const AsciiString &) const;
    const UpgradeTemplate *findUpgradeByMaskIndex(int maskIndex) const; // 0x0026EEA0: the template whose m_maskIndex (+0x38) is maskIndex
    const UpgradeTemplate *findVeterancyUpgrade(VeterancyLevel) const;
    UpgradeTemplate *newUpgrade(const AsciiString &,bool);
    void linkUpgrade(UpgradeTemplate *);
    // 0x0026F11A: ignores this (a shared body: the money test and, when asked, the message); the third argument is 0 at its caller.
    bool canAffordUpgrade(Player *player, const UpgradeTemplate *upgrade, unsigned int unused, bool displayReason);
    const UpgradeTemplate *findFirstUpgradeInMask(const UpgradeMaskType *mask) const; // 0x0026F0F0 (first template whose m_maskIndex bit is set in the mask)
    void rva0026F93C(); // native post-load sub-upgrade resolution walk; reset26FBBC
    static void parseUpgradeDefinition(INI *);
    UpgradeTemplate *m_upgradeList; //0C
    int m_nextTemplateMaskBit; //10
    bool m_buttonImagesCached; //14
    std::vector<UpgradeTemplate *> m_retiredTemplates; // +0x18; vector of retired templates, allocator identity unresolved
protected:
    void unlinkUpgrade(UpgradeTemplate *);
};
// SIZE 0x24
typedef char UpgradeCenterSize[sizeof(UpgradeCenter)==0x24?1:-1];
extern UpgradeCenter *TheUpgradeCenter;
#endif
