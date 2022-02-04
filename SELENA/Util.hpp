#pragma once
#include <string>
#include <vector>
#include <sstream>
#include "Intent.hpp"

using namespace std;

namespace util {
	vector<string> split(string str, char del);
	Intent* processString(string msg);

}