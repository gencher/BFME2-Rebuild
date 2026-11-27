#pragma once

#include "Common/Coord2D.h"
#include "Common/Snapshot.h"
#include "Common/Vector.h"
#include "GameLogic/ArmySummaryEntry.h"
#include "GameLogic/LivingWorldTurnObserver.h"

class BuildingNuggetView;
class LivingWorldBuildingTemplate;
class LivingWorldRegion;
class Rva0052B278;

// 0x004E119C stores the Snapshot table 0x00861618 at +0 and the
// turn-observer table 0x00861610 at +4. Both base tables are restored
// by 0x004E0D67 in reverse construction order.
class LivingWorldBuilding : public Snapshot, public LivingWorldTurnObserver
{
public:
    LivingWorldBuilding(LivingWorldRegion *region, const Coord2D &position,
        const LivingWorldBuildingTemplate *buildingTemplate, Int id);
    virtual ~LivingWorldBuilding();

    virtual void BeginTurn(Int turn, Int phase);
    void createIcon();
    void createGhostObject();
    UnsignedByte get() const;
    virtual const char *GetSnapshotName() const;

protected:
    virtual void loadPostProcess() {} // native 0x000B3FD0
    virtual void xfer(Xfer *xfer); // native 0x004E0E78

private:
    UnsignedByte m_unk0x08[0x10]; // 0x00330757 constructs the list at +8
    Int m_unk0x18; // fourth constructor argument
    Int m_unk0x1C; // signed integer saved at 0x004E0EB3
    Int m_unk0x20; // signed integer saved at 0x004E0EF1 for version >=5
    LivingWorldRegion *m_region; // first constructor argument
    const LivingWorldBuildingTemplate *m_buildingTemplate; // third constructor argument
    Rva0052B278 *m_icon; // createIcon constructs this at 0x004E067E
    _STL::vector<BuildingNuggetView *> m_nuggets; // 0x002E00F9 clones the template nuggets
    AptRef<ArmySummaryEntry> m_armySummaryEntry; // set by 0x004E0BDC and released via +0xAC
    UnsignedByte m_unk0x40[8]; // cached callbacks returned by the nuggets at 0x004E0809
    Coord2D m_position; // constructor copies the second argument, xfer slot 20 transfers it
    Bool m_unk0x50;
    Bool m_unk0x51;
};
