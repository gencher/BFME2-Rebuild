// Portions derived from Command & Conquer Generals and Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
// Portions derived from Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
// PlaneClass.h: WWMath's plane, a unit normal and its distance from the origin.
// Target facts: the comdat default constructor 0x000F0E6C stores 0, 0, 1.0f
// (0x00BBB8D8) into +0x0..+0x8 and 0 into +0xC; CollisionMath::Overlap_Test(
// const FrustumClass &, const Vector3 &) 0x00719940 dots the point with the
// normal at +0x0..+0x8 and subtracts the distance at +0xC. SIZE 0x10.
#pragma once
#ifndef BFME2_PLANECLASS_H
#define BFME2_PLANECLASS_H

#include "WWMath/Vector3.h"

class PlaneClass
{
public:
	PlaneClass(void) : N(0.0f, 0.0f, 1.0f), D(0.0f) {}
	// The plane with the given normal and distance (Zero Hour's plane.h).
	PlaneClass(const Vector3 &normal, float dist) { N = normal; D = dist; }
	// The plane through a point with the given normal (Zero Hour's plane.h: the constructor forwards to Set).
	PlaneClass(const Vector3 &normal, const Vector3 &point) { Set(normal, point); }
	// The plane through three points (Zero Hour's plane.h: the constructor forwards to the three-point Set).
	// Bridge::getBridgeHeight 0x0027FD2A calls Set 0x0007BFC2 for it.
	PlaneClass(const Vector3 &point1, const Vector3 &point2, const Vector3 &point3) { Set(point1, point2, point3); }
	inline void Set(const Vector3 &normal, const Vector3 &point)
	{
		N = normal;
		D = Vector3::Dot_Product(normal, point);
	}

	// The plane through three points (unit normal of (p2-p1) x (p3-p1), or the
	// default plane when they are collinear). Out of line: 0x0007BFC2 is called
	// by MeshGeometryClass::Compute_Plane 0x0016ACC0 and FrustumClass::Init.
	void Set(const Vector3 &point1, const Vector3 &point2, const Vector3 &point3);	// 0x0007BFC2

	// Ray test against the segment p0..p1 (Zero Hour's plane.h inline). Retail keeps one out-of-line copy,
	// 0x00069168 in BaseHeightMap's unit, and Bridge::getBridgeHeight 0x0027FD2A calls it.
	bool Compute_Intersection(const Vector3 &p0, const Vector3 &p1, float *set_t) const;	// 0x00069168

	Vector3 N;		// +0x00
	Real D;			// +0x0C
};

typedef char PlaneClassSize[(sizeof(PlaneClass) == 0x10) ? 1 : -1];

#endif
