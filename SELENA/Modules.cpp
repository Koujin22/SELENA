
#include "Modules.hpp"
#include "PythonService.hpp"

void mods::addModulesEventHandlers(FrameworkResources* fwRsrc, ModulesResources* modRsrc)
{

    cout << "Setting module events" << endl;
    modRsrc->addEventListenerPersist("lol_start", [fwRsrc, modRsrc](Intent* _) -> void {
        cout << "reciby lol_start" << endl;
        fwRsrc->sendTTS("Iniciando League of Legends. Diviértete.");
        fwRsrc->addEventListenerPersist("stop", *pys::startProcess("lol.py 50005 50004 -d=true -s=lol", true));
        });

    modRsrc->addEventListenerPersist("lol_queue_start", [fwRsrc, modRsrc](Intent* _) ->void {
        modRsrc->sendToMod("lol", "lol_queue_start");
        fwRsrc->sendTTS("Empezando cola. Mucho exito");
        });

    modRsrc->addEventListenerPersist("lol_queue_quit", [fwRsrc, modRsrc](Intent* _) ->void {
        modRsrc->sendToMod("lol", "lol_queue_quit");
        fwRsrc->sendTTS("Cancelando cola");
        });

}
