#pragma once
#include <Windows.h>

const UINT WINDOW_WIDTH = 1280;
const UINT WINDOW_HEIGHT = 720;
extern HWND g_hWnd;
void StartApp(const TCHAR* appName); // これを呼んだらアプリが実行するようにする

