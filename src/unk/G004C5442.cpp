// Generated from the ledger; map snapshot c591f7146f88c8a8248ebb8ae22f0c978c22aed463db64e05ef57ff0dfae99cc
// object=G004C5442 name-evidence=invented
// Extents and edges are hypotheses; byte verification alone does not establish original TU boundaries.
#include "Common/INI.h"
#include "Common/INI/FieldParse.h"
#include "GameLogic/Module/DeflectSpecialPowerModuleData.h"
static const FieldParse s_deflectSpecialPowerModuleDataFieldParse[] =
{
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x004C5442 size=27 name=?buildFieldParse@DeflectSpecialPowerModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
void DeflectSpecialPowerModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	SpecialPowerUpdateModuleData::buildFieldParse(parse);
	parse.add(s_deflectSpecialPowerModuleDataFieldParse, 0);
}

// FUNCTION: BFME2 0x004C545D size=78 name=??0DeflectSpecialPower@@QAE@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
