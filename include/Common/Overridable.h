// Overridable.h: SAGE's INI override chain (Zero Hour Common/Overridable.h).
// SIZE 0x10.
// Target facts: retail table 0x007DDC48 has one slot, the deleting destructor
// 0x001E4445, so the class stands alone with a virtual destructor (no memory-
// pool glue slot). The inline constructor (out-of-line copy 0x001E35CA, never
// called) stores 0 at +4, -1 at +0xC, the table and false at +8.
// Retail game.dat .text calls: ~Overridable 0x001E3624 16 calls and 27 jumps,
// friend_getFinalOverride 0x001E35DF 113, its const form 0x00288609 216,
// deleteOverrides 0x001E35ED 16, so those are out of line; markAsOverride
// (copy 0x00248D08, 2 calls) is a one-store inline. deleteOverrides matches
// Zero Hour's body: an override destroys itself (virtual destructor call with
// flag 0, then the free at 0x0002FD60), otherwise it recurses down the chain.
#pragma once
#ifndef BFME2_OVERRIDABLE_H
#define BFME2_OVERRIDABLE_H

#include "Common/GameType.h"

class Overridable
{
public:
	Overridable() : m_nextOverride(0), m_isOverride(false), m_reloadState(-1) {}
	virtual ~Overridable();											// 0x001E3624

	// Zero Hour's inline recursion; retail inlines the +4 test at call sites and calls
	// the out-of-line loop below for the rest.
	const Overridable *getFinalOverride() const
	{
		if (m_nextOverride)
			return m_nextOverride->getFinalOverride();
		return this;
	}
	// Non-const form: Zero Hour's inline recursion. Retail's out-of-line copy is the
	// 14-byte loop at 0x001E35DF, and each caller inlines the +4 test before it.
	Overridable *friend_getFinalOverride()
	{
		if (m_nextOverride)
			return m_nextOverride->friend_getFinalOverride();
		return this;
	}
	const Overridable *friend_getFinalOverride() const;				// 0x00288609
	Overridable *friend_getNextOverride() { return m_nextOverride; }
	void setNextOverride(Overridable *next) { m_nextOverride = next; }
	void markAsOverride() { m_isOverride = true; }
	// The +0x0C word is -1 after construction. CommandButton's INI reload path
	// (load type 5, parser 0x001DAF81 and ControlBar 0x0031BF1B) stores 1 into a
	// retired entry and 0 into its replacement, so it is a reload state; the
	// store is a bare inline `mov dword [reg+0xC], imm`. Role-inferred name.
	void setReloadState(Int state) { m_reloadState = state; }
	Int getReloadState() const { return m_reloadState; }				// CommandSet::refreshIfStale re-looks a button up once this is at least 1
	Bool isOverride() const { return m_isOverride; }
	Overridable *deleteOverrides();									// 0x001E35ED

private:
	Overridable *m_nextOverride;									// 0x04
	Bool m_isOverride;												// 0x08
	Int m_reloadState;													// 0x0C, -1 after construction
};
// SIZE 0x10

// OVERRIDE<T>: the handle every overridable settings global is declared as
// (the map's own names: ?TheWaterTransparency@@3V?$OVERRIDE@VWaterTransparencySetting@@@@A
// at 0x009FF488 and the like). It is one pointer to the FIRST definition; reading
// it yields the FINAL override of the chain. Evidence: INI::parseWaterTransparencyDefinition
// 0x00200EE4 tests the pointer against 0 (the first definition), then `mov ecx,[esi+4];
// test ecx,ecx; call 0x001E35DF` -- the +4 next-override test is inlined at the
// call site and 0x001E35DF is the out-of-line recursion (a loop, 14 bytes).
// getNonOverloadedPointer is Zero Hour's name for the raw first pointer; the
// parser reads it to copy the current setting into a new override.
template <class T>
class OVERRIDE
{
public:
	OVERRIDE(const T *overridable = 0) : m_overridable(overridable) {}
	OVERRIDE &operator=(const T *overridable) { m_overridable = overridable; return *this; }
	const T *getNonOverloadedPointer() const { return m_overridable; }
	const T *operator->() const { return *this; }
	operator const T *() const
	{
		if (!m_overridable)
			return 0;
		return (const T *)m_overridable->getFinalOverride();
	}

private:
	const T *m_overridable;	// 0x00: the first definition (not the final override)
};
// SIZE 0x04 per instance

#endif
