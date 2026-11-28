// BfmeFixedObject60.h: a 60-byte value the retail image copies and assigns out of line.
// Target facts: three leading words, then a Matrix3D at +0x0C (the copy constructor default-constructs it,
// then assigns it).
#pragma once
#ifndef BFME2_BFMEFIXEDOBJECT60_H
#define BFME2_BFMEFIXEDOBJECT60_H

#include "Common/GameType.h"
#include "WWMath/Matrix3D.h"

struct BfmeFixedObject60
{
	BfmeFixedObject60(const BfmeFixedObject60 &src);
	BfmeFixedObject60 &operator=(const BfmeFixedObject60 &src);

	Int m_word0;			// 0x00
	Int m_word4;			// 0x04
	Int m_word8;			// 0x08
	Matrix3D m_matrix;		// 0x0C
};
typedef char BfmeFixedObject60Size[(sizeof(BfmeFixedObject60) == 0x3C) ? 1 : -1];

#endif
