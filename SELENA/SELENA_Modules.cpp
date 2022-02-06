#include "SELENA_Modules.hpp"
#include "ModuleMsgService.hpp"


ModulesResources* SELENA_Mods::init() {


    cout << "||                                                ||" << endl;
    ModulesResources* modRsrc = new ModulesResources(5);
    cout << "||  Booting Modules Service...                    ||" << endl;

    modRsrc->addThread(new thread(mms::moduleSenderService, "50004", "50005", modRsrc));
    cout << "||  [OK] Module thread started!                   ||" << endl;

    while (!modRsrc->isReady()) {};
    cout << "||  Modules Services Booted correctly!            ||" << endl;

    return modRsrc;

}