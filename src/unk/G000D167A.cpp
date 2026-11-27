// Generated from the ledger; map snapshot c591f7146f88c8a8248ebb8ae22f0c978c22aed463db64e05ef57ff0dfae99cc
// object=G000D167A name-evidence=invented
// Extents and edges are hypotheses; byte verification alone does not establish original TU boundaries.
#include <stddef.h>
#include "Common/INI.h"
#include "Common/INI/FieldParse.h"
#include "GameLogic/Module/W3DTornadoDrawModuleData.h"
#include "Common/RadiusDecalTemplate.h"
// FUNCTION: BFME2 0x000D167A size=13 name=?addFlags@Rva000D167A@@QAEXI@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
static const FieldParse s_w3DTornadoDrawModuleDataFieldParse[] =
{
	{ "DecalTemplate", RadiusDecalTemplate::parseRadiusDecalTemplate, 0, offsetof(W3DTornadoDrawModuleData, m_decalTemplate) },
	{ "DecalCount", INI::parseUnsignedInt, 0, offsetof(W3DTornadoDrawModuleData, m_decalCount) },
	{ "DecalMaxRadius", INI::parseReal, 0, offsetof(W3DTornadoDrawModuleData, m_decalMaxRadius) },
	{ 0, 0, 0, 0 }
};

// FUNCTION: BFME2 0x000D16A3 size=17 name=?buildFieldParse@W3DTornadoDrawModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
void W3DTornadoDrawModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	parse.add(s_w3DTornadoDrawModuleDataFieldParse, 0);
}

// FUNCTION: BFME2 0x000D16B4 size=67 name=??0W3DTornadoDrawModuleData@@QAE@XZ evidence=N:ledger tier=B hypothesis:mm1-gap-unit
W3DTornadoDrawModuleData::W3DTornadoDrawModuleData()
{
	m_decalCount = 0;
	m_decalMaxRadius = 200.0f;
}

// FUNCTION: BFME2 0x000D1713 size=48 name=??1W3DTornadoDrawModuleData@@UAE@XZ evidence=N:ledger tier=B hypothesis:mm1-gap-unit
