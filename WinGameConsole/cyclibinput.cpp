#define WIN32_LEAN_AND_MEAN  
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <iostream>
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <io.h>
#include <fcntl.h>
#include <sys/timeb.h>
#include <time.h>

#include <ddraw.h>

#include <objbase.h>
#include <dinput.h>

#include "cyclibinput.h"
//#include "cyclibdraw.h"


HWND _main_window_handle = NULL;
HINSTANCE _main_instance = NULL;

// 没有赋初值，导致编译报错，什么IDirectInput8A 啥啥的报错
LPDIRECTINPUT8 lpdi = NULL;

LPDIRECTINPUTDEVICE8 lpdikey = NULL;
LPDIRECTINPUTDEVICE8 lpdimouse = NULL;
LPDIRECTINPUTDEVICE8 lpdijoy = NULL;

GUID joystickGUID;
char joyname[80];

UCHAR keybord_state[256];
DIMOUSESTATE mouse_state;
DIJOYSTATE joy_state;
int joystick_found = 0;


int DInput_Init(HINSTANCE mainInstance, HWND mainWindowHandle)
{
	_main_instance = mainInstance;
	_main_window_handle = mainWindowHandle;

	if (FAILED(DirectInput8Create(_main_instance, DIRECTINPUT_VERSION,
		IID_IDirectInput8, (void **)&lpdi, NULL)))
	{
		return 0;
	}
	return 1;
}

void DInput_Shutdown()
{
	_main_instance = NULL;
	_main_window_handle = NULL;
	if (lpdi)
	{
		lpdi->Release();
		lpdi = NULL;
	}
}

int DInput_Init_Keyboard()
{
	if (FAILED(lpdi->CreateDevice(GUID_SysKeyboard, &lpdikey, NULL)))
	{
		return 0;
	}
	if (lpdikey->SetCooperativeLevel(_main_window_handle,
		DISCL_NONEXCLUSIVE | DISCL_BACKGROUND) != DI_OK)
	{
		return 0;
	}

	if (lpdikey->SetDataFormat(&c_dfDIKeyboard) != DI_OK)
	{
		return 0;
	}
	if (lpdikey->Acquire() != DI_OK)
	{
		return 0;
	}
	return 1;
}

int DInput_Release_Keyboard()
{
	if (lpdikey)
	{
		lpdikey->Unacquire();
		lpdikey->Release();
	}
	return 1;

}

int DInput_Read_Keyboard()
{
	if (lpdikey->GetDeviceState(256, (LPVOID)keybord_state) != DI_OK)
	{
		return 0;
	}
	else
	{
		memset(keybord_state, 0, sizeof(keybord_state));
		return 0;
	}
	return 1;
}

int DInput_Init_Mouse()
{
	if (lpdi->CreateDevice(GUID_SysMouse, &lpdimouse, NULL) != DI_OK)
	{
		return 0;
	}
	if (lpdimouse->SetCooperativeLevel(_main_window_handle,
		DISCL_NONEXCLUSIVE | DISCL_BACKGROUND) != DI_OK)
	{
		return 0;
	}

	if (lpdimouse->SetDataFormat(&c_dfDIMouse) != DI_OK)
	{
		return 0;
	}
	if (lpdimouse->Acquire() != DI_OK)
	{
		return 0;
	}
	return 1;
}

int DInput_Read_Mouse()
{
	if (lpdimouse)
	{
		if(lpdimouse->GetDeviceState(sizeof(DIMOUSESTATE)
			,(LPVOID)&mouse_state)!=DI_OK)
		{
			return 0;
		}
		return 1;
	}
	else
	{
		memset(&mouse_state, 0, sizeof(mouse_state));
		return 0;
	}
	return 1;
}
int DInput_Release_Mouse()
{
	if (lpdimouse)
	{
		lpdimouse->Unacquire();
		lpdimouse->Release();
	}
}


int DInput_Init_Joystick(int min_x /* = -256 */, int max_x /* = 256 */, int min_y /* = -256 */, int max_y /* = 256 */)
{

}
int DInput_Read_Jyostick()
{

}
int DInput_Release_Joystick()
{

}

