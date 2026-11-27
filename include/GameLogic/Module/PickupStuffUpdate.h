// PickupStuffUpdate.h: the PickupStuffUpdate module, SIZE 0x28 (the module factory allocates that many bytes). It derives from
// UpdateModule, which ends at 0x20; xfer transfers a Bool at +0x20 and an UnsignedInt at +0x24 after the base's data (their
// roles are not recovered). update is the first slot of the table at +0x10, and loadPostProcess is retail's jump to
// UpdateModule's, folded across classes. moveToStuff sends the module's object to another object through its AI.
#pragma once
#ifndef BFME2_PICKUP_STUFF_UPDATE_H
#define BFME2_PICKUP_STUFF_UPDATE_H

#include <stddef.h>
#include "Common/GameType.h"
#include "GameLogic/Module/UpdateModule.h"

class Thing;
class ModuleData;
class Xfer;

// UpdateModule ends at 0x20; this class's members follow it.
typedef char PickupStuffUpdate_base_size[(sizeof(UpdateModule) == 0x20) ? 1 : -1];

class PickupStuffUpdate : public UpdateModule
{
public:
	PickupStuffUpdate(Thing *thing, const ModuleData *moduleData);
	virtual ~PickupStuffUpdate();
	static Module *friend_newModuleInstance(Thing *thing, const ModuleData *moduleData);
	virtual NameKeyType getModuleNameKey() const;
	virtual UpdateSleepTime update();
	// The ledger spells the parameter void *; the body casts it to Object * and starts a temporary state machine that
	// walks the module's own object to it.
	void moveToStuff(void *target);

protected:
	virtual const char *GetSnapshotName() const;
	virtual void xfer(Xfer *xfer);

public:
	Bool m_unk0x20;                                // +0x20; xfer Bool
	UnsignedInt m_unk0x24;                         // +0x24; xfer UnsignedInt
protected:
	virtual void loadPostProcess() { UpdateModule::loadPostProcess(); }	// retail slot 0x00497805: a jmp to UpdateModule's, ICF-folded across classes
};
// SIZE 0x28
typedef char PickupStuffUpdate_size[(sizeof(PickupStuffUpdate) == 0x28) ? 1 : -1];
typedef char PickupStuffUpdate_off_m_unk0x20[(offsetof(PickupStuffUpdate, m_unk0x20) == 0x20) ? 1 : -1];
typedef char PickupStuffUpdate_off_m_unk0x24[(offsetof(PickupStuffUpdate, m_unk0x24) == 0x24) ? 1 : -1];

#endif
