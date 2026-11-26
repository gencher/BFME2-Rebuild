// Generated from the ledger; map snapshot c591f7146f88c8a8248ebb8ae22f0c978c22aed463db64e05ef57ff0dfae99cc
// object=G00425DC2 name-evidence=invented
// Extents and edges are hypotheses; byte verification alone does not establish original TU boundaries.
#include "Common/GameType.h"

static const char *const s_stanceNames[6] =
{
	"Uninitialized",
	"Battle",
	"Aggressive",
	"HoldGround",
	"Porcupine",
	"HoldGroundMoving"
};

// FUNCTION: BFME2 0x00425DC2 size=27 name=?getStanceName@@YAPBDH@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
const char *getStanceName(Int stance)
{
	if (stance < 0 || stance >= 6)
		return "Unknown";
	return s_stanceNames[stance];
}
