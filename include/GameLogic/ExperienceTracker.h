// ExperienceTracker.h: an object's experience and current level. SIZE 0x34.
//
// Target facts: the constructor 0x0039AEBC (ExperienceTracker(const ThingTemplate *))
// stores the Snapshot-derived table 0x00C1AD20, the template at +0x04, zeroes
// +0x08..+0x14 and +0x24..+0x30, sets +0x18 = -1, +0x1C = 1.0f and the byte
// +0x20, keys the template's name string (+0x64) into +0x30 through
// NameKeyGenerator::nameToKey, and allocates a 0x14-byte helper (constructor
// 0x0039AE30, given this) into +0x2C. xfer 0x0039AFC8 (version 5) transfers
// +0x10 as a Real (an Int before version 2), the AsciiString +0x08 (re-keyed
// into +0x0C on load), the Real +0x1C, the Int +0x14, a dropped Int before
// version 4, the Int +0x24, the Bool +0x20, the Int +0x18, the helper through
// its slot 2 (version 2+), the name key +0x30 (version 3+) and the Int +0x28
// (version 5+). loadPostProcess 0x0039AC9C forwards to the helper's slot 1.
// isTrainable 0x0039ADAF reads the template's IsTrainable (+0x5E7).
// The levels themselves live in TheExperienceLevelStore (0x00DFECC4), which
// FindCurrentLevel / FindExperienceLevelList are given this tracker.
// Open: retail's table has __purecall in slot 2 (GetSnapshotName); it is
// declared here so the class is concrete.
// Donor facts: Zero Hour's ExperienceTracker names m_currentExperience and
// m_experienceScalar (1.0 by default) and isTrainable.
#pragma once
#ifndef BFME2_EXPERIENCETRACKER_H
#define BFME2_EXPERIENCETRACKER_H

#include <stddef.h>
#include "Common/GameType.h"
#include "Common/AsciiString.h"
#include "Common/Snapshot.h"

class ThingTemplate;
class Xfer;

class ExperienceTracker : public Snapshot
{
public:
	ExperienceTracker(const ThingTemplate *thingTemplate);				// 0x0039AEBC
	virtual ~ExperienceTracker();

	Bool rva0039ABFF() const;											// 0x0039ABFF (placeholder-owned row in ExperienceTracker.cpp; VeterancyCrateCollide::isValidToExecute 0x004BCEF0 calls it)
	Bool rva0039B4EC(Int levels, Bool a, Bool b);						// 0x0039B4EC (placeholder-owned row in ExperienceTracker.cpp; the crate collides call it to grant levels)
	Bool isTrainable() const;											// 0x0039ADAF
	Bool rva0039AE04() const;										// 0x0039AE04 (map: ?rva0039AE04@Rva0039ADF3@@QBE_NXZ; the object at +0x34 is in the right state and the tracker is trainable: SupplyCenterDockUpdate::action 0x004A0EAD and 8 more callers)
	void rva0039B315(Real amount, Bool a, Bool b, Bool c, Bool d);
	void rva0039B3D1(Real amount, Bool flag);	// 0x0039B3D1 (map: ?rva0039B3D1@ExperienceTracker@@QAEXM_N@Z; follows the +0x38 owner chain first)	// 0x0039B315 (map: ?rva0039B315@ExperienceTracker@@QAEXM_N000@Z; grants experience)
	void reset();												// 0x0039B548 (placeholder-owned row in ExperienceTracker.cpp; ActiveBody 0x004BDA67 calls it): clears the level, the experience and the scalar
	void resetCurrentLevel();											// 0x0039AD45 (descriptive; the map holds a placeholder): clears +0x08 and +0x0C

protected:
	virtual void loadPostProcess();										// 0x0039AC9C
	virtual const char *GetSnapshotName() const;
	virtual void xfer(Xfer *xfer);										// 0x0039AFC8

public:
	const ThingTemplate *m_thingTemplate;								// 0x04
	AsciiString m_currentLevelName;										// 0x08
	NameKeyType m_currentLevelKey;										// 0x0C
	Real m_currentExperience;											// 0x10
	Int m_unk0x14;														// 0x14
	Int m_unk0x18;														// 0x18 = -1
	Real m_experienceScalar;											// 0x1C = 1.0
	Bool m_unk0x20;														// 0x20
	Int m_unk0x24;														// 0x24
	Int m_unk0x28;														// 0x28
	void *m_helper;														// 0x2C 0x14-byte helper, constructor 0x0039AE30
	NameKeyType m_templateNameKey;										// 0x30
};
// SIZE 0x34

typedef char ExperienceTracker_off_m_currentExperience[(offsetof(ExperienceTracker, m_currentExperience) == 0x10) ? 1 : -1];
typedef char ExperienceTracker_off_m_templateNameKey[(offsetof(ExperienceTracker, m_templateNameKey) == 0x30) ? 1 : -1];
typedef char ExperienceTracker_size[(sizeof(ExperienceTracker) == 0x34) ? 1 : -1];

#endif
