// MapUtil.h: the map-list and start-position helpers (GameClient/MapUtil). Only the functions a
// written body calls are declared; signatures are the map's decorated names.
#pragma once
#ifndef BFME2_MAPUTIL_H
#define BFME2_MAPUTIL_H

#include "Common/GameType.h"
#include "Common/AsciiString.h"

class GameInfo;
class GameWindow;
class MapMetaData;

void positionStartSpots(AsciiString mapName, GameWindow **buttonMapStartPositions, GameWindow *mapWindow, GameWindow *extra);
void positionStartSpots(GameInfo *myGame, GameWindow **buttonMapStartPositions, GameWindow *mapWindow, GameWindow *extra);

Bool Rva00300E42Check(GameInfo *game);										// map-transfer eligibility check

// Orders the map list by player count, then by display name.
struct MapSortByPlayers
{
	bool operator()(MapMetaData *left, MapMetaData *right) const;
};

// Orders the map list by file name.
struct MapSortByFileName
{
	bool operator()(MapMetaData *left, MapMetaData *right) const;
};

#endif
