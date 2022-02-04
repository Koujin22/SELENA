#pragma once
#include "Handles.hpp"
#include <vector>
#include <windows.hpp>
#include <thread>
#include "HandlesMods.hpp"
namespace mdl {
	void eventListeners(HandlesMods* handle, Handles* handle_fw);


    void startProcess(string pathToExe, string arguments);


}