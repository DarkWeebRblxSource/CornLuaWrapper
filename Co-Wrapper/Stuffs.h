#pragma once
#include <Windows.h>
#include <WinINet.h>
#include <string>

#pragma comment(lib, "wininet") 

using namespace std;


void get_file(const char* dllName, const char* fileName, char* buffer, int bfSize) {
	GetModuleFileName(GetModuleHandle(dllName), buffer, bfSize);
	if (strlen(fileName) + strlen(buffer) < MAX_PATH) {
		char* pathEnd = strrchr(buffer, '\\');
		strcpy(pathEnd + 1, fileName);
	}
	else {
		*buffer = 0;
	}
}

std::string replaceAll(std::string subject, const std::string& search,
	const std::string& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
	return subject;
}

std::string DownloadURL(const char* URL) {
	HINTERNET interwebs = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
	HINTERNET urlFile;
	std::string rtn;
	if (interwebs) {
		urlFile = InternetOpenUrlA(interwebs, URL, NULL, NULL, NULL, NULL);
		if (urlFile) {
			char buffer[2000];
			DWORD bytesRead;
			do {
				InternetReadFile(urlFile, buffer, 2000, &bytesRead);
				rtn.append(buffer, bytesRead);
				memset(buffer, 0, 2000);
			} while (bytesRead);
			InternetCloseHandle(interwebs);
			InternetCloseHandle(urlFile);
			std::string p = replaceAll(rtn, "|n", "\r\n");
			return p;
		}
	}
	InternetCloseHandle(interwebs);
	std::string p = replaceAll(rtn, "|n", "\r\n");
	return p;
}

void erase_peheader() {
	
	HMODULE hdll;
	char cpath[MAX_PATH];
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, "RyoWrapper.dll", &hdll);
	GetModuleFileName(hdll, cpath, MAX_PATH);

	DWORD old;
	VirtualProtect(hdll, 0x500, PAGE_READWRITE, &old);
	memset(hdll, 0, 0x500);
	VirtualProtect(hdll, 0x500, old, &old);
	
}