// The horde's formation and movement configuration extends the transport data.
#pragma once

#include <stddef.h>
#include <list>
#include <set>
#include <vector>
#include "GameLogic/Module/TransportContainModuleData.h"

class HordeContainSplitEntry;

// One RankInfo line of the module data (the parse is 0x0047414D). Only the two leading members are recovered: +0 is the rank
// number 0x00469294 compares, +4 the unit type name 0x0046AF12 passes to ThingFactory::findTemplate; the parse allocates more.
struct HordeRankInfo
{
  Int m_rank;           // +0x0
  AsciiString m_unitType; // +0x4
};

class HordeContainModuleData : public TransportContainModuleData
{
public:
  HordeContainModuleData();
  virtual ~HordeContainModuleData();

  virtual const HordeContainModuleData *getHordeModuleData() const { return this; }
  static void buildFieldParse(MultiIniFieldParse &parse);
  static ModuleData *friend_newModuleData(INI *ini);
  HordeRankInfo *rva00469294(Int rank) const;

  _STL::vector<HordeRankInfo *> RankInfos; // offset=0x18C: the RankInfo lines; 0x00469294 finds the entry whose rank matches, 0x0046AF12 hands its unit type to ThingFactory::findTemplate
  unsigned char m_unk0x198[0xC]; // ComboHordes storage at +0x198
  _STL::vector<HordeContainSplitEntry *> SplitHordes; // offset=0x1A4: the SplitHordes lines (HordeContain interface slot 127, 0x004698BC, tests it for empty); ends at 0x1B0
  AsciiString AlternateFormation; // Name of the alternate formation passed to ThingFactory::findTemplate.
  _STL::list<Int> RanksThatStopAdvance; // offset=0x1B4 (parseHordeContainRanksThatStopAdvance)
  _STL::set<Int> RanksToReleaseWhenAttacking; // offset=0x1B8 (parseHordeContainRankSets)
  _STL::set<Int> RanksToJustFreeWhenAttacking; // offset=0x1C4 (parseHordeContainRankSets)
  unsigned char RandomOffset[0x8];
  bool ThisFormationIsTheMainFormation;
  unsigned char m_unk0x1D9[0x3];
  unsigned int BackUpMinDelayTime;
  unsigned int BackUpMaxDelayTime;
  float BackUpMinDistance;
  float BackUpMaxDistance;
  float BackupPercentage;
  float CowerRadius;
  unsigned char LeadersAllowed[0xC];
  unsigned char LeaderPosition[0xC];
  unsigned char m_unk0x20C[0xC];
  unsigned char BannerCarriersAllowed[0xC];
  _STL::vector<AsciiString> AttributeModifiers; // offset=0x224 (the strings HordeContain 0x0046981C/0x00469851/0x00469886 hand out)
  bool IsPorcupineFormation;
  unsigned char m_unk0x231[0x3];
  unsigned char ForcedLocomotorSet[0x4];
  bool MachineAllowed;
  unsigned char m_unk0x239[0x3];
  unsigned char MachineType[0x4];
  bool UseSlowHordeMovement;
  unsigned char m_unk0x241[0x3];
  float MeleeAttackLeashDistance;
  unsigned char m_unk0x248[0x4];
  bool RankSplit;
  unsigned char m_unk0x24D[0x3];
  int SplitHordeNumber;
  bool NotComboFormation;
  bool UseMarchingAnims;
  unsigned char m_unk0x256[0x6];
  float FrontAngle;
  unsigned int FlankedDelay;
  unsigned int FlankedDuration;
  int MinimumHordeSize;
  float VisionRearOverride;
  float VisionSideOverride;
};

typedef char HordeContainModuleData_rank_infos_offset[(offsetof(HordeContainModuleData, RankInfos) == 0x18C) ? 1 : -1];
typedef char HordeContainModuleData_split_hordes_offset[(offsetof(HordeContainModuleData, SplitHordes) == 0x1A4) ? 1 : -1];
typedef char check_size_HordeContainModuleData[sizeof(HordeContainModuleData)==0x274?1:-1];
typedef char check_HordeContainModuleData_AlternateFormation[offsetof(HordeContainModuleData,AlternateFormation)==0x1B0?1:-1];
typedef char check_HordeContainModuleData_RanksToReleaseWhenAttacking[offsetof(HordeContainModuleData,RanksToReleaseWhenAttacking)==0x1B8?1:-1];
typedef char check_HordeContainModuleData_RandomOffset[offsetof(HordeContainModuleData,RandomOffset)==0x1D0?1:-1];
typedef char check_HordeContainModuleData_ThisFormationIsTheMainFormation[offsetof(HordeContainModuleData,ThisFormationIsTheMainFormation)==0x1D8?1:-1];
typedef char check_HordeContainModuleData_BackUpMinDelayTime[offsetof(HordeContainModuleData,BackUpMinDelayTime)==0x1DC?1:-1];
typedef char check_HordeContainModuleData_BackUpMaxDelayTime[offsetof(HordeContainModuleData,BackUpMaxDelayTime)==0x1E0?1:-1];
typedef char check_HordeContainModuleData_BackUpMinDistance[offsetof(HordeContainModuleData,BackUpMinDistance)==0x1E4?1:-1];
typedef char check_HordeContainModuleData_BackUpMaxDistance[offsetof(HordeContainModuleData,BackUpMaxDistance)==0x1E8?1:-1];
typedef char check_HordeContainModuleData_BackupPercentage[offsetof(HordeContainModuleData,BackupPercentage)==0x1EC?1:-1];
typedef char check_HordeContainModuleData_CowerRadius[offsetof(HordeContainModuleData,CowerRadius)==0x1F0?1:-1];
typedef char check_HordeContainModuleData_LeadersAllowed[offsetof(HordeContainModuleData,LeadersAllowed)==0x1F4?1:-1];
typedef char check_HordeContainModuleData_LeaderPosition[offsetof(HordeContainModuleData,LeaderPosition)==0x200?1:-1];
typedef char check_HordeContainModuleData_BannerCarriersAllowed[offsetof(HordeContainModuleData,BannerCarriersAllowed)==0x218?1:-1];
typedef char check_HordeContainModuleData_AttributeModifiers[offsetof(HordeContainModuleData,AttributeModifiers)==0x224?1:-1];
typedef char check_HordeContainModuleData_IsPorcupineFormation[offsetof(HordeContainModuleData,IsPorcupineFormation)==0x230?1:-1];
typedef char check_HordeContainModuleData_ForcedLocomotorSet[offsetof(HordeContainModuleData,ForcedLocomotorSet)==0x234?1:-1];
typedef char check_HordeContainModuleData_MachineAllowed[offsetof(HordeContainModuleData,MachineAllowed)==0x238?1:-1];
typedef char check_HordeContainModuleData_MachineType[offsetof(HordeContainModuleData,MachineType)==0x23C?1:-1];
typedef char check_HordeContainModuleData_UseSlowHordeMovement[offsetof(HordeContainModuleData,UseSlowHordeMovement)==0x240?1:-1];
typedef char check_HordeContainModuleData_MeleeAttackLeashDistance[offsetof(HordeContainModuleData,MeleeAttackLeashDistance)==0x244?1:-1];
typedef char check_HordeContainModuleData_RankSplit[offsetof(HordeContainModuleData,RankSplit)==0x24C?1:-1];
typedef char check_HordeContainModuleData_SplitHordeNumber[offsetof(HordeContainModuleData,SplitHordeNumber)==0x250?1:-1];
typedef char check_HordeContainModuleData_NotComboFormation[offsetof(HordeContainModuleData,NotComboFormation)==0x254?1:-1];
typedef char check_HordeContainModuleData_UseMarchingAnims[offsetof(HordeContainModuleData,UseMarchingAnims)==0x255?1:-1];
typedef char check_HordeContainModuleData_FrontAngle[offsetof(HordeContainModuleData,FrontAngle)==0x25C?1:-1];
typedef char check_HordeContainModuleData_FlankedDelay[offsetof(HordeContainModuleData,FlankedDelay)==0x260?1:-1];
typedef char check_HordeContainModuleData_FlankedDuration[offsetof(HordeContainModuleData,FlankedDuration)==0x264?1:-1];
typedef char check_HordeContainModuleData_MinimumHordeSize[offsetof(HordeContainModuleData,MinimumHordeSize)==0x268?1:-1];
typedef char check_HordeContainModuleData_VisionRearOverride[offsetof(HordeContainModuleData,VisionRearOverride)==0x26C?1:-1];
typedef char check_HordeContainModuleData_VisionSideOverride[offsetof(HordeContainModuleData,VisionSideOverride)==0x270?1:-1];
