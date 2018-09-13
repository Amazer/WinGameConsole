#include "CTime.h"
float CTime::deltaTime = 0;
float CTime::lastFrameTimePoint = 0;
float CTime::timeSinceStartUp = 0;

DWORD CTime::start_clock_count = 0;

DWORD CTime::Get_Clock(void)
{
	return GetTickCount();
}

DWORD CTime::Start_Clock(void)
{
	return (start_clock_count=Get_Clock());
}

DWORD CTime::Wait_Clock(DWORD count)
{
	while (Get_Clock() - start_clock_count < count);
	return Get_Clock();
}
