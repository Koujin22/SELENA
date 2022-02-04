#pragma once

#include "Event.hpp"
#include <thread>
#include <zmq.hpp>
#include <iostream>
#include <functional>
#include <vector>
#include "Handles.hpp"

namespace mgs {
	void messageReqRes(string portNumber, Event* event, function<string(string msg)>& comunicate, zmq::socket_t* socke);
	void messageSender(string portNumber, Event* event, function<void(string msg)>& sendMsg, zmq::socket_t* socke);
	void createMessageService(string portNumber, Event* event, function<void(string)>& sendMsg, zmq::socket_t* socke);
}
