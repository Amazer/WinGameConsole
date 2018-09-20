#include <stdio.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN	1
#endif
#include <windows.h>
#include "Debug.h"


//void Debug::OpenLogFile()
//{
//}
//void Debug::CloseLogFile()
//{
//}
//void Debug::Log(const char* text)
//{
//}

int Debug::VDebugPrintF(const char* format, va_list argList)
{
	const int MAX_CHARS = 1023;
	static char s_buffer[MAX_CHARS + 1];

	int charsWritten
		= vsnprintf(s_buffer, MAX_CHARS, format, argList);

	s_buffer[MAX_CHARS] = '\0';
	OutputDebugString(s_buffer);
	return charsWritten;
}
int Debug::DebugPrintF(const char* format, ...)
{
	va_list argList;
	va_start(argList, format);
	int charsWritten = VDebugPrintF(format, argList);
	va_end(argList);
	return charsWritten;

}