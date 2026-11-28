// Geometry.h: the geometry block embedded in Object at +0xA8. SIZE 0x5C.
// Target facts: clientUpdate reads Object+0xA8 (0x006BD7C0,
// getMaxHeightAbovePosition) directly off the drawable's object, and
// DockUpdate reads the major radius at +0x10 (Object+0xB8) where Zero Hour's
// DockUpdate calls getMajorRadius. Only the queries module code makes are
// declared.
#pragma once
#ifndef BFME2_GEOMETRY_H
#define BFME2_GEOMETRY_H

#include "Common/GameType.h"
#include "Common/AsciiString.h"
#include "Common/Coord3DBase.h"
#include <vector>

class INI;
class Coord3D;

// Zero Hour's GeometryType: the shape of one geometry record (retail's GeometryInfo::set takes it as W4GeometryType).
enum GeometryType
{
	GEOMETRY_SPHERE = 0,
	GEOMETRY_CYLINDER,
	GEOMETRY_BOX,
	GEOMETRY_NUM_TYPES
};

// One 0x24-byte shape record of a GeometryInfo (ActiveBody::setCorrectDamageState 0x004BE8BD compares the
// string at +0x1C with "Bookend"). Layout from the max-height queries 0x006BD7C0 and 0x006BD8A0 (type at +0, the
// height at +4 for a cylinder or box and the major radius at +8 for a sphere, the z offset at +0x18, the enable
// byte at +0x20), expandFootprint 0x006BEB40 (adds a radius to +8 and +0xC) and the INI parsers 0x006BEA00 ..
// 0x006BEAB0 (they store height, major radius and minor radius into the last shape).
class GeometryShape
{
public:
	GeometryType m_type;									// 0x00
	Real m_height;											// 0x04
	Real m_majorRadius;										// 0x08
	Real m_minorRadius;										// 0x0C
	Real m_offsetX;											// 0x10
	Real m_offsetY;											// 0x14
	Real m_offsetZ;											// 0x18
	AsciiString m_name;										// 0x1C
	char m_flag;											// 0x20 enabled (the max-height queries skip a shape without it); a char, as setFlag stores its char argument unconverted
	Bool m_usedForHealthBox;								// 0x21 (0x006BD830 additionally requires it)
	UnsignedByte m_unk0x22[2];

	// Zero Hour's GeometryInfo::getMaxHeightBelowPosition, per shape (0x006BD8A0 inlines it into its loop).
	Real getMaxHeightBelowPosition() const
	{
		switch (m_type)
		{
			case GEOMETRY_SPHERE:
				return m_majorRadius;

			case GEOMETRY_BOX:
			case GEOMETRY_CYLINDER:
				return 0.0f;
		}

		return 0.0f;
	}
};

class GeometryInfo
{
public:
	Real getMaxHeightAbovePosition() const;				// 0x006BD7C0
	// 0x006BEB00 (map ?bfmeOneCNG@GeometryInfo@@QAEXPAX@Z; Object::setGeometryInfoZ 0x0028ABFC passes the new height as a float): Zero Hour's name.
	void setMaxHeightAbovePosition(Real z);
	// 0x006BE160: whether the default shadow texture is the plural "shadows" (ThingTemplate::validate 0x0033B4CD).
	Bool usePluralShadowName() const;
	// 0x006BE4C0: the lowest and highest pitch from a point of this geometry to a point of the other
	// (Weapon::isWithinTargetPitch 0x002C9C82 compares them with the weapon's pitch limits).
	void calcPitches(const Coord3D &thisPos, const GeometryInfo &that, const Coord3D &thatPos, Real &minPitch, Real &maxPitch) const;
	// 0x006BEB80: whether this geometry at thisPos (with its height) overlaps the other at thatPos; the map spells
	// it ?CollidesWith@GeometryInfo@@QBE_NABVCoord3D@@MABV1@0M@Z. SpecialAbilityUpdate 0x0044F4F7 and 0x0044F28E call it.
	Bool CollidesWith(const Coord3D &thisPos, Real thisHeight, const GeometryInfo &that, const Coord3D &thatPos, Real thatHeight) const;
	GeometryInfo &operator=(const GeometryInfo &that);	// 0x00064605 (out of line in W3DGhostObject's unit)
	Real getMajorRadius() const { return m_majorRadius; }
	// 0x006BD470: copies m_center (+0x18) to the argument (SpecialAbilityUpdate::initLaser 0x0044FB28; the map still spells it ?get@GeometryInfo@@QAEXPAURva0087DC00Vec@@@Z).
	void getLocalCenter(Coord3DBase *center);
	// 0x006BD8A0: the largest depth below the position over the enabled shapes (a sphere's major radius, else 0).
	Real getMaxHeightBelowPosition() const;
	// 0x006BD830: getMaxHeightAbovePosition over the enabled shapes that are also used for the health box
	// (Drawable 0x00277640 area: its only caller, 0x0027765A; the map spells it ?rva006BD830@GeometryInfo@@QBEMXZ).
	Real getHealthBoxMaxHeightAbovePosition() const;
	// 0x006BD780: whether the line segment from..to passes within the bounding sphere radius (+0x14) of loc.
	Bool isIntersectedByLineSegment(const Coord3D &loc, const Coord3D &from, const Coord3D &to) const;
	// 0x006BEB40: grows the major and minor radius of every enabled shape by radius, then refreshes the bounds.
	void expandFootprint(Real radius);
	// Static INI field parsers (tokens of the Geometry block): the table's parse functions; each stores into the
	// object or its last shape and refreshes the bounds (retail 0x006BE9D0 .. 0x006BEAB0).
	static void parseGeometryActive(INI *ini, void *instance, void *store, const void *userData);		// 0x006BD590
	static void parseGeometryUsedForHealthBox(INI *ini, void *instance, void *store, const void *userData);	// 0x006BD5D0
	static void parseGeometryOffset(INI *ini, void *instance, void *store, const void *userData);		// 0x006BD610
	static void parseGeometryIsSmall(INI *ini, void *instance, void *store, const void *userData);		// 0x006BE9D0
	static void parseGeometryHeight(INI *ini, void *instance, void *store, const void *userData);		// 0x006BEA00
	static void parseGeometryMajorRadius(INI *ini, void *instance, void *store, const void *userData);	// 0x006BEA60
	static void parseGeometryMinorRadius(INI *ini, void *instance, void *store, const void *userData);	// 0x006BEAB0
	// 0x006BD980: the shape record at an index (the map still spells the class BfmeObjE15).
	GeometryShape *getGeometryShape(Int index);
	// 0x006BF450: sets every shape whose name differs from `name` to `active` (map ?rva0087FA50@BfmeObjF9@@..., ?:placeholder).
	void setShapesActiveExcept(const AsciiString &name, Bool active);
	// 0x006BF3C0: sets the named shape on or off (GateOpenAndCloseBehavior 0x0049924B, 0x004992D5; the map still spells the class BfmeObjF9).
	void setFlag(const AsciiString &name, char flag);
	Int getNumShapes() const { return (Int)m_shapes.size(); }

	UnsignedByte m_unk0x00[4];							// 0x00 vtable pointer (DoXfer 0x006C0030 is virtual)
	Bool m_isSmall;										// 0x04 (parseGeometryIsSmall)
	UnsignedByte m_unk0x05[0x10 - 5];
	Real m_majorRadius;									// 0x10 calcBoundingStuff 0x006BE700 starts it at 0.01 and raises it to each enabled shape's circle radius
	Real m_minorRadius;									// 0x14 (Object +0xBC): the bounding sphere radius; BattlePlanUpdate::createVisionObject 0x00497A4B passes it to setShroudClearingRange, isIntersectedByLineSegment 0x006BD780 tests against it
	Coord3DBase m_center;								// 0x18 the middle of the shapes' bounding box (calcBoundingStuff); calcPitches 0x006BE4C0 adds the position to it
	Real m_unk0x24;										// 0x24 (Object +0xCC) StructureToppleUpdate::applyCrushingDamage 0x004A5EAC (times Sin)
	Real m_unk0x28;										// 0x28 (Object +0xD0) same row (times Cos)
	// 0x2C..0x37: an STLport vector of the 0x24-byte shape records (begin +0x2C, end +0x30, capacity end +0x34; ActiveBody
	// 0x004BE8BD reads Object +0xD4). Evidence: the INI parsers 0x006BD590 / 0x006BEA60 take size() as (end - begin) / 36 and then
	// back() through a second load of end, which compiles to retail's bytes only with a real vector.
	_STL::vector<GeometryShape> m_shapes;
	UnsignedByte m_unk0x38[0x5C - 0x38];

	// Role names; applyCrushingDamage 0x004A5EAC, beginStructureTopple 0x004A5C8C. Retail copies each into
	// a stack temporary before the sin/cos call, which the inline accessor reproduces.
	Real getBoundsMajorRadius() const { return m_unk0x24; }
	Real getBoundsMinorRadius() const { return m_unk0x28; }

private:
	// 0x006BE700 (map ?calcBoundingStuff@GeometryInfo@@AAEXXZ): recomputes +0x10 .. +0x28 from the shapes.
	void calcBoundingStuff();
};
// SIZE 0x5C

#endif
