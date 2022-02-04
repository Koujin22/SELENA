
#include "util.hpp"

vector<string> util::split(string str, char del) {

	stringstream test(str);
	string segment;
	vector<string> seglist;

	while (getline(test, segment, del))
	{
		seglist.push_back(segment);
	}
	return seglist;
}


Intent* util::processString(string msg) {
    //check if msg is serialized obj
    vector<string> splitMsg = util::split(msg, '(');
    cout << "Size: " << splitMsg.size() << endl;
    if (splitMsg.size() > 1) {
        //it is
        cout << "First word: " << splitMsg[0] << endl;
        if (splitMsg[0] == "Inference") {
            //its of inference type
            Intent newIntent(msg);
            cout << "New intent detected. Parse values | is_understood: " << newIntent.isUnderstood() << " | intent: " << newIntent.getIntent() << " | slots: { ";
            newIntent.printSlot();
            return &newIntent;
        }
    }
}