// Portions derived from Command & Conquer Generals and Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
// Object boundaries may be unproven.
// FUNCTION: BFME2 0x0048B03E size=24 name=?killGridDecals@DynamicShroudClearingRangeUpdate@@QAEXXZ evidence=N:ledger
// FUNCTION: BFME2 0x0048B056 size=17 name=?buildFieldParse@DynamicShroudClearingRangeUpdateModuleData@@SAXAAVMultiIniFieldParse@@@Z evidence=N:ledger
// FUNCTION: BFME2 0x0048B067 size=253 name=??0DynamicShroudClearingRangeUpdate@@QAE@PAVThing@@PBVModuleData@@@Z evidence=N:ledger
// FUNCTION: BFME2 0x0048B164 size=6 name=?Rva0048B164Get@@YAHXZ evidence=?:placeholder
// FUNCTION: BFME2 0x0048B16A size=69 name=?getModuleNameKey@DynamicShroudClearingRangeUpdate@@UBE?AW4NameKeyType@@XZ evidence=N:ledger
// FUNCTION: BFME2 0x0048B1AF size=95 name=??1DynamicShroudClearingRangeUpdate@@UAE@XZ evidence=N:ledger
// FUNCTION: BFME2 0x0048B20E size=72 name=?createGridDecals@DynamicShroudClearingRangeUpdate@@QAEXABVRadiusDecalTemplate@@MABUCoord3D@@@Z evidence=N:ledger
// FUNCTION: BFME2 0x0048B256 size=233 name=?rva0048B256@DynamicShroudClearingRangeUpdate@@QAEXXZ evidence=?:placeholder
// FUNCTION: BFME2 0x0048B33F size=454 name=?update@DynamicShroudClearingRangeUpdate@@UAE?AW4UpdateSleepTime@@XZ evidence=N:ledger
// FUNCTION: BFME2 0x0048B505 size=165 name=?xfer@DynamicShroudClearingRangeUpdate@@UAEXPAVXfer@@@Z evidence=N:ledger
// FUNCTION: BFME2 0x0048B5C6 size=82 name=??0DynamicShroudClearingRangeUpdateModuleData@@QAE@XZ evidence=N:ledger
// GameLogic/Object/Update/DynamicShroudClearingRangeUpdate.cpp: retail translation unit (skeleton).
// cl: 

#include "Common/Coord3D.h"
#include <math.h>
#include <stddef.h>
#include "Common/INI.h"
#include "Common/NameKeyGenerator.h"
#include "Common/Xfer.h"
#include "GameLogic/GameLogic.h"
#include "GameLogic/Module/DynamicShroudClearingRangeUpdate.h"
#include "GameLogic/Object.h"


void DynamicShroudClearingRangeUpdate::killGridDecals()
{
	for (int d = 0; d < 30; ++d)
	{
		m_gridDecal[d].clear();
	}
}


// The module data's own INI table. Retail keeps it as one data block at
// 0x00C4BE20; the row is only the `add` that registers it.
static const FieldParse s_dynamicShroudFields[] =
{
	{ "ChangeInterval", INI::parseDurationUnsignedInt, 0, offsetof(DynamicShroudClearingRangeUpdateModuleData, m_changeInterval) },
	{ "GrowInterval", INI::parseDurationUnsignedInt, 0, offsetof(DynamicShroudClearingRangeUpdateModuleData, m_growInterval) },
	{ "ShrinkDelay", INI::parseDurationUnsignedInt, 0, offsetof(DynamicShroudClearingRangeUpdateModuleData, m_shrinkDelay) },
	{ "ShrinkTime", INI::parseDurationUnsignedInt, 0, offsetof(DynamicShroudClearingRangeUpdateModuleData, m_shrinkTime) },
	{ "GrowDelay", INI::parseDurationUnsignedInt, 0, offsetof(DynamicShroudClearingRangeUpdateModuleData, m_growDelay) },
	{ "GrowTime", INI::parseDurationUnsignedInt, 0, offsetof(DynamicShroudClearingRangeUpdateModuleData, m_growTime) },
	{ "FinalVision", INI::parseReal, 0, offsetof(DynamicShroudClearingRangeUpdateModuleData, m_finalVision) },
	{ "GridDecalTemplate", RadiusDecalTemplate::parseRadiusDecalTemplate, 0, offsetof(DynamicShroudClearingRangeUpdateModuleData, m_gridDecalTemplate) },
	{ 0, 0, 0, 0 }
};

void DynamicShroudClearingRangeUpdateModuleData::buildFieldParse(MultiIniFieldParse &parse)
{
	parse.add(s_dynamicShroudFields, 0);
}


// The frame budget is clamped through a reference-returning max, which is what
// retail's `lea ecx,[ebp+0xc]; lea eax,[esi+0x24]; cmp; jl` selects between.
static const int &bfmeMax(const int &left, const int &right)
{
	return right < left ? left : right;
}

static const float &bfmeMax(const float &left, const float &right)
{
	return right < left ? left : right;
}

DynamicShroudClearingRangeUpdate::DynamicShroudClearingRangeUpdate(
	Thing *thing, const ModuleData *moduleData)
	: UpdateModule(thing, moduleData)
{
	const DynamicShroudClearingRangeUpdateModuleData *md = (const DynamicShroudClearingRangeUpdateModuleData *)moduleData;
	m_changeIntervalCountdown = 0;
	m_visionChangePerInterval = 0.0f;
	m_stateCountDown = md->m_shrinkDelay + md->m_shrinkTime;
	m_totalFrames = bfmeMax(1, m_stateCountDown);
	m_shrinkStartDeadline = m_stateCountDown - md->m_shrinkDelay;
	m_growStartDeadline = m_stateCountDown - md->m_growDelay;
	m_sustainDeadline = m_growStartDeadline - md->m_growTime;
	m_doneForeverFrame = TheGameLogic->getFrame() + m_stateCountDown;
	m_decalsCreated = false;
	m_nativeClearingRange = 200.0f;
	m_currentClearingRange = 0.0f;
	m_state = DSCRU_NOT_STARTED_YET;

	Object *object = m_object;
	if (!object)
		return;
	Player *controller = object->getControllingPlayer();
	if (!controller)
		return;
	m_nativeClearingRange = object->getShroudClearingRange();

	RadiusDecal *decal = m_gridDecal;
	int decalsLeft = 30;
	do {
		decal->clear();
		++decal;
	} while (--decalsLeft != 0);
}


// The module's INI name, read as a plain address: retail returns 0x00BF5614, the
// same string getModuleNameKey interns.
int Rva0048B164Get(void)
{
	return (int)"DynamicShroudClearingRangeUpdate";
}


NameKeyType DynamicShroudClearingRangeUpdate::getModuleNameKey() const
{
	static NameKeyType TheDynamicShroudClearingRangeUpdatePoolKey =
		TheNameKeyGenerator->nameToKey("DynamicShroudClearingRangeUpdate");
	return TheDynamicShroudClearingRangeUpdatePoolKey;
}


// The decal array teardown and the UpdateModule base destructor are implicit; the
// body is only the "just in case" clear the Zero Hour donor spells.
DynamicShroudClearingRangeUpdate::~DynamicShroudClearingRangeUpdate()
{
	killGridDecals();
}


void DynamicShroudClearingRangeUpdate::createGridDecals(
	const RadiusDecalTemplate &radiusDecalTemplate, Real radius, const Coord3D &pos)
{
	RadiusDecal *decal = m_gridDecal;
	int decalsLeft = 30;
	do {
		decal->clear();
		radiusDecalTemplate.createRadiusDecal(pos, radius, m_object->getControllingPlayer(), *decal);
		decal->setPosition(pos);
		++decal;
	} while (--decalsLeft != 0);
}


// The 30 decals are spread over a circle whose radius grows with the frames
// already spent, each snapped to a 23-unit grid: retail reads the object's
// position at +0x38 directly and walks the decal array by pointer.
void DynamicShroudClearingRangeUpdate::rva0048B256()
{
	const Coord3D *ctr = m_object->getPosition();
	Coord3D pos;
	pos.z = 0.0f;
	int span = m_totalFrames - m_stateCountDown;
	int doubled = span;
	doubled += span;
	float radius = m_currentClearingRange + doubled;
	float angle = 0.0f;
	float opacity = 1.0f - (m_currentClearingRange / m_nativeClearingRange);
	RadiusDecal *decal = m_gridDecal;
	int left = 30;
	do {
		pos.x = ctr->x + sin((double)angle) * radius;
		pos.y = ctr->y + cos((double)angle) * radius;
		pos.x -= ((Int)pos.x) % 23;
		pos.y -= ((Int)pos.y) % 23;
		decal->setPosition(pos);
		decal->setOpacity(opacity);
		angle += 0.20943951f;
		++decal;
	} while (--left != 0);
}


UpdateSleepTime DynamicShroudClearingRangeUpdate::update()
{
	if (m_state == DSCRU_SLEEPING)
		return UPDATE_SLEEP_NONE;
	Object *me = getObject();
	const DynamicShroudClearingRangeUpdateModuleData *md = getDynamicShroudClearingRangeUpdateModuleData();
	UnsignedInt currentFrame = TheGameLogic->getFrame();

	if (!m_decalsCreated) {
		createGridDecals(md->getGridDecalTemplate(), 100, *me->getPosition());
		m_decalsCreated = true;
	}

	if (m_stateCountDown <= 0 || currentFrame > m_doneForeverFrame)
		m_state = DSCRU_DONE_FOREVER;
	else if ((UnsignedInt)m_stateCountDown <= m_shrinkStartDeadline)
		m_state = DSCRU_SHRINKING;
	else if ((UnsignedInt)m_stateCountDown <= m_sustainDeadline)
		m_state = DSCRU_SUSTAINING;
	else if ((UnsignedInt)m_stateCountDown <= m_growStartDeadline)
		m_state = DSCRU_GROWING;

	switch (m_state) {
	case DSCRU_NOT_STARTED_YET:
		rva0048B256();
		break;
	case DSCRU_GROWING:
		rva0048B256();
		m_currentClearingRange += m_nativeClearingRange / bfmeMax(1.0f, (Real)md->m_growTime);
		if (m_currentClearingRange >= m_nativeClearingRange)
			m_state = DSCRU_SUSTAINING;
		break;
	case DSCRU_SUSTAINING:
		m_currentClearingRange = m_nativeClearingRange;
		killGridDecals();
		break;
	case DSCRU_SHRINKING:
		m_currentClearingRange -= (m_nativeClearingRange - md->m_finalVision) / bfmeMax(1.0f, (Real)md->m_shrinkTime);
		break;
	case DSCRU_DONE_FOREVER:
		killGridDecals();
		m_currentClearingRange = md->m_finalVision;
		break;
	}

	// This runs every frame whatever the state: it drives the animation and
	// reacts to scripted vision changes.
	if (m_stateCountDown > 0)
		m_stateCountDown--;

	if (m_changeIntervalCountdown > 0) {
		m_changeIntervalCountdown--;
	} else {
		m_changeIntervalCountdown = (m_state == DSCRU_GROWING) ? md->m_growInterval : md->m_changeInterval;
		me->setShroudClearingRange(m_currentClearingRange);
		if (m_state == DSCRU_DONE_FOREVER)
			m_state = DSCRU_SLEEPING;
	}

	return UPDATE_SLEEP_NONE;
}

// Base xfer, the light-CRC early out, Version1, then two int runs, five unsigned
// runs, the decals-created flag and three real runs.
void DynamicShroudClearingRangeUpdate::xfer(Xfer *xfer)
{
	UpdateModule::xfer(xfer);

	if (xfer->IsLightCRC())
		return;

	xfer->Version1();

	*xfer == m_stateCountDown;
	*xfer == m_totalFrames;
	*xfer == m_growStartDeadline;
	*xfer == m_sustainDeadline;
	*xfer == m_shrinkStartDeadline;
	*xfer == m_doneForeverFrame;
	*xfer == m_changeIntervalCountdown;
	*xfer == m_decalsCreated;
	*xfer == m_visionChangePerInterval;
	*xfer == m_nativeClearingRange;
	*xfer == m_currentClearingRange;
}



DynamicShroudClearingRangeUpdateModuleData::DynamicShroudClearingRangeUpdateModuleData()
{
	m_shrinkDelay = 0;
	m_shrinkTime = 0;
	m_growDelay = 0;
	m_growTime = 0;
	m_changeInterval = 0;
	m_growInterval = 0;
	m_doSpySatFX = false;
	m_finalVision = 0.0f;
}


void DynamicShroudClearingRangeUpdate::loadPostProcess()
{
	UpdateModule::loadPostProcess();
}

// 0x0048B164
const char *DynamicShroudClearingRangeUpdate::GetSnapshotName() const { return "DynamicShroudClearingRangeUpdate"; }

// FUNCTION: BFME2 0x0048B634 size=48 name=??1DynamicShroudClearingRangeUpdateModuleData@@UAE@XZ evidence=N:ledger tier=B hypothesis:legacy-membership
