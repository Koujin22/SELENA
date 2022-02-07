
#pragma once

#include <string>
#include <map>
#include <list>
#include "Event.hpp"
#include <iostream>
#include <functional>

#include "Intent.hpp"
#include "ThreadPool.hpp"

using namespace std;
using token = shared_ptr<void>;
 
template<typename T>
class Event {
public:
	Event(size_t n);

	template<typename ...Args>
	void emit(string, Args...);

	token addEventListener(string, T );
	void addEventListenerPersist(string, T);

	~Event();
protected:
	shared_ptr<T> wrap_target(T t);
	map<string, list<weak_ptr<T>>> eventsListeners;
private:
	ThreadPoolMng<T>* thPool;
	vector<token>* tokens;
};



template<typename T>
inline token Event<T>::addEventListener(string eventName, T fun)
{
	auto t = this->wrap_target(move(fun));

	list<weak_ptr<T>>* funList = &(this->eventsListeners[eventName]);
	funList->push_back(t);
	return t;
}

template<typename T>
inline void Event<T>::addEventListenerPersist(string eventName, T fun)
{
	this->tokens->push_back(this->addEventListener(eventName, fun));
}

template<typename T>
inline Event<T>::~Event()
{

	delete this->tokens;
	delete this->thPool;
}

template<typename T>
inline shared_ptr<T> Event<T>::wrap_target(T t)
{
	return make_shared<T>(move(t));
};

template<typename T>
template<typename ...Args>
inline void Event<T>::emit(string eventName, Args... args) {
	cout << "Emiting: " << eventName << endl;
	list<weak_ptr<T>>* funList = &eventsListeners[eventName];

	funList->erase(
		remove_if(funList->begin(), funList->end(), [&](weak_ptr<T> t)->bool { return t.expired(); }),
		funList->end()
	);


	auto funList_Copy = funList; // in case targets is modified by listeners
	for (auto wp : *funList_Copy) {
		if (auto sp = wp.lock()) {
			thPool->call(*sp, args...);
		}
	}
}

template<typename T>
inline Event<T>::Event(size_t N) {
	this->thPool = new ThreadPoolMng<T>(N);

	tokens = new vector<token>();
}