

#pragma once
#include <functional>
#include <thread>
#include <vector>
#include <utility>


#include "Event.hpp"
#include "MessageService.hpp"
#include "PythonService.hpp"
#include "Handles.hpp"

namespace SELENA_FW {

	pair<vector<pair<thread*, token>*>*, Handles*> init(vector<zmq::socket_t*>* sockets);

}