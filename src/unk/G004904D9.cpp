// cl: /O1 -Ireference/shims/bfmealloc_alloconly
#include "GameLogic/Module/SiegeAIUpdateModuleData.h"
#include "Common/INI/FieldParse.h"

// Generated from the ledger; map snapshot c591f7146f88c8a8248ebb8ae22f0c978c22aed463db64e05ef57ff0dfae99cc
// object=G004904D9 name-evidence=invented
// Extents and edges are hypotheses; byte verification alone does not establish original TU boundaries.

// Siege configuration inherits the AI fields.
static const FieldParse s_siegeAIFields[] =
{
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x004904D9 size=18 name=??0SiegeAIUpdateModuleData@@QAE@XZ evidence=N:ledger tier=B hypothesis:mm1-gap-unit
SiegeAIUpdateModuleData::SiegeAIUpdateModuleData()
{
}

// FUNCTION: BFME2 0x004904EB size=27 name=?buildFieldParse@SiegeAIUpdateModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
void SiegeAIUpdateModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	AIUpdateModuleData::buildFieldParse(parse);
	parse.add(s_siegeAIFields, 0);
}
