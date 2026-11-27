// Portions derived from Command & Conquer Generals and Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
// Object boundaries may be unproven.
// FUNCTION: BFME2 0x00390119 size=176 name=??0PhysicsBehaviorModuleData@@QAE@XZ evidence=N:ledger
// FUNCTION: BFME2 0x003901C9 size=17 name=?buildFieldParse@PhysicsBehaviorModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger
// FUNCTION: BFME2 0x003901DA size=836 name=?calcFlightPath@PhysicsBehavior@@QAE_N_NM@Z evidence=N:ledger
// FUNCTION: BFME2 0x0039051E size=21 name=?rva0039051E@PhysicsBehavior@@QBE_NXZ evidence=?:placeholder
// FUNCTION: BFME2 0x00390533 size=36 name=?rva00390533@Rva00390533@@QAE_NXZ evidence=?:placeholder
// FUNCTION: BFME2 0x00390557 size=170 name=?rva00390557@PhysicsBehavior@@QAEXPBUCoord3D@@MMHH@Z evidence=?:placeholder
// FUNCTION: BFME2 0x00390601 size=40 name=?rva00390601@PhysicsBehavior@@QAE?AW4UpdateSleepTime@@XZ evidence=?:placeholder
// FUNCTION: BFME2 0x00390629 size=150 name=?rva00390629@PhysicsBehavior@@QAEX_N@Z evidence=?:placeholder
// FUNCTION: BFME2 0x003906BF size=106 name=?rva003906BF@PhysicsBehavior@@QAEXXZ evidence=?:placeholder
// FUNCTION: BFME2 0x003907A6 size=182 name=??0PhysicsBehavior@@QAE@PAVThing@@PBVModuleData@@@Z evidence=N:ledger
// FUNCTION: BFME2 0x0039085C size=69 name=?rva00039085C@PhysicsBehavior@@SA?AW4NameKeyType@@XZ evidence=?:placeholder
// FUNCTION: BFME2 0x003908A1 size=6 name=?name@PhysicsBehavior@@QBEPBDXZ evidence=N:ledger
// FUNCTION: BFME2 0x003908A7 size=27 name=?rva003908A7@@YGPAXPAX@Z evidence=?:placeholder
// FUNCTION: BFME2 0x003908C2 size=79 name=??1PhysicsBehavior@@UAE@XZ evidence=N:ledger
// FUNCTION: BFME2 0x00390911 size=205 name=?Rva00390911XferCoordVector@@YAPAVXfer@@PAV1@PAUAICommandCoordVector@@@Z evidence=?:placeholder
// FUNCTION: BFME2 0x003909FA size=720 name=?consume@Rva003909FAObj@@QAEXPAXHH@Z evidence=?:placeholder
// FUNCTION: BFME2 0x00390CCA size=134 name=?rva00390CCA@PhysicsBehavior@@QAEX_N@Z evidence=?:placeholder
// FUNCTION: BFME2 0x00390D50 size=230 name=?xfer@PhysicsBehavior@@MAEXPAVXfer@@@Z evidence=N:ledger
// FUNCTION: BFME2 0x00390E36 size=43 name=?rva00390E36@PhysicsBehavior@@QAEXHH@Z evidence=?:placeholder
// FUNCTION: BFME2 0x00390E61 size=334 name=?rva00390E61@PhysicsBehavior@@QAEX_N@Z evidence=?:placeholder
// FUNCTION: BFME2 0x00390FAF size=412 name=?rva00390FAF@PhysicsBehavior@@QAEXXZ evidence=?:placeholder
// FUNCTION: BFME2 0x0039114B size=665 name=?update@PhysicsBehavior@@UAE?AW4UpdateSleepTime@@XZ evidence=N:ledger
// stlport46
#include <vector>
class Coord3D;
namespace _STL { template<> Coord3D *allocator<Coord3D>::allocate(size_type,const void *); }
// GameLogic/Object/Update/PhysicsUpdate.cpp: retail translation unit (skeleton).
// cl: /GX /EHsc- /D_STLP_USE_MALLOC /D_STLP_USE_STATIC_LIB /D_CRTIMP=
#include "GameLogic/Module/AIUpdate.h"
extern "C" double __cdecl fabs(double);
// Retail calls the CRT fabs here rather than expanding the intrinsic (PhysicsBehavior::consume 0x003909FA).
#include "Common/Vector.h"
#include "Common/XferException.h"
#include "Common/GlobalData.h"
#include "GameLogic/AI.h"
#include "GameLogic/AIPathfind.h"
#include "WWMath/Matrix3D.h"
#include "GameLogic/FXList.h"
#include "GameClient/Drawable.h"
#include "Common/RandomValue.h"
#include "GameLogic/TerrainLogic.h"
#include "WWMath/Vector3.h"
#include "WWMath/wwmath.h"
#include "GameLogic/GameLogic.h"

// The element vector's destructor releases +0x20 through the game allocator
// directly (retail 0x003908C2), not through the CRT import: take <stdlib.h>
// without dllimport.
#define _CRTIMP
#include <stdlib.h>
#include <math.h>
namespace PhysicsCRT { extern "C" __declspec(dllimport) double __cdecl ceil(double); }
#undef _CRTIMP

#include <string.h>

#include "Common/GameType.h"
#include "Common/INI.h"
#include "Common/NameKeyGenerator.h"
#include "Common/ThingTemplate.h"
#include "GameLogic/Module/PhysicsBehavior.h"
#include "GameLogic/Module/PhysicsBehaviorModuleData.h"
#include "GameLogic/Object.h"
#include "GameLogic/Rva00390533.h"
#include "GameLogic/WeaponStore.h"
#include "Common/Xfer.h"
#include "GameLogic/Module/PhysicsFlightPoint.h"
#include "WWMath/BezierSegment.h"
PhysicsFlightPoint::PhysicsFlightPoint() {}
PhysicsFlightPoint::~PhysicsFlightPoint() {}

// BFME 2's logic frame rate (5), the int global at 0x00DBA4E4.
extern Int BfmeLogicFramesPerSecond;

// Retail 0x00C19DE8, including the null terminator.
static const FieldParse PhysicsBehaviorModuleDataFieldParse[] =
{
	{ "TumbleRandomly", &INI::parseBool, 0, 0x40 },
	{ "AllowBouncing", &INI::parseBool, 0, 0x58 },
	{ "KillWhenRestingOnGround", &INI::parseBool, 0, 0x59 },
	{ "GravityMult", &INI::parseReal, 0, 0x4C },
	{ "OrientToFlightPath", &INI::parseBool, 0, 0x41 },
	{ "ShockStunnedTimeLow", &INI::parseDurationUnsignedInt, 0, 0x18 },
	{ "ShockStunnedTimeHigh", &INI::parseDurationUnsignedInt, 0, 0x1C },
	{ "ShockStandingTime", &INI::parseDurationUnsignedInt, 0, 0x20 },
	{ "FirstHeight", &INI::parseReal, 0, 0x08 },
	{ "SecondHeight", &INI::parseReal, 0, 0x0C },
	{ "FirstPercentIndent", &INI::parsePercentToReal, 0, 0x10 },
	{ "SecondPercentIndent", &INI::parsePercentToReal, 0, 0x14 },
	{ "BounceCount", &INI::parseInt, 0, 0x24 },
	{ "BounceFirstHeight", &INI::parseReal, 0, 0x2C },
	{ "BounceSecondHeight", &INI::parseReal, 0, 0x30 },
	{ "BounceFirstPercentIndent", &INI::parsePercentToReal, 0, 0x34 },
	{ "BounceSecondPercentIndent", &INI::parsePercentToReal, 0, 0x38 },
	{ "GroundHitFX", &INI::parseFXList, 0, 0x50 },
	{ "GroundBounceFX", &INI::parseFXList, 0, 0x54 },
	{ "IgnoreTerrainHeight", &INI::parseBool, 0, 0x42 },
	{ "FirstPercentHeight", &INI::parsePercentToReal, 0, 0x44 },
	{ "SecondPercentHeight", &INI::parsePercentToReal, 0, 0x48 },
	{ "CurveFlattenMinDist", &INI::parseReal, 0, 0x3C },
	{ 0, 0, 0, 0x00 },
};

PhysicsBehaviorModuleData::PhysicsBehaviorModuleData() :
	m_firstHeight(1.3f),
	m_secondHeight(1.3f),
	m_firstPercentIndent(0.33f),
	m_secondPercentIndent(0.66f),
	m_bounceCount(2),
	m_unk0x28(5.0f),
	m_bounceFirstHeight(1.3f),
	m_bounceSecondHeight(1.3f),
	m_bounceFirstPercentIndent(0.33f),
	m_bounceSecondPercentIndent(0.66f),
	m_curveFlattenMinDist(0.0f),
	m_tumbleRandomly(false),
	m_orientToFlightPath(false),
	m_ignoreTerrainHeight(false),
	m_firstPercentHeight(0.33f),
	m_secondPercentHeight(0.66f),
	m_gravityMult(1.0f),
	m_groundHitFX(0),
	m_groundBounceFX(0),
	m_allowBouncing(false),
	m_killWhenRestingOnGround(false)
{
	m_shockStunnedTimeLow = BfmeLogicFramesPerSecond;
	m_shockStunnedTimeHigh = 2 * BfmeLogicFramesPerSecond;
	m_shockStandingTime = BfmeLogicFramesPerSecond;
}

void PhysicsBehaviorModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	parse.add(PhysicsBehaviorModuleDataFieldParse, 0);
}


template<class T> inline const T &physicsRefMax(const T&lhs,const T&rhs){return lhs>rhs?lhs:rhs;}
bool PhysicsBehavior::calcFlightPath(bool recalc,float maxHeight)
{
 const PhysicsBehaviorModuleData *physicsFlightData=(const PhysicsBehaviorModuleData *)m_moduleData;
 if(maxHeight<0.5f)maxHeight=0.5f;
 float firstPct,secondPct;
 if(!m_bfme54)firstPct=physicsFlightData->m_firstPercentIndent;else firstPct=physicsFlightData->m_bounceFirstPercentIndent;
 if(!m_bfme54)secondPct=physicsFlightData->m_secondPercentIndent;else secondPct=physicsFlightData->m_bounceSecondPercentIndent;
 PhysicsFlightPoint cp[4];
 static_cast<Coord3D&>(cp[0])=m_bfme2C;static_cast<Coord3D&>(cp[3])=m_bfme38;
 float z=m_bfme2C.z-m_bfme38.z;if(z<0.0f)z=0.0f;
 float scale=(maxHeight-z)/maxHeight;if(scale<0.0f)scale=0.0f;
 m_bfme48=maxHeight+z;
 cp[1].x=firstPct*(cp[3].x-cp[0].x)+cp[0].x;
 cp[1].y=firstPct*(cp[3].y-cp[0].y)+cp[0].y;
 cp[2].x=(cp[3].x-cp[0].x)*secondPct+cp[0].x;
 cp[2].y=(cp[3].y-cp[0].y)*secondPct+cp[0].y;
 if(physicsFlightData->m_ignoreTerrainHeight){
  cp[1].z=(cp[3].z-cp[0].z)*physicsFlightData->m_firstPercentHeight+cp[0].z;
  cp[2].z=(cp[3].z-cp[0].z)*physicsFlightData->m_secondPercentHeight+cp[0].z;
 }else{
  float highest=TheTerrainLogic->getHighestHeight(&cp[0],&cp[3]);
  float firstHeight,secondHeight;
  if(!m_bfme54)firstHeight=physicsFlightData->m_firstHeight;else firstHeight=physicsFlightData->m_bounceFirstHeight;
  if(!m_bfme54)secondHeight=physicsFlightData->m_secondHeight;else secondHeight=physicsFlightData->m_bounceSecondHeight;
  firstHeight*=maxHeight;secondHeight*=scale*maxHeight;
  if(physicsFlightData->m_curveFlattenMinDist>0.0f){
   Vector3 delta;delta.X=cp[3].x-cp[0].x;delta.Y=cp[3].y-cp[0].y;delta.Z=cp[3].z-cp[0].z;
   float distance=delta.Length();
   float factor=distance/physicsFlightData->m_curveFlattenMinDist;if(factor>1.0f)factor=1.0f;
   cp[1].z=firstPct*(cp[3].z-cp[0].z)+cp[0].z;
   float z2=(cp[3].z-cp[0].z)*secondPct+cp[0].z;
   if(cp[1].z<highest)cp[1].z=highest;
   cp[1].z=factor*firstHeight+cp[1].z;
   if(z2<highest)z2=highest;
   cp[2].z=factor*secondHeight+z2;
  }else{
   highest=physicsRefMax(highest,cp[0].z);highest=physicsRefMax(highest,cp[3].z);
   cp[1].z=highest+firstHeight;cp[2].z=highest+secondHeight;
  }
 }
 Rva0055A246 segment(*reinterpret_cast<const Rva0055A246*>(cp));
 if(recalc){float step=m_bfme44;float rounded=(float)PhysicsCRT::ceil(segment.rva0055A627(1.0f)/step);m_bfme4C=(int)(rounded+1.0f);}
 if(m_bfme4C<3)m_bfme4C=3;
 segment.rva0055A7DB(m_bfme4C,&m_elements);
 return true;
}

Bool PhysicsBehavior::rva0039051E() const
{
	UnsignedInt count = ((const char *)m_elements.end() - (const char *)m_elements.begin()) / 12;
	return count > 0;
}

// Retail reads the containing condition word and shifts it (word 3 bit 31,
// then word 2 bit 8) rather than masking it, so the shifted spelling is used.
Bool Rva00390533::rva00390533()
{
	if (!m_object->m_modelConditions.testShifted(3 * 32 + 31))
	{
		if (!m_object->m_modelConditions.testShifted(2 * 32 + 8))
			return false;
	}
	return true;
}

void PhysicsBehavior::rva00390557(const Coord3D *where, Real strength, Real value44, Int value60, Int value64)
{
	Object *object = getObject();
	Object *container = object->getContainedBy();
	if (container && !container->getTemplate()->isKindOf0x115())
	{
		setWakeFrame(object, UPDATE_SLEEP_FOREVER);
		return;
	}
	m_bfme38 = *where;
	m_bfme2C = *object->getPosition();
	m_bfme44 = value44;
	if (calcFlightPath(true, strength))
	{
		m_bfme50 = 0;
		if (value60)
			m_bfme60 = (const Object *)value60;
		if (value64)
			m_bfme64 = (const WeaponTemplate *)value64;
		static_cast<UpdateModuleInterface *>(this)->update();
		getObject()->rva0023D3AF((void *)TheGameLogic->getFrame());
		static_cast<UpdateModuleInterface *>(this)->update();
		setWakeFrame(getObject(), UPDATE_SLEEP_NONE);
	}
}


UpdateSleepTime PhysicsBehavior::rva00390601()
{
	if (rva0039051E())
		return UPDATE_SLEEP_NONE;
	if (m_bfme5C && m_bfme58 > 0)
		return UPDATE_SLEEP_NONE;
	return UPDATE_SLEEP_FOREVER;
}

// Both this body and rva003906BF refuse when the object's template, or the
// template of the object it is contained by, resists shockwaves at 100 or more.
void PhysicsBehavior::rva00390629(Bool enable)
{
	Object *obj = m_object;
	const ThingTemplate *thingTemplate = obj->getTemplate();
	const Object *container = obj->getContainedBy();
	if (thingTemplate->m_shockwaveResistance >= 100.0f || (container && container->getTemplate()->m_shockwaveResistance >= 100.0f))
		enable = false;
	m_bfme5C = enable;
	if (enable)
		return;
	obj->clearModelConditionState(5 * 32 + 3);
	obj->clearModelConditionState(3 * 32 + 31);
	obj->clearModelConditionState(4 * 32 + 0);
	m_bfme58 = 0;
}

void PhysicsBehavior::rva003906BF()
{
	Object *obj = m_object;
	const ThingTemplate *thingTemplate = obj->getTemplate();
	const Object *container = obj->getContainedBy();
	if (thingTemplate->m_shockwaveResistance >= 100.0f || (container && container->getTemplate()->m_shockwaveResistance >= 100.0f))
		return;
	obj->setModelConditionState(4 * 32 + 0);
	m_bfme5C = true;
	m_bfme58 = ((const PhysicsBehaviorModuleData *)m_moduleData)->m_shockStandingTime;
	setWakeFrame(obj, UPDATE_SLEEP_NONE);
}

// Builds UpdateModule, the element vector, then zeroes the run state in retail
// store order, takes the module data's AllowBouncing byte into +0x5D and parks
// the module forever.
PhysicsBehavior::PhysicsBehavior(Thing *thing, const ModuleData *moduleData)
	: UpdateModule(thing, moduleData)
{
	m_bfme44 = 0.0f;
	m_bfme48 = 0.0f;
	m_bfme4C = 0;
	m_bfme50 = 0;
	m_bfme54 = 0;
	m_bfme58 = 0;
	m_bfme5C = false;
	m_bfme5E = false;
	m_bfme5F = false;
	m_bfme60 = 0;
	m_bfme64 = 0;
	m_bfme2C.zero();
	m_bfme38.zero();
	m_bfme5D = ((const PhysicsBehaviorModuleData *)m_moduleData)->m_allowBouncing;
	setWakeFrame(m_object, UPDATE_SLEEP_FOREVER);
}

NameKeyType PhysicsBehavior::getModuleNameKey() const
{
	static NameKeyType ThePhysicsBehaviorNameKey = TheNameKeyGenerator->nameToKey("PhysicsBehavior");
	return ThePhysicsBehaviorNameKey;
}

DisabledMaskType PhysicsBehavior::getDisabledTypesToProcess() const
{
	return DisabledMaskType(DisabledMaskType::kInit, 4);
}


// Retail releases the element buffer through the game's own deallocator at
// 0x00030830, an out-of-line function the compiler cannot see through, so the
// destructor keeps an unwind state across the free.
void __cdecl Rva00030830GameFree(void *);

PhysicsBehavior::~PhysicsBehavior()
{
}

// Transfers a vector of positions (a flight path or a command's waypoints).
Xfer *Rva00390911XferCoordVector(Xfer *xfer, _STL::vector<Coord3D> *vec)
{
	Xfer::Version version(1, 1);
	*xfer == version;
	UnsignedInt count = vec->size();
	(*xfer == "std::vector") == count;
	if (xfer->IsStoring())
	{
		Coord3D *end = vec->end();
		Coord3D *cur = vec->begin();
		while (cur != end)
		{
			*xfer == *cur;
			++cur;
		}
	}
	else
	{
		if (!vec->empty())
			throw XferException(4, "Vector must be empty on load");
		vec->reserve(count);
		while (count != 0)
		{
			--count;
			Coord3D value;
			vec->push_back(value);
			*xfer == vec->back();
		}
	}
	return xfer;
}


void PhysicsBehavior::consume(void *forceArg, Int source, Int weapon)
{
	const Coord3D *force = (const Coord3D *)forceArg;
	const PhysicsBehaviorModuleData *data = (const PhysicsBehaviorModuleData *)m_moduleData;
	Object *obj = getObject();
	if (obj->getContainedBy() && !obj->getContainedBy()->getTemplate()->isKindOf0x115())
	{
		setWakeFrame(obj, UPDATE_SLEEP_FOREVER);
		return;
	}
	if (obj->testStatus(OBJECT_STATUS_46))
		obj->rva00293105();
	AIUpdateInterface *ai = obj->getAI();
	if (ai && !ai->isBusy())
	{
		ai->rva0026331C();
		ai->getAICommandInterface()->aiIdle(CMD_FROM_AI);
	}
	setWakeFrame(obj, UPDATE_SLEEP_NONE);

	Coord3D pos(*obj->getPosition());
	Real height = pos.z - TheTerrainLogic->getLayerHeight(pos.x,pos.y,(PathfindLayerEnum)obj->rva0028B511(),NULL,true);
	if (height < 0.0f)
		height = 0.0f;
	int maxFrames = 3 * BfmeLogicFramesPerSecond;
	Real sim = height;
	int i;
	for (i = 0; i < maxFrames && sim > 0.0f; ++i)
		sim += data->m_gravityMult * TheWritableGlobalData->m_gravity * i;
	Real vz = force->z;
	if (force->length() * 0.25f > vz && i < 2)
		vz = force->length() * 0.25f;
	unsigned duration = (unsigned)fabs(2.0f * vz / (data->m_gravityMult * TheWritableGlobalData->m_gravity));
	if (duration < 1)
		duration = 1;
	duration += i;
	Real peak = (vz * 0.5f) * ((Real)duration * 0.5f);
	if (peak < 0.0f)
		peak = 0.0f;
	Coord3D delta(*force);
	delta.z = 0.0f;
	delta.scale((Real)duration);
	Coord3D dest;
	dest = *obj->getPosition();
	dest.add(&delta);
	TheAI->m_pathfinder->AdjustFlingDestination(obj, obj->getPosition(), &dest);
	dest.z += 10.0f;
	dest.z = TheTerrainLogic->getLayerHeight(dest.x, dest.y, TheTerrainLogic->getLayerForDestination(NULL, &dest), NULL, true);
	Real speed = (delta.length() + 2.0f * peak + height) / (Real)duration;
	Real minSpeed = -TheWritableGlobalData->m_gravity;
	if (speed < minSpeed)
		speed = minSpeed;
	rva00390557(&dest, peak, speed, source, weapon);
}


// Drops the element list and the pending force, then either settles the object
// on the ground (condition 3*32+26 and a falling kill) or parks the module,
// and finally hands any pending weapon template back to the store.
void PhysicsBehavior::rva00390CCA(Bool arg)
{
	Object *obj = m_object;
	m_elements.clear();
	m_bfme50 = 0;
	m_bfme54 = 0;
	if (m_bfme5F || ((const PhysicsBehaviorModuleData *)m_moduleData)->m_killWhenRestingOnGround != 0)
	{
		if (!obj->m_modelConditions.test(3 * 32 + 26))
		{
			obj->m_modelConditions.set(3 * 32 + 26);
			obj->rva0028AE6D();
		}
		obj->kill((DamageType)8, (DeathType)0);
	}
	else if (!arg)
	{
		setWakeFrame(obj, UPDATE_SLEEP_FOREVER);
	}
	if (m_bfme64 != 0)
	{
		TheWeaponStore->rva002CE964(m_bfme64, m_bfme60, obj);
		m_bfme64 = 0;
	}
	m_bfme60 = 0;
}

// Version 2 added the coordinate vector at +0x20; loading clears it first.
Xfer *Rva00390911XferCoordVector(Xfer *xfer, _STL::vector<Coord3D> *coords);

void PhysicsBehavior::xfer(Xfer *xfer)
{
	Xfer::Version version(1, 2);
	*xfer == version;
	UpdateModule::xfer(xfer);
	*xfer == m_bfme4C;
	*xfer == m_bfme44;
	*xfer == m_bfme48;
	*xfer == *(Coord3DBase *)&m_bfme2C;
	*xfer == *(Coord3DBase *)&m_bfme38;
	*xfer == m_bfme58;
	*xfer == m_bfme5C;
	*xfer == m_bfme5D;
	*xfer == m_bfme54;
	*xfer == m_bfme5E;
	*xfer == m_bfme5F;
	*xfer == m_bfme50;
	if (version.m_minimum >= 2)
	{
		if (xfer->IsLoading())
			m_elements.clear();
		Rva00390911XferCoordVector(xfer, &m_elements);
	}
}

void PhysicsBehavior::rva00390E36(Int source, Int weapon)
{
	Coord3D force;
	force.zero();
	consume(&force, source, weapon);
}

void PhysicsBehavior::rva00390E61(Bool skipWake)
{
	Object *obj = getObject();
	if (!skipWake)
		setWakeFrame(obj, UPDATE_SLEEP_NONE);
	++m_bfme54;
	int count = m_elements.size();
	if (count < 2)
	{
		rva00390CCA(skipWake);
		return;
	}
	Vector3 direction;
	direction.X = ((const Coord3D *)m_elements.begin())[count - 1].x - ((const Coord3D *)m_elements.begin())[count - 2].x;
	direction.Y = ((const Coord3D *)m_elements.begin())[count - 1].y - ((const Coord3D *)m_elements.begin())[count - 2].y;
	direction.Z = 0.0f;
	direction.Normalize();
	const Coord3D *pos = obj->getPosition();
	Real distance;
	{
		Coord3D delta(m_bfme38);
		delta.sub(&m_bfme2C);
		delta.z = 0.0f;
		distance = delta.length();
	}
	distance *= 0.5f;
	{
		Vector3 next;
		next.X = direction.X * distance + pos->x;
		next.Y = direction.Y * distance + pos->y;
		next.Z = TheTerrainLogic->getGroundHeight(next.X, next.Y, NULL);
		rva00390557((const Coord3D *)&next, m_bfme48 * 0.35f, m_bfme44 * 0.85f, 0, 0);
	}
	m_bfme5E = true;
}


void PhysicsBehavior::rva00390FAF()
{
	if ((UnsignedInt)((const Coord3D *)m_elements.end() - (const Coord3D *)m_elements.begin()) <= 0)
		return;
	const PhysicsBehaviorModuleData *data = (const PhysicsBehaviorModuleData *)m_moduleData;
	Object *obj = getObject();
	FXList::doFXPos((const FXList *)data->m_groundHitFX, obj->getPosition(), NULL, 0.0f, NULL);
	Bool changed = false;
	if (obj->getModelConditionFlags().testShifted(MODELCONDITION_72))
	{
		obj->setModelConditionState(MODELCONDITION_122);
		obj->clearModelConditionState(MODELCONDITION_72);
		obj->clearModelConditionState(MODELCONDITION_127);
		changed = true;
	}
	if (m_bfme5C && (m_bfme58 <= 0 || obj->isEffectivelyDead()))
	{
		obj->clearModelConditionState(MODELCONDITION_127);
		obj->clearModelConditionState(MODELCONDITION_72);
		if (obj->isEffectivelyDead())
		{
			obj->setModelConditionState(MODELCONDITION_62);
			obj->setModelConditionState(MODELCONDITION_122);
		}
		else
		{
			obj->setModelConditionState(MODELCONDITION_128);
			m_bfme58 = GetGameLogicRandomValue(data->m_shockStunnedTimeLow, data->m_shockStunnedTimeHigh,
				"C:\\projects\\bfme2patch103\\bfme2\\Code\\GameEngine\\Source\\GameLogic\\Object\\Update\\PhysicsUpdate.cpp", 752);
			changed = true;
		}
	}
	if (changed && obj->getDrawable())
		obj->getDrawable()->rva00274176(false);
	if ((m_bfme5D || data->m_allowBouncing) && m_bfme54 < data->m_bounceCount)
	{
		rva00390E61(false);
		return;
	}
	rva00390CCA(true);
}


UpdateSleepTime PhysicsBehavior::update()
{
	const PhysicsBehaviorModuleData *data = (const PhysicsBehaviorModuleData *)m_moduleData;
	Object *obj = m_object;
	m_bfme5E = false;
	Bool changed = false;
	if (m_bfme5C && m_bfme58 > 0 && !obj->isEffectivelyDead())
	{
		if (--m_bfme58 <= 0)
		{
			if (obj->m_modelConditions.testMasked(MODELCONDITION_128))
			{
				obj->clearModelConditionState(MODELCONDITION_128);
				obj->setModelConditionState(MODELCONDITION_163);
				m_bfme58 = data->m_shockStandingTime;
				changed = true;
			}
			else if (obj->getModelConditionFlags().testShifted(MODELCONDITION_163))
			{
				obj->clearModelConditionState(MODELCONDITION_163);
				rva00390629(false);
			}
			else
			{
				rva00390629(false);
			}
		}
	}
	if ((UnsignedInt)m_bfme50 >= m_elements.size())
	{
		rva00390FAF();
		return rva00390601();
	}
	if (changed && obj->getDrawable())
		obj->getDrawable()->rva00274176(false);
	const Coord3D *current = &((const Coord3D *)m_elements.begin())[m_bfme50];
	if (data->m_orientToFlightPath && !data->m_tumbleRandomly && m_bfme50 > 0)
	{
		const Coord3D *previous = &((const Coord3D *)m_elements.begin())[m_bfme50 - 1];
		Vector3 direction(current->x - previous->x, current->y - previous->y, current->z - previous->z);
		if (m_bfme54 > 0)
			direction.Z = TheWritableGlobalData->m_gravity * data->m_gravityMult * 100.0f;
		direction.Normalize();
		Vector3 position(current->x, current->y, current->z);
		Matrix3D transform;
		transform.buildTransformMatrix(position, direction);
		m_object->setTransformMatrix(&transform);
	}
	else
	{
		m_object->setPosition(current);
	}
	Coord3D next;
	const Coord3D *nextPoint;
	if ((UnsignedInt)m_bfme50 < m_elements.size() - 1)
	{
		nextPoint = &((const Coord3D *)m_elements.begin())[m_bfme50 + 1];
	}
	else
	{
		Vector3 a(current->x, current->y, current->z);
		Vector3 p(obj->getPosition()->x, obj->getPosition()->y, obj->getPosition()->z);
		a.X *= 2.0f;
		a.Y *= 2.0f;
		a.Z *= 2.0f;
		a.X -= p.X;
		a.Y -= p.Y;
		a.Z -= p.Z;
		next.x = a.X;
		next.y = a.Y;
		next.z = a.Z;
		nextPoint = &next;
	}
	obj->setNextPosition(nextPoint);
	TheAI->m_pathfinder->AdjustLayer(m_object);
	++m_bfme50;
	return rva00390601();
}

void PhysicsBehavior::loadPostProcess()
{
	UpdateModule::loadPostProcess();
}

// 0x003908A1
const char *PhysicsBehavior::GetSnapshotName() const { return "PhysicsBehavior"; }
