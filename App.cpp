#include <windowsx.h>
#include <Windows.h>
#include "App.h"
#include "Engine.h"
#include "Scene.h"
#include <iostream>
#include <tchar.h>

using namespace std;

HINSTANCE g_hInst;
HWND g_hWnd = NULL;

float normalizedX;
float normalizedY;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_DESTROY: {
		PostQuitMessage(0);
		break;
	}

	case WM_MOUSEMOVE: {
			int mouseX = GET_X_LPARAM(lp); // マウスのX座標
			int mouseY = GET_Y_LPARAM(lp); // マウスのY座標

			// マウス座標を(-1.0 ～ 1.0) に変換
			normalizedX = (static_cast<float>(mouseX) / WINDOW_WIDTH) * 2.0f - 1.0f;
			normalizedY = 1.0f - (static_cast<float>(mouseY) / WINDOW_HEIGHT) * 2.0f;

			normalizedY = max(-1.0f, min(1.0f, normalizedY));
			normalizedX = max(-1.0f, min(1.0f, normalizedX));
			break;
		}

	default:
		break;

	}

	return DefWindowProc(hWnd, msg, wp, lp);
}

void InitWindow(const TCHAR* appName)
{
	g_hInst = GetModuleHandle(nullptr);
	if (g_hInst == nullptr)
	{
		return;
	}

	// ウィンドウの設定
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hIcon = LoadIcon(g_hInst, IDI_APPLICATION);
	wc.hCursor = LoadCursor(g_hInst, IDC_ARROW);
	wc.hbrBackground = GetSysColorBrush(COLOR_BACKGROUND);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = appName;
	wc.hIconSm = LoadIcon(g_hInst, IDI_APPLICATION);

	// ウィンドウクラスの登録。
	RegisterClassEx(&wc);

	// ウィンドウサイズの設定
	RECT rect = {};
	rect.right = static_cast<LONG>(WINDOW_WIDTH);
	rect.bottom = static_cast<LONG>(WINDOW_HEIGHT);

	// ウィンドウサイズを調整
	auto style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
	AdjustWindowRect(&rect, style, FALSE);

	// ウィンドウの生成
	g_hWnd = CreateWindowEx(
		0,
		appName,
		appName,
		style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		nullptr,
		nullptr,
		g_hInst,
		nullptr
	);

	// ウィンドウを表示
	ShowWindow(g_hWnd, SW_SHOWNORMAL);

	// ウィンドウにフォーカスする
	SetFocus(g_hWnd);
}

void MainLoop()
{
	MSG msg = {};
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE == TRUE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			g_Scene->Update(normalizedX, normalizedY); // シーンのアップデート
			g_Engine->BeginRender(); // 描画開始
			g_Scene->Draw(); // シーン描画
			g_Engine->EndRender(); // 描画終了
		}
	}
}

void StartApp(const TCHAR* appName)
{
	// ウィンドウ生成
	InitWindow(appName);

	// 描画エンジンの初期化を行う
	g_Engine = new Engine();
	if (!g_Engine->Init(g_hWnd, WINDOW_WIDTH, WINDOW_HEIGHT))
	{
		return;
	}

	// シーン初期化
	g_Scene = new Scene();
	if (!g_Scene->Init())
	{
		return;
	}

	// メイン処理ループ
	MainLoop();
}

