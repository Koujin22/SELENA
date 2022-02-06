#pragma once
#include "Resources.hpp"
#include <functional>

using namespace zmq;

class FrameworkResources : public Resources<function<void()>> {
public:

	FrameworkResources() :Resources<function<void()>>(3) {};

	void setWUS(function<void(string msg)>* wusPtr);
	void setTTS(function<void(string msg)>* ttsPtr);
	void setSTI(function<string(string msg)>* stiPtr);

	void sendWUS(string msg);
	void sendTTS(string msg);
	string sendSTI(string msg);
	bool isReady();

	~FrameworkResources();

private:

	function<void(string msg)>* sendMsgWUS = nullptr;
	function<void(string msg)>* sendMsgTTS = nullptr;
	function<string(string msg)>* sendMsgSTI = nullptr;

};
