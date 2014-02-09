#pragma once
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#pragma comment(lib, "advapi32.lib")

class WindowsService
{
public :
	static VOID WINAPI ServiceMain (DWORD argc, LPTSTR *argv);
	static VOID WINAPI ServiceCtrlHandler (DWORD);
	static DWORD WINAPI ServiceWorkerThread (LPVOID lpParam);
private:
	WindowsService();
	~WindowsService();
	WindowsService(const WindowsService&){}
	WindowsService operator=(const WindowsService&){}
};