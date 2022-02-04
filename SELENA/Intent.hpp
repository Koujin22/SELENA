#pragma once
#include <string>
#include <map>
#include <iostream>


using namespace std;

class Intent {
public:
	Intent(string serializedObj);
	bool isUnderstood();
	string getIntent();
	string getSlot(string key);
	void printSlot();
private:
	bool is_understood;
	string intent;
	map<string, string> slots;
};