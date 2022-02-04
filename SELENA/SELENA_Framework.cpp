#include "SELENA_Framework.hpp"

namespace {

    Event eventHandler;

    function<void(string msg)> sendMsgWUS = nullptr;
    zmq::socket_t* socketWUS = nullptr;

    function<void(string msg)> sendMsgTTS = nullptr;
    zmq::socket_t* socketTTS = nullptr;

    function<string(string msg)> comunicate = nullptr;
    zmq::socket_t* socketSTI = nullptr;

}


pair<vector<pair<thread*, token>*>*, Handles*> SELENA_FW::init(vector<zmq::socket_t*>* sockets) {

    cout << "||  Booting Framewoork...                         ||" << endl;


    //Start Thread msg reciever for wakeup service
    thread* wakeUpThread = new thread(mgs::createMessageService, "50000", &eventHandler, ref(sendMsgWUS), socketWUS);
    cout << "||  [OK] Wake-up thread started!                  ||" << endl;

    //Start Thread msg sender for tts
    thread* stiThread = new thread(mgs::messageSender, "50002", &eventHandler, ref(sendMsgTTS), socketTTS);
    cout << "||  [OK] Text-to-speech thread started!           ||" << endl;

    //Start Thread msg sender for sti
    thread* ttsThread = new thread(mgs::messageReqRes, "50003", &eventHandler, ref(comunicate), socketSTI);
    cout << "||  [OK] Speech-to-intent thread started!         ||" << endl;



    //Start wake up recognition
    token wusListener = pys::startProcess(&eventHandler, "demoPorcupine.py", true);
    cout << "||  [OK] Wakeu-p service started!                 ||" << endl;

    //Start speech to intent
    token stiListener = pys::startProcess(&eventHandler, "speechToIntent.py", true);
    cout << "||  [OK] Speech-to-intent service started!        ||" << endl;

    //Start text to speech service
    token ttsListener = pys::startProcess(&eventHandler, "textToSpeech.py", true);
    cout << "||  [OK] Text-to-speech service started!          ||" << endl;

    cout << "||  [..] Waiting for threas to finish init...     ||" << endl;
    while (sendMsgWUS == nullptr || comunicate == nullptr || sendMsgTTS == nullptr || socketSTI == nullptr || socketWUS == nullptr || socketTTS == nullptr) {
    }

    sockets->push_back(socketSTI);
    sockets->push_back(socketWUS);
    sockets->push_back(socketTTS);

    vector<pair<thread*, token>*>* frameworkThreads = new vector<pair<thread*, token>*>;

    frameworkThreads->push_back(new pair<thread*, token>(wakeUpThread, wusListener));
    frameworkThreads->push_back(new pair<thread*, token>(stiThread, stiListener));
    frameworkThreads->push_back(new pair<thread*, token>(ttsThread, ttsListener));

    cout << "||  [OK] Threads finish init correctly!           ||" << endl;

    cout << "||  Framework booted correctly!                   ||" << endl;
    return pair<vector<pair<thread*, token>*>*, Handles*>(frameworkThreads, new Handles(&sendMsgTTS, &sendMsgWUS, &comunicate, &eventHandler));
}