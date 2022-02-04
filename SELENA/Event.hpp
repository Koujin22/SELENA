
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
 
template<typename T>
class Event {
public:
	virtual void emit(string) = 0;
	virtual token add(string, T ) = 0;
	virtual void add(string, T, bool) = 0;
protected:
	shared_ptr<T> wrap_target(T t);
	map<string, list<weak_ptr<T>>> eventsListeners;
};


