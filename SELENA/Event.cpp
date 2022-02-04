#include "Event.hpp"

using namespace std;



void Event::emit(string eventName){


    cout << "Emiting: " << eventName << endl;
    list<weak_ptr<function<void()>>>* funList = &eventsListeners[eventName];

    funList->erase(
        remove_if(funList->begin(), funList->end(), [&](weak_ptr<function<void()>> t)->bool { return t.expired(); }),
        funList->end()
    );


    auto funList_Copy = funList; // in case targets is modified by listeners
    for (auto wp : *funList_Copy) {
        if (auto sp = wp.lock()) {
            (*sp)();
        }
    }
}

token Event::add(string eventName, function<void()> fun) {
	auto t = wrap_target(move(fun));

	list<weak_ptr<function<void()>>>*funList = &eventsListeners[eventName];
	funList->push_back(t);
	return t;
}

shared_ptr<function<void()>> wrap_target(function<void()> t) {
	return make_shared<function<void()>> (move(t));
};


/*
* struct broadcaster {

    static shared_ptr<function<void()>> wrap_target( function<void()> t ) {
        return std::make_shared<function<void()>>(std::move(t));
    };

    token start_to_listen( function<void()> f ) {
        auto t = wrap_target(move(f));
        targets.push_back(t);
        return t;
    }

    void broadcast( Args... args ) {
        targets.erase(
            remove_if( targets.begin(), targets.end(),[&]( weak_ptr<function<void()> t )->bool { return t.expired(); }),
            targets.end()
        );
        auto targets_copy = targets; // in case targets is modified by listeners
        for (auto wp : targets_copy) {
        if (auto sp = wp.lock()) {
            (*sp)(args...);
        }
        }
    }
    std::vector<weak_ptr<function<void()>>> targets;
};
*/