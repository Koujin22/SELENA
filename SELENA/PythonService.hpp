#pragma once
#include <windows.hpp>
#include <stdio.hpp>
#include <string>
#include <tchar.hpp>
#include <functional>
#include <cstdio>
#include "Intent.hpp"
#include "Event.hpp"

namespace pys {
	shared_ptr<void>  startProcess(Event* event, string pythonFile, bool show);
}
