#pragma once
#include "GameLogic/Module/UpdateModule.h"
#include "GameLogic/Module/DamageModule.h"
#include "GameLogic/Module/DieModuleInterface.h"
#include "Common/ObjectIDList.h"

enum BridgeTowerType { BRIDGE_TOWER_FROM_LEFT = 0, BRIDGE_TOWER_FROM_RIGHT, BRIDGE_TOWER_TO_LEFT, BRIDGE_TOWER_TO_RIGHT, BRIDGE_MAX_TOWERS = 4 };
class FXList;
class ObjectCreationList;
class TerrainRoadType;
class Bridge;
class BridgeInfo;
class Coord3D;

// Six-slot table at8408FC, installed by ctor457472 at object+20.
// Native tower callers and scaffold methods corroborate the donor interface.
class BridgeBehaviorInterface {
public:
    virtual void setTower(BridgeTowerType, Object *) = 0;
    virtual ObjectID getTowerID(BridgeTowerType) = 0;
    virtual void createScaffolding() = 0;
    virtual void removeScaffolding() = 0;
    virtual bool isScaffoldInMotion() = 0;
    virtual bool isScaffoldPresent() = 0;
};

// Native ctor457472/dtor457113: UpdateModule0..1F, bridge interface20,
// damage interface24, die interface28; size108. Tower methods prove2C..38.
// Four 4x3 pointer arrays3C/6C/9C/CC and flagsFC/FD are initialized by ctor.
// xfer457E72 proves pooled ObjectID list100 and death frame104.
class BridgeBehavior : public UpdateModule, public BridgeBehaviorInterface,
    public DamageModuleInterface, public DieModuleInterface {
public:
    BridgeBehavior(Thing *, const ModuleData *);
    static Module *friend_newModuleInstance(Thing *thing, const ModuleData *moduleData);
    virtual ~BridgeBehavior();
    virtual NameKeyType getModuleNameKey() const;
    virtual UpdateSleepTime update();
    virtual void onDelete();
    virtual DamageModuleInterface *getDamage() { return this; }
    virtual DieModuleInterface *getDie() { return this; }
    virtual BridgeBehaviorInterface *getBridgeBehaviorInterface() { return this; }
    static BridgeBehaviorInterface *getBridgeBehaviorInterfaceFromObject(Object *);
    virtual void setTower(BridgeTowerType, Object *);
    virtual ObjectID getTowerID(BridgeTowerType);
    virtual void createScaffolding();
    virtual void removeScaffolding();
    virtual bool isScaffoldInMotion();
    virtual bool isScaffoldPresent() { return m_scaffoldPresent; }
    virtual void onDamage(DamageInfo *);
    virtual void onHealing(DamageInfo *);
    virtual void onBodyDamageStateChange(const DamageInfo *, BodyDamageType, BodyDamageType);
    virtual void onDie(const DamageInfo *);
protected:
    virtual const char *GetSnapshotName() const;
    virtual void xfer(Xfer *);
    virtual void loadPostProcess();
    void handleObjectsOnBridgeOnDie();
    void resolveFX();
    void getRandomSurfacePosition(TerrainRoadType *, const BridgeInfo *, Coord3D *);
    void doAreaEffects(TerrainRoadType *, Bridge *, const ObjectCreationList *, const FXList *);
    void setScaffoldData(Object *, float *, float *, const Coord3D *, const Coord3D *, const Coord3D *);
    ObjectID m_towerID[4];
    const ObjectCreationList *m_damageToOCL[4][3];
    const FXList *m_damageToFX[4][3];
    const ObjectCreationList *m_repairToOCL[4][3];
    const FXList *m_repairToFX[4][3];
    bool m_fxResolved;
    bool m_scaffoldPresent;
    ObjectIDList m_scaffoldObjectIDList;
    unsigned int m_deathFrame;
};
typedef char BridgeBehaviorSize[sizeof(BridgeBehavior)==0x108 ? 1 : -1];
