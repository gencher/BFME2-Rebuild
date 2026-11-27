#pragma once

#include <map>
#include <set>
#include <vector>

#include "Common/Snapshot.h"
#include "Common/SubsystemInterface.h"

class LivingWorldArmy;
class LivingWorldPlayer;
class LivingWorldRegion;
class LivingWorldRegionManager;
class RegionAwardDispute;

#pragma pack(push, 1)
// The constructor initializes SubsystemInterface, then Snapshot at +0xC.
class LivingWorldLogic : public SubsystemInterface, public Snapshot
{
public:
	LivingWorldLogic();
	virtual ~LivingWorldLogic();
	virtual void init();
	virtual void reset();
	virtual void update();

	virtual void loadPostProcess();
	virtual const char *GetSnapshotName() const;
	virtual void xfer(Xfer *xfer);

	// Collect unique team keys for either the active players or all players.
	void collectActiveTeams(_STL::set<int> *teams);
	void collectTeams(_STL::set<int> *teams);
	unsigned int getNumActiveTeams();
	unsigned int getNumTeams();
	void ValidatePlayers();
	bool isSelectionLocked() const;
	bool rva0051AEEF() const;
	int rva002B2B66();
	bool rva002B3669();
	bool rva002B4B83();
	LivingWorldPlayer *findPlayer(int id, unsigned int *index);
	LivingWorldPlayer *findPlayer(const AsciiString &name, unsigned int *index);
	LivingWorldPlayer *GetPlayerAtIndex(int index);
	void RemovePlayer(LivingWorldPlayer *player);
	void removePlayerByID(int id);
	LivingWorldRegion *getBattleRegion();
	void updateBattleMarkers();
	void requestEndGame(int arg);
	void hideEndGame();
	LivingWorldRegion *getRegionByID(int id);
	LivingWorldArmy *findPlayerArmy(int id);
	LivingWorldArmy *findCityArmy(int id);
	LivingWorldArmy *findArmy(int id);
	bool hasUnresolvedDisputeFor(const LivingWorldPlayer *player);
	int getNumHumanPlayers(bool includeEliminated);
	void setLocalPlayer(LivingWorldPlayer *player);
	void cycleLocalPlayer();
	bool rva002B5A5F(LivingWorldArmy *army);
	void refreshLocalArmies();
	void destroyArmy(LivingWorldArmy *army);
	void processArmyDestroyList();
	bool IsCurrentTurnPhaseFinished();
	bool rva002B3740();
	int rva002B4DE2();
	int rva002B5C1D();
	bool AdvanceTurnPhase();

	unsigned char m_unk0x10[0x8C - 0x10]; // Includes the observer state and its embedded records.
	_STL::vector<LivingWorldPlayer *> m_players; // +0x8C
	LivingWorldPlayer *m_localPlayer; // +0x98
	unsigned char m_unk0x9C[0xB0 - 0x9C]; // +0x9C not recovered
	LivingWorldRegionManager *m_regionManager; // The constructor allocates this manager.
	bool m_flagB4; // +0xB4
	bool m_flagB5; // +0xB5
	bool m_flagB6; // +0xB6
	unsigned char m_unk0xB7; // +0xB7 not recovered
	int m_battleRegionIndex; // +0xB8
	unsigned char m_unk0xBC[0xF4 - 0xBC]; // +0xBC not recovered
	int m_turnPhase; // +0xF4
	int m_unk0xF8; // +0xF8
	int m_turn; // +0xFC
	unsigned int m_unk0x100; // +0x100
	unsigned int m_unk0x104; // +0x104
	unsigned char m_unk0x108[0x10C - 0x108]; // +0x108 not recovered
	_STL::vector<LivingWorldArmy *> m_armies; // +0x10C
	_STL::vector<LivingWorldArmy *> m_armyDestroyList; // +0x118
	_STL::vector<LivingWorldArmy *> m_cityArmies; // +0x124
	_STL::map<int, RegionAwardDispute *> m_regionAwardDisputes; // +0x130
	unsigned char m_unk0x13C[0x17C - 0x13C];
private:
	void CheckTurnPhaseTransitions();
};
#pragma pack(pop)

typedef char LivingWorldLogicSizeCheck[sizeof(LivingWorldLogic) == 0x17C ? 1 : -1];

extern LivingWorldLogic *TheLivingWorldLogic;
