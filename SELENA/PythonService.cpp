#include "PythonService.hpp"

using namespace std;

BOOL CALLBACK SendWMCloseMsg(HWND hwnd, LPARAM lParam)
{
    DWORD dwProcessId = 0;
    GetWindowThreadProcessId(hwnd, &dwProcessId);
    if (dwProcessId == lParam)
        SendMessageTimeout(hwnd, WM_CLOSE, 0, 0, SMTO_ABORTIFHUNG, 30000, NULL);
    return TRUE;
}


shared_ptr<void>  pys::startProcess(Event* event, string pythonFile, bool show) {

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    string command;
    string source;

   

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!show) {
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;
        source = "C:\\Windows\\system32\\cmd.exe";
        command = "py C:\\Users\\world\\source\\repos\\SELENA\\SELENA\\Python\\Scripts\\" + pythonFile;

        source = "C:\\Users\\world\\AppData\\Local\\Programs\\Python\\Python310\\python.exe";
        command = "C:\\Users\\world\\AppData\\Local\\Programs\\Python\\Python310\\python.exe C:\\Users\\world\\source\\repos\\SELENA\\SELENA\\Python\\Scripts\\" + pythonFile;
    }
    else {
        source = "C:\\Users\\world\\AppData\\Local\\Programs\\Python\\Python310\\python.exe";
        command = "C:\\Users\\world\\AppData\\Local\\Programs\\Python\\Python310\\python.exe C:\\Users\\world\\source\\repos\\SELENA\\SELENA\\Python\\Scripts\\" + pythonFile;
    }

    cout << "Starting python file with | cmd.exe : " << source << " | Script: " << command << endl;

    // Start the child process. 
    if (!CreateProcessA(
        source.c_str(),   // No module name (use command line)
        &command.front(),        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        DETACHED_PROCESS,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi)           // Pointer to PROCESS_INFORMATION structure
        )
    {
        printf("CreateProcess failed (%d).\n", GetLastError());
        return nullptr;
    }

    shared_ptr<void> listenerHandle = event->add("stop", [pythonFile, pi]() -> void
        {
            CloseHandle(pi.hppThread);
            WaitForInputIdle(pi.hppProcess, 1000);
            if (WaitForSingleObject(pi.hppProcess, 1000) == WAIT_TIMEOUT)
            {
                EnumWindows(&SendWMCloseMsg, pi.dwProcessId);
                if (WaitForSingleObject(pi.hppProcess, 1000) == WAIT_TIMEOUT)
                {
                    // application did not close in a timely manner, do something...

                    // in this example, just kill it.  In a real world
                    // app, you should ask the user what to do...
                    TerminateProcess(pi.hppProcess, 0);
                }
            }
            // Close process and thread handles. 
        });

    return listenerHandle;

}
