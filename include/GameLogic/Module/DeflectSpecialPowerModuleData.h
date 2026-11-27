// Configuration for the deflect special power. It adds no fields of its own: ModuleFactory allocates
// SpecialPowerUpdateModuleData's size for it, and buildFieldParse forwards to the base and then adds an empty
// field table.
#pragma once
#ifndef BFME2_DEFLECT_SPECIAL_POWER_MODULE_DATA_H
#define BFME2_DEFLECT_SPECIAL_POWER_MODULE_DATA_H

#include "GameLogic/Module/SpecialPowerUpdateModule.h"

class MultiIniFieldParse;

class DeflectSpecialPowerModuleData : public SpecialPowerUpdateModuleData
{
public:
	static void buildFieldParse(MultiIniFieldParse &parse);
};
// SIZE 0x18
typedef char DeflectSpecialPowerModuleData_size[(sizeof(DeflectSpecialPowerModuleData) == 0x18) ? 1 : -1];

#endif
