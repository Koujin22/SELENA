#include "SELENA_Framework.hpp"
#include "MessageService.hpp"
#include "PythonService.hpp"

FrameworkResources* SELENA_FW::init()
{

    FrameworkResources* fwRsrc = new FrameworkResources();

    cout << "||  Booting Framewoork...                         ||" << endl;


    //Start Thread msg reciever for wakeup service
    thread* wusThread = new thread(mgs::createMessageService, "50000", fwRsrc);
    fwRsrc->addThread(wusThread);

    cout << "||  [OK] Wake-up thread started! ("<<setfill('0') <<setw(6)<<wusThread->get_id() << ")         ||" << endl;

    //Start Thread msg sender for tts
    thread* ttsThread = new thread(mgs::messageSender, "50002", fwRsrc);
    fwRsrc->addThread(ttsThread);
    cout << "||  [OK] Text-to-speech thread started! (" << setfill('0') << setw(6) << ttsThread->get_id() << ")  ||" << endl;

    //Start Thread msg sender for sti
    thread* stiThread = new thread(mgs::messageReqRes, "50003", fwRsrc);
    fwRsrc->addThread(stiThread);
    cout << "||  [OK] Speech-to-intent thread started! (" << setfill('0') << setw(6) << stiThread->get_id() << ")||" << endl;



    //Start wake up recognition
    fwRsrc->addEventListenerPersist("stop", *pys::startProcess("demoPorcupine.py", false));
    cout << "||  [OK] Wake-up service started!                 ||" << endl;

    //Start speech to intent
    fwRsrc->addEventListenerPersist("stop", *pys::startProcess("speechToIntent.py", false));
    cout << "||  [OK] Speech-to-intent service started!        ||" << endl;

    //Start text to speech service
    fwRsrc->addEventListenerPersist("stop", *pys::startProcess("textToSpeech.py", false));

    cout << "||  [OK] Text-to-speech service started!          ||" << endl;

    cout << "||  [..] Waiting for threas to finish init...     ||" << endl;


    while (!fwRsrc->isReady()) {
        this_thread::sleep_for(500ms);
    }
   
    cout << "||  [OK] Threads finish init correctly!           ||" << endl;

    cout << "||  Framework booted correctly!                   ||" << endl;
    return fwRsrc;
}
