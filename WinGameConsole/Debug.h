#pragma once
class Debug
{
public:
//	static void OpenLogFile();
//	static void CloseLogFile();
//	static void Log(const char* text);
	static int VDebugPrintF(const char* format, va_list argList);
	static int DebugPrintF(const char* format, ...);
};

