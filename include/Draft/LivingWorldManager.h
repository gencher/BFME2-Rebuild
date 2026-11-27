// DRAFT: LivingWorldManager, from its constructor and its three retail vtables
// (SubsystemInterface part, Snapshot part at +0xC, LivingWorldObserver part at +0x10).
// Members after the map info are added as the functions that use them are written.
#pragma once
#ifndef BFME2_LIVING_WORLD_MANAGER_H
#define BFME2_LIVING_WORLD_MANAGER_H

#include <stddef.h>
#include <map>
#include "Common/SubsystemInterface.h"
#include "Common/Snapshot.h"
#include "Draft/LivingWorldMapInfo.h"

class INI;
class LivingWorldEyeTower;
class LivingWorldRegionEffectsManager;

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

	bool rva00210DC9();
	void rva00211075();
	int rva002122D4(int key);
	int rva002122FD(const AsciiString &name);

	LivingWorldMapInfo m_mapInfo; // +0x14, filled by parseLivingWorldMapInfoDefinition
	unsigned char m_unk0x204[0x268 - 0x204]; // +0x204 not recovered
	LivingWorldRegionEffectsManager *m_regionEffectsManager; // +0x268
	unsigned char m_unk0x26C[0x2B4 - 0x26C]; // +0x26C not recovered
	_STL::map<int, int> m_keyValues; // +0x2B4, looked up by name key
	bool m_flag2C0; // +0x2C0 cleared by init and reset, set on entering the map
	LivingWorldEyeTower *m_eyeTower; // +0x2C4
};
typedef char LivingWorldManager_off_m_mapInfo[(offsetof(LivingWorldManager, m_mapInfo) == 0x14) ? 1 : -1];
typedef char LivingWorldManager_off_m_regionEffectsManager[(offsetof(LivingWorldManager, m_regionEffectsManager) == 0x268) ? 1 : -1];
typedef char LivingWorldManager_off_m_keyValues[(offsetof(LivingWorldManager, m_keyValues) == 0x2B4) ? 1 : -1];
typedef char LivingWorldManager_off_m_flag2C0[(offsetof(LivingWorldManager, m_flag2C0) == 0x2C0) ? 1 : -1];
typedef char LivingWorldManager_off_m_eyeTower[(offsetof(LivingWorldManager, m_eyeTower) == 0x2C4) ? 1 : -1];

extern LivingWorldManager *TheLivingWorldManager;
extern const FieldParse LivingWorldMapInfoFields[];

void parseLivingWorldMapInfoDefinition(INI *ini);

#endif
