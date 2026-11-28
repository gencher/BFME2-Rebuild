// Portions derived from Command & Conquer Generals and Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
// Portions derived from Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
// AABoxClass: WWMath's axis-aligned box, a centre and a half-size extent.
//
// Target facts: CullableClass's constructor 0x007244C0 writes six floats
// starting at +0xC — three zeroed (centre) then three copies of the 1.0f at
// 0x00BBB8D8 (extent) — so the box is 0x18 bytes with Center at +0 and Extent
// at +0xC. Set_Cull_Box 0x007245A0 assigns the whole thing at once.
#pragma once
#ifndef BFME2_AABOXCLASS_H
#define BFME2_AABOXCLASS_H

#include "WWMath/Vector3.h"
#include "WWMath/Matrix3D.h"

class Matrix3D;
class MinMaxAABoxClass;

class AABoxClass
{
public:
	AABoxClass(void) {}
	AABoxClass(Vector3 *points, int num);

	// 0x0071AFA0: whether a point lies inside the box (gridcull.cpp's leaf test).
	bool Contains(const Vector3 &point) const;
	AABoxClass(const Vector3 &center, const Vector3 &extent) : Center(center), Extent(extent) {}

	void Init(const Vector3 &center, const Vector3 &extent)
	{
		Center = center;
		Extent = extent;
	}

	void Init(const MinMaxAABoxClass &minmaxbox);
	void Init_Random(float min_center, float max_center, float min_extent, float max_extent);	// 0x00719730
	static void Transform(const Matrix3D &tm, const AABoxClass &in, AABoxClass *out);		// 0x007197B0
	// Zero Hour aabox.h: MeshClass::Update_Cached_Bounding_Volumes 0x0014B650 copies the centre and
	// extent before Matrix3D::Transform_Center_Extent_AABox 0x00712CA0.
	__forceinline void Transform(const Matrix3D & tm)
	{
		Vector3 oldcenter = Center;
		Vector3 oldextent = Extent;
		tm.Transform_Center_Extent_AABox(oldcenter,oldextent,&Center,&Extent);
	}

	Vector3 Center;		// +0x00
	Vector3 Extent;		// +0x0C
};

// A box kept as its two corners.
class MinMaxAABoxClass
{
public:
	MinMaxAABoxClass(void) {}
	__forceinline MinMaxAABoxClass(const Vector3 &min_corner, const Vector3 &max_corner) : MinCorner(min_corner), MaxCorner(max_corner) {}
	void Init(const AABoxClass &box)
	{
		MinCorner = box.Center - box.Extent;
		MaxCorner = box.Center + box.Extent;
	}
	void Add_Box(const AABoxClass &box)
	{
		if (box.Extent == Vector3(0.0f, 0.0f, 0.0f)) return;

		MinCorner.Update_Min(box.Center - box.Extent);
		MaxCorner.Update_Max(box.Center + box.Extent);
	}
	void Init_Empty(void);		// 0x007197D0

	Vector3 MinCorner;		// +0x00
	Vector3 MaxCorner;		// +0x0C
};

inline void AABoxClass::Init(const MinMaxAABoxClass &minmaxbox)
{
	Center = (minmaxbox.MaxCorner + minmaxbox.MinCorner) * 0.5f;
	Extent = (minmaxbox.MaxCorner - minmaxbox.MinCorner) * 0.5f;
}

// SIZE 0x18 (RenderObjClass embeds one at +0x5C; its copy constructor moves six dwords)
typedef char AABoxClassSize[(sizeof(AABoxClass) == 0x18) ? 1 : -1];

#endif
