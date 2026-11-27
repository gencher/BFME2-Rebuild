// random.h -- WWLib random number generators.
//
// Layout evidence (WWLib/random.cpp object O00619440):
// - Random4Class's constructor writes mt[0] from its seed argument and fills
//   mt[1..623] with the 69069 recurrence, so the table is the first member and
//   mti follows it.
// - Random2Class's constructor stores 0 at +0 and 103 at +4 before filling the
//   250-entry table at +8, so the two indices come first.
// - Pick_Random_Number<RandomClass> (0x006195B0) carries the generator inline:
//   Seed = Seed*0x41C64E6D + 0x3039, then (Seed >> 10) & 0x7FFF. RandomClass's
//   and Random2Class's operator()() are therefore defined here, while
//   Random3Class's and Random4Class's stay out of line (0x00619340, 0x00619490).
// Body: Westwood WWLib random.cpp/random.h (Random4Class's Mersenne Twister and
// Pick_Random_Number's magnitude/mask loop are unchanged upstream code).
#pragma once

class RandomClass
{
public:
	RandomClass(unsigned seed = 0);

	operator int (void) {return(operator()());}
	int operator () (void)
	{
		Seed = (Seed * MULT_CONSTANT) + ADD_CONSTANT;
		return (Seed >> THROW_AWAY_BITS) & (~(~0u << SIGNIFICANT_BITS));
	}
	int operator () (int minval, int maxval);

	enum {
		SIGNIFICANT_BITS = 15
	};

protected:
	unsigned long Seed;		// +0x00

	enum {
		MULT_CONSTANT = 0x41C64E6D,
		ADD_CONSTANT = 0x00003039,
		THROW_AWAY_BITS = 10
	};
};
// SIZE 0x04

class Random2Class
{
public:
	Random2Class(unsigned seed = 0);

	operator int (void) {return(operator()());}
	int operator () (void)
	{
		Table[Index1] ^= Table[Index2];
		int val = Table[Index1];
		Index1++;
		Index2++;
		if (Index1 >= 250) Index1 = 0;
		if (Index2 >= 250) Index2 = 0;
		return(val);
	}
	int operator () (int minval, int maxval);

	enum {
		SIGNIFICANT_BITS = 32
	};

protected:
	int Index1;				// +0x000
	int Index2;				// +0x004
	int Table[250];			// +0x008
};
// SIZE 0x3F0

class Random3Class
{
public:
	Random3Class(unsigned seed1 = 0, unsigned seed2 = 0);

	operator int (void) {return(operator()());}
	int operator () (void);
	int operator () (int minval, int maxval);

	enum {
		SIGNIFICANT_BITS = 32
	};

protected:
	static int Mix1[20];
	static int Mix2[20];
	int Seed;				// +0x00
	int Index;				// +0x04
};
// SIZE 0x08

class Random4Class
{
public:
	Random4Class(unsigned int seed = 4357);

	operator int (void) {return(operator()());}
	int operator () (void);
	int operator () (int minval, int maxval);
	float Get_Float (void);

	enum {
		SIGNIFICANT_BITS = 32
	};

protected:
	unsigned int mt[624];	// +0x000
	int mti;				// +0x9C0
};
// SIZE 0x9C4

template <class T>
int Pick_Random_Number(T & generator, int minval, int maxval)
{
	if (minval == maxval)
		return minval;

	if (minval > maxval) {
		int swapped = minval;
		minval = maxval;
		maxval = swapped;
	}

	// Rejection sampling: mask the generator's output down to the smallest all-ones value covering the span and
	// draw again until the masked value lies inside it.
	int span = maxval - minval;
	int top = T::SIGNIFICANT_BITS - 1;
	while ((span & (1 << top)) == 0 && top > 0)
		top--;

	int mask = ~((~0L) << (top + 1));

	int draw = span + 1;
	while (draw > span)
		draw = generator() & mask;

	return draw + minval;
}
