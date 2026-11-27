// cl: /O1 /D_BFME_RETAIL_TREE_INSERT_LAYOUT /D_STLP_USE_MALLOC /D_STLP_USE_STATIC_LIB /D_STLP_BFME_COPY_TRIVIAL_EXTERN /D_STLP_NO_EXCEPTIONS
// Portions derived from Command & Conquer Generals and Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
// FUNCTION: BFME2 0x004A4C39 size=6 name=?name@Rva004A4C39Named@@QBEPBDXZ evidence=?:placeholder
// FUNCTION: BFME2 0x004A4C3F size=67 name=?rva004A4C3F@@YAXHHQAH@Z evidence=?:placeholder
// FUNCTION: BFME2 0x004A4C82 size=69 name=?getModuleNameKey@RubbleRiseUpdate@@UBE?AW4NameKeyType@@XZ evidence=N:ledger
// FUNCTION: BFME2 0x004A4CC7 size=141 name=??0RubbleRiseUpdate@@QAE@PAVThing@@PBVModuleData@@@Z evidence=N:ledger
// FUNCTION: BFME2 0x004A4D70 size=40 name=?rva004A4D70@RubbleRiseUpdate@@QAEXXZ evidence=?:placeholder
// FUNCTION: BFME2 0x004A4D98 size=38 name=?onDie@RubbleRiseUpdate@@UAEXPBVDamageInfo@@@Z evidence=N:ledger
// FUNCTION: BFME2 0x004A4DBE size=96 name=?doRubbleDoneStuff@RubbleRiseUpdate@@IAEXXZ evidence=N:ledger
// FUNCTION: BFME2 0x004A4E1E size=203 name=?doPhaseStuff@RubbleRiseUpdate@@IAEXW4RubbleRisePhaseType@@PBUCoord3D@@@Z evidence=N:ledger
// FUNCTION: BFME2 0x004A4EE9 size=137 name=?xfer@FireWeaponWhenDamagedBehavior@@MAEXPAVXfer@@@Z evidence=N:ledger
// FUNCTION: BFME2 0x004A4F72 size=1101 name=?update@RubbleRiseUpdate@@UAE?AW4UpdateSleepTime@@XZ evidence=N:ledger
// stlport46
// bfmealloc_alloconly
#include <vector>
#include "GameLogic/Module/RubbleRiseUpdate.h"
#include "GameLogic/Module/BoneFXUpdate.h"
#include "GameLogic/Module/BodyModule.h"
#include "GameLogic/Object.h"
#include "GameLogic/ObjectCreationList.h"
#include "GameLogic/ObjectCreationListStore.h"
#include "GameLogic/FXList.h"
#include "GameLogic/GameLogic.h"
#include "GameLogic/TerrainLogic.h"
#include "GameClient/Drawable.h"
#include "GameClient/FXListStore.h"
#include "Common/GlobalData.h"
#include "Common/INI.h"
#include "Common/NameKeyGenerator.h"
#include "WWMath/Vector3.h"
#include "Common/Xfer.h"
extern const FieldParse BfmeRubbleRiseUpdateFields[];
int Rva004CE52EGet();
float GetGameClientRandomValueReal(float,float,char*,int);
int GetGameLogicRandomValue(int,int,char*,int);
float GetGameLogicRandomValueReal(float,float,char*,int);
#define GameClientRandomValueReal(lo,hi) GetGameClientRandomValueReal((lo),(hi),__FILE__,__LINE__)
#define GameLogicRandomValue(lo,hi) GetGameLogicRandomValue((lo),(hi),__FILE__,__LINE__)
#define GameLogicRandomValueReal(lo,hi) GetGameLogicRandomValueReal((lo),(hi),__FILE__,__LINE__)
#define TheGlobalData ((const GlobalData *)TheWritableGlobalData)
static const char *TheRubbleRisePhaseNames[]={"INITIAL","DELAY","BURST","FINAL",0};
bool inList(int,int,const int *);
// GameLogic/Object/Update/RubbleRiseUpdate.cpp: retail translation unit (skeleton).
// cl: 





RubbleRiseUpdate::~RubbleRiseUpdate() {}

const char *RubbleRiseUpdate::GetSnapshotName() const { return "RubbleRiseUpdate"; }


static void rva004A4C3F(Int range, Int count, Int idxList[])
{
	for (Int i = 0; i < count; ++i)
	{
		Int idx;
		do
		{
#line 272 "C:\\projects\\bfme2patch103\\bfme2\\Code\\GameEngine\\Source\\GameLogic\\Object\\Update\\RubbleRiseUpdate.cpp"
			idx = GameLogicRandomValue(0, range-1);
		}
		while (inList(idx, i, idxList));
		idxList[i] = idx;
	}
}


NameKeyType RubbleRiseUpdate::getModuleNameKey() const
{
	static NameKeyType TheRubbleRiseUpdatePoolKey =
		TheNameKeyGenerator->nameToKey("RubbleRiseUpdate");
	return TheRubbleRiseUpdatePoolKey;
}


RubbleRiseUpdate::RubbleRiseUpdate(Thing *thing,const ModuleData *moduleData) : UpdateModule(thing,moduleData)
{
 float fzero=0.0f;
 float *lingerSlot=&m_rubbleHeight;
 Object **objSlot=&m_object;
 int zero=0;
 m_unused24=zero; m_riseState=(RubbleRiseStateType)zero; m_riseVelocity=fzero;
 m_nextBurstFrame=zero; m_currentHeight=fzero; *lingerSlot=fzero;
 m_risePosition.x=fzero;m_risePosition.y=fzero;m_risePosition.z=fzero;
 setWakeFrame(*objSlot,UPDATE_SLEEP_FOREVER);
}



void RubbleRiseUpdate::rva004A4D70()
{
	int zero = 0;
	float fzero = 0.0f;
	m_unused24 = zero;
	m_riseState = (RubbleRiseStateType)zero;
	m_riseVelocity = fzero;
	m_nextBurstFrame = zero;
	m_currentHeight = fzero;
	m_rubbleHeight = fzero;
	setWakeFrame(getObject(), UPDATE_SLEEP_NONE);
}


void RubbleRiseUpdate::onDie(const DamageInfo *damageInfo)
{
	Object *obj = getObject();
	const RubbleRiseUpdateModuleData *rubbleRiseUpdateModuleData = getRubbleRiseUpdateModuleData();
	if (!rubbleRiseUpdateModuleData->m_dieMuxData.isDieApplicable(obj, damageInfo))
		return;
	rva004A4D70();
}


void RubbleRiseUpdate::doRubbleDoneStuff() 
{
	static NameKeyType key_BoneFXUpdate = TheNameKeyGenerator->nameToKey("BoneFXUpdate");
	BoneFXUpdate *bfxu = (BoneFXUpdate *)getObject()->findUpdateModule(key_BoneFXUpdate);
	if (bfxu != NULL) 
	{
		bfxu->stopAllBoneFX();
	}
}


void RubbleRiseUpdate::doPhaseStuff(RubbleRisePhaseType rrphase, const Coord3D *target)
{
	const RubbleRiseUpdateModuleData *rubbleRiseUpdateModuleData = getRubbleRiseUpdateModuleData();
	Int i, idx, count, listSize;
	Int idxList[MAX_IDX];

	listSize = rubbleRiseUpdateModuleData->m_fxs[rrphase].size();
	if (listSize > 0)
	{
		count = rubbleRiseUpdateModuleData->m_fxCount[rrphase];
		rva004A4C3F(listSize, count, idxList);
		for (i = 0; i < count; ++i)
		{
			idx = idxList[i];
			const FXVec &v = rubbleRiseUpdateModuleData->m_fxs[rrphase];
			const FXList *fxl = v[idx];
			FXList::doFXPos(fxl, target, 0, 0.0f, 0);
		}
	}

	listSize = rubbleRiseUpdateModuleData->m_ocls[rrphase].size();
	if (listSize > 0)
	{
		count = rubbleRiseUpdateModuleData->m_oclCount[rrphase];
		rva004A4C3F(listSize, count, idxList);
		for (i = 0; i < count; ++i)
		{
			idx = idxList[i];
			const OCLVec &v = rubbleRiseUpdateModuleData->m_ocls[rrphase];
			const ObjectCreationList *ocl = v[idx];
			if (ocl != 0)
				((ObjectCreationList *)ocl)->create(getObject(), (void *)target, 0, 0);
		}
	}
}


void RubbleRiseUpdate::xfer(Xfer *xfer)
{
	UpdateModule::xfer(xfer);
	if (xfer->IsLightCRC())
		return;
	Xfer::Version version(1, 2);
	*xfer == version;
	*xfer == m_unused24;
	*xfer == m_nextBurstFrame;
	xfer->XferRawBytes(&m_riseState, sizeof(m_riseState));
	*xfer == m_riseVelocity;
	*xfer == m_currentHeight;
	*xfer == m_risePosition;
	if (version.m_minimum >= 2)
		*xfer == m_rubbleHeight;
}

UpdateSleepTime RubbleRiseUpdate::update()
{
	const RubbleRiseUpdateModuleData *rubbleRiseUpdateModuleData = getRubbleRiseUpdateModuleData();
	Object *obj = getObject();
	Drawable *drawable = obj->getDrawable();

	if (m_riseState == RISESTATE_WAITING && drawable && obj->getModelConditionFlags().testBool(MODELCONDITION_RUBBLE_RISE_START))
	{
		const Coord3D *pos = obj->getPosition();
		doPhaseStuff(RRPHASE_INITIAL, pos);
		m_riseState = RISESTATE_RISING;

		Vector3 shudder;
#line 157
		shudder.Set(GameClientRandomValueReal(-(rubbleRiseUpdateModuleData->m_maxShudder), rubbleRiseUpdateModuleData->m_maxShudder), GameClientRandomValueReal(-(rubbleRiseUpdateModuleData->m_maxShudder), rubbleRiseUpdateModuleData->m_maxShudder), 0);

		const Matrix3D *instMatrix = obj->getDrawable()->getInstanceMatrix();
		Matrix3D newInstMatrix;
		newInstMatrix = *instMatrix;
		newInstMatrix.Set_Translation(shudder);
		drawable->setInstanceMatrix(&newInstMatrix, true);

		m_riseState = RISESTATE_RISING;
		doPhaseStuff(RRPHASE_BURST, pos);

		UnsignedInt now = TheGameLogic->getFrame();
#line 171
		m_nextBurstFrame = now + GameLogicRandomValue(rubbleRiseUpdateModuleData->m_minBurstDelay, rubbleRiseUpdateModuleData->m_maxBurstDelay);
		m_riseVelocity = -TheGlobalData->m_gravity * (1.0 - rubbleRiseUpdateModuleData->m_rubbleRiseDamping) * 20.0;
		m_rubbleHeight = TheTerrainLogic->getLayerHeight(obj->getPosition()->x, obj->getPosition()->y, (PathfindLayerEnum)obj->rva0028B511(), 0, true);
		m_currentHeight = 0;
		m_risePosition = *pos;
	}

	if (m_riseState == RISESTATE_RISING && drawable)
	{
		UnsignedInt now = TheGameLogic->getFrame();

#line 196
		Vector3 shudder(GameLogicRandomValueReal(-(rubbleRiseUpdateModuleData->m_maxShudder), rubbleRiseUpdateModuleData->m_maxShudder), GameLogicRandomValueReal(-(rubbleRiseUpdateModuleData->m_maxShudder), rubbleRiseUpdateModuleData->m_maxShudder), (m_currentHeight += m_riseVelocity));

		Coord3D newPos;
		const Coord3D *rise = &m_risePosition;
		newPos.x = shudder.X + rise->x;
		newPos.y = shudder.Y + rise->y;
		newPos.z = shudder.Z + rise->z;
		obj->setPosition(&newPos);

		if (now >= m_nextBurstFrame)
		{
#line 205
			if (GameLogicRandomValue(1, rubbleRiseUpdateModuleData->m_bigBurstFrequency) == 1)
			{
				doPhaseStuff(RRPHASE_BURST, obj->getPosition());
			}
			else
			{
				doPhaseStuff(RRPHASE_DELAY, obj->getPosition());
			}
#line 214
			m_nextBurstFrame += GameLogicRandomValue(rubbleRiseUpdateModuleData->m_minBurstDelay, rubbleRiseUpdateModuleData->m_maxBurstDelay);
		}

		if (m_currentHeight >= m_rubbleHeight - m_risePosition.z)
		{
			Coord3D finalPos;
			finalPos.x = m_risePosition.x;
			finalPos.y = m_risePosition.y;
			finalPos.z = m_rubbleHeight;
			obj->setPosition(&finalPos);
			m_riseState = RISESTATE_DONE;
			doPhaseStuff(RRPHASE_FINAL, obj->getPosition());

			doRubbleDoneStuff();

			obj->clearModelConditionState(MODELCONDITION_RUBBLE_RISE_START);
			obj->setModelConditionState(MODELCONDITION_RUBBLE_RISE_DONE);
			obj->setOrientation(obj->getOrientation());

			BodyModuleInterface *body = obj->getBodyModule();
			body->updateBodyParticleSystems();

			Vector3 shudder;
			shudder.Set(0, 0, 0);
			const Matrix3D *instMatrix = obj->getDrawable()->getInstanceMatrix();
			Matrix3D newInstMatrix;
			newInstMatrix = *instMatrix;
			newInstMatrix.Set_Translation(shudder);
			obj->getDrawable()->setInstanceMatrix(&newInstMatrix, true);

			return UPDATE_SLEEP_FOREVER;
		}
	}

	return UPDATE_SLEEP_NONE;
}


void RubbleRiseUpdateModuleData::parseFXList(INI *ini, void *instance, void * /*store*/, const void * /*userData*/)
{
	RubbleRiseUpdateModuleData *self = (RubbleRiseUpdateModuleData *)instance;
	int phase = ini->scanIndexList(ini->getNextToken(NULL), TheRubbleRisePhaseNames);
	for (const char *token = ini->getNextToken(); token != NULL; token = ini->getNextTokenOrNull())
	{
		const FXList *fxl = TheFXListStore->findFXList(token);	// could be null! this is OK!
		self->m_fxs[phase].push_back(fxl);
	}
}


void RubbleRiseUpdateModuleData::parseOCL(INI *ini, void *instance, void * /*store*/, const void * /*userData*/)
{
	RubbleRiseUpdateModuleData *self = (RubbleRiseUpdateModuleData *)instance;
	int phase = ini->scanIndexList(ini->getNextToken(NULL), TheRubbleRisePhaseNames);
	for (const char *token = ini->getNextToken(); token != NULL; token = ini->getNextTokenOrNull())
	{
		const ObjectCreationList *ocl = TheObjectCreationListStore->findObjectCreationList(token);	// could be null! this is OK!
		self->m_ocls[phase].push_back(ocl);
	}
}


// RVA 0x852960
const FieldParse BfmeRubbleRiseUpdateFields[] =
{
	{ "MinRubbleRiseDelay", INI::parseDurationUnsignedInt, 0, offsetof(RubbleRiseUpdateModuleData, m_minRubbleRiseDelay) },
	{ "MaxRubbleRiseDelay", INI::parseDurationUnsignedInt, 0, offsetof(RubbleRiseUpdateModuleData, m_maxRubbleRiseDelay) },
	{ "MinBurstDelay", INI::parseDurationUnsignedInt, 0, offsetof(RubbleRiseUpdateModuleData, m_minBurstDelay) },
	{ "MaxBurstDelay", INI::parseDurationUnsignedInt, 0, offsetof(RubbleRiseUpdateModuleData, m_maxBurstDelay) },
	{ "RubbleRiseDamping", INI::parseReal, 0, offsetof(RubbleRiseUpdateModuleData, m_rubbleRiseDamping) },
	{ "RubbleHeight", INI::parseReal, 0, offsetof(RubbleRiseUpdateModuleData, m_rubbleHeight) },
	{ "MaxShudder", INI::parseReal, 0, offsetof(RubbleRiseUpdateModuleData, m_maxShudder) },
	{ "BigBurstFrequency", INI::parseInt, 0, offsetof(RubbleRiseUpdateModuleData, m_bigBurstFrequency) },
	{ "OCL", RubbleRiseUpdateModuleData::parseOCL, 0, 0 },
	{ "FXList", RubbleRiseUpdateModuleData::parseFXList, 0, 0 },
	{ 0, 0, 0, 0 }
};

void RubbleRiseUpdateModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	parse.add(BfmeRubbleRiseUpdateFields, 0);
	parse.add(reinterpret_cast<const FieldParse *>(Rva004CE52EGet()), 8);
}

void RubbleRiseUpdate::loadPostProcess()
{
	UpdateModule::loadPostProcess();
}
