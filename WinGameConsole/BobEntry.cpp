
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <stdio.h>
#include <ddraw.h>
#include <math.h>


#include "cyclibdraw.h"
#include "cyclibbob.h"
#include "Debug.h"

#include "BobEntry.h"

BITMAP_FILE bob_bitmap;
BITMAP_IMAGE bob_image;
BOB bob;
int frameCounter = 0;
const int num_bob = 10;


int BobEntry::Init()
{
	Load_Bitmap_File(&bob_bitmap, "anim_bmp.bmp");
	Create_BOB(&bob, 0, 120, 94, 99, 16, BOB_ATTR_MULTI_ANIM | BOB_ATTR_VISIBLE, DDSCAPS_VIDEOMEMORY);
	Set_Anim_Speed_BOB(&bob, 2);
	Set_Vel_BOB(&bob, 0, 0);
//	Set_Vel_BOB(&bob, 2 + rand() % 6, 0);
	Set_Pos_BOB(&bob,  screen_width/2, screen_height/2);
	int frame_index = 0;
	for (int i = 0; i < 4; ++i)
	{
		for (int k = 0; k < 4; ++k)
		{
			Load_Frame_BOB24(&bob, &bob_bitmap, frame_index, k, i, BITMAP_EXTRACT_MODE_CELL);
			frame_index++;
		}
	}
	bob.varsI[0] = 94;
	bob.varsI[1] = 99;

	int anim_1[]=  {0, 1, 2, 3};
	Load_Animation_BOB(&bob, 0, 4, anim_1);

	int anim_2[]=  {4, 5, 6, 7};
	Load_Animation_BOB(&bob, 1, 4, anim_2);

	int anim_3[]=  {8, 9, 10, 11};
	Load_Animation_BOB(&bob, 2, 4, anim_3);

	int anim_4[]=  {12,13,14,15};
	Load_Animation_BOB(&bob, 3, 4, anim_4);

	Set_Animation_BOB(&bob, 0);

	Unload_Bitmap_Flie(&bob_bitmap);

	return 0;

}
int BobEntry::Main()
{
	DDraw_Fill_Surface(lpddsback, 0);
	//	DDraw_Lock_BackSurface();
	//	DDraw_Unlock_BackSurface();
	//    Draw_Scaled_BOB(&bob, bob.varsI[0], bob.varsI[1],lpddsback);
	Draw_BOB(&bob, lpddsback);
	DDraw_Flip();
	frameCounter++;
	if (frameCounter >= 60)
	{
		frameCounter = 0;
		int anim_index = bob.anim_index;
		anim_index = ++anim_index % 4;
		Set_Animation_BOB(&bob, anim_index);
	}
	Move_BOB(&bob);
	Animate_BOB(&bob);
	if (bob.x > screen_width)
	{
		bob.x = -bob.width;
	}
	return 0;

}
int BobEntry::Exit()
{
	Destroy_BOB(&bob);
	return 0;
}
