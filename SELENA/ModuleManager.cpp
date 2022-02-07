#include "ModuleManager.hpp"

ModuleManager::ModuleManager(ModulesResources* modRsrc, FrameworkResources* fwRsrc)
{

    cout << "Setting module events" << endl;
	modules = new vector<Module*>();

	//LoL.py module
    cout << "Setting lol module" << endl;
    Module* lolMod = new Module(ModPlatform::python, "lol.py", "lol", modRsrc, false);

    lolMod->addEventToken(modRsrc->addEventListener(
        "lol_start",
        [lolMod, fwRsrc](Intent* _) -> void {
            cout << "reciby lol_start" << endl;
            if (!lolMod->isAlive()) {
                lolMod->startProcess(fwRsrc);
                fwRsrc->sendTTS("Iniciando League of Legends. Diviértete.");
            }
            else {
                fwRsrc->sendTTS("Ya se inicio League of Legends.");
            }
        }
    ));

    lolMod->addEventToken(modRsrc->addEventListener(
        "lol_queue_start",
        [modRsrc, fwRsrc, lolMod](Intent* _) ->void {
            if (!lolMod->isAlive()) {
                try {
                    lolMod->startProcess(fwRsrc);
                }
                catch (...) {
                    cout << "No se recivio prueba de vida de LOL"<<endl;
                }
            }
            modRsrc->sendToMod("lol", "lol_queue_start");
            fwRsrc->sendTTS("Empezando cola. Mucho exito");
        }
    ));

    lolMod->addEventToken(modRsrc->addEventListener(
        "lol_queue_quit",
        [fwRsrc, modRsrc, lolMod](Intent* _) ->void {
            if (!lolMod->isAlive())
                lolMod->startProcess(fwRsrc);
            modRsrc->sendToMod("lol", "lol_queue_quit");
            fwRsrc->sendTTS("Cancelando cola");
        }
    ));

}

ModuleManager::~ModuleManager()
{
	for (Module* mod : *modules) {
		delete mod;
	}
	delete modules;
}
