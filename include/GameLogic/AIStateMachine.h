// AIStateMachine.h: SAGE AI state machine support types (Zero Hour AIStateMachine.h names).
// AttackExitConditionsInterface: a one-slot interface held by value inside the
// guard states. Target facts: AITNGuardInnerState's ctor 0x00545D50 stores the
// one-slot table 0x00C6A13C at +0x20 (TunnelNetworkExitConditions::shouldExit
// 0x00545B72, `ret 4`); AIGuardRetaliate's override 0x005453E2 is mangled
// ?shouldExit@GuardRetaliateExitConditions@@UBE_NPBVStateMachine@@@Z.
//
// AIStateMachine (SIZE 0x68: AnimalAIUpdate::makeStateMachine 0x0047EC69 and
// GiantBird's 0x0036819D allocate 0x68). Target facts: ctor 0x00351C48
// (Object *, UnsignedInt name hash) calls StateMachine's, constructs the
// vector<Coord3D> at +0x3C and defines the AI states; table 0x00C14CD0.
// xfer 0x00353667: StateMachine::xfer, the goal path (count, then Coord3Ds
// push_back'd), the goal waypoint by name (+0x48), the squad (+0x4C, new
// Squad 0x1C bytes), the temporary state by ID (+0x50, internalGetState),
// UnsignedInt +0x54, ObjectID +0x58, Coord3D +0x5C. isInBusyState 0x0033FD77
// asks the temporary state's isBusy first. Slots 9 and 10 are factories
// (0x0034BA20, 0x0034145C: new AIGuardMachine). Zero Hour AIStateMachine.h
// is the name donor.
#pragma once
#ifndef BFME2_AISTATEMACHINE_H
#define BFME2_AISTATEMACHINE_H

#include "Common/StateMachine.h"
#include "Common/Vector.h"
#include "Common/AsciiString.h"

class Waypoint;
class Squad;
class Team;
class AIGroup;

class AttackExitConditionsInterface
{
public:
	virtual Bool shouldExit(const StateMachine *machine) const = 0;	// slot 0
};
// SIZE 0x4

// NotifyWeaponFiredInterface (Zero Hour name): the second base of AIAttackState,
// at +0x20. Base table 0x00C078DC is five __purecall slots. AIAttackState's
// table 0x00C13B64: 0 empty (notifyFired), 1 0x003413FD forwards the victim to
// the machine's and the attack machine's setGoalObject (notifyNewVictimChosen;
// the map spells it bfmeGoFAC), 2 the shared one-argument `return true`,
// 3 returns the Bool at state +0x41 (isAttackingObject), 4 0x003F712F returns
// state +0x30 (getOriginalVictimPos; the map spells it slot4). Retail's order
// differs from Zero Hour's.
class NotifyWeaponFiredInterface
{
public:
	virtual void notifyFired() = 0;										// slot 0
	virtual void notifyNewVictimChosen(Object *victim) = 0;				// slot 1
	virtual Bool isWeaponSlotOkToFire(Int wslot) const = 0;				// slot 2
	virtual Bool isAttackingObject() const = 0;							// slot 3
	virtual const Coord3D *getOriginalVictimPos() const = 0;			// slot 4
};
// SIZE 0x4

// AIAttackState (SIZE 0x50). Target facts: ctor 0x0034B0DD (machine, Bool follow,
// Bool attackingObject, Bool forceAttacking, AttackExitConditionsInterface *)
// calls State(machine, 0xE7D2F1FF), stores tables 0x00C13B78 (+0) and
// 0x00C13B64 (+0x20), the exit conditions at +0x28, zero at +0x24/+0x2C/+0x3C/
// +0x44, the Bools at +0x40/+0x41/+0x42, false at +0x48/+0x49, 3 at +0x4C and a
// zero Coord3D at +0x30. Table 0x00C13B78 has 19 slots: State's 17 plus 17
// (0x0034B150, returns +0x42) and 18 (0x003413A1). isAttack 0x0034141F asks the
// attack machine at +0x24. Zero Hour AIStateMachine.h is the name donor.
class AIAttackState : public State, public NotifyWeaponFiredInterface
{
public:
	AIAttackState(StateMachine *machine, Bool follow, Bool attackingObject, Bool forceAttacking,
		AttackExitConditionsInterface *attackParameters);					// 0x0034B0DD
	virtual ~AIAttackState();												// 0x0034F73C (deleting)

	virtual StateReturnType onEnter();										// 0x0034B502
	virtual void onExit(StateExitType status);								// 0x0034B889
	virtual StateReturnType update();										// 0x0034F758
	virtual Bool isAttack() const;											// 0x0034141F
	virtual Bool isGuardIdle() const;										// 0x0034B167
	virtual Bool unknownQuery15() const;									// 0x0034B154
	virtual void unknownNotify16();											// 0x0034143C
	virtual Bool isForceAttacking() const;									// slot 17, 0x0034B150
	virtual Bool rva003413A1();												// slot 18, 0x003413A1

	virtual void notifyFired() {}
	virtual void notifyNewVictimChosen(Object *victim);						// 0x003413FD
	virtual Bool isWeaponSlotOkToFire(Int wslot) const { return true; }	// Slot 2, 0x005CB9FA
	virtual Bool isAttackingObject() const { return m_isAttackingObject; }
	virtual const Coord3D *getOriginalVictimPos() const { return &m_originalVictimPos; }	// 0x003F712F

protected:
	virtual void loadPostProcess();											// 0x00346063
	virtual const char *GetSnapshotName() const;							// 0x0034B172
	virtual void xfer(Xfer *xfer);											// 0x0034B3C0

	void createAttackMachine(Object *victim);								// 0x0034B1E9
	Bool chooseWeapon();													// 0x003413B2 (Zero Hour's name; placeholder row ?rva003413B2@AIAttackState@@)

	StateMachine *m_attackMachine;							// +0x24
	AttackExitConditionsInterface *m_attackParameters;		// +0x28
	Team *m_victimTeam;										// +0x2C, loadPostProcess 0x00346063 stores the goal object's team
	Coord3D m_originalVictimPos;							// +0x30
	AsciiString m_weaponTemplateName;						// +0x3C, loadPostProcess 0x00346063 sets it to the current weapon's template name; the dtor 0x0034B178 releases it
	Bool m_follow;											// +0x40
	Bool m_isAttackingObject;								// +0x41
	Bool m_isForceAttacking;								// +0x42
	ObjectID m_unk0x44;										// +0x44, xfer 0x0034B3C0 passes it to XferObjectID
	Bool m_unk0x48;											// +0x48
	Bool m_unk0x49;											// +0x49
	Int m_unk0x4C;											// +0x4C, 3
};
// SIZE 0x50

// AIInternalMoveToState (SIZE 0x4C). Target facts: ctor 0x0033F279 (machine,
// name hash) calls State's, stores table 0x00C10DE8, zeroes the Coord3Ds at
// +0x20/+0x34, +0x2C, +0x30 and +0x44 and the Bools +0x49..+0x4B, stores 1 at
// +0x40 and true at +0x48. Table 0x00C10DE8 has 18 slots: State's 17 plus
// computePath 0x003441F7; slot 7 0x0033F2D7 clears +0x49. Derived states'
// constructors call it directly (AIGuardRetaliateReturnState 0x005453BB).
// Zero Hour AIStateMachine.h is the name donor.
class AIInternalMoveToState : public State
{
public:
	AIInternalMoveToState(StateMachine *machine, UnsignedInt name);	// 0x0033F279
	virtual ~AIInternalMoveToState();							// 0x0033FF2B

	virtual StateReturnType onEnter();							// 0x0034C146
	virtual void onExit(StateExitType status);					// 0x003473A4
	virtual StateReturnType update();							// 0x00347460
	virtual void clearWaitingForPath() { m_waitingForPath = false; }	// 0x0033F2D7

protected:
	virtual const char *GetSnapshotName() const;				// 0x0033F2F7
	virtual void xfer(Xfer *xfer);								// 0x0033FF76
	virtual Bool computePath();									// slot 17, 0x003441F7
	Bool getAdjustsDestination() const;					// 0x00344138
	void forceRepath();									// 0x0034002A: marks the path goal unknown and the timestamp stale

	Coord3D m_goalPosition;									// +0x20
	Real m_unk0x2C;											// +0x2C
	Int m_goalLayer;										// +0x30
	Coord3D m_pathGoalPosition;								// +0x34
	Int m_ambientPlayingHandle;								// +0x40, 1 (Zero Hour name: an AudioHandle, xfer 0x0033FF76 passes it to TheAudio->xferAudioHandle)
	UnsignedInt m_pathTimestamp;							// +0x44
	Bool m_adjustDestinations;								// +0x48, true
	Bool m_waitingForPath;									// +0x49
	Bool m_tryOneMoreRepath;								// +0x4A
	Bool m_unk0x4B;											// +0x4B
};
// SIZE 0x4C

// AIEnterState (SIZE 0x54). Target facts: ctor 0x00342EAC (map placeholder
// Rva00342EAC) calls AIInternalMoveToState(machine, 0x822AD648), zeroes +0x4C and
// +0x50 and stores table 0x00C12E90; update 0x0035455A; xfer 0x00341930
// (placeholder Rva00341930): version 2, AIInternalMoveToState::xfer, ObjectID
// +0x4C, (v2) UnsignedInt +0x50. AITNGuardReturnState derives from it.
//
// AIMoveToState (SIZE 0x50: AIStateMachine's ctor 0x00351C48 allocates 0x50 for
// state 1). Its ctor 0x0034005D passes 0x6E6FE691 (CRC-32 "AIMoveToState") to
// AIInternalMoveToState's, stores table 0x00C11EB8 and sets the Bool +0x4C; the
// ctors of AIMoveToPositionAndEnterState 0x00346CDD, AIMoveToAndEvacuateState
// 0x00342DF9 and AIMoveForBoarding 0x00343077 call it. Table 0x00C11EB8: slots 2
// and 3 are AIInternalMoveToState's (no name or xfer of its own), 4 onEnter
// 0x0034C7BD, 5 onExit 0x0034BA5F, 6 update 0x00353A65. 0x0034BA5F is a bare jump
// to AIInternalMoveToState::onExit; every class that overrides onExit that way
// shares it (folded), which is why 0x4C-byte states such as AIMoveToStateSA, the
// attack approach states, AIDockMoveToRallyState and AIGuardReturnState show it
// in their tables while deriving AIInternalMoveToState.
class AIMoveToState : public AIInternalMoveToState
{
public:
	AIMoveToState(StateMachine *machine);						// 0x0034005D
	virtual StateReturnType onEnter();							// 0x0034C7BD
	virtual void onExit(StateExitType status);					// 0x0034BA5F
	virtual StateReturnType update();							// 0x00353A65

protected:
	Bool m_unk0x4C;												// +0x4C, the ctor sets it
};
// SIZE 0x50

class AIEnterState : public AIInternalMoveToState
{
public:
	AIEnterState(StateMachine *machine);						// 0x00342EAC
	virtual StateReturnType onEnter();							// 0x0034FB77
	virtual void onExit(StateExitType status);					// 0x003510A5
	virtual StateReturnType update();							// 0x0035455A

protected:
	virtual void xfer(Xfer *xfer);								// 0x00341930

	ObjectID m_entryToClear;									// +0x4C
	UnsignedInt m_unk0x50;										// +0x50, version 2

protected:
	virtual const char *GetSnapshotName() const;			// 0x00342ED1
};
// SIZE 0x54

class AIStateMachine : public StateMachine
{
public:
	AIStateMachine(Object *owner, UnsignedInt name);		// 0x00351C48
	virtual ~AIStateMachine();								// 0x00352B25 (deleting)

	virtual StateReturnType updateStateMachine();			// slot 4, 0x003502CA
	virtual void clear();									// slot 5, 0x00352B41
	virtual StateReturnType resetToDefaultState();			// slot 6, 0x00350392
	virtual StateReturnType setState(StateID newStateID);	// slot 8, 0x003503D0
	virtual StateMachine *makeSubMachine();					// slot 9, 0x0034BA20
	virtual StateMachine *makeGuardMachine();				// slot 10, 0x0034145C
	virtual Bool isInBusyState() const;						// slot 12, 0x0033FD77
	virtual void setGoalObject(const Object *obj);			// slot 14, 0x0033FCDC

	StateReturnType setTemporaryState(StateID newStateID, Int frameLimit);	// 0x0033FCE1
	void rva0035033F();										// 0x0035033F: ends the temporary state (EXIT_RESET) and restores the saved goal
	void addToGoalPath(const Coord3D *pathPoint);			// 0x0035385E
	void setGoalSquad(const Squad *squad);					// 0x00352B9D (creates the goal squad on first use and assigns it, 0x00351761)
	void setGoalTeam(const Team *team);						// 0x0035042D (creates the goal squad on first use; AIUpdateInterface::privateAttackTeam 0x0026BFD9 calls it)
	void setGoalAIGroup(const AIGroup *group);				// 0x00350462 (setGoalTeam's twin: the goal squad from an AI group)
	Int getGoalPathSize() const { return m_goalPath.size(); }
	const Coord3D *getGoalPathPosition(Int i) const;	// 0x00346FA5 (Zero Hour's name; map placeholder ?rva00346FA5@Rva00346FA5@@QBEPAXH@Z): &m_goalPath[i], NULL out of range
	void setGoalPath(const _STL::vector<Coord3D> *path);		// 0x00351759: m_goalPath = *path (out of line; AIUpdateInterface::bfmePrivateCommand24 0x0026BB5A calls it)
	void setGoalWaypoint(const Waypoint *waypoint);			// 0x003E3BFB: stores +0x48 (out of line in retail; AIUpdateInterface::bfmeGoCSG 0x00264A4E and bfmeGoCSH 0x00264ADE call it)
	const Waypoint *getGoalWaypoint() const { return m_goalWaypoint; }	// inline (Zero Hour's): AIWanderState::onEnter 0x0034F1E1 and AIPanicState::onEnter 0x0034F505 read +0x48 in place
	void rva0033FCC8();										// 0x0033FCC8: saves the goal object ID and position into +0x58/+0x5C (AIUpdateInterface::bfmePrivateCommand31 0x00264B86 calls it before setTemporaryState)
	friend class AIUpdateInterface;						// getCurrentStateID 0x00262FC3 reads the temporary state at +0x50 in place

protected:
	virtual const char *GetSnapshotName() const;			// slot 2, 0x00351753
	virtual void xfer(Xfer *xfer);							// slot 3, 0x00353667

	_STL::vector<Coord3D> m_goalPath;						// +0x3C
	const Waypoint *m_goalWaypoint;							// +0x48
	Squad *m_goalSquad;										// +0x4C
	State *m_temporaryState;								// +0x50
	UnsignedInt m_temporaryStateFramEnd;					// +0x54
	ObjectID m_unk0x58;										// +0x58
	Coord3D m_unk0x5C;										// +0x5C
};
// SIZE 0x68

#endif
