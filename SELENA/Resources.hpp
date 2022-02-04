
#pragma once
#include "Event.hpp"
#include <zmq.hpp>
#include <vector>
#include <thread>

using namespace std;
using namespace zmq;

template<typename T>
class Resources : public Event<T> {
public:

	Resources();

	virtual void add(thread*) = 0;
	virtual vector<thread*>* getThreads() = 0; 

	virtual void add(socket_t*) = 0;


protected:
	context_t* context;

	vector<socket_t*>* sockets;
	vector<thread*>* threads;
	vector<token>* tokens;



};
