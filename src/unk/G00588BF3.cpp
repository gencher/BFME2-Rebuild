// cl: /O1 /D_BFME_RETAIL_TREE_INSERT_LAYOUT /D_STLP_USE_MALLOC /D_STLP_USE_STATIC_LIB
// stlport46
#include <float.h>
#include <list>
#include "GameLogic/Module/HordeGarrisonContain.h"
#include "GameLogic/Module/HordeContainInterface.h"
#include "GameLogic/GameLogic.h"
#include "GameClient/ControlBar.h"
#include "GameClient/Drawable.h"
#include "GameClient/InGameUI.h"
#include "Common/MessageStream.h"
#include "Common/PlayerList.h"
#include "GameLogic/Object.h"
// Generated from the ledger; map snapshot c591f7146f88c8a8248ebb8ae22f0c978c22aed463db64e05ef57ff0dfae99cc
// object=G00588BF3 name-evidence=invented
// Extents and edges are hypotheses; byte verification alone does not establish original TU boundaries.
// FUNCTION: BFME2 0x00588BF3 size=91 name=?rva00588BF3@Rva0047A040Base9E0@@QAEPAVRva00588E44Contain@@PAXPAVObject@@@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
HordeContainInterface *HordeGarrisonContain::ExitState::findHordeWithRoom(HordeGarrisonContain *container, Object *unit)
{
	ContainmentListView view = container->getList();
	for (ContainmentList::const_iterator it = view.objects().begin(); it != view.objects().end(); ++it)
	{
		HordeContainInterface *horde = getContainedHorde(*it);
		if (horde != 0 && horde->hasSpotForUnit(unit) == TRUE)
			return horde;
	}
	return 0;
}
// FUNCTION: BFME2 0x00588C4E size=214 name=?rva00588C4E@Rva0047A040Base9E0@@QAEPAVObject@@PAXPBVCoord3D@@@Z evidence=X:retail PE exports class Coord3D: ??0Matrix4D@@QAE@ABVCoord3D@@@Z tier=B hypothesis:mm1-gap-unit
Object *HordeGarrisonContain::ExitState::getClosestRider(HordeGarrisonContain *container, const Coord3D *position)
{
	ContainmentListView occupants = container->getList();
	Object *closest = 0;
	Real closestDistanceSquared = FLT_MAX;
	for (ContainmentList::const_iterator occupant = occupants.objects().begin(); occupant != occupants.objects().end(); ++occupant)
	{
		const ContainModuleInterface *contain = (*occupant)->getContain();
		if (contain == 0)
			continue;
		const HordeContainInterface *horde = contain->getHordeContainInterface();
		if (horde == 0)
			continue;

		ContainmentListView riders = const_cast<HordeContainInterface *>(horde)->hordeSlot66();
		for (ContainmentList::const_iterator rider = riders.objects().begin(); rider != riders.objects().end(); ++rider)
		{
			Object *object = *rider;
			if (object->getDrawable()->isDrawableEffectivelyHidden() == TRUE)
				continue;

			const Coord3D *objectPosition = object->getPosition();
			Real dx = position->x - objectPosition->x;
			Real dy = position->y - objectPosition->y;
			Real distanceSquared = dx * dx + dy * dy;
			if (distanceSquared < closestDistanceSquared)
			{
				closest = object;
				closestDistanceSquared = distanceSquared;
			}
		}
	}
	return closest;
}

// FUNCTION: BFME2 0x00588D24 size=117 name=?rva00588D24@Rva0047A040Base9E0@@QAE_NPAXPAVObject@@@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
Bool HordeGarrisonContain::ExitState::regatherUnit(HordeGarrisonContain *container, Object *unit)
{
	HordeContainInterface *horde = findHordeWithRoom(container, unit);
	if (horde)
	{
		if (!horde->hordeSlot61())
		{
			horde->regatherUnit(unit);
			horde->hordeSlot64(TheGameLogic->getFrame());
			if (horde->hordeSlot61())
			{
				TheControlBar->m_uiDirty = true;
				horde->hordeSlot64(0);
			}
			return true;
		}
		horde->hordeSlot64(0);
	}
	return false;
}

// FUNCTION: BFME2 0x00588D99 size=135 name=?rva00588D99@Rva0047A040Base9E0@@QAEXPAVObject@@@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
void HordeGarrisonContain::ExitState::prepareNewOccupant(Object *object)
{
	Drawable *drawable = object->getDrawable();
	if (drawable && drawable->m_selected)
	{
		Player *localPlayer = ThePlayerList->getLocalPlayer();
		if (object->getControllingPlayer() == localPlayer)
		{
			GameMessage *message = TheMessageStream->appendMessage(GameMessage::MSG_REMOVE_FROM_SELECTED_GROUP);
			message->appendObjectIDArgument(object->getID());
			TheInGameUI->deselectDrawable(drawable);
		}
		object->setStatus(OBJECT_STATUS_UNSELECTABLE, true);
	}
	if (object->m_isRegisteredInWorld && !object->testStatus(OBJECT_STATUS_HOLDING_THE_RING))
		object->rva0028BAC0();
}
// FUNCTION: BFME2 0x00588E20 size=36 name=?rva00588E20@Rva0047A040Base9E0@@QAEXPAVObject@@@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
void HordeGarrisonContain::ExitState::restoreFormerOccupant(Object *object)
{
	if (!object->m_isRegisteredInWorld)
		object->rva0028DCC4();
	object->setStatus(OBJECT_STATUS_UNSELECTABLE, false);
}
// FUNCTION: BFME2 0x00588E44 size=285 name=?rva00588E44@Rva0047A040Base9E0@@QAEXPAX@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
// FUNCTION: BFME2 0x00588F61 size=153 name=?rva00588F61@Rva0047A040Base9E0@@QAEXPAXPAVObject@@W4CommandSourceType@@@Z evidence=?:placeholder tier=B hypothesis:mm1-gap-unit
