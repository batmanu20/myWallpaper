// cloudWallpaper.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>

HWND workerw;

BOOL CALLBACK EnumWindowsFindDesktopWindow(HWND hwnd,LPARAM lParam) {
	//if this hwnd has the child window "SHELLDLL_DefView",then set workerw as the next window.
	HWND p = FindWindowEx(hwnd, NULL, "SHELLDLL_DefView", NULL);
	if (p != NULL) {
		workerw = FindWindowEx(NULL, hwnd, "WorkerW", NULL);
	}
	return true;
}

int main(){
	DWORD_PTR result = 0;
	
	//find th progman window, then send 0x052c message,which 
	//create a WorkerW behind icons(if already exist,do nothing)
	HWND hwnd_Progman = FindWindow("Progman", NULL);
	if (hwnd_Progman == NULL) {
		std::cout << "cannot get handle of progman" << std::endl;
	}
	else{
		std::cout << "handle of progman get" << std::endl;
	}
	SendMessageTimeout(hwnd_Progman, 0x052c, NULL, NULL, SMTO_NORMAL, 1000, &result);

	EnumWindows(EnumWindowsFindDesktopWindow, NULL);
	//enum all the windows to find workerw.
	//std::cout << workerw << std::endl;

}

