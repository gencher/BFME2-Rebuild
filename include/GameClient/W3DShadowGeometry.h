// Portions derived from Command & Conquer Generals and Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
// W3DShadowGeometry.h: the cleaned-up model geometry volumetric shadows extrude.
#pragma once
#ifndef BFME2_W3DSHADOWGEOMETRY_H
#define BFME2_W3DSHADOWGEOMETRY_H

#include <stddef.h>
#include "Common/GameType.h"
#include "Common/AsciiString.h"
#include "WWLib/RefCountClass.h"
#include "WWLib/HashableClass.h"

class MeshClass;
class RenderObjClass;
class Vector3;
class W3DShadowGeometry;

#define MAX_POLYGON_NEIGHBORS 3

struct NeighborEdge
{
	Short neighborIndex;					// the neighboring polygon
	Short neighborEdgeIndex[2];				// the two vertex indices of the shared edge
};

struct PolyNeighbor
{
	Short myIndex;							// our polygon index so we know who we are
	Byte status;							// status flags used when processing neighbors
	NeighborEdge neighbor[MAX_POLYGON_NEIGHBORS];
};

// One mesh of a shadow-casting model, with the polygon data the silhouette search needs.
// Fields the functions written so far do not read keep neutral names.
class W3DShadowGeometryMesh
{
	friend class W3DShadowGeometry;
	friend class W3DVolumetricShadow;

public:
	W3DShadowGeometryMesh();
	~W3DShadowGeometryMesh();

	void buildPolygonNeighbors();

protected:
	PolyNeighbor *GetPolyNeighbor(Int polyIndex);

	MeshClass *m_mesh;
	Int m_unk04;
	const Vector3 *m_verts;
	Int m_meshRobjIndex;					// index of this mesh within the hlod, -1 after construction
	Vector3 *m_polygonNormals;
	Int m_numVerts;
	Int m_numPolygons;
	Int m_unk1C;
	UnsignedShort *m_parentVerts;			// parent vertex index for each vertex
	PolyNeighbor *m_polyNeighbors;
	Int m_numPolyNeighbors;					// length of m_polyNeighbors and the number of polygons
	W3DShadowGeometry *m_parentGeometry;	// mesh hierarchy containing this mesh
	Bool m_unk30;
};

#define MAX_SHADOW_CASTER_MESHES 160

// The geometry of one model, shared by every shadow cast from it.
class W3DShadowGeometry : public RefCountClass, public HashableClass
{
public:
	W3DShadowGeometry();
	~W3DShadowGeometry();

	virtual const char *Get_Key();

	Int initFromHLOD(RenderObjClass *robj);		// initialize the geometry from a W3D HLOD object
	Int initFromMesh(RenderObjClass *robj, Int meshRobjIndex, W3DShadowGeometry *parent);

	Int getMeshCount() { return m_meshCount; }
	W3DShadowGeometryMesh *getMesh(Int index) { return &m_meshList[index]; }
	Int GetNumTotalVertex() { return m_numTotalsVerts; }	// total number of vertices in all meshes of this geometry

private:
	AsciiString m_name;										// name of the model hierarchy
	W3DShadowGeometryMesh m_meshList[MAX_SHADOW_CASTER_MESHES];
	Int m_meshCount;										// number of meshes in the hierarchy
	Int m_numTotalsVerts;									// number of verts in the entire hierarchy
};

typedef char PolyNeighborSize[(sizeof(PolyNeighbor) == 0x16) ? 1 : -1];
typedef char W3DShadowGeometryMeshSize[(sizeof(W3DShadowGeometryMesh) == 0x34) ? 1 : -1];

typedef char W3DShadowGeometrySize[(sizeof(W3DShadowGeometry) == 0x209C) ? 1 : -1];

#endif
