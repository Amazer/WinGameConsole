#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <stdio.h>
#include <ddraw.h>
#include <math.h>

#include "cyclibdraw.h"
#include "GameEntry.h"

POLYGON2D poly;

int GameEntry::Game_Init()
{
	poly.state = 1;
	poly.color = (int)RGBColor(255, 150, 0, 0);
	poly.num_verts = 3;
	poly.x0 = 10;
	poly.y0 = 200;
	poly.vlist = new VERTEX2DF[3];
	poly.vlist[0].x = 0;
	poly.vlist[0].y = 0;
	poly.vlist[1].x = 100;
	poly.vlist[1].y = 0;

	poly.vlist[2].x = 100;
	poly.vlist[2].y = 100;
	return 0;
}

int GameEntry::Game_Main()
{
	DDraw_Fill_Surface(lpddsback, 0);

	DDraw_Lock_BackSurface();

	Draw_Polygon2D(&default_clipRect, &poly, back_buffer, back_lpitch);

	DDraw_Unlock_BackSurface();

	DDraw_Flip();
	return 0;
}

int GameEntry::Game_Shutdown()
{
	return 0;
}
