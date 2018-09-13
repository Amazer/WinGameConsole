#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
class CTime
{
public:
	static float deltaTime;
	static float lastFrameTimePoint;
	static float timeSinceStartUp;

	static DWORD start_clock_count;
	static DWORD Get_Clock(void);
	static DWORD Start_Clock(void);
	static DWORD Wait_Clock(DWORD count);
	
};

