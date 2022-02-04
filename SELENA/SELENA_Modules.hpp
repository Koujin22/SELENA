

#pragma once
#include "ModuleLoader.hpp"
#include "ModuleMsgService.hpp"
#include "HandlesMods.hpp"

namespace SELENA_Mods {
	pair<vector<thread*>*, HandlesMods*> init();
}