// DRAFT: LivingWorldManager, from its constructor and its three retail vtables
// (SubsystemInterface part, Snapshot part at +0xC, LivingWorldObserver part at +0x10).
// Members after the map info are added as the functions that use them are written.
#pragma once
#ifndef BFME2_LIVING_WORLD_MANAGER_H
#define BFME2_LIVING_WORLD_MANAGER_H

#include <stddef.h>
#include "Common/SubsystemInterface.h"
#include "Common/Snapshot.h"
#include "Draft/LivingWorldMapInfo.h"

class INI;

// Receives War of the Ring events; the base body is empty.
class LivingWorldObserver
{
public:
	virtual void onLivingWorldEvent(int event, int arg1, int arg2, int arg3);
};

class LivingWorldManager : public SubsystemInterface, public Snapshot, public LivingWorldObserver
{
public:
	LivingWorldManager();
	virtual ~LivingWorldManager();

	virtual void init();
	virtual void reset();
	virtual void update();

	LivingWorldMapInfo m_mapInfo; // +0x14, filled by parseLivingWorldMapInfoDefinition
};
typedef char LivingWorldManager_off_m_mapInfo[(offsetof(LivingWorldManager, m_mapInfo) == 0x14) ? 1 : -1];

extern LivingWorldManager *TheLivingWorldManager;
extern const FieldParse LivingWorldMapInfoFields[];

void parseLivingWorldMapInfoDefinition(INI *ini);

#endif
