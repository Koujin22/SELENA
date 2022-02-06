
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

	Resources(size_t N);
	
	void addThread(thread*);
	vector<thread*>* getThreads();

	socket_t* addSocket(socket_type);

	virtual bool isReady() = 0;


	~Resources();

protected:
	context_t* context;

	vector<socket_t*>* sockets;
	vector<thread*>* threads;



};


template<typename T>
inline vector<thread*>* Resources<T>::getThreads()
{
	return this->threads;
}

template<typename T>
inline void Resources<T>::addThread(thread* thread)
{
	this->threads->push_back(thread);
}

template<typename T>
inline socket_t* Resources<T>::addSocket(socket_type socketType)
{

	socket_t* socket = new socket_t(*this->context, socketType);
	this->sockets->push_back(socket);
	return socket;
}

template<typename T>
inline Resources<T>::Resources(size_t N) : Event<T>(N)
{
	context = new context_t(1);

	sockets = new vector<socket_t*>();
	threads = new vector<thread*>();
}

template<typename T>
inline Resources<T>::~Resources()
{
	for (socket_t* b : *this->sockets) {
		b->close();
		delete b;
	}
	delete this->sockets;
	for (thread* t : *this->threads) {
		if(t->joinable())
			t->join();
		delete t;
	}
	delete this->threads;
	this->context->close();
	delete this->context;
}
