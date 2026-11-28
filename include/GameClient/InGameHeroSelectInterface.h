// InGameHeroSelectInterface.h: the in-game hero bar (the row of hero buttons beside the command bar).
// Target facts (Impl member functions at 0x00524F35..0x0052718F):
//  - Impl +0x10 points at a HeroSelectData whose hero ObjectID list sits at +0x10;
//  - Impl +0x48 holds 16 hero slots of 0x18 bytes; each starts with the iterator that names the slot's
//    hero in that list (the list's end marks an empty slot) and has a byte flag at +0x16.
#pragma once
#ifndef BFME2_INGAMEHEROSELECTINTERFACE_H
#define BFME2_INGAMEHEROSELECTINTERFACE_H

#include <list>
#include "Common/GameType.h"

class HeroSelectData
{
public:
	unsigned char m_pad00[0x10];
	_STL::list<ObjectID> m_heroes;						// +0x10
};

struct HeroSlot
{
	_STL::list<ObjectID>::iterator m_hero;				// +0x00
	unsigned char m_pad04[0x12];
	bool m_flag16;										// +0x16
	unsigned char m_pad17;
};

typedef char HeroSlotSizeCheck[sizeof(HeroSlot) == 0x18 ? 1 : -1];

class InGameHeroSelectInterface
{
public:
	enum { MAX_HERO_SLOTS = 16 };

	class Impl
	{
	public:
		void SelectAllHeroes();								// 0x00525E55
		void OnButtonPressed(const char *name);				// 0x00527122
		void SelectNearestBuilder(bool);					// 0x00526F85

		unsigned char m_pad00[0x10];
		HeroSelectData *m_data;								// +0x10
		unsigned char m_pad14[0x34];
		HeroSlot m_slots[MAX_HERO_SLOTS];					// +0x48
	};
};

#endif
