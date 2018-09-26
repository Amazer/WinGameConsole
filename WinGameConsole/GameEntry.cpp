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


POLYGON2D poly;
BITMAP_FILE bitmap;
BITMAP_IMAGE image;

int GameEntry::Game_Init()
{
	Debug::DebugPrintF("output window === Log ===========\n");
	Write_Error("Game_Init:call by GameEntry\n");
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

	Load_Bitmap_File(&bitmap, "bitmap24.bmp");
	Create_Bitmap(&image, 0, 0, 400, 400, 24);
	Load_Image_Bitmap24(&image, &bitmap, 0, 0, BITMAP_EXTRACT_MODE_ABS);

	return 0;
}

int GameEntry::Game_Main()
{
	DDraw_Fill_Surface(lpddsback, (int)RGBColor(255,255,255,255));

	const char* txt = "cyc Test DrawText";
	DDraw_Lock_BackSurface();

	VLine(&default_clipRect, 100, 400, 100, (int)RGBColor(255, 0, 0, 0), back_buffer, back_lpitch);
	VLine(&default_clipRect, 100, 400, 200, (int)RGBColor(0,255,  0, 0), back_buffer, back_lpitch);
	VLine(&default_clipRect, 100, 400, 300, (int)RGBColor(0, 0, 255, 0), back_buffer, back_lpitch);

	HLine(&default_clipRect, 100, 400, 100, (int)RGBColor(255, 0, 0, 0), back_buffer, back_lpitch);
	HLine(&default_clipRect, 100, 400, 200, (int)RGBColor(0, 255, 0, 0), back_buffer, back_lpitch);
	HLine(&default_clipRect, 100, 400, 300, (int)RGBColor(0, 0, 255, 0), back_buffer, back_lpitch);
	
	Draw_Bitmap_Image24(&image, back_buffer, back_lpitch, 0);

	Draw_Text_GDI_IN_DD(txt, 50, 50, RGB(255, 0, 0, 0), lpddsback);

	DDraw_Unlock_BackSurface();


	DDraw_Flip();
//	Scan_Image_Bitmap24(&bitmap, lpddsprimary, 0, 0);
	return 0;
}

int GameEntry::Game_Shutdown()
{
	return 0;
}
