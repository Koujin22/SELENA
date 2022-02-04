#include "EventMods.hpp"

using namespace std;



void EventMods::emit(string eventName, Intent* intent) {


    cout << "Emiting: " << eventName << endl;
    list<weak_ptr<function<void(Intent*)>>>* funList = &eventsListeners[eventName];

    funList->erase(
        remove_if(funList->begin(), funList->end(), [&](weak_ptr<function<void(Intent*)>> t)->bool { return t.expired(); }),
        funList->end()
    );


    auto funList_Copy = funList; // in case targets is modified by listeners
    for (auto wp : *funList_Copy) {
        if (auto sp = wp.lock()) {
            (*sp)(intent);
        }
    }
}

token EventMods::add(string eventName, function<void(Intent*)> fun) {
    auto t = wrap_target(move(fun));

    list<weak_ptr<function<void(Intent*)>>>* funList = &eventsListeners[eventName];
    funList->push_back(t);
    return t;
}

shared_ptr<function<void(Intent*)>> wrap_target(function<void(Intent*)> t) {
    return make_shared<function<void(Intent*)>>(move(t));
};
