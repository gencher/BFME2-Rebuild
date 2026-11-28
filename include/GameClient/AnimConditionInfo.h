// AnimConditionInfo.h: the animation list of one model condition state of the scripted model draw module.
// Target facts: the animation vector sits at +0x50 (begin) and +0x54 (end) and holds 0x40-byte entries;
// an entry's weight is its word at +0x30 (pickRandomAnimation sums them, one less for the entry to avoid).
#pragma once
#ifndef BFME2_ANIMCONDITIONINFO_H
#define BFME2_ANIMCONDITIONINFO_H

#include <vector>
#include "Common/GameType.h"

struct AnimConditionAnimation
{
	Byte m_unk0x00[0x30];
	Int m_weight;				// 0x30
	Byte m_unk0x34[0xC];
};
typedef char AnimConditionAnimationSize[(sizeof(AnimConditionAnimation) == 0x40) ? 1 : -1];

struct AnimConditionInfo
{
	// Picks an animation index weighted by each entry's weight, skipping the one to avoid once; 0 when there is nothing to choose between.
	Int pickRandomAnimation(Int avoid, Int maximum);

	Byte m_unk0x00[0x50];
	std::vector<AnimConditionAnimation> m_animations;		// 0x50
};

#endif
