#include "SELENA_Modules.hpp"
#include "EventMods.hpp"
#include "HandlesMods.hpp"

namespace {
    EventMods eventHandler;
    function<void(string topic, string msg)> sendToModule = nullptr;
}

pair<vector<thread*>*, HandlesMods*> SELENA_Mods::init() {


    cout << "||                                                ||" << endl;
    cout << "||  Booting Modules Service...                    ||" << endl;

    vector<thread*>* moduleThreads = new vector<thread*>();

    moduleThreads->push_back(new thread(mms::moduleSenderService, "50004", "5005", &eventHandler, ref(sendToModule)));
    cout << "||  [OK] Module thread started!                   ||" << endl;

    //while (sendToModule == nullptr) {}
    cout << "||  Modules Services Booted correctly!            ||" << endl;

    return pair<vector<thread*>*, HandlesMods*>(moduleThreads, new HandlesMods(&eventHandler, &sendToModule));

}