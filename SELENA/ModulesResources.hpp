
#pragma once
#include "Resources.hpp"
#include <functional>
#include "Intent.hpp"

class ModulesResources : public Resources<function<void(Intent*)>>{
public:
	ModulesResources(size_t N) : Resources<function<void(Intent*)>>(N) {};

	void setSendToMod(function<void(string, string)>* send2mod);

	void sendToMod(string topic, string msg);

	bool isReady();

	~ModulesResources();
private:
	function<void(string, string)>* sendMsgToMod = nullptr;
};
