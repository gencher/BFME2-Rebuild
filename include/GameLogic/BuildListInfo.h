// BuildListInfo: one entry of a side's build list (SidesInfo +0, Player +0x278,
// AISideBuildList), chained through +0x2C. A Snapshot. SIZE 0x80.
// Retail evidence: SidesInfo's copy 0x0032C6AD allocates 0x80 bytes per entry; the
// constructor 0x0032A0CE stores the table 0x00C0D904, copies AsciiString::TheEmptyString
// into +0x04 and +0x30, zeroes the rest, sets +0x34 to 100 and +0x38/+0x3A/+0x3B to true
// and clears ten dwords from +0x50; the destructor 0x0032A186 deletes the entries chained
// after this one, then releases the three strings (+0x30, +0x08, +0x04) and stores
// Snapshot's table. The map spells the destructor MAE (protected) and GetSnapshotName UBE.
// Member names follow Zero Hour's BuildListInfo where the offsets agree (donor names);
// the BFME 2 fields after +0x44 are unnamed.
#pragma once
#ifndef BFME2_BUILDLISTINFO_H
#define BFME2_BUILDLISTINFO_H

#include "Common/GameType.h"
#include "Common/AsciiString.h"
#include "Common/Coord3D.h"
#include "Common/Coord2D.h"
#include "Common/Snapshot.h"

class Xfer;

class BuildListInfo : public Snapshot
{
	friend class SidesInfo;		// SidesInfo::init 0x0032CDAA deletes its list
	friend class Player;		// Player::setBuildList 0x002A99D2 deletes the old list
public:
	BuildListInfo();									// 0x0032A0CE
	// The copy constructor (0x00329DE6) and assignment (0x003299AD) are the implicit
	// memberwise ones: the ten dwords at +0x50 are copied in member order (rep movs).
	BuildListInfo *duplicate();
	AsciiString getScript() const;
	AsciiString getTemplateName() const;				// 0x000AF1DD (the shared 27-byte copy of the string at +0x08: AIPlayer::computeCenterAndRadiusOfBase calls it for every entry)
	void setBuildingName(AsciiString name);				// 0x0032A438 (m_buildingName = name; the map spells it ?rva0032A438@Rva0032A438@@QAEXVAsciiString@@@Z; callers 0x0032CF16, 0x0032F22D, 0x0032F6BC are the build list parsers)
	void setScript(AsciiString script);					// 0x0032A46C (m_script = script; the map spells it ?rva0032A46C@Rva0032A46C@@QAEXVAsciiString@@@Z; callers 0x0032CFAA, 0x0032F2B9)
	void setSupplyBuilding(Bool isSupplyBuilding);

	virtual const char *GetSnapshotName() const;		// slot 2, 0x00329EE3

protected:
	virtual ~BuildListInfo();
	virtual void loadPostProcess() {}					// slot 1, the shared empty body 0x000B3FD0
	virtual void xfer(Xfer *xfer);						// slot 3, 0x00329BBF

public:
	AsciiString m_buildingName;							// +0x04
	AsciiString m_templateName;							// +0x08
	Coord3D m_location;									// +0x0C
	Coord2D m_rallyPointOffset;							// +0x18 (xfer 0x00329BBF transfers it as a Coord2D)
	Real m_angle;										// +0x20
	Bool m_isInitiallyBuilt;							// +0x24
	UnsignedInt m_numRebuilds;							// +0x28
	BuildListInfo *m_next;								// +0x2C, the link AISideBuildList::addInfo walks
	AsciiString m_script;								// +0x30 (getScript 0x0032A4A0)
	Int m_health;										// +0x34, 100 in the constructor
	Bool m_whiner;										// +0x38
	Bool m_unsellable;									// +0x39
	Bool m_repairable;									// +0x3A
	Bool m_automaticallyBuild;							// +0x3B
	ObjectID m_objectID;								// +0x3C
	UnsignedInt m_objectTimestamp;						// +0x40
	Bool m_underConstruction;							// +0x44
	Bool m_unk0x45;										// +0x45
	Bool m_isSupplyBuilding;									// +0x46: set by setSupplyBuilding
	Bool m_unk0x47;										// +0x47
	ObjectID m_unk0x48;									// +0x48 (xfer: XferObjectID)
	UnsignedInt m_unk0x4C;								// +0x4C (xfer: unsigned int)
	ObjectID m_unk0x50[10];								// +0x50 (xfer: ten XferObjectID calls)
	Int m_unk0x78;										// +0x78
	Int m_unk0x7C;										// +0x7C
};
// SIZE 0x80
typedef char BuildListInfoSize[(sizeof(BuildListInfo) == 0x80) ? 1 : -1];

#endif
