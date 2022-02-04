//
//  Hello World server in C++
//  Binds REP socket to tcp://*:5555
//  Expects "Hello" from client, replies with "World"
//
#include <thread>
#include <iostream>
#include <tuple>

#include "SELENA_Framework.hpp"
#include "SELENA_Modules.hpp"

#ifndef _WIN32
#include <unistd.hpp>
#else
#include <windows.hpp>
#endif

using namespace std;

namespace {
    vector<pair<thread*, token>*>* fw;
    vector<thread*>* mods;

    Handles* fwHandle;
    HandlesMods* modsHandle;
}


void wakeUp() {

    //eventHandler.emit("pausewakeup");
    fwHandle->sendWUS("pause");

    fwHandle->sendTTS("¿En qué te puedo ayudar?");

    string response = fwHandle->getIntent("process");

    Intent intent(response);

    if (intent.isUnderstood()) {
        if (intent.getIntent() == "stop") {
            fwHandle->sendTTS("Descansa");
            this_thread::sleep_for(1750ms);
            fwHandle->emit("stop");
        }
        modsHandle->emit(intent.getIntent(), &intent);
    }
    else
        fwHandle->sendTTS("Lo lamento, no te pude entender");

    fwHandle->sendWUS("resume");

}


int main() {
      
    cout << "####################################################" << endl;
    cout << "||                                                ||" << endl;
    cout << "||               S.E.L.E.N.A.                     ||" << endl;
    cout << "||                                                ||" << endl;
    cout << "####################################################" << endl;
    cout << "||                                                ||" << endl;

    tie(fw, fwHandle) = SELENA_FW::init();

    cout << "||                                                ||" << endl;
    cout << "####################################################" << endl;
    cout << "||                                                ||" << endl;

    tie(mods, modsHandle) = SELENA_Mods::init();

    cout << "||                                                ||" << endl;
    cout << "####################################################" << endl;

    //Subscribe wake to events
    cout << "Subscribe wake to events" << endl;
    shared_ptr<void> listenerHandle = fwHandle->add("wake", wakeUp);

    mdl::eventListeners(modsHandle, fwHandle);

    token tkn = fwHandle->add("ready", []() -> void {
        fwHandle->sendTTS("Estoy lista");
        });

    //system ready
    fwHandle->emit("ready");


    for (vector<thread*>::iterator it = mods->begin(); it != mods->end(); ++it) {
        (*it)->join();
    }

    for (vector<pair<thread*, token>*>::iterator it = fw->begin(); it != fw->end(); ++it) {
        ((*it)->first)->join();
    }

    delete mods;
    delete fw;


    return 0;
}
