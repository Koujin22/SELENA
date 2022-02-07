#include "PythonService.hpp"

function<bool(void)>* pys::startProcess(string pythonFile, bool show, FrameworkResources* fwRsrc)
{

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
        
        source = "C:\\Users\\world\\AppData\\Local\\Programs\\Python\\Python310\\python.exe";
        command = "C:\\Users\\world\\AppData\\Local\\Programs\\Python\\Python310\\python.exe .\\Python\\Scripts\\" + pythonFile;
    }
    else {
        source = "C:\\Windows\\system32\\cmd.exe";
        command = "C:\\Windows\\system32\\cmd.exe /k py .\\Python\\Scripts\\" + pythonFile;
    }
    //cout << "Starting python file with | exe : " << source << " | Script: " << command << endl;

    // Start the child process. 
    if (!CreateProcessA(
        source.c_str(),   // No module name (use command line)
        &command.front(),        // Command line
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
        return nullptr;
    }

    auto stopFn = [pythonFile, pi]() -> void
    {
        CloseHandle(pi.hThread);
        WaitForInputIdle(pi.hProcess, 500);
        if (WaitForSingleObject(pi.hProcess, 500) == WAIT_TIMEOUT)
        {
            EnumWindows(&SendWMCloseMsg, pi.dwProcessId);
            if (WaitForSingleObject(pi.hProcess, 500) == WAIT_TIMEOUT)
            {
                // application did not close in a timely manner, do something...

                // in this example, just kill it.  In a real world
                // app, you should ask the user what to do...
                TerminateProcess(pi.hThread, 0);
            }
        }
        // Close process and thread handles. 
    };
     
    fwRsrc->addEventListenerPersist("stop", stopFn);

    return new function<bool()>([pi]()->bool {
        switch (WaitForSingleObject(pi.hProcess, 0))
        {
        case WAIT_OBJECT_0:
            return false;
            break;
        case WAIT_TIMEOUT:
            return true;
            break;
        }
        });
}

function<bool(void)>* pys::startProcess(string pythonFile, bool show, FrameworkResources* fwRsrc, token& token)
{

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

        source = "C:\\Users\\world\\AppData\\Local\\Programs\\Python\\Python310\\python.exe";
        command = "C:\\Users\\world\\AppData\\Local\\Programs\\Python\\Python310\\python.exe .\\Python\\Scripts\\" + pythonFile;
    }
    else {
        source = "C:\\Windows\\system32\\cmd.exe";
        command = "C:\\Windows\\system32\\cmd.exe /k py .\\Python\\Scripts\\" + pythonFile;
    }
    //cout << "Starting python file with | exe : " << source << " | Script: " << command << endl;

    // Start the child process. 
    if (!CreateProcessA(
        source.c_str(),   // No module name (use command line)
        &command.front(),        // Command line
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
        return nullptr;
    }

    auto stopFn = [pythonFile, pi]() -> void
    {
        CloseHandle(pi.hThread);
        WaitForInputIdle(pi.hProcess, 500);
        if (WaitForSingleObject(pi.hProcess, 500) == WAIT_TIMEOUT)
        {
            EnumWindows(&SendWMCloseMsg, pi.dwProcessId);
            if (WaitForSingleObject(pi.hProcess, 500) == WAIT_TIMEOUT)
            {
                // application did not close in a timely manner, do something...

                // in this example, just kill it.  In a real world
                // app, you should ask the user what to do...
                TerminateProcess(pi.hThread, 0);
            }
        }
        // Close process and thread handles. 
    };
    
    token = fwRsrc->addEventListener("stop", stopFn);

    return new function<bool()>([pi]()->bool {
        switch (WaitForSingleObject(pi.hProcess, 0))
        {
        case WAIT_OBJECT_0:
            return false;
            break;
        case WAIT_TIMEOUT:
            return true;
            break;
        }
        });
}
