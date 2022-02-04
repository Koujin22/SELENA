
#pragma once
#include <string>
#include <map>
#include <list>
#include "Event.hpp"
#include <iostream>
#include <functional>

#include "Intent.hpp"

using namespace std;
using token = shared_ptr<void>;

class EventMods {

public:
	void emit(string eventName, Intent* intent);
	token add(string eventName, function<void(Intent*)> fun);

private:
	map<string, list<weak_ptr<function<void(Intent*)>>>> eventsListeners;
};

shared_ptr<function<void(Intent*)>> wrap_target(function<void(Intent*)> t);

