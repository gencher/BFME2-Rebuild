// Generated from the ledger; map snapshot c591f7146f88c8a8248ebb8ae22f0c978c22aed463db64e05ef57ff0dfae99cc
// object=G004EF5BE name-evidence=invented
// Extents and edges are hypotheses; byte verification alone does not establish original TU boundaries.
#include "GameLogic/TeamInQueue.h"

// FUNCTION: BFME2 0x004EF5BE size=16 name=?go@Rva000C9EF0@@QAEXXZ evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x004EF5CE size=50 name=??0TeamInQueue@@QAE@XZ evidence=N:ledger tier=B hypothesis:mm1-gap-unit
TeamInQueue::TeamInQueue() throw() :
	m_buildPrev(0),
	m_buildNext(0),
	m_readyPrev(0),
	m_readyNext(0),
	m_workOrders(0),
	m_priorityBuild(false),
	m_team(0),
	m_nextTeamInQueue(0),
	m_frameStarted(0),
	m_sentToStartLocation(false),
	m_stopQueueing(false),
	m_reinforcement(false),
	m_reinforcementID(INVALID_ID)
{
}
