// Portions derived from Command & Conquer Generals and Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
// Object boundaries may be unproven.
// FUNCTION: BFME2 0x0048B97A size=58 name=?Rva0048B97A_InitObject@@YAXPAVINI@@PAXHH@Z evidence=?:placeholder tier=B hypothesis:mm1-held-object-same-path
// FUNCTION: BFME2 0x0048B9D0 size=74 name=?rva0048B9D0@FireWeaponUpdate@@QAEXPBVWeaponTemplate@@@Z evidence=?:placeholder
// FUNCTION: BFME2 0x0048BA3E size=105 name=?rva0048BA3E@FireWeaponUpdate@@IAE?AW4UpdateSleepTime@@XZ evidence=?:placeholder
// FUNCTION: BFME2 0x0048BAA7 size=332 name=?xfer@FireWeaponUpdate@@MAEXPAVXfer@@@Z evidence=N:ledger
// FUNCTION: BFME2 0x0048BBF3 size=16 name=?rva0048BBF3@Rva0048BBF3@@QAEXH@Z evidence=?:placeholder
// FUNCTION: BFME2 0x0048BC03 size=67 name=??0FireWeaponUpdateModuleData@@QAE@XZ evidence=N:ledger
// FUNCTION: BFME2 0x0048BC46 size=117 name=??1FireWeaponUpdateModuleData@@UAE@XZ evidence=N:ledger
// FUNCTION: BFME2 0x0048BCBB size=86 name=?Rva0048BCBBParse@@YAXPAVINI@@PAX1PBX@Z evidence=?:placeholder
// FUNCTION: BFME2 0x0048BD11 size=156 name=??1FireWeaponUpdate@@UAE@XZ evidence=N:ledger
// FUNCTION: BFME2 0x0048BDAD size=6 name=?GetSnapshotName@FireWeaponUpdate@@MBEPBDXZ evidence=N:ledger
// FUNCTION: BFME2 0x0048BDB3 size=69 name=?getModuleNameKey@FireWeaponUpdate@@UBE?AW4NameKeyType@@XZ evidence=N:ledger
// FUNCTION: BFME2 0x0048BDF8 size=240 name=?Rva0048BDF8Helper@FireWeaponUpdate@@QAEXXZ evidence=?:placeholder
// FUNCTION: BFME2 0x0048BEE8 size=432 name=?update@FireWeaponUpdate@@UAE?AW4UpdateSleepTime@@XZ evidence=N:ledger
// FUNCTION: BFME2 0x0048C0B4 size=17 name=?buildFieldParse@FireWeaponUpdateModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger
// stlport46
#include <list>
#include "GameLogic/Module/FireWeaponUpdate.h"
#include "Common/INI.h"
#include "GameLogic/Object.h"
#include "GameLogic/Weapon.h"
#include "GameLogic/WeaponStore.h"
// GameLogic/Object/Update/FireWeaponUpdate.cpp: retail translation unit (skeleton).
// cl: /EHsc- -Ireference/shims/bfmealloc_alloconly /D_STLP_USE_STATIC_LIB
#include "Common/Xfer.h"
#include "WWMath/Vector3.h"
#include <math.h>
__forceinline void Vector3::Rotate_Z(Real angle)
{
 Rotate_Z(sinf(angle),cosf(angle));
}
#include "GameLogic/Module/FireWeaponUpdateEntry.h"
#include "Common/StlAlgobase.h"
#include "GameLogic/GameLogic.h"

// Replaces the module's weapon: deletes the old one, allocates a primary from
// the store, copies the owner id in and loads its ammo.
void FireWeaponUpdate::rva0048B9D0(const WeaponTemplate *weaponTemplate)
{
	if (weaponTemplate == 0)
		return;
	if (m_weapon != 0)
		::delete m_weapon;
	Weapon *weapon = TheWeaponStore->allocateNewWeapon(weaponTemplate, PRIMARY_WEAPON);
	m_weapon = weapon;
	weapon->m_ownerID = m_object->getID();
	m_weapon->loadAmmoNow(m_object);
}


FireWeaponUpdateModuleData::FireWeaponNugget::FireWeaponNugget()
	: m_fireDelay(0)
	, m_oneShot(false)
{
	m_offset.set(0.0f, 0.0f, 0.0f);
}


// The larger of two values by reference; retail keeps the first unless the second is larger.
static inline const UnsignedInt &maxOf(const UnsignedInt &a, const UnsignedInt &b)
{
	return a > b ? a : b;
}

UpdateSleepTime FireWeaponUpdate::rva0048BA3E()
{
	UnsignedInt next = 0xFFFFFFFF;
	for (_STL::list<int>::iterator it = m_entries.begin(); it != m_entries.end(); ++it)
	{
		FireWeaponUpdateEntry *entry = (FireWeaponUpdateEntry *)*it;
		UnsignedInt frame = maxOf(entry->m_nextFireFrame, entry->m_weapon->getPossibleNextShotFrame());
		next = _STL::min(frame, next);
	}
	if (next == 0xFFFFFFFF)
		return UPDATE_SLEEP_FOREVER;
	UnsignedInt now = TheGameLogic->getFrame();
	if (next <= now)
		return UPDATE_SLEEP_NONE;
	return (UpdateSleepTime)(next - now);
}


void FireWeaponUpdate::xfer(Xfer *xfer)
{
	Xfer::Version version(1, 2);
	*xfer == version;
	UpdateModule::xfer(xfer);
	if (!xfer->IsLightCRC())
	{
		Bool hasWeapon = m_weapon != NULL;
		*xfer == hasWeapon;
		if (hasWeapon)
		{
			if (xfer->IsLoading())
				m_weapon = TheWeaponStore->allocateNewWeapon(NULL, PRIMARY_WEAPON);
			*xfer == *m_weapon;
		}
	}
	if (version.m_minimum >= 2)
	{
		*xfer == m_entriesReady;
		if (xfer->IsLoading())
		{
			for (_STL::list<int>::iterator it = m_entries.begin(); it != m_entries.end(); ++it)
			{
				FireWeaponUpdateEntry *entry = (FireWeaponUpdateEntry *)*it;
				*xfer == entry->m_oneShot;
				*xfer == entry->m_nextFireFrame;
				*xfer == entry->m_offset;
				*xfer == *entry->m_weapon;
			}
		}
		else if (xfer->IsStoring())
		{
			for (_STL::list<int>::iterator it = m_entries.begin(); it != m_entries.end(); ++it)
			{
				FireWeaponUpdateEntry *entry = (FireWeaponUpdateEntry *)*it;
				*xfer == entry->m_oneShot;
				*xfer == entry->m_nextFireFrame;
				*xfer == entry->m_offset;
				*xfer == *entry->m_weapon;
			}
		}
	}
}



// Appends one parsed FireWeaponNugget pointer to the module data's list at
// +0x08; retail reaches this from the nugget parser 0x0048BCBB.
void FireWeaponUpdateModuleData::addNugget(int nugget)
{
	m_nuggets.push_back(nugget);
}


FireWeaponUpdateModuleData::FireWeaponUpdateModuleData()
	: m_nuggets(_STL::allocator<int>())
{
	m_heroModeTrigger = false;
	m_chargingModeTrigger = false;
	m_aliveOnly = false;
}

// Releases each nugget (its first member is the WeaponName string) and then the
// list itself.
FireWeaponUpdateModuleData::~FireWeaponUpdateModuleData()
{
	_STL::list<int>::iterator it = m_nuggets.begin();
	while (it != m_nuggets.end())
	{
		int nugget = *it;
		++it;
		if (nugget != 0)
		{
			((AsciiString *)nugget)->~AsciiString();
			::operator delete((void *)nugget);
		}
		m_nuggets.remove(nugget);
	}
}

// The nugget block proc: news a nugget, fills it through the rowed
// 0x0048B97A single-entry parse list and appends it to the module data's list.
void Rva0048B97A_InitObject(INI *ini, void *obj, Int unusedA, Int unusedB);

void Rva0048BCBBParse(INI *ini, void *instance, void *store, const void *userData)
{
	FireWeaponUpdateModuleData::FireWeaponNugget *nugget = new FireWeaponUpdateModuleData::FireWeaponNugget;
	Rva0048B97A_InitObject(ini, nugget, 0, 0);
	((FireWeaponUpdateModuleData *)instance)->m_nuggets.push_back((Int)nugget);
}

FireWeaponUpdate::~FireWeaponUpdate()
{
	_STL::list<int>::iterator it = m_entries.begin();
	while (it != m_entries.end())
	{
		int value = *it;
		++it;
		if (value != 0)
		{
			((FireWeaponUpdateEntry *)value)->~FireWeaponUpdateEntry();
			::operator delete((void *)value);
		}
		m_entries.remove(value);
	}
	if (m_weapon != NULL)
		::delete m_weapon;
}



const char *FireWeaponUpdate::GetSnapshotName() const
{
	return "FireWeaponUpdate";
}


NameKeyType FireWeaponUpdate::getModuleNameKey() const
{
	static NameKeyType TheFireWeaponUpdatePoolKey = TheNameKeyGenerator->nameToKey("FireWeaponUpdate");
	return TheFireWeaponUpdatePoolKey;
}

void FireWeaponUpdate::Rva0048BDF8Helper()
{
	const FireWeaponUpdateModuleData *data = getFireWeaponUpdateModuleData();
	UnsignedInt now = TheGameLogic ? TheGameLogic->getFrame() : 0;
	for (_STL::list<int>::const_iterator it = data->m_nuggets.begin(); it != data->m_nuggets.end(); ++it)
	{
		const FireWeaponUpdateModuleData::FireWeaponNugget *nugget = (const FireWeaponUpdateModuleData::FireWeaponNugget *)*it;
		const WeaponTemplate *weaponTemplate = TheWeaponStore->findWeaponTemplate(nugget->m_weaponName);
		if (weaponTemplate)
		{
			FireWeaponUpdateEntry *entry = new FireWeaponUpdateEntry;
			entry->m_oneShot = nugget->m_oneShot;
			entry->m_nextFireFrame = nugget->m_fireDelay + now;
			entry->m_offset = nugget->m_offset;
			int listValue = (int)entry;
			entry->m_weapon = TheWeaponStore->allocateNewWeapon(weaponTemplate, PRIMARY_WEAPON);
			entry->m_weapon->setOwnerID(getObject()->getID());
			entry->m_weapon->loadAmmoNow(getObject());
			m_entries.push_back(listValue);
		}
	}
	m_entriesReady = true;
}


UpdateSleepTime FireWeaponUpdate::update()
{
	const FireWeaponUpdateModuleData *data = getFireWeaponUpdateModuleData();
	if (data->m_heroModeTrigger && !getObject()->getModelConditionFlags().testShifted(MODELCONDITION_220))
		return rva0048BA3E();
	if (data->m_chargingModeTrigger && !getObject()->getModelConditionFlags().testShifted(MODELCONDITION_132))
		return rva0048BA3E();
	if (data->m_aliveOnly && getObject()->isEffectivelyDead())
		return rva0048BA3E();

	if (m_weapon)
	{
		if (m_weapon->getStatus() == READY_TO_FIRE)
			m_weapon->forceFireWeapon(getObject(), getObject()->getPosition());
	}
	else
	{
		UnsignedInt now = TheGameLogic->getFrame();
		for (_STL::list<int>::iterator it = m_entries.begin(); it != m_entries.end(); ++it)
		{
			FireWeaponUpdateEntry *entry = (FireWeaponUpdateEntry *)*it;
			if (now > entry->m_nextFireFrame && entry->m_weapon->getStatus() == READY_TO_FIRE)
			{
				Object *me = getObject();
				Coord3D pos;
				pos.set(me->getPosition());
				if (entry->m_offset.lengthSqr() > 0.0f)
				{
					Real angle = me->getOrientation();
					Vector3 offset(entry->m_offset.x, entry->m_offset.y, 0.0f);
					offset.Rotate_Z(angle);
					Coord3D delta;
					delta.set(offset.X, offset.Y, 0.0f);
					pos.add(&delta);
				}
				entry->m_weapon->forceFireWeapon(getObject(), &pos);
				if (entry->m_oneShot)
					entry->m_nextFireFrame = 0xFFFFFFFF;
			}
		}
	}
	return rva0048BA3E();
}




void FireWeaponUpdateModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	// Zero Hour's shape: the module data's table is this function's static (retail .rdata 0x00C4C210).
	static const FieldParse dataFieldParse[] =
	{
		{ "FireWeaponNugget", Rva0048BCBBParse, NULL, 0 },
		{ "HeroModeTrigger", INI::parseBool, NULL, offsetof(FireWeaponUpdateModuleData, m_heroModeTrigger) },
		{ "ChargingModeTrigger", INI::parseBool, NULL, offsetof(FireWeaponUpdateModuleData, m_chargingModeTrigger) },
		{ "AliveOnly", INI::parseBool, NULL, offsetof(FireWeaponUpdateModuleData, m_aliveOnly) },
		{ 0, 0, 0, 0 }
	};
	parse.add(dataFieldParse, 0);
}

FireWeaponUpdate::FireWeaponUpdate(Thing *thing, const ModuleData *moduleData)
	: UpdateModule(thing, moduleData)
	, m_weapon(NULL)
	, m_entriesReady(false)
{
	Rva0048BDF8Helper();
	Object **objectSlot = &m_object;
	setWakeFrame(*objectSlot, UPDATE_SLEEP_NONE);
}

// Retail 0x00497805 (the shared `jmp 0x0058B03E` fold of the module classes'
// own forwarders): out of line, because this class's subclasses have their own
// forwarder (0x00483CEF, `jmp 0x00497805`) that calls it.
void FireWeaponUpdate::loadPostProcess()
{
	UpdateModule::loadPostProcess();
}
