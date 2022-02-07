#pragma once
#include <vector>
#include "Module.hpp"
#include "ModulesResources.hpp"
#include "FrameworkResources.hpp"

class ModuleManager {
public:
	ModuleManager(ModulesResources* modRsrc, FrameworkResources* fwRsrc);

	~ModuleManager();
private:
	vector<Module*>* modules;
};