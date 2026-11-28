// TeamInQueue.h: one team in AIPlayer's build queue, with the WorkOrders it still has to
// build. Target facts: the destructor 0x004F05F0 stores table 0x00C62B78, walks the
// WorkOrder list at +0x14 deleting its entries, activates its team (Zero Hour's Team::setActive, +0x5D/+0x5E)
// and clears +0x14. The predicates isAllBuilt 0x004F0656, isMinimumBuilt 0x004F06A6,
// areBuildsComplete 0x004F06CD and includesADozer 0x004F10C2 read the list; the
// predicate isBuildTimeExpired 0x004F0681 reads the team's prototype. xfer 0x004F19F6 and GetSnapshotName 0x004EF600 are the Snapshot
// overrides; the destructor ends by storing Snapshot's table 0x00BBB554. Size 0x30.
#pragma once
#ifndef BFME2_TEAMINQUEUE_H
#define BFME2_TEAMINQUEUE_H

#include "Common/GameType.h"
#include "Common/Snapshot.h"

class AIPlayer;
class Team;
class WorkOrder;
class Xfer;

class TeamInQueue : public Snapshot
{
	friend class AIPlayer;
public:
	// Zero Hour's constructor, out of line in BFME 2: AIPlayer::xfer 0x004F0DDE calls it at
	// 0x004EF5CE (table 0x00C62B78, every member cleared in declaration order; no row yet).
	TeamInQueue() throw();
	virtual ~TeamInQueue();							// slot 0, 0x004F05F0 (deleting 0x004EF606)
	Bool isAllBuilt();								// 0x004F0656
	Bool isBuildTimeExpired();						// 0x004F0681
	Bool isMinimumBuilt();							// 0x004F06A6
	Bool areBuildsComplete();						// 0x004F06CD
	Bool includesADozer();							// 0x004F10C2
	// The two intrusive lists the queue links the entry into (AIPlayer keeps the heads).
	// Zero Hour's MAKE_DLINK members, inline in the class as there: retail emits out-of-line
	// copies (0x004F0341, 0x004F035B, 0x004EF342, 0x004EF35C) that its callers call without
	// knowing their register use, as for a COMDAT compiled after them.
	Bool dlink_isInList_TeamBuildQueue(TeamInQueue * const *pListHead) const { return *pListHead == this || m_buildPrev || m_buildNext; }	// 0x004F0341
	void dlink_prependTo_TeamBuildQueue(TeamInQueue **pListHead)		// 0x004F035B (the map's placeholder Rva00160530::set)
	{
		m_buildNext = *pListHead;
		if (*pListHead)
			(*pListHead)->m_buildPrev = this;
		*pListHead = this;
	}
	void dlink_removeFrom_TeamBuildQueue(TeamInQueue **head);			// 0x004F0372
	void dlink_removeFrom_TeamReadyQueue(TeamInQueue **head);			// 0x004F03AF
	Bool dlink_isInList_TeamReadyQueue(TeamInQueue * const *pListHead) const { return *pListHead == this || m_readyPrev || m_readyNext; }	// 0x004EF342
	void dlink_prependTo_TeamReadyQueue(TeamInQueue **pListHead)		// 0x004EF35C (the map's placeholder Rva00160620::set)
	{
		m_readyNext = *pListHead;
		if (*pListHead)
			(*pListHead)->m_readyPrev = this;
		*pListHead = this;
	}
	// Zero Hour's MAKE_DLINK accessors, inline (AIPlayer::reverse_TeamBuildQueue 0x004F0456 and flip
	// 0x004F04C0 swap the links in place).
	TeamInQueue *dlink_next_TeamBuildQueue() const { return m_buildNext; }
	void dlink_swapLinks_TeamBuildQueue()
	{
		TeamInQueue *originalNext = m_buildNext;
		m_buildNext = m_buildPrev;
		m_buildPrev = originalNext;
	}
	TeamInQueue *dlink_next_TeamReadyQueue() const { return m_readyNext; }
	void dlink_swapLinks_TeamReadyQueue()
	{
		TeamInQueue *originalNext = m_readyNext;
		m_readyNext = m_readyPrev;
		m_readyPrev = originalNext;
	}

protected:
	virtual void loadPostProcess() {}				// slot 1, the shared empty body 0x000B3FD0
	virtual char const *GetSnapshotName() const;	// slot 2, 0x004EF600
	virtual void xfer(Xfer *xfer);					// slot 3, 0x004F19F6

	// Zero Hour's members in Zero Hour's order; the constructor 0x004EF5CE writes each at
	// this offset with this width (bytes at +0x18 and +0x28..+0x2A).
	TeamInQueue *m_buildPrev;					// +0x04, build-queue link
	TeamInQueue *m_buildNext;					// +0x08, build-queue link
	TeamInQueue *m_readyPrev;					// +0x0C, ready-queue link
	TeamInQueue *m_readyNext;					// +0x10, ready-queue link
public:
	WorkOrder *m_workOrders;					// +0x14, list of work orders (+0x0C links)
	Bool m_priorityBuild;						// +0x18
	Team *m_team;								// +0x1C, isBuildTimeExpired 0x004F0681 reads its prototype; the destructor activates it
	TeamInQueue *m_nextTeamInQueue;				// +0x20
	Int m_frameStarted;							// +0x24
	Bool m_sentToStartLocation;					// +0x28
	Bool m_stopQueueing;						// +0x29
	Bool m_reinforcement;						// +0x2A
	ObjectID m_reinforcementID;					// +0x2C
};
// SIZE 0x30
typedef char TeamInQueueSize[(sizeof(TeamInQueue) == 0x30) ? 1 : -1];
#endif
