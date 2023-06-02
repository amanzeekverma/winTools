
#include<Windows.h>
#include<stdio.h>

int main() 
{
    WCHAR name[] = L"notepad";
    PROCESS_INFORMATION pi;
    STARTUPINFO si = { sizeof(si) };
    // above is a shorthand, c++ thingy
    // its actually:
    // memset(&si, 0, sizeof(si));
    // si.cb = sizeof(si) //=== cb => count_buyes, first member of datastructure. which is required.

    //creating a process (checkout all the parameters)
    BOOL success = CreateProcessW(nullptr, name, nullptr, nullptr, FALSE, HIGH_PRIORITY_CLASS, nullptr, nullptr, &si, &pi);

    if (success) {
        printf("%ls created:  PID: %u    TID: %u\n", name, pi.dwProcessId, pi.dwThreadId);
        //Following part is for waiting till the notepad actually ends.
        WaitForSingleObject(pi.hThread, INFINITE);
        DWORD code;
        GetExitCodeProcess(pi.hProcess, &code);
        printf("%ls terminated with %u code\n", name, code);

        //following is not required per se, code exit would anyway close handle.
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        //These handles can be used (to be learnt later, they are powerful).
        //
    }
    else {
        printf("Error creating process: %u\n", GetLastError());
    }
    return 0;
}

