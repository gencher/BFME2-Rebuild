// HordeContainInterface.h: the horde contain's member-management interface.
//
// Target facts (retail game.dat): HordeContain's constructor 0x0046F543 stores
// the interface's pure table 0x00C448B8 at +0x11C, then HordeContain's own
// 0x00C44C58; HorseHordeContain (0x00C45838) and AODHordeContain (0x00C46930)
// carry their own tables at the same offset. ContainModuleInterface slots 30
// and 31 both point at 0x0046F7B9, which returns this+0xFC (the +0x11C
// subobject) or null; every other contain returns null (0x000D43D0).
// The pure table 0x00C448B8 is a run of 76 __purecall slots (the next word, 0x0050B238,
// starts another table), and HordeContain's 0x00C44C58 fills all 76; slots 17-75 are declared below with
// the map's names and signatures for HordeContain's bodies, neutral hordeSlotNN names where the map
// spells a placeholder type or files a folded body under another class.
// The interface has no base, no data and no virtual destructor (slot 0 of the
// derived table is 0x00472329, a real body). The table has 155 slots; only
// all 76 are declared here.
//
// Names: WorldBuilder (same game, same layout) asserts
// "!(!contain||!contain->getHordeContainInterface())" and "Expected
// HordeContainInterface", which name the class and the contain accessor.
// WorldBuilder's matching table at VA 0x01EC5EE0 puts the functions that
// reference "HordeContain::attackTargetNow", "HordeContain::returnToFormation",
// "HordeContain::_getUnitsIntendedPosition" (AODHordeContain's override:
// "AODHordeContain::getUnitsIntendedPosition"), "HordeContain::assignSpotToUnit",
// "HordeContain::notifyOfMemberDeath", "HordeContain::removeMemberFromHorde"
// and "HordeContain::performReform" in slots 1, 4, 7, 11, 13, 14 and 16.
// PROVISIONAL (no string; named from what HordeContain's body does):
//   5 0x004726DD: returns members to formation (slot 4), then orders every
//     member whose AI is not already busy on the given object to stop.
//   6 0x0046BB38: true when the unit is contained by this horde or has a spot
//     in the ObjectID map at HordeContain +0x170.
//   8 0x0046F7C9: forwards to HordeContain's primary slot 38 (gatherUnitBack)
//     through a virtual call, so it is a separate method, not an override.
// Slots 0, 2, 3, 9, 10 and 12 take the map's rvaXXXXXXXX names (HordeContain's rows are spelled so); 15 is neutral.
#pragma once
#ifndef BFME2_HORDECONTAININTERFACE_H
#define BFME2_HORDECONTAININTERFACE_H

#include "Common/GameType.h"
#include "GameLogic/ContainmentListView.h"

class AsciiString;
class Object;
class Player;
class ThingTemplate;
class UpgradeTemplate;
class Coord3D;
class FXList;
class Matrix3D;
class Team;
class ObjectFilter;
enum CommandSourceType;
// The member lists slots 17, 30 and 67 take (STLport lists); TUs built without STLport see the same
// slot layout with an opaque reference type.
#if defined(_STLPORT_VERSION)
#include <list>
typedef _STL::list<Object *> HordeObjectList;
typedef _STL::list<const Object *> HordeConstObjectList;
#else
struct HordeObjectList;
struct HordeConstObjectList;
#endif

class HordeContainInterface
{
public:
	virtual void rva00472329(const Coord3D *pos, Int arg) = 0;			// 0  0x00472329
	virtual void attackTargetNow(Object *victim, Int arg) = 0;			// 1  0x00471FFA
	virtual void rva00472235() = 0;										// 2  0x00472235
	virtual void rva0046E253() = 0;										// 3  0x0046E253
	virtual void returnToFormation(Bool flag) = 0;						// 4  0x00472790
	virtual void stopMembersAttacking(Object *victim) = 0;				// 5  0x004726DD (provisional)
	virtual Bool hasSpotForUnit(const Object *unit) = 0;				// 6  0x0046BB38 (provisional)
	virtual Coord3D getUnitsIntendedPosition(Object *unit, Real *angle) = 0;	// 7  0x00474B51
	virtual void regatherUnit(Object *unit) = 0;						// 8  0x0046F7C9 (provisional)
	virtual AsciiString rva0046D1AC() = 0;								// 9  0x0046D1AC
	virtual Int rva0046979B() = 0;										// 10 0x0046979B
	virtual void assignSpotToUnit(Object *unit) = 0;					// 11 0x00470D09
	virtual void rva00469F2F(Object *unit) = 0;							// 12 0x00469F2F
	virtual void notifyOfMemberDeath(Object *unit) = 0;					// 13 0x004704E4
	virtual void removeMemberFromHorde(Object *unit) = 0;				// 14 0x00470717
	virtual void hordeSlot15() = 0;										// 15 0x000B3FD0 (shared ret)
	virtual void performReform() = 0;									// 16 0x00474BDA
	virtual void rva0046FE99(HordeObjectList &list) = 0;	// 17 0x0046FE99
	virtual Object *rva0046C7A4(Bool flag, const Coord3D *pos, Real distance, Object *obj, Int arg) = 0;	// 18 0x0046C7A4
	virtual Object *rva0046CB2C() = 0;	// 19 0x0046CB2C
	virtual Object *rva0046CC09() = 0;	// 20 0x0046CC09
	virtual Object *rva0046CBCA() = 0;	// 21 0x0046CBCA
	virtual void *rva004696CD() = 0;	// 22 0x004696CD
	virtual Bool rva0046CDC9() = 0;	// 23 0x0046CDC9
	virtual void rva004696E5() = 0;	// 24 0x004696E5
	virtual Bool rva0046CCEF(const ThingTemplate *tmpl) = 0;	// 25 0x0046CCEF
	virtual void changeToFormation(const ThingTemplate *tmpl) = 0;	// 26 0x00472D43
	virtual Bool hordeSlot27(Object *obj, Int arg, const void *names, Bool flag) = 0;	// 27 0x0046970D (the map spells the third argument with a placeholder struct)
	virtual Bool rva0046EA94(Object *obj) = 0;	// 28 0x0046EA94
	virtual Object *rva00473212(Object *obj, const Object *other, Bool flag) = 0;	// 29 0x00473212
	virtual void rva00473799(const HordeObjectList &list) = 0;	// 30 0x00473799
	virtual void rva00472A24(const Coord3D *pos, CommandSourceType cmdSource, Int arg) = 0;	// 31 0x00472A24
	virtual void rva00472B54(Object *obj, CommandSourceType cmdSource) = 0;	// 32 0x00472B54 (the map files the body under a placeholder class)
	virtual void rva00472C8E(Object *obj, CommandSourceType cmdSource) = 0;	// 33 0x00472C8E
	virtual Bool rva00468D11() = 0;	// 34 0x00468D11
	virtual Bool rva00468D2C() = 0;	// 35 0x00468D2C
	virtual Bool rva0046BB6F(Int *out, UnsignedInt arg) = 0;	// 36 0x0046BB6F
	virtual Bool rva0046A2A7() = 0;	// 37 0x0046A2A7
	virtual void rva0046BEAC(Object *obj) = 0;	// 38 0x0046BEAC
	virtual Bool rva0046C65C() = 0;	// 39 0x0046C65C
	virtual Bool rva0046C71E() = 0;	// 40 0x0046C71E
	virtual Bool rva0046C6E7() = 0;	// 41 0x0046C6E7
	virtual void slot42(const Object *obj) = 0;	// 42 0x0046B925
	virtual Bool rva0046B9DC(Int arg) = 0;	// 43 0x0046B9DC
	virtual Bool rva0046B95E(Int arg) = 0;	// 44 0x0046B95E
	virtual Bool hordeSlot45(const UpgradeTemplate *upgrade) = 0;	// 45 0x00468CA3: returns slot 44 for the upgrade's mask index (+0x38); ScriptActions::doUnitReceiveUpgrade 0x003BE26D tests the Bool
	virtual void rva0046E7E3(const UpgradeTemplate *upgrade, Bool flag) = 0;	// 46 0x0046E7E3
	virtual void containSlot93() = 0;	// 47 0x00474142: HordeContain's table holds an adjustor thunk (-0xFC) to its ContainModuleInterface slot 93 body 0x00472444, so the two interfaces declare one method
	virtual void rva004707DB(Object *obj, Real value) = 0;	// 48 0x004707DB
	virtual void rva004739B4() = 0;	// 49 0x004739B4
	virtual void rva0046BC30() = 0;	// 50 0x0046BC30
	virtual void rva0046BD70() = 0;	// 51 0x0046BD70
	virtual void rva0046BE0E() = 0;	// 52 0x0046BE0E
	virtual void rva0046C3FE(Player *player) = 0;	// 53 0x0046C3FE
	virtual void hordeSlot54(void *arg) = 0;	// 54 0x0047A69C (the shared one-argument empty body in HordeContain)
	virtual void rva0046C4C0() = 0;	// 55 0x0046C4C0
	virtual void rva0046C327() = 0;	// 56 0x0046C327
	virtual void rva0046C20B() = 0;	// 57 0x0046C20B
	virtual Bool rva0046C5D7(Int arg) = 0;	// 58 0x0046C5D7
	virtual Bool rva0046F8A5() = 0;	// 59 0x0046F8A5
	virtual Bool rva0046F8F4() = 0;	// 60 0x0046F8F4
	virtual Bool hordeSlot61() = 0;	// 61 0x0046A5E6 (a folded field test)
	virtual Bool rva0046AAB8() = 0;	// 62 0x0046AAB8
	virtual void rva004738B6(Object *obj) = 0;	// 63 0x004738B6
	virtual void hordeSlot64(Int value) = 0;	// 64 0x0046F7EB (a folded field setter)
	virtual Int hordeSlot65() = 0;	// 65 0x0046F7F8 (a folded getter: the frame slot 64 stored)
	virtual ContainmentListView hordeSlot66() = 0;	// 66 0x0046F7FF (placeholder struct in the map)
	virtual void rva0046D1F7(HordeConstObjectList &list) = 0;	// 67 0x0046D1F7
	virtual Object *rva0046D27A() = 0;	// 68 0x0046D27A
	virtual Object *rva0046D2C3(Int index) = 0;	// 69 0x0046D2C3
	virtual Object *rva0046D372() = 0;	// 70 0x0046D372
	virtual Int hordeSlot71() = 0;	// 71 0x00381D34 (a folded field getter)
	virtual void hordeSlot72(Int value) = 0;	// 72 0x0046F815 (a folded field setter)
	virtual void hordeSlot73() = 0;	// 73 0x0046F822 (a folded flag setter)
	virtual void rva0046F8B2() = 0;	// 74 0x0046F8B2
	virtual UnsignedByte hordeSlot75() = 0;	// 75 0x0046F82A (a folded byte getter)
	// 76-154: the rest of retail's table 0x00C44C58 (155 slots), decoded from the map names (neutral hordeSlotNN where the map names a placeholder class or a fold)
	virtual void hordeSlot76(void *, void *) = 0;	// 76 0x0050B238
	virtual void startMeleeAttack(Object *) = 0;	// 77 0x0046A5EF
	virtual void rva00468FDC() = 0;	// 78 0x00468FDC
	virtual void rva00473ADF() = 0;	// 79 0x00473ADF
	virtual Bool rva0046A381(Object *) = 0;	// 80 0x0046A381
	virtual Bool rva0046A46F() = 0;	// 81 0x0046A46F
	virtual Bool rva0046A2EC(Object *) = 0;	// 82 0x0046A2EC
	virtual Bool rva0046A416() = 0;	// 83 0x0046A416
	virtual Bool canEngageInMelee(Object *, Int) = 0;	// 84 0x0047306E
	virtual Bool rva00468DCD(Object *) = 0;	// 85 0x00468DCD
	virtual void rva004730B0(Object *) = 0;	// 86 0x004730B0
	virtual Bool rva0046A4C8() = 0;	// 87 0x0046A4C8
	virtual Bool hordeSlot88(Object *obj) = 0;	// 88 0x0046CE9D
	virtual void rva00468D7D(Object *) = 0;	// 89 0x00468D7D
	virtual void rva003BA648(Int value) = 0;	// 90 0x003BA648
	virtual void hordeSlot91() = 0;	// 91 0x005BF27A
	virtual void rva0046D384(Team *) = 0;	// 92 0x0046D384
	virtual void hordeSlot93(Int value) = 0;	// 93 0x0046F7D7
	virtual void hordeSlot94() = 0;	// 94 0x0046F7E4
	virtual UnsignedInt rva004697CD() = 0;	// 95 0x004697CD: a total read unsigned (ScriptConditions::evaluateUnitHealth 0x003E5587 converts it as unsigned)
	virtual UnsignedInt rva0046D3FC(ObjectFilter *) = 0;	// 96 0x0046D3FC: a count (riders plus filtered entries); ModifyVictimForHorde 0x00268BD5 compares it unsigned (test; jbe)
	virtual Int rva00468F68() = 0;	// 97 0x00468F68
	virtual UnsignedInt rva00468F7E() = 0;	// 98 0x00468F7E: a total read unsigned (evaluateUnitHealth 0x003E5587)
	virtual Object * rva004708BC(const Matrix3D *) = 0;	// 99 0x004708BC
	virtual void hordeSlot100() = 0;	// 100 0x0046D4BD
	virtual void hordeSlot101() = 0;	// 101 0x00468F9F
	virtual void hordeSlot102(Int value) = 0;	// 102 0x0046F831
	virtual void backAwaySomeMembersFrom(Object *) = 0;	// 103 0x004756C9
	virtual void hordeSlot104(Bool value) = 0;	// 104 0x0046F84C
	virtual Bool hordeSlot105() = 0;	// 105 0x0046F859
	virtual void hordeSlot106() = 0;	// 106 0x00473B36
	virtual void rva0046D8AE() = 0;	// 107 0x0046D8AE
	virtual void rva0046D7AF(const Coord3D *position) = 0;	// 108 0x0046D7AF: teleports every rider that is not a horde member to `position` (the map spells the argument int)
	virtual void rva0046A78F(const Matrix3D *) = 0;	// 109 0x0046A78F
	virtual void rva0046A712(const Coord3D *) = 0;	// 110 0x0046A712: Object::teleportTo 0x0029660C passes its const Coord3D * (push [esp+8]; call [edx+0x1B8]); the body (ret 4) never reads it
	virtual void hordeSlot111() = 0;	// 111 0x0046ECDE
	virtual void hordeSlot112() = 0;	// 112 0x005CB9FA
	virtual Bool rva0046D80B() = 0;	// 113 0x0046D80B
	virtual Bool rva0046A6C1() = 0;	// 114 0x0046A6C1
	virtual Bool rva0046A677() = 0;	// 115 0x0046A677
	virtual void hordeSlot116() = 0;	// 116 0x003860FA
	virtual void rva0046DDC5(const Coord3D *position, Int index) = 0;	// 117 0x0046DDC5: appends `position` to the locomotor path of every rider that is not a horde member (the map spells the first argument int)
	virtual void rva0046DB6D(const AsciiString &, ObjectFilter *, Int) = 0;	// 118 0x0046DB6D
	virtual void rva0046DC92(const AsciiString &, ObjectFilter *) = 0;	// 119 0x0046DC92
	virtual void hordeSlot120() = 0;	// 120 0x00475EA2
	virtual void rva0046981C() = 0;	// 121 0x0046981C
	virtual void rva00469851() = 0;	// 122 0x00469851
	virtual void rva0046DE2D(const FXList *) = 0;	// 123 0x0046DE2D
	virtual void hordeSlot124() = 0;	// 124 0x0046DA6B
	virtual Bool rva0046992C() = 0;	// 125 0x0046992C
	virtual void hordeSlot126() = 0;	// 126 0x00475F61
	virtual Bool rva004698BC() = 0;	// 127 0x004698BC
	virtual const void *rva004698D6() = 0;	// 128 0x004698D6
	virtual const void *rva004698E2() = 0;	// 129 0x004698E2
	virtual void rva00469967() = 0;	// 130 0x00469967
	virtual void hordeSlot131(Real angle) = 0;	// 131 0x00469075
	virtual void rva004690A9(const Coord3D *) = 0;	// 132 0x004690A9
	virtual void hordeSlot133() = 0;	// 133 0x004690C8
	virtual void *hordeSlot134() = 0;	// 134 0x0046F83E
	virtual void hordeSlot135() = 0;	// 135 0x0046FF81
	virtual Bool rva0046F8CD() = 0;	// 136 0x0046F8CD
	virtual void hordeSlot137() = 0;	// 137 0x0046EC7C
	virtual void hordeSlot138() = 0;	// 138 0x0046DEA1
	virtual Bool rva0046DF9A(Coord3D *) = 0;	// 139 0x0046DF9A
	virtual Bool rva0046E113(Coord3D *) = 0;	// 140 0x0046E113
	virtual void rva004690D0(Int) = 0;	// 141 0x004690D0
	virtual Bool rva00468C37() = 0;	// 142 0x00468C37
	virtual void rva00468BDC(Int) = 0;	// 143 0x00468BDC
	virtual Real rva00468B5B(Real) = 0;	// 144 0x00468B5B
	virtual void rva00468C60(const Coord3D *) = 0;	// 145 0x00468C60
	virtual Bool rva00468C7B(Coord3D *) = 0;	// 146 0x00468C7B
	virtual Bool isFlankedBy(Object *) = 0;	// 147 0x00473D55
	virtual void rva0046E2BC() = 0;	// 148 0x0046E2BC
	virtual UnsignedByte hordeSlot149() = 0;	// 149 0x0046F845
	virtual Real rva0046F8DA() = 0;	// 150 0x0046F8DA
	virtual Real rva0046F8E7() = 0;	// 151 0x0046F8E7
	virtual void hordeSlot152() = 0;	// 152 0x004692BA
	virtual Real rva0046B850() = 0;	// 153 0x0046B850
	virtual void endMove() = 0;	// 154 0x0046A0B2
};

#endif
