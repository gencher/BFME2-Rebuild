// Object boundaries may be unproven.
// FUNCTION: BFME2 0x004AE7A7 size=6 name=?GetSnapshotName@DetachableRiderUpdate@@MBEPBDXZ evidence=N:ledger tier=B hypothesis:mm1-held-object-same-path
// FUNCTION: BFME2 0x004AE7AD size=72 name=?xfer@DetachableRiderUpdate@@MAEXPAVXfer@@@Z evidence=N:ledger tier=B hypothesis:mm1-held-object-same-path
// FUNCTION: BFME2 0x004AE832 size=69 name=?getModuleNameKey@DetachableRiderUpdate@@UBE?AW4NameKeyType@@XZ evidence=N:ledger tier=B hypothesis:mm1-held-object-same-path
// FUNCTION: BFME2 0x004AE883 size=91 name=??0DetachableRiderUpdate@@QAE@PAVThing@@PBVModuleData@@@Z evidence=N:ledger tier=B hypothesis:mm1-held-object-same-path
// FUNCTION: BFME2 0x004AE8FA size=15 name=?rva004AE8FA@DetachableRiderUpdate@@QAEXXZ evidence=?:placeholder tier=B hypothesis:mm1-held-object-same-path
// FUNCTION: BFME2 0x004AE909 size=12 name=?get@Rva004AE909ImulAddField@@QBEHH@Z evidence=?:placeholder tier=B hypothesis:mm1-held-object-same-path
// FUNCTION: BFME2 0x004AE988 size=114 name=?killRider@DetachableRiderUpdate@@QAEXXZ evidence=N:ledger
#include "Common/INI.h"
#include "Common/BitFlags.h"
#include "Common/INIException.h"
#include <string.h>
void *AllocateRawBytes(unsigned int,const void *);
inline const unsigned int &RiderArrayMax(const unsigned int &a,const unsigned int &b) { return a < b ? b : a; }
#include "GameLogic/Module/RiderInfoAllocator.h"
#include <stdlib.h>
#include "GameLogic/ObjectCreationList.h"
#include "GameLogic/GameLogic.h"
#include "GameLogic/Module/AIUpdate.h"
#include "GameClient/Drawable.h"
#include "GameLogic/Module/DetachableRiderUpdateModuleData.h"
#include "GameLogic/Object.h"
#include "Common/INI/FieldParse.h"
extern const FieldParse DetachableRiderFields[];
int GetGameLogicRandomValue(int,int,char*,int);
#include "GameLogic/Module/DetachableRiderUpdate.h"
#include "GameLogic/Module/RiderInfoArray.h"
#include "Common/NameKeyGenerator.h"
#include "Common/Xfer.h"
// GameLogic/Object/Update/DetachableRiderUpdate.cpp: retail translation unit (skeleton).
// cl: /Oi- 


DetachableRiderUpdate::~DetachableRiderUpdate() {}


const char *DetachableRiderUpdate::GetSnapshotName() const { return "DetachableRiderUpdate"; }


void DetachableRiderUpdate::xfer(Xfer *xfer)
{
 Xfer::Version version(1,1);
 *xfer == version;
 UpdateModule::xfer(xfer);
 *xfer == m_active;
 *xfer == m_riderIndex;
}


RiderInfo *RiderInfoAllocator::allocate(unsigned int count,const void *) const
{ return count ? (RiderInfo *)AllocateRawBytes(count*sizeof(RiderInfo),0) : 0; }


RiderInfo::RiderInfo(const RiderInfo &that)
 : m_conditions(that.m_conditions), m_animationTime(that.m_animationTime), m_creation(that.m_creation) {}


NameKeyType DetachableRiderUpdate::getModuleNameKey() const
{
 static NameKeyType TheDetachableRiderUpdatePoolKey = TheNameKeyGenerator->nameToKey("DetachableRiderUpdate");
 return TheDetachableRiderUpdatePoolKey;
}


RiderInfo::RiderInfo() {}


DetachableRiderUpdate::DetachableRiderUpdate(Thing *thing,const ModuleData *moduleData)
 : UpdateModule(thing,moduleData),m_active(false),m_riderIndex(false)
{ setWakeFrame(getObject(),UPDATE_SLEEP_FOREVER); }



void DetachableRiderUpdate::rva004AE8FA()
{ m_active=false; setWakeFrame(m_object,UPDATE_SLEEP_NONE); }


RiderInfo *RiderInfoArray::get(int index) const { return m_begin + index; }


void ConstructRiderInfo(RiderInfo *p,const RiderInfo &value)
{ if (p) p->RiderInfo::RiderInfo(value); }


RiderInfo *CopyUninitializedRiderInfo(RiderInfo *first,RiderInfo *last,RiderInfo *result,const FalseType &)
{ RiderInfo *cur=result; for(;first!=last;++first,++cur) ConstructRiderInfo(cur,*first); return cur; }


RiderInfo *FillUninitializedRiderInfo(RiderInfo *first,unsigned int count,const RiderInfo &value,const FalseType &)
{ RiderInfo *cur=first; for(;count>0;--count,++cur) ConstructRiderInfo(cur,value); return cur; }


DisabledMaskType DetachableRiderUpdate::getDisabledTypesToProcess() const
{ return DisabledMaskType(DisabledMaskType::kInit,3,4); }


void DetachableRiderUpdate::killRider()
{
	const DetachableRiderUpdateModuleData *arrayHolder = (const DetachableRiderUpdateModuleData *)getModuleData();
	int count = (int)(((char *)arrayHolder->m_records.m_end - (char *)arrayHolder->m_records.m_begin) / 0x54);
	Object *obj = m_object;
	if (count < 1)
		return;
	int randomIndex = GetGameLogicRandomValue(0, count - 1, "C:\\projects\\bfme2patch103\\bfme2\\Code\\GameEngine\\Source\\GameLogic\\Object\\Update\\DetachableRiderUpdate.cpp", 0x114);
	bool b = randomIndex != 0;
	m_riderIndex = b;
	RiderInfo *deathEntry = (RiderInfo *)((char *)arrayHolder->m_records.m_begin + (unsigned int)b * 0x54);
	obj->setModelConditionFlags(deathEntry->m_conditions);
	obj->setDisabled((DisabledType)4);
	m_active = 1;
	UpdateSleepTime st = *(UpdateSleepTime *)((char *)arrayHolder->m_records.m_begin + (unsigned int)m_riderIndex * 0x54 + 0x4c);
	setWakeFrame(obj, st);
}


void RiderInfoArray::insertOverflow(RiderInfo *position,const RiderInfo &value,const FalseType &,unsigned int count,bool atEnd)
{
 unsigned oldSize = m_end-m_begin;
 unsigned length = oldSize + RiderArrayMax(oldSize,count);
 RiderInfo *start=((RiderInfoAllocator *)&m_capacity)->allocate(length);
 RiderInfo *finish=start;
 finish=CopyUninitializedRiderInfo(m_begin,position,start,FalseType());
 if(count == 1) { ConstructRiderInfo(finish,value); ++finish; }
 else finish=FillUninitializedRiderInfo(finish,count,value,FalseType());
 if(!atEnd) finish=CopyUninitializedRiderInfo(position,m_end,finish,FalseType());
 if(m_begin) free(m_begin);
 m_begin=start; m_end=finish; m_capacity=start+length;
}


void RiderInfoArray::push_back(const RiderInfo &value)
{
 if(m_end != m_capacity) { ConstructRiderInfo(m_end,value); ++m_end; }
 else insertOverflow(m_end,value,FalseType(),1,true);
}


void ParseRiderInfo(INI *ini,void *instance,void *store,const void *)
{
 RiderInfo info;
 const char *token=ini->getNextToken(ini->m_sepsColon);
 if(token==0 || strcmp(token,"AnimState")!=0) throw INIException(3,"AnimState expected");
 int bit=BitFlags<591>::getSingleBitFromName(ini->getNextToken());
 info.m_conditions=ModelConditionFlags(ModelConditionFlags::kInit,bit);
 token=ini->getNextToken(ini->m_sepsColon);
 if(token==0 || strcmp(token,"AnimTime")!=0) throw INIException(3,"AnimDuration expected");
 INI::parseDurationUnsignedInt(ini,instance,&info.m_animationTime,0);
 token=ini->getNextToken(ini->m_sepsColon);
 if(token==0 || strcmp(token,"RiderOCL")!=0) throw INIException(3,"RiderOCL expected");
 INI::parseObjectCreationList(ini,instance,&info.m_creation,0);
 ((RiderInfoArray *)store)->push_back(info);
}


UpdateSleepTime DetachableRiderUpdate::update(){
 const DetachableRiderUpdateModuleData*data=(const DetachableRiderUpdateModuleData*)m_moduleData;
 Object*object=m_object;
 Drawable*draw=object->getDrawable();object->clearDisabled((DisabledType)4);
 if(m_active){
  if(draw)for(unsigned i=0;i<data->m_names.size();++i)draw->rva002724FD(data->m_names[i],0,0,0.0f,0.0f);
  ObjectCreationList*creation=data->m_records.m_begin[m_riderIndex].m_creation;
  if(creation)creation->create(object,object,0);
  object->clearModelConditionFlags(data->m_records.m_begin[m_riderIndex].m_conditions);
  object->setWeaponLock((WeaponSlotType)data->m_weapon,(WeaponLockType)2);object->setStatus((ObjectStatusTypes)39,true);
  object->setModelConditionState(MODELCONDITION_RIDERLESS);
  if(data->m_removeRiderlessFromHorde){
   Object*parent=object->m_containedBy;
   if(parent){
    HordeContainInterface*horde=parent->getHordeContainInterface();
    if(horde)horde->removeMemberFromHorde(object);else {ContainModuleInterface*riderContain=parent->m_contain;if(riderContain)riderContain->removeFromContain(object,false);}
    if(object->m_ai)object->m_ai->aiIdle((CommandSourceType)2);
   }
   TheGameLogic->deselectObject(object,0xfffff,true);
  }else if(data->m_riderlessHordeFlees){
   Object*parent=object->m_containedBy;
   if(parent){
    if(parent->getHordeContainInterface()){
     int total=parent->m_contain->getCount(false);int count=0;
     ContainmentListView list=parent->m_contain->getList();
    
     ContainmentList::const_iterator begin=list.objects().begin();
     ContainmentList::const_iterator end=list.objects().end();
     ContainmentList::const_iterator i=list.objects().begin();
     while(i!=end){Object*child=*i;if(child->testStatus((ObjectStatusTypes)39))++count;++i;}
     if(count==total && begin!=end){
      i=begin;
      do {Object*child=*i;++i;child->rva00298893();}while(i!=list.objects().end());
     }
    }
   }else object->rva00298893();
  }
 }else{
  if(draw)for(unsigned i=0;i<data->m_names.size();++i)draw->rva002724FD(data->m_names[i],1,0,0.0f,0.0f);
  object->releaseWeaponLock((WeaponLockType)2);object->setStatus((ObjectStatusTypes)39,false);
  object->clearModelConditionState(MODELCONDITION_RIDERLESS);
 }
 return UPDATE_SLEEP_FOREVER;
}



void DetachableRiderUpdateModuleData::buildFieldParse(MultiIniFieldParse &parse)
{ parse.add(DetachableRiderFields,0); }




// FUNCTION: BFME2 0x004AEAB7 size=53 name=??0DetachableRiderUpdateModuleData@@QAE@XZ evidence=N:ledger tier=B hypothesis:mm1-held-object-same-path

// FUNCTION: BFME2 0x004AEB3F size=69 name=??1DetachableRiderUpdateModuleData@@UAE@XZ evidence=N:ledger tier=B hypothesis:mm1-held-object-same-path

// FUNCTION: BFME2 0x004AEB84 size=278 name=?Rva004AEB84Parse@@YAXPAVINI@@PAX1PBX@Z evidence=?:placeholder tier=B hypothesis:mm1-held-object-same-path

// FUNCTION: BFME2 0x004AEC9B size=618 name=?update@DetachableRiderUpdateUpdateReceiver@@QAE?AW4UpdateSleepTime@@XZ evidence=N:ledger tier=B hypothesis:mm1-neighbour-pointers-mixed
