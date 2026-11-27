// ClientBehaviorModule.h: BFME 2's base of the client behavior modules, the Drawable's third module list (+0x154).
// It adds no data to DrawableModule (sizeof 0xC). Its table has 19 slots: DrawableModule's 11, then 11-18.
// The inline constructor calls DrawableModule's and stores this class's table; AnimationSound, ModelConditionSoundSelector,
// UpgradeSoundSelector, RandomSoundSelector, ModelConditionAudioLoop and TerrainResource client behaviors derive from it.
// Only slot 11 is declared so far. Slots 12-16 hold the shared `xor eax,eax; ret` in the base (pointer-sized getters whose
// pointee types are not recovered) and slots 17-18 the shared `ret` (TerrainResourceClientBehavior's overrides are the
// ledger's createSelectionFeedback and removeSelectionFeedback); declare them before compiling any table of this class.
#pragma once
#ifndef BFME2_CLIENT_BEHAVIOR_MODULE_H
#define BFME2_CLIENT_BEHAVIOR_MODULE_H

#include "Common/GameType.h"
#include "GameLogic/Module/Module.h"

class Matrix3D;
class Coord3D;
class Thing;
class ModuleData;

class ClientBehaviorModule : public DrawableModule
{
public:
	ClientBehaviorModule(Thing *thing, const ModuleData *moduleData) : DrawableModule(thing, moduleData) {}

	// Slot 11, the shared `ret 0Ch` in the base: Drawable::reactToTransformChange passes its three arguments to
	// every module of the list.
	virtual void reactToTransformChange(const Matrix3D *oldMtx, const Coord3D *oldPos, Real oldAngle) {}
};
// SIZE 0xC
typedef char ClientBehaviorModule_size[(sizeof(ClientBehaviorModule) == 0xC) ? 1 : -1];

#endif
