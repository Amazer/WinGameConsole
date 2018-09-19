#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <stdio.h>
#include <ddraw.h>
#include <math.h>

#include "cyclibdraw.h"
#include "GameEntry.h"

#define WINCLASSNAME "WINCLASS1"
#define WNDNAME "EngineEntry"

// window创建相关
#define SCREEN_WIDTH 640	//640		// 屏幕宽度   窗口模式,不设置display mode,使用系统的色彩位深
#define SCREEN_HEIGHT 480	//480 		// 屏幕高度   

#define FULL_SCREEN_MODE 1		// 是否为全屏模式

#define SCREEN_BPP 32			// 色彩位深  窗口模式不起作用

HWND main_window_handle = NULL;
HINSTANCE main_instance = NULL;

// 临时数据
MSG msg;
DWORD start_time;
bool mainRuned = false;

// 函数声明
LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

int Game_Main(void *params = NULL, int num_parms = 0);
int Game_Init(void *params = NULL, int num_parms = 0);
int Game_Exit(void *params = NULL, int num_parms = 0);

void On_GameInit();
void On_GameMain();
void On_GameExit();

LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	switch (msg)
	{
	case WM_KEYDOWN:
	{
		int virtual_code = (int)wparam;
		if (virtual_code == VK_ESCAPE)
		{
			PostQuitMessage(0);
			return 0;
		}
	}
	break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	break;
	case WM_PAINT:
	{
		hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		return 0;
	}
	break;
	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	WNDCLASSEX wclass;
	wclass.cbSize = sizeof(WNDCLASSEX);
	wclass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC;
	wclass.lpfnWndProc = WinProc;
	wclass.cbClsExtra = 0;
	wclass.cbWndExtra = 0;
	wclass.hInstance = hInstance;
	wclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wclass.lpszMenuName = NULL;
	wclass.lpszClassName = WINCLASSNAME;
	wclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);


	RegisterClassEx(&wclass);


	HWND hwnd;
	if (!(hwnd = CreateWindowEx(
		NULL,
		WINCLASSNAME,
		WNDNAME,
		(FULL_SCREEN_MODE ? (WS_POPUP | WS_VISIBLE) : (WS_OVERLAPPEDWINDOW | WS_VISIBLE)),
		0, 0,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL)))
	{
		//		sprintf("error !! create window failed");
		MessageBox(NULL, "Create window Error!", "Error", MB_OK);
		return 0;
	}
	main_window_handle = hwnd;
	if (!FULL_SCREEN_MODE)
	{
		RECT window_rect = { 0,0,SCREEN_WIDTH - 1,SCREEN_HEIGHT - 1 };
		AdjustWindowRectEx(&window_rect,
			GetWindowStyle(main_window_handle),
			GetMenu(main_window_handle) != NULL,
			GetWindowExStyle(main_window_handle));
		win_client_x0 = -window_rect.left;
		win_client_y0 = -window_rect.top;
		MoveWindow(main_window_handle, 0, 0, window_rect.right - window_rect.left, window_rect.bottom - window_rect.top, FALSE);

		ShowWindow(main_window_handle, SW_SHOW);
	}

	Game_Init();

	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (!mainRuned)
		{
			mainRuned = true;
			start_time = GetTickCount();
			Game_Main();
		}
		else
		{
			Sleep(1);
			DWORD delta = GetTickCount() - start_time;
			if (delta >= MS_PER_FRAME)
			{
				mainRuned = false;
			}
		}
	}
	Game_Exit();
	return msg.wParam;
}


int Game_Init(void *params, int num_parms)
{
	DDraw_Init(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, !FULL_SCREEN_MODE);

	On_GameInit();

	if (FULL_SCREEN_MODE)
	{
		// 不显示鼠标
		ShowCursor(false);
	}
	return 1;
}

int Game_Exit(void *params, int num_parms)
{
	On_GameExit();
	DDraw_ShutDown();
	return 1;
}

int Game_Main(void *params, int num_parms)
{
	On_GameMain();

	// 等待垂直空白结束
	DDraw_Wait_For_Vsync();

	if (KEYDOWN(VK_ESCAPE))
	{
		PostMessage(main_window_handle, WM_DESTROY, 0, 0);
		return 0;
	}

	return 1;
}

void On_GameInit()
{
	Init_LookTable();
	GameEntry::Game_Init();
}
void On_GameMain()
{
	DDraw_CheckWinClient();
	GameEntry::Game_Main();

}
void On_GameExit()
{
	GameEntry::Game_Shutdown();
}


