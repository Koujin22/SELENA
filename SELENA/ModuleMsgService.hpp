#pragma once

#include <zmq.hpp>
#include <functional>
#include <thread>
#include "ModulesResources.hpp"

namespace mms {
	void moduleSenderService(string portNumberPub, string portNumberSub, ModulesResources* modRsrc );
}