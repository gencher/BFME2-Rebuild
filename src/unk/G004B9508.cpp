// cl: /O1
// Generated from the ledger; map snapshot c591f7146f88c8a8248ebb8ae22f0c978c22aed463db64e05ef57ff0dfae99cc
// object=G004B9508 name-evidence=invented
// Extents and edges are hypotheses; byte verification alone does not establish original TU boundaries.
#include "GameLogic/Module/InheritUpgradeCreate.h"

// FUNCTION: BFME2 0x004B9508 size=69 name=??0InheritUpgradeCreateModuleData@@QAE@XZ evidence=N:ledger tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x004B9569 size=51 name=??1InheritUpgradeCreateModuleData@@UAE@XZ evidence=N:ledger tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x004B959C size=269 name=?rva004B959C@InheritUpgradeCreate@@QAEXXZ evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x004B96A9 size=27 name=?onBuildComplete@InheritUpgradeCreate@@UAEXXZ evidence=N:ledger tier=B hypothesis:mm1-gap-unit
void InheritUpgradeCreate::onBuildComplete()
{
	if (!shouldDoOnBuildComplete())
		return;

	m_needToRunOnBuildComplete = false;
	rva004B959C();
}
