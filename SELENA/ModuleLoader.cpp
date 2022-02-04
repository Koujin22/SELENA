#include "ModuleLoader.hpp"
#include "HandlesMods.hpp"
#include "Intent.hpp"
#include "MessageService.hpp"
#include "PythonService.hpp"
using namespace std;

void mdl::eventListeners(HandlesMods* handle, Handles* handle_fw) {
    cout << "Setting module events" << endl;
	handle->addPtrListener(handle->add("lol_start", [handle, handle_fw](Intent* _) -> void {
        cout << "reciby lol_start" << endl;
        handle_fw->sendTTS("Iniciando League of Legends. Diviértete.");
        pys::startProcess(handle_fw->getEventPtr(), "lol.py 50004 -d=true -s=lol", true);
		}));

    handle->addPtrListener(handle->add("lol_queue_start", [handle, handle_fw](Intent* _) ->void {
        handle->sendMod("lol", "lol_queue_start");
        handle_fw->sendTTS("Empezando cola. Mucho exito");
        }));
}


void  mdl::startProcess(string pathToExe, string arguments) {

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;



    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    ZeroMemory(&pi, sizeof(pi));

    // Start the child process. 
    if (!CreateProcessA(
        &pathToExe.front(),   // No module name (use command line)
        &arguments.front(),        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        CREATE_NEW_CONSOLE,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi)           // Pointer to PROCESS_INFORMATION structure
        )
    {
        printf("CreateProcess failed (%d).\n", GetLastError());
    }

    CloseHandle(pi.hppThread);
    CloseHandle(pi.hppProcess);

}