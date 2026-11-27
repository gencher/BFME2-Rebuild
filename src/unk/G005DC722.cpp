// Generated from the ledger; map snapshot c591f7146f88c8a8248ebb8ae22f0c978c22aed463db64e05ef57ff0dfae99cc
#include "Common/Xfer.h"
#include "GameLogic/SkirmishAI/AITacticKinds.h"
// object=G005DC722 name-evidence=invented
// Extents and edges are hypotheses; byte verification alone does not establish original TU boundaries.
// FUNCTION: BFME2 0x005DC722 size=26 name=??0AITacticOffensive@@QAE@ABVAsciiString@@@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x005DC73C size=11 name=??1AITacticOffensive@@UAE@XZ evidence=N:ledger tier=B hypothesis:mm1-gap-unit
AITacticOffensive::~AITacticOffensive()
{
}
// FUNCTION: BFME2 0x005DC763 size=252 name=?checkTarget@AITacticOffensive@@QAEEPAX@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x005DC85F size=28 name=??0AITacticSiege@@QAE@ABVAsciiString@@@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
AITacticSiege::AITacticSiege(const AsciiString &name)
    : AITacticOffensive(name), m_unk0x58(INVALID_ID)
{
}
// FUNCTION: BFME2 0x005DC87B size=11 name=??1AITacticSiege@@UAE@XZ evidence=N:ledger tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x005DC8A2 size=97 name=?sideHasIdleSiegeWeapons@AITacticSiege@@QAE_NXZ evidence=N:ledger tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x005DC903 size=56 name=?xfer@AITacticSiege@@UAEXPAVXfer@@@Z evidence=N:ledger tier=B hypothesis:mm1-gap-unit
void AITacticSiege::xfer(Xfer *xfer)
{
	Xfer::Version version(1, 1);
	*xfer == version;
	AITactic::xfer(xfer);
	XferObjectID(xfer, &m_unk0x58);
}
// FUNCTION: BFME2 0x005DC93B size=141 name=?isWallBreached@AITacticSiege@@QAE_NXZ evidence=N:ledger tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x005DC9C8 size=285 name=?findWallTarget@AITacticSiege@@QAE_NXZ evidence=N:ledger tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x005DCAE5 size=15 name=?rva005DCAE5@AITacticSiege@@QAEPAVObject@@XZ evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x005DCAF4 size=25 name=?rva005DCAF4@AITacticSiege@@QAE_NXZ evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
