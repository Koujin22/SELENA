#include "ModulesResources.hpp"

void ModulesResources::setSendToMod(function<void(string, string)>* send2mod)
{
	sendMsgToMod = send2mod;
}

void ModulesResources::sendToMod(string topic, string msg)
{
	(*sendMsgToMod)(topic, msg);
}

bool ModulesResources::isReady()
{
	if (sendMsgToMod == nullptr) return false;
	else return true;
}

ModulesResources::~ModulesResources()
{
	delete sendMsgToMod;
}
