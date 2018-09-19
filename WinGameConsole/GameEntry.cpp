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
	poly.color = (int)RGBColor(255, 0, 0, 0);
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
	DDRAW_INIT_STRUCT(ddsd);
	lpddsback->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);

	poly.color = (int)RGBColor(255, 0, 0, 0);
	poly.x0 = 10;
	Draw_Filled_Polygon2D(&default_clipRect, &poly, (UCHAR*)ddsd.lpSurface, ddsd.lPitch);

	poly.color = (int)RGBColor(0, 255, 0, 0);
	poly.x0 = 120;
	Draw_Filled_Polygon2D(&default_clipRect, &poly, (UCHAR*)ddsd.lpSurface, ddsd.lPitch);

	poly.color = (int)RGBColor(0,0 , 255, 0);
	poly.x0 = 230;
	Draw_Filled_Polygon2D(&default_clipRect, &poly, (UCHAR*)ddsd.lpSurface, ddsd.lPitch);

	Rotate_Polygon2d_Mat(&poly, 1);

//	Draw_Line32(100, 100, 400, 400, __RGB32BIT(0, 0, 255, 200), (UINT*)ddsd.lpSurface, ddsd.lPitch);
//	Draw_QuadFP_2D(&default_clipRect, 100, 100, 200, 100, 200, 200, 100, 200, 255, (UCHAR*)ddsd.lpSurface, ddsd.lPitch);


//	Draw_TriangleFP_2D(&default_clipRect, 100, 100, 200, 150, 200, 200, (int)RGBColor(255, 0, 255,0), (UCHAR*)ddsd.lpSurface, ddsd.lPitch);
	lpddsback->Unlock(NULL);

	DDraw_Flip();
	return 0;
}

int GameEntry::Game_Shutdown()
{
	return 0;
}
