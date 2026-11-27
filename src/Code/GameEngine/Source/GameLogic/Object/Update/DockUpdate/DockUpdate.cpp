// Portions derived from Command & Conquer Generals and Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
// Object boundaries may be unproven.
// FUNCTION: BFME2 0x0058964F size=18 name=?isClearToEnter@DockUpdate@@UBE_NPBVObject@@@Z evidence=N:ledger
// FUNCTION: BFME2 0x00589661 size=10 name=?setDockCrippled@DockUpdate@@UAEX_N@Z evidence=N:ledger
// FUNCTION: BFME2 0x005896B0 size=17 name=??0DockUpdateModuleData@@QAE@XZ evidence=N:ledger
// FUNCTION: BFME2 0x005896C1 size=17 name=?buildFieldParse@DockUpdateModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger
// FUNCTION: BFME2 0x005896D9 size=73 name=?isClearToApproach@DockUpdate@@UBE_NPBVObject@@@Z evidence=N:ledger
// FUNCTION: BFME2 0x00589722 size=93 name=?onDockReached@DockUpdate@@UAEXPAVObject@@@Z evidence=N:ledger
// FUNCTION: BFME2 0x0058977F size=124 name=?onExitReached@DockUpdate@@UAEXPAVObject@@@Z evidence=N:ledger
// FUNCTION: BFME2 0x005897FB size=270 name=?loadDockPositions@DockUpdate@@IAEXXZ evidence=N:ledger
// FUNCTION: BFME2 0x00589909 size=22 name=?loadPostProcess@DockUpdate@@MAEXXZ evidence=N:ledger
// FUNCTION: BFME2 0x0058991F size=83 name=?isClearToAdvance@DockUpdate@@UBE_NPBVObject@@H@Z evidence=N:ledger
// FUNCTION: BFME2 0x00589972 size=121 name=?getEnterPosition@DockUpdate@@UAEXPAVObject@@PAUCoord3D@@@Z evidence=N:ledger
// FUNCTION: BFME2 0x005899EB size=99 name=?getDockPosition@DockUpdate@@UAEXPAVObject@@PAUCoord3D@@@Z evidence=N:ledger
// FUNCTION: BFME2 0x00589A4E size=99 name=?getExitPosition@DockUpdate@@UAEXPAVObject@@PAUCoord3D@@@Z evidence=N:ledger
// FUNCTION: BFME2 0x00589AB1 size=78 name=?onApproachReached@DockUpdate@@UAEXPAVObject@@@Z evidence=N:ledger
// FUNCTION: BFME2 0x00589AFF size=126 name=?rva00589AFF@DockUpdate@@QAEXH@Z evidence=?:placeholder
// FUNCTION: BFME2 0x00589B7D size=449 name=?computeApproachPosition@DockUpdate@@IAE?AUCoord3D@@HPAVObject@@@Z evidence=N:ledger
// FUNCTION: BFME2 0x00589D3E size=159 name=?advanceApproachPosition@DockUpdate@@UAE_NPAVObject@@PAUCoord3D@@PAH@Z evidence=N:ledger
// FUNCTION: BFME2 0x00589DDD size=154 name=?onEnterReached@DockUpdate@@UAEXPAVObject@@@Z evidence=N:ledger
// FUNCTION: BFME2 0x00589E77 size=146 name=?cancelDock@DockUpdate@@UAEXPAVObject@@@Z evidence=N:ledger
// FUNCTION: BFME2 0x00589F09 size=258 name=?update@DockUpdate@@UAE?AW4UpdateSleepTime@@XZ evidence=N:ledger
// FUNCTION: BFME2 0x0058A0F4 size=125 name=??1DockUpdate@@UAE@XZ evidence=N:ledger
// FUNCTION: BFME2 0x0058A171 size=6 name=?name@DockUpdate@@QBEPBDXZ evidence=N:ledger
// FUNCTION: BFME2 0x0058A193 size=14 name=?rva0058A193@Rva0058A193@@QAEXI@Z evidence=?:placeholder
// FUNCTION: BFME2 0x0058A1A1 size=239 name=?reserveApproachPosition@DockUpdate@@UAE_NPAVObject@@PAUCoord3D@@PAH@Z evidence=N:ledger
// FUNCTION: BFME2 0x0058A290 size=384 name=??0DockUpdate@@QAE@PAVThing@@PBVModuleData@@@Z evidence=N:ledger
// FUNCTION: BFME2 0x0058A410 size=439 name=?xfer@DockUpdate@@MAEXPAVXfer@@@Z evidence=N:ledger

// cl: /Ob2 /EHsc /D_STLP_USE_STATIC_LIB -Ireference/shims/bfmealloc_alloconly

// stlport46

// Retail's DockUpdate object also carries vector<ObjectID>'s growth helpers
// (_M_fill_insert, insert) and the two algorithms they instantiate, so this TU
// compiles against the real STLport tree rather than a hand-written copy of it.
// vector::_M_insert_overflow inlines max(size(), n); a file-static unsigned
// overload takes that call, so the unit emits no external max COMDAT.
#define _STLP_USE_MALLOC 1
#include <stl/_algobase.h>
namespace _STL {
static inline const unsigned int &max(const unsigned int &a, const unsigned int &b)
{
	return a < b ? a : b;
}
}
#define _STLP_BFME_COPY_TRIVIAL_EXTERN
#include <vector>
#include <stdlib.h>

#include "Common/INI.h"
#include "Common/Xfer.h"
#include "GameLogic/Module/DockUpdate.h"
#include "GameLogic/Object.h"
#include "GameLogic/GameLogic.h"
#include "GameLogic/PartitionManager.h"
#include "WWMath/Vector3.h"
#include "Common/BfmeConditionMasks.h"
#include "Common/ModelConditionFlags.h"
#include "Common/ModelConditionMask.h"
#include "Common/ThingTemplate.h"
#include "GameClient/Drawable.h"

// Retail releases every container buffer through the game's own deallocator at
// 0x00030830, an out-of-line function the compiler cannot see through, so the
// destructor keeps one unwind state per buffer and frees inline. Binding the
// three base teardowns to it reproduces both, exactly as retail's ~DockUpdate
// (0x0058A0F4) shows: three guarded frees at +0x6C, +0x60 and +0x54.
void __cdecl Rva00030830GameFree(void *);

namespace _STL
{
template <> _Vector_base<Coord3D, allocator<Coord3D> >::~_Vector_base()
{
	if (_M_start)
		Rva00030830GameFree(_M_start);
}
template <> _Vector_base<ObjectID, allocator<ObjectID> >::~_Vector_base()
{
	if (_M_start)
		Rva00030830GameFree(_M_start);
}
template <> _Bvector_base<allocator<bool> >::~_Bvector_base()
{
	if (_M_start._M_p)
		Rva00030830GameFree(_M_start._M_p);
}
}


Bool DockUpdate::isClearToEnter(const Object *docker) const
{
	return docker->getID() == m_activeDocker;
}


void DockUpdate::setDockCrippled(Bool setting)
{
	m_dockCrippled = setting;
}




DockUpdateModuleData::DockUpdateModuleData()
{
	m_numberApproachPositionsData = 0;
	m_isAllowPassthrough = true;
}


const FieldParse BfmeDockUpdateFields[] =
{
	{ "NumberApproachPositions", &INI::parseInt, 0, 0x8 },
	{ "AllowsPassthrough", &INI::parseBool, 0, 0xC },
	{ 0, 0, 0, 0 }
};

void DockUpdateModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	parse.add(BfmeDockUpdateFields, 0);
}


unsigned char Rva005896D2ByteChaseField::get() const
{
	return ((const Rva005896D2Sub *)((const char *)this - 0x1C))->m_holder->m_value;
}


Bool DockUpdate::isClearToApproach(const Object *docker) const
{
	if (m_numberApproachPositions == DYNAMIC_APPROACH_VECTOR_FLAG)
		return true;

	ObjectID dockerID = docker->getID();
	for (UnsignedInt positionIndex = 0;
		positionIndex < m_approachPositionOwners.size(); ++positionIndex)
	{
		if (m_approachPositionOwners[positionIndex] == INVALID_ID)
			return true;
		if (m_approachPositionOwners[positionIndex] == dockerID)
			return true;
	}

	return false;
}


void DockUpdate::onDockReached(Object *docker)
{
	getObject()->clearAndSetCondition(82, 83);
	docker->clearAndSetCondition(82, 83);
}


void DockUpdate::onExitReached(Object *docker)
{
	Object *me = getObject();
	me->rva0028CFB2(
		(const int *)Rva001E4912().rva001E4912(0, 0x53, 0x51),
		(const int *)&ModelConditionFlags(ModelConditionFlags::kInit, 0x54));
	docker->rva0028CFB2(
		(const int *)Rva001E4912().rva001E4912(0, 0x53, 0x51),
		(const int *)&ModelConditionFlags(ModelConditionFlags::kInit, 0x54));
	m_dockerInside = false;
	if (docker->getID() == m_activeDocker)
		m_activeDocker = INVALID_ID;
	else
		isDockOpen();
}


void DockUpdate::loadDockPositions()
{
	Object *obj = getObject();
	Drawable *myDrawable = obj->getDrawable();

	if (myDrawable != NULL)
	{
		myDrawable->getPristineBonePositions("DockStart", 0, &m_enterPosition, NULL, 1, 0);
		myDrawable->getPristineBonePositions("DockAction", 0, &m_dockPosition, NULL, 1, 0);
		myDrawable->getPristineBonePositions("DockEnd", 0, &m_exitPosition, NULL, 1, 0);
		if (m_numberApproachPositions != DYNAMIC_APPROACH_VECTOR_FLAG)
		{
			Rva005897FBCoordLifetime approachBones[DEFAULT_APPROACH_VECTOR_SIZE];
			m_numberApproachPositionBones = myDrawable->getPristineBonePositions("DockWaiting", 1, approachBones, NULL, m_numberApproachPositions, 0);
			if (m_numberApproachPositions == (Int)m_approachPositions.size())
			{
				for (Int copyIndex = 0; copyIndex < m_numberApproachPositions; ++copyIndex)
				{
					*(m_approachPositions.begin() + copyIndex) = approachBones[copyIndex];
				}
			}
		}
		else
		{
			m_numberApproachPositionBones = 0;
		}

		m_positionsLoaded = true;
	}
}


void DockUpdate::loadPostProcess()
{
	if (m_positionsLoaded)
		loadDockPositions();

	UpdateModule::loadPostProcess();
}


Bool DockUpdate::isClearToAdvance(const Object *docker, Int dockerIndex) const
{
	if (dockerIndex < 0)
		return false;

	ObjectID dockerID = docker->getID();
	Bool correctRequest = dockerID == m_approachPositionOwners[dockerIndex];
	Bool approachReached = m_approachPositionReached[dockerIndex];
	Bool nextSpotFree = (dockerIndex > 0) && (m_approachPositionOwners[dockerIndex - 1] == INVALID_ID);

	return correctRequest && approachReached && nextSpotFree;
}


void DockUpdate::getEnterPosition(Object *docker, Coord3D *position)
{
	// load dock positions if not loaded yet
	if (m_positionsLoaded == false)
		loadDockPositions();

	// sanity
	if (position == NULL)
		return;

	// If I don't have a bone, you are fine where you are, unless you fly, in which case I should recenter you
	Coord3D zero;
	zero.zero();
	if (m_enterPosition == zero)
	{
		if (docker->isUsingAirborneLocomotor())
		{
			*position = *getObject()->getPosition();
			return;
		}
		*position = *docker->getPosition();
		return;
	}

	// take local space position and convert to world space
	getObject()->convertBonePosToWorldPos(&m_enterPosition, NULL, position, NULL);
}


void DockUpdate::getDockPosition(Object *docker, Coord3D *position)
{
	// load dock positions if not loaded yet
	if (m_positionsLoaded == false)
		loadDockPositions();

	// sanity
	if (position == NULL)
		return;

	// If I don't have a bone, you are fine where you are.
	Coord3D zero;
	zero.zero();
	if (m_enterPosition == zero)
	{
		*position = *docker->getPosition();
		return;
	}

	// take local space position and convert to world space
	getObject()->convertBonePosToWorldPos(&m_dockPosition, NULL, position, NULL);
}


void DockUpdate::getExitPosition(Object *docker, Coord3D *position)
{
	// load dock positions if not loaded yet
	if (m_positionsLoaded == false)
		loadDockPositions();

	// sanity
	if (position == NULL)
		return;

	// If I don't have a bone, you are fine where you are.
	Coord3D zero;
	zero.zero();
	if (m_enterPosition == zero)
	{
		*position = *docker->getPosition();
		return;
	}

	// take local space position and convert to world space
	getObject()->convertBonePosToWorldPos(&m_exitPosition, NULL, position, NULL);
}


void DockUpdate::onApproachReached(Object *docker)
{
	ObjectID target = docker->getID();
	for (UnsignedInt i = 0; i < m_approachPositionOwners.size(); ++i)
	{
		if (m_approachPositionOwners[i] == target)
		{
			m_approachPositionReached[i] = true;
			break;
		}
	}
}


void DockUpdate::rva00589AFF(Int index)
{
	Int last = (Int)m_approachPositionOwners.size() - 1;
	for (; index < last; ++index)
	{
		if (m_approachPositionOwners[index] == INVALID_ID)
			break;
		m_approachPositionOwners[index] = m_approachPositionOwners[index + 1];
		m_approachPositionReached[index] = m_approachPositionReached[index + 1];
	}
	m_approachPositionOwners[last] = INVALID_ID;
	m_approachPositionReached[last] = false;
}


Coord3D DockUpdate::computeApproachPosition(Int positionIndex, Object *forWhom)
{
	if (m_positionsLoaded == false)
		loadDockPositions();

	Coord3D bestPosition;
	Coord3D workingPosition;
	FindPositionOptions fpOptions;

	Object *us = getObject();
	us->convertBonePosToWorldPos(&m_approachPositions[positionIndex], 0, &workingPosition, 0);

	if (m_numberApproachPositionBones == 0)
	{
		Coord3D ourPosition = *us->getPosition();
		Coord3D theirPosition = *forWhom->getPosition();
		Vector3 offset(theirPosition.x - ourPosition.x, theirPosition.y - ourPosition.y, theirPosition.z - ourPosition.z);
		offset.Normalize();
		offset = offset * (us->getMajorRadius() / 2);

		workingPosition.x += offset.X;
		workingPosition.y += offset.Y;
		workingPosition.z += offset.Z;
	}

	fpOptions.minRadius = 0.0f;
	fpOptions.maxRadius = 100.0f;
	fpOptions.sourceToPathToDest = forWhom;
	if (forWhom->isUsingAirborneLocomotor())
		fpOptions.ignoreObject = getObject();

	Bool spotFound = ThePartitionManager->findPositionAround(&workingPosition, &fpOptions, &bestPosition);

	if (spotFound)
		return bestPosition;
	return workingPosition;
}


Bool DockUpdate::advanceApproachPosition(Object *docker, Coord3D *position, Int *index)
{
	// load dock positions if not loaded yet
	if (m_positionsLoaded == false)
		loadDockPositions();

	// sanity
	if (position == NULL)
		return false;
	if (*index <= 0)
		return false;
	if (m_approachPositionOwners[(*index) - 1] != INVALID_ID)
		return false;

	Int hisIndex = *index;
	Int previousIndex = hisIndex - 1;

	m_approachPositionOwners[previousIndex] = docker->getID();
	m_approachPositionReached[previousIndex] = false;

	m_approachPositionOwners[hisIndex] = INVALID_ID;
	m_approachPositionReached[hisIndex] = false;

	*position = computeApproachPosition(previousIndex, docker);
	*index = previousIndex;
	return true;
}


void DockUpdate::onEnterReached(Object *docker)
{
	Object *me = getObject();
	me->rva0028CFB2(
		(const int *)&ModelConditionFlags(ModelConditionFlags::kInit, 0x54),
		(const int *)Rva001E4912().rva001E4912(0, 0x52, 0x51));
	docker->rva0028CFB2(
		(const int *)&ModelConditionFlags(ModelConditionFlags::kInit, 0x54),
		(const int *)Rva001E4912().rva001E4912(0, 0x52, 0x51));
	m_dockerInside = true;
	ObjectID dockerID = docker->getID();
	for (UnsignedInt i = 0; i < m_approachPositionOwners.size(); ++i)
	{
		if (m_approachPositionOwners[i] == dockerID)
		{
			rva00589AFF(i);
			return;
		}
	}
}


void DockUpdate::cancelDock(Object *docker)
{
	ObjectID dockerID = docker->getID();
	for (UnsignedInt i = 0; i < m_approachPositionOwners.size(); ++i)
	{
		if (m_approachPositionOwners[i] == dockerID)
		{
			rva00589AFF(i);
			break;
		}
	}
	if (m_activeDocker == dockerID)
	{
		Object *dockingObject = TheGameLogic->findObjectByID(m_activeDocker);
		m_activeDocker = INVALID_ID;
		m_dockerInside = false;
		ModelConditionFlags clear;
		clear.set(0x54);
		clear.set(0x52);
		clear.set(0x53);
		clear.set(0x51);
		getObject()->clearModelConditionFlags(clear);
		if (dockingObject)
			dockingObject->clearModelConditionFlags(clear);
	}
}


UpdateSleepTime DockUpdate::update()
{
	ObjectID positionIndex = m_activeDocker;
	if (positionIndex == INVALID_ID && !m_dockCrippled)
	{
		for (UnsignedInt i = 0; i < m_approachPositionReached.size(); ++i)
		{
			if (m_approachPositionReached[i])
			{
				m_activeDocker = m_approachPositionOwners[i];
				return UPDATE_SLEEP_NONE;
			}
		}
	}
	else
	{
		const ThingTemplate *myTemplate = getObject()->getTemplate();
		if ((((const unsigned char *)myTemplate->m_kindof.m_bits)[0xA] & 0x40) != 0)
		{
			Object *docker = TheGameLogic->findObjectByID(positionIndex);
			if (docker != 0)
			{
				const ThingTemplate *theirTemplate = docker->getTemplate();
				if (theirTemplate->isKindOf(KINDOF_DOZER) && theirTemplate->isKindOf(KINDOF_HARVESTER))
				{
					ModelConditionMask test;
					((unsigned char *)&test)[10] |= 4;
					if (test.rva00263546((const ModelConditionMask *)&docker->m_modelConditions))
					{
						docker->clearModelConditionFlags(ModelConditionFlags(ModelConditionFlags::kInit, 0x3D));
					}
				}
			}
		}
	}
	return UPDATE_SLEEP_NONE;
}


template void _STL::vector<ObjectID>::_M_fill_insert(ObjectID *, size_t, const ObjectID &);


template void _STL::vector<ObjectID>::insert(ObjectID *, size_t, const ObjectID &);


DockUpdate::~DockUpdate()
{
}




void Rva0058A193::rva0058A193(UnsignedInt count)
{
	resize(count, INVALID_ID);
}


Bool DockUpdate::reserveApproachPosition(Object *docker, Coord3D *position, Int *index)
{
	// load dock positions if not loaded yet
	if (m_positionsLoaded == false)
		loadDockPositions();

	// sanity
	if (position == NULL)
		return false;

	ObjectID dockerID = docker->getID();

	for (UnsignedInt positionIndex = 0; positionIndex < m_approachPositionOwners.size(); ++positionIndex)
	{
		if (m_approachPositionOwners[positionIndex] == dockerID)
		{
			*position = computeApproachPosition(positionIndex, docker);
			*index = positionIndex;
			return true;
		}
		if (m_approachPositionOwners[positionIndex] == INVALID_ID)
		{
			m_approachPositionOwners[positionIndex] = dockerID;
			*position = computeApproachPosition(positionIndex, docker);
			*index = positionIndex;
			return true;
		}
	}

	// If I make it out of the loop, I am full, so dynamic approach buildings should make a new entry instead of saying no
	if (m_numberApproachPositions == DYNAMIC_APPROACH_VECTOR_FLAG)
	{
		{
			Coord3D zero;
			zero.zero();
			m_approachPositions.push_back(zero);
		}
		m_approachPositionOwners.push_back(INVALID_ID);
		m_approachPositionReached.push_back(false);

		loadDockPositions();	// refresh this new one

		Int newIndex = (Int)m_approachPositionOwners.size() - 1;	// The new last spot
		m_approachPositionOwners[newIndex] = dockerID;
		*position = computeApproachPosition(newIndex, docker);
		*index = newIndex;
		return true;
	}

	return false;
}


DockUpdate::DockUpdate(Thing *thing, const ModuleData *moduleData)
	: UpdateModule(thing, moduleData)
{
	m_dockOpen = true;
	m_positionsLoaded = false;
	m_numberApproachPositionBones = -1;
	m_activeDocker = INVALID_ID;
	m_dockerInside = false;
	m_dockCrippled = false;

	const DockUpdateModuleData *md = (const DockUpdateModuleData *)moduleData;
	m_exitPosition.zero();
	m_dockPosition.zero();
	m_enterPosition.zero();

	m_numberApproachPositions = md->m_numberApproachPositionsData;
	if (m_numberApproachPositions != DYNAMIC_APPROACH_VECTOR_FLAG)
	{
		m_approachPositions.resize(m_numberApproachPositions);
		m_approachPositionOwners.resize(m_numberApproachPositions, INVALID_ID);
		m_approachPositionReached.resize(m_numberApproachPositions, false);
	}
	else
	{
		m_approachPositions.resize(DEFAULT_APPROACH_VECTOR_SIZE, Coord3D());
		m_approachPositionOwners.resize(DEFAULT_APPROACH_VECTOR_SIZE, INVALID_ID);
		m_approachPositionReached.resize(DEFAULT_APPROACH_VECTOR_SIZE, false);
	}

	for (UnsignedInt vectorIndex = 0;
		vectorIndex < m_approachPositions.size(); ++vectorIndex)
	{
		m_approachPositions[vectorIndex].zero();
		m_approachPositionOwners[vectorIndex] = INVALID_ID;
		m_approachPositionReached[vectorIndex] = false;
	}
}


void DockUpdate::xfer(Xfer *xfer)
{
	UpdateModule::xfer(xfer);
	if (xfer->IsLightCRC())
		return;
	xfer->Version1();
	*xfer == *(Coord3DBase *)&m_enterPosition;
	*xfer == *(Coord3DBase *)&m_dockPosition;
	*xfer == *(Coord3DBase *)&m_exitPosition;
	*xfer == m_numberApproachPositions;
	*xfer == m_positionsLoaded;
	Int vectorSize = (Int)m_approachPositions.size();
	*xfer == vectorSize;
	m_approachPositions.resize(vectorSize);
	for (Int vectorIndex = 0; vectorIndex < vectorSize; ++vectorIndex)
		*xfer == *(Coord3DBase *)&m_approachPositions[vectorIndex];
	vectorSize = (Int)m_approachPositionOwners.size();
	*xfer == vectorSize;
	m_approachPositionOwners.resize(vectorSize, INVALID_ID);
	for (vectorIndex = 0; vectorIndex < vectorSize; ++vectorIndex)
		XferObjectID(xfer, &m_approachPositionOwners[vectorIndex]);
	vectorSize = (Int)m_approachPositionReached.size();
	*xfer == vectorSize;
	m_approachPositionReached.resize(vectorSize, FALSE);
	for (vectorIndex = 0; vectorIndex < vectorSize; ++vectorIndex)
	{
		Bool unpack = m_approachPositionReached[vectorIndex];
		*xfer == unpack;
	}
	XferObjectID(xfer, &m_activeDocker);
	*xfer == m_dockerInside;
	*xfer == m_dockCrippled;
	*xfer == m_dockOpen;
}



// 0x0058A171
const char *DockUpdate::GetSnapshotName() const { return "DockUpdate"; }
