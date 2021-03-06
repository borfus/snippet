#include <stdio.h>
#include <Windows.h>
#include <TlHelp32.h>

DWORD search_process_list_for_pid(HANDLE process_list, wchar_t* process_name) {
    // must initialize dwSize to struct or Process32First will fail
    PROCESSENTRY32 current_process = { .dwSize = sizeof(PROCESSENTRY32) };

    if (Process32First(process_list, &current_process)) {
        while (Process32Next(process_list, &current_process)) {
            if (wcscmp(current_process.szExeFile, process_name) == 0) {
                return current_process.th32ProcessID;
            }
        }
    }
    return 0;
}

DWORD get_pid(wchar_t* process_name) {
    HANDLE process_list = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (process_list != NULL) {
        return search_process_list_for_pid(process_list, process_name);
    }
    return 0;
}

HANDLE open_process(DWORD pid) {
    HANDLE process = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, pid);
    return process;
}

int main() {
    wchar_t process_name[32] = L"notepad.exe";
    DWORD pid = get_pid(process_name);
    if (!pid) {
        printf("Unable to get PID of process \"%ls\"\n", process_name);
        return 1;
    }

    HANDLE process = open_process(pid);
    if (process == INVALID_HANDLE_VALUE) {
        printf("Failed to open PID %d, error code %d\n", pid, GetLastError());
        return 1;
    }

    printf("Process %ls opened with PID: %d\n", process_name, pid);

    return 0;
}
