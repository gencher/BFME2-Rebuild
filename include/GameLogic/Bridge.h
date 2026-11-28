// Bridge.h: one bridge of the map (TerrainLogic's bridge list) and its BridgeInfo.
// Target facts (code RVAs): the list is walked through +0x04 (Pathfinder::IsPointOnRamp 0x002E757D,
// TerrainLogic::findBridgeAt 0x0027F30A). TerrainLogic::isBridgeBroken 0x0027D447 and isBridgeRepaired
// 0x0027D40B compare the object's ID with the Int at +0x60, read the byte at +0x74 and the state at +0x5C
// (BODY_RUBBLE is 3); GameLogic::prepareLogicForObjectLoad 0x00242C86 reads the ID at +0x60 and four tower
// IDs from +0x64 (the BridgeInfo, which begins at +0x0C: from, to, then the width/index words, the damage
// state at BridgeInfo +0x50, the object ID at +0x54, the four tower IDs at +0x58, the changed flag at +0x68);
// TerrainLogic::findBridgeLayerAt 0x0027F337 compares the Int at +0xC4 with the layer. updateDamageState is
// 0x00281C13 (TerrainLogic::updateBridgeDamageStates 0x00281EA5 calls it on every bridge), isPointOnBridge
// 0x0027EEAD, the virtual destructor 0x00280C05. Names are Zero Hour's; the allocation size and the members
// that no measured row reads stay unnamed spans.
#pragma once
#ifndef BFME2_BRIDGE_H
#define BFME2_BRIDGE_H

#include <stddef.h>
#include "Common/GameType.h"
#include "Common/Coord3D.h"
#include "Common/AsciiString.h"

enum PathfindLayerEnum;

class BridgeInfo
{
public:
	BridgeInfo();	// 0x0027C36A (the map spells the class Rva0027C36A)
	BridgeInfo &operator=(const BridgeInfo &that);	// 0x00085420
	Coord3D from;						// +0x00 (Bridge +0x0C)
	Coord3D to;							// +0x0C
	Real bridgeWidth;					// +0x18 (Bridge +0x24)
	Coord3D fromLeft;					// +0x1C (Bridge +0x28): the corner points isPointOnBridge 0x0027EEAD copies out
	Coord3D fromRight;					// +0x28
	Coord3D toLeft;						// +0x34
	Coord3D toRight;					// +0x40
	Int bridgeIndex;					// +0x4C
	Int curDamageState;					// +0x50 (Bridge +0x5C) a BodyDamageType
	ObjectID bridgeObjectID;			// +0x54 (Bridge +0x60)
	ObjectID towerObjectID[4];			// +0x58 (Bridge +0x64): BRIDGE_MAX_TOWERS
	Bool damageStateChanged;			// +0x68 (Bridge +0x74)
	UnsignedByte m_unk0x69[3];			// padding to the dword
	UnsignedByte m_unk0x6C[0x3C];		// +0x6C (Bridge +0x78 .. +0xB3): the rest of the 0xA8-byte record handleObjectsOnBridgeOnDie 0x00457577 copies through BridgeInfo's out-of-line assignment 0x00085420
};
typedef char BridgeInfo_off_toRight[(offsetof(BridgeInfo, toRight) == 0x40) ? 1 : -1];
typedef char BridgeInfo_off_curDamageState[(offsetof(BridgeInfo, curDamageState) == 0x50) ? 1 : -1];
typedef char BridgeInfo_off_bridgeObjectID[(offsetof(BridgeInfo, bridgeObjectID) == 0x54) ? 1 : -1];
typedef char BridgeInfo_off_damageStateChanged[(offsetof(BridgeInfo, damageStateChanged) == 0x68) ? 1 : -1];
typedef char BridgeInfo_size[(sizeof(BridgeInfo) == 0xA8) ? 1 : -1];

class Drawable;
class Object;
struct Region2D;
class PolygonTrigger;
class Vector3;
class Bridge
{
public:
	Bridge(Object *bridgeObj);					// 0x00280C21 (a landmark bridge: the object is the bridge)
	virtual ~Bridge();							// slot 0, 0x00280C05 (deleting)
	Bool isPointOnBridge(const Coord3D *pos);
	Drawable *getDrawable();
	void updateDamageState();
	Bool isCellOnEnd(const Region2D *cell);	// 0x0027C79C
	Real getBridgeHeight(const Coord3D *pos, Coord3D *normal);	// 0x0027FD2A

	Bridge *getNext() const { return m_next; }
	void setNext(Bridge *next) { m_next = next; }
	const BridgeInfo *peekBridgeInfo() const { return &m_bridgeInfo; }
	PathfindLayerEnum getLayer() const { return m_layer; }
	void setLayer(PathfindLayerEnum layer) { m_layer = layer; }
	AsciiString getBridgeTemplateName() const;	// 0x000AF1DD (27 B: the template name at +0x08; the map spells it Anim2DTemplate::getName)
	void setBridgeObjectID(ObjectID id) { m_bridgeInfo.bridgeObjectID = id; }
	void setTowerObjectID(ObjectID id, Int which) { m_bridgeInfo.towerObjectID[which] = id; }

	Bridge *m_next;								// +0x04
	unsigned char m_unk0x08[4];					// +0x08
	BridgeInfo m_bridgeInfo;					// +0x0C
	Real m_boundsLoX;							// +0xB4 the 2D bounds isPointOnBridge tests first (lo x, lo y, hi x, hi y)
	Real m_boundsLoY;							// +0xB8
	Real m_boundsHiX;							// +0xBC
	Real m_boundsHiY;							// +0xC0
	PathfindLayerEnum m_layer;					// +0xC4
	PolygonTrigger *m_polygonTrigger;			// +0xC8 isPointOnBridge 0x0027EEAD asks it first when it is set (a bridge placed by a polygon trigger)
};
typedef char Bridge_off_m_next[(offsetof(Bridge, m_next) == 0x04) ? 1 : -1];
typedef char Bridge_off_m_bridgeInfo[(offsetof(Bridge, m_bridgeInfo) == 0x0C) ? 1 : -1];
typedef char Bridge_off_m_boundsLoX[(offsetof(Bridge, m_boundsLoX) == 0xB4) ? 1 : -1];
typedef char Bridge_off_m_layer[(offsetof(Bridge, m_layer) == 0xC4) ? 1 : -1];
typedef char Bridge_off_m_polygonTrigger[(offsetof(Bridge, m_polygonTrigger) == 0xC8) ? 1 : -1];
// 0x0027EAFC: the 2D point-in-triangle test (Zero Hour's Point_In_Triangle_2D); the last argument receives the edge flags.
Bool Point_In_Triangle_2D(const Vector3 &a, const Vector3 &b, const Vector3 &c, const Vector3 &point, Int axis0, Int axis1, UnsignedByte &flags);

#endif
