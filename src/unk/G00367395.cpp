#include "GameLogic/Module/GiantBirdAIUpdate.h"

// Generated from the ledger; map snapshot c591f7146f88c8a8248ebb8ae22f0c978c22aed463db64e05ef57ff0dfae99cc
// object=G00367395 name-evidence=invented
// Extents and edges are hypotheses; byte verification alone does not establish original TU boundaries.
// FUNCTION: BFME2 0x00367395 size=7 name=?get@Rva00367395DwordField@@QBEHXZ evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x0036739C size=7 name=?get@Rva0036739CDwordField@@QBEHXZ evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x003673A3 size=7 name=?get@Rva003673A3ByteField@@QBEEXZ evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x003673AA size=17 name=?rva003673AA@Rva00362461@@QAEXM@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x003673BB size=19 name=?get@Rva003673BBByteFloatField@@QBEMXZ evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x003673CE size=7 name=?get@Rva003673CEFloatField@@QBEMXZ evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x003673D5 size=7 name=?get@Rva003673D5LeaGetter@@QBEPAXXZ evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x003673DC size=17 name=?rva003673DC@Rva003673DC@@QAEXM@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x003673ED size=20 name=?rva003673ED@Rva003673DC@@QAEXPBURva003673EDPoint@@@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x00367401 size=20 name=?rva00367401@Rva003673DC@@QAEXPBURva003673EDPoint@@@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x00367415 size=13 name=?set@Rva00367415ByteSlot@@QAEXE@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x00367422 size=55 name=?addDelta@Rva00367422@@QAEXPBURva00367422Point@@@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x00367459 size=13 name=?set@Rva00367459DwordSlot@@QAEXH@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x00367466 size=13 name=?set@Rva00367466DwordSlot@@QAEXH@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x00367473 size=7 name=?get@Rva00367473DwordField@@QBEHXZ evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x0036747A size=13 name=?set@Rva0036747AByteSlot@@QAEXE@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x00367487 size=7 name=?get@Rva00367487ByteField@@QBEEXZ evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x0036748E size=37 name=?setUnk0x4B8Bit@GiantBirdAIUpdate@@QAEXH_N@Z evidence=X:map request (link (from standalone link a1a88198eb)) tier=B hypothesis:mm1-gap-unit
void GiantBirdAIUpdate::setUnk0x4B8Bit(Int bit, Bool value)
{
	if (value)
		m_unk0x4B8 |= 1 << bit;
	else
		m_unk0x4B8 &= ~(1 << bit);
}

// FUNCTION: BFME2 0x003674B3 size=10 name=?set@Rva003674B3ByteSlot@@QAEXE@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x003674E7 size=15 name=?getTeam@Rva003674E7@@QBEPAVTeam@@XZ evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x003674F6 size=4 name=?get@Rva003674F6LeaField@@QBEPAXXZ evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x003674FA size=4 name=?get@Rva003674FAByteField@@QBEEXZ evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x003674FE size=10 name=?rva003674FE@GameSpyInfo@@UAEXH@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
