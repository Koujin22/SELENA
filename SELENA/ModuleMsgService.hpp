#pragma once

#include "EventMods.hpp"
#include <zmq.hpp>
#include <functional>
#include <thread>

namespace mms {
	void moduleSenderService(string portNumberPub, string portNumberSub, EventMods* event, function<void(string topic, string msg)>& sendMsg);
}