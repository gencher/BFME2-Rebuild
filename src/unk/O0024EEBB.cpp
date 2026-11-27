// cl: /O1 /D_BFME_RETAIL_TREE_INSERT_LAYOUT /D_STLP_USE_MALLOC /D_STLP_USE_STATIC_LIB /D_STLP_BFME_COPY_TRIVIAL_EXTERN /D_STLP_NO_EXCEPTIONS
// stlport46
// bfmealloc_alloconly
#include "Common/INI.h"
#include "GameLogic/Module/PorcupineFormationBodyModuleData.h"
#include "GameLogic/Module/SpecialPowerModule.h"
#include "GameLogic/Module/SupplyTruckAIUpdate.h"
#include "GameLogic/Module/CombineHordeSpecialPowerModuleData.h"
#include "GameLogic/Module/HordeDispatchSpecialPowerModuleData.h"
#include "GameLogic/Module/SiegeDeployHordeSpecialPowerModuleData.h"
#include "GameLogic/Module/SiegeDeploySpecialPowerModuleData.h"
#include "GameLogic/Module/ManTheWallsSpecialPowerModuleData.h"
#include "GameLogic/Module/RepairSpecialPowerModuleData.h"
#include "GameLogic/Module/NotifyTargetsOfImminentProbableCrushingUpdate.h"
#include "GameLogic/Module/Module.h"
#include "GameLogic/Module/UpdateModule.h"
#include "GameLogic/Module/ToggleHiddenSpecialAbilityUpdate.h"
#include "GameLogic/Module/ToggleMountedSpecialAbilityUpdateModuleData.h"
#include "GameLogic/Module/ToggleMountedSpecialAbilityUpdate.h"
#include "GameLogic/Module/GloriousChargeUpdate.h"
#include "GameLogic/Module/RousingSpeechUpdate.h"
#include "GameLogic/Module/PartTheHeavensUpdate.h"
#include "GameLogic/Module/DestroyEnvironmentUpdate.h"
#include "GameLogic/Module/LargeGroupAudioUpdate.h"
#include "GameLogic/Module/WanderAIUpdateModuleData.h"
#include "GameLogic/Module/WanderAIUpdate.h"
#include "GameLogic/Module/TransportAIUpdate.h"
#include "GameLogic/Module/DelayedLuaEventUpdateModuleData.h"
#include "GameLogic/Module/DelayedLuaEventUpdate.h"
#include "GameLogic/Module/ToppleUpdateModuleData.h"
#include "GameLogic/Module/CommandSetUpgrade.h"
#include "GameLogic/Module/BaseUpgrade.h"
#include "GameLogic/Module/ArmorUpgrade.h"
#include "GameLogic/Module/AISpecialPowerUpdateModuleData.h"
#include "GameLogic/Module/ReplaceObjectUpdate.h"
#include "GameLogic/Module/ProjectileStreamUpdate.h"
#include "GameLogic/Module/EntEnragedUpdate.h"
#include "GameLogic/Module/EmotionTrackerUpdateModuleData.h"
#include "GameLogic/Module/EmotionTrackerUpdate.h"
#include "GameLogic/Module/AIGateUpdateModuleData.h"
#include "GameLogic/Module/AIGateUpdate.h"
#include "GameLogic/Module/RespawnUpdateModuleData.h"
#include "GameLogic/Module/RespawnUpdate.h"
#include "GameLogic/Module/ToggleDeploySpecialAbilityUpdateModuleData.h"
#include "GameLogic/Module/WeaponSetUpgradeModuleData.h"
#include "GameLogic/Module/WeaponSetUpgrade.h"
#include "GameLogic/Module/WeaponBonusUpgrade.h"
#include "GameLogic/Module/UnpauseSpecialPowerUpgradeModuleData.h"
#include "GameLogic/Module/UnpauseSpecialPowerUpgrade.h"
#include "GameLogic/Module/StealthUpgrade.h"
#include "GameLogic/Module/SubObjectsUpgrade.h"
#include "GameLogic/Module/DoCommandUpgrade.h"
#include "GameLogic/Module/StatusBitsUpgradeIfEldestKindof.h"
#include "GameLogic/Module/StatusBitsUpgrade.h"
#include "GameLogic/Module/ObjectCreationUpgradeModuleData.h"
#include "GameLogic/Module/LocomotorSetUpgrade.h"
#include "GameLogic/Module/LevelUpUpgrade.h"
#include "GameLogic/Module/DelayedUpgrade.h"
#include "GameLogic/Module/ReplaceSelfUpgrade.h"
#include "GameLogic/Module/CastleUpgrade.h"
#include "GameLogic/Module/AttributeModifierUpgrade.h"
#include "GameLogic/Module/ModelConditionUpgradeModuleData.h"
#include "GameLogic/Module/ModelConditionUpgrade.h"
#include "GameLogic/Module/MaxHealthUpgradeModuleData.h"
#include "GameLogic/Module/MaxHealthUpgrade.h"
#include "GameLogic/Module/ExperienceScalarUpgradeModuleData.h"
#include "GameLogic/Module/ExperienceScalarUpgrade.h"
#include "GameLogic/Module/SpellRechargeModifierUpgradeModuleData.h"
#include "GameLogic/Module/SpellRechargeModifierUpgrade.h"
#include "GameLogic/Module/CostModifierUpgradeModuleData.h"
#include "GameLogic/Module/CostModifierUpgrade.h"
#include "GameLogic/Module/GarrisonUpgrade.h"
#include "GameLogic/Module/ReplaceSelfUpgradeModuleData.h"
#include "GameLogic/Module/SupplyCenterCreate.h"
#include "GameLogic/Module/LockWeaponCreateModuleData.h"
#include "GameLogic/Module/LockWeaponCreate.h"
#include "GameLogic/Module/OathbreakersFadeAwayBehavior.h"
#include "GameLogic/Module/CommandPointsUpgrade.h"
#include "GameLogic/Module/AllowBannerSpawnUpgrade.h"
#include "GameLogic/Module/BuildableHeroListUpgrade.h"
#include "GameLogic/Module/RemoveUpgradeUpgradeModuleData.h"
#include "GameLogic/Module/RemoveUpgradeUpgrade.h"
#include "GameLogic/Module/AudioLoopUpgradeModuleData.h"
#include "GameLogic/Module/TooltipUpgrade.h"
#include "GameLogic/Module/SupplyWarehouseCreate.h"
#include "GameLogic/Module/FireWeaponCollide.h"
#include "GameLogic/Module/HordeTransportContainDamageModuleData.h"
#include "GameLogic/Module/HordeTransportContainDamage.h"
#include "GameLogic/Module/ReflectDamage.h"
#include "GameLogic/Module/InheritUpgradeCreate.h"
#include "GameLogic/Module/SpecialPowerCreate.h"
#include "GameLogic/Module/PreorderCreate.h"
#include "GameLogic/Module/ExperienceLevelCreateModuleData.h"
#include "GameLogic/Module/GrantUpgradeCreateModuleData.h"
#include "GameLogic/Module/SquishCollide.h"
#include "GameLogic/Module/SymbioticStructuresBody.h"
#include "GameLogic/Module/StructureBody.h"
#include "GameLogic/Module/ImmortalBody.h"
#include "GameLogic/Module/HighlanderBody.h"
#include "GameLogic/Module/ActiveBodyModuleData.h"
#include "GameLogic/Module/ActiveBody.h"
#include "GameLogic/Module/InactiveBody.h"
#include "GameLogic/Module/CivilianSpawnCollideModuleData.h"
#include "GameLogic/Module/CrateCollideModuleData.h"
#include "GameLogic/Module/HordeMemberCollide.h"
#include "GameLogic/Module/AODCrushCollideModuleData.h"
#include "GameLogic/Module/AODCrushCollide.h"
#include "GameLogic/Module/InvisibilitySpecialPowerModuleData.h"
#include "GameLogic/Module/InvisibilitySpecialPower.h"
#include "GameLogic/Module/HeroDie.h"
#include "GameLogic/Module/PorcupineFormationBodyModule.h"
#include "GameLogic/Module/OathbreakerBody.h"
#include "GameLogic/Module/DetachableRiderBodyModuleData.h"
#include "GameLogic/Module/DetachableRiderBody.h"
#include "GameLogic/Module/FreeLifeBodyModuleData.h"
#include "GameLogic/Module/FreeLifeBody.h"
#include "GameLogic/Module/DelayedDeathBodyModuleData.h"
#include "GameLogic/Module/DelayedDeathBody.h"
#include "GameLogic/Module/RespawnBodyModuleData.h"
#include "GameLogic/Module/RespawnBody.h"
#include "GameLogic/Module/ScavengerSpecialPowerModuleData.h"
#include "GameLogic/Module/ScavengerSpecialPower.h"
#include "GameLogic/Module/WeaponChangeSpecialPowerModule.h"
#include "GameLogic/Module/ElvenWoodSpecialPowerModuleData.h"
#include "GameLogic/Module/ElvenWoodSpecialPower.h"
#include "GameLogic/Module/OCLSpecialPowerModuleData.h"
#include "GameLogic/Module/OCLSpecialPower.h"
#include "GameLogic/Module/ProductionSpeedBonusModuleData.h"
#include "GameLogic/Module/ProductionSpeedBonus.h"
#include "GameLogic/Module/LevelGrantSpecialPowerModuleData.h"
#include "GameLogic/Module/LevelGrantSpecialPower.h"
#include "GameLogic/Module/DefectorSpecialPowerModuleData.h"
#include "GameLogic/Module/DefectorSpecialPower.h"
#include "GameLogic/Module/StopSpecialPower.h"
#include "GameLogic/Module/SiegeDeploySpecialPower.h"
#include "GameLogic/Module/GrabPassengerSpecialPower.h"
#include "GameLogic/Module/DarknessSpecialPowerModuleData.h"
#include "GameLogic/Module/DarknessSpecialPower.h"
#include "GameLogic/Module/FreezingRainSpecialPowerModuleData.h"
#include "GameLogic/Module/FreezingRainSpecialPower.h"
#include "GameLogic/Module/TaintSpecialPowerModuleData.h"
#include "GameLogic/Module/TaintSpecialPower.h"
#include "GameLogic/Module/CloudBreakSpecialPower.h"
#include "GameLogic/Module/SplitHordeSpecialPower.h"
#include "GameLogic/Module/DevastateSpecialPower.h"
#include "GameLogic/Module/PlayerHealSpecialPower.h"
#include "GameLogic/Module/PlayerUpgradeSpecialPower.h"
#include "GameLogic/Module/UntamedAllegianceSpecialPowerModuleData.h"
#include "GameLogic/Module/UntamedAllegianceSpecialPower.h"
#include "GameLogic/Module/ManTheWallsSpecialPower.h"
#include "GameLogic/Module/FellBeastSwoopPower.h"
#include "GameLogic/Module/WoundArrowUpdateModuleData.h"
#include "GameLogic/Module/WoundArrowUpdate.h"
#include "GameLogic/Module/StopSpecialPowerModuleData.h"
#include "GameLogic/Module/BeaconClientUpdateModuleData.h"
#include "GameLogic/Module/BeaconClientUpdate.h"
#include "GameLogic/Module/SwayClientUpdate.h"
#include "GameLogic/Module/AnimatedParticleSysBoneClientUpdate.h"
#include "GameLogic/Module/CritterEmitterUpdateModuleData.h"
#include "GameLogic/Module/CritterEmitterUpdate.h"
#include "GameLogic/Module/HordeDispatchSpecialPower.h"
#include "GameLogic/Module/CombineHordeSpecialPower.h"
#include "GameLogic/Module/RepairSpecialPower.h"
#include "GameLogic/Module/ActivateModuleSpecialPower.h"
#include "GameLogic/Module/EvacuateGarrisonSpecialPowerModuleData.h"
#include "GameLogic/Module/EvacuateGarrisonSpecialPower.h"
#include "GameLogic/Module/GiveOrRestoreUpgradeSpecialPower.h"
#include "GameLogic/Module/CurseSpecialPowerModuleData.h"
#include "GameLogic/Module/CurseSpecialPower.h"
#include "GameLogic/Module/DominateEnemySpecialPowerModuleData.h"
#include "GameLogic/Module/DominateEnemySpecialPower.h"
#include "GameLogic/Module/TemporarilyDefectUpdate.h"
#include "GameLogic/Module/RadarMarkerClientUpdateModuleData.h"
#include "GameLogic/Module/RadarMarkerClientUpdate.h"
#include "GameLogic/Module/CommandPointsUpgradeModuleData.h"
#include "GameLogic/Module/WorkerAIUpdateModuleData.h"
#include "GameLogic/Module/RadarUpdateModuleData.h"
#include "GameLogic/Module/AutoDepositUpdateModuleData.h"
#include "GameLogic/Module/GateOpenAndCloseBehaviorModuleData.h"
#include "GameLogic/Module/KeepObjectDieModuleData.h"
#include "GameLogic/Module/SpecialPowerCompletionDieModuleData.h"
#include "GameLogic/Module/CrushDieModuleData.h"
#include "GameLogic/Module/FireWeaponWhenDeadBehaviorModuleData.h"
#include "GameLogic/Module/UnleashSpecialPower.h"
#include "GameLogic/Module/FXListDie.h"
#include "GameLogic/Module/FXListDieModuleData.h"
#include "GameLogic/Module/QueueProductionExitUpdateModuleData.h"
#include "GameLogic/Module/TeleportToCasterSpecialPowerModuleData.h"
#include "GameLogic/Module/TeleportToCasterSpecialPower.h"
#include "GameLogic/Module/SalvageCrateCollide.h"
#include "GameLogic/Module/VeterancyCrateCollide.h"
#include "GameLogic/Module/UnitCrateCollide.h"
#include "GameLogic/Module/ShroudCrateCollide.h"
#include "GameLogic/Module/MoneyCrateCollide.h"
#include "GameLogic/Module/HealCrateCollide.h"
#include "GameLogic/Module/BoneFXDamage.h"
#include "GameLogic/Module/GrantUpgradeCreate.h"
#include "GameLogic/Module/GeometryUpgrade.h"
#include "GameLogic/Module/RadarUpgrade.h"
#include "GameLogic/Module/SpecialDisguiseUpdate.h"
#include "GameLogic/Module/RainOfFireUpdate.h"
#include "GameLogic/Module/WallUpgradeUpdate.h"
#include "GameLogic/Module/SupplyWarehouseDockUpdate.h"
#include "GameLogic/Module/MissileUpdate.h"
#include "Common/INI/FieldParse.h"
#include "GameLogic/Module/ToggleHiddenSpecialAbilityUpdateModuleData.h"
#include "GameLogic/Module/MoneyCrateCollideModuleData.h"
#include "GameLogic/Module/UnitCrateCollideModuleData.h"
#include "GameLogic/Module/VeterancyCrateCollideModuleData.h"
#include "GameLogic/Module/SalvageCrateCollideModuleData.h"
#include "GameLogic/Module/StructureBodyModuleData.h"
#include "GameLogic/Module/SymbioticStructuresBodyModuleData.h"
#include "GameLogic/Module/FellBeastSwoopPowerModuleData.h"
#include "GameLogic/Module/DefaultProductionExitUpdateModuleData.h"
#include "GameLogic/Module/SlavedUpdateModuleData.h"
#include "GameLogic/Module/SpawnPointProductionExitUpdateModuleData.h"
#include "GameLogic/Module/HijackerUpdateModuleData.h"
#include "GameLogic/Module/SpecialEnemySenseUpdateModuleData.h"
#include "GameLogic/Module/RefundDieModuleData.h"
#include "GameLogic/Module/DieMuxData.h"
#include "GameLogic/Module/CreateCrateDieModuleData.h"
#include "GameLogic/Module/UpgradeDieModuleData.h"
#include "GameLogic/Module/PillageModuleData.h"
#include "GameLogic/Module/UnleashSpecialPowerModuleData.h"
#include "GameLogic/Module/StoreObjectsSpecialPowerModuleData.h"
#include "GameLogic/Module/GiveOrRestoreUpgradeSpecialPowerModuleData.h"
#include "GameLogic/Module/ModelConditionAudioLoopClientBehaviorModuleData.h"
#include "GameLogic/Module/UpgradeSoundSelectorClientBehaviorModuleData.h"
#include "GameLogic/Module/ModelConditionSoundSelectorClientBehaviorModuleData.h"
#include "GameLogic/Module/AnimationSoundClientBehaviorModuleData.h"
#include "GameLogic/Module/EvaAnnounceClientCreateModuleData.h"
#include "GameLogic/Module/DevastateSpecialPowerModuleData.h"
#include "GameLogic/Module/PlayerHealSpecialPowerModuleData.h"
#include "GameLogic/Module/PlayerUpgradeSpecialPowerModuleData.h"
#include "GameLogic/Module/GrabPassengerSpecialPowerModuleData.h"
#include "GameLogic/Module/WeaponChangeSpecialPowerModuleData.h"
#include "GameLogic/Module/CallHelpOnDamageModuleData.h"
#include "GameLogic/Module/EvacuateDamageModuleData.h"
#include "GameLogic/Module/ReflectDamageModuleData.h"
#include "GameLogic/Module/InheritUpgradeCreateModuleData.h"
#include "GameLogic/Module/GeometryUpgradeModuleData.h"
#include "GameLogic/Module/BaseUpgradeModuleData.h"
#include "GameLogic/Module/ReplaceObjectUpdateModuleData.h"
#include "GameLogic/Module/EntEnragedUpdateModuleData.h"
#include "GameLogic/Module/GloriousChargeUpdateModuleData.h"
#include "GameLogic/Module/RousingSpeechUpdateModuleData.h"
#include "GameLogic/Module/PartTheHeavensUpdateModuleData.h"
#include "GameLogic/Module/DestroyEnvironmentUpdateModuleData.h"
#include "GameLogic/Module/LargeGroupAudioUpdateModuleData.h"
#include "GameLogic/Module/DeletionUpdateModuleData.h"
#include "GameLogic/Module/FadeAndDieOrnamentUpdateModuleData.h"
#include "GameLogic/Module/FadeAndDieOrnamentUpdateModuleData.h"
#include "GameLogic/Module/ReplaceObjectUpdateModuleData.h"
#include "GameLogic/Module/CallHelpOnDamage.h"
#include "GameLogic/Module/EvacuateDamage.h"
#include "GameLogic/Module/AudioLoopUpgrade.h"
#include "GameLogic/Module/AISpecialPowerUpdate.h"
#include "GameLogic/Module/ExperienceLevelCreate.h"
#include "GameLogic/Module/CivilianSpawnCollide.h"
#include "GameLogic/Module/TransitionDamageFX.h"
#include "GameLogic/Module/ToppleUpdate.h"
#include "GameLogic/Module/BoneFXUpdate.h"
#include "GameLogic/Module/CloudBreakSpecialPowerModuleData.h"
#include "GameLogic/Module/CashHackSpecialPower.h"
#include "GameLogic/Module/DetachableRiderUpdateModuleData.h"
#include "GameLogic/Module/AssistedTargetingUpdateModuleData.h"
#include "GameLogic/Module/FireWeaponWhenDamagedBehaviorModuleData.h"
#include "GameLogic/Module/LevelUpUpgradeModuleData.h"
#include "GameLogic/Module/MissileUpdateModuleData.h"
#include "GameLogic/Module/OathbreakersFadeAwayBehaviorModuleData.h"
#include "GameLogic/Module/StatusBitsUpgradeModuleData.h"
// Module creators. Retail registers each module class with a static friend_newModuleInstance that allocates the
// module for a thing and its module data, and each module data class with a static friend_newModuleData that
// allocates the data and, when an INI block is open, fills it from the class's buildFieldParse table. The bodies
// below are those two shapes. A class whose creator is not here yet has no real header, lacks the constructor or
// buildFieldParse declaration, or did not compile with its current header (the usual cause is a pure virtual that
// the header does not override yet); its row stays unwritten.
// Generated from the ledger; map snapshot c591f7146f88c8a8248ebb8ae22f0c978c22aed463db64e05ef57ff0dfae99cc
// object=O0024EEBB name-evidence=W
// Extents and edges are hypotheses; byte verification alone does not establish original TU boundaries.
// FUNCTION: BFME2 0x0024EE77 size=68 name=??0SupplyTruckAIUpdateModuleData@@QAE@XZ evidence=N:ledger tier=B hypothesis:mm1-neighbour-callers+class
SupplyTruckAIUpdateModuleData::SupplyTruckAIUpdateModuleData()
{
	m_maxBoxesData = 0;
	m_centerDelay = 0;
	m_warehouseDelay = 0;
	m_warehouseScanDistance = 100;
	m_harvestTrees = false;
	m_harvestActivationRange = 50;
	m_harvestPreparationTime = 1;
	m_harvestActionTime = 1;
}

static const FieldParse s_supplyTruckFields[] =
{
	{ "MaxBoxes", INI::parseInt, 0, offsetof(SupplyTruckAIUpdateModuleData, m_maxBoxesData) },
	{ "SupplyCenterActionDelay", INI::parseDurationUnsignedInt, 0, offsetof(SupplyTruckAIUpdateModuleData, m_centerDelay) },
	{ "SupplyWarehouseActionDelay", INI::parseDurationUnsignedInt, 0, offsetof(SupplyTruckAIUpdateModuleData, m_warehouseDelay) },
	{ "SupplyWarehouseScanDistance", INI::parseReal, 0, offsetof(SupplyTruckAIUpdateModuleData, m_warehouseScanDistance) },
	{ "HarvestTrees", INI::parseBool, 0, offsetof(SupplyTruckAIUpdateModuleData, m_harvestTrees) },
	{ "HarvestActivationRange", INI::parseReal, 0, offsetof(SupplyTruckAIUpdateModuleData, m_harvestActivationRange) },
	{ "HarvestPreparationTime", INI::parseDurationUnsignedInt, 0, offsetof(SupplyTruckAIUpdateModuleData, m_harvestPreparationTime) },
	{ "HarvestActionTime", INI::parseDurationUnsignedInt, 0, offsetof(SupplyTruckAIUpdateModuleData, m_harvestActionTime) },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x0024EEBB size=27 name=?buildFieldParse@SupplyTruckAIUpdateModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void SupplyTruckAIUpdateModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	AIUpdateModuleData::buildFieldParse(parse);
	parse.add(s_supplyTruckFields, 0);
}

// FUNCTION: BFME2 0x0024EED6 size=59 name=?friend_newModuleInstance@SupplyTruckAIUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *SupplyTruckAIUpdate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new SupplyTruckAIUpdate(thing, moduleData);
}

// FUNCTION: BFME2 0x0024EF11 size=84 name=?friend_newModuleData@SupplyTruckAIUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *SupplyTruckAIUpdateModuleData::friend_newModuleData(INI *ini)
{
	SupplyTruckAIUpdateModuleData *data = new SupplyTruckAIUpdateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, SupplyTruckAIUpdateModuleData::buildFieldParse);
	return data;
}

// FUNCTION: BFME2 0x0024EF65 size=59 name=?friend_newModuleInstance@MissileUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *MissileUpdate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new MissileUpdate(thing, moduleData);
}
// FUNCTION: BFME2 0x0024EFA0 size=84 name=?friend_newModuleData@MissileUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *MissileUpdateModuleData::friend_newModuleData(INI *ini)
{
	MissileUpdateModuleData *data = new MissileUpdateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, MissileUpdateModuleData::buildFieldParse);
	return data;
}

// FUNCTION: BFME2 0x0024EFF4 size=59 name=?friend_newModuleInstance@SupplyWarehouseDockUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *SupplyWarehouseDockUpdate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new SupplyWarehouseDockUpdate(thing, moduleData);
}
// FUNCTION: BFME2 0x0024F02F size=81 name=?friend_newModuleData@SupplyWarehouseDockUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x0024F080 size=56 name=?friend_newModuleInstance@ToppleUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *ToppleUpdate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new ToppleUpdate(thing, moduleData);
}
// FUNCTION: BFME2 0x0024F0B8 size=81 name=?friend_newModuleData@ToppleUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *ToppleUpdateModuleData::friend_newModuleData(INI *ini)
{
	ToppleUpdateModuleData *data = new ToppleUpdateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, ToppleUpdateModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x0024F109 size=56 name=?friend_newModuleInstance@DelayedLuaEventUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *DelayedLuaEventUpdate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new DelayedLuaEventUpdate(thing, moduleData);
}
// FUNCTION: BFME2 0x0024F141 size=81 name=?friend_newModuleData@DelayedLuaEventUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *DelayedLuaEventUpdateModuleData::friend_newModuleData(INI *ini)
{
	DelayedLuaEventUpdateModuleData *data = new DelayedLuaEventUpdateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, DelayedLuaEventUpdateModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x0024F192 size=59 name=?friend_newModuleInstance@TransportAIUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *TransportAIUpdate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new TransportAIUpdate(thing, moduleData);
}
// FUNCTION: BFME2 0x0024F1CD size=37 name=??0WanderAIUpdateModuleData@@QAE@XZ evidence=N:ledger tier=A
static const FieldParse s_wanderAIUpdateModuleDataFieldParse[] =
{
	{ "WildBeast", INI::parseBool, 0, 0x64 },
	{ "ConditionForEntry", INI::iniParseAnim, 0, 0x68 },
	{ "Selectable", INI::parseBool, 0, 0x6C },
	{ "WanderDistance", INI::parseUnsignedInt, 0, 0x70 },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x0024F1F2 size=27 name=?buildFieldParse@WanderAIUpdateModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void WanderAIUpdateModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	AIUpdateModuleData::buildFieldParse(parse);
	parse.add(s_wanderAIUpdateModuleDataFieldParse, 0);
}

// FUNCTION: BFME2 0x0024F20D size=59 name=?friend_newModuleInstance@WanderAIUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *WanderAIUpdate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new WanderAIUpdate(thing, moduleData);
}
// FUNCTION: BFME2 0x0024F248 size=81 name=?friend_newModuleData@WanderAIUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *WanderAIUpdateModuleData::friend_newModuleData(INI *ini)
{
	WanderAIUpdateModuleData *data = new WanderAIUpdateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, WanderAIUpdateModuleData::buildFieldParse);
	return data;
}
static const FieldParse s_workerAIUpdateModuleDataFieldParse[] =
{
	{ "MaxBoxes", INI::parseInt, 0, 0x64 },
	{ "RepairHealthPercentPerSecond", INI::parsePercentToReal, 0, 0x68 },
	{ "BoredTime", INI::parseDurationReal, 0, 0x6C },
	{ "BoredRange", INI::parseReal, 0, 0x70 },
	{ "SupplyCenterActionDelay", INI::parseDurationUnsignedInt, 0, 0x74 },
	{ "SupplyWarehouseActionDelay", INI::parseDurationUnsignedInt, 0, 0x78 },
	{ "SupplyWarehouseScanDistance", INI::parseReal, 0, 0x7C },
	{ "HarvestTrees", INI::parseBool, 0, 0x80 },
	{ "SuppliesDepletedVoice", INI::parseAudioEventRTS, 0, 0x90 },
	{ "HarvestActivationRange", INI::parseReal, 0, 0x84 },
	{ "HarvestPreparationTime", INI::parseDurationUnsignedInt, 0, 0x88 },
	{ "HarvestActionTime", INI::parseDurationUnsignedInt, 0, 0x8C },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x0024F299 size=27 name=?buildFieldParse@WorkerAIUpdateModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void WorkerAIUpdateModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	AIUpdateModuleData::buildFieldParse(parse);
	parse.add(s_workerAIUpdateModuleDataFieldParse, 0);
}

// FUNCTION: BFME2 0x0024F2B4 size=59 name=?friend_newModuleInstance@WorkerAIUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x0024F2EF size=56 name=?friend_newModuleInstance@WallUpgradeUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *WallUpgradeUpdate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new WallUpgradeUpdate(thing, moduleData);
}
// FUNCTION: BFME2 0x0024F327 size=59 name=?friend_newModuleInstance@LargeGroupAudioUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *LargeGroupAudioUpdate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new LargeGroupAudioUpdate(thing, moduleData);
}
// FUNCTION: BFME2 0x0024F362 size=81 name=?friend_newModuleData@LargeGroupAudioUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *LargeGroupAudioUpdateModuleData::friend_newModuleData(INI *ini)
{
	LargeGroupAudioUpdateModuleData *data = new LargeGroupAudioUpdateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, LargeGroupAudioUpdateModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x0024F3B3 size=56 name=?friend_newModuleInstance@RainOfFireUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *RainOfFireUpdate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new RainOfFireUpdate(thing, moduleData);
}
// FUNCTION: BFME2 0x0024F3EB size=81 name=?friend_newModuleData@RainOfFireUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *RainOfFireUpdateModuleData::friend_newModuleData(INI *ini)
{
	RainOfFireUpdateModuleData *data = new RainOfFireUpdateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, RainOfFireUpdateModuleData::buildFieldParse);
	return data;
}

// FUNCTION: BFME2 0x0024F43C size=56 name=?friend_newModuleInstance@DestroyEnvironmentUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *DestroyEnvironmentUpdate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new DestroyEnvironmentUpdate(thing, moduleData);
}
// FUNCTION: BFME2 0x0024F474 size=81 name=?friend_newModuleData@DestroyEnvironmentUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *DestroyEnvironmentUpdateModuleData::friend_newModuleData(INI *ini)
{
	DestroyEnvironmentUpdateModuleData *data = new DestroyEnvironmentUpdateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, DestroyEnvironmentUpdateModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x0024F4C5 size=56 name=?friend_newModuleInstance@PartTheHeavensUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *PartTheHeavensUpdate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new PartTheHeavensUpdate(thing, moduleData);
}
// FUNCTION: BFME2 0x0024F4FD size=84 name=?friend_newModuleData@PartTheHeavensUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *PartTheHeavensUpdateModuleData::friend_newModuleData(INI *ini)
{
	PartTheHeavensUpdateModuleData *data = new PartTheHeavensUpdateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, PartTheHeavensUpdateModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x0024F551 size=59 name=?friend_newModuleInstance@RousingSpeechUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *RousingSpeechUpdate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new RousingSpeechUpdate(thing, moduleData);
}
// FUNCTION: BFME2 0x0024F58C size=84 name=?friend_newModuleData@RousingSpeechUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *RousingSpeechUpdateModuleData::friend_newModuleData(INI *ini)
{
	RousingSpeechUpdateModuleData *data = new RousingSpeechUpdateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, RousingSpeechUpdateModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x0024F5E0 size=59 name=?friend_newModuleInstance@GloriousChargeUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *GloriousChargeUpdate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new GloriousChargeUpdate(thing, moduleData);
}
// FUNCTION: BFME2 0x0024F61B size=84 name=?friend_newModuleData@GloriousChargeUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *GloriousChargeUpdateModuleData::friend_newModuleData(INI *ini)
{
	GloriousChargeUpdateModuleData *data = new GloriousChargeUpdateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, GloriousChargeUpdateModuleData::buildFieldParse);
	return data;
}
static const FieldParse s_toggleMountedSpecialAbilityUpdateModuleDataFieldParse[] =
{
	{ "OpacityTarget", INI::parseReal, 0, 0xC8 },
	{ "TriggerInstantlyOnCreate", INI::parseBool, 0, 0xCC },
	{ "CancelDisguiseWhenDismounting", INI::parseBool, 0, 0xCD },
	{ "MountedTemplate", INI::parseAsciiString, 0, 0xD0 },
	{ "SynchronizeTimerOnSpecialPower", INI::parseAsciiStringVector, 0, 0xD4 },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x0024F66F size=27 name=?buildFieldParse@ToggleMountedSpecialAbilityUpdateModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void ToggleMountedSpecialAbilityUpdateModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	SpecialAbilityUpdateModuleData::buildFieldParse(parse);
	parse.add(s_toggleMountedSpecialAbilityUpdateModuleDataFieldParse, 0);
}

// FUNCTION: BFME2 0x0024F68A size=59 name=?friend_newModuleInstance@ToggleMountedSpecialAbilityUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *ToggleMountedSpecialAbilityUpdate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new ToggleMountedSpecialAbilityUpdate(thing, moduleData);
}
// FUNCTION: BFME2 0x0024F6C5 size=84 name=?friend_newModuleData@ToggleMountedSpecialAbilityUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *ToggleMountedSpecialAbilityUpdateModuleData::friend_newModuleData(INI *ini)
{
	ToggleMountedSpecialAbilityUpdateModuleData *data = new ToggleMountedSpecialAbilityUpdateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, ToggleMountedSpecialAbilityUpdateModuleData::buildFieldParse);
	return data;
}
static const FieldParse s_toggleHiddenSpecialAbilityUpdateModuleDataFieldParse[] =
{
	{ "ShowPalantirTimer", INI::parseBool, 0, 0xC8 },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x0024F719 size=27 name=?buildFieldParse@ToggleHiddenSpecialAbilityUpdateModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void ToggleHiddenSpecialAbilityUpdateModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	SpecialAbilityUpdateModuleData::buildFieldParse(parse);
	parse.add(s_toggleHiddenSpecialAbilityUpdateModuleDataFieldParse, 0);
}

// FUNCTION: BFME2 0x0024F734 size=59 name=?friend_newModuleInstance@ToggleHiddenSpecialAbilityUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *ToggleHiddenSpecialAbilityUpdate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new ToggleHiddenSpecialAbilityUpdate(thing, moduleData);
}
// FUNCTION: BFME2 0x0024F76F size=84 name=?friend_newModuleData@ToggleHiddenSpecialAbilityUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *ToggleHiddenSpecialAbilityUpdateModuleData::friend_newModuleData(INI *ini)
{
	ToggleHiddenSpecialAbilityUpdateModuleData *data = new ToggleHiddenSpecialAbilityUpdateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, ToggleHiddenSpecialAbilityUpdateModuleData::buildFieldParse);
	return data;
}
static const FieldParse s_toggleDeploySpecialAbilityUpdateModuleDataFieldParse[] =
{
	{ "SoundDeploy", INI::parseAudioEventRTS, 0, 0xC8 },
	{ "SoundUndeploy", INI::parseAudioEventRTS, 0, 0xCC },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x0024F7C3 size=27 name=?buildFieldParse@ToggleDeploySpecialAbilityUpdateModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void ToggleDeploySpecialAbilityUpdateModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	SpecialAbilityUpdateModuleData::buildFieldParse(parse);
	parse.add(s_toggleDeploySpecialAbilityUpdateModuleDataFieldParse, 0);
}

// FUNCTION: BFME2 0x0024F7DE size=59 name=?friend_newModuleInstance@ToggleDeploySpecialAbilityUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x0024F819 size=84 name=?friend_newModuleData@ToggleDeploySpecialAbilityUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *ToggleDeploySpecialAbilityUpdateModuleData::friend_newModuleData(INI *ini)
{
	ToggleDeploySpecialAbilityUpdateModuleData *data = new ToggleDeploySpecialAbilityUpdateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, ToggleDeploySpecialAbilityUpdateModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x0024F86D size=56 name=?friend_newModuleInstance@DetachableRiderUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x0024F8A5 size=81 name=?friend_newModuleData@DetachableRiderUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *DetachableRiderUpdateModuleData::friend_newModuleData(INI *ini)
{
	DetachableRiderUpdateModuleData *data = new DetachableRiderUpdateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, DetachableRiderUpdateModuleData::buildFieldParse);
	return data;
}

// FUNCTION: BFME2 0x0024F8F6 size=56 name=?friend_newModuleInstance@RespawnUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *RespawnUpdate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new RespawnUpdate(thing, moduleData);
}
// FUNCTION: BFME2 0x0024F92E size=84 name=?friend_newModuleData@RespawnUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *RespawnUpdateModuleData::friend_newModuleData(INI *ini)
{
	RespawnUpdateModuleData *data = new RespawnUpdateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, RespawnUpdateModuleData::buildFieldParse);
	return data;
}
static const FieldParse s_specialDisguiseUpdateModuleDataFieldParse[] =
{
	{ "OpacityTarget", INI::parseReal, 0, 0xCC },
	{ "TriggerInstantlyOnCreate", INI::parseBool, 0, 0xC8 },
	{ "DisguiseAsTemplate", INI::parseAsciiString, 0, 0xD0 },
	{ "DisguisedAsTemplate_EnemyPerspective", INI::parseAsciiString, 0, 0xD4 },
	{ "DisguiseFX", INI::parseFXList, 0, 0xD8 },
	{ "ForceMountedWhenDisguising", INI::parseBool, 0, 0xDC },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x0024F982 size=27 name=?buildFieldParse@SpecialDisguiseUpdateModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void SpecialDisguiseUpdateModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	SpecialAbilityUpdateModuleData::buildFieldParse(parse);
	parse.add(s_specialDisguiseUpdateModuleDataFieldParse, 0);
}

// FUNCTION: BFME2 0x0024F99D size=59 name=?friend_newModuleInstance@SpecialDisguiseUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *SpecialDisguiseUpdate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new SpecialDisguiseUpdate(thing, moduleData);
}
// FUNCTION: BFME2 0x0024F9D8 size=84 name=?friend_newModuleData@SpecialDisguiseUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *SpecialDisguiseUpdateModuleData::friend_newModuleData(INI *ini)
{
	SpecialDisguiseUpdateModuleData *data = new SpecialDisguiseUpdateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, SpecialDisguiseUpdateModuleData::buildFieldParse);
	return data;
}

// FUNCTION: BFME2 0x0024FA2C size=56 name=?friend_newModuleInstance@AIGateUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *AIGateUpdate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new AIGateUpdate(thing, moduleData);
}
// FUNCTION: BFME2 0x0024FA64 size=81 name=?friend_newModuleData@AIGateUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *AIGateUpdateModuleData::friend_newModuleData(INI *ini)
{
	AIGateUpdateModuleData *data = new AIGateUpdateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, AIGateUpdateModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x0024FAB5 size=59 name=?friend_newModuleInstance@EmotionTrackerUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *EmotionTrackerUpdate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new EmotionTrackerUpdate(thing, moduleData);
}
// FUNCTION: BFME2 0x0024FAF0 size=81 name=?friend_newModuleData@EmotionTrackerUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *EmotionTrackerUpdateModuleData::friend_newModuleData(INI *ini)
{
	EmotionTrackerUpdateModuleData *data = new EmotionTrackerUpdateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, EmotionTrackerUpdateModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x0024FB41 size=6 name=?Rva0024FB41Get@@YAHXZ evidence=?:placeholder tier=A
// No retail code or data refers to this function. It follows the creators of EmotionTrackerUpdate and returns the bit set 0x201, which looks like a module interface mask;
// nothing ties it to EmotionTrackerUpdate, so it keeps its ledger name.
int Rva0024FB41Get()
{
	return 513;
}
// FUNCTION: BFME2 0x0024FB47 size=56 name=?friend_newModuleInstance@EntEnragedUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *EntEnragedUpdate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new EntEnragedUpdate(thing, moduleData);
}
// FUNCTION: BFME2 0x0024FB7F size=81 name=?friend_newModuleData@EntEnragedUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *EntEnragedUpdateModuleData::friend_newModuleData(INI *ini)
{
	EntEnragedUpdateModuleData *data = new EntEnragedUpdateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, EntEnragedUpdateModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x0024FBD0 size=56 name=?friend_newModuleInstance@ThreatFinderUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x0024FC08 size=81 name=?friend_newModuleData@ThreatFinderUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x0024FC59 size=59 name=?friend_newModuleInstance@ProjectileStreamUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *ProjectileStreamUpdate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new ProjectileStreamUpdate(thing, moduleData);
}
static const FieldParse s_replaceObjectUpdateModuleDataFieldParse[] =
{
	{ "ReplaceObject", INI::Rva004B2B64_ParseReplaceObject, 0, 0x0 },
	{ "ReplaceRadius", INI::parseReal, 0, 0xD4 },
	{ "ReplaceFX", INI::parseFXList, 0, 0xD8 },
	{ "Scatter", INI::parseBool, 0, 0xDC },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x0024FC94 size=27 name=?buildFieldParse@ReplaceObjectUpdateModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void ReplaceObjectUpdateModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	SpecialAbilityUpdateModuleData::buildFieldParse(parse);
	parse.add(s_replaceObjectUpdateModuleDataFieldParse, 0);
}

// FUNCTION: BFME2 0x0024FCAF size=59 name=?friend_newModuleInstance@ReplaceObjectUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *ReplaceObjectUpdate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new ReplaceObjectUpdate(thing, moduleData);
}
// FUNCTION: BFME2 0x0024FCEA size=84 name=?friend_newModuleData@ReplaceObjectUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *ReplaceObjectUpdateModuleData::friend_newModuleData(INI *ini)
{
	ReplaceObjectUpdateModuleData *data = new ReplaceObjectUpdateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, ReplaceObjectUpdateModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x0024FD3E size=56 name=?friend_newModuleInstance@AISpecialPowerUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *AISpecialPowerUpdate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new AISpecialPowerUpdate(thing, moduleData);
}
// FUNCTION: BFME2 0x0024FD76 size=81 name=?friend_newModuleData@AISpecialPowerUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *AISpecialPowerUpdateModuleData::friend_newModuleData(INI *ini)
{
	AISpecialPowerUpdateModuleData *data = new AISpecialPowerUpdateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, AISpecialPowerUpdateModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x0024FDC7 size=56 name=?friend_newModuleInstance@ArmorUpgrade@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *ArmorUpgrade::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new ArmorUpgrade(thing, moduleData);
}
// FUNCTION: BFME2 0x0024FDFF size=56 name=?friend_newModuleInstance@BaseUpgrade@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *BaseUpgrade::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new BaseUpgrade(thing, moduleData);
}
// FUNCTION: BFME2 0x0024FE37 size=84 name=?friend_newModuleData@BaseUpgradeModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *BaseUpgradeModuleData::friend_newModuleData(INI *ini)
{
	BaseUpgradeModuleData *data = new BaseUpgradeModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, BaseUpgradeModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x0024FE8B size=56 name=?friend_newModuleInstance@CommandSetUpgrade@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *CommandSetUpgrade::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new CommandSetUpgrade(thing, moduleData);
}
// FUNCTION: BFME2 0x0024FEC3 size=56 name=?friend_newModuleInstance@DelayedUpgrade@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *DelayedUpgrade::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new DelayedUpgrade(thing, moduleData);
}
// FUNCTION: BFME2 0x0024FEFB size=56 name=?friend_newModuleInstance@LevelUpUpgrade@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *LevelUpUpgrade::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new LevelUpUpgrade(thing, moduleData);
}
// FUNCTION: BFME2 0x0024FF33 size=56 name=?friend_newModuleInstance@LocomotorSetUpgrade@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *LocomotorSetUpgrade::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new LocomotorSetUpgrade(thing, moduleData);
}
// FUNCTION: BFME2 0x0024FF6B size=65 name=?friend_newModuleInstance@ObjectCreationUpgrade@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x0024FFAC size=84 name=?friend_newModuleData@ObjectCreationUpgradeModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *ObjectCreationUpgradeModuleData::friend_newModuleData(INI *ini)
{
	ObjectCreationUpgradeModuleData *data = new ObjectCreationUpgradeModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, ObjectCreationUpgradeModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00250000 size=6 name=?Rva00250000Get@@YAHXZ evidence=?:placeholder tier=A
// It follows the creators of ObjectCreationUpgrade and returns the bit set 0x81, which looks like a module interface mask;
// nothing ties it to ObjectCreationUpgrade, so it keeps its ledger name.
int Rva00250000Get()
{
	return 129;
}
// FUNCTION: BFME2 0x00250006 size=56 name=?friend_newModuleInstance@RadarUpgrade@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *RadarUpgrade::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new RadarUpgrade(thing, moduleData);
}
// FUNCTION: BFME2 0x0025003E size=56 name=?friend_newModuleInstance@StatusBitsUpgrade@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *StatusBitsUpgrade::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new StatusBitsUpgrade(thing, moduleData);
}
// FUNCTION: BFME2 0x00250076 size=56 name=?friend_newModuleInstance@StatusBitsUpgradeIfEldestKindof@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *StatusBitsUpgradeIfEldestKindof::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new StatusBitsUpgradeIfEldestKindof(thing, moduleData);
}
// FUNCTION: BFME2 0x002500AE size=56 name=?friend_newModuleInstance@DoCommandUpgrade@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *DoCommandUpgrade::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new DoCommandUpgrade(thing, moduleData);
}
// FUNCTION: BFME2 0x002500E6 size=56 name=?friend_newModuleInstance@SubObjectsUpgrade@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *SubObjectsUpgrade::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new SubObjectsUpgrade(thing, moduleData);
}
// FUNCTION: BFME2 0x0025011E size=56 name=?friend_newModuleInstance@StealthUpgrade@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *StealthUpgrade::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new StealthUpgrade(thing, moduleData);
}
// FUNCTION: BFME2 0x00250156 size=56 name=?friend_newModuleInstance@UnpauseSpecialPowerUpgrade@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *UnpauseSpecialPowerUpgrade::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new UnpauseSpecialPowerUpgrade(thing, moduleData);
}
// FUNCTION: BFME2 0x0025018E size=84 name=?friend_newModuleData@UnpauseSpecialPowerUpgradeModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *UnpauseSpecialPowerUpgradeModuleData::friend_newModuleData(INI *ini)
{
	UnpauseSpecialPowerUpgradeModuleData *data = new UnpauseSpecialPowerUpgradeModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, UnpauseSpecialPowerUpgradeModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x002501E2 size=56 name=?friend_newModuleInstance@WeaponBonusUpgrade@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *WeaponBonusUpgrade::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new WeaponBonusUpgrade(thing, moduleData);
}
// FUNCTION: BFME2 0x0025021A size=56 name=?friend_newModuleInstance@WeaponSetUpgrade@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *WeaponSetUpgrade::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new WeaponSetUpgrade(thing, moduleData);
}
// FUNCTION: BFME2 0x00250252 size=84 name=?friend_newModuleData@WeaponSetUpgradeModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *WeaponSetUpgradeModuleData::friend_newModuleData(INI *ini)
{
	WeaponSetUpgradeModuleData *data = new WeaponSetUpgradeModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, WeaponSetUpgradeModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x002502A6 size=56 name=?friend_newModuleInstance@GarrisonUpgrade@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *GarrisonUpgrade::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new GarrisonUpgrade(thing, moduleData);
}
// FUNCTION: BFME2 0x002502DE size=56 name=?friend_newModuleInstance@CostModifierUpgrade@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *CostModifierUpgrade::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new CostModifierUpgrade(thing, moduleData);
}
// FUNCTION: BFME2 0x00250316 size=84 name=?friend_newModuleData@CostModifierUpgradeModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *CostModifierUpgradeModuleData::friend_newModuleData(INI *ini)
{
	CostModifierUpgradeModuleData *data = new CostModifierUpgradeModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, CostModifierUpgradeModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x0025036A size=56 name=?friend_newModuleInstance@SpellRechargeModifierUpgrade@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *SpellRechargeModifierUpgrade::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new SpellRechargeModifierUpgrade(thing, moduleData);
}
// FUNCTION: BFME2 0x002503A2 size=84 name=?friend_newModuleData@SpellRechargeModifierUpgradeModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *SpellRechargeModifierUpgradeModuleData::friend_newModuleData(INI *ini)
{
	SpellRechargeModifierUpgradeModuleData *data = new SpellRechargeModifierUpgradeModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, SpellRechargeModifierUpgradeModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x002503F6 size=56 name=?friend_newModuleInstance@ExperienceScalarUpgrade@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *ExperienceScalarUpgrade::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new ExperienceScalarUpgrade(thing, moduleData);
}
// FUNCTION: BFME2 0x0025042E size=84 name=?friend_newModuleData@ExperienceScalarUpgradeModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *ExperienceScalarUpgradeModuleData::friend_newModuleData(INI *ini)
{
	ExperienceScalarUpgradeModuleData *data = new ExperienceScalarUpgradeModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, ExperienceScalarUpgradeModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00250482 size=56 name=?friend_newModuleInstance@MaxHealthUpgrade@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *MaxHealthUpgrade::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new MaxHealthUpgrade(thing, moduleData);
}
// FUNCTION: BFME2 0x002504BA size=84 name=?friend_newModuleData@MaxHealthUpgradeModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *MaxHealthUpgradeModuleData::friend_newModuleData(INI *ini)
{
	MaxHealthUpgradeModuleData *data = new MaxHealthUpgradeModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, MaxHealthUpgradeModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x0025050E size=56 name=?friend_newModuleInstance@ModelConditionUpgrade@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *ModelConditionUpgrade::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new ModelConditionUpgrade(thing, moduleData);
}
// FUNCTION: BFME2 0x00250546 size=84 name=?friend_newModuleData@ModelConditionUpgradeModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *ModelConditionUpgradeModuleData::friend_newModuleData(INI *ini)
{
	ModelConditionUpgradeModuleData *data = new ModelConditionUpgradeModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, ModelConditionUpgradeModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x0025059A size=56 name=?friend_newModuleInstance@AttributeModifierUpgrade@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *AttributeModifierUpgrade::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new AttributeModifierUpgrade(thing, moduleData);
}
// FUNCTION: BFME2 0x002505D2 size=56 name=?friend_newModuleInstance@CastleUpgrade@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *CastleUpgrade::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new CastleUpgrade(thing, moduleData);
}
// FUNCTION: BFME2 0x0025060A size=56 name=?friend_newModuleInstance@GeometryUpgrade@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *GeometryUpgrade::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new GeometryUpgrade(thing, moduleData);
}
// FUNCTION: BFME2 0x00250642 size=84 name=?friend_newModuleData@GeometryUpgradeModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *GeometryUpgradeModuleData::friend_newModuleData(INI *ini)
{
	GeometryUpgradeModuleData *data = new GeometryUpgradeModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, GeometryUpgradeModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00250696 size=56 name=?friend_newModuleInstance@ReplaceSelfUpgrade@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *ReplaceSelfUpgrade::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new ReplaceSelfUpgrade(thing, moduleData);
}
// FUNCTION: BFME2 0x002506CE size=84 name=?friend_newModuleData@ReplaceSelfUpgradeModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *ReplaceSelfUpgradeModuleData::friend_newModuleData(INI *ini)
{
	ReplaceSelfUpgradeModuleData *data = new ReplaceSelfUpgradeModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, ReplaceSelfUpgradeModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00250722 size=6 name=?Rva00250722Get@@YAHXZ evidence=?:placeholder tier=A
// No retail code or data refers to this function. It follows the creators of ReplaceSelfUpgrade and returns the bit set 0x8C, which looks like a module interface mask;
// nothing ties it to ReplaceSelfUpgrade, so it keeps its ledger name.
int Rva00250722Get()
{
	return 140;
}
// FUNCTION: BFME2 0x00250728 size=56 name=?friend_newModuleInstance@TooltipUpgrade@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *TooltipUpgrade::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new TooltipUpgrade(thing, moduleData);
}
// FUNCTION: BFME2 0x00250760 size=56 name=?friend_newModuleInstance@AudioLoopUpgrade@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *AudioLoopUpgrade::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new AudioLoopUpgrade(thing, moduleData);
}
// FUNCTION: BFME2 0x00250798 size=84 name=?friend_newModuleData@AudioLoopUpgradeModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *AudioLoopUpgradeModuleData::friend_newModuleData(INI *ini)
{
	AudioLoopUpgradeModuleData *data = new AudioLoopUpgradeModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, AudioLoopUpgradeModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x002507EC size=6 name=?Rva002507ECGet@@YAHXZ evidence=?:placeholder tier=A
// No retail code or data refers to this function. It follows the creators of AudioLoopUpgrade and returns the bit set 0x83, which looks like a module interface mask;
// nothing ties it to AudioLoopUpgrade, so it keeps its ledger name.
int Rva002507ECGet()
{
	return 131;
}
// FUNCTION: BFME2 0x002507F2 size=56 name=?friend_newModuleInstance@RemoveUpgradeUpgrade@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *RemoveUpgradeUpgrade::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new RemoveUpgradeUpgrade(thing, moduleData);
}
// FUNCTION: BFME2 0x0025082A size=84 name=?friend_newModuleData@RemoveUpgradeUpgradeModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *RemoveUpgradeUpgradeModuleData::friend_newModuleData(INI *ini)
{
	RemoveUpgradeUpgradeModuleData *data = new RemoveUpgradeUpgradeModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, RemoveUpgradeUpgradeModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x0025087E size=56 name=?friend_newModuleInstance@BuildableHeroListUpgrade@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *BuildableHeroListUpgrade::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new BuildableHeroListUpgrade(thing, moduleData);
}
// FUNCTION: BFME2 0x002508B6 size=56 name=?friend_newModuleInstance@AllowBannerSpawnUpgrade@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *AllowBannerSpawnUpgrade::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new AllowBannerSpawnUpgrade(thing, moduleData);
}
// FUNCTION: BFME2 0x002508EE size=56 name=?friend_newModuleInstance@CommandPointsUpgrade@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *CommandPointsUpgrade::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new CommandPointsUpgrade(thing, moduleData);
}
// FUNCTION: BFME2 0x00250926 size=56 name=?friend_newModuleInstance@SpawnUnitBehavior@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x0025095E size=56 name=?friend_newModuleInstance@OathbreakersFadeAwayBehavior@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *OathbreakersFadeAwayBehavior::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new OathbreakersFadeAwayBehavior(thing, moduleData);
}
// FUNCTION: BFME2 0x00250996 size=56 name=?friend_newModuleInstance@LockWeaponCreate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *LockWeaponCreate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new LockWeaponCreate(thing, moduleData);
}
// FUNCTION: BFME2 0x002509CE size=81 name=?friend_newModuleData@LockWeaponCreateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *LockWeaponCreateModuleData::friend_newModuleData(INI *ini)
{
	LockWeaponCreateModuleData *data = new LockWeaponCreateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, LockWeaponCreateModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00250A1F size=42 name=??0SpawnedModelConditionCreate@@QAE@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00250A4E size=69 name=?getModuleNameKey@SpawnedModelConditionCreate@@UBE?AW4NameKeyType@@XZ evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00250A93 size=6 name=?name@SpawnedModelConditionCreate@@QBEPBDXZ evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00250A99 size=56 name=?friend_newModuleInstance@SupplyCenterCreate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *SupplyCenterCreate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new SupplyCenterCreate(thing, moduleData);
}
// FUNCTION: BFME2 0x00250AD1 size=56 name=?friend_newModuleInstance@SupplyWarehouseCreate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *SupplyWarehouseCreate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new SupplyWarehouseCreate(thing, moduleData);
}
// FUNCTION: BFME2 0x00250B09 size=56 name=?friend_newModuleInstance@GrantUpgradeCreate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *GrantUpgradeCreate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new GrantUpgradeCreate(thing, moduleData);
}
// FUNCTION: BFME2 0x00250B41 size=81 name=?friend_newModuleData@GrantUpgradeCreateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *GrantUpgradeCreateModuleData::friend_newModuleData(INI *ini)
{
	GrantUpgradeCreateModuleData *data = new GrantUpgradeCreateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, GrantUpgradeCreateModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00250B92 size=56 name=?friend_newModuleInstance@ExperienceLevelCreate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *ExperienceLevelCreate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new ExperienceLevelCreate(thing, moduleData);
}
// FUNCTION: BFME2 0x00250BCA size=81 name=?friend_newModuleData@ExperienceLevelCreateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *ExperienceLevelCreateModuleData::friend_newModuleData(INI *ini)
{
	ExperienceLevelCreateModuleData *data = new ExperienceLevelCreateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, ExperienceLevelCreateModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00250C1B size=56 name=?friend_newModuleInstance@PreorderCreate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *PreorderCreate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new PreorderCreate(thing, moduleData);
}
// FUNCTION: BFME2 0x00250C53 size=56 name=?friend_newModuleInstance@SpecialPowerCreate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *SpecialPowerCreate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new SpecialPowerCreate(thing, moduleData);
}
// FUNCTION: BFME2 0x00250C8B size=56 name=?friend_newModuleInstance@InheritUpgradeCreate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *InheritUpgradeCreate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new InheritUpgradeCreate(thing, moduleData);
}
// FUNCTION: BFME2 0x00250CC3 size=84 name=?friend_newModuleData@InheritUpgradeCreateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *InheritUpgradeCreateModuleData::friend_newModuleData(INI *ini)
{
	InheritUpgradeCreateModuleData *data = new InheritUpgradeCreateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, InheritUpgradeCreateModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00250D17 size=56 name=?friend_newModuleInstance@BoneFXDamage@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *BoneFXDamage::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new BoneFXDamage(thing, moduleData);
}
// FUNCTION: BFME2 0x00250D4F size=59 name=?friend_newModuleInstance@TransitionDamageFX@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *TransitionDamageFX::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new TransitionDamageFX(thing, moduleData);
}
// FUNCTION: BFME2 0x00250D8A size=56 name=?friend_newModuleInstance@ReflectDamage@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *ReflectDamage::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new ReflectDamage(thing, moduleData);
}
// FUNCTION: BFME2 0x00250DC2 size=81 name=?friend_newModuleData@ReflectDamageModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *ReflectDamageModuleData::friend_newModuleData(INI *ini)
{
	ReflectDamageModuleData *data = new ReflectDamageModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, ReflectDamageModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00250E13 size=56 name=?friend_newModuleInstance@EvacuateDamage@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *EvacuateDamage::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new EvacuateDamage(thing, moduleData);
}
// FUNCTION: BFME2 0x00250E4B size=81 name=?friend_newModuleData@EvacuateDamageModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *EvacuateDamageModuleData::friend_newModuleData(INI *ini)
{
	EvacuateDamageModuleData *data = new EvacuateDamageModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, EvacuateDamageModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00250E9C size=56 name=?friend_newModuleInstance@HordeTransportContainDamage@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *HordeTransportContainDamage::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new HordeTransportContainDamage(thing, moduleData);
}
// FUNCTION: BFME2 0x00250ED4 size=81 name=?friend_newModuleData@HordeTransportContainDamageModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *HordeTransportContainDamageModuleData::friend_newModuleData(INI *ini)
{
	HordeTransportContainDamageModuleData *data = new HordeTransportContainDamageModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, HordeTransportContainDamageModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00250F25 size=56 name=?friend_newModuleInstance@CallHelpOnDamage@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *CallHelpOnDamage::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new CallHelpOnDamage(thing, moduleData);
}
// FUNCTION: BFME2 0x00250F5D size=81 name=?friend_newModuleData@CallHelpOnDamageModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *CallHelpOnDamageModuleData::friend_newModuleData(INI *ini)
{
	CallHelpOnDamageModuleData *data = new CallHelpOnDamageModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, CallHelpOnDamageModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00250FAE size=56 name=?friend_newModuleInstance@FireWeaponCollide@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *FireWeaponCollide::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new FireWeaponCollide(thing, moduleData);
}
// FUNCTION: BFME2 0x00250FE6 size=56 name=?friend_newModuleInstance@SquishCollide@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *SquishCollide::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new SquishCollide(thing, moduleData);
}
// FUNCTION: BFME2 0x0025101E size=56 name=?friend_newModuleInstance@AODCrushCollide@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *AODCrushCollide::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new AODCrushCollide(thing, moduleData);
}
// FUNCTION: BFME2 0x00251056 size=81 name=?friend_newModuleData@AODCrushCollideModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *AODCrushCollideModuleData::friend_newModuleData(INI *ini)
{
	AODCrushCollideModuleData *data = new AODCrushCollideModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, AODCrushCollideModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x002510A7 size=56 name=?friend_newModuleInstance@HordeMemberCollide@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *HordeMemberCollide::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new HordeMemberCollide(thing, moduleData);
}
// FUNCTION: BFME2 0x002510DF size=81 name=?friend_newModuleData@CrateCollideModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *CrateCollideModuleData::friend_newModuleData(INI *ini)
{
	CrateCollideModuleData *data = new CrateCollideModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, CrateCollideModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00251130 size=56 name=?friend_newModuleInstance@HealCrateCollide@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *HealCrateCollide::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new HealCrateCollide(thing, moduleData);
}
static const FieldParse s_moneyCrateCollideModuleDataFieldParse[] =
{
	{ "MoneyProvided", INI::parseUnsignedInt, 0, 0x5C },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x00251168 size=27 name=?buildFieldParse@MoneyCrateCollideModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void MoneyCrateCollideModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	CrateCollideModuleData::buildFieldParse(parse);
	parse.add(s_moneyCrateCollideModuleDataFieldParse, 0);
}

// FUNCTION: BFME2 0x00251183 size=56 name=?friend_newModuleInstance@MoneyCrateCollide@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *MoneyCrateCollide::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new MoneyCrateCollide(thing, moduleData);
}
// FUNCTION: BFME2 0x002511BB size=56 name=?friend_newModuleInstance@ShroudCrateCollide@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *ShroudCrateCollide::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new ShroudCrateCollide(thing, moduleData);
}
static const FieldParse s_unitCrateCollideModuleDataFieldParse[] =
{
	{ "UnitCount", INI::parseUnsignedInt, 0, 0x5C },
	{ "UnitName", INI::parseAsciiString, 0, 0x60 },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x002511F3 size=27 name=?buildFieldParse@UnitCrateCollideModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void UnitCrateCollideModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	CrateCollideModuleData::buildFieldParse(parse);
	parse.add(s_unitCrateCollideModuleDataFieldParse, 0);
}

// FUNCTION: BFME2 0x0025120E size=56 name=?friend_newModuleInstance@UnitCrateCollide@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *UnitCrateCollide::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new UnitCrateCollide(thing, moduleData);
}
static const FieldParse s_veterancyCrateCollideModuleDataFieldParse[] =
{
	{ "EffectRange", INI::parseUnsignedInt, 0, 0x5C },
	{ "AddsOwnerVeterancy", INI::parseBool, 0, 0x60 },
	{ "IsPilot", INI::parseBool, 0, 0x61 },
	{ "AffectsUpToLevel", INI::parseInt, 0, 0x64 },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x00251246 size=27 name=?buildFieldParse@VeterancyCrateCollideModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void VeterancyCrateCollideModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	CrateCollideModuleData::buildFieldParse(parse);
	parse.add(s_veterancyCrateCollideModuleDataFieldParse, 0);
}

// FUNCTION: BFME2 0x00251261 size=56 name=?friend_newModuleInstance@VeterancyCrateCollide@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *VeterancyCrateCollide::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new VeterancyCrateCollide(thing, moduleData);
}
static const FieldParse s_salvageCrateCollideModuleDataFieldParse[] =
{
	{ "PorterChance", INI::parsePercentToReal, 0, 0x5C },
	{ "BannerChance", INI::parsePercentToReal, 0, 0x60 },
	{ "LevelUpChance", INI::parsePercentToReal, 0, 0x64 },
	{ "LevelUpRadius", INI::parsePercentToReal, 0, 0x68 },
	{ "ResourceChance", INI::parsePercentToReal, 0, 0x6C },
	{ "Upgrade", INI::parseAsciiString, 0, 0x78 },
	{ "MinResource", INI::parseInt, 0, 0x70 },
	{ "MaxResource", INI::parseInt, 0, 0x74 },
	{ "AllowAIPickup", INI::parseBool, 0, 0x7C },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x00251299 size=27 name=?buildFieldParse@SalvageCrateCollideModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void SalvageCrateCollideModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	CrateCollideModuleData::buildFieldParse(parse);
	parse.add(s_salvageCrateCollideModuleDataFieldParse, 0);
}

// FUNCTION: BFME2 0x002512B4 size=56 name=?friend_newModuleInstance@SalvageCrateCollide@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *SalvageCrateCollide::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new SalvageCrateCollide(thing, moduleData);
}
// FUNCTION: BFME2 0x002512EC size=56 name=?friend_newModuleInstance@CivilianSpawnCollide@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *CivilianSpawnCollide::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new CivilianSpawnCollide(thing, moduleData);
}
// FUNCTION: BFME2 0x00251324 size=81 name=?friend_newModuleData@CivilianSpawnCollideModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *CivilianSpawnCollideModuleData::friend_newModuleData(INI *ini)
{
	CivilianSpawnCollideModuleData *data = new CivilianSpawnCollideModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, CivilianSpawnCollideModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00251375 size=56 name=?friend_newModuleInstance@InactiveBody@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *InactiveBody::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new InactiveBody(thing, moduleData);
}
// FUNCTION: BFME2 0x002513AD size=59 name=?friend_newModuleInstance@ActiveBody@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *ActiveBody::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new ActiveBody(thing, moduleData);
}
// FUNCTION: BFME2 0x002513E8 size=81 name=?friend_newModuleData@ActiveBodyModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *ActiveBodyModuleData::friend_newModuleData(INI *ini)
{
	ActiveBodyModuleData *data = new ActiveBodyModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, ActiveBodyModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00251439 size=59 name=?friend_newModuleInstance@HighlanderBody@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *HighlanderBody::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new HighlanderBody(thing, moduleData);
}
// FUNCTION: BFME2 0x00251474 size=59 name=?friend_newModuleInstance@ImmortalBody@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *ImmortalBody::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new ImmortalBody(thing, moduleData);
}
static const FieldParse s_structureBodyModuleDataFieldParse[] =
{
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x002514AF size=27 name=?buildFieldParse@StructureBodyModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void StructureBodyModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	ActiveBodyModuleData::buildFieldParse(parse);
	parse.add(s_structureBodyModuleDataFieldParse, 0);
}

// FUNCTION: BFME2 0x002514CA size=59 name=?friend_newModuleInstance@StructureBody@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *StructureBody::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new StructureBody(thing, moduleData);
}
static const FieldParse s_symbioticStructuresBodyModuleDataFieldParse[] =
{
	{ "Symbiote", INI::parseAsciiString, 0, 0x64 },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x00251505 size=27 name=?buildFieldParse@SymbioticStructuresBodyModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void SymbioticStructuresBodyModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	ActiveBodyModuleData::buildFieldParse(parse);
	parse.add(s_symbioticStructuresBodyModuleDataFieldParse, 0);
}

// FUNCTION: BFME2 0x00251520 size=59 name=?friend_newModuleInstance@SymbioticStructuresBody@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *SymbioticStructuresBody::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new SymbioticStructuresBody(thing, moduleData);
}
// FUNCTION: BFME2 0x0025155B size=59 name=?friend_newModuleInstance@RespawnBody@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *RespawnBody::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new RespawnBody(thing, moduleData);
}
// FUNCTION: BFME2 0x00251596 size=81 name=?friend_newModuleData@RespawnBodyModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *RespawnBodyModuleData::friend_newModuleData(INI *ini)
{
	RespawnBodyModuleData *data = new RespawnBodyModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, RespawnBodyModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x002515E7 size=59 name=?friend_newModuleInstance@DelayedDeathBody@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *DelayedDeathBody::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new DelayedDeathBody(thing, moduleData);
}
// FUNCTION: BFME2 0x00251622 size=84 name=?friend_newModuleData@DelayedDeathBodyModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *DelayedDeathBodyModuleData::friend_newModuleData(INI *ini)
{
	DelayedDeathBodyModuleData *data = new DelayedDeathBodyModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, DelayedDeathBodyModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00251676 size=59 name=?friend_newModuleInstance@FreeLifeBody@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *FreeLifeBody::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new FreeLifeBody(thing, moduleData);
}
// FUNCTION: BFME2 0x002516B1 size=84 name=?friend_newModuleData@FreeLifeBodyModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *FreeLifeBodyModuleData::friend_newModuleData(INI *ini)
{
	FreeLifeBodyModuleData *data = new FreeLifeBodyModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, FreeLifeBodyModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00251705 size=59 name=?friend_newModuleInstance@DetachableRiderBody@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *DetachableRiderBody::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new DetachableRiderBody(thing, moduleData);
}
// FUNCTION: BFME2 0x00251740 size=84 name=?friend_newModuleData@DetachableRiderBodyModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *DetachableRiderBodyModuleData::friend_newModuleData(INI *ini)
{
	DetachableRiderBodyModuleData *data = new DetachableRiderBodyModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, DetachableRiderBodyModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00251794 size=59 name=?friend_newModuleInstance@OathbreakerBody@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *OathbreakerBody::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new OathbreakerBody(thing, moduleData);
}
// FUNCTION: BFME2 0x002517CF size=59 name=?friend_newModuleInstance@PorcupineFormationBodyModule@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *PorcupineFormationBodyModule::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new PorcupineFormationBodyModule(thing, moduleData);
}
// FUNCTION: BFME2 0x0025180A size=81 name=?friend_newModuleData@PorcupineFormationBodyModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *PorcupineFormationBodyModuleData::friend_newModuleData(INI *ini)
{
	PorcupineFormationBodyModuleData *data = new PorcupineFormationBodyModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, PorcupineFormationBodyModuleData::buildFieldParse);
	return data;
}

// FUNCTION: BFME2 0x0025185B size=56 name=?friend_newModuleInstance@HeroDie@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *HeroDie::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new HeroDie(thing, moduleData);
}
// FUNCTION: BFME2 0x00251893 size=56 name=?friend_newModuleInstance@InvisibilitySpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *InvisibilitySpecialPower::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new InvisibilitySpecialPower(thing, moduleData);
}
// FUNCTION: BFME2 0x002518CB size=84 name=?friend_newModuleData@InvisibilitySpecialPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *InvisibilitySpecialPowerModuleData::friend_newModuleData(INI *ini)
{
	InvisibilitySpecialPowerModuleData *data = new InvisibilitySpecialPowerModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, InvisibilitySpecialPowerModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x0025191F size=56 name=?friend_newModuleInstance@CashHackSpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00251957 size=84 name=?friend_newModuleData@CashHackSpecialPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *CashHackSpecialPowerModuleData::friend_newModuleData(INI *ini)
{
	CashHackSpecialPowerModuleData *data = new CashHackSpecialPowerModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, CashHackSpecialPowerModuleData::buildFieldParse);
	return data;
}

// FUNCTION: BFME2 0x002519AB size=56 name=?friend_newModuleInstance@DefectorSpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *DefectorSpecialPower::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new DefectorSpecialPower(thing, moduleData);
}
// FUNCTION: BFME2 0x002519E3 size=84 name=?friend_newModuleData@DefectorSpecialPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *DefectorSpecialPowerModuleData::friend_newModuleData(INI *ini)
{
	DefectorSpecialPowerModuleData *data = new DefectorSpecialPowerModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, DefectorSpecialPowerModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00251A37 size=59 name=?friend_newModuleInstance@LevelGrantSpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *LevelGrantSpecialPower::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new LevelGrantSpecialPower(thing, moduleData);
}
// FUNCTION: BFME2 0x00251A72 size=84 name=?friend_newModuleData@LevelGrantSpecialPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *LevelGrantSpecialPowerModuleData::friend_newModuleData(INI *ini)
{
	LevelGrantSpecialPowerModuleData *data = new LevelGrantSpecialPowerModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, LevelGrantSpecialPowerModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00251AC6 size=56 name=?friend_newModuleInstance@ProductionSpeedBonus@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *ProductionSpeedBonus::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new ProductionSpeedBonus(thing, moduleData);
}
// FUNCTION: BFME2 0x00251AFE size=84 name=?friend_newModuleData@ProductionSpeedBonusModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *ProductionSpeedBonusModuleData::friend_newModuleData(INI *ini)
{
	ProductionSpeedBonusModuleData *data = new ProductionSpeedBonusModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, ProductionSpeedBonusModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00251B52 size=56 name=?friend_newModuleInstance@OCLSpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *OCLSpecialPower::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new OCLSpecialPower(thing, moduleData);
}
// FUNCTION: BFME2 0x00251B8A size=84 name=?friend_newModuleData@OCLSpecialPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *OCLSpecialPowerModuleData::friend_newModuleData(INI *ini)
{
	OCLSpecialPowerModuleData *data = new OCLSpecialPowerModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, OCLSpecialPowerModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00251C08 size=9 name=?rva00251C08@Rva00251C08@@QAE_NXZ evidence=?:placeholder tier=A
// FUNCTION: BFME2 0x00251C11 size=19 name=?slot43@SpecialPowerModule@@UAEXM@Z evidence=N:ledger tier=A
void SpecialPowerModule::slot43(float readyFraction)
{
	startPowerRecharge(readyFraction);
}

// FUNCTION: BFME2 0x00251C24 size=8 name=?slot44@SpecialPowerModule@@UAEXXZ evidence=N:ledger tier=A
void SpecialPowerModule::slot44()
{
	rva00493805();
}

// FUNCTION: BFME2 0x00251C2C size=5 name=?stopSpecialPower@SpecialPowerModule@@UAEXXZ evidence=X:map request (link (from standalone link 0cf3e833)) tier=A
// FUNCTION: BFME2 0x00251C31 size=69 name=?getModuleNameKey@SpecialPowerTimerRefreshSpecialPower@@UBE?AW4NameKeyType@@XZ evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00251C76 size=6 name=?name@Rva00251C76Named@@QBEPBDXZ evidence=?:placeholder tier=A
// FUNCTION: BFME2 0x00251C7C size=56 name=?friend_newModuleInstance@ElvenWoodSpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *ElvenWoodSpecialPower::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new ElvenWoodSpecialPower(thing, moduleData);
}
// FUNCTION: BFME2 0x00251CB4 size=84 name=?friend_newModuleData@ElvenWoodSpecialPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *ElvenWoodSpecialPowerModuleData::friend_newModuleData(INI *ini)
{
	ElvenWoodSpecialPowerModuleData *data = new ElvenWoodSpecialPowerModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, ElvenWoodSpecialPowerModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00251D08 size=56 name=?friend_newModuleInstance@WeaponChangeSpecialPowerModule@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *WeaponChangeSpecialPowerModule::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new WeaponChangeSpecialPowerModule(thing, moduleData);
}
// FUNCTION: BFME2 0x00251D40 size=84 name=?friend_newModuleData@WeaponChangeSpecialPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *WeaponChangeSpecialPowerModuleData::friend_newModuleData(INI *ini)
{
	WeaponChangeSpecialPowerModuleData *data = new WeaponChangeSpecialPowerModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, WeaponChangeSpecialPowerModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00251D94 size=56 name=?friend_newModuleInstance@ScavengerSpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *ScavengerSpecialPower::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new ScavengerSpecialPower(thing, moduleData);
}
// FUNCTION: BFME2 0x00251DCC size=84 name=?friend_newModuleData@ScavengerSpecialPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *ScavengerSpecialPowerModuleData::friend_newModuleData(INI *ini)
{
	ScavengerSpecialPowerModuleData *data = new ScavengerSpecialPowerModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, ScavengerSpecialPowerModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00251E20 size=56 name=?friend_newModuleInstance@CloudBreakSpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *CloudBreakSpecialPower::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new CloudBreakSpecialPower(thing, moduleData);
}
// FUNCTION: BFME2 0x00251E58 size=84 name=?friend_newModuleData@CloudBreakSpecialPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *CloudBreakSpecialPowerModuleData::friend_newModuleData(INI *ini)
{
	CloudBreakSpecialPowerModuleData *data = new CloudBreakSpecialPowerModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, CloudBreakSpecialPowerModuleData::buildFieldParse);
	return data;
}

// FUNCTION: BFME2 0x00251EAC size=56 name=?friend_newModuleInstance@TaintSpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *TaintSpecialPower::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new TaintSpecialPower(thing, moduleData);
}
// FUNCTION: BFME2 0x00251EE4 size=84 name=?friend_newModuleData@TaintSpecialPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *TaintSpecialPowerModuleData::friend_newModuleData(INI *ini)
{
	TaintSpecialPowerModuleData *data = new TaintSpecialPowerModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, TaintSpecialPowerModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00251F38 size=56 name=?friend_newModuleInstance@FreezingRainSpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *FreezingRainSpecialPower::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new FreezingRainSpecialPower(thing, moduleData);
}
// FUNCTION: BFME2 0x00251F70 size=84 name=?friend_newModuleData@FreezingRainSpecialPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *FreezingRainSpecialPowerModuleData::friend_newModuleData(INI *ini)
{
	FreezingRainSpecialPowerModuleData *data = new FreezingRainSpecialPowerModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, FreezingRainSpecialPowerModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00251FC4 size=56 name=?friend_newModuleInstance@DarknessSpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *DarknessSpecialPower::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new DarknessSpecialPower(thing, moduleData);
}
// FUNCTION: BFME2 0x00251FFC size=84 name=?friend_newModuleData@DarknessSpecialPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *DarknessSpecialPowerModuleData::friend_newModuleData(INI *ini)
{
	DarknessSpecialPowerModuleData *data = new DarknessSpecialPowerModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, DarknessSpecialPowerModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00252050 size=56 name=?friend_newModuleInstance@GrabPassengerSpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *GrabPassengerSpecialPower::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new GrabPassengerSpecialPower(thing, moduleData);
}
// FUNCTION: BFME2 0x00252088 size=84 name=?friend_newModuleData@GrabPassengerSpecialPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *GrabPassengerSpecialPowerModuleData::friend_newModuleData(INI *ini)
{
	GrabPassengerSpecialPowerModuleData *data = new GrabPassengerSpecialPowerModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, GrabPassengerSpecialPowerModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x002520DC size=56 name=?friend_newModuleInstance@DeflectSpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00252114 size=81 name=?friend_newModuleData@DeflectSpecialPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00252165 size=6 name=?Rva00252165Get@@YAHXZ evidence=?:placeholder tier=A
// No retail code or data refers to this function. It follows the creators of DeflectSpecialPower and returns the bit set 0x111, which looks like a module interface mask;
// nothing ties it to DeflectSpecialPower, so it keeps its ledger name.
int Rva00252165Get()
{
	return 273;
}
// FUNCTION: BFME2 0x0025216B size=56 name=?friend_newModuleInstance@SiegeDeploySpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *SiegeDeploySpecialPower::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new SiegeDeploySpecialPower(thing, moduleData);
}
// FUNCTION: BFME2 0x002521A3 size=81 name=?friend_newModuleData@SiegeDeploySpecialPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *SiegeDeploySpecialPowerModuleData::friend_newModuleData(INI *ini)
{
	SiegeDeploySpecialPowerModuleData *data = new SiegeDeploySpecialPowerModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, SiegeDeploySpecialPowerModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x002521F4 size=56 name=?friend_newModuleInstance@SiegeDeployHordeSpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x0025222C size=81 name=?friend_newModuleData@SiegeDeployHordeSpecialPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *SiegeDeployHordeSpecialPowerModuleData::friend_newModuleData(INI *ini)
{
	SiegeDeployHordeSpecialPowerModuleData *data = new SiegeDeployHordeSpecialPowerModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, SiegeDeployHordeSpecialPowerModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x0025227D size=56 name=?friend_newModuleInstance@StopSpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *StopSpecialPower::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new StopSpecialPower(thing, moduleData);
}
// FUNCTION: BFME2 0x002522B5 size=84 name=?friend_newModuleData@StopSpecialPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *StopSpecialPowerModuleData::friend_newModuleData(INI *ini)
{
	StopSpecialPowerModuleData *data = new StopSpecialPowerModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, StopSpecialPowerModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00252309 size=59 name=?friend_newModuleInstance@WoundArrowUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *WoundArrowUpdate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new WoundArrowUpdate(thing, moduleData);
}
// FUNCTION: BFME2 0x00252344 size=84 name=?friend_newModuleData@WoundArrowUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *WoundArrowUpdateModuleData::friend_newModuleData(INI *ini)
{
	WoundArrowUpdateModuleData *data = new WoundArrowUpdateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, WoundArrowUpdateModuleData::buildFieldParse);
	return data;
}
static const FieldParse s_fellBeastSwoopPowerModuleDataFieldParse[] =
{
	{ "SpecialWeapon", INI::parseWeaponTemplate, 0, 0xC8 },
	{ "WhichSpecialWeapon", INI::parseInt, 0, 0xCC },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x00252398 size=27 name=?buildFieldParse@FellBeastSwoopPowerModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void FellBeastSwoopPowerModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	SpecialAbilityUpdateModuleData::buildFieldParse(parse);
	parse.add(s_fellBeastSwoopPowerModuleDataFieldParse, 0);
}

// FUNCTION: BFME2 0x002523B3 size=59 name=?friend_newModuleInstance@FellBeastSwoopPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *FellBeastSwoopPower::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new FellBeastSwoopPower(thing, moduleData);
}
// FUNCTION: BFME2 0x002523EE size=84 name=?friend_newModuleData@FellBeastSwoopPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *FellBeastSwoopPowerModuleData::friend_newModuleData(INI *ini)
{
	FellBeastSwoopPowerModuleData *data = new FellBeastSwoopPowerModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, FellBeastSwoopPowerModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00252442 size=56 name=?friend_newModuleInstance@ManTheWallsSpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *ManTheWallsSpecialPower::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new ManTheWallsSpecialPower(thing, moduleData);
}
// FUNCTION: BFME2 0x0025247A size=56 name=?friend_newModuleInstance@UntamedAllegianceSpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *UntamedAllegianceSpecialPower::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new UntamedAllegianceSpecialPower(thing, moduleData);
}
// FUNCTION: BFME2 0x002524B2 size=81 name=?friend_newModuleData@UntamedAllegianceSpecialPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *UntamedAllegianceSpecialPowerModuleData::friend_newModuleData(INI *ini)
{
	UntamedAllegianceSpecialPowerModuleData *data = new UntamedAllegianceSpecialPowerModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, UntamedAllegianceSpecialPowerModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00252503 size=56 name=?friend_newModuleInstance@PlayerUpgradeSpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *PlayerUpgradeSpecialPower::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new PlayerUpgradeSpecialPower(thing, moduleData);
}
// FUNCTION: BFME2 0x0025253B size=84 name=?friend_newModuleData@PlayerUpgradeSpecialPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *PlayerUpgradeSpecialPowerModuleData::friend_newModuleData(INI *ini)
{
	PlayerUpgradeSpecialPowerModuleData *data = new PlayerUpgradeSpecialPowerModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, PlayerUpgradeSpecialPowerModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x0025258F size=56 name=?friend_newModuleInstance@PlayerHealSpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *PlayerHealSpecialPower::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new PlayerHealSpecialPower(thing, moduleData);
}
// FUNCTION: BFME2 0x002525C7 size=84 name=?friend_newModuleData@PlayerHealSpecialPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *PlayerHealSpecialPowerModuleData::friend_newModuleData(INI *ini)
{
	PlayerHealSpecialPowerModuleData *data = new PlayerHealSpecialPowerModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, PlayerHealSpecialPowerModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x0025261B size=56 name=?friend_newModuleInstance@DevastateSpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *DevastateSpecialPower::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new DevastateSpecialPower(thing, moduleData);
}
// FUNCTION: BFME2 0x00252653 size=84 name=?friend_newModuleData@DevastateSpecialPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *DevastateSpecialPowerModuleData::friend_newModuleData(INI *ini)
{
	DevastateSpecialPowerModuleData *data = new DevastateSpecialPowerModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, DevastateSpecialPowerModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x002526A7 size=56 name=?friend_newModuleInstance@SplitHordeSpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *SplitHordeSpecialPower::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new SplitHordeSpecialPower(thing, moduleData);
}
// FUNCTION: BFME2 0x002526DF size=81 name=?friend_newModuleData@ManTheWallsSpecialPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *ManTheWallsSpecialPowerModuleData::friend_newModuleData(INI *ini)
{
	ManTheWallsSpecialPowerModuleData *data = new ManTheWallsSpecialPowerModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, ManTheWallsSpecialPowerModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00252730 size=56 name=?friend_newModuleInstance@RepairSpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *RepairSpecialPower::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new RepairSpecialPower(thing, moduleData);
}
// FUNCTION: BFME2 0x00252768 size=81 name=?friend_newModuleData@RepairSpecialPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *RepairSpecialPowerModuleData::friend_newModuleData(INI *ini)
{
	RepairSpecialPowerModuleData *data = new RepairSpecialPowerModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, RepairSpecialPowerModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x002527B9 size=56 name=?friend_newModuleInstance@CombineHordeSpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *CombineHordeSpecialPower::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new CombineHordeSpecialPower(thing, moduleData);
}
// FUNCTION: BFME2 0x002527F1 size=84 name=?friend_newModuleData@CombineHordeSpecialPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *CombineHordeSpecialPowerModuleData::friend_newModuleData(INI *ini)
{
	CombineHordeSpecialPowerModuleData *data = new CombineHordeSpecialPowerModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, CombineHordeSpecialPowerModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00252845 size=56 name=?friend_newModuleInstance@HordeDispatchSpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *HordeDispatchSpecialPower::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new HordeDispatchSpecialPower(thing, moduleData);
}
// FUNCTION: BFME2 0x0025287D size=81 name=?friend_newModuleData@HordeDispatchSpecialPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *HordeDispatchSpecialPowerModuleData::friend_newModuleData(INI *ini)
{
	HordeDispatchSpecialPowerModuleData *data = new HordeDispatchSpecialPowerModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, HordeDispatchSpecialPowerModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x002528CE size=56 name=?friend_newModuleInstance@CritterEmitterUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *CritterEmitterUpdate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new CritterEmitterUpdate(thing, moduleData);
}
// FUNCTION: BFME2 0x00252906 size=81 name=?friend_newModuleData@CritterEmitterUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *CritterEmitterUpdateModuleData::friend_newModuleData(INI *ini)
{
	CritterEmitterUpdateModuleData *data = new CritterEmitterUpdateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, CritterEmitterUpdateModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00252957 size=56 name=?friend_newModuleInstance@AnimatedParticleSysBoneClientUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *AnimatedParticleSysBoneClientUpdate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new AnimatedParticleSysBoneClientUpdate(thing, moduleData);
}
// FUNCTION: BFME2 0x0025298F size=56 name=?friend_newModuleInstance@SwayClientUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *SwayClientUpdate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new SwayClientUpdate(thing, moduleData);
}
// FUNCTION: BFME2 0x002529C7 size=56 name=?friend_newModuleInstance@BeaconClientUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *BeaconClientUpdate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new BeaconClientUpdate(thing, moduleData);
}
// FUNCTION: BFME2 0x002529FF size=81 name=?friend_newModuleData@BeaconClientUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *BeaconClientUpdateModuleData::friend_newModuleData(INI *ini)
{
	BeaconClientUpdateModuleData *data = new BeaconClientUpdateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, BeaconClientUpdateModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00252A50 size=56 name=?friend_newModuleInstance@EvaAnnounceClientCreate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00252A88 size=81 name=?friend_newModuleData@EvaAnnounceClientCreateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *EvaAnnounceClientCreateModuleData::friend_newModuleData(INI *ini)
{
	EvaAnnounceClientCreateModuleData *data = new EvaAnnounceClientCreateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, EvaAnnounceClientCreateModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00252AD9 size=56 name=?friend_newModuleInstance@RadarMarkerClientUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *RadarMarkerClientUpdate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new RadarMarkerClientUpdate(thing, moduleData);
}
// FUNCTION: BFME2 0x00252B11 size=81 name=?friend_newModuleData@RadarMarkerClientUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *RadarMarkerClientUpdateModuleData::friend_newModuleData(INI *ini)
{
	RadarMarkerClientUpdateModuleData *data = new RadarMarkerClientUpdateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, RadarMarkerClientUpdateModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00252B62 size=6 name=?Rva00252B62Get@@YAHXZ evidence=?:placeholder tier=A
// No retail code or data refers to this function. It follows the creators of RadarMarkerClientUpdate and returns the bit set 0x1000, which looks like a module interface mask;
// nothing ties it to RadarMarkerClientUpdate, so it keeps its ledger name.
int Rva00252B62Get()
{
	return 4096;
}
// FUNCTION: BFME2 0x00252B84 size=6 name=?GetSnapshotName@DrawableModule@@MBEPBDXZ evidence=N:ledger tier=A
const char *DrawableModule::GetSnapshotName() const
{
	return "DrawableModule";
}
// FUNCTION: BFME2 0x00252B8A size=56 name=?friend_newModuleInstance@AnimationSoundClientBehavior@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00252BC2 size=81 name=?friend_newModuleData@AnimationSoundClientBehaviorModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *AnimationSoundClientBehaviorModuleData::friend_newModuleData(INI *ini)
{
	AnimationSoundClientBehaviorModuleData *data = new AnimationSoundClientBehaviorModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, AnimationSoundClientBehaviorModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00252C13 size=81 name=?friend_newModuleData@ModelConditionSoundSelectorClientBehaviorModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *ModelConditionSoundSelectorClientBehaviorModuleData::friend_newModuleData(INI *ini)
{
	ModelConditionSoundSelectorClientBehaviorModuleData *data = new ModelConditionSoundSelectorClientBehaviorModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, ModelConditionSoundSelectorClientBehaviorModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00252C64 size=81 name=?friend_newModuleData@UpgradeSoundSelectorClientBehaviorModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *UpgradeSoundSelectorClientBehaviorModuleData::friend_newModuleData(INI *ini)
{
	UpgradeSoundSelectorClientBehaviorModuleData *data = new UpgradeSoundSelectorClientBehaviorModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, UpgradeSoundSelectorClientBehaviorModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00252CB5 size=42 name=??0UpgradeSoundSelectorClientBehavior@@QAE@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00252CDF size=69 name=?getModuleNameKey@UpgradeSoundSelectorClientBehavior@@UBE?AW4NameKeyType@@XZ evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00252D24 size=6 name=?name@UpgradeSoundSelectorClientBehavior@@QBEPBDXZ evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00252D2A size=56 name=?friend_newModuleInstance@RandomSoundSelectorClientBehavior@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00252D62 size=56 name=?friend_newModuleInstance@ModelConditionAudioLoopClientBehavior@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00252D9A size=81 name=?friend_newModuleData@ModelConditionAudioLoopClientBehaviorModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *ModelConditionAudioLoopClientBehaviorModuleData::friend_newModuleData(INI *ini)
{
	ModelConditionAudioLoopClientBehaviorModuleData *data = new ModelConditionAudioLoopClientBehaviorModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, ModelConditionAudioLoopClientBehaviorModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00252DEB size=32 name=??0TerrainResourceClientBehavior@@QAE@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00252E0B size=69 name=?getModuleNameKey@TerrainResourceClientBehavior@@UBE?AW4NameKeyType@@XZ evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00252E50 size=6 name=?name@TerrainResourceClientBehavior@@QBEPBDXZ evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00252E72 size=56 name=?friend_newModuleInstance@TemporarilyDefectUpdate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *TemporarilyDefectUpdate::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new TemporarilyDefectUpdate(thing, moduleData);
}
// FUNCTION: BFME2 0x00252EAA size=59 name=?friend_newModuleInstance@DominateEnemySpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *DominateEnemySpecialPower::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new DominateEnemySpecialPower(thing, moduleData);
}
// FUNCTION: BFME2 0x00252EE5 size=84 name=?friend_newModuleData@DominateEnemySpecialPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *DominateEnemySpecialPowerModuleData::friend_newModuleData(INI *ini)
{
	DominateEnemySpecialPowerModuleData *data = new DominateEnemySpecialPowerModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, DominateEnemySpecialPowerModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00252F39 size=59 name=?friend_newModuleInstance@CurseSpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *CurseSpecialPower::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new CurseSpecialPower(thing, moduleData);
}
// FUNCTION: BFME2 0x00252F74 size=84 name=?friend_newModuleData@CurseSpecialPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *CurseSpecialPowerModuleData::friend_newModuleData(INI *ini)
{
	CurseSpecialPowerModuleData *data = new CurseSpecialPowerModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, CurseSpecialPowerModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00252FC8 size=59 name=?friend_newModuleInstance@GiveOrRestoreUpgradeSpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *GiveOrRestoreUpgradeSpecialPower::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new GiveOrRestoreUpgradeSpecialPower(thing, moduleData);
}
// FUNCTION: BFME2 0x00253003 size=84 name=?friend_newModuleData@GiveOrRestoreUpgradeSpecialPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *GiveOrRestoreUpgradeSpecialPowerModuleData::friend_newModuleData(INI *ini)
{
	GiveOrRestoreUpgradeSpecialPowerModuleData *data = new GiveOrRestoreUpgradeSpecialPowerModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, GiveOrRestoreUpgradeSpecialPowerModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00253057 size=59 name=?friend_newModuleInstance@TeleportToCasterSpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *TeleportToCasterSpecialPower::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new TeleportToCasterSpecialPower(thing, moduleData);
}
// FUNCTION: BFME2 0x00253092 size=84 name=?friend_newModuleData@TeleportToCasterSpecialPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *TeleportToCasterSpecialPowerModuleData::friend_newModuleData(INI *ini)
{
	TeleportToCasterSpecialPowerModuleData *data = new TeleportToCasterSpecialPowerModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, TeleportToCasterSpecialPowerModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x002530E6 size=59 name=?friend_newModuleInstance@StoreObjectsSpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00253121 size=84 name=?friend_newModuleData@StoreObjectsSpecialPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *StoreObjectsSpecialPowerModuleData::friend_newModuleData(INI *ini)
{
	StoreObjectsSpecialPowerModuleData *data = new StoreObjectsSpecialPowerModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, StoreObjectsSpecialPowerModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00253175 size=59 name=?friend_newModuleInstance@EvacuateGarrisonSpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *EvacuateGarrisonSpecialPower::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new EvacuateGarrisonSpecialPower(thing, moduleData);
}
// FUNCTION: BFME2 0x002531B0 size=84 name=?friend_newModuleData@EvacuateGarrisonSpecialPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *EvacuateGarrisonSpecialPowerModuleData::friend_newModuleData(INI *ini)
{
	EvacuateGarrisonSpecialPowerModuleData *data = new EvacuateGarrisonSpecialPowerModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, EvacuateGarrisonSpecialPowerModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00253204 size=59 name=?friend_newModuleInstance@ActivateModuleSpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *ActivateModuleSpecialPower::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new ActivateModuleSpecialPower(thing, moduleData);
}
// FUNCTION: BFME2 0x0025323F size=59 name=?friend_newModuleInstance@UnleashSpecialPower@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
Module *UnleashSpecialPower::friend_newModuleInstance(Thing *thing, const ModuleData *moduleData)
{
	return new UnleashSpecialPower(thing, moduleData);
}
// FUNCTION: BFME2 0x0025327A size=84 name=?friend_newModuleData@UnleashSpecialPowerModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *UnleashSpecialPowerModuleData::friend_newModuleData(INI *ini)
{
	UnleashSpecialPowerModuleData *data = new UnleashSpecialPowerModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, UnleashSpecialPowerModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x002532CE size=6 name=?Rva002532CEGet@@YAHXZ evidence=?:placeholder tier=A
// FUNCTION: BFME2 0x0025330B size=29 name=?clear@?$BitFlags@$0CEP@W4ModelConditionFlagType@@@@QAEXI@Z evidence=X:map request tier=A
// FUNCTION: BFME2 0x00253390 size=61 name=??0UpdateModule@@QAE@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x002533CD size=6 name=?GetSnapshotName@UpdateModule@@MBEPBDXZ evidence=N:ledger tier=A
const char *UpdateModule::GetSnapshotName() const
{
	return "UpdateModule";
}
// FUNCTION: BFME2 0x002533FC size=48 name=?ModuleFactoryDataAt002533FC@@YAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x0025342C size=73 name=??0UpgradeMuxData@@QAE@XZ evidence=X:map request (link / hybrid) tier=A
// FUNCTION: BFME2 0x00253475 size=18 name=?rva00253475@@YAXAAVMultiIniFieldParse@@@Z evidence=?:placeholder tier=A
// FUNCTION: BFME2 0x002534B1 size=17 name=?buildFieldParse@Rva0025349C@@SAXAAVMultiIniFieldParse@@@Z evidence=?:placeholder tier=A
// FUNCTION: BFME2 0x002534C2 size=60 name=?ModuleFactoryDataAt002534C2@@YAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x002534FE size=18 name=?buildFieldParse@DestroyDieModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00253510 size=50 name=??0DestroyDieModuleData@@QAE@XZ evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00253542 size=51 name=?buildFieldParse@SpawnBehaviorModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00253575 size=48 name=?ModuleFactoryDataAt00253575@@YAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x002535A5 size=31 name=?setAll@Rva002535A5Mask@@SAXPAX@Z evidence=?:placeholder tier=A
// FUNCTION: BFME2 0x002535C4 size=90 name=?friend_newModuleData@HorseHordeContain@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x0025363A size=72 name=?parseInitialRoster@GarrisonContainModuleData@@SAXPAVINI@@PAX1PBX@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00253682 size=95 name=??0FireWeaponWhenDamagedBehaviorModuleData@@QAE@XZ evidence=N:ledger tier=A
// FUNCTION: BFME2 0x002536E1 size=34 name=?buildFieldParse@FireWeaponWhenDamagedBehaviorModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00253703 size=52 name=?friend_newModuleData@FireWeaponWhenDamagedBehaviorModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *FireWeaponWhenDamagedBehaviorModuleData::friend_newModuleData(INI *ini)
{
	FireWeaponWhenDamagedBehaviorModuleData *data = new FireWeaponWhenDamagedBehaviorModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, FireWeaponWhenDamagedBehaviorModuleData::buildFieldParse);
	return data;
}

// FUNCTION: BFME2 0x00253737 size=111 name=??0FireWeaponWhenDeadBehaviorModuleData@@QAE@XZ evidence=N:ledger tier=A
// FUNCTION: BFME2 0x002537A6 size=54 name=?buildFieldParse@FireWeaponWhenDeadBehaviorModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x002537DC size=84 name=?friend_newModuleData@FireWeaponWhenDeadBehaviorModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *FireWeaponWhenDeadBehaviorModuleData::friend_newModuleData(INI *ini)
{
	FireWeaponWhenDeadBehaviorModuleData *data = new FireWeaponWhenDeadBehaviorModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, FireWeaponWhenDeadBehaviorModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00253830 size=57 name=??0PillageModuleData@@QAE@XZ evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00253869 size=33 name=?buildFieldParse@PillageModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x0025388A size=81 name=?friend_newModuleData@PillageModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *PillageModuleData::friend_newModuleData(INI *ini)
{
	PillageModuleData *data = new PillageModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, PillageModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x002538DB size=67 name=??0RefundDieModuleData@@QAE@XZ evidence=N:ledger tier=A
static const FieldParse s_refundDieModuleDataFieldParse[] =
{
	{ "UpgradeRequired", INI::parseUpgradeTemplate, 0, 0x38 },
	{ "BuildingRequired", iniParseObjectFilter, 0, 0x40 },
	{ "RefundPercent", INI::parsePercentToReal, 0, 0x3C },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x0025391E size=34 name=?buildFieldParse@RefundDieModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void RefundDieModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	parse.add(DieMuxData::getFieldParse(), 0x8);
	parse.add(s_refundDieModuleDataFieldParse, 0);
}

// FUNCTION: BFME2 0x00253940 size=81 name=?friend_newModuleData@RefundDieModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *RefundDieModuleData::friend_newModuleData(INI *ini)
{
	RefundDieModuleData *data = new RefundDieModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, RefundDieModuleData::buildFieldParse);
	return data;
}
static const FieldParse s_createCrateDieModuleDataFieldParse[] =
{
	{ "CrateData", CreateCrateDieModuleData::parseCrateData, 0, 0x0 },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x00253991 size=34 name=?buildFieldParse@CreateCrateDieModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void CreateCrateDieModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	parse.add(DieMuxData::getFieldParse(), 0x8);
	parse.add(s_createCrateDieModuleDataFieldParse, 0);
}

// FUNCTION: BFME2 0x002539B3 size=82 name=??0CrushDieModuleData@@QAE@XZ evidence=N:ledger tier=A
static const FieldParse s_crushDieModuleDataFieldParse[] =
{
	{ "TotalCrushSound", INI::parseAudioEventRTS, 0, 0x38 },
	{ "BackEndCrushSound", INI::parseAudioEventRTS, 0, 0x3C },
	{ "FrontEndCrushSound", INI::parseAudioEventRTS, 0, 0x40 },
	{ "TotalCrushSoundPercent", INI::parseInt, 0, 0x48 },
	{ "BackEndCrushSoundPercent", INI::parseInt, 0, 0x4C },
	{ "FrontEndCrushSoundPercent", INI::parseInt, 0, 0x50 },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x00253A05 size=34 name=?buildFieldParse@CrushDieModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void CrushDieModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	parse.add(DieMuxData::getFieldParse(), 0x8);
	parse.add(s_crushDieModuleDataFieldParse, 0);
}

// FUNCTION: BFME2 0x00253A27 size=81 name=?friend_newModuleData@CrushDieModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *CrushDieModuleData::friend_newModuleData(INI *ini)
{
	CrushDieModuleData *data = new CrushDieModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, CrushDieModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00253A78 size=26 name=??0FXListDieModuleData@@QAE@XZ evidence=N:ledger tier=A
static const FieldParse s_fXListDieModuleDataFieldParse[] =
{
	{ "DeathFX", INI::parseFXList, 0, 0x38 },
	{ "OrientToObject", INI::parseBool, 0, 0x3C },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x00253A92 size=34 name=?buildFieldParse@FXListDieModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void FXListDieModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	parse.add(DieMuxData::getFieldParse(), 0x8);
	parse.add(s_fXListDieModuleDataFieldParse, 0);
}

// FUNCTION: BFME2 0x00253AB4 size=81 name=?friend_newModuleData@FXListDie@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *FXListDie::friend_newModuleData(INI *ini)
{
	FXListDieModuleData *data = new FXListDieModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, FXListDieModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00253B05 size=81 name=?friend_newModuleData@SpecialPowerCompletionDieModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *SpecialPowerCompletionDieModuleData::friend_newModuleData(INI *ini)
{
	SpecialPowerCompletionDieModuleData *data = new SpecialPowerCompletionDieModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, SpecialPowerCompletionDieModuleData::buildFieldParse);
	return data;
}
static const FieldParse s_upgradeDieModuleDataFieldParse[] =
{
	{ "UpgradeToRemove", INI::parseAsciiString, 0, 0x38 },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x00253B56 size=34 name=?buildFieldParse@UpgradeDieModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void UpgradeDieModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	parse.add(DieMuxData::getFieldParse(), 0x8);
	parse.add(s_upgradeDieModuleDataFieldParse, 0);
}

// FUNCTION: BFME2 0x00253B78 size=29 name=??0KeepObjectDieModuleData@@QAE@XZ evidence=N:ledger tier=A
static const FieldParse s_keepObjectDieModuleDataFieldParse[] =
{
	{ "CollapsingTime", INI::parseDurationUnsignedInt, 0, 0x38 },
	{ "StayOnRadar", INI::parseBool, 0, 0x3C },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x00253B95 size=34 name=?buildFieldParse@KeepObjectDieModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void KeepObjectDieModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	parse.add(DieMuxData::getFieldParse(), 0x8);
	parse.add(s_keepObjectDieModuleDataFieldParse, 0);
}

// FUNCTION: BFME2 0x00253BB7 size=81 name=?friend_newModuleData@KeepObjectDieModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *KeepObjectDieModuleData::friend_newModuleData(INI *ini)
{
	KeepObjectDieModuleData *data = new KeepObjectDieModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, KeepObjectDieModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00253C08 size=27 name=??0AssistedTargetingUpdateModuleData@@QAE@XZ evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00253C23 size=68 name=?friend_newModuleData@AssistedTargetingUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
static const FieldParse s_boneFXUpdateModuleDataFieldParse[] =
{
	{ "DamageFXTypes", INI::parseDamageTypeFlags, 0, 0x8 },
	{ "DamageOCLTypes", INI::parseDamageTypeFlags, 0, 0x48C },
	{ "DamageParticleTypes", INI::parseDamageTypeFlags, 0, 0x910 },
	{ "PristineFXList1", BoneFXUpdateModuleData::parseFXList, 0, 0xC },
	{ "PristineFXList2", BoneFXUpdateModuleData::parseFXList, 0, 0x30 },
	{ "PristineFXList3", BoneFXUpdateModuleData::parseFXList, 0, 0x54 },
	{ "PristineFXList4", BoneFXUpdateModuleData::parseFXList, 0, 0x78 },
	{ "PristineFXList5", BoneFXUpdateModuleData::parseFXList, 0, 0x9C },
	{ "PristineFXList6", BoneFXUpdateModuleData::parseFXList, 0, 0xC0 },
	{ "PristineFXList7", BoneFXUpdateModuleData::parseFXList, 0, 0xE4 },
	{ "PristineFXList8", BoneFXUpdateModuleData::parseFXList, 0, 0x108 },
	{ "DamagedFXList1", BoneFXUpdateModuleData::parseFXList, 0, 0x12C },
	{ "DamagedFXList2", BoneFXUpdateModuleData::parseFXList, 0, 0x150 },
	{ "DamagedFXList3", BoneFXUpdateModuleData::parseFXList, 0, 0x174 },
	{ "DamagedFXList4", BoneFXUpdateModuleData::parseFXList, 0, 0x198 },
	{ "DamagedFXList5", BoneFXUpdateModuleData::parseFXList, 0, 0x1BC },
	{ "DamagedFXList6", BoneFXUpdateModuleData::parseFXList, 0, 0x1E0 },
	{ "DamagedFXList7", BoneFXUpdateModuleData::parseFXList, 0, 0x204 },
	{ "DamagedFXList8", BoneFXUpdateModuleData::parseFXList, 0, 0x228 },
	{ "ReallyDamagedFXList1", BoneFXUpdateModuleData::parseFXList, 0, 0x24C },
	{ "ReallyDamagedFXList2", BoneFXUpdateModuleData::parseFXList, 0, 0x270 },
	{ "ReallyDamagedFXList3", BoneFXUpdateModuleData::parseFXList, 0, 0x294 },
	{ "ReallyDamagedFXList4", BoneFXUpdateModuleData::parseFXList, 0, 0x2B8 },
	{ "ReallyDamagedFXList5", BoneFXUpdateModuleData::parseFXList, 0, 0x2DC },
	{ "ReallyDamagedFXList6", BoneFXUpdateModuleData::parseFXList, 0, 0x300 },
	{ "ReallyDamagedFXList7", BoneFXUpdateModuleData::parseFXList, 0, 0x324 },
	{ "ReallyDamagedFXList8", BoneFXUpdateModuleData::parseFXList, 0, 0x348 },
	{ "RubbleFXList1", BoneFXUpdateModuleData::parseFXList, 0, 0x36C },
	{ "RubbleFXList2", BoneFXUpdateModuleData::parseFXList, 0, 0x390 },
	{ "RubbleFXList3", BoneFXUpdateModuleData::parseFXList, 0, 0x3B4 },
	{ "RubbleFXList4", BoneFXUpdateModuleData::parseFXList, 0, 0x3D8 },
	{ "RubbleFXList5", BoneFXUpdateModuleData::parseFXList, 0, 0x3FC },
	{ "RubbleFXList6", BoneFXUpdateModuleData::parseFXList, 0, 0x420 },
	{ "RubbleFXList7", BoneFXUpdateModuleData::parseFXList, 0, 0x444 },
	{ "RubbleFXList8", BoneFXUpdateModuleData::parseFXList, 0, 0x468 },
	{ "PristineOCL1", BoneFXUpdateModuleData::parseObjectCreationList, 0, 0x490 },
	{ "PristineOCL2", BoneFXUpdateModuleData::parseObjectCreationList, 0, 0x4B4 },
	{ "PristineOCL3", BoneFXUpdateModuleData::parseObjectCreationList, 0, 0x4D8 },
	{ "PristineOCL4", BoneFXUpdateModuleData::parseObjectCreationList, 0, 0x4FC },
	{ "PristineOCL5", BoneFXUpdateModuleData::parseObjectCreationList, 0, 0x520 },
	{ "PristineOCL6", BoneFXUpdateModuleData::parseObjectCreationList, 0, 0x544 },
	{ "PristineOCL7", BoneFXUpdateModuleData::parseObjectCreationList, 0, 0x568 },
	{ "PristineOCL8", BoneFXUpdateModuleData::parseObjectCreationList, 0, 0x58C },
	{ "DamagedOCL1", BoneFXUpdateModuleData::parseObjectCreationList, 0, 0x5B0 },
	{ "DamagedOCL2", BoneFXUpdateModuleData::parseObjectCreationList, 0, 0x5D4 },
	{ "DamagedOCL3", BoneFXUpdateModuleData::parseObjectCreationList, 0, 0x5F8 },
	{ "DamagedOCL4", BoneFXUpdateModuleData::parseObjectCreationList, 0, 0x61C },
	{ "DamagedOCL5", BoneFXUpdateModuleData::parseObjectCreationList, 0, 0x640 },
	{ "DamagedOCL6", BoneFXUpdateModuleData::parseObjectCreationList, 0, 0x664 },
	{ "DamagedOCL7", BoneFXUpdateModuleData::parseObjectCreationList, 0, 0x688 },
	{ "DamagedOCL8", BoneFXUpdateModuleData::parseObjectCreationList, 0, 0x6AC },
	{ "ReallyDamagedOCL1", BoneFXUpdateModuleData::parseObjectCreationList, 0, 0x6D0 },
	{ "ReallyDamagedOCL2", BoneFXUpdateModuleData::parseObjectCreationList, 0, 0x6F4 },
	{ "ReallyDamagedOCL3", BoneFXUpdateModuleData::parseObjectCreationList, 0, 0x718 },
	{ "ReallyDamagedOCL4", BoneFXUpdateModuleData::parseObjectCreationList, 0, 0x73C },
	{ "ReallyDamagedOCL5", BoneFXUpdateModuleData::parseObjectCreationList, 0, 0x760 },
	{ "ReallyDamagedOCL6", BoneFXUpdateModuleData::parseObjectCreationList, 0, 0x784 },
	{ "ReallyDamagedOCL7", BoneFXUpdateModuleData::parseObjectCreationList, 0, 0x7A8 },
	{ "ReallyDamagedOCL8", BoneFXUpdateModuleData::parseObjectCreationList, 0, 0x7CC },
	{ "RubbleOCL1", BoneFXUpdateModuleData::parseObjectCreationList, 0, 0x7F0 },
	{ "RubbleOCL2", BoneFXUpdateModuleData::parseObjectCreationList, 0, 0x814 },
	{ "RubbleOCL3", BoneFXUpdateModuleData::parseObjectCreationList, 0, 0x838 },
	{ "RubbleOCL4", BoneFXUpdateModuleData::parseObjectCreationList, 0, 0x85C },
	{ "RubbleOCL5", BoneFXUpdateModuleData::parseObjectCreationList, 0, 0x880 },
	{ "RubbleOCL6", BoneFXUpdateModuleData::parseObjectCreationList, 0, 0x8A4 },
	{ "RubbleOCL7", BoneFXUpdateModuleData::parseObjectCreationList, 0, 0x8C8 },
	{ "RubbleOCL8", BoneFXUpdateModuleData::parseObjectCreationList, 0, 0x8EC },
	{ "PristineParticleSystem1", BoneFXUpdateModuleData::parseParticleSystem, 0, 0x914 },
	{ "PristineParticleSystem2", BoneFXUpdateModuleData::parseParticleSystem, 0, 0x938 },
	{ "PristineParticleSystem3", BoneFXUpdateModuleData::parseParticleSystem, 0, 0x95C },
	{ "PristineParticleSystem4", BoneFXUpdateModuleData::parseParticleSystem, 0, 0x980 },
	{ "PristineParticleSystem5", BoneFXUpdateModuleData::parseParticleSystem, 0, 0x9A4 },
	{ "PristineParticleSystem6", BoneFXUpdateModuleData::parseParticleSystem, 0, 0x9C8 },
	{ "PristineParticleSystem7", BoneFXUpdateModuleData::parseParticleSystem, 0, 0x9EC },
	{ "PristineParticleSystem8", BoneFXUpdateModuleData::parseParticleSystem, 0, 0xA10 },
	{ "DamagedParticleSystem1", BoneFXUpdateModuleData::parseParticleSystem, 0, 0xA34 },
	{ "DamagedParticleSystem2", BoneFXUpdateModuleData::parseParticleSystem, 0, 0xA58 },
	{ "DamagedParticleSystem3", BoneFXUpdateModuleData::parseParticleSystem, 0, 0xA7C },
	{ "DamagedParticleSystem4", BoneFXUpdateModuleData::parseParticleSystem, 0, 0xAA0 },
	{ "DamagedParticleSystem5", BoneFXUpdateModuleData::parseParticleSystem, 0, 0xAC4 },
	{ "DamagedParticleSystem6", BoneFXUpdateModuleData::parseParticleSystem, 0, 0xAE8 },
	{ "DamagedParticleSystem7", BoneFXUpdateModuleData::parseParticleSystem, 0, 0xB0C },
	{ "DamagedParticleSystem8", BoneFXUpdateModuleData::parseParticleSystem, 0, 0xB30 },
	{ "ReallyDamagedParticleSystem1", BoneFXUpdateModuleData::parseParticleSystem, 0, 0xB54 },
	{ "ReallyDamagedParticleSystem2", BoneFXUpdateModuleData::parseParticleSystem, 0, 0xB78 },
	{ "ReallyDamagedParticleSystem3", BoneFXUpdateModuleData::parseParticleSystem, 0, 0xB9C },
	{ "ReallyDamagedParticleSystem4", BoneFXUpdateModuleData::parseParticleSystem, 0, 0xBC0 },
	{ "ReallyDamagedParticleSystem5", BoneFXUpdateModuleData::parseParticleSystem, 0, 0xBE4 },
	{ "ReallyDamagedParticleSystem6", BoneFXUpdateModuleData::parseParticleSystem, 0, 0xC08 },
	{ "ReallyDamagedParticleSystem7", BoneFXUpdateModuleData::parseParticleSystem, 0, 0xC2C },
	{ "ReallyDamagedParticleSystem8", BoneFXUpdateModuleData::parseParticleSystem, 0, 0xC50 },
	{ "RubbleParticleSystem1", BoneFXUpdateModuleData::parseParticleSystem, 0, 0xC74 },
	{ "RubbleParticleSystem2", BoneFXUpdateModuleData::parseParticleSystem, 0, 0xC98 },
	{ "RubbleParticleSystem3", BoneFXUpdateModuleData::parseParticleSystem, 0, 0xCBC },
	{ "RubbleParticleSystem4", BoneFXUpdateModuleData::parseParticleSystem, 0, 0xCE0 },
	{ "RubbleParticleSystem5", BoneFXUpdateModuleData::parseParticleSystem, 0, 0xD04 },
	{ "RubbleParticleSystem6", BoneFXUpdateModuleData::parseParticleSystem, 0, 0xD28 },
	{ "RubbleParticleSystem7", BoneFXUpdateModuleData::parseParticleSystem, 0, 0xD4C },
	{ "RubbleParticleSystem8", BoneFXUpdateModuleData::parseParticleSystem, 0, 0xD70 },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x00253C67 size=17 name=?buildFieldParse@BoneFXUpdateModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void BoneFXUpdateModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	parse.add(s_boneFXUpdateModuleDataFieldParse, 0);
}

// FUNCTION: BFME2 0x00253C78 size=84 name=?friend_newModuleData@BoneFXUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *BoneFXUpdateModuleData::friend_newModuleData(INI *ini)
{
	BoneFXUpdateModuleData *data = new BoneFXUpdateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, BoneFXUpdateModuleData::buildFieldParse);
	return data;
}

static const FieldParse s_defaultProductionExitUpdateModuleDataFieldParse[] =
{
	{ "UnitCreatePoint", INI::parseCoord3D, 0, 0x8 },
	{ "NaturalRallyPoint", INI::parseCoord3D, 0, 0x14 },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x00253CCC size=17 name=?buildFieldParse@DefaultProductionExitUpdateModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void DefaultProductionExitUpdateModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	parse.add(s_defaultProductionExitUpdateModuleDataFieldParse, 0);
}

static const FieldParse s_deletionUpdateModuleDataFieldParse[] =
{
	{ "MinLifetime", INI::parseDurationUnsignedInt, 0, 0x8 },
	{ "MaxLifetime", INI::parseDurationUnsignedInt, 0, 0xC },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x00253CEE size=17 name=?buildFieldParse@DeletionUpdateModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void DeletionUpdateModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	parse.add(s_deletionUpdateModuleDataFieldParse, 0);
}

// FUNCTION: BFME2 0x00253CFF size=56 name=?friend_newModuleData@DeletionUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00253D37 size=81 name=?ModuleFactoryDataAt00253D37@@YAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00253D88 size=42 name=??0HordeNotifyTargetsOfImminentProbableCrushingUpdate@@QAE@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
HordeNotifyTargetsOfImminentProbableCrushingUpdate::HordeNotifyTargetsOfImminentProbableCrushingUpdate(Thing *thing, const ModuleData *moduleData)
	: UpdateModule(thing, moduleData)
{
}
// FUNCTION: BFME2 0x00253DB2 size=69 name=?getModuleNameKey@HordeNotifyTargetsOfImminentProbableCrushingUpdate@@UBE?AW4NameKeyType@@XZ evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00253DF7 size=6 name=?name@Rva00253DF7Named@@QBEPBDXZ evidence=?:placeholder tier=A
// FUNCTION: BFME2 0x00253E64 size=42 name=??0NotifyTargetsOfImminentProbableCrushingUpdate@@QAE@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
NotifyTargetsOfImminentProbableCrushingUpdate::NotifyTargetsOfImminentProbableCrushingUpdate(Thing *thing, const ModuleData *moduleData)
	: UpdateModule(thing, moduleData)
{
}
// FUNCTION: BFME2 0x00253E8E size=69 name=?getModuleNameKey@NotifyTargetsOfImminentProbableCrushingUpdate@@UBE?AW4NameKeyType@@XZ evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00253ED3 size=6 name=?name@Rva00253ED3Named@@QBEPBDXZ evidence=?:placeholder tier=A
static const FieldParse s_gateOpenAndCloseBehaviorModuleDataFieldParse[] =
{
	{ "OpenByDefault", INI::parseBool, 0, 0x8 },
	{ "ResetTimeInMilliseconds", INI::parseDurationUnsignedInt, 0, 0xC },
	{ "PercentOpenForPathing", INI::parseUnsignedInt, 0, 0x10 },
	{ "Proxy", INI::parseAsciiString, 0, 0x14 },
	{ "RepelCollidingUnits", INI::parseBool, 0, 0x18 },
	{ "GeometryForOpen", INI::parseAsciiStringVectorAppend, 0, 0x2C },
	{ "GeometryForClosed", INI::parseAsciiStringVectorAppend, 0, 0x38 },
	{ "SoundOpeningGateLoop", INI::parseAudioEventRTS, 0, 0x1C },
	{ "SoundFinishedOpeningGate", INI::parseAudioEventRTS, 0, 0x20 },
	{ "SoundClosingGateLoop", INI::parseAudioEventRTS, 0, 0x24 },
	{ "SoundFinishedClosingGate", INI::parseAudioEventRTS, 0, 0x28 },
	{ "TimeBeforePlayingOpenSound", INI::parseDurationUnsignedInt, 0, 0x44 },
	{ "TimeBeforePlayingClosedSound", INI::parseDurationUnsignedInt, 0, 0x48 },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x00253F0E size=17 name=?buildFieldParse@GateOpenAndCloseBehaviorModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void GateOpenAndCloseBehaviorModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	parse.add(s_gateOpenAndCloseBehaviorModuleDataFieldParse, 0);
}

// FUNCTION: BFME2 0x00253F1F size=81 name=?friend_newModuleData@GateOpenAndCloseBehaviorModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *GateOpenAndCloseBehaviorModuleData::friend_newModuleData(INI *ini)
{
	GateOpenAndCloseBehaviorModuleData *data = new GateOpenAndCloseBehaviorModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, GateOpenAndCloseBehaviorModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00253F70 size=65 name=?friend_newModuleInstance@GateProxyBehavior@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00253FB1 size=87 name=?friend_newModuleData@GateProxyBehaviorModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x0025404A size=65 name=??0SpecialEnemySenseUpdateModuleData@@QAE@XZ evidence=N:ledger tier=A
static const FieldParse s_specialEnemySenseUpdateModuleDataFieldParse[] =
{
	{ "SpecialEnemyFilter", iniParseObjectFilter, 0, 0x8 },
	{ "ScanRange", INI::parseReal, 0, 0xC },
	{ "ScanInterval", INI::parseDurationUnsignedInt, 0, 0x10 },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x0025408B size=17 name=?buildFieldParse@SpecialEnemySenseUpdateModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void SpecialEnemySenseUpdateModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	parse.add(s_specialEnemySenseUpdateModuleDataFieldParse, 0);
}

// FUNCTION: BFME2 0x0025409C size=81 name=?friend_newModuleData@SpecialEnemySenseUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *SpecialEnemySenseUpdateModuleData::friend_newModuleData(INI *ini)
{
	SpecialEnemySenseUpdateModuleData *data = new SpecialEnemySenseUpdateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, SpecialEnemySenseUpdateModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x002540ED size=84 name=??0AutoDepositUpdateModuleData@@QAE@XZ evidence=N:ledger tier=A
static const FieldParse s_autoDepositUpdateModuleDataFieldParse[] =
{
	{ "DepositTiming", INI::parseDurationUnsignedInt, 0, 0x8 },
	{ "Upgrade", INI::parseUpgradeTemplate, 0, 0x14 },
	{ "DepositAmount", INI::parseInt, 0, 0xC },
	{ "InitialCaptureBonus", INI::parseInt, 0, 0x10 },
	{ "UpgradeBonusPercent", INI::parsePercentToReal, 0, 0x18 },
	{ "UpgradeMustBePresent", iniParseObjectFilter, 0, 0x1C },
	{ "GiveNoXP", INI::parseBool, 0, 0x20 },
	{ "OnlyWhenGarrisoned", INI::parseBool, 0, 0x21 },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x00254141 size=17 name=?buildFieldParse@AutoDepositUpdateModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void AutoDepositUpdateModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	parse.add(s_autoDepositUpdateModuleDataFieldParse, 0);
}

// FUNCTION: BFME2 0x00254152 size=81 name=?friend_newModuleData@AutoDepositUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *AutoDepositUpdateModuleData::friend_newModuleData(INI *ini)
{
	AutoDepositUpdateModuleData *data = new AutoDepositUpdateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, AutoDepositUpdateModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x002541A3 size=87 name=?friend_newModuleData@HordeAIUpdate@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x002541FA size=68 name=??0QueueProductionExitUpdateModuleData@@QAE@XZ evidence=N:ledger tier=A
static const FieldParse s_queueProductionExitUpdateModuleDataFieldParse[] =
{
	{ "UnitCreatePoint", INI::parseCoord3D, 0, 0x8 },
	{ "PlacementViewAngle", INI::parseAngleReal, 0, 0x2C },
	{ "NaturalRallyPoint", INI::parseCoord3D, 0, 0x14 },
	{ "ExitDelay", INI::parseDurationUnsignedInt, 0, 0x20 },
	{ "AllowAirborneCreation", INI::parseBool, 0, 0x24 },
	{ "InitialBurst", INI::parseUnsignedInt, 0, 0x28 },
	{ "NoExitPath", INI::parseBool, 0, 0x30 },
	{ "CanRallyToSlaughter", INI::parseBool, 0, 0x31 },
	{ "UseReturnToFormation", INI::parseBool, 0, 0x32 },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x0025423E size=17 name=?buildFieldParse@QueueProductionExitUpdateModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void QueueProductionExitUpdateModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	parse.add(s_queueProductionExitUpdateModuleDataFieldParse, 0);
}

// FUNCTION: BFME2 0x0025424F size=49 name=?friend_newModuleData@QueueProductionExitUpdate@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
static const FieldParse s_radarUpdateModuleDataFieldParse[] =
{
	{ "RadarExtendTime", INI::parseDurationReal, 0, 0x8 },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x00254280 size=17 name=?buildFieldParse@RadarUpdateModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void RadarUpdateModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	parse.add(s_radarUpdateModuleDataFieldParse, 0);
}

// FUNCTION: BFME2 0x00254291 size=81 name=?friend_newModuleData@RadarUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *RadarUpdateModuleData::friend_newModuleData(INI *ini)
{
	RadarUpdateModuleData *data = new RadarUpdateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, RadarUpdateModuleData::buildFieldParse);
	return data;
}
static const FieldParse s_slavedUpdateModuleDataFieldParse[] =
{
	{ "LeashRange", INI::parseInt, 0, 0x8 },
	{ "GuardMaxRange", INI::parseInt, 0, 0xC },
	{ "GuardWanderRange", INI::parseInt, 0, 0x10 },
	{ "AttackRange", INI::parseInt, 0, 0x14 },
	{ "AttackWanderRange", INI::parseInt, 0, 0x18 },
	{ "ScoutRange", INI::parseInt, 0, 0x1C },
	{ "ScoutWanderRange", INI::parseInt, 0, 0x20 },
	{ "RepairRange", INI::parseInt, 0, 0x28 },
	{ "RepairMinAltitude", INI::parseReal, 0, 0x2C },
	{ "RepairMaxAltitude", INI::parseReal, 0, 0x30 },
	{ "DistToTargetToGrantRangeBonus", INI::parseInt, 0, 0x24 },
	{ "RepairRatePerSecond", INI::parseReal, 0, 0x34 },
	{ "RepairWhenBelowHealth%", INI::parseInt, 0, 0x38 },
	{ "RepairMinReadyTime", INI::parseDurationUnsignedInt, 0, 0x3C },
	{ "RepairMaxReadyTime", INI::parseDurationUnsignedInt, 0, 0x40 },
	{ "RepairMinWeldTime", INI::parseDurationUnsignedInt, 0, 0x44 },
	{ "RepairMaxWeldTime", INI::parseDurationUnsignedInt, 0, 0x48 },
	{ "RepairWeldingSys", INI::parseAsciiString, 0, 0x4C },
	{ "RepairWeldingFXBone", INI::parseAsciiString, 0, 0x50 },
	{ "StayOnSameLayerAsMaster", INI::parseBool, 0, 0x54 },
	{ "UseSlaverAsControlForEvaObjectSightedEvents", INI::parseBool, 0, 0x6D },
	{ "DieOnMastersDeath", INI::parseBool, 0, 0x55 },
	{ "GuardPositionOffset", INI::parseCoord3D, 0, 0x58 },
	{ "FadeOutRange", INI::parseInt, 0, 0x64 },
	{ "FadeTime", INI::parseDurationUnsignedInt, 0, 0x68 },
	{ "MarkUnselectable", INI::parseBool, 0, 0x6C },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x002542E2 size=17 name=?buildFieldParse@SlavedUpdateModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void SlavedUpdateModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	parse.add(s_slavedUpdateModuleDataFieldParse, 0);
}

static const FieldParse s_spawnPointProductionExitUpdateModuleDataFieldParse[] =
{
	{ "SpawnPointBoneName", INI::parseAsciiString, 0, 0x8 },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x0025436A size=17 name=?buildFieldParse@SpawnPointProductionExitUpdateModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void SpawnPointProductionExitUpdateModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	parse.add(s_spawnPointProductionExitUpdateModuleDataFieldParse, 0);
}

static const FieldParse s_hijackerUpdateModuleDataFieldParse[] =
{
	{ "AttachToTargetBone", INI::parseAsciiString, 0, 0x8 },
	{ "ParachuteName", INI::parseAsciiString, 0, 0xC },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x0025437B size=17 name=?buildFieldParse@HijackerUpdateModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void HijackerUpdateModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	parse.add(s_hijackerUpdateModuleDataFieldParse, 0);
}

static const FieldParse s_fadeAndDieOrnamentUpdateModuleDataFieldParse[] =
{
	{ "AttachToTargetBone", INI::parseAsciiString, 0, 0x8 },
	{ "Envelope", INI::Rva00338E52_ParseEnvelope, 0, 0xC },
	{ "FollowTarget", INI::Rva00338E52_ParseEnvelope, 0, 0x34 },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x0025438C size=17 name=?buildFieldParse@FadeAndDieOrnamentUpdateModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
void FadeAndDieOrnamentUpdateModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	parse.add(s_fadeAndDieOrnamentUpdateModuleDataFieldParse, 0);
}

// FUNCTION: BFME2 0x0025439D size=42 name=??0DefaultProductionExitUpdateModuleData@@QAE@XZ evidence=N:ledger tier=A
// FUNCTION: BFME2 0x002543C7 size=81 name=?friend_newModuleData@DefaultProductionExitUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00254434 size=101 name=??0WorkerAIUpdateModuleData@@QAE@XZ evidence=N:ledger tier=A
// FUNCTION: BFME2 0x002544F1 size=84 name=?friend_newModuleData@WorkerAIUpdateModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *WorkerAIUpdateModuleData::friend_newModuleData(INI *ini)
{
	WorkerAIUpdateModuleData *data = new WorkerAIUpdateModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, WorkerAIUpdateModuleData::buildFieldParse);
	return data;
}
static const FieldParse s_rva00254545FieldParse[] =
{
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x00254545 size=17 name=?rva00254545@@YAXAAVMultiIniFieldParse@@@Z evidence=?:placeholder tier=A
void rva00254545(MultiIniFieldParse &parse)
{
	parse.add(s_rva00254545FieldParse, 0);
}

// FUNCTION: BFME2 0x00254556 size=42 name=??0ArmorUpgradeModuleData@@QAE@XZ evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00254580 size=91 name=?buildFieldParse@ArmorUpgradeModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x002545DB size=52 name=?friend_newModuleData@ArmorUpgradeModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00254628 size=34 name=?buildFieldParse@ModuleFactory@@SAXAAVMultiIniFieldParse@@@Z evidence=P:provisional (wb-file) tier=A
// FUNCTION: BFME2 0x0025464A size=52 name=?ModuleFactoryDataAt0025464A@@YAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x0025467E size=32 name=??0LevelUpUpgradeModuleData@@QAE@XZ evidence=N:ledger tier=A
// FUNCTION: BFME2 0x0025469E size=52 name=?friend_newModuleData@LevelUpUpgradeModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *LevelUpUpgradeModuleData::friend_newModuleData(INI *ini)
{
	LevelUpUpgradeModuleData *data = new LevelUpUpgradeModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, LevelUpUpgradeModuleData::buildFieldParse);
	return data;
}

// FUNCTION: BFME2 0x002546D2 size=25 name=??0RadarUpgradeModuleData@@QAE@XZ evidence=N:ledger tier=A
// FUNCTION: BFME2 0x002546EB size=34 name=?buildFieldParse@Rva002546EBModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=?:placeholder tier=A
// FUNCTION: BFME2 0x0025470D size=52 name=?ModuleFactoryDataAt0025470D@@YAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00254741 size=52 name=?friend_newModuleData@RadarUpgradeModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00254775 size=40 name=??0StatusBitsUpgradeModuleData@@QAE@XZ evidence=N:ledger tier=A
// FUNCTION: BFME2 0x0025479D size=52 name=?friend_newModuleData@StatusBitsUpgradeModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *StatusBitsUpgradeModuleData::friend_newModuleData(INI *ini)
{
	StatusBitsUpgradeModuleData *data = new StatusBitsUpgradeModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, StatusBitsUpgradeModuleData::buildFieldParse);
	return data;
}

// FUNCTION: BFME2 0x0025480B size=58 name=?ModuleFactoryDataAt0025480B@@YAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00254857 size=58 name=?ModuleFactoryDataAt00254857@@YAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00254891 size=65 name=??0CommandPointsUpgradeModuleData@@QAE@XZ evidence=N:ledger tier=A
// FUNCTION: BFME2 0x002548D2 size=84 name=?friend_newModuleData@CommandPointsUpgradeModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
ModuleData *CommandPointsUpgradeModuleData::friend_newModuleData(INI *ini)
{
	CommandPointsUpgradeModuleData *data = new CommandPointsUpgradeModuleData;
	if (ini)
		ini->initFromINIMultiProc(data, CommandPointsUpgradeModuleData::buildFieldParse);
	return data;
}
// FUNCTION: BFME2 0x00254926 size=16 name=??0OathbreakersFadeAwayBehaviorModuleData@@QAE@XZ evidence=N:ledger tier=A
// FUNCTION: BFME2 0x00254936 size=55 name=?friend_newModuleData@OathbreakersFadeAwayBehaviorModuleData@@SAPAVModuleData@@PAVINI@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x0025496D size=56 name=?friend_newModuleInstance@SpawnedModelConditionCreate@@SAPAVModule@@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=A
// FUNCTION: BFME2 0x002549C1 size=33 name=?buildFieldParse@TransitionDamageFXModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=A
