// SceneClass.h: WW3D's abstract scene (a ref-counted container of render
// objects with ambient light and fog settings) and its iterator interface.
//
// Target facts:
// - vftable 0x007D3350, 26 slots: 0 Delete_This 0x0006CE57 (RefCountClass's),
//   1 deleting destructor 0x00184150 (calls the shared one-store destructor
//   0x00724510 that restores RefCountClass's table), 2/3 Add/Remove_Render_Object
//   0x00140CD0/0x00140CE0 (tail calls into RenderObjClass slots 26/27,
//   Notify_Added/Notify_Removed), 4/5 pure (SimpleSceneClass's are
//   Create_Iterator 0x00141CD0 and the "delete it if non-null" body 0x006132A0),
//   6..13 the ambient/fog accessors (0x0006E265 copies three words to +8,
//   0x005C4AD5 returns this+8, 0x0006EDE3/0x004C54EC store/load the byte at
//   +0x1C, 0x00049F0F/0x004F5FA3 the Vector3 at +0x20, 0x0006EDED/0x0006EE06 the
//   two floats at +0x2C/+0x30), 14/15 pure (Register/Unregister in
//   SimpleSceneClass), 16 Compute_Point_Visibility 0x00140CC0 (returns 1.0f),
//   17..19 pure, 20/21 Save/Load 0x00140CF0/0x00140DC0, 22 Render 0x00141170,
//   23 pure (Customized_Render), 24/25 the empty `ret 4` bodies 0x0047A69C and
//   0x00180FD0. Render calls slot 24 first and slot 25 last, with slot 23 in
//   between, so 24 is the pre-render hook and 25 the post-render one.
// - The constructor 0x00140C60 sets NumRefs 1, then +8..+0x10 to 0.5f
//   (0x00BC26F0), +0x14 to 2, +0x18 to 0, the byte +0x1C to 0, +0x20..+0x2C to
//   0 and +0x30 to 1000.0f (0x00BBE358). SIZE 0x34.
// - Save 0x00140CF0 writes one chunk 0x42300 with micro chunks 0..5: +8 (12
//   bytes), +0x14 (4), +0x20 (12), +0x1C (1), +0x2C (4), +0x30 (4); Load reads
//   them back by micro-chunk id.
// Slots 17..19 have no recovered names: SimpleSceneClass's overrides return the
// int at its +0x104 (17) and forward to its spatial index at +0x34, whose
// methods take a two-corner box (18, rows 0x00141F90) and a subdivision level
// (19, BfmeSceneVector::Set_Level 0x00142060). The names below are role names.
#pragma once
#ifndef BFME2_SCENECLASS_H
#define BFME2_SCENECLASS_H

#include "WWLib/RefCountClass.h"
#include "WWLib/multilist.h"
#include "WWMath/Vector3.h"

class RenderObjClass;
class RenderInfoClass;
class ChunkSaveClass;
class ChunkLoadClass;
class MinMaxAABoxClass;
class CameraClass;

// A scene's cursor over its render objects. Its own vftable 0x007D3338 is the
// deleting destructor 0x00140C00 and four pure slots; SimpleSceneIterator's
// 0x007D33BC fills them with First/Next/Is_Done/Current_Item.
class SceneIterator
{
public:
	virtual ~SceneIterator(void) {}
	virtual void First(void) = 0;
	virtual void Next(void) = 0;
	virtual bool Is_Done(void) = 0;
	virtual RenderObjClass *Current_Item(void) = 0;
};

class SceneClass : public RefCountClass
{
public:
	// Register's switch (SimpleSceneClass 0x00141400) has five cases.
	enum RegType
	{
		ON_FRAME_UPDATE = 0,
		LIGHT,
		RELEASE,
		OBJECT_LIST,		// 3, 4: BFME additions, role names (SimpleSceneClass's
		FORCE_VISIBLE_LIST	// non-ref lists at +0xBC and +0xD4)
	};

	// The constructor's 2 at +0x14 is the filled-polygon mode.
	enum PolyRenderType
	{
		POINT = 0,
		LINE,
		FILL
	};

	// Values of ExtraPassPolyRenderMode.
	enum ExtraPassPolyRenderType
	{
		EXTRA_PASS_DISABLE,
		EXTRA_PASS_LINE,
		EXTRA_PASS_CLEAR_LINE
	};

	SceneClass(void);											// 0x00140C60
	virtual ~SceneClass(void);

	virtual void Add_Render_Object(RenderObjClass *obj);						// 2: 0x00140CD0
	virtual void Remove_Render_Object(RenderObjClass *obj);					// 3: 0x00140CE0

	virtual SceneIterator *Create_Iterator(void) = 0;						// 4
	virtual void Destroy_Iterator(SceneIterator *it) = 0;					// 5

	virtual void Set_Ambient_Light(const Vector3 &color) { AmbientLight = color; }		// 6: 0x0006E265
	virtual const Vector3 &Get_Ambient_Light(void) { return AmbientLight; }			// 7: 0x005C4AD5
	virtual void Set_Fog_Enable(bool set) { FogEnabled = set; }					// 8: 0x0006EDE3
	virtual bool Get_Fog_Enable(void) { return FogEnabled; }					// 9: 0x004C54EC
	virtual void Set_Fog_Color(const Vector3 &color) { FogColor = color; }			// 10: 0x00049F0F
	virtual const Vector3 &Get_Fog_Color(void) { return FogColor; }				// 11: 0x004F5FA3
	virtual void Set_Fog_Range(float start, float end) { FogStart = start; FogEnd = end; }	// 12: 0x0006EDED
	virtual void Get_Fog_Range(float *start, float *end);	// 13: 0x0006EE06

	virtual void Register(RenderObjClass *obj, RegType for_what) = 0;				// 14
	virtual void Unregister(RenderObjClass *obj, RegType for_what) = 0;			// 15

	virtual float Compute_Point_Visibility(RenderInfoClass &rinfo, const Vector3 &point);	// 16: 0x00140CC0

	virtual int Get_Spatial_Setting(void) const = 0;						// 17 (role name)
	virtual void Set_Spatial_Bounds(const MinMaxAABoxClass &bounds) = 0;			// 18 (role name)
	virtual void Set_Spatial_Level(unsigned int level) = 0;					// 19 (role name)

	virtual void Save(ChunkSaveClass &csave);							// 20: 0x00140CF0
	virtual void Load(ChunkLoadClass &cload);							// 21: 0x00140DC0

protected:
	virtual void Render(RenderInfoClass &rinfo);						// 22: 0x00141170
	virtual void Customized_Render(RenderInfoClass &rinfo) = 0;				// 23
	virtual void Pre_Render_Processing(RenderInfoClass &rinfo) {}				// 24: 0x0047A69C
	virtual void Post_Render_Processing(RenderInfoClass &rinfo) {}				// 25: 0x00180FD0

	Vector3 AmbientLight;			// +0x08
	int PolyRenderMode;			// +0x14: a PolyRenderType
	int ExtraPassPolyRenderMode;		// +0x18
	bool FogEnabled;			// +0x1C
	Vector3 FogColor;			// +0x20
	float FogStart;				// +0x2C
	float FogEnd;				// +0x30
};

typedef char SceneClassSize[(sizeof(SceneClass) == 0x34) ? 1 : -1];

// The scene's uniform grid over the map (BFME 2's addition to SimpleSceneClass).
// Target facts: SimpleSceneClass keeps one at +0x34 and its destructor frees
// the cell array at index +0x18 with the vector destructor (element 0x1C, each
// cell's destructor 0x00141CA0 runs MultiListClass<RenderObjClass>'s on cell+4).
// The bounds setter 0x00141F90 copies six floats to +0..+0x14 and stores
// 1/max(width, height) at +0x20; Set_Level 0x00142060 keeps 1 << level at +0x24
// (levels 0..10); Insert 0x001415B0 walks the cells from +0x18 with the count
// at +0x1C quartered per level, bumping each cell's count, then links the
// object into the leaf's list and stores the cell code in the object.
// The map spells this class BFME2SceneSpatialIndex for Insert/Remove and
// BfmeSceneVector for the bounds setter and Set_Level (one object, one class).
class BFME2SceneSpatialIndex
{
public:
	struct CellClass
	{
		int Count;					// +0x0 objects at or below this cell
		MultiListClass<RenderObjClass> Objects;		// +0x4
	};

	// Inline: ~SimpleSceneClass 0x00141DF0 ends by freeing the cell array
	// (vector destructor with the cell destructor 0x00141CA0, then operator delete[]).
	~BFME2SceneSpatialIndex() { delete[] Cells; }

	void Insert(RenderObjClass *obj, int x, int y, int z);		// 0x001415B0
	void Remove(RenderObjClass *obj);				// 0x00141640
	void Set_Bounds(const MinMaxAABoxClass &bounds);		// 0x00141F90
	void Set_Level(unsigned int level);				// 0x00142060

	Vector3 MinCorner;		// +0x00
	Vector3 MaxCorner;		// +0x0C
	CellClass *Cells;		// +0x18
	unsigned int CellCount;		// +0x1C (unsigned: Insert/Remove shift it with shr)
	float InvSize;			// +0x20
	unsigned int Resolution;	// +0x24 cells per side, 1 << level (unsigned)
};

typedef char BFME2SceneSpatialIndexSize[(sizeof(BFME2SceneSpatialIndex) == 0x28) ? 1 : -1];

class SimpleSceneClass;

// SimpleSceneClass's iterator: vftable 0x007D33BC, the scene's render list and
// the current node (constructor 0x00141BF0 stores the list, then its first node).
class SimpleSceneIterator : public SceneIterator
{
public:
	virtual void First(void);				// 0x00140E70
	virtual void Next(void);				// 0x00140E80
	virtual bool Is_Done(void);			// 0x00140E90
	virtual RenderObjClass *Current_Item(void);	// 0x00141590

protected:
	SimpleSceneIterator(RefMultiListClass<RenderObjClass> *list) : RobjIterator(list) {}	// 0x00141BF0

	RefMultiListIterator<RenderObjClass> RobjIterator;	// +0x4

	friend class SimpleSceneClass;
};

typedef char SimpleSceneIteratorSize[(sizeof(SimpleSceneIterator) == 0xC) ? 1 : -1];

// Target facts: vftable 0x007D33D0, 28 slots: SceneClass's 26 with 2..5, 14..19,
// 23 and 25 replaced, then Remove_All_Render_Objects 0x00141370 and
// Visibility_Check 0x001426D0. Layout from the destructor 0x00141DF0, which
// empties +0x5C through slot 3, then destroys +0xEC, +0xD4, +0xBC, +0xA4, +0x8C,
// +0x74, +0x5C and frees the grid's cells: RefMultiListClass destructor
// 0x0006F29B for +0x5C/+0x74/+0x8C/+0xA4/+0xEC, MultiListClass 0x00141780 for
// +0xBC/+0xD4. Register 0x00141400 maps RegType 0..4 to +0x74 (Add), +0x8C
// (Add_Tail), +0xA4, +0xBC, +0xD4; slot 17 0x0006EE1B returns +0x104.
class SimpleSceneClass : public SceneClass
{
public:
	virtual ~SimpleSceneClass(void);								// 0x00141DF0

	virtual void Add_Render_Object(RenderObjClass *obj);					// 2: 0x001413A0
	virtual void Remove_Render_Object(RenderObjClass *obj);					// 3: 0x00141860

	virtual SceneIterator *Create_Iterator(void);						// 4: 0x00141CD0
	virtual void Destroy_Iterator(SceneIterator *it) { ::delete it; }			// 5: 0x006132A0

	virtual void Register(RenderObjClass *obj, RegType for_what);				// 14: 0x00141400
	virtual void Unregister(RenderObjClass *obj, RegType for_what);				// 15: 0x001414D0

	virtual float Compute_Point_Visibility(RenderInfoClass &rinfo, const Vector3 &point);	// 16: 0x001418F0

	virtual int Get_Spatial_Setting(void) const { return SpatialSetting; }			// 17: 0x0006EE1B
	virtual void Set_Spatial_Bounds(const MinMaxAABoxClass &bounds) { SpatialIndex.Set_Bounds(bounds); }	// 18: 0x001428E0
	virtual void Set_Spatial_Level(unsigned int level) { SpatialIndex.Set_Level(level); }	// 19: 0x001428F0

	virtual void Remove_All_Render_Objects(void);						// 26: 0x00141370
	virtual void Visibility_Check(CameraClass *camera);					// 27: 0x001426D0

protected:
	virtual void Customized_Render(RenderInfoClass &rinfo);					// 23: 0x00141A30
	virtual void Post_Render_Processing(RenderInfoClass &rinfo);				// 25: 0x00141BB0

	BFME2SceneSpatialIndex SpatialIndex;				// +0x34
	RefMultiListClass<RenderObjClass> RenderList;			// +0x5C every added object
	RefMultiListClass<RenderObjClass> UpdateList;			// +0x74 ON_FRAME_UPDATE
	RefMultiListClass<RenderObjClass> LightList;			// +0x8C LIGHT
	RefMultiListClass<RenderObjClass> ReleaseList;			// +0xA4 RELEASE
	MultiListClass<RenderObjClass> ObjectList;			// +0xBC every added object, unreferenced
	MultiListClass<RenderObjClass> ForceVisibleList;		// +0xD4 added objects that are Is_Force_Visible
	RefMultiListClass<RenderObjClass> TrackedList;			// +0xEC role name; filled by derived scenes
	int SpatialSetting;						// +0x104 role name
};

#endif
