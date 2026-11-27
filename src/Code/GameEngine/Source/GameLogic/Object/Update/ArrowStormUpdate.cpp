#include "GameLogic/Module/ArrowStormUpdate.h"
#include "GameLogic/Module/SpecialAbilityUpdate.h"
// Object boundaries may be unproven.
// FUNCTION: BFME2 0x00490628 size=17 name=?continuePreparation@ArrowStormUpdate@@UAE_NXZ evidence=N:ledger
bool ArrowStormUpdate::continuePreparation()
{
	if (m_finished)
		return false;
	return SpecialAbilityUpdate::continuePreparation();
}

// FUNCTION: BFME2 0x00490639 size=64 name=??0ArrowStormUpdateModuleData@@QAE@XZ evidence=N:ledger
// FUNCTION: BFME2 0x00490695 size=56 name=??1ArrowStormUpdateModuleData@@UAE@XZ evidence=N:ledger
// FUNCTION: BFME2 0x004906CD size=213 name=?vslot08@ArrowStormUpdate@@UAE_NH@Z evidence=N:ledger
// FUNCTION: BFME2 0x004907C0 size=49 name=??1ArrowStormUpdate@@UAE@XZ evidence=N:ledger
// FUNCTION: BFME2 0x004907F1 size=6 name=?name@ArrowStormUpdate@@QBEPBDXZ evidence=N:ledger
// FUNCTION: BFME2 0x004907F7 size=69 name=?getModuleNameKey@ArrowStormUpdate@@UBE?AW4NameKeyType@@XZ evidence=N:ledger
// FUNCTION: BFME2 0x0049083C size=411 name=?rva0049083C@ArrowStormUpdate@@AAE_NXZ evidence=?:placeholder
// FUNCTION: BFME2 0x004909D7 size=129 name=??0ArrowStormUpdate@@QAE@PAVThing@@PBVModuleData@@@Z evidence=N:ledger
// FUNCTION: BFME2 0x00490A74 size=81 name=?triggerAbilityEffect@ArrowStormUpdate@@UAEXXZ evidence=N:ledger
// FUNCTION: BFME2 0x00490AC5 size=470 name=?rva00490AC5@ArrowStormUpdate@@QAEXXZ evidence=?:placeholder
// FUNCTION: BFME2 0x00490C9B size=111 name=?xfer@ArrowStormUpdate@@MAEXPAVXfer@@@Z evidence=N:ledger
// FUNCTION: BFME2 0x00490D0A size=53 name=?startPreparation@ArrowStormUpdate@@UAEXXZ evidence=N:ledger
// cl: /D_BFME_RETAIL_TREE_INSERT_LAYOUT /D_STLP_USE_MALLOC /D_STLP_USE_STATIC_LIB
// stlport46
#include "Common/ObjectIDList.h"
#include "GameLogic/PartitionManager.h"
#include "GameLogic/PartitionFilterAlive.h"
#include "GameLogic/PartitionFilterSameMapStatus.h"
#include "GameLogic/PartitionFilterRelationship.h"
#include "GameLogic/WeaponStore.h"
#include "GameLogic/WeaponTemplate.h"
#include "GameLogic/WeaponBonus.h"
#include "GameLogic/GameLogic.h"
#include "GameLogic/TerrainLogic.h"
#include "GameLogic/Object.h"
#include "Common/ThingTemplate.h"
float GetGameLogicRandomValueReal(float,float,char*,int);
#include "GameLogic/Module/ArrowStormUpdate.h"
#include "Common/Xfer.h"
Xfer *xferSTLObjectIDList(Xfer *, ObjectIDList *);
// GameLogic/Object/Update/ArrowStormUpdate.cpp: retail translation unit (skeleton).


ArrowStormUpdateModuleData::ArrowStormUpdateModuleData()
	: m_targetRadius(0.0f), m_shotsPerTarget(1), m_shotsPerBurst(1), m_maxShots(1), m_canShootEmptyGround(false) {}





bool ArrowStormUpdate::vslot08(const Coord3D *location)
{
	const ArrowStormUpdateModuleData *mod = (const ArrowStormUpdateModuleData *)m_moduleData;
	if (!mod) return false;
	if (!location) return SpecialAbilityUpdate::vslot08(0);
	Object *obj = m_object;
	if (!obj) return false;
	const WeaponTemplate *wt = TheWeaponStore->findWeaponTemplate(mod->m_weaponTemplate);
	if (!wt) return false;
	float minR = wt->getMinimumAttackRange();
	if (obj->rva002C97E8(&obj->m_cachedPos,location) < minR * minR) return false;
	if (obj->m_template->isKindOf(KINDOF_IMMOBILE)) {
		WeaponBonus bonus;
		float range = wt->getAttackRange(obj,bonus,&obj->m_cachedPos);
		if (obj->rva002C97E8(&obj->m_cachedPos,location) > range * range) return false;
	}
	return SpecialAbilityUpdate::vslot08(location);
}




ArrowStormUpdate::~ArrowStormUpdate() {}


const char *ArrowStormUpdate::GetSnapshotName() const
{
	return "ArrowStormUpdate";
}


NameKeyType ArrowStormUpdate::getModuleNameKey() const
{
	static NameKeyType TheArrowStormUpdatePoolKey = TheNameKeyGenerator->nameToKey("ArrowStormUpdate");
	return TheArrowStormUpdatePoolKey;
}


bool ArrowStormUpdate::rva0049083C()
{
	Object *owner=m_object;
	const ArrowStormUpdateModuleData *arrowStormUpdateModuleData=(const ArrowStormUpdateModuleData *)m_moduleData;
	if(m_shotsAtCurrent>=arrowStormUpdateModuleData->m_shotsPerTarget){m_currentTarget=INVALID_ID;m_shotsAtCurrent=0;}
	Object *victim=0;
	if(m_currentTarget)victim=TheGameLogic->findObjectByID(m_currentTarget);
	while(!victim && !m_targets.empty()){
		m_currentTarget=(ObjectID)m_targets.front();
		m_shotsAtCurrent=0;
		m_targets.pop_front();
		victim=TheGameLogic->findObjectByID(m_currentTarget);
		if(victim && (victim->m_privateStatus&1))victim=0;
	}
	const WeaponTemplate *weapon=TheWeaponStore->findWeaponTemplate(arrowStormUpdateModuleData->m_weaponTemplate);
	if(!victim){
		if(arrowStormUpdateModuleData->m_canShootEmptyGround){
			Coord3D position;
			position.x=GetGameLogicRandomValueReal(m_targetPos.x-arrowStormUpdateModuleData->m_targetRadius,m_targetPos.x+arrowStormUpdateModuleData->m_targetRadius,"C:\\projects\\bfme2patch103\\bfme2\\Code\\GameEngine\\Source\\GameLogic\\Object\\Update\\ArrowStormUpdate.cpp",217);
			position.y=GetGameLogicRandomValueReal(m_targetPos.y-arrowStormUpdateModuleData->m_targetRadius,m_targetPos.y+arrowStormUpdateModuleData->m_targetRadius,"C:\\projects\\bfme2patch103\\bfme2\\Code\\GameEngine\\Source\\GameLogic\\Object\\Update\\ArrowStormUpdate.cpp",218);
			position.z=TheTerrainLogic->getGroundHeight(position.x,position.y);
			TheWeaponStore->createAndFireTempWeapon(weapon,owner,&position);
		}else return true;
	}else{
		TheWeaponStore->rva002CE964(weapon,owner,victim);
		++m_shotsAtCurrent;
	}
	++m_shotsTotal; if(m_shotsTotal>=arrowStormUpdateModuleData->m_maxShots)return true;return false;
}


ArrowStormUpdate::ArrowStormUpdate(Thing *thing, const ModuleData *moduleData)
	: SpecialAbilityUpdate(thing, moduleData)
{
	m_targets.clear();
	m_currentTarget = INVALID_ID;
	m_shotsAtCurrent = 0;
	m_shotsTotal = 0;
	m_finished = false;
}



void ArrowStormUpdate::triggerAbilityEffect()
{
	SpecialAbilityUpdate::triggerAbilityEffect();
	const ArrowStormUpdateModuleData *data = (const ArrowStormUpdateModuleData *)m_moduleData;
	if (!data->m_canShootEmptyGround && m_targets.empty()) { m_finished = true; return; }
	for (int i = 0; i < data->m_shotsPerBurst; ) {
		bool done = rva0049083C();
		++i;
		m_finished = done;
		if (done) break;
	}
}


void ArrowStormUpdate::rva00490AC5()
{
	Object *obj = m_object;
	const ArrowStormUpdateModuleData *data = (const ArrowStormUpdateModuleData *)m_moduleData;
	ObjectIterator hits = ThePartitionManager->iterateObjectsInRange(&m_targetPos, data->m_targetRadius, 0,
		PartitionFilterRelationship(obj, 1, false).link(PartitionFilterAlive().link(&PartitionFilterSameMapStatus(obj))), 1);
	ObjectIDList extra;
	Object *other;
	while ((other = hits.next()) != 0) {
		if ((other->getTemplate()->isKindOf(KINDOF_BASE_FOUNDATION)) || (other->getTemplate()->isKindOf(KINDOF_INERT))
				|| (other->getTemplate()->isKindOf(KINDOF_IGNORED_IN_GUI)) || (other->getTemplate()->isKindOf(KINDOF_WALL_UPGRADE))
				|| (other->getTemplate()->isKindOf(KINDOF_UNATTACKABLE)) || (other->getTemplate()->isKindOf(KINDOF_MOVE_ONLY)))
			continue;
		m_targets.push_back(other->getID());
		if ((other->getTemplate()->isKindOf(KINDOF_MONSTER)))
			extra.push_back(other->getID());
	}
	if (m_targets.size() == 0)
		return;
	if (m_targets.size() < data->m_maxShots) {
		ObjectIDList *source = &m_targets;
		if (!extra.empty())
			source = &extra;
		ObjectIDListIterator it = source->begin();
		while (m_targets.size() < data->m_maxShots) {
			m_targets.push_back(*it);
			++it;
			if (it == source->end())
				it = source->begin();
		}
	}
	extra.clear();
}


void ArrowStormUpdate::xfer(Xfer *xfer)
{
	SpecialAbilityUpdate::xfer(xfer);
	if (xfer->IsLightCRC()) return;
	xfer->Version1();
	xferSTLObjectIDList(xfer, &m_targets);
	XferObjectID(xfer, &m_currentTarget);
	*xfer == m_shotsAtCurrent;
	*xfer == m_finished;
	*xfer == m_shotsTotal;
}


void ArrowStormUpdate::startPreparation()
{
	SpecialAbilityUpdate::startPreparation();
	m_targets.clear();
	m_currentTarget = INVALID_ID;
	m_shotsAtCurrent = 0;
	m_shotsTotal = 0;
	m_finished = false;
	rva00490AC5();
}

void ArrowStormUpdate::loadPostProcess()
{
	SpecialAbilityUpdate::loadPostProcess();	// retail jumps to 0x00497805, SpecialAbilityUpdate's
}
