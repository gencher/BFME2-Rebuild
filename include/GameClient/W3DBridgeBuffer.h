// Portions derived from Command & Conquer Generals and Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
// W3DBridgeBuffer.h: the draw buffer for all the bridges in a scene (Zero Hour's W3DBridgeBuffer.h, John Ahlquist).
// Target facts (retail game.dat): W3DBridge is 0x114 bytes: start +0, end +0xC, two floats +0x18/+0x1C the constructor
// 0x000DD97F sets to the same constant (the first is Zero Hour's m_scale), bridge type +0x20, bounds sphere +0x24, the texture
// handle +0x34 (a TextureClass value: ~W3DBridge 0x000DDA6E releases its factory), the left mesh +0x38 and transform +0x3C,
// four floats +0x6C..+0x78, the section mesh +0x7C and transform +0x80, two floats +0xB0/+0xB4, the right mesh +0xB8 and
// transform +0xBC, two floats +0xEC/+0xF0, first index +0xF4, vertex count +0xF8, first vertex +0xFC, polygon count +0x100
// (renderBridge 0x000DDAB7 pushes them in that order for Draw_Triangles), visible +0x104, template name +0x108, damage state
// +0x10C, enabled +0x110. W3DBridgeBuffer: vertex buffer +0, index buffer +4, vertex count +8, index count +0xC, 200 bridges at
// +0x10 (stride 0x114), bridge count +0xD7B0, initialized +0xD7B4 (clearAllBridges 0x000DD948, constructor 0x000DE52E).
#pragma once
#ifndef BFME2_W3DBRIDGEBUFFER_H
#define BFME2_W3DBRIDGEBUFFER_H

#include "Common/AsciiString.h"
#include "Common/GameType.h"
#include "WW3D2/MeshClass.h"
#include "WW3D2/MeshModelClass.h"
#include "WW3D2/TextureBaseClass.h"
#include "WWMath/Matrix3D.h"
#include "WWMath/SphereClass.h"
#include "WWMath/Vector3.h"

class CameraClass;
class W3DTerrainLogic;
class DX8IndexBufferClass;
class BfmeDynamicNativeVB;

typedef enum {
	FIXED_BRIDGE = 0,
	SECTIONAL_BRIDGE = 1
} TBridgeType;

/// The individual data for a bridge.
class W3DBridge
{
protected:
	Vector3 m_start;						// +0x00 drawing location
	Vector3 m_end;							// +0x0C drawing location
	Real m_scale;							// +0x18 width scale
	Real m_scaleUnk1C;						// +0x1C set with m_scale by the constructor; role unproven
	TBridgeType m_bridgeType;				// +0x20
	SphereClass m_bounds;					// +0x24 bounding sphere for culling
	TextureClass m_bridgeTexture;			// +0x34
	MeshClass *m_leftMesh;					// +0x38
	Matrix3D m_leftMtx;						// +0x3C
	Real m_minY;							// +0x6C
	Real m_maxY;							// +0x70
	Real m_leftMinX;						// +0x74
	Real m_leftMaxX;						// +0x78
	MeshClass *m_sectionMesh;				// +0x7C
	Matrix3D m_sectionMtx;					// +0x80
	Real m_sectionMinX;						// +0xB0
	Real m_sectionMaxX;						// +0xB4
	MeshClass *m_rightMesh;					// +0xB8
	Matrix3D m_rightMtx;					// +0xBC
	Real m_rightMinX;						// +0xEC
	Real m_rightMaxX;						// +0xF0
	Int m_firstIndex;						// +0xF4
	Int m_numVertex;						// +0xF8
	Int m_firstVertex;						// +0xFC
	Int m_numPolygons;						// +0x100
	Bool m_visible;							// +0x104
	AsciiString m_templateName;				// +0x108
	BodyDamageType m_curDamageState;		// +0x10C
	Bool m_enabled;							// +0x110

protected:
	Int getModelIndices(UnsignedShort *destination_ib, Int curIndex, Int vertexOffset, MeshClass *pMesh);

public:
	W3DBridge(void);														// 0x000DD97F
	~W3DBridge(void);
	void init(Vector3 fromLoc, Vector3 toLoc, AsciiString name);
	Bool cullBridge(CameraClass *camera);
	void clearBridge(void);
};

/// Draw buffer for the bridges.
class W3DBridgeBuffer
{
public:
	W3DBridgeBuffer(void);													// 0x000DE52E
	~W3DBridgeBuffer(void);
	void clearAllBridges(void);
	void loadBridges(W3DTerrainLogic *terrainLogic, Bool saveGame);
	enum { MAX_BRIDGE_VERTEX = 8000,MAX_BRIDGE_INDEX = 2 * MAX_BRIDGE_VERTEX, MAX_BRIDGES = 200 };

protected:
	BfmeDynamicNativeVB *m_vertexBridge;	// +0x00
	DX8IndexBufferClass *m_indexBridge;		// +0x04
	Int m_curNumBridgeVertices;				// +0x08
	Int m_curNumBridgeIndices;				// +0x0C
	W3DBridge m_bridges[MAX_BRIDGES];		// +0x10
	Int m_numBridges;						// +0xD7B0
	Bool m_initialized;						// +0xD7B4

	void allocateBridgeBuffers(void);
	void freeBridgeBuffers(void);			// 0x0074011F
};

#endif
