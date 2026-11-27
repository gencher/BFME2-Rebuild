// Portions derived from Command & Conquer Generals and Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
// W3DShadowGeometryMesh.h: per-mesh polygon data for volumetric shadow silhouettes.
#pragma once
#ifndef BFME2_W3DSHADOWGEOMETRYMESH_H
#define BFME2_W3DSHADOWGEOMETRYMESH_H

#include <stddef.h>
#include "Common/GameType.h"

class MeshClass;
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

typedef char PolyNeighborSize[(sizeof(PolyNeighbor) == 0x16) ? 1 : -1];
typedef char W3DShadowGeometryMeshSize[(sizeof(W3DShadowGeometryMesh) == 0x34) ? 1 : -1];

#endif
