// PickAndPlayInfo.h: what a unit voice response is about (Zero Hour CommandXlat).
// SIZE 0x24. Target facts: the constructor is
// out of line; BloodthirstyUpdate::sacrificeHorde sets the target drawable at +4
// and the position at +0x14.
#pragma once
#ifndef BFME2_PICKANDPLAYINFO_H
#define BFME2_PICKANDPLAYINFO_H

#include "Common/GameType.h"
#include "Common/Coord3D.h"

class Drawable;
class SpecialPowerTemplate;

struct PickAndPlayInfo
{
	PickAndPlayInfo();

	Bool m_unk0x00;				// 0x00
	Drawable *m_drawTarget;		// 0x04
	void *m_unk0x08;			// 0x08
	const int *m_weaponSlot;	// 0x0C pointer to the weapon slot the response is for (Zero Hour's name; CommandTranslator::issueFireWeaponCommand 0x00429471 stores &slot)
	const SpecialPowerTemplate *m_specialPowerTemplate;	// 0x10 the power the response is for (CommandTranslator::issueSpecialPowerCommand and SpecialAbilityUpdate store the button's template)
	Coord3D m_position;			// 0x14
	UnsignedInt m_unk0x20;		// 0x20
};
// SIZE 0x24

#endif
