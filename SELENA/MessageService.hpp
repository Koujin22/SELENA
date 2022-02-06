#pragma once

#include <functional>
#include "FrameworkResources.hpp"


namespace mgs {
	void messageReqRes(string portNumber, FrameworkResources* fwRsrc);
	void messageSender(string portNumber, FrameworkResources* fwRsrc);
	void createMessageService(string portNumber, FrameworkResources* fwRsrc);
}
