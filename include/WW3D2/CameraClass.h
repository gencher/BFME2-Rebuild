// CameraClass.h: WW3D's camera render object (projection, viewport, clip
// planes and the cached world and view-space frustums).
//
// Target facts (camera.cpp, O00133A00):
// - vftable 0x007D26E0, 131 slots: RenderObjClass's 129 with 1 (deleting
//   destructor 0x00133EF0), 2 Clone 0x00135330, 3 Class_ID 0x0007C3F8 (returns
//   8), 12 Render (the empty `ret 4` 0x0047A69C), 21 Set_Transform 0x00133A90,
//   22 Set_Position 0x00133AB0, 67/68 the object-space bounds 0x00133A20 and
//   0x00133A50 replaced, then two of its own: 129 Cull_Sphere 0x00135960 and
//   130 Cull_Box 0x00135990. The secondary table 0x007D26D8 sits at +8
//   (RenderObjClass's MultiListObjectClass base).
// - The constructor 0x00134AB0 runs RenderObjClass's 0x0013BF00, then sets
//   +0xC4 to 0 (perspective), the viewport +0xC8..+0xD4 and view plane
//   +0xD8..+0xE4 to (0,0)-(1,1), +0xE8 to 4/3 (0x00BD28EC), the clip planes
//   +0xEC/+0xF0 to 1 and 1000, the z-buffer range +0xF4/+0xF8 to 0 and 1, the
//   byte +0xFC to 0, builds the planes of the two FrustumClass members at
//   +0x100 and +0x208 (planes at +0x130 and +0x238), sets an identity
//   transform and calls Set_View_Plane(50 degrees, -1).
// - The getters Get_Near_Clip_Bounding_Box 0x00135950, Get_Projection_Matrix
//   0x00134A60 and Get_View_Matrix 0x00134A70 call Update_Frustum 0x001340B0
//   first and return +0x310 (OBBoxClass), +0x34C (Matrix4x4) and +0x3CC
//   (Matrix3D); Set_Reflected_Projection_Matrix 0x00133DA0 copies a Matrix4x4 to
//   +0x38C. Clone allocates 0x3FC bytes. SIZE 0x3FC.
#pragma once
#ifndef BFME2_CAMERACLASS_H
#define BFME2_CAMERACLASS_H

#include "WW3D2/RenderObjClass.h"
#include "WWMath/Vector2.h"
#include "WWMath/Matrix4.h"
#include "WWMath/FrustumClass.h"
#include "WWMath/OBBoxClass.h"

// A rectangle in normalised or view-plane coordinates.
class ViewportClass
{
public:
	ViewportClass(void) : Min(0.0f, 0.0f), Max(1.0f, 1.0f) {}

	float Width(void) const { return Max.X - Min.X; }
	float Height(void) const { return Max.Y - Min.Y; }

	Vector2 Min;		// +0x0
	Vector2 Max;		// +0x8
};

class CameraClass : public RenderObjClass
{
public:
	enum ProjectionType
	{
		PERSPECTIVE = 0,
		ORTHO
	};

	enum ProjectionResType
	{
		INSIDE_FRUSTUM = 0,
		OUTSIDE_FRUSTUM,
		OUTSIDE_NEAR_CLIP,
		OUTSIDE_FAR_CLIP
	};

	CameraClass(void);										// 0x00134AB0
	CameraClass(const CameraClass &src);								// 0x00134C50
	CameraClass &operator=(const CameraClass &src);							// 0x00135010
	virtual ~CameraClass(void);									// 1: 0x00133A00

	virtual RenderObjClass *Clone(void) const;							// 2: 0x00135330
	virtual int Class_ID(void) const { return 8; }							// 3: 0x0007C3F8
	virtual void Render(RenderInfoClass &rinfo) {}							// 12: 0x0047A69C
	virtual void Set_Transform(const Matrix3D &m);							// 21: 0x00133A90
	virtual void Set_Position(const Vector3 &v);							// 22: 0x00133AB0
	virtual void Get_Obj_Space_Bounding_Sphere(SphereClass &sphere) const;				// 67: 0x00133A20
	virtual void Get_Obj_Space_Bounding_Box(AABoxClass &box) const;					// 68: 0x00133A50
	virtual bool Cull_Sphere(const SphereClass &sphere) const;					// 129: 0x00135960
	virtual bool Cull_Box(const AABoxClass &box) const;						// 130: 0x00135990

	const ViewportClass &Get_Viewport(void) const { return Viewport; }
	void Get_Viewport(Vector2 &set_min, Vector2 &set_max) const { set_min = Viewport.Min; set_max = Viewport.Max; }
	void Set_Projection_Type(ProjectionType type);				// 0x0007B261
	void Set_Viewport(const Vector2 &min, const Vector2 &max) { Viewport.Min = min; Viewport.Max = max; FrustumValid = false; }
	float Get_Depth(void) const { return ZFar; }						// W3DView::getPickRay 0x00089658 reads +0xF0 for the far-plane distance
	void Set_View_Plane(const Vector2 &min, const Vector2 &max);					// 0x00134020
	void Set_View_Plane(float hfov, float vfov);							// 0x00133AD0
	void Get_View_Plane(Vector2 &set_min, Vector2 &set_max) const;					// 0x00134080
	void Set_Aspect_Ratio(float width_to_height);							// 0x00133B70
	float Get_Aspect_Ratio(void) const;								// 0x00133D90
	void Set_Clip_Planes(float znear, float zfar);							// 0x00133D00
	void Get_Clip_Planes(float &znear, float &zfar) const;						// 0x00133D30
	float Get_Horizontal_FOV(void) const;								// 0x00133D50
	float Get_Vertical_FOV(void) const;								// 0x00133D70

	ProjectionResType Project(Vector3 &dest, const Vector3 &ws_point) const;			// 0x00135390
	ProjectionResType Project_Camera_Space_Point(Vector3 &dest, const Vector3 &cam_point) const;	// 0x001355E0
	void Un_Project(Vector3 &dest, const Vector2 &view_point) const;				// 0x00133BC0
	void Transform_To_View_Space(Vector3 &dest, const Vector3 &ws_point) const;			// 0x00135770
	void Rotate_To_View_Space(Vector3 &dest, const Vector3 &ws_vector) const;			// 0x00135870
	void Device_To_View_Space(const Vector2 &device_coord, Vector3 *view_coord);			// 0x00134650
	void Device_To_World_Space(const Vector2 &device_coord, Vector3 *world_coord);			// 0x00134730
	float Compute_Projected_Sphere_Radius(float dist, float radius);				// 0x00133E80

	const OBBoxClass &Get_Near_Clip_Bounding_Box(void) const;					// 0x00135950
	const FrustumClass &Get_Frustum(void) const;								// 0x0004CB06: the world-space frustum, refreshed first
	const Vector3 *Get_Frustum_Corners(void) const;							// 0x00140C50: its eight corners, refreshed first
	void Get_Projection_Matrix(Matrix4x4 *set_tm);							// 0x00134820
	void Get_D3D_Projection_Matrix(Matrix4x4 *set_tm);						// 0x001348C0
	void Get_D3D_View_Projection_Matrix(Matrix4x4 *set_tm);								// 0x00134A80 (the map's name: the D3D projection times the inverse camera transform)
	void Get_View_Matrix(Matrix3D *set_tm);								// 0x001349E0
	const Matrix4x4 &Get_Projection_Matrix(void);							// 0x00134A60
	const Matrix3D &Get_View_Matrix(void);								// 0x00134A70
	void Set_Reflected_Projection_Matrix(const Matrix4x4 &tm);					// 0x00133DA0
	void Apply(void);										// 0x00135A70

	static void Convert_Old(Vector3 &pos);								// 0x00133E40

protected:
	void Update_Frustum(void) const;								// 0x001340B0

	ProjectionType Projection;			// +0x0C4
	ViewportClass Viewport;				// +0x0C8
	ViewportClass ViewPlane;			// +0x0D8
	float AspectRatio;				// +0x0E8
	float ZNear;					// +0x0EC
	float ZFar;					// +0x0F0
	float ZBufferMin;				// +0x0F4
	float ZBufferMax;				// +0x0F8
	mutable bool FrustumValid;			// +0x0FC
	mutable FrustumClass Frustum;			// +0x100 world space
	mutable FrustumClass ViewSpaceFrustum;		// +0x208
	mutable OBBoxClass NearClipBBox;		// +0x310
	mutable Matrix4x4 ProjectionTransform;		// +0x34C
	Matrix4x4 ReflectedProjectionTransform;		// +0x38C
	mutable Matrix3D CameraInvTransform;		// +0x3CC
};

typedef char CameraClassSize[(sizeof(CameraClass) == 0x3FC) ? 1 : -1];

// Projection override switch read by CameraClass::Apply and the scene render.
extern bool bfmeCameraProjectionOverride;

#endif
