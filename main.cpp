#include "App.h"

int main(int argc, wchar_t** argv, wchar_t** envp)
{
	StartApp(TEXT("BrickBreaker"));
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	StartApp(TEXT("BrickBreaker"));
	return 0;
}