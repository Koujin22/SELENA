
#include "Intent.hpp"
#include "Util.hpp"

using namespace std;

Intent::Intent(string serializedObj) {

	//example string Inference(is_understood=True, intent='Iniciar', slots={'aplication': 'lol'})
	//remove Inference( ... )
	string cleanString = util::split(util::split(serializedObj, '(')[1], ')')[0];
	//split attributes
	vector<string> attr = util::split(cleanString, ' ');

	//setting is understood
	if (attr[0] == "is_understood=True,") {
		is_understood = true;
	}
	else {
		is_understood = false;
	}

	if (is_understood) {

		//setting intent
		intent = util::split(attr[1], '\'')[1];

		//cehck if object hav slots
		if (attr[2] != "slots={}") {
			//it does
			vector<string> slotsLists = util::split(util::split(util::split(serializedObj, '{')[1], '}')[0], ',');
			for (string slot : slotsLists) {
				vector<string> currentSlot = util::split(slot, '\'');
				slots[currentSlot[1]] = currentSlot[3];
			}
		}
	}
};
bool Intent::isUnderstood() {
	return is_understood;
};
string Intent::getIntent() {
	return intent;
};
string Intent::getSlot(string slot) {
	return slots[slot];
};

void Intent::printSlot() {
	for (auto it = slots.cbegin(); it != slots.cend(); ++it)
	{
		cout << it->first << ": " << it->second << ", ";
	}
	cout << "}\n";
};