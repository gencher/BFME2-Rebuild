// The shared state and behavior of a horde?s member formation.
#pragma once
#ifndef BFME2_HORDE_CONTAIN_H
#define BFME2_HORDE_CONTAIN_H

#include <stddef.h>
#include <set>
#include <map>
#include <list>
#include <vector>
#include "Common/DisabledTypes.h"
#include "Common/GameType.h"
#include "GameLogic/Module/TransportContain.h"
#include "GameLogic/Module/HordeContainInterface.h"
#include "Common/Coord3D.h"
#include "GameLogic/Module/HordeMeleeBase.h"

class Thing;
class ModuleData;
class Xfer;

// TransportContain ends at 0x11C; this class's members follow it.
typedef char HordeContain_base_size[(sizeof(TransportContain) == 0x11C) ? 1 : -1];

// The block the best-experience search shares with iterateContained (HordeContainInterface slot 21, 0x0046CBCA): the highest
// experience seen so far and the id of its owner (0 until a unit has been seen).
struct HordeBestExperienceSearch
{
	Real m_best;
	ObjectID m_id;
};
// 0x00469689: the iterateContained visitor that fills a HordeBestExperienceSearch.
void hordeBestExperienceVisitor(Object *unit, void *userData);

// The block the template scan 0x004698EE fills through iterateContained (HordeContain 0x0046992C starts it): whether two
// different templates were seen, and the template id of the first unit.
struct HordeTemplateScan
{
	Bool m_mixed;
	UnsignedByte m_pad;
	UnsignedShort m_templateID;
};
// 0x004698EE: the iterateContained visitor that fills a HordeTemplateScan.
void hordeTemplateScanVisitor(Object *unit, void *userData);

// One banner carrier entry (the vector at +0x270): +0 the carrier's template, +4 the index getBannerCarrierIndexToUse
// (0x0046A521) hands back. Only these two members are recovered.
struct HordeBannerCarrier
{
	const ThingTemplate *m_template;	// +0x0
	Int m_index;						// +0x4
};

// One entry of HordeContain's slot vector (+0x188): 0x1C bytes. +0 is the rank number of the module data's RankInfo the slot
// stands for (0x00469294 matches it); +0x18 is tested signed by HordeMeleeFormation::canUnitRotate (0x00584F6B).
struct HordeFormationSlot
{
	Int m_rank;							// +0x00
	UnsignedByte m_unk0x04[0x14];
	Int m_unk0x18;						// +0x18
};
typedef char HordeFormationSlot_size[(sizeof(HordeFormationSlot) == 0x1C) ? 1 : -1];

class HordeMeleeSwarm;

class HordeContain : public TransportContain, public HordeContainInterface
{
public:
	friend class HordeMeleeSwarm;	// 0x00583B6F reads the horde's object
	friend class HordeMeleeFormation;	// canUnitRotate reads the horde's formation slots
	HordeContain(Thing *thing, const ModuleData *moduleData);
	static Module *friend_newModuleInstance(Thing *thing, const ModuleData *moduleData);
	virtual NameKeyType getModuleNameKey() const;
	virtual ~HordeContain();
	virtual DisabledMaskType getDisabledTypesToProcess() const
	{
		return DisabledMaskType(DisabledMaskType::kInit, DISABLED_HELD);
	}

	void rva0046D738();	// 0x0046D738 (non-virtual)
	void rva00469F3A(Object *unit);	// 0x00469F3A (non-virtual; adjusts the unit's model condition flags, not written yet)
	void rva0046AA85();	// 0x0046AA85 (non-virtual; hands every rider to rva00469F3A)
	UnsignedByte usingMeleeAttack();	// 0x004695DA
	Coord3D rva00472620(Object *unit, Real *angle);	// 0x00472620 (non-virtual; getUnitsIntendedPosition's fallback; the map spells it QAEPAVCoord3D@@PAU2@PAVObject@@H@Z)
	void notifyContainerOfUnit(Object *unit);	// 0x00469B73 (the map spells the argument void *)
	void rva0046E740(Int);	// 0x0046E740 (not written yet)
	Bool isPositionBehindUnit(Object *unit, const Coord3D *position);	// 0x00469012 (the map's class is HorseHordeContain)
	void refreshWhenObjectHasAI();	// 0x0046E7C2 (calls 0x0046E740 with 0)
	// HordeContain's own primary slots 31..38 (retail table 0x00C45050, after TransportContain's 31 slots)
	virtual Bool dissolveComboHordeIfNeeded();	// 31 0x0046AD8B (the ledger spells it QAE)
	virtual void rva00472CA9(Object *unit);	// 32 0x00472CA9
	virtual void rva00474519(Int);	// 33 0x00474519 (the map's class is the placeholder Rva00474431)
	virtual void rva0046AF85();	// 34 0x0046AF85
	virtual void rva00470DC1();	// 35 0x00470DC1
	virtual void rva00474842(Object *unit, const Coord3D *position, Real, Bool);	// 36 0x00474842 (the ledger spells it QAE)
	virtual Bool rva00470B21();	// 37 0x00470B21
	virtual void gatherUnitBack(Object *unit);	// 38 0x004725D5
	Int getBannerCarrierIndexToUse(const Object *unit, const ThingTemplate **carrier);	// 0x0046A521
	const ThingTemplate *pickRandomRankTemplate();	// 0x0046AF12 (the map spells the return void *)
	Bool rankUnitTypeMatches(Object *unit, const AsciiString &unitType);	// 0x004723F2
	Bool rva00468F0F(Object *unit, Object *other);	// 0x00468F0F (non-virtual)
	Int rva0046ACF6(Int id);	// 0x0046ACF6 (non-virtual; the member's formation slot index, 0 when absent)
	void rva00469886(Object *unit);	// 0x00469886 (non-virtual; HordeContainInterface slot 12 forwards to it)
	virtual void onDelete();	// override: onDelete slot 8 at +0x0 (0x0046E697)
	virtual void createPayload();	// override: createPayload slot 28 at +0x0 (0x0046E8EE)
	virtual void onContaining(Object *obj, Bool wasSelected);	// override: onContaining slot 22 at +0x20 (0x0046FA46)
	virtual void onRemoving(Object *obj);	// override: onRemoving slot 23 at +0x20 (0x00469E03)
	virtual Bool isValidContainerFor(Object *obj, Bool checkCapacity, Bool flag);	// override: ContainModuleInterface slot 38 at +0x20 (0x00469647; the map spells it slot38(Object *, int, int))
	virtual Bool containSlot11();	// override: ContainModuleInterface slot 11 at +0x20 (0x00469626)
	virtual Bool isPassengerAllowedToFire() const;	// override: ContainModuleInterface slot 45 at +0x20 (0x00469602)
	virtual Bool attemptBestFirePointPosition(Object *source, Weapon *weapon, Object *victim);	// override: ContainModuleInterface slot 78 at +0x20 (0x00468D47)
	virtual void iterateContained(ContainIterateFunc func, void *userData, UnsignedInt flags);	// override: iterateContained slot 68 at +0x20 (0x0046DD67)

	// HordeContainInterface at +0x11C (constructor 0x0046F543 stores its pure table 0x00C448B8, then
	// HordeContain's 0x00C44C58): HordeContain fills all 76 slots.
	virtual void rva00472329(const Coord3D *pos, Int arg);	// 0 0x00472329
	virtual void attackTargetNow(Object *victim, Int arg);	// 1 0x00471FFA
	virtual void rva00472235();	// 2 0x00472235
	virtual void rva0046E253();	// 3 0x0046E253
	virtual void returnToFormation(Bool flag);	// 4 0x00472790
	virtual void stopMembersAttacking(Object *victim);	// 5 0x004726DD
	virtual Bool hasSpotForUnit(const Object *unit);	// 6 0x0046BB38
	virtual Coord3D getUnitsIntendedPosition(Object *unit, Real *angle);	// 7 0x00474B51
	virtual void regatherUnit(Object *unit);	// 8 0x0046F7C9
	virtual AsciiString rva0046D1AC();	// 9 0x0046D1AC
	virtual Int rva0046979B();	// 10 0x0046979B
	virtual void assignSpotToUnit(Object *unit);	// 11 0x00470D09
	virtual void rva00469F2F(Object *unit) { rva00469886(unit); }	// 12 0x00469F2F (emitted inline)
	virtual void notifyOfMemberDeath(Object *unit);	// 13 0x004704E4
	virtual void removeMemberFromHorde(Object *unit);	// 14 0x00470717
	virtual void hordeSlot15() {}	// 15: the shared empty body 0x000B3FD0
	virtual void performReform();	// 16 0x00474BDA
	virtual void rva0046FE99(HordeObjectList &list);	// 17 0x0046FE99
	virtual Object *rva0046C7A4(Bool flag, const Coord3D *pos, Real distance, Object *obj, Int arg);	// 18 0x0046C7A4
	virtual Object *rva0046CB2C();	// 19 0x0046CB2C
	virtual Object *rva0046CC09();	// 20 0x0046CC09
	virtual Object *rva0046CBCA();	// 21 0x0046CBCA
	virtual void *rva004696CD();	// 22 0x004696CD
	virtual Bool rva0046CDC9();	// 23 0x0046CDC9
	virtual void rva004696E5();	// 24 0x004696E5
	virtual Bool rva0046CCEF(const ThingTemplate *tmpl);	// 25 0x0046CCEF
	virtual void changeToFormation(const ThingTemplate *tmpl);	// 26 0x00472D43
	virtual Bool hordeSlot27(Object *obj, Int arg, const void *names, Bool flag);	// 27 0x0046970D
	virtual Bool rva0046EA94(Object *obj);	// 28 0x0046EA94
	virtual Object *rva00473212(Object *obj, const Object *other, Bool flag);	// 29 0x00473212
	virtual void rva00473799(const HordeObjectList &list);	// 30 0x00473799
	virtual void rva00472A24(const Coord3D *pos, CommandSourceType cmdSource, Int arg);	// 31 0x00472A24
	virtual void rva00472B54(Object *obj, CommandSourceType cmdSource);	// 32 0x00472B54
	virtual void rva00472C8E(Object *obj, CommandSourceType cmdSource);	// 33 0x00472C8E
	virtual Bool rva00468D11();	// 34 0x00468D11
	virtual Bool rva00468D2C();	// 35 0x00468D2C
	virtual Bool rva0046BB6F(Int *out, UnsignedInt arg);	// 36 0x0046BB6F
	virtual Bool rva0046A2A7();	// 37 0x0046A2A7
	virtual void rva0046BEAC(Object *obj);	// 38 0x0046BEAC
	virtual Bool rva0046C65C();	// 39 0x0046C65C
	virtual Bool rva0046C71E();	// 40 0x0046C71E
	virtual Bool rva0046C6E7();	// 41 0x0046C6E7
	virtual void slot42(const Object *obj);	// 42 0x0046B925
	virtual Bool rva0046B9DC(Int arg);	// 43 0x0046B9DC
	virtual Bool rva0046B95E(Int arg);	// 44 0x0046B95E
	virtual Bool hordeSlot45(const UpgradeTemplate *upgrade);	// 45 0x00468CA3
	virtual void rva0046E7E3(const UpgradeTemplate *upgrade, Bool flag);	// 46 0x0046E7E3
	virtual void containSlot93();	// 47 0x00474142
	virtual void rva004707DB(Object *obj, Real value);	// 48 0x004707DB
	virtual void rva004739B4();	// 49 0x004739B4
	virtual void rva0046BC30();	// 50 0x0046BC30
	virtual void rva0046BD70();	// 51 0x0046BD70
	virtual void rva0046BE0E();	// 52 0x0046BE0E
	virtual void rva0046C3FE(Player *player);	// 53 0x0046C3FE
	virtual void hordeSlot54(void *arg) {}	// 54: the shared one-argument empty body 0x0047A69C
	virtual void rva0046C4C0();	// 55 0x0046C4C0
	virtual void rva0046C327();	// 56 0x0046C327
	virtual void rva0046C20B();	// 57 0x0046C20B
	virtual Bool rva0046C5D7(Int arg);	// 58 0x0046C5D7
	virtual Bool rva0046F8A5();	// 59 0x0046F8A5
	virtual Bool rva0046F8F4();	// 60 0x0046F8F4
	virtual Bool hordeSlot61();	// 61 0x0046A5E6
	virtual Bool rva0046AAB8();	// 62 0x0046AAB8
	virtual void rva004738B6(Object *obj);	// 63 0x004738B6
	virtual void hordeSlot64(Int value);	// 64 0x0046F7EB
	virtual Int hordeSlot65();	// 65 0x0046F7F8
	virtual ContainmentListView hordeSlot66();	// 66 0x0046F7FF
	virtual void rva0046D1F7(HordeConstObjectList &list);	// 67 0x0046D1F7
	virtual Object *rva0046D27A();	// 68 0x0046D27A
	virtual Object *rva0046D2C3(Int index);	// 69 0x0046D2C3
	virtual Object *rva0046D372();	// 70 0x0046D372
	virtual Int hordeSlot71();	// 71 0x00381D34
	virtual void hordeSlot72(Int value);	// 72 0x0046F815
	virtual void hordeSlot73();	// 73 0x0046F822
	virtual void rva0046F8B2();	// 74 0x0046F8B2
	virtual UnsignedByte hordeSlot75();	// 75 0x0046F82A
	virtual void hordeSlot76(void *, void *);	// 76 0x0050B238
	virtual void startMeleeAttack(Object *);	// 77 0x0046A5EF
	virtual void rva00468FDC();	// 78 0x00468FDC
	virtual void rva00473ADF();	// 79 0x00473ADF
	virtual Bool rva0046A381(Object *);	// 80 0x0046A381
	virtual Bool rva0046A46F();	// 81 0x0046A46F
	virtual Bool rva0046A2EC(Object *);	// 82 0x0046A2EC
	virtual Bool rva0046A416();	// 83 0x0046A416
	virtual Bool canEngageInMelee(Object *, Int);	// 84 0x0047306E
	virtual Bool rva00468DCD(Object *);	// 85 0x00468DCD
	virtual void rva004730B0(Object *);	// 86 0x004730B0
	virtual Bool rva0046A4C8();	// 87 0x0046A4C8
	virtual Bool hordeSlot88(Object *obj);	// 88 0x0046CE9D
	virtual void rva00468D7D(Object *);	// 89 0x00468D7D
	virtual void rva003BA648(Int value);	// 90 0x003BA648
	virtual void hordeSlot91();	// 91 0x005BF27A
	virtual void rva0046D384(Team *);	// 92 0x0046D384
	virtual void hordeSlot93(Int value);	// 93 0x0046F7D7
	virtual void hordeSlot94();	// 94 0x0046F7E4
	virtual UnsignedInt rva004697CD();	// 95 0x004697CD
	virtual UnsignedInt rva0046D3FC(ObjectFilter *);	// 96 0x0046D3FC
	virtual Int rva00468F68();	// 97 0x00468F68
	virtual UnsignedInt rva00468F7E();	// 98 0x00468F7E
	virtual Object * rva004708BC(const Matrix3D *);	// 99 0x004708BC
	virtual void hordeSlot100();	// 100 0x0046D4BD
	virtual void hordeSlot101();	// 101 0x00468F9F
	virtual void hordeSlot102(Int value);	// 102 0x0046F831
	virtual void backAwaySomeMembersFrom(Object *);	// 103 0x004756C9
	virtual void hordeSlot104(Bool value);	// 104 0x0046F84C
	virtual Bool hordeSlot105();	// 105 0x0046F859
	virtual void hordeSlot106();	// 106 0x00473B36
	virtual void rva0046D8AE();	// 107 0x0046D8AE
	virtual void rva0046D7AF(const Coord3D *position);	// 108 0x0046D7AF
	virtual void rva0046A78F(const Matrix3D *);	// 109 0x0046A78F
	virtual void rva0046A712(const Coord3D *);	// 110 0x0046A712
	virtual void hordeSlot111();	// 111 0x0046ECDE
	virtual void hordeSlot112();	// 112 0x005CB9FA
	virtual Bool rva0046D80B();	// 113 0x0046D80B
	virtual Bool rva0046A6C1();	// 114 0x0046A6C1
	virtual Bool rva0046A677();	// 115 0x0046A677
	virtual void hordeSlot116();	// 116 0x003860FA
	virtual void rva0046DDC5(const Coord3D *position, Int index);	// 117 0x0046DDC5
	virtual void rva0046DB6D(const AsciiString &, ObjectFilter *, Int);	// 118 0x0046DB6D
	virtual void rva0046DC92(const AsciiString &, ObjectFilter *);	// 119 0x0046DC92
	virtual void hordeSlot120();	// 120 0x00475EA2
	virtual void rva0046981C();	// 121 0x0046981C
	virtual void rva00469851();	// 122 0x00469851
	virtual void rva0046DE2D(const FXList *);	// 123 0x0046DE2D
	virtual void hordeSlot124();	// 124 0x0046DA6B
	virtual Bool rva0046992C();	// 125 0x0046992C
	virtual void hordeSlot126();	// 126 0x00475F61
	virtual Bool rva004698BC();	// 127 0x004698BC
	virtual const void *rva004698D6();	// 128 0x004698D6
	virtual const void *rva004698E2();	// 129 0x004698E2
	virtual void rva00469967();	// 130 0x00469967
	virtual void hordeSlot131(Real angle);	// 131 0x00469075
	virtual void rva004690A9(const Coord3D *);	// 132 0x004690A9
	virtual void hordeSlot133();	// 133 0x004690C8
	virtual void *hordeSlot134();	// 134 0x0046F83E
	virtual void hordeSlot135();	// 135 0x0046FF81
	virtual Bool rva0046F8CD();	// 136 0x0046F8CD
	virtual void hordeSlot137();	// 137 0x0046EC7C
	virtual void hordeSlot138();	// 138 0x0046DEA1
	virtual Bool rva0046DF9A(Coord3D *);	// 139 0x0046DF9A
	virtual Bool rva0046E113(Coord3D *);	// 140 0x0046E113
	virtual void rva004690D0(Int);	// 141 0x004690D0
	virtual Bool rva00468C37();	// 142 0x00468C37
	virtual void rva00468BDC(Int);	// 143 0x00468BDC
	virtual Real rva00468B5B(Real);	// 144 0x00468B5B
	virtual void rva00468C60(const Coord3D *);	// 145 0x00468C60
	virtual Bool rva00468C7B(Coord3D *);	// 146 0x00468C7B
	virtual Bool isFlankedBy(Object *);	// 147 0x00473D55
	void rva00468B24(Real value);	// 0x00468B24
	virtual void rva0046E2BC();	// 148 0x0046E2BC
	virtual UnsignedByte hordeSlot149();	// 149 0x0046F845
	virtual Real rva0046F8DA();	// 150 0x0046F8DA
	virtual Real rva0046F8E7();	// 151 0x0046F8E7
	virtual void hordeSlot152();	// 152 0x004692BA
	virtual Real rva0046B850();	// 153 0x0046B850
	virtual void endMove();	// 154 0x0046A0B2

protected:
	virtual const char *GetSnapshotName() const;
	virtual void xfer(Xfer *xfer);

public:
	Bool m_unk0x120;                               // +0x120; xfer Bool
	Bool m_unk0x121;                               // +0x121; xfer Bool
	Coord3D m_unk0x124;                            // +0x124; xfer Coord3D
	UnsignedByte m_gap0x130[0x40];
	_STL::set<Int> m_memberIDs;							// +0x170; slot42 0x0046B925 inserts through set<int>::insert 0x000BC15D, hasSpotForUnit (slot 6) looks members up in it
	_STL::map<Int, Int> m_slotIndexByMemberID;						// +0x17C; xfer: a count, then (ObjectID, Int) pairs; maps a member's id to its index in m_formationSlots (operator[] 0x0028932C, find 0x00388F63)
	_STL::vector<HordeFormationSlot> m_formationSlots;		// +0x188; 0x1C-byte entries, indexed through m_slotIndexByMemberID (the divide by 0x1C in 0x00470B21 is its size())
	_STL::list<Int> m_unk0x194;							// +0x194; xfer: a count, then Ints (push_back 0x0005548F); 0x0046AF12 picks one at random
	UnsignedByte m_gap0x198[4];
	UnsignedInt m_unk0x19C;                        // +0x19C; xfer UnsignedInt
	UnsignedByte m_gap0x1A0[0xC4];
	ObjectID m_unk0x264;                           // +0x264; zero in the constructor; HordeContainInterface slot 97 (0x00468F68) counts it and +0x26C, 0x0046936B clears whichever equals a dying unit's id
	Int m_unk0x268;                                // +0x268; xfer Int
	ObjectID m_unk0x26C;								// +0x26C; HordeContainInterface slot 70 (0x0046D372) looks this object up
	_STL::vector<HordeBannerCarrier *> m_unk0x270;					// +0x270; onDelete 0x0046E697 frees each entry with operator delete, then erases them (vector<void *>::erase 0x0051BD55)
	UnsignedInt m_unk0x27C;                        // +0x27C; xfer UnsignedInt
	Int m_unk0x280;                                // +0x280; xfer Int
	Int m_unk0x284;                                // +0x284; xfer Int
	ObjectID m_unk0x288;                           // +0x288; xfer XferObjectID 0x003060B2 (0x004751F6), zero in the constructor; HordeContainInterface slots 85 (0x00468DCD) and 89 (0x00468D7D) compare the object's id (and its 0x002931F5 object's) with it
	UnsignedInt m_unk0x28C;                        // +0x28C; xfer UnsignedInt
	UnsignedInt m_unk0x290;                        // +0x290; xfer UnsignedInt
	Bool m_unk0x294;                               // +0x294; xfer Bool
	UnsignedByte m_gap0x295[3];
	UnsignedInt m_unk0x298;                        // +0x298; xfer UnsignedInt (the constructor stores -1)
	ObjectID m_unk0x29C;                           // +0x29C; xfer XferObjectID
	ObjectID m_unk0x2A0;                           // +0x2A0; zero in the constructor, xfer XferObjectID; HordeContainInterface slots 77 (0x0046A5EF) and 78 (0x00468FDC) store/clear it, 0x0046D738 looks the object up with it
	Bool m_unk0x2A4;                               // +0x2A4; xfer Bool (the constructor stores a byte); HordeContainInterface slot 74 (0x0046F8B2) clears it
	UnsignedByte m_gap0x2A5[3];
	Int m_unk0x2A8;                                // +0x2A8; xfer Int (the constructor stores 100)
	Bool m_unk0x2AC;                               // +0x2AC; xfer Bool
	UnsignedByte m_gap0x2AD[3];
	ObjectID m_unk0x2B0;                           // +0x2B0; xfer XferObjectID
	Bool m_unk0x2B4;                               // +0x2B4; the constructor stores a byte
	UnsignedByte m_gap0x2B5[3];
	Coord3D m_unk0x2B8;                            // +0x2B8; xfer Coord3D
	Bool m_unk0x2C4;                               // +0x2C4; xfer Bool
	Bool m_unk0x2C5;                               // +0x2C5; xfer Bool
	UnsignedByte m_gap0x2C6[2];
	HordeMeleeBase *m_unk0x2C8;                    // +0x2C8; the constructor stores a HordeMeleeSwarm (0x005843DA) or the template factory's object here; slots 85/89 call its slot 14 (offset 0x38), xfer its slot 15 (0x004751F3)
	UnsignedByte m_gap0x2CC[0x10];
	_STL::map<ObjectID, UnsignedInt> m_unk0x2DC;		// +0x2DC; cleared by rva0046E2BC
	Bool m_unk0x2E8;                               // +0x2E8; the constructor stores 1; HordeContainInterface slot 149 (0x0046F845) reads it
	UnsignedByte m_gap0x2E9[3];
	Real m_unk0x2EC;									// +0x2EC; raised by rva00468B24 and capped
	Int m_unk0x2F0;										// +0x2F0; set by rva00468BDC
	UnsignedByte m_gap0x2F4[4];
	Coord3D m_unk0x2F8;									// +0x2F8; written by rva00468C60, taken by rva00468C7B
	Bool m_unk0x304;									// +0x304; set when m_unk0x2F8 holds a position
	UnsignedByte m_gap0x305[3];
	Int m_unk0x308;                                // +0x308; HordeContainInterface slot 64 (0x0046F7EB) stores into it and slot 65 (0x0046F7F8) reads it (both at interface +0x1EC)
};
// SIZE 0x30C
typedef char HordeContain_size[(sizeof(HordeContain) == 0x30C) ? 1 : -1];
typedef char HordeContain_off_m_unk0x120[(offsetof(HordeContain, m_unk0x120) == 0x120) ? 1 : -1];
typedef char HordeContain_off_m_unk0x121[(offsetof(HordeContain, m_unk0x121) == 0x121) ? 1 : -1];
typedef char HordeContain_off_m_unk0x124[(offsetof(HordeContain, m_unk0x124) == 0x124) ? 1 : -1];
typedef char HordeContain_off_m_memberIDs[(offsetof(HordeContain, m_memberIDs) == 0x170) ? 1 : -1];
typedef char HordeContain_off_m_slotIndexByMemberID[(offsetof(HordeContain, m_slotIndexByMemberID) == 0x17C) ? 1 : -1];
typedef char HordeContain_off_m_unk0x19C[(offsetof(HordeContain, m_unk0x19C) == 0x19C) ? 1 : -1];
typedef char HordeContain_off_m_unk0x268[(offsetof(HordeContain, m_unk0x268) == 0x268) ? 1 : -1];
typedef char HordeContain_off_m_unk0x27C[(offsetof(HordeContain, m_unk0x27C) == 0x27C) ? 1 : -1];
typedef char HordeContain_off_m_unk0x280[(offsetof(HordeContain, m_unk0x280) == 0x280) ? 1 : -1];
typedef char HordeContain_off_m_unk0x284[(offsetof(HordeContain, m_unk0x284) == 0x284) ? 1 : -1];
typedef char HordeContain_off_m_unk0x288[(offsetof(HordeContain, m_unk0x288) == 0x288) ? 1 : -1];
typedef char HordeContain_off_m_unk0x2C8[(offsetof(HordeContain, m_unk0x2C8) == 0x2C8) ? 1 : -1];
typedef char HordeContain_off_m_unk0x298[(offsetof(HordeContain, m_unk0x298) == 0x298) ? 1 : -1];
typedef char HordeContain_off_m_unk0x264[(offsetof(HordeContain, m_unk0x264) == 0x264) ? 1 : -1];
typedef char HordeContain_off_m_unk0x2E8[(offsetof(HordeContain, m_unk0x2E8) == 0x2E8) ? 1 : -1];
typedef char HordeContain_off_m_unk0x2A0[(offsetof(HordeContain, m_unk0x2A0) == 0x2A0) ? 1 : -1];
typedef char HordeContain_off_m_unk0x28C[(offsetof(HordeContain, m_unk0x28C) == 0x28C) ? 1 : -1];
typedef char HordeContain_off_m_unk0x290[(offsetof(HordeContain, m_unk0x290) == 0x290) ? 1 : -1];
typedef char HordeContain_off_m_unk0x294[(offsetof(HordeContain, m_unk0x294) == 0x294) ? 1 : -1];
typedef char HordeContain_off_m_unk0x2A4[(offsetof(HordeContain, m_unk0x2A4) == 0x2A4) ? 1 : -1];
typedef char HordeContain_off_m_unk0x2B8[(offsetof(HordeContain, m_unk0x2B8) == 0x2B8) ? 1 : -1];
typedef char HordeContain_off_m_unk0x2C4[(offsetof(HordeContain, m_unk0x2C4) == 0x2C4) ? 1 : -1];

#endif
