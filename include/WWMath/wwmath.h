// Portions derived from Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
// Portions derived from Command & Conquer Generals and Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
// wwmath.h: the inline x87 forms of WWMath::Sin, WWMath::Cos and WWMath::Sqrt (Zero Hour's wwmath.h).
// Target facts (retail game.dat): callers expand `fld [val]; fsin/fcos/fsqrt; fstp [float temp]` in place
// (Quaternion axis-angle constructor 0x007173E0, Matrix3D::Look_At 0x00712150, Collide(LineSeg, Sphere)
// 0x00714160); the class WWMath itself is declared in WWMath/Vector3.h.
#pragma once
#ifndef BFME2_WWMATH_H
#define BFME2_WWMATH_H

#include <math.h>
#include "WWMath/Vector3.h"
#include "WWMath/wwinline.h"

// WWINLINE (Zero Hour's always.h spelling) is defined in WWMath/wwinline.h.

// Zero Hour's constants (wwmath.h).
#define WWMATH_PI				3.141592654f
#define RAD_TO_DEGF(x)			((x) * (180.0f / WWMATH_PI))

// ----------------------------------------------------------------------------
// Cos
// ----------------------------------------------------------------------------

#if defined(_MSC_VER) && defined(_M_IX86)
WWINLINE float WWMath::Cos(float val)
{
	float retval;
	__asm {
		fld [val]
		fcos
		fstp [retval]
	}
	return retval;
}
#else
WWINLINE float WWMath::Cos(float val)
{
	return cosf(val);
}
#endif

// ----------------------------------------------------------------------------
// Sin
// ----------------------------------------------------------------------------

#if defined(_MSC_VER) && defined(_M_IX86)
WWINLINE float WWMath::Sin(float val)
{
	float retval;
	__asm {
		fld [val]
		fsin
		fstp [retval]
	}
	return retval;
}
#else
WWINLINE float WWMath::Sin(float val)
{
	return sinf(val);
}
#endif

// ----------------------------------------------------------------------------
// Sqrt
// ----------------------------------------------------------------------------

#if defined(_MSC_VER) && defined(_M_IX86)
WWINLINE float WWMath::Sqrt(float val)
{
	float retval;
	__asm {
		fld [val]
		fsqrt
		fstp [retval]
	}
	return retval;
}
#else
WWINLINE float WWMath::Sqrt(float val)
{
	return (float)sqrt(val);
}
#endif

// ----------------------------------------------------------------------------
// Acos
// ----------------------------------------------------------------------------

WWINLINE float WWMath::Acos(float val)
{
	return (float)acos(val);
}

// ----------------------------------------------------------------------------
// Floor
// ----------------------------------------------------------------------------

WWINLINE float WWMath::Floor(float val)
{
	return floorf(val);
}

#endif
