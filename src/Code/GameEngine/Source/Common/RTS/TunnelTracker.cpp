// cl: /O1 /D_BFME_RETAIL_TREE_INSERT_LAYOUT /D_STLP_USE_STATIC_LIB /D_CRTIMP=
// stlport46
// Portions derived from Command & Conquer Generals and Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
#include "Common/TunnelTracker.h"
#include "GameLogic/Damage.h"
#include "GameLogic/Module/BodyModule.h"
#include "GameLogic/Object.h"
#include "GameLogic/ObjectFilter.h"
#include "Common/GlobalData.h"
#include "Common/ThingTemplate.h"
#include "GameLogic/GameLogic.h"
#include "Common/Xfer.h"
#include "Common/XferException.h"
#include "GameClient/Drawable.h"
#include "GameLogic/AI.h"
#include "GameLogic/AIPathfind.h"

// The list transfer xfer calls (map: Rva0036ABAFXfer).
Xfer *Rva0036ABAFXfer(Xfer *xfer, std::list<ObjectID> *list);
// FUNCTION: BFME2 0x004F5385 size=12 name=?getContainMax@TunnelTracker@@QBEHXZ evidence=N:ledger
Int TunnelTracker::getContainMax() const
{
	return TheWritableGlobalData->m_maxTunnelCapacity;
}

// FUNCTION: BFME2 0x004F5391 size=18 name=?rva004F5391@TunnelTracker@@UAEX_N@Z evidence=?:placeholder
void TunnelTracker::adjustTunnelCount(Bool add)
{
	if (add)
		++m_unk0x0C;
	else
		--m_unk0x0C;
}

// FUNCTION: BFME2 0x004F53A3 size=32 name=?Rva004F53A3Destroy@@YAXPAVObject@@@Z evidence=?:placeholder

// FUNCTION: BFME2 0x004F53C3 size=115 name=?healObject@TunnelTracker@@SAXPAVObject@@PAX@Z evidence=N:ledger
void TunnelTracker::healObject(Object *obj, void *frames)
{
	Real *framesForFullHeal = (Real *)frames;

	DamageInfo healInfo;
	healInfo.in.m_damageType = DAMAGE_HEALING;
	healInfo.in.m_deathType = DEATH_NONE;

	BodyModuleInterface *body = obj->getBodyModule();

	if (TheGameLogic->getFrame() - obj->m_containedByFrame >= *framesForFullHeal)
	{
		healInfo.in.m_amount = body->getMaxHealth();

		body->attemptHealing(&healInfo);
	}
	else
	{
		healInfo.in.m_amount = body->getMaxHealth() / *framesForFullHeal;

		body->attemptHealing(&healInfo);
	}
}

// FUNCTION: BFME2 0x004F5436 size=61 name=?rva004F5436@Rva004F5436@@QAEHPAVBfmeTab1026@@@Z evidence=?:placeholder
Int TunnelTracker::getContainCount(const ObjectFilter *filter) const
{
	Int count = 0;
	if (filter == NULL)
		return m_containListSize;
	for (std::list<Object *>::const_iterator rider = m_containList.begin();
		rider != m_containList.end(); ++rider)
	{
		if (filter->accepts(*rider, NULL))
			++count;
	}
	return count;
}

// FUNCTION: BFME2 0x004F5473 size=76 name=?isValidContainerFor@TunnelTracker@@QAE_NPAVObject@@_N@Z evidence=N:ledger
Bool TunnelTracker::isValidContainerFor(Object *obj, Bool checkCapacity)
{
	if (obj != 0)
	{
		if (!obj->getTemplate()->isKindOf(KINDOF_AIRCRAFT))
		{
			if (!checkCapacity || obj->testStatus((ObjectStatusTypes)0x26))
				return true;
			int limit = TheWritableGlobalData->m_maxTunnelCapacity;
			int count = m_containListSize;
			return count < limit;
		}
	}
	return false;
}

// FUNCTION: BFME2 0x004F54BF size=75 name=?removeFromContain@TunnelTracker@@QAEXW4ObjectID@@H@Z evidence=N:ledger

// FUNCTION: BFME2 0x004F550A size=53 name=?isInContainer@TunnelTracker@@QAE_NW4ObjectID@@@Z evidence=N:ledger

// FUNCTION: BFME2 0x004F553F size=77 name=?iterateContained@TunnelTracker@@QAEXP6AXPAVObject@@PAX@Z1_N@Z evidence=N:ledger
void TunnelTracker::iterateContained(ContainIterateFunc func, void *userData, Bool reverse)
{
	if (reverse)
	{
		std::list<Object *>::iterator it = m_containList.end();
		if (it == m_containList.begin())
			return;
		do
		{
			--it;
			func(*it, userData);
		} while (it != m_containList.begin());
		return;
	}
	std::list<Object *>::iterator it = m_containList.begin();
	if (it == m_containList.end())
		return;
	do
	{
		Object *object = *it;
		++it;
		func(object, userData);
	} while (it != m_containList.end());
}

// FUNCTION: BFME2 0x004F558C size=20 name=?healObjects@TunnelTracker@@QAEXM@Z evidence=N:ledger
void TunnelTracker::healObjects(Real frames)
{
	iterateContained(healObject, &frames, false);
}

// FUNCTION: BFME2 0x004F55A0 size=119 name=??0TunnelTracker@@QAE@XZ evidence=N:ledger
TunnelTracker::TunnelTracker()
{
	// empty lists, no tunnels, nothing inside, no nemesis yet
	m_tunnelCount = 0;
	m_containListSize = 0;
	m_curNemesisID = INVALID_ID;
	m_nemesisTimestamp = 0;
	m_unk0x0C = 0;
}

// FUNCTION: BFME2 0x004F5617 size=6 name=?GetSnapshotName@TunnelTracker@@MBEPBDXZ evidence=N:ledger
const char *TunnelTracker::GetSnapshotName() const
{
	return "TunnelTracker";
}

// FUNCTION: BFME2 0x004F561D size=103 name=??1TunnelTracker@@MAE@XZ evidence=N:ledger
TunnelTracker::~TunnelTracker()
{
	m_tunnelIDs.clear();
}

// FUNCTION: BFME2 0x004F5684 size=97 name=?getCurNemesis@TunnelTracker@@QAEPAVObject@@XZ evidence=N:ledger
Object *TunnelTracker::getCurNemesis()
{
	if (m_curNemesisID == 0)
		return NULL;
	// a nemesis is forgotten four seconds after it was last seen
	if (m_nemesisTimestamp + BfmeLogicFramesPerSecond * 4 < TheGameLogic->getFrame())
	{
		m_curNemesisID = INVALID_ID;
		return NULL;
	}
	Object *nemesis = TheGameLogic->findObjectByID(m_curNemesisID);
	if (nemesis != NULL && nemesis->rva002943B2(NULL))
		nemesis = NULL;
	if (nemesis != NULL && nemesis->isEffectivelyDead())
		nemesis = NULL;
	if (nemesis == NULL)
		m_curNemesisID = INVALID_ID;
	return nemesis;
}

// FUNCTION: BFME2 0x004F56E5 size=23 name=?addToContainList@TunnelTracker@@QAEXPAVObject@@@Z evidence=N:ledger
void TunnelTracker::addToContainList(Object *obj)
{
	m_containList.push_back(obj);
	++m_containListSize;
}

// FUNCTION: BFME2 0x004F56FC size=31 name=?rva004F56FC@Rva004F56FC@@QAEXPAVObject@@@Z evidence=?:placeholder
void TunnelTracker::onTunnelCreated(const Object *newTunnel)
{
	++m_tunnelCount;
	m_tunnelIDs.push_back(newTunnel->getID());
}

// FUNCTION: BFME2 0x004F571B size=129 name=?rva004F571B@Rva004F56FC@@QAE_NPBVObject@@@Z evidence=?:placeholder

// FUNCTION: BFME2 0x004F579C size=192 name=?xfer@TunnelTracker@@MAEXPAVXfer@@@Z evidence=N:ledger
void TunnelTracker::xfer(Xfer *xfer)
{
	ObjectID objectID;
	if (xfer->IsLightCRC())
		return;
	xfer->Version1();
	Rva0036ABAFXfer(xfer, &m_tunnelIDs);
	*xfer == m_containListSize;
	if (xfer->IsStoring())
	{
		for (std::list<Object *>::const_iterator it = m_containList.begin(); it != m_containList.end(); ++it)
		{
			objectID = (*it)->getID();
			XferObjectID(xfer, &objectID);
		}
	}
	else
	{
		for (UnsignedShort i = 0; i < m_containListSize; ++i)
		{
			XferObjectID(xfer, &objectID);
			m_xferContainList.push_back(objectID);
		}
	}
	*xfer == m_tunnelCount;
	XferObjectID(xfer, &m_curNemesisID);
	*xfer == m_nemesisTimestamp;
}

// FUNCTION: BFME2 0x004F585C size=188 name=?loadPostProcess@TunnelTracker@@MAEXXZ evidence=N:ledger
void TunnelTracker::loadPostProcess()
{
	if (m_containList.size() != 0)
		throw XferException(5, 0);

	Object *obj;
	std::list<ObjectID>::const_iterator it;
	for (it = m_xferContainList.begin(); it != m_xferContainList.end(); ++it)
	{
		obj = TheGameLogic->findObjectByID(*it);
		if (obj == NULL)
			throw XferException(5, 0);

		m_containList.push_back(obj);
		obj->leaveGroup();
		obj->rva0028BAC0();

		if (obj->getDrawable())
			obj->getDrawable()->setDrawableHidden(true);

		if (TheAI)
			TheAI->pathfinder()->RemoveObjectFromPathfindMap(obj);
	}

	m_xferContainList.clear();
}

// FUNCTION: BFME2 0x004F5935 size=74 name=?updateNemesis@TunnelTracker@@QAEXPBVObject@@@Z evidence=N:ledger
void TunnelTracker::updateNemesis(const Object *target)
{
	if (getCurNemesis() == 0)
	{
		if (target == 0)
			return;
		const UnsignedInt mobileKinds = (1u << KINDOF_INFANTRY) | (1u << KINDOF_CAVALRY) | (1u << KINDOF_AIRCRAFT);
		const UnsignedInt buildingKind = 1u << KINDOF_STRUCTURE;
		UnsignedInt kind = target->getTemplate()->m_kindof.m_bits[0];
		if ((kind & mobileKinds) == 0 && (kind & buildingKind) == 0)
			return;
		m_curNemesisID = target->getID();
	}
	else if (getCurNemesis() != target)
	{
		return;
	}
	m_nemesisTimestamp = TheGameLogic->getFrame();
}

