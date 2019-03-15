#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <stdio.h>
#include <ddraw.h>
#include <math.h>


#include "cyclibdraw.h"
#include "GameEntry.h"
#include "Debug.h"

#include "baseEntry.h"
#include "drawlibEntry.h"
#include "BobEntry.h"



baseEntry * testEntry;

int GameEntry::Game_Init()
{
	testEntry = new drawlibEntry();
//	testEntry = new BobEntry();

	testEntry->Init();


	return 0;
}

int GameEntry::Game_Main()
{

	testEntry->Main();
	return 0;
}

int GameEntry::Game_Shutdown()
{
	testEntry->Exit();
	return 0;
}
