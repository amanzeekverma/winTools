// ZOpenProcess.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#include <iostream>
#include <conio.h>

int main(int argc, char* argv[]) {
    printf("HI %s\n", argv[1]);
    if (argc < 1) {
        printf("Need one target PID argument");
        return 1;
    }
    DWORD pid = static_cast<DWORD>(strtoul(argv[1], nullptr, 10));

    // Open the process
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (processHandle == NULL) {
        std::cerr << "Failed to open process!" +GetLastError()<< std::endl;
        return 1;
    }

    // Use the process handle...
    // ...
    _getch();
    LPVOID loadLibraryAddr = (LPVOID)GetProcAddress(GetModuleHandle(LPCWSTR("kernel32.dll")), "LoadLibraryA");
    LPVOID paramAddr = (LPVOID)VirtualAllocEx(processHandle, NULL, strlen("user32.dll") + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    WriteProcessMemory(processHandle, paramAddr, "user32.dll", strlen("user32.dll") + 1, NULL);

    HANDLE hRemoteThread = CreateRemoteThread(processHandle, NULL, NULL, (LPTHREAD_START_ROUTINE)loadLibraryAddr, paramAddr, NULL, NULL);

    WaitForSingleObject(hRemoteThread, INFINITE);

    VirtualFreeEx(processHandle, paramAddr, 0, MEM_RELEASE);

    CloseHandle(hRemoteThread);
    CloseHandle(processHandle);

    return 0;
}