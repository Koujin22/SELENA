#pragma once
#include <Windows.h>
#include <functional>
#include "FrameworkResources.hpp"


inline BOOL CALLBACK SendWMCloseMsg(HWND hwnd, LPARAM lParam)
{
    DWORD dwProcessId = 0;
    GetWindowThreadProcessId(hwnd, &dwProcessId);
    if (dwProcessId == lParam)
        SendMessageTimeout(hwnd, WM_CLOSE, 0, 0, SMTO_ABORTIFHUNG, 30000, NULL);
    return TRUE;
}

namespace pys {

    function<bool(void)>* startProcess(string pythonFile, bool show, FrameworkResources* fwRsrc);
    function<bool(void)>* startProcess(string pythonFile, bool show, FrameworkResources* fwRsrc, token& token);
    
}
