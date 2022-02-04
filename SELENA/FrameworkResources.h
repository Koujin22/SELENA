#pragma once
#include "Resources.hpp"

class FrameworkResources : public Resources<T> {
public:

	void emit(string);
	token add(string, T);
	void add(string, T, bool);

	void add(thread*);
	vector<thread*>* getThreads();

	void add(socket_t*);

	void setWUS(function<void(string msg)>);
	void setTTS(function<void(string msg)>);
	void setSTI(function<string(string msg)>);

	void sendWUS(string msg);
	void sendTTS(string msg);
	string sendSTI(string msg);

private:

	function<void(string msg)>* sendMsgWUS = nullptr;
	function<void(string msg)>* sendMsgTTS = nullptr;
	function<string(string msg)>* sendMsgSTI = nullptr;

};
