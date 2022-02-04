#pragma once
#include "EventMods.hpp"
#include <vector>

class HandlesMods {
public:
    HandlesMods(EventMods* event, function<void(string, string)>* sendMsgMod);

    EventMods* getEventPtr();
    token add(string msg, function<void(Intent*)> fn);
    void emit(string event, Intent* intent);

    void addPtrListener(token);
    void emptyPtrListener();

    void sendMod(string topic, string msg);
private:
    EventMods* eventHandler;
    vector<token> ptrToListener;
    function<void(string, string)>* sendMsgMod;
};