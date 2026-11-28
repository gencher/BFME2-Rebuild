// Portions derived from Command & Conquer Generals and Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
// WorldHeightMap.h: terrain height grid and cell flip-state storage.
#pragma once
#ifndef BFME2_WORLDHEIGHTMAP_H
#define BFME2_WORLDHEIGHTMAP_H

#include <stddef.h>
#include <vector>
#include "Common/GameType.h"
#include "Common/ICoord2D.h"
#include "WWLib/RefCountClass.h"

// World units per height-map cell.
#define MAP_XY_FACTOR (10.0f)

struct DataChunkInfo;
class DataChunkInput;

class WorldHeightMap : public RefCountClass
{
public:
	WorldHeightMap();
	virtual ~WorldHeightMap();
	virtual bool parseBlendTileData(DataChunkInput &file, DataChunkInfo *info);

	UnsignedShort getHeight(Int xIndex, Int yIndex) const;
	void setHeight(Int xIndex, Int yIndex, UnsignedShort height);
	Bool getFlipState(Int xIndex, Int yIndex) const;

	Int getXExtent() const { return m_width; }
	Int getYExtent() const { return m_height; }
	Int getBorderSize() const { return m_borderSize; }
	Int getDataSize() const { return m_dataSize; }
	UnsignedShort *getDataPtr() const { return m_data; }

	Int m_width;
	Int m_height;
	Int m_borderSize;
	std::vector<ICoord2D> m_boundaries;
	Int m_dataSize;
	UnsignedShort *m_data;
	std::vector<UnsignedByte> m_cellFlipState;
	Int m_flipStateWidth;
};

typedef char WorldHeightMapWidthOffset[(offsetof(WorldHeightMap, m_width) == 0x08) ? 1 : -1];
typedef char WorldHeightMapBorderOffset[(offsetof(WorldHeightMap, m_borderSize) == 0x10) ? 1 : -1];
typedef char WorldHeightMapDataSizeOffset[(offsetof(WorldHeightMap, m_dataSize) == 0x20) ? 1 : -1];
typedef char WorldHeightMapDataOffset[(offsetof(WorldHeightMap, m_data) == 0x24) ? 1 : -1];
typedef char WorldHeightMapFlipWidthOffset[(offsetof(WorldHeightMap, m_flipStateWidth) == 0x34) ? 1 : -1];

#endif
