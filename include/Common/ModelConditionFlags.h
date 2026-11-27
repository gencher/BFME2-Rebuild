// ModelConditionFlags.h: Object's model-condition bit set. SIZE 0x4C.
// Target facts: retail's ModelConditionFlag name table 0x009BAA98 has 591
// entries (TOPPLED .. SPECIAL_WEAPON_SIX), so the set takes 19 words; at
// Object+0x10C it runs up to the Matrix3D at +0x158. DetachableRiderUpdate sets
// RIDERLESS (297) as word 9 bit 9 at Object+0x130, and WeaponFireSpecialAbilityUpdate
// tests 588-590 in word 18. Condition 200 is the bit OnActivation
// 0x004DCC84 sets and OnDeactivation 0x004DD2C0 clears, and retail reaches it as
// `test dword ptr [obj+0x124], 0x100` there (word 6, bit 8) but as
// `test byte ptr [obj+0x125], 1` in OnDeactivation, so
// both spellings are kept.
#pragma once
#ifndef BFME2_MODELCONDITIONFLAGS_H
#define BFME2_MODELCONDITIONFLAGS_H

#include "Common/GameType.h"
#include <string.h>

class INI;
class AsciiString;

class ModelConditionFlags
{
public:
	// Inline, like BitFlags(): a member built by an enclosing constructor calls the COMDAT
	// (0x00042526: memset 0x4C, returns this; the WeaponTemplateSet ctor 0x0033A888 builds
	// its six sets through it), a local is memset in place. clear() is inline
	// (WeaponTemplateSet::clear 0x002C752F memsets in place).
	ModelConditionFlags() { memset(m_bits, 0, sizeof(m_bits)); }
	// 0x000B6253 (called, returns this): a set holding the five listed conditions;
	// WeaponSet::setWeaponLock 0x002C8AAE and releaseWeaponLock 0x002C8B9B build their
	// WEAPONLOCK_* mask through it.
	enum InitType { kInit = 0 };
	// 0x001E4912 (out of line): a set holding the two listed conditions; Locomotor 0x001E5F1D builds (kInit, 240, 239) with it.
	// The other out-of-line (kInit, conditions...) constructors retail calls on temporaries passed to Object's
	// condition setters (each memsets 0x4C bytes and sets its bits; the map accepts these names):
	// 0x0028F59A one condition (DockUpdate, DeployStyleAIUpdate, SpecialAbilityUpdate),
	// 0x00265254 three, 0x0028F5C6 four (StructureCollapseUpdate::update 0x004A46BF: 0x43, 0x44, 0x45, 5),
	// 0x002652AC eight (SpecialAbilityUpdate::handlePackingProcessing 0x0044F0B3), 0x00265366 eleven.
	ModelConditionFlags(InitType init, UnsignedInt a);
	ModelConditionFlags(InitType init, UnsignedInt a, UnsignedInt b, UnsignedInt c);
	ModelConditionFlags(InitType init, UnsignedInt a, UnsignedInt b, UnsignedInt c, UnsignedInt d);
	ModelConditionFlags(InitType init, UnsignedInt a, UnsignedInt b, UnsignedInt c, UnsignedInt d, UnsignedInt e, UnsignedInt f, UnsignedInt g, UnsignedInt h);
	ModelConditionFlags(InitType init, UnsignedInt a, UnsignedInt b, UnsignedInt c, UnsignedInt d, UnsignedInt e, UnsignedInt f, UnsignedInt g, UnsignedInt h, UnsignedInt i, UnsignedInt j, UnsignedInt k);
	ModelConditionFlags(InitType init, UnsignedInt a, UnsignedInt b);
	ModelConditionFlags(InitType init, UnsignedInt a, UnsignedInt b, UnsignedInt c, UnsignedInt d, UnsignedInt e);
	// 0x0044E9C7 (out of line): a set holding fourteen conditions (SpecialAbilityUpdate 0x0044EE07).
	ModelConditionFlags(InitType init, UnsignedInt a, UnsignedInt b, UnsignedInt c, UnsignedInt d, UnsignedInt e, UnsignedInt f, UnsignedInt g, UnsignedInt h, UnsignedInt i, UnsignedInt j, UnsignedInt k, UnsignedInt l, UnsignedInt m, UnsignedInt n);
	void set(Int bit, Int value);	// 0x000B3FA5, out of line (CrushDie::onDie 0x00486327)
	void clear() { memset(m_bits, 0, sizeof(m_bits)); }
	// 0x00045455 (out of line, `??0ModelConditionFlags@@QAE@ABV0@@Z`): the copy; SlowDeathBehavior's complement 0x0045D859 copies through it twice.
	// Inline like BitFlags' copy (memcpy); /O1 declines it where retail calls the COMDAT (complement 0x0045D859 twice).
	ModelConditionFlags(const ModelConditionFlags &other) { memcpy(m_bits, other.m_bits, sizeof(m_bits)); }
	// 0x0045D859 (map: rva0045D859 on the placeholder class WeaponTemplateSetHead): every condition flipped.
	ModelConditionFlags complement() const;
	// 0x000B3EB3, ?rva000B3EB3@ModelConditionFlags@@QBE_NXZ (out of line): true when any condition is set (ModelConditionUpgrade 0x004B6532 tests both of its sets with it)
	Bool rva000B3EB3() const;
	// The 591-name set's parser (0x000B937E): tokens to the end of the line, set by name;
	// WeaponTemplateSet's OnlyInCondition (0x002C883E) calls it with no name list.
	void parse(INI *ini, AsciiString *str);
	Bool testMasked(UnsignedInt bit) const { return (m_bits[bit>>5] & (1u<<(bit&31))) != 0; }
	Bool testBool(UnsignedInt bit) const { return ((m_bits[bit>>5]>>(bit&31))&1)!=0; }
	UnsignedInt test(UnsignedInt bit) const { return m_bits[bit >> 5] & (1u << (bit & 31)); }
	// The shift-and-mask spelling (BitFlags::test): SpecialDisguiseUpdate 0x004B05F5 tests
	// bit 300 as `mov ecx,word; shr ecx,0xc; test cl,1`.
	Bool testShifted(UnsignedInt bit) const { return ((m_bits[bit >> 5] >> (bit & 31)) & 1) != 0; }
	// 0x001DFE56 (emitted in FXList.cpp; FXNugget's slot 4 runs it on the source's and the
	// secondary's conditions): every bit of mustBeSet present and no bit of mustBeClear.
	Bool testSetAndClear(const ModelConditionFlags &mustBeSet, const ModelConditionFlags &mustBeClear) const
	{
		for (UnsignedInt i = 0; i < 19; ++i)
		{
			if (m_bits[i] & mustBeClear.m_bits[i])
				return false;
			if ((m_bits[i] & mustBeSet.m_bits[i]) != mustBeSet.m_bits[i])
				return false;
		}
		return true;
	}
	void set(UnsignedInt bit) { m_bits[bit >> 5] |= 1u << (bit & 31); }
	void clear(UnsignedInt bit) { m_bits[bit >> 5] &= ~(1u << (bit & 31)); }
	// Object's single-condition clear/set test (Object.h). Retail reads the WORD and lets the compiler narrow it to a
	// byte test: two conditions of one word share one address (AIInternalMoveToState::onExit 0x003473A4,
	// `lea ebp,[obj+0x118]; test byte [ebp],0x80 ... test byte [ebp+1],2` for 3*32+7 and 3*32+9), which a byte-indexed
	// read does not give (it addresses [obj+0x118] and [obj+0x119] separately). Same bytes for one test.
	UnsignedInt testByte(UnsignedInt bit) const { return m_bits[bit >> 5] & (1u << (bit & 31)); }
	void clearByte(UnsignedInt bit) { ((Byte *)m_bits)[bit >> 3] &= (Byte)~(1u << (bit & 7)); }

	UnsignedInt m_bits[19];						// 0x0
};
// SIZE 0x4C

#endif
