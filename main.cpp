#include <iostream>
#include <Windows.h>

#define _CRT_SECURE_NO_WARNINGS

bool Inject(char* dll, LPCWSTR window)
{
    DWORD ProcessID;

    char filepath[MAX_PATH];
    GetFullPathNameA(dll, MAX_PATH, filepath, NULL);

    GetWindowThreadProcessId(FindWindow(0, window), &ProcessID);

    HANDLE Handle = OpenProcess(PROCESS_ALL_ACCESS, false, ProcessID);

    if (!Handle)
    {
        std::cout << "Couldn't get handle. \n";
        return false;
    }

    LPVOID LLib = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");
    LPVOID AllocatedSpace = VirtualAllocEx(Handle, 0, strlen(filepath) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    WriteProcessMemory(Handle, AllocatedSpace, filepath, strlen(filepath) + 1, 0);
    CreateRemoteThread(Handle, 0, 0, (LPTHREAD_START_ROUTINE)LLib, AllocatedSpace, 0, 0);
    CloseHandle(Handle);
    VirtualFreeEx(Handle, (LPVOID)AllocatedSpace, 0, MEM_RELEASE);

    return true;
}

int main()
{
    char temp[] = "example.dll";

    char* dll = new char[strlen(temp) + 1];
    strcpy(dll, temp);

    LPCWSTR window = L"example.exe";

    if(Inject(dll, window))
        std::cout << "Injected dll. \n";

    Sleep(1000);
}