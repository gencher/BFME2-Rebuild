// Portions derived from Command & Conquer Generals and Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
// W3DGhostObject embeds this four-field record at +0xD0. Native 0x64675
// passes its address to the render snapshot; 0x63F78 reads the saved ID there.
#pragma once

#include "Common/GameType.h"

class Drawable;
class GhostObject;

struct DrawableInfo
{
	DrawableInfo() : m_shroudStatusObjectID(INVALID_ID), m_drawable(0), m_ghostObject(0), m_flags(0) {}

	ObjectID m_shroudStatusObjectID;
	Drawable *m_drawable;
	GhostObject *m_ghostObject;
	Int m_flags;
};

typedef char DrawableInfoSize[(sizeof(DrawableInfo) == 0x10) ? 1 : -1];
