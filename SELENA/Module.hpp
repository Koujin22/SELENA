#pragma once
#include <string>
#include "ModulesResources.hpp"
#include "FrameworkResources.hpp"
#include "ModulePlatform.hpp"
#include <functional>
#include <mutex>

class Module {
public:
	Module(ModPlatform platformType, string nameOfFile, string identifier, ModulesResources* modRsrc, bool debug);

	void startProcess(FrameworkResources* fwRsrc);

	void addEventToken(token);

	bool isAlive();

	~Module();
private:
	function<bool()>* isAliveFn = nullptr;

	string id;
	string arguments;
	string fileName;
	bool debug_f;

	ModPlatform platform;

	token processStop;
	token aliveListener;
	vector<token> eventListeners;

	mutex lockObj;
	condition_variable signal;

};
