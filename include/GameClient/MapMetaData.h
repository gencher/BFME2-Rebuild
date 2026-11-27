// MapMetaData.h: what the map list remembers about one map file. SIZE 0x100.
//
// Target facts (retail RVAs): no base, no virtual function, no vptr. The default
// constructor 0x003031D3 zeroes the two string handles at +0 and +4, the Bools and
// ints from +0x20, builds the 0x10-byte waypoint map at +0x38 (0x00413727), the two
// coordinate lists at +0x48 and +0x4C (0x00280A8D), clears +0x50, builds the eight
// player records at +0x54 (0x00302C81), clears +0xF4 and zeroes the six floats at
// +8. Assignment 0x0030328D copies the wide strings at +0 and +4 (StringBase<G>::set
// 0x00037150), six dwords at +8, the int at +0x20, the bytes at +0x24..+0x26, the
// dwords at +0x28..+0x34, the waypoint map (0x00302C9C: the map assignment
// 0x0030200E then the count at +0x0C), the two lists (0x00301B54), the narrow string
// at +0x50 (0x000366F0), the players (0x00302CB7: eight PlayerPosition assignments,
// 0x0030242A) and +0xF4, +0xF8 and +0xFC. The object is allocated with 0x100 bytes
// (0x0057CFC0). getDescription 0x003009CD reads the description (+4), the file name
// (+0x50) and the cache (+0xFC); bfme_getBaseDisplayName 0x00300AEA the label (+0) and
// its cache (+0xF8).
//
// The INI reader's token table (0x00868AE8, fields extentMin/extentMax, numPlayers,
// isMultiplayer, isScenarioMP, displayName, description, isOfficial, timestampLo/Hi,
// fileSize, fileCRC, Player_N_Start, InitialCameraPosition) gives the names. The
// order of the Bools (+0x24 multiplayer, +0x25 scenario, +0x26 official) and of the
// ints (+0x28/+0x2C size and CRC, which the constructor's initializer list stores
// before the member constructors run, then +0x30/+0x34 the timestamp halves, stored
// at the end of the constructor body) is read from the constructor and the copy. Under `// stlport46` the three
// containers are the real STLport types; elsewhere they are same-size byte storage.
#pragma once
#ifndef BFME2_MAPMETADATA_H
#define BFME2_MAPMETADATA_H

#include <stddef.h>
#include "Common/GameType.h"
#include "Common/AsciiString.h"
#include "Common/UnicodeString.h"
#include "Common/Coord3D.h"
#include "Common/Region3D.h"

#include <map>
#include <list>
#include <set>

class DataChunkInput;
class DataChunkOutput;
struct DataChunkInfo;

bool operator<(const AsciiString &left, const AsciiString &right);			// 0x0005598C, the maps' key order

// The map's named waypoints and how many of them are start spots. SIZE 0x10.
class WaypointMap : public _STL::map<AsciiString, Coord3D>
{
public:
	WaypointMap() : m_numStartSpots(0) {}
	~WaypointMap();
	void update();															// 0x00302B7D

	int m_numStartSpots;													// +0x0C
};

// One player slot of the map. SIZE 0x14. Target facts: default constructor
// 0x00302805 sets the three bytes to 1 and the int to -1 and builds the set
// (0x000D3A71); the copy 0x00302CE2 and assignment 0x0030242A move the bytes, the
// int and the set (0x00301AB1); 0x003023B8 inserts a name into the set.
struct PlayerPosition
{
	PlayerPosition() : m_allowHuman(true), m_allowComputer(true), m_loadAIScripts(true), m_forceTeam(-1) {}
	~PlayerPosition();
	void rva003023B8(const AsciiString &name);								// 0x003023B8
	Bool parse(DataChunkInput &file, DataChunkInfo *info);					// reads one MPPositionInfo chunk (role-inferred name)
	void write(DataChunkOutput *out);										// writes one MPPositionInfo chunk

	bool m_allowHuman;														// +0x00 (role-inferred names)
	bool m_allowComputer;													// +0x01
	bool m_loadAIScripts;													// +0x02
	int m_forceTeam;														// +0x04 -1 = none
	_STL::set<AsciiString> m_factions;										// +0x08
};
// SIZE 0x14

Bool operator==(const PlayerPosition &left, const PlayerPosition &right);
Bool operator!=(const PlayerPosition &left, const PlayerPosition &right);

struct MapPlayers
{
	void write(DataChunkOutput *out);										// writes the MPPositionList chunk

	PlayerPosition m_slot[8];												// 0xA0
};

Bool operator==(const MapPlayers &left, const MapPlayers &right);

typedef _STL::list<Coord3D> MapCoordList;

class MapMetaData
{
public:
	MapMetaData();															// 0x003031D3
	MapMetaData(const MapMetaData &other);									// 0x003039E8
	~MapMetaData();															// 0x0022DBC6
	MapMetaData &operator=(const MapMetaData &other);						// 0x0030328D

	UnicodeString getDescription();											// 0x003009CD
	UnicodeString bfme_getBaseDisplayName();								// 0x00300AEA
	UnicodeString bfme_getDisplayName(bool includePlayerCount);				// 0x00300C7E
	UnicodeString rva00300D0E();											// 0x00300D0E
	UnicodeString bfme_getDescriptionFirstLine();							// 0x00301534

	UnicodeString m_displayName;											// +0x00
	UnicodeString m_description;											// +0x04
	Region3D m_extent;														// +0x08
	Int m_numPlayers;														// +0x20
	Bool m_isMultiplayer;													// +0x24
	Bool m_isScenarioMP;													// +0x25
	Bool m_isOfficial;														// +0x26
	UnsignedInt m_fileSize;													// +0x28
	UnsignedInt m_fileCRC;													// +0x2C
	UnsignedInt m_timestampLo;												// +0x30
	UnsignedInt m_timestampHi;												// +0x34
	WaypointMap m_waypoints;												// +0x38
	MapCoordList m_supplyPositions;											// +0x48
	MapCoordList m_techPositions;											// +0x4C
	AsciiString m_fileName;													// +0x50
	MapPlayers m_players;													// +0x54
	UnsignedInt m_wordF4;													// +0xF4
	UnicodeString m_cachedDisplayName;										// +0xF8
	UnicodeString m_cachedDescription;										// +0xFC
};
typedef char MapMetaDataSizeCheck[sizeof(MapMetaData) == 0x100 ? 1 : -1];

#endif
