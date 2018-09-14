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
	poly.color = 255;
	poly.num_verts = 3;
	poly.x0 = 100;
	poly.y0 = 100;
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
	DDRAW_INIT_STRUCT(ddsd);
	lpddsback->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);

	Draw_Filled_Polygon2D(&default_clipRect, &poly, (UCHAR*)ddsd.lpSurface, ddsd.lPitch);

	Rotate_Polygon2d_Mat(&poly, 1);

	lpddsback->Unlock(NULL);

	lpddsprimary->Flip(NULL, DDFLIP_WAIT);
	return 0;
}

int GameEntry::Game_Shutdown()
{
	return 0;
}
