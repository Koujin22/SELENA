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
#include "FrameworkResources.hpp"
#include "ModulesResources.hpp"

#include "ModuleManager.hpp"

#ifndef _WIN32
#include <unistd.hpp>
#else
#include <Windows.h>
#endif

using namespace std;

namespace {
    FrameworkResources* fwRsrc = nullptr;
    ModulesResources* modRsrc = nullptr;
}

void wakeUp() {

    //eventHandler.emit("pausewakeup");
    fwRsrc->sendWUS("pause");

    fwRsrc->sendTTS("¿En qué te puedo ayudar?");

    string response = fwRsrc->sendSTI("process");

    Intent intent(response);

    if (intent.isUnderstood()) {
        if (intent.getIntent() == "stop") {
            fwRsrc->sendTTS("Descansa");
            this_thread::sleep_for(1750ms);
            fwRsrc->emit("stop");
        }
        modRsrc->emit(intent.getIntent(), &intent);
    }
    else
        fwRsrc->sendTTS("Lo lamento, no te pude entender");

    fwRsrc->sendWUS("resume");

}


int main() {
      
    cout << "####################################################" << endl;
    cout << "||                                                ||" << endl;
    cout << "||               S.E.L.E.N.A.                     ||" << endl;
    cout << "||                                                ||" << endl;
    cout << "####################################################" << endl;
    cout << "||                                                ||" << endl;

    fwRsrc = SELENA_FW::init();

    cout << "||                                                ||" << endl;
    cout << "####################################################" << endl;
    cout << "||                                                ||" << endl;

    modRsrc = SELENA_Mods::init();

    cout << "||                                                ||" << endl;
    cout << "####################################################" << endl;

    //Subscribe wake to events
    cout << "Subscribe wake to events" << endl;
    fwRsrc->addEventListenerPersist("wake", wakeUp);

    ModuleManager* modMng = new ModuleManager(modRsrc, fwRsrc);

        

    //system ready
    fwRsrc->sendTTS("Estoy lista");

    vector<thread*>* modsThreadToWiat = modRsrc->getThreads();
    for (vector<thread*>::iterator it = modsThreadToWiat->begin(); it != modsThreadToWiat->end(); ++it) {
        (*it)->join();
    }

    vector<thread*>* threadsToWait = fwRsrc->getThreads();
    for (vector<thread*>::iterator it = threadsToWait->begin(); it != threadsToWait->end(); ++it) {
        (*it)->join();
    }

    delete modMng;
    delete modRsrc;
    delete fwRsrc;

    return 0;
}
