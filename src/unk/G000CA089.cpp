// Generated from the ledger; map snapshot c591f7146f88c8a8248ebb8ae22f0c978c22aed463db64e05ef57ff0dfae99cc
// object=G000CA089 name-evidence=invented
// Extents and edges are hypotheses; byte verification alone does not establish original TU boundaries.
#include <stddef.h>
#include "Common/INI.h"
#include "Common/INI/FieldParse.h"
#include "GameLogic/Module/W3DQuadrupedDrawModuleData.h"
static const FieldParse s_w3DQuadrupedDrawModuleDataFieldParse[] =
{
	{ "LeftFrontFootBone", INI::parseAsciiString, 0, offsetof(W3DQuadrupedDrawModuleData, m_footBones[0]) },
	{ "RightFrontFootBone", INI::parseAsciiString, 0, offsetof(W3DQuadrupedDrawModuleData, m_footBones[1]) },
	{ "LeftRearFootBone", INI::parseAsciiString, 0, offsetof(W3DQuadrupedDrawModuleData, m_footBones[2]) },
	{ "RightRearFootBone", INI::parseAsciiString, 0, offsetof(W3DQuadrupedDrawModuleData, m_footBones[3]) },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x000CA089 size=27 name=?buildFieldParse@W3DQuadrupedDrawModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
void W3DQuadrupedDrawModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	W3DModelDrawModuleData::buildFieldParse(parse);
	parse.add(s_w3DQuadrupedDrawModuleDataFieldParse, 0);
}

