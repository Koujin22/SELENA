#include "FrameworkResources.hpp"

void FrameworkResources::setWUS(function<void(string msg)>* wusPtr)
{
	sendMsgWUS = wusPtr;
}

void FrameworkResources::setTTS(function<void(string msg)>* ttsPtr)
{
	sendMsgTTS = ttsPtr;
}

void FrameworkResources::setSTI(function<string(string msg)>* stiPtr)
{
	sendMsgSTI = stiPtr;
}

void FrameworkResources::sendWUS(string msg)
{
	(*sendMsgWUS)(msg);
}

void FrameworkResources::sendTTS(string msg)
{
	(*sendMsgTTS)(msg);
}

string FrameworkResources::sendSTI(string msg)
{
	return (*sendMsgSTI)(msg);
}

bool FrameworkResources::isReady()
{
	if (sendMsgSTI == nullptr || sendMsgTTS == nullptr || sendMsgWUS == nullptr)
		return false;
	else
		return true;
}

FrameworkResources::~FrameworkResources()
{
	delete this->sendMsgSTI;
	delete this->sendMsgTTS;
	delete this->sendMsgWUS;
}
