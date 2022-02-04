#include "HandlesMods.hpp"

HandlesMods::HandlesMods(EventMods* event, function<void(string, string)>* msgMod) {
	eventHandler = event;
	sendMsgMod = msgMod;
};


token HandlesMods::add(string msg, function<void(Intent* intent)> fn) {
	return eventHandler->add(msg, fn);
};

EventMods* HandlesMods::getEventPtr() {
	return eventHandler;
};

void HandlesMods::emit(string event, Intent* intent) {
	eventHandler->emit(event, intent);
}


void HandlesMods::addPtrListener(token tkn) {
	ptrToListener.push_back(tkn);
};

void HandlesMods::emptyPtrListener() {
	ptrToListener.clear();
};


void HandlesMods::sendMod(string topic, string msg) {
	(*sendMsgMod)(topic, msg);
}