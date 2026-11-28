#pragma once

#include "Common/Coord3D.h"
#include "Common/GameType.h"
#include "Common/Snapshot.h"
#include "Common/Vector.h"

struct ICoord2D;
class LivingWorldListener;
class Vector2;
class Vector3;

// Base table 0x007FE508 has 43 slots. The W3D leaf calls 0x002C0120,
// installs its own table 0x007C89C8, then starts its fields at +0xC0.
// The base destructor 0x002BFB2D restores Snapshot's table.
class LivingWorld : public Snapshot
{
public:
	LivingWorld();	// 0x002C0120
	virtual ~LivingWorld();

	// Pure slot signatures follow the W3D leaf. Ordinals mark methods whose
	// role is still unknown.
	virtual void unknownSlot4() = 0;	// leaf 0x0009D862
	virtual void unknownSlot5() = 0;	// leaf 0x0009D9FD
	virtual void unknownSlot6() = 0;	// leaf 0x0009AB6B
	virtual void unknownSlot7() = 0;	// leaf 0x0009C144
	virtual void unknownSlot8() = 0;	// leaf 0x0009BC17
	virtual void setViewport(Int x, Int y, Int width, Int height) = 0;	// slot 9, stores all four viewport fields at 0x0009C545
	virtual void setActive(Bool active);	// slot 10, 0x002BF391 writes +0x18 and notifies listeners
	virtual void unknownSlot11(const Vector2 *position, Real factor) = 0;	// leaf 0x0009BD98
	virtual void unknownSlot12() = 0;	// leaf 0x0009AD42
	virtual void unknownSlot13(const ICoord2D *screenPoint, Vector3 *origin, Coord3D *direction) = 0;	// leaf 0x0009BF33
	virtual void screenToTerrain(const ICoord2D *screenPoint, Coord3D *position) = 0;	// slot 14, leaf 0x0009ADA0; MilesAudioManager::getScreenToTerrainPositions 0x0005294A passes a ScreenToTerrainSet entry's Coord3D
	virtual Bool worldToScreen(const Vector3 *position, ICoord2D *screenPoint) = 0;	// slot 15, leaf 0x0009AE98
	virtual Real unknownSlot16() { return 0.0f; }	// 0x002BFBF0 loads native +0.0
	virtual void unknownSlot17(Int value) = 0;	// leaf 0x0009AD06
	virtual void unknownSlot18(Int value) = 0;	// leaf 0x0009AD24
	virtual void setMode(UnsignedInt mode, UnsignedInt argument);	// slot 19, 0x002BE7FA changes +0x14 then dispatches
	virtual void unknownSlot20(Bool enabled) { m_unk0x19 = enabled; }	// 0x0009AA01
	virtual void unknownSlot21(Int value) = 0;	// leaf 0x0009AFCE
	virtual void unknownSlot22(const Vector3 *position) = 0;	// leaf 0x0009B023, location from 0x002BEC98
	virtual void unknownSlot23(Int first, Int second) = 0;	// leaf 0x0009B0AB
	virtual Bool unknownSlot24() = 0;	// leaf 0x0009AF90
	virtual Real unknownSlot25() = 0;	// leaf 0x0045A20A
	virtual void unknownSlot26(Real fraction) = 0;	// leaf 0x0009AFA5 clamps to [0, 1]
	virtual void update();	// slot 27, 0x002C021A updates the active transition and dispatches slot 10
	virtual void unknownSlot28(Vector3 *position) = 0;	// leaf 0x0009AC4E
	virtual void getPosition(Coord3D *position) = 0;	// slot 29, writes the camera transition origin at 0x002BF01B
	virtual Real unknownSlot30() = 0;	// leaf 0x0009ACA0
	virtual void unknownSlot31(Real value) = 0;	// leaf 0x0009ACA7
	virtual void unknownSlot32(Vector3 *position) = 0;	// leaf 0x0009ACB8, copies RenderObjClass::Get_Position
	virtual void unknownSlot33() { m_unk0x24 = true; }	// 0x004A18C0
	virtual void unknownSlot34() { m_unk0x25 = true; }	// 0x0009D6E8
	virtual Bool unknownSlot35() const { return m_unk0x24; }	// 0x0009D6ED
	virtual Bool unknownSlot36() const { return m_unk0x25; }	// 0x0009D6F1
	virtual void unknownSlot37() { m_unk0x24 = false; }	// 0x002886A2
	virtual void unknownSlot38() { m_unk0x25 = false; }	// 0x0009D6F5
	virtual void startCameraTransition(const Coord3D *position, Real value, Real fraction, UnsignedInt duration);	// slot 39, 0x002BEFE5
	virtual void unknownSlot40(Bool value) = 0;	// leaf 0x0009AD4D
	virtual void unknownSlot41(Bool value) = 0;	// leaf 0x0009AD78
	virtual Int unknownSlot42() const = 0;	// leaf 0x003F3FD2

	void rva002BED10();	// LivingWorldLogic::reset calls it

protected:
	virtual void loadPostProcess() {}	// Snapshot slot 1, 0x000B3FD0
	virtual const char *GetSnapshotName() const;	// Snapshot slot 2, 0x002BE8CE
	virtual void xfer(Xfer *xfer);	// Snapshot slot 3, 0x002BE833

private:
	_STL::vector<LivingWorldListener *> m_listeners;	// +0x04, 0x002BF4E9 dispatches the active-state change
	Int m_listenerIndex;	// +0x10, 0x002BF10F tracks each callback
	UnsignedInt m_mode;	// +0x14, setMode 0x002BE7FA
	Bool m_active;	// +0x18, setActive 0x002BF391
	Bool m_unk0x19;
	Bool m_unk0x1A;
	Bool m_unk0x1B;
	Real m_unk0x1C;
	Real m_unk0x20;
	Bool m_unk0x24;
	Bool m_unk0x25;
	UnsignedByte m_unk0x26[2];
	UnsignedByte m_unk0x28[0x54 - 0x28];	// 0x002BF807 builds the embedded record and its vector
	Coord3D m_transitionOrigin;	// +0x54, captured by slot 29 at 0x002BF01B
	Coord3D m_transitionDestination;	// +0x60, copied from the caller's Coord3D at 0x002BF05A
	Real m_transitionOriginFraction;	// +0x6C, slot 25 at 0x002BF06F
	Real m_transitionDestinationFraction;	// +0x70, clamped input at 0x002BF094
	Real m_transitionProgress;	// +0x74, advanced by 0x002BEC1D
	Bool m_transitionActive;	// +0x78, cleared on completion at 0x002BEC3C
	Real m_transitionRate;	// +0x7C, reciprocal duration in 0x002BF047
	Real m_transitionOriginValue;	// +0x80, cached from slot 30 at 0x002BF025
	Real m_transitionDestinationValue;	// +0x84, interpolated with +0x80 at 0x002BECBA
	UnsignedByte m_unk0x88[0x98 - 0x88];
	UnsignedByte m_unk0x98[0x14];	// hash table, typed record pending
	UnsignedByte m_unk0xAC[0x14];	// hash table, typed record pending
};

typedef char LivingWorld_size[(sizeof(LivingWorld) == 0xC0) ? 1 : -1];

extern LivingWorld *TheLivingWorld;
