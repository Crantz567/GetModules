#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>

const char* PROCESS_NAME = "chrome.exe";

bool getModules(DWORD PID);
DWORD getPID(const char* processName);

int main()
{
    DWORD pID = getPID(PROCESS_NAME);

    if (pID == 0)
    {
        std::cout << "Failed to find process\n";
        std::getchar();
        return 1;
    }

    if (!getModules(pID))
    {
        std::cout << "Failed to print modules\n";
        std::getchar();
        return 1;
    }

    return 0;
}


bool getModules(DWORD PID)
{
    uintptr_t moduleBaseAddress = 0;
    HANDLE snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);

    if (snapShot == INVALID_HANDLE_VALUE)
    {
        std::cout << GetLastError() << std::endl;
        return false;
    }

    MODULEENTRY32 mInfo;
    mInfo.dwSize = sizeof(MODULEENTRY32);

    if (!Module32First(snapShot, &mInfo))
    {
        std::cout << GetLastError() << std::endl;
        CloseHandle(snapShot);
        return false;
    }

    do
    {
        std::cout << "Name: " << mInfo.szModule << " BaseAddress: " << static_cast<void*>(mInfo.modBaseAddr) << " EndAddress: " << static_cast<void*>(mInfo.modBaseAddr + mInfo.modBaseSize) << std::endl;
    } while (Module32Next(snapShot, &mInfo));

    CloseHandle(snapShot);
    return true;

}

DWORD getPID(const char* processName)
{
    DWORD pID = 0;
    HANDLE snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (snapShot == INVALID_HANDLE_VALUE)
    {
        std::cout << GetLastError() << std::endl;
        return 0;
    }

    PROCESSENTRY32 pInfo;
    pInfo.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(snapShot, &pInfo))
    {
        std::cout << GetLastError() << std::endl;
        CloseHandle(snapShot);
        return 0;
    }
    
    do
    {
        if (strcmp(processName, pInfo.szExeFile) == 0)
        {
            CloseHandle(snapShot);
            pID = pInfo.th32ProcessID;
            return pID;
        }

    } while (Process32Next(snapShot, &pInfo));

    CloseHandle(snapShot);
    return 0;
}

