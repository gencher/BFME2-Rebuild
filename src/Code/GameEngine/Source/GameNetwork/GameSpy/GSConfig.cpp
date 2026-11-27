// Portions derived from Command & Conquer Generals and Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
// cl: /D_BFME_RETAIL_TREE_INSERT_LAYOUT /D_STLP_USE_MALLOC /D_STLP_USE_STATIC_LIB /D_STLP_BFME_COPY_TRIVIAL_EXTERN /D_STLP_NO_EXCEPTIONS /EHs /EHc-
// stlport46
// bfmealloc_alloconly

#include <stdlib.h>
#include <stddef.h>
#include <algorithm>
#include "GameNetwork/GameSpy/GSConfig.h"
#include "Common/GameState.h"
#include "GameClient/MapCache.h"

GameSpyConfigInterface *TheGameSpyConfig = 0;

// FUNCTION: BFME2 0x0054E796 size=7 name=?apply@Rva0054E796DwordImmSetter@@QAEXXZ evidence=?:placeholder tier=B hypothesis:mm1-neighbour-callers+shared-data
// FUNCTION: BFME2 0x0054E7EA size=66 name=?getManglerLocation@GameSpyConfig@@UAE_NHAAVAsciiString@@AAG@Z evidence=N:ledger tier=B hypothesis:name:Z
Bool GameSpyConfig::getManglerLocation(Int index, AsciiString &host, UnsignedShort &port)
{
	if (index < 0 || index >= m_manglerHosts.size())
		return false;

	host = m_manglerHosts[index];
	port = m_manglerPorts[index];
	return true;
}

// FUNCTION: BFME2 0x0054E84C size=33 name=?rva0054E84C@Rva0054E84C@@QAE_NXZ evidence=?:placeholder tier=B hypothesis:cohesion
Bool SectionChecker::isInSection()
{
	Bool found = false;
	for (SectionList::iterator it = m_bools.begin(); it != m_bools.end(); ++it)
	{
		found = found || *(*it);
	}
	return found;
}

// FUNCTION: BFME2 0x0054E86D size=47 name=?isPlayerVIP@GameSpyConfig@@UAE_NH@Z evidence=N:ledger tier=B hypothesis:cohesion
Bool GameSpyConfig::isPlayerVIP(Int id)
{
	_STL::set<Int>::const_iterator it = _STL::find(m_vip.begin(), m_vip.end(), id);
	return it != m_vip.end();
}

// FUNCTION: BFME2 0x0054E8CF size=13 name=?pushValue@Rva0054E8CF@@QAEXH@Z evidence=?:placeholder tier=B hypothesis:K
void SectionChecker::addVar(const Bool *var)
{
	m_bools.push_back(var);
}

// FUNCTION: BFME2 0x0054E8DC size=2795 name=??0GameSpyConfig@@QAE@VAsciiString@@@Z evidence=N:ledger tier=B hypothesis:K

// FUNCTION: BFME2 0x0054F4AF size=89 name=?create@GameSpyConfigInterface@@SAPAV1@VAsciiString@@@Z evidence=N:ledger tier=B hypothesis:K
