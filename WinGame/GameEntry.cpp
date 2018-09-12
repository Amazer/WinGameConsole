#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <stdio.h>
#include <ddraw.h>

#include <math.h>

#include "cyclib1.h"
#include "cycmath2dLib.h"
#include "CTime.h"

#define WINCLASSNAME "WINCLASS1"
#define WNDNAME "EngineEntry"

// 自定义的位图文件类型。
typedef struct BITMAP_FILE_TAG
{
	BITMAPFILEHEADER bitmapfileHeader;		// 位图文件信息
	BITMAPINFOHEADER bitmapInfoHeader;		// 位图信息
	PALETTEENTRY palette[256];				// 调色板，如果有
	UCHAR *buffer;							// 位图像素数据
}BITMAP_FILE, *BITMAP_FILE_PTR;

// 定义存储动画
typedef struct ALIEN_OBJ_TYP
{
	LPDIRECTDRAWSURFACE7 frames[3]; // 3 frames of animation for complete walk cycle
	int x, y;                        // position of alien
	int velocity;                   // x-velocity
	int current_frame;              // current frame of animation
	int counter;                    // used to time animation

	int width;
	int height;
	float scale;					// 缩放

} ALIEN_OBJ, *ALIEN_OBJ_PTR;




// 全局变量

HWND main_window_handle = NULL;
int is_window_closed = 0;
LPDIRECTDRAW7 lpdd = NULL;
LPDIRECTDRAWSURFACE7 lpddsprimary = NULL;		// 主显示表面(主表面)
LPDIRECTDRAWSURFACE7 lpddsback = NULL;			// 后备表面(主表面的子表面)
LPDIRECTDRAWCLIPPER lpddclipper = NULL;			// 裁剪器
DDSURFACEDESC2 ddsd;
LPDIRECTDRAWPALETTE lpddpal;
PALETTEENTRY palette[256];
UCHAR *double_buffer = NULL;

DDPIXELFORMAT ddpixelformat;					// 像素格式
RECT client;									// 用户区

LPDIRECTDRAWSURFACE7 lpddsbackground = NULL;	// 加载背景图

BITMAP_FILE  bitmap;							// 测试加载位图

ALIEN_OBJ             aliens[3];				// 3 aliens, one on each level
int gwidth = -1;
int gheight = -1;

char buffer[80];	// 记log

// mouse

POINT  mousePoint;
POINT  lastMousePoint;
bool mouseDown = false;

// 函数声明
LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

void TestPixel(HWND hwnd);
void TestLine(HWND hwnd);
void TestRectangle(HWND hwnd);
void TestEllipse(HWND hwnd);
void TestMoveingEllipse(HWND hwnd);
void TestPolygon(HWND hwnd);
void TestTextOut(HWND hwnd);
void TestDraw(HWND hwnd);

void TestDrawPixels();

int Game_Main(void *params = NULL, int num_parms = 0);
int Game_Init(void *params = NULL, int num_parms = 0);
int Game_Exit(void *params = NULL, int num_parms = 0);

void On_GameInit();
void On_GameMain();

COLORREF RandomColor();

void TestPixelFormat()
{
	DDPIXELFORMAT ddpixel;
	LPDIRECTDRAWSURFACE7 lpdds_primary = NULL;
	DDRAW_INIT_STRUCT(ddpixel);
	lpdds_primary->GetPixelFormat(&ddpixel);

	if (ddpixel.dwFlags&DDPF_RGB)
	{
		switch (ddpixel.dwRGBBitCount)
		{
		case 15:// 一定是rgb 555
		{}
		break;
		case 16:// 一定是rgb 565
		{}
		break;
		case 24:// 一定是rgb 888
		{}
		break;
		case 32:// 一定是argb 8888
		{}
		break;
		default:
			break;
		}
	}
	else if (ddpixel.dwFlags&DDPF_PALETTEINDEXED8)
	{
		// 8位调色板模式
	}
	else
	{

		// 其他模式
	}
}

void TestDoubleBuffering()
{
	UCHAR *primary_buffer = NULL;
	memset((void*)double_buffer, 0, SCREEN_WIDTH*SCREEN_HEIGHT);

	int mempitch = ddsd.lPitch;
	for (int i = 0; i < 5000; ++i)
	{
		//		UCHAR red = rand() % 256;
		//		UCHAR green = rand() % 256;
		//		UCHAR blue = rand() % 256;
		UCHAR col = rand() % 256;
		int x = rand() % SCREEN_WIDTH;
		int y = rand() % SCREEN_HEIGHT;
		double_buffer[x + y * SCREEN_WIDTH] = col;
	}

	DDRAW_INIT_STRUCT(ddsd);

	lpddsprimary->Lock(NULL, &ddsd,
		DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);

	primary_buffer = (UCHAR *)ddsd.lpSurface;

	if (SCREEN_WIDTH == mempitch)
	{
		memcpy((void *)primary_buffer, (void *)double_buffer, SCREEN_WIDTH*SCREEN_HEIGHT);
	}
	else
	{
		UCHAR *dest_ptr = primary_buffer;
		UCHAR *src_ptr = double_buffer;
		for (int i = 0; i < SCREEN_HEIGHT; ++i)
		{
			memcpy((void *)dest_ptr, (void *)src_ptr, SCREEN_WIDTH);
			src_ptr += SCREEN_WIDTH;
			dest_ptr += mempitch;
		}
	}

	lpddsprimary->Unlock(NULL);

}

// 后备缓冲
int TestBackBuffer()
{
	DDRAW_INIT_STRUCT(ddsd);
	if (FAILED(lpddsback->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL)))
	{
		return 0;
	}

	UCHAR *back_buffer = (UCHAR *)ddsd.lpSurface;

	// 1.清空备用缓冲
	if (ddsd.lPitch == SCREEN_WIDTH)
	{
		memset(back_buffer, 0, SCREEN_WIDTH*SCREEN_HEIGHT);
	}
	else   // 如果不是正好等于Widnw_w，一行一行清空
	{
		UCHAR* dest_ptr = back_buffer;
		for (int y = 0; y < SCREEN_HEIGHT; ++y)
		{
			memset(dest_ptr, 0, SCREEN_WIDTH);
			dest_ptr += ddsd.lPitch;
		}
	}

	// 2.将内容绘制到back_buffer
	for (int i = 0; i < 5000; ++i)
	{
		UCHAR color = rand() % 255;
		int x = rand() % SCREEN_WIDTH;
		int y = rand() % SCREEN_HEIGHT;
		Plot8(x, y, color, back_buffer, ddsd.lPitch);
	}

	lpddsback->Unlock(NULL);

	// 3.页面切换
	while (FAILED(lpddsprimary->Flip(NULL, DDFLIP_WAIT)))
	{
		Sleep(500);
		return 1;
	}
	return 1;
}


int TestBlit()
{
	DDBLTFX ddbltfx;
	RECT dest_rect;
	DDRAW_INIT_STRUCT(ddbltfx);
	ddbltfx.dwFillColor = RandomRGB16BIT565();
	dest_rect.left = rand() % SCREEN_WIDTH;
	dest_rect.right = rand() % SCREEN_WIDTH;
	dest_rect.top = rand() % SCREEN_HEIGHT;
	dest_rect.bottom = rand() % SCREEN_HEIGHT;

	lpddsprimary->Blt(&dest_rect, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
	return 1;

}

int TestBlitFast()
{
	DDBLTFX ddbltfx;
	RECT dest_rect;
	DDRAW_INIT_STRUCT(ddbltfx);
	ddbltfx.dwFillColor = RandomRGB16BIT565();
	dest_rect.left = rand() % SCREEN_WIDTH;
	dest_rect.right = rand() % SCREEN_WIDTH;
	dest_rect.top = rand() % SCREEN_HEIGHT;
	dest_rect.bottom = rand() % SCREEN_HEIGHT;

	lpddsprimary->BltFast(dest_rect.left, dest_rect.top, lpddsprimary, &dest_rect, DDBLT_COLORFILL | DDBLT_WAIT);
	//	lpddsprimary->Blt(&dest_rect, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
	return 1;

}
void TestInitBackSurfaceColor16BIT()
{
	lpddsback->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
	USHORT *video_buffer = (USHORT *)ddsd.lpSurface;

	// draw the gradient
	for (int index_y = 0; index_y < SCREEN_HEIGHT; index_y++)
	{
		// build color word up
		DWORD color = __RGB16BIT565(0, (index_y >> 3), 0);

		// replicate color in upper and lower 16 bits of 32-bit word 
		color = (color) | (color << 16);

		// now color has two pixel in it in 16.16 or RGB.RGB format, use a DWORD
		// or 32-bit copy to move the bytes into the next video line, we'll need
		// inline assembly though...

		// draw next line, use a little inline asm baby!
		_asm
		{
			CLD; clear direction of copy to forward
			MOV EAX, color; color goes here
			MOV ECX, (SCREEN_WIDTH / 2); number of DWORDS goes here
			MOV EDI, video_buffer; address of line to move data
			REP STOSD; send the pentium X on its way
		} // end asm

	// now advance video_buffer to next line
		video_buffer += (ddsd.lPitch >> 1);

	} // end for index_y
	lpddsback->Unlock(NULL);
}
int TestBlitCopy()
{
	DDBLTFX ddbltfx;
	RECT dest_rect, src_rect;
	DDRAW_INIT_STRUCT(ddbltfx);
	int x1 = rand() % SCREEN_WIDTH;
	int y1 = rand() % SCREEN_HEIGHT;
	int x2 = rand() % SCREEN_WIDTH;
	int y2 = rand() % SCREEN_HEIGHT;
	int x3 = rand() % SCREEN_WIDTH;
	int y3 = rand() % SCREEN_HEIGHT;
	int x4 = rand() % SCREEN_WIDTH;
	int y4 = rand() % SCREEN_HEIGHT;

	//	int x1 = 0;
	//	int y1 = 0;
	//	int x2 =  SCREEN_WIDTH;
	//	int y2 =  SCREEN_HEIGHT;
	//	int x3 = 0;
	//	int y3 = 0;
	//	int x4 =  SCREEN_WIDTH;
	//	int y4 =  SCREEN_HEIGHT;

	src_rect.left = x1;
	src_rect.top = y1;
	src_rect.right = x2;
	src_rect.bottom = y2;

	dest_rect.left = x3;
	dest_rect.top = y3;
	dest_rect.right = x4;
	dest_rect.bottom = y4;

	lpddsprimary->Blt(&dest_rect, lpddsback, &src_rect, DDBLT_WAIT, NULL);
	return 1;

}

// nothing happened
int TestBlitCopyOnPrimarySurface()
{
	DDBLTFX ddbltfx;
	RECT dest_rect, src_rect;
	DDRAW_INIT_STRUCT(ddbltfx);
	ddbltfx.dwFillColor = RandomRGB16BIT565();
	int x1 = rand() % SCREEN_WIDTH;
	int y1 = rand() % SCREEN_HEIGHT;
	int x2 = rand() % SCREEN_WIDTH;
	int y2 = rand() % SCREEN_HEIGHT;
	int x3 = rand() % SCREEN_WIDTH;
	int y3 = rand() % SCREEN_HEIGHT;
	int x4 = rand() % SCREEN_WIDTH;
	int y4 = rand() % SCREEN_HEIGHT;

	//	int x1 = 0;
	//	int y1 = 0;
	//	int x2 =  SCREEN_WIDTH;
	//	int y2 =  SCREEN_HEIGHT;
	//	int x3 = 0;
	//	int y3 = 0;
	//	int x4 =  SCREEN_WIDTH;
	//	int y4 =  SCREEN_HEIGHT;

	src_rect.left = x1;
	src_rect.top = y1;
	src_rect.right = x2;
	src_rect.bottom = y2;

	dest_rect.left = x3;
	dest_rect.top = y3;
	dest_rect.right = x4;
	dest_rect.bottom = y4;

	lpddsprimary->Blt(&dest_rect, NULL, &src_rect, DDBLT_WAIT | DDBLT_COLORFILL, &ddbltfx);
	return 1;

}


#pragma region Clip bitmap HappyFace demo_7_8



UCHAR happy_bitmap[64] = { 0,0,0,0,0,0,0,0,
						  0,0,1,1,1,1,0,0,
						  0,1,0,1,1,0,1,0,
						  0,1,1,1,1,1,1,0,
						  0,1,0,1,1,0,1,0,
						  0,1,1,0,0,1,1,0,
						  0,0,1,1,1,1,0,0,
						  0,0,0,0,0,0,0,0 };

UCHAR sad_bitmap[64] = { 0,0,0,0,0,0,0,0,
						0,0,1,1,1,1,0,0,
						0,1,0,1,1,0,1,0,
						0,1,1,1,1,1,1,0,
						0,1,1,0,0,1,1,0,
						0,1,0,1,1,0,1,0,
						0,0,1,1,1,1,0,0,
						0,0,0,0,0,0,0,0 };

typedef struct HappyFace_TYP
{
	int x, y;
	int vx, vy;

}HappyFace, *HappyFace_PTR;

HappyFace happy_faces[100];

void TestInitHappyFace()
{
	for (int i = 0; i < 100; ++i)
	{
		happy_faces[i].x = rand() % SCREEN_WIDTH;
		happy_faces[i].y = rand() % SCREEN_HEIGHT;
		happy_faces[i].vx = -2 + rand() % 5;
		happy_faces[i].vy = -2 + rand() % 5;
	}
}

int TestGameMainHappyFace()
{
	DDBLTFX ddbltfx;
	DDRAW_INIT_STRUCT(ddbltfx);
	ddbltfx.dwFillColor = 255;
	// 将backsurface设置为黑色
	if (FAILED(lpddsback->Blt(NULL,
		NULL,
		NULL,
		DDBLT_COLORFILL | DDBLT_WAIT,
		&ddbltfx)))
	{
		return 0;
	}

	DDRAW_INIT_STRUCT(ddsd);

	if (FAILED(lpddsback->Lock(NULL, &ddsd,
		DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL)))
	{
		return 0;
	}

	for (int i = 0; i < 100; ++i)
	{
		UCHAR *video_buffer = (UCHAR*)ddsd.lpSurface;

		Blit_Clipped(happy_faces[i].x, happy_faces[i].y, 8, 8, happy_bitmap, video_buffer, ddsd.lPitch);

	}
	for (int i = 0; i < 100; ++i)
	{
		happy_faces[i].x += happy_faces[i].vx;
		happy_faces[i].y += happy_faces[i].vy;

		if (happy_faces[i].x > SCREEN_WIDTH)
		{
			happy_faces[i].x = -8;
		}
		else if (happy_faces[i].x < -8)
		{

			happy_faces[i].x = SCREEN_WIDTH;
		}

		if (happy_faces[i].y > SCREEN_HEIGHT)
		{
			happy_faces[i].y = -8;
		}
		else if (happy_faces[i].y < -8)
		{

			happy_faces[i].y = SCREEN_HEIGHT;
		}

	}

	if (FAILED(lpddsback->Unlock(NULL)))
	{
		return 0;
	}

	while (FAILED(lpddsprimary->Flip(NULL, DDFLIP_WAIT)));

	return 1;
}
void TestExitHappyFace()
{

}

#pragma endregion


#pragma region DDraw_Attach_Clipper page243

// 创建一个裁剪器
LPDIRECTDRAWCLIPPER DDraw_Attach_Clipper(LPDIRECTDRAWSURFACE7 lpdds, int num_rects, LPRECT clip_list)
{
	int index;
	LPDIRECTDRAWCLIPPER lpddclipper;
	LPRGNDATA region_data;

	if (FAILED(lpdd->CreateClipper(0, &lpddclipper, NULL)))
	{
		return NULL;
	}

	region_data = (LPRGNDATA)malloc(sizeof(RGNDATAHEADER) + sizeof(RECT)*num_rects);

	memcpy(region_data->Buffer, clip_list, sizeof(RECT)*num_rects);

	region_data->rdh.dwSize = sizeof(RGNDATAHEADER);
	region_data->rdh.iType = RDH_RECTANGLES;
	region_data->rdh.nCount = num_rects;
	region_data->rdh.nRgnSize = sizeof(RECT)*num_rects;
	region_data->rdh.rcBound.left = 64000;
	region_data->rdh.rcBound.top = 64000;
	region_data->rdh.rcBound.right = -64000;
	region_data->rdh.rcBound.bottom = -64000;

	for (index = 0; index < num_rects; ++index)
	{
		if (clip_list[index].left < region_data->rdh.rcBound.left)
		{
			region_data->rdh.rcBound.left = clip_list[index].left;
		}

		if (clip_list[index].top < region_data->rdh.rcBound.top)
		{
			region_data->rdh.rcBound.top = clip_list[index].top;
		}

		if (clip_list[index].right > region_data->rdh.rcBound.right)
		{
			region_data->rdh.rcBound.right = clip_list[index].right;
		}

		if (clip_list[index].bottom > region_data->rdh.rcBound.bottom)
		{
			region_data->rdh.rcBound.bottom = clip_list[index].bottom;
		}
	}

	if (FAILED(lpddclipper->SetClipList(region_data, 0)))
	{
		free(region_data);
		return NULL;
	}

	if (FAILED(lpdds->SetClipper(lpddclipper)))
	{
		free(region_data);
		return NULL;
	}
	free(region_data);
	return lpddclipper;
}
int InitAttachClip()
{
	RECT rect_list[3] = {
		{10,10,100,100},
		{100,100,200,200},
		{300,300,500,450}
	};

	if (FAILED(lpddclipper = DDraw_Attach_Clipper(lpddsprimary, 3, rect_list)))
	{
		return 0;
	}
	return 1;
}

int ExitAttackClip()
{
	if (lpddclipper != NULL)
	{
		lpddclipper->Release();
		lpddclipper = NULL;
	}
	return 1;
}


int TestClipperHappyFace()
{
	DDBLTFX ddbltfx;
	DDRAW_INIT_STRUCT(ddbltfx);
	ddbltfx.dwFillColor = 255;
	// 将backsurface设置为白色
	if (FAILED(lpddsback->Blt(NULL,
		NULL,
		NULL,
		DDBLT_COLORFILL | DDBLT_WAIT,
		&ddbltfx)))
	{
		return 0;
	}

	DDRAW_INIT_STRUCT(ddsd);
	if (FAILED(lpddsback->Lock(NULL, &ddsd,
		DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL)))
	{
		return 0;
	}

	for (int i = 0; i < 100; ++i)
	{
		UCHAR *video_buffer = (UCHAR*)ddsd.lpSurface;
		Blit_Clipped(happy_faces[i].x, happy_faces[i].y, 8, 8, happy_bitmap, video_buffer, ddsd.lPitch);

	}
	for (int i = 0; i < 100; ++i)
	{
		happy_faces[i].x += happy_faces[i].vx;
		happy_faces[i].y += happy_faces[i].vy;

		if (happy_faces[i].x > SCREEN_WIDTH)
		{
			happy_faces[i].x = -8;
		}
		else if (happy_faces[i].x < -8)
		{

			happy_faces[i].x = SCREEN_WIDTH;
		}

		if (happy_faces[i].y > SCREEN_HEIGHT)
		{
			happy_faces[i].y = -8;
		}
		else if (happy_faces[i].y < -8)
		{

			happy_faces[i].y = SCREEN_HEIGHT;
		}

	}

	if (FAILED(lpddsback->Unlock(NULL)))
	{
		return 0;
	}

	// 不需要翻转了
//	while (FAILED(lpddsprimary->Flip(NULL, DDFLIP_WAIT)));

//	RECT source_rect, dest_rect;  
//	int x1 = rand()%SCREEN_WIDTH;
//	int y1 = rand()%SCREEN_HEIGHT;
//	int x2 = rand()%SCREEN_WIDTH;
//	int y2 = rand()%SCREEN_HEIGHT;
//
//	int x3 = rand()%SCREEN_WIDTH;
//	int y3 = rand()%SCREEN_HEIGHT;
//	int x4 = rand()%SCREEN_WIDTH;
//	int y4 = rand()%SCREEN_HEIGHT;
//
//	source_rect.left   = x1;
//	source_rect.top    = y1;
//	source_rect.right  = x2;
//	source_rect.bottom = y2;
//
//	dest_rect.left   = x3;
//	dest_rect.top    = y3;
//	dest_rect.right  = x4;
//	dest_rect.bottom = y4;

	// make the blitter call
//	if (FAILED(lpddsprimary->Blt(&dest_rect, lpddsback, &source_rect, DDBLT_WAIT, NULL)))

	// 这里直接全屏拷贝像素
	if (FAILED(lpddsprimary->Blt(NULL, lpddsback, NULL, DDBLT_WAIT, NULL)))
	{
		return(0);
	}
	return 1;
}

#pragma endregion

#pragma region BitMap




// 上下翻转bitmap内存
int Flip_Bitmap(UCHAR* image, int bytes_per_line, int height)
{
	UCHAR *buffer;
	if (!(buffer = (UCHAR*)malloc(bytes_per_line*height)))
	{
		return 0;
	}

	memcpy(buffer, image, bytes_per_line*height);

	for (int i = 0; i < height; ++i)
	{
		memcpy(&image[(height - 1 - i)*bytes_per_line], &buffer[i*bytes_per_line], bytes_per_line);
	}
	free(buffer);
	return 1;
}

// 加载bitmap文件
int Load_Bitmap_File(BITMAP_FILE_PTR bitmap, const char* filename)
{
	int file_handle, index;

	UCHAR *temp_buffer = NULL;
	OFSTRUCT file_data;

	if ((file_handle = OpenFile(filename, &file_data, OF_READ)) == -1)
	{
		printf("load_bitmap_file error! openFile failed!!");
		return 0;
	}

	// 读取bitmap文件头部分的信息
	_lread(file_handle, &bitmap->bitmapfileHeader, sizeof(BITMAPFILEHEADER));

	// 如果读取之后，不是bitmap类型
	if (bitmap->bitmapfileHeader.bfType != BITMAP_ID)
	{
		_lclose(file_handle);
		printf("load_bitmap_file error! read headinfo failed!!");
		return 0;
	}

	// 读取bitmap头部分的信息
	_lread(file_handle, &bitmap->bitmapInfoHeader, sizeof(BITMAPINFOHEADER));

	// 如果是8位模式，读取调色板信息
	// 并且调整rgb顺序。读取信息是BGR顺序，Paletteentry是RGB顺序. G的顺序不用改变.
	if (bitmap->bitmapInfoHeader.biBitCount == 8)
	{
		_lread(file_handle, &bitmap->palette, MAX_COLORS_PALETTE * sizeof(PALETTEENTRY));

		for (index = 0; index < MAX_COLORS_PALETTE; ++index)
		{
			int tmp_color = bitmap->palette[index].peRed;
			bitmap->palette[index].peRed = bitmap->palette[index].peBlue;
			bitmap->palette[index].peBlue = tmp_color;

			bitmap->palette[index].peFlags = PC_NOCOLLAPSE;
		}
	}

	// 最后，读取像素信息
	// 定位开始位置
	_llseek(file_handle, -(int)(bitmap->bitmapInfoHeader.biSizeImage), SEEK_END);

	if (bitmap->bitmapInfoHeader.biBitCount == 8 ||
		bitmap->bitmapInfoHeader.biBitCount == 16 ||
		bitmap->bitmapInfoHeader.biBitCount == 24
		)
	{
		if (bitmap->buffer)
		{
			free(bitmap->buffer);
		}

		if (!(bitmap->buffer = (UCHAR*)malloc(bitmap->bitmapInfoHeader.biSizeImage)))
		{
			_lclose(file_handle);
			printf("load_bitmap_file error! malloc failed!!");
			return 0;
		}

		// 读取到buffer中
		_lread(file_handle, bitmap->buffer, bitmap->bitmapInfoHeader.biSizeImage);

	}
	else		// 其他位的像素（serious problem) (试试32位？？)
	{
		printf("load_bitmap_file error! not 8pixel !!");
		return 0;
	}

	_lclose(file_handle);

	Flip_Bitmap(bitmap->buffer,
		bitmap->bitmapInfoHeader.biWidth*(bitmap->bitmapInfoHeader.biBitCount / 8),
		bitmap->bitmapInfoHeader.biHeight);

	return 1;
}

// 释放内存
int Unload_Bitmap_Flie(BITMAP_FILE_PTR bitmap)
{
	if (bitmap->buffer)
	{
		free(bitmap->buffer);
		bitmap->buffer = NULL;
	}

	return 1;
}

int DDraw_Fill_Surface(LPDIRECTDRAWSURFACE7 lpdds, int color)
{
	DDBLTFX ddbltfx;
	DDRAW_INIT_STRUCT(ddbltfx);

	if (ddpixelformat.dwRGBBitCount == 8)
	{

		ddbltfx.dwFillColor = color;
	}
	else if (ddpixelformat.dwRGBBitCount == 8)
	{
		ddbltfx.dwFillColor = __RGB16BIT565(0, 0, 0);
	}
	else
	{
		ddbltfx.dwFillColor = __RGB32BIT(1, 0, 0, 0);
	}
	lpdds->Blt(NULL,
		NULL,
		NULL,
		DDBLT_COLORFILL | DDBLT_WAIT,
		&ddbltfx);

	return 1;
}
int TestBitMapLoad()
{
	//	const char *bmpName = "cyc_tree.bmp";
	//	const char *bmpName = "cyc_flower.bmp";
	//	const char *bmpName = "bitmap8.bmp";
	//	const char *bmpName = "bitmap24.bmp";
	const char *bmpName = "bitmap8.bmp";
	if (!Load_Bitmap_File(&bitmap, bmpName))
	{
		return 0;
	}
	if (SCREEN_BPP == 8)
	{
		if (FAILED(lpddpal->SetEntries(0, 0, MAX_COLORS_PALETTE, bitmap.palette)))
		{
			return 0;
		}
		DDraw_Fill_Surface(lpddsprimary, 120);
	}
	return 1;
}
int TestBitmap8Main()
{
	//	DDraw_Fill_Surface(lpddsprimary, 120);
		//	return 1;
	lpddpal->SetEntries(0, 0, MAX_COLORS_PALETTE, bitmap.palette);
	lpddsprimary->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
	UCHAR *primary_buffer = (UCHAR*)ddsd.lpSurface;

	// test if memory is linear
//	if (ddsd.lPitch == SCREEN_WIDTH)
//	{
//		memcpy((void *)primary_buffer, (void *)bitmap.buffer, SCREEN_WIDTH*SCREEN_HEIGHT);
//	} // end if
//	else
	{ // non-linear

	// make copy of source and destination addresses
		UCHAR *dest_ptr = primary_buffer;
		UCHAR *src_ptr = bitmap.buffer;
		int bitmapWidth = bitmap.bitmapInfoHeader.biWidth;

		// memory is non-linear, copy line by line
		for (int y = 0; y < SCREEN_HEIGHT; y++)
		{
			// copy line
			memcpy((void *)dest_ptr, (void *)src_ptr, SCREEN_WIDTH);

			// advance pointers to next line
			dest_ptr += ddsd.lPitch;
			src_ptr += bitmapWidth;
		} // end for

	} // end else

 // now unlock the primary surface
	if (FAILED(lpddsprimary->Unlock(NULL)))
		return(0);
	return 1;

}
void TestBitmapUnload()
{
	Unload_Bitmap_Flie(&bitmap);
}
// 16位位图
int TestBitMap16Main()
{
	lpddsprimary->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);

	USHORT *primary_buffer = (USHORT *)ddsd.lpSurface;
	if (primary_buffer)
	{
		for (int index_y = 0; index_y < SCREEN_HEIGHT; index_y++)
		{
			for (int index_x = 0; index_x < SCREEN_WIDTH; index_x++)
			{
				UCHAR blue = (bitmap.buffer[index_y*SCREEN_WIDTH * 3 + index_x * 3 + 0]) >> 3,
					green = (bitmap.buffer[index_y*SCREEN_WIDTH * 3 + index_x * 3 + 1]) >> 3,
					red = (bitmap.buffer[index_y*SCREEN_WIDTH * 3 + index_x * 3 + 2]) >> 3;

				USHORT pixel = __RGB16BIT565(red, green, blue);

				primary_buffer[index_x + (index_y*ddsd.lPitch >> 1)] = pixel;

			}

		}

	}

	lpddsprimary->Unlock(NULL);
	return 1;
}

int TestBitMap24Main()
{
	lpddsprimary->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);

	DWORD *primary_buffer = (DWORD *)ddsd.lpSurface;

	for (int index_y = 0; index_y < SCREEN_HEIGHT; index_y++)
	{
		for (int index_x = 0; index_x < SCREEN_WIDTH; index_x++)
		{
			// get BGR values
			UCHAR blue = (bitmap.buffer[index_y*SCREEN_WIDTH * 3 + index_x * 3 + 0]),
				green = (bitmap.buffer[index_y*SCREEN_WIDTH * 3 + index_x * 3 + 1]),
				red = (bitmap.buffer[index_y*SCREEN_WIDTH * 3 + index_x * 3 + 2]);

			// this builds a 32 bit color value in A.8.8.8 format (8-bit alpha mode)	// 系统不支持24位。会补一个alpha，成32位。
			DWORD pixel = __RGB32BIT(0, red, green, blue);

			// write the pixel
			primary_buffer[index_x + (index_y*ddsd.lPitch >> 2)] = pixel;

		} // end for index_x

	} // end for index_y

// now unlock the primary surface
	if (FAILED(lpddsprimary->Unlock(NULL)))
		return(0);

	// do nothing -- look at pretty picture

	// return success or failure or your own return code here
	return(1);

	return 1;
}

#pragma endregion

#pragma region OffScreen Render;demo_7_13 
// 创建离屏表面
LPDIRECTDRAWSURFACE7 DDraw_Create_Surface(int width, int height, int mem_flags, int color_key = 0)
{

	DDSURFACEDESC2 ddsd;         // working description
	LPDIRECTDRAWSURFACE7 lpdds;  // temporary surface

	// set to access caps, width, and height
	DDRAW_INIT_STRUCT(ddsd);
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;

	ddsd.dwWidth = width;
	ddsd.dwHeight = height;

	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | mem_flags;

	if (FAILED(lpdd->CreateSurface(&ddsd, &lpdds, NULL)))
		return(NULL);

	if (color_key >= 0)
	{
		DDCOLORKEY color_key;
		if (ddpixelformat.dwRGBBitCount == 8)
		{
			color_key.dwColorSpaceLowValue = 0;				// 从Low到High都是透明键
			color_key.dwColorSpaceHighValue = 0;
		}
		else if (ddpixelformat.dwRGBBitCount == 16)
		{
			color_key.dwColorSpaceLowValue = __RGB16BIT565(0, 0, 0);				// 从Low到High都是透明键
			color_key.dwColorSpaceHighValue = __RGB16BIT565(0, 0, 0);
		}
		else // 24 or 32 (24位系统不支持，会转换为32位)
		{
			color_key.dwColorSpaceLowValue = __RGB32BIT(1, 0, 0, 0);				// 从Low到High都是透明键
			color_key.dwColorSpaceHighValue = __RGB32BIT(1, 0, 0, 0);
		}

		lpdds->SetColorKey(DDCKEY_SRCBLT, &color_key);
	} // end if

 // return surface
	return(lpdds);
} // end DDraw_Create_Surface

/// 将Source blt到 dest. 目标位置是x,y;目标的宽度和高度是width,height; transparent表示是否使用透明色键
int DDraw_Draw_Surface(LPDIRECTDRAWSURFACE7 source, int x, int y, int width, int height, LPDIRECTDRAWSURFACE7 dest, int transparent = 1, float scale = 1.0f)
{
	RECT dest_rect;
	RECT src_rect;

	src_rect.left = 0;
	src_rect.top = 0;
	src_rect.right = width - 1;
	src_rect.bottom = height - 1;

	dest_rect.left = x;
	dest_rect.top = y;
	dest_rect.right = x + width * scale - 1;
	dest_rect.bottom = y + height * scale - 1;

	if (transparent)
	{
		if (FAILED(dest->Blt(&dest_rect, source, &src_rect, (DDBLT_WAIT | DDBLT_KEYSRC), NULL)))
		{
			return 0;
		}
	}
	else
	{
		if (FAILED(dest->Blt(&dest_rect, source, &src_rect, (DDBLT_WAIT), NULL)))
		{
			return 0;
		}
	}
	return 1;
}

// 将bitmap从cy行，cx个sprite的内容渲染到lpdds
int Scan_Image_Bitmap8(BITMAP_FILE_PTR bitmap,     // bitmap file to scan image data from
	LPDIRECTDRAWSURFACE7 lpdds, // surface to hold data
	int cx, int cy)             // cell to scan image from
{
	UCHAR * source_ptr;
	UCHAR * dest_ptr;
	DDSURFACEDESC2 ddsd;
	ddsd.dwSize = sizeof(ddsd);

	lpdds->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);

	// 计算在surface中的开始的位置(平移到相对位置的(0,0)点)
	// 因为使用的是专门用于加载一帧动画的内存，ddsd的宽度和高度都是提前设定好的一个sprite的宽度和高度（72x80）
	// 有1个像素的空白
	gwidth = ddsd.dwWidth;
	gheight = ddsd.dwHeight;
	cx = cx * (gwidth + 1) + 1;		// x
	cy = cy * (gheight + 1) + 1;		// y


	source_ptr = bitmap->buffer + cy * bitmap->bitmapInfoHeader.biWidth + cx;
	dest_ptr = (UCHAR *)ddsd.lpSurface;
	for (int i = 0; i < gheight; ++i)
	{
		memcpy((void *)dest_ptr, (void *)source_ptr, gwidth);
		dest_ptr += ddsd.lPitch;
		source_ptr += bitmap->bitmapInfoHeader.biWidth;
	}

	lpdds->Unlock(NULL);

	return 1;
}

// 将bitmap从cy行，cx个sprite的内容渲染到lpdds
int Scan_Image_Bitmap24(BITMAP_FILE_PTR bitmap,     // bitmap file to scan image data from
	LPDIRECTDRAWSURFACE7 lpdds, // surface to hold data
	int cx, int cy)             // cell to scan image from
{
	DWORD * dest_ptr;
	DDSURFACEDESC2 ddsd;
	DDRAW_INIT_STRUCT(ddsd);

	lpdds->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);

	// 计算在surface中的开始的位置(平移到相对位置的(0,0)点)
	// 因为使用的是专门用于加载一帧动画的内存，ddsd的宽度和高度都是提前设定好的一个sprite的宽度和高度（72x80）
	// 有1个像素的空白
	gwidth = ddsd.dwWidth;
	gheight = ddsd.dwHeight;
	int bitmapWidth = bitmap->bitmapInfoHeader.biWidth;

	cx = cx * (gwidth + 1) + 1;		// x
	cy = cy * (gheight + 1) + 1;		// y


	// 24位位图，一个像素占3个字节
	dest_ptr = (DWORD *)ddsd.lpSurface;

	int xoff = cx * 3;
	int yoff = cy * bitmapWidth * 3;

	for (int y = 0; y < gheight; ++y)
	{
		for (int x = 0; x < gwidth; ++x)
		{

			int off = xoff + yoff + y * bitmapWidth * 3 + x * 3;

			// 逐像素转换
			UCHAR blue = bitmap->buffer[off];
			UCHAR green = bitmap->buffer[off + 1];
			UCHAR red = bitmap->buffer[off + 2];

			DWORD pixel = __RGB32BIT(0, red, green, blue);
			dest_ptr[(y*ddsd.lPitch >> 2) + x] = pixel;
		}
	}

	lpdds->Unlock(NULL);

	return 1;
}

int Test_BG_Bitmap8_Render(LPDIRECTDRAWSURFACE7 lpdds, BITMAP_FILE bitmap)
{
	DDSURFACEDESC2 ddsd;
	DDRAW_INIT_STRUCT(ddsd);
	if (FAILED(lpdds->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL)))
		return 0;

	UCHAR * image_buffer = (UCHAR *)ddsd.lpSurface;
	if (ddsd.lPitch == SCREEN_WIDTH)
	{
		memcpy((void *)image_buffer, (void *)bitmap.buffer, SCREEN_WIDTH*SCREEN_HEIGHT);
	}
	else
	{
		UCHAR * buffer = bitmap.buffer;
		for (int i = 0; i < SCREEN_HEIGHT; ++i)
		{
			memcpy((void *)image_buffer, (void *)buffer, SCREEN_WIDTH);
			image_buffer += ddsd.lPitch;
			buffer += SCREEN_WIDTH;
		}
	}
	if (FAILED(lpdds->Unlock(NULL)))
		return 0;
	return 1;
}

int Test_BG_Bitmap24_Render(LPDIRECTDRAWSURFACE7 lpdds, BITMAP_FILE bitmap)
{
	DDSURFACEDESC2 ddsd;
	DDRAW_INIT_STRUCT(ddsd);
	if (FAILED(lpdds->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL)))
		return 0;

	DWORD * dest_ptr = (DWORD *)ddsd.lpSurface;
	DWORD* src_ptr = (DWORD *)bitmap.buffer;

	for (int y = 0; y < SCREEN_HEIGHT; ++y)
	{
		for (int x = 0; x < SCREEN_WIDTH; ++x)
		{
			UCHAR blue = (bitmap.buffer[y*SCREEN_WIDTH * 3 + x * 3 + 0]);
			UCHAR green = (bitmap.buffer[y*SCREEN_WIDTH * 3 + x * 3 + 1]);
			UCHAR red = (bitmap.buffer[y*SCREEN_WIDTH * 3 + x * 3 + 2]);

			DWORD pixel = __RGB32BIT(0, red, green, blue);
			dest_ptr[x + (y * ddsd.lPitch >> 2)] = pixel;
		}
	}


	if (FAILED(lpdds->Unlock(NULL)))
		return 0;

	return 1;
}


// 加载动画的init
int Test_Sprite_Anim_Init()
{
	// 备用缓冲添加裁剪
	RECT screen_rect = { 0,0,SCREEN_WIDTH - 1,SCREEN_HEIGHT - 1 };
	lpddclipper = DDraw_Attach_Clipper(lpddsback, 1, &screen_rect);
	const char * bg_bitmap_name = "alley24.bmp";
	//	const char * bg_bitmap_name = "bitmap24.bmp";
	if (SCREEN_BPP == 8)
	{
		bg_bitmap_name = "alley8.bmp";

	}
	if (!Load_Bitmap_File(&bitmap, bg_bitmap_name))
		return 0;
	if (SCREEN_BPP == 8)
	{
		if (FAILED(lpddpal->SetEntries(0, 0, MAX_COLORS_PALETTE, bitmap.palette)))
			return 0;
	}
	DDraw_Fill_Surface(lpddsprimary, 0);
	DDraw_Fill_Surface(lpddsback, 0);

	// 创建离线缓存（0表示在显存创建，1表示在系统内存创建）
	lpddsbackground = DDraw_Create_Surface(SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1);

	// 将alley8.bmp渲染到lpddsbackground

	if (SCREEN_BPP == 8)
	{
		Test_BG_Bitmap8_Render(lpddsbackground, bitmap);
	}
	else
	{
		Test_BG_Bitmap24_Render(lpddsbackground, bitmap);
	}

	Unload_Bitmap_Flie(&bitmap);

	srand(GetTickCount());

	aliens[0].x = rand() % SCREEN_WIDTH;
	aliens[0].y = 116 - 72;
	aliens[0].velocity = 1 + rand() % 4;
	aliens[0].current_frame = 0;
	aliens[0].counter = 0;
	aliens[0].scale = 1.0f;

	// alien on level 2 of complex

	aliens[1].x = rand() % SCREEN_WIDTH;
	aliens[1].y = 246 - 72;
	aliens[1].velocity = 1 + rand() % 4;
	aliens[1].current_frame = 0;
	aliens[1].counter = 0;
	aliens[1].scale = 1.5f;

	// alien on level 3 of complex

	aliens[2].x = rand() % SCREEN_WIDTH;
	aliens[2].y = 382 - 72;
	aliens[2].velocity = 1 + rand() % 4;
	aliens[2].current_frame = 0;
	aliens[2].counter = 0;
	aliens[2].scale = 0.5f;

	const char * sprite_bmp_name = "dedsp1_24.bmp";
	if (SCREEN_BPP == 8)
	{
		sprite_bmp_name = "dedsp0.bmp";
	}
	if (!Load_Bitmap_File(&bitmap, sprite_bmp_name))
		return 0;
	for (int index = 0; index < 3; ++index)
	{
		aliens[0].frames[index] = DDraw_Create_Surface(72, 80, 0);
		if (SCREEN_BPP == 8)
		{

			Scan_Image_Bitmap8(&bitmap, aliens[0].frames[index], index, 0);
		}
		else
		{
			Scan_Image_Bitmap24(&bitmap, aliens[0].frames[index], index, 0);
		}
	}
	Unload_Bitmap_Flie(&bitmap);

	// 将第0个的sprite指给到另外两个。
	for (int i = 0; i < 3; ++i)
	{
		aliens[1].frames[i] = aliens[2].frames[i] = aliens[0].frames[i];
	}
	return 1;
}

// 动画 main
void Test_Sprite_Anim_Main()
{
	static int animation_seq[4] = { 0,1,0,2 };
	// blt aliens到 backsurface
	// 1.计算位置和速度
	for (int i = 0; i < 3; ++i)
	{
		aliens[i].counter++;
		if (aliens[i].counter >= 10)
		{
			aliens[i].counter = 0;
			aliens[i].current_frame = (++aliens[i].current_frame) % 4;
		}

		aliens[i].x++;// = aliens[i].velocity*0.5f;
		if (aliens[i].x > SCREEN_WIDTH)
		{
			aliens[i].x = -80;
		}
	}
	if (SCREEN_BPP == 8)
	{
		lpddpal->SetEntries(0, 0, MAX_COLORS_PALETTE, bitmap.palette);
	}
	else
	{
	}

	// 1.1 将背景图都blt到备用缓冲
	DDraw_Draw_Surface(lpddsbackground, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, lpddsback, 0);

	// 2.blt 将sprite blt到备用缓冲
	for (int i = 0; i < 3; ++i)
	{
		int cur_frame = aliens[i].current_frame;
		DDraw_Draw_Surface(aliens[i].frames[animation_seq[cur_frame]], aliens[i].x, aliens[i].y, 72, 80, lpddsback, 1, aliens[i].scale);
	}

	// 3.页面交换
	while (FAILED(lpddsprimary->Flip(NULL, DDFLIP_WAIT)));
}


#pragma endregion

#pragma region demo_7_15; 256 light
#define BLINKER_ADD 1
#define BLINKER_DEL 2 
#define BLINKER_UPDATE 3
#define BLINKER_RUN 4

// blinking light structure
typedef struct BLINKER_TYP
{
	// user sets these
	int color_index;         // index of color to blink
	PALETTEENTRY on_color;   // RGB value of "on" color
	PALETTEENTRY off_color;  // RGB value of "off" color
	int on_time;             // number of frames to keep "on" 
	int off_time;            // number of frames to keep "off"

	// internal member
	int counter;             // counter for state transitions
	int state;               // state of light, -1 off, 1 on, 0 dead
} BLINKER, *BLINKER_PTR;


int Blink_Colors(int command, BLINKER_PTR new_light, int id)
{
	static BLINKER lights[256];
	static int initialized = 0;
	if (!initialized)
	{
		initialized = 1;
		memset((void *)lights, 0, sizeof(lights));
	}

	switch (command)
	{
	case BLINKER_ADD:
		for (int i = 0; i < 256; ++i)
		{
			if (lights[i].state == 0)
			{
				lights[i] = *new_light;
				lights[i].state = 1;
				lights[i].counter = 0;
				return i;
			}
		}
		break;
	case BLINKER_DEL:
		for (int i = 0; i < 256; ++i)
		{
			if (lights[i].state != 0)
			{
				lights[i].state = 0;
			}
		}
		break;
	case BLINKER_UPDATE:
		break;
	case BLINKER_RUN:
		for (int i = 0; i < 256; ++i)
		{
			if (lights[i].state == 0)
				continue;
			lights[i].counter++;
			if (lights[i].state == 1 && lights[i].counter >= lights[i].on_time)
			{
				lights[i].counter = 0;
				lights[i].state = -1;
				bitmap.palette[lights[i].color_index] = lights[i].off_color;

				// 颜色滚动
//				PALETTEENTRY tmp = bitmap.palette[0];
//				for (int i = 0; i < 254; ++i)
//				{
//					bitmap.palette[i] = bitmap.palette[i + 1];
//
//				}
//				bitmap.palette[255] = tmp;
			}
			else if (lights[i].state == -1 && lights[i].counter >= lights[i].off_time)
			{
				lights[i].counter = 0;
				lights[i].state = 1;
				bitmap.palette[lights[i].color_index] = lights[i].on_color;
			}
		}
		break;
	default:
		break;
	}


}
void Test_light_256_Init()
{
	if (FAILED(Load_Bitmap_File(&bitmap, "cyc_flower_8b.bmp")))
		return;
	lpddpal->SetEntries(0, 0, MAX_COLORS_PALETTE, bitmap.palette);
	DDraw_Fill_Surface(lpddsprimary, 0);

	PALETTEENTRY	red_id = { 255,0,0,PC_NOCOLLAPSE };
	PALETTEENTRY	green_id = { 0,255,0,PC_NOCOLLAPSE };

	BLINKER tmp;
	// 自定义的bmp中，index 0-10 和index 245-255修改都无效；
	// 另外,index 32不行，手动修改了调色板的颜色，就可以了...(1,2修改手动修改调色板也不行) 

	tmp.color_index = 144;
	tmp.on_color = green_id;
	tmp.off_color = red_id;
	tmp.on_time = 30;
	tmp.off_time = 30;

	Blink_Colors(BLINKER_ADD, &tmp, 0);

	tmp.color_index = 222;// 113;
	tmp.on_color = red_id;
	tmp.off_color = green_id;
	tmp.on_time = 20;
	tmp.off_time = 40;
	Blink_Colors(BLINKER_ADD, &tmp, 0);

	//	for (int i = 0; i < 256; ++i)
	//	{
	//		bitmap.palette[i].peRed = 255;
	//		bitmap.palette[i].peBlue = 0;
	//		bitmap.palette[i].peGreen = 0;
	//	}


}
void Test_light_256_Main()
{
	Blink_Colors(BLINKER_RUN, NULL, 0);
	if (SCREEN_BPP == 8)
	{
		lpddpal->SetEntries(0, 0, MAX_COLORS_PALETTE, bitmap.palette);
	}
	Test_BG_Bitmap8_Render(lpddsprimary, bitmap);
}

#pragma endregion

#pragma region DirectDraw+GDI;demo_7_17 GDI和DirectDraw联合使用

int Draw_Text_GDI_IN_DD(const char * txt, int x, int y, COLORREF color, LPDIRECTDRAWSURFACE7 lpdds)
{
	HDC xdc;
	if (FAILED(lpdds->GetDC(&xdc)))
		return 0;
	SetTextColor(xdc, color);
	SetBkMode(xdc, TRANSPARENT);
	TextOut(xdc, x, y, txt, strlen(txt));

	if (FAILED(lpdds->ReleaseDC(xdc)))
		return 0;
	return 1;

}
void Test_dd_gdi_Init()
{

}
void Test_dd_gdi_Main()
{
	static int drawTxtCounter = 0;
	drawTxtCounter--;
	if (drawTxtCounter <= 0)
	{
		drawTxtCounter = 30;
		Draw_Text_GDI_IN_DD("DirectDraw_Text", rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT, RandomColor(), lpddsprimary);
	}
}
#pragma endregion


#pragma region demo_7_18 非全屏，窗口模式
// 窗口模式设置lpddsprimary裁剪器
int Win_Clipper_Init()
{
	if (FULL_SCREEN_MODE)
		return 0;

	if (lpddclipper)
		return 0;

	if (FAILED(lpdd->CreateClipper(0, &lpddclipper, NULL)))
		return 0;
	if (FAILED(lpddclipper->SetHWnd(0, main_window_handle)))
		return 0;
	if (FAILED(lpddsprimary->SetClipper(lpddclipper)))
		return 0;
	return 1;

}
void Test_win_Init()
{
	lpddsbackground = DDraw_Create_Surface(WIN_OFFSCREEN_WIDTH, WIN_OFFSCREEN_HEIGHT, 0);

}
void Test_win_Main()
{
	GetWindowRect(main_window_handle, &client);

	//	DDraw_Fill_Surface(lpddsbackground, 0);

	DDRAW_INIT_STRUCT(ddsd);
	lpddsbackground->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);
	if (ddpixelformat.dwRGBBitCount == 32)
	{
		DWORD * buffer = (DWORD*)ddsd.lpSurface;
		for (int i = 0; i < 1000; ++i)
		{
			DWORD pixel = RandomRGBA32();
			int rand_x = rand() % ddsd.dwWidth;
			int rand_y = rand() % ddsd.dwHeight;

			// 窗口模式下，创建的离屏缓冲区和lpddsprimary的设置一样，也就是说，32位深度下，ddsd.dwWidth是宽度，单位是像素，每个像素是4个字节，
			// 因此，使用ddsd.dwWidth作为宽度偏移的时候，不需要偏移了
			// 因为buffer是DWORD为单位的，所以ddsd.lPitch>>2等同于ddsd.dwWdith
			// ddsd.lPitch的大小是分配内存的宽度的字节数,由于内存分配的缘故，lpitch不一定是和需要的内存宽度的字节数一样
			buffer[rand_y*(ddsd.lPitch >> 2) + rand_x] = pixel;
			//			buffer[rand_y*(ddsd.dwWidth) + rand_x] = pixel;
		}

	}
	else if (ddpixelformat.dwRGBBitCount == 8)
	{
		UCHAR * buffer = (UCHAR *)ddsd.lpSurface;
		for (int i = 0; i < 100; ++i)
		{
			int rand_x = client.left + rand() % (client.right - client.left);
			int rand_y = client.top + rand() % (client.bottom - client.top);
			buffer[rand_y*(ddsd.lPitch) + rand_x] = rand() % 255;
		}
	}

	lpddsbackground->Unlock(NULL);
	RECT src_rect = { 0,0,ddsd.dwWidth - 1,ddsd.dwHeight - 1 };
	lpddsprimary->Blt(&client, lpddsbackground, &src_rect, DDBLT_WAIT, NULL);
	//	lpddsprimary->Blt(NULL, lpddsbackground,NULL, DDBLT_WAIT, NULL);
}
#pragma endregion 

#pragma region chapter8, draw_line


// vb_start: video buffer start
int Draw_Line8(int x0, int y0, int x1, int y1, UCHAR color, UCHAR *vb_start, int lpitch)
{
	int dx, dy, dx2, dy2, error;
	int x_inc, y_inc;
	int index;

	vb_start = vb_start + x0 + y0 * lpitch;

	dx = x1 - x0;
	dy = y1 - y0;


	if (dx >= 0)	//  让dx永远是正值,x_inc表示x变换的方向
	{
		x_inc = 1;
	}
	else
	{
		x_inc = -1;
		dx = -dx;
	}
	if (dy >= 0)
	{
		y_inc = lpitch;
	}
	else
	{
		dy = -dy;
		y_inc = -lpitch;
	}

	// 放到dx dy 都为正之后再移位
	dx2 = dx << 1;
	dy2 = dy << 1;

	if (dx > dy)	// 近x轴
	{
		error = dy2 - dx;
		for (index = 0; index <= dx; ++index)
		{
			*vb_start = color;
			//			vb_start[y*lpitch + x] = color;
			//			Plot8(x, y, color, vb_start, lpitch);
			if (error > 0)
			{
				error -= dx2;
				vb_start += y_inc;
			}
			error += dy2;
			vb_start += x_inc;
		}
	}
	else
	{
		error = dx2 - dy;
		for (index = 0; index <= dy; index++)
		{
			*vb_start = color;
			if (error >= 0)
			{
				error -= dy2;
				vb_start += x_inc;
			}
			error += dx2;
			vb_start += y_inc;
		}
	}

	return 1;
}

// vb_start: video buffer start
int Draw_Line16(int x0, int y0, int x1, int y1, USHORT color, USHORT *vb_start, int lpitch)
{
	int dx, dy, dx2, dy2, error;
	int x_inc, y_inc;
	int index;
	int x = x0, y = y0;

	vb_start = vb_start + x0 + y0 * (lpitch >> 1);

	dx = x1 - x0;
	dy = y1 - y0;


	if (dx >= 0)	//  让dx永远是正值,x_inc表示x变换的方向
	{
		x_inc = 1;
	}
	else
	{
		x_inc = -1;
		dx = -dx;
	}
	if (dy >= 0)
	{
		y_inc = 1;
	}
	else
	{
		dy = -dy;
		y_inc = -1;
	}

	dx2 = dx << 1;
	dy2 = dy << 1;

	if (dx > dy)	// 近x轴
	{
		error = dy2 - dx;
		for (index = 0; index <= dx; ++index)
		{
			*vb_start = color;
			if (error > 0)
			{
				error -= dx2;
				y += y_inc;
				vb_start = vb_start + y_inc * (lpitch >> 1);
			}
			error += dy2;
			x += x_inc;
			vb_start = vb_start + x_inc;
		}
	}
	else
	{
		error = dx2 - dy;
		for (index = 0; index <= dy; index++)
		{
			*vb_start = color;
			if (error >= 0)
			{
				error -= dy2;
				x += x_inc;
				vb_start = vb_start + x_inc;
			}
			error += dx2;
			y += y_inc;
			vb_start = vb_start + y_inc * (lpitch >> 1);
		}
	}

	return 1;
}

// vb_start: video buffer start
int Draw_Line32(int x0, int y0, int x1, int y1, UINT color, UINT *vb_start, int lpitch)
{
	int dx, dy, dx2, dy2, error;
	int x_inc, y_inc;
	int index;
	int x = x0, y = y0;

	vb_start = vb_start + x0 + y0 * (lpitch >> 2);

	dx = x1 - x0;
	dy = y1 - y0;


	if (dx >= 0)	//  让dx永远是正值,x_inc表示x变换的方向
	{
		x_inc = 1;
	}
	else
	{
		x_inc = -1;
		dx = -dx;
	}
	if (dy >= 0)
	{
		y_inc = 1;
	}
	else
	{
		dy = -dy;
		y_inc = -1;
	}

	dx2 = dx << 1;
	dy2 = dy << 1;

	if (dx > dy)	// 近x轴
	{
		error = dy2 - dx;
		for (index = 0; index <= dx; ++index)
		{
			*vb_start = color;
			if (error > 0)
			{
				error -= dx2;
				y += y_inc;
				vb_start = vb_start + y_inc * (lpitch >> 2);
			}
			error += dy2;
			x += x_inc;
			vb_start = vb_start + x_inc;
		}
	}
	else
	{
		error = dx2 - dy;
		for (index = 0; index <= dy; index++)
		{
			*vb_start = color;
			if (error >= 0)
			{
				error -= dy2;
				x += x_inc;
				vb_start = vb_start + x_inc;
			}
			error += dx2;
			y += y_inc;
			vb_start = vb_start + y_inc * (lpitch >> 2);
		}
	}

	return 1;
}

// 裁剪线段。返回值为1的时候，没有完全被裁剪掉；返回值为0的时候，完全被裁减掉了
int Clip_Line(RECT clipRect, int &x0, int &y0, int &x1, int &y1)
{
	int point0_code = CLIP_CODE_C, point1_code = CLIP_CODE_C;

	// line's pointcode
	if (x0 < clipRect.left)
	{
		point0_code |= CLIP_CODE_W;
	}
	else if (x0 > clipRect.right)
	{
		point0_code |= CLIP_CODE_E;
	}

	if (y0 < clipRect.top)
	{
		point0_code |= CLIP_CODE_N;
	}
	else if (y0 > clipRect.bottom)
	{
		point0_code |= CLIP_CODE_S;
	}


	if (x1 < clipRect.left)
	{
		point1_code |= CLIP_CODE_W;
	}
	else if (x1 > clipRect.right)
	{
		point1_code |= CLIP_CODE_E;
	}

	if (y1 < clipRect.top)
	{
		point1_code |= CLIP_CODE_N;
	}
	else if (y1 > clipRect.bottom)
	{
		point1_code |= CLIP_CODE_S;
	}

	// 判断端点的几种情况：
	if (point0_code == CLIP_CODE_C && point1_code == CLIP_CODE_C)		// 端点都在rect内，不被裁剪
	{
		return 1;

	}
	if (point0_code & point1_code)		// 端点都在某一边的rect外，完全裁减掉
	{
		return 0;
	}

	// 一个端点在外，一个端点在内，需要裁剪一部分. 裁剪得到新的端点; 或者是两个端点都在rect外，但是线段会经过裁剪区域（端点裁剪的情况）
	// 2.用新端点替换在rect外的端点

	// 1.计算出与垂直或者水平线的交点  (两种特殊情况，可以优化： 1.x1=x0;2.y1=y0)
	// 普通情况：m=(y1-y0)/(x1-x0)  y=m*(x-x1)+y1  x=(y-y1)/m+x1

	if (x0 == x1)		// 是一条垂直的线
	{
		if (point0_code != CLIP_CODE_C)		// point0在外面
		{
			//			outpoint_index = 0;
			//			inter_x = x0;
			if (point0_code&CLIP_CODE_N)		// 在north
			{
				y0 = clipRect.top;
			}
			else   // 在south
			{
				y0 = clipRect.bottom;
			}
		}
		if (point1_code != CLIP_CODE_C)		// point1在外面
		{
			//			outpoint_index = 1;
			//			inter_x = x1;
			if (point1_code&CLIP_CODE_N)
			{
				y1 = clipRect.top;
			}
			else
			{
				y1 = clipRect.bottom;
			}
		}
		return 1;
	} // end if  x0==x1

	if (y0 == y1)  //与水平线相交
	{
		if (point0_code != CLIP_CODE_C)		// point0在外面
		{
			//			outpoint_index = 0;
			//			inter_y = y0;
			if (point0_code&CLIP_CODE_W)		// 在west
			{
				x0 = clipRect.left;
			}
			else   // 在east
			{
				x0 = clipRect.right;
			}
		}
		if (point1_code != CLIP_CODE_C)		// point1在外面
		{
			//			outpoint_index = 1;
			//			inter_y = y1;
			if (point1_code&CLIP_CODE_W)
			{
				x1 = clipRect.left;
			}
			else
			{
				x1 = clipRect.right;
			}
		}
		return 1;
	} // end if y0==y1

	// 一般情况 
	float m = (y1 - y0)*1.0f / (x1 - x0);

	// 普通情况：m=(y1-y0)/(x1-x0)  y=m*(x-x1)+y1  x=(y-y1)/m+x1
	switch (point0_code)
	{
	case CLIP_CODE_N:
		y0 = clipRect.top;
		x0 = (y0 - y1) / m + x1;
		break;
	case CLIP_CODE_S:
		y0 = clipRect.bottom;
		x0 = (y0 - y1) / m + x1;
		break;
	case CLIP_CODE_W:
		x0 = clipRect.left;
		y0 = m * (x0 - x1) + y1;
		break;
	case CLIP_CODE_E:
		x0 = clipRect.right;
		y0 = m * (x0 - x1) + y1;
		break;
	case CLIP_CODE_NE:
		// 在N线上
		y0 = clipRect.top;		// 先判断交点是否在N线上
		x0 = (y0 - y1) / m + x1;

		if (x0<clipRect.left || x0>clipRect.right)// 交点在E线上
		{
			x0 = clipRect.right;
			y0 = m * (x0 - x1) + y1;
		}
		break;
	case CLIP_CODE_SE:
		// S线上
		y0 = clipRect.bottom;
		x0 = (y0 - y1) / m + x1;
		if (x0<clipRect.left || x0>clipRect.right)  // E线上
		{
			x0 = clipRect.right;
			y0 = m * (x0 - x1) + y1;
		}
		break;
	case CLIP_CODE_NW:
		// N线
		y0 = clipRect.top;
		x0 = (y0 - y1) / m + x1;
		if (x0<clipRect.left || x0>clipRect.right)
		{
			x0 = clipRect.left;
			y0 = m * (x0 - x1) + y1;
		}
		break;
	case CLIP_CODE_SW:
		// s line
		y0 = clipRect.bottom;
		x0 = (y0 - y1) / m + x1;

		if (x0<clipRect.left || x0>clipRect.right)
		{
			x0 = clipRect.left;
			y0 = m * (x0 - x1) + y1;
		}
		break;
	}

	// 普通情况：m=(y1-y0)/(x1-x0)  y=m*(x-x0)+y0  x=(y-y0)/m+x0
	switch (point1_code)
	{
	case CLIP_CODE_N:
		y1 = clipRect.top;
		x1 = (y1 - y0) / m + x0;
		break;
	case CLIP_CODE_S:
		y1 = clipRect.bottom;
		x1 = (y1 - y0) / m + x0;
		break;
	case CLIP_CODE_W:
		x1 = clipRect.left;
		y1 = m * (x1 - x0) + y0;
		break;
	case CLIP_CODE_E:
		x1 = clipRect.right;
		y1 = m * (x1 - x0) + y0;
		break;
	case CLIP_CODE_NE:
		// 在N线上
		y1 = clipRect.top;		// 先判断交点是否在N线上
		x1 = (y1 - y0) / m + x0;
		if (x1<clipRect.left || x1>clipRect.right)// 交点在E线上
		{
			x1 = clipRect.right;
			y1 = m * (x1 - x0) + y0;
		}
		break;
	case CLIP_CODE_SE:
		// S线上
		y1 = clipRect.bottom;
		x1 = (y1 - y0) / m + x0;
		if (x1<clipRect.left || x1>clipRect.right)  // E线上
		{
			x1 = clipRect.right;
			y1 = m * (x1 - x0) + y0;
		}
		break;
	case CLIP_CODE_NW:
		// N线
		y1 = clipRect.top;
		x1 = (y1 - y0) / m + x0;
		if (x1<clipRect.left || x1>clipRect.right)
		{
			x1 = clipRect.left;
			y1 = m * (x1 - x0) + y0;
		}
		break;
	case CLIP_CODE_SW:
		// on S line
		y1 = clipRect.bottom;
		x1 = (y1 - y0) / m + x0;

		if (x1<clipRect.left || x1>clipRect.right)
		{
			x1 = clipRect.left;
			y1 = m * (x1 - x0) + y0;
		}
		break;
	}
	// 最后再check一下端点是否正确. 有错误就返回0
	if (x0<clipRect.left || x0>clipRect.right
		|| x1<clipRect.left || y1>clipRect.right
		|| y0<clipRect.top || y0>clipRect.bottom
		|| y1<clipRect.top || y1>clipRect.bottom)
	{
		//		printf("clip (%d,%d),(%d,%d)", x0, y0, x1, y1);
		return 0;
	}
	return 1;
}

void Draw_Clip_Line8(RECT clipRect, int x0, int y0, int x1, int y1, UCHAR color, UCHAR *vb_start, int lpitch)
{
	int cx0 = x0, cy0 = y0, cx1 = x1, cy1 = y1;
	if (Clip_Line(clipRect, cx0, cy0, cx1, cy1))
	{
		Draw_Line8(cx0, cy0, cx1, cy1, color, vb_start, lpitch);
	}
}

void Test_DrawLine_Init()
{
}
void Test_DrawLine_Main()
{
	DDraw_Fill_Surface(lpddsprimary, 0);
	DDRAW_INIT_STRUCT(ddsd);
	lpddsprimary->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);
	int x0 = rand() % SCREEN_WIDTH;
	int y0 = rand() % SCREEN_HEIGHT;
	int x1 = rand() % SCREEN_WIDTH;
	int y1 = rand() % SCREEN_HEIGHT;
	if (ddpixelformat.dwRGBBitCount == 8)
	{
		Draw_Line8(x0, y0, x1, y1, rand() % 255, (UCHAR *)ddsd.lpSurface, ddsd.lPitch);
	}
	else if (ddpixelformat.dwRGBBitCount == 16)
	{
		Draw_Line16(x0, y0, x1, y1, RandomRGB16BIT565(), (USHORT *)ddsd.lpSurface, ddsd.lPitch);
	}
	else
	{

		Draw_Line32(x0, y0, x1, y1, RandomRGBA32(), (UINT *)ddsd.lpSurface, ddsd.lPitch);
	}
	lpddsprimary->Unlock(NULL);

}

void Test_DrawClipLine_Main()
{
	//	DDraw_Fill_Surface(lpddsprimary, 255);
	DDRAW_INIT_STRUCT(ddsd);
	lpddsprimary->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);
	int x0 = rand() % SCREEN_WIDTH;
	int y0 = rand() % SCREEN_HEIGHT;

	int x1 = rand() % SCREEN_WIDTH;
	int y1 = rand() % SCREEN_HEIGHT;
	RECT clip = { 50,50,400,400 };
	Draw_Clip_Line8(clip, x0, y0, x1, y1, rand() % 255, (UCHAR*)ddsd.lpSurface, ddsd.lPitch);
	lpddsprimary->Unlock(NULL);

}
#pragma endregion

#pragma region chapter8,demo_8_3, 多边形的绘制
const int num_poly = 64;
POLYGON2D airships[num_poly];
RECT default_clip_rect = { 0,0,SCREEN_WIDTH - 1,SCREEN_HEIGHT - 1 };
VERTEX2DF ship_vertexs[11] = { -17,0,-25,-10,-2,-10,-2,-30,2,-30,2,-28,20,-20,2,-12,2,-10,25,-10,17,0 };
//VERTEX2DI ship_vertexs[11] = { -17,0,-25,-10,-2,-10,-2,-30,2,-30,2,-28,20,-20,2,-12,2,-10,25,-10,17,0 };

int Draw_Polygon2D(RECT clipRect, POLYGON2D_PTR poly, UCHAR *vbuffer, int lpitch)
{
	if (poly->state)
	{
		int i = 0;
		for (; i < poly->num_verts - 1; ++i)
		{
			Draw_Clip_Line8(clipRect, poly->vlist[i].x + poly->x0, poly->vlist[i].y + poly->y0, poly->vlist[i + 1].x + poly->x0, poly->vlist[i + 1].y + poly->y0, poly->color, vbuffer, lpitch);
			//			Draw_Line8(poly->vlist[i].x + poly->x0, poly->vlist[i].y + poly->y0, poly->vlist[i + 1].x + poly->x0, poly->vlist[i + 1].y + poly->y0, poly->color, vbuffer, lpitch);
		}
		Draw_Clip_Line8(clipRect, poly->vlist[i].x + poly->x0, poly->vlist[i].y + poly->y0, poly->vlist[0].x + poly->x0, poly->vlist[0].y + poly->y0, poly->color, vbuffer, lpitch);
		//		Draw_Line8(poly->vlist[i].x + poly->x0, poly->vlist[i].y + poly->y0, poly->vlist[0].x + poly->x0, poly->vlist[0].y + poly->y0, poly->color, vbuffer, lpitch);
		return 1;
	}
	return 0;
}
void Test_Draw_Polygon_Init()
{
	for (int i = 0; i < num_poly; ++i)
	{
		airships[i].color = rand() % 255;
		airships[i].num_verts = 11;
		airships[i].state = 0;
		airships[i].x0 = rand() % SCREEN_WIDTH;
		airships[i].y0 = rand() % SCREEN_HEIGHT;
		airships[i].xv = 1 + rand() % 6;
		airships[i].yv = 0;
		airships[i].vlist = new VERTEX2DF[airships[i].num_verts];
		for (int k = 0; k < airships[i].num_verts; ++k)
		{
			airships[i].vlist[k] = ship_vertexs[k];
		}
	}

}

void Test_Draw_Mouse_Line(UCHAR * buffer, int lpitch)
{
	if (mouseDown)
	{
		Draw_Clip_Line8(default_clip_rect, lastMousePoint.x, lastMousePoint.y, mousePoint.x, mousePoint.y, rand() % 256, buffer, lpitch);
		//		Plot8(mousePoint.x, mousePoint.y, rand() % 256, (UCHAR*)ddsd.lpSurface, ddsd.lPitch);
	}
}

// 出现一个转成小点的问题。原因是，顶点用int表示，由于float->int的误差，导致最后顶点都变成了0：。
void Test_Draw_Polygon2D_Main()
{
	//	DDraw_Fill_Surface(lpddsback, 0);

	DDRAW_INIT_STRUCT(ddsd);

	lpddsback->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);

	for (int i = 0; i < num_poly; ++i)
	{
		airships[i].state = 1;
		Draw_Polygon2D(default_clip_rect, &airships[i], (UCHAR*)ddsd.lpSurface, ddsd.lPitch);
		//		Rotate_Polygon2d(&airships[i], 5);
		//		Rotate_Polygon2d_Fast(&airships[i], 5);
		Translate_Polygon2d(&airships[i], airships[i].xv, airships[i].yv);
		//		Scale_Polygon2d(&airships[i], 1.1f, 1.1f);
		if (airships[i].x0 > SCREEN_WIDTH)
		{
			airships[i].x0 = 0;
		}

	}
	//	Test_Draw_Mouse_Line((UCHAR*)ddsd.lpSurface, ddsd.lPitch);

	lpddsback->Unlock(NULL);

	while (lpddsprimary->Flip(NULL, DDFLIP_WAIT));

}
void Test_Draw_Mouse_Pixel_Main()
{
	if (KEYDOWN(VK_SPACE))
	{
		DDraw_Fill_Surface(lpddsback, 0);
	}
	DDRAW_INIT_STRUCT(ddsd);
	lpddsback->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);

	if (mouseDown)
	{
		Draw_Line8(lastMousePoint.x, lastMousePoint.y, mousePoint.x, mousePoint.y, 255, (UCHAR*)ddsd.lpSurface, ddsd.lPitch);
		//		Draw_Clip_Line8(default_clip_rect, lastMousePoint.x, lastMousePoint.y, mousePoint.x, mousePoint.y, 255, (UCHAR*)ddsd.lpSurface, ddsd.lPitch);
		//		Plot8(mousePoint.x, mousePoint.y, rand() % 256, (UCHAR*)ddsd.lpSurface, ddsd.lPitch);
	}

	lpddsback->Unlock(NULL);

	while (lpddsprimary->Flip(NULL, DDFLIP_WAIT));

}

void Test_Draw_Mouse_Pixel_Blt_Main()
{
	if (KEYDOWN(VK_SPACE))
	{
		DDraw_Fill_Surface(lpddsback, 0);
	}
	DDRAW_INIT_STRUCT(ddsd);
	lpddsback->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);

	if (mouseDown)
	{
		Draw_Line8(lastMousePoint.x, lastMousePoint.y, mousePoint.x, mousePoint.y, 255, (UCHAR*)ddsd.lpSurface, ddsd.lPitch);
		//		Draw_Clip_Line8(default_clip_rect, lastMousePoint.x, lastMousePoint.y, mousePoint.x, mousePoint.y, 255, (UCHAR*)ddsd.lpSurface, ddsd.lPitch);
		//		Plot8(mousePoint.x, mousePoint.y, rand() % 256, (UCHAR*)ddsd.lpSurface, ddsd.lPitch);
	}

	lpddsback->Unlock(NULL);

	lpddsprimary->Blt(NULL, lpddsback, NULL, DDBLT_WAIT, NULL);

}

#pragma endregion

#pragma region chapter8,demo_8_6,矩阵运算

POLYGON2D ship;
void Test_matrix_Init()
{
	ship.color = rand() % 256;
	ship.state = 1;
	ship.x0 = SCREEN_WIDTH * 0.5;
	ship.y0 = SCREEN_HEIGHT * 0.5;
	ship.xv = 0;
	ship.yv = 0;
	ship.num_verts = 11;
	ship.vlist = ship_vertexs;
}

void Test_matrix_Main()
{
	DDraw_Fill_Surface(lpddsback, 0);
	DDRAW_INIT_STRUCT(ddsd);
	lpddsback->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);

	Draw_Polygon2D(default_clip_rect, &ship, (UCHAR*)ddsd.lpSurface, ddsd.lPitch);

	if (KEYDOWN('A'))
	{
		Scale_Polygon2d_Mat(&ship, 1.1, 1.1);
	}
	else
		if (KEYDOWN('S'))
		{
			Scale_Polygon2d_Mat(&ship, 0.9, 0.9);
		}

	if (KEYDOWN('Z'))
	{
		Rotate_Polygon2d_Mat(&ship, 5);
	}
	else
		if (KEYDOWN('X'))
		{
			Rotate_Polygon2d_Mat(&ship, -5);
		}

	if (KEYDOWN(VK_UP))
	{
		Translate_Polygon2d_Mat(&ship, 0, -5);
	}
	else if (KEYDOWN(VK_DOWN))
	{
		Translate_Polygon2d_Mat(&ship, 0, 5);
	}
	else if (KEYDOWN(VK_LEFT))
	{
		Translate_Polygon2d_Mat(&ship, -5, 0);
	}
	else if (KEYDOWN(VK_RIGHT))
	{
		Translate_Polygon2d_Mat(&ship, 5, 0);
	}

	Draw_Text_GDI_IN_DD("cycycycycycy", 10, 10, RGB(255, 255, 255), lpddsback);
	Draw_Text_GDI_IN_DD(buffer, 10, SCREEN_HEIGHT - 10, RGB(255, 255, 255), lpddsback);

	lpddsback->Unlock(NULL);

	while (lpddsprimary->Flip(NULL, DDFLIP_WAIT));
}
#pragma endregion

#pragma region chapter8,demo_8_7,填充多边形 （光栅化算法）
// 填充平底三角形 ,按照逆时针方向作为正方向发送顶点。函数内部调换为：p0是顶点，p1是left点，p2是right点
void Draw_Bottom_Tri(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR* dest_buffer, int mempitch)
{
	float dxy_right,
		dxy_left,
		xs, xe,
		height,
		delta_hegith;
	int tmp_x,
		tmp_y,
		right,
		left;

	UCHAR *dest_addr;

	// 移动顶点，令p0点是顶点,p1p2是底边
	// 并且，判断令p1点在左边，p2点在右边
	if (y1 != y0 && y1 != y2)
	{
		tmp_y = y0;
		tmp_x = x0;
		y0 = y1;
		x0 = x1;

		y1 = tmp_y;
		x1 = tmp_x;
	}
	else if (y2 != y0 && y2 != y1)
	{
		tmp_y = y0;
		tmp_x = x0;

		y0 = y2;
		x0 = x2;

		y2 = tmp_y;
		x2 = tmp_x;
	}
	// 交换p1点和p2点 （y值是一样的，不用换）
	if (x1 > x2)
	{
		tmp_x = x1;
		x1 = x2;

		x2 = tmp_x;
	}

	height = y1 - y0;

	dxy_left = (x1 - x0)*1.0f / height;
	dxy_right = (x2 - x0)*1.0f / height;

	xs = x0, xe = x0;

	// 查看clip范围
	if (y0 < clipRect->top)	// 顶点在clip框外，获取新的height和xs xe
	{
		delta_hegith = clipRect->top - y0;

		xs += dxy_left * delta_hegith;		// 新起点和终点
		xe += dxy_right * delta_hegith;

		y0 = clipRect->top;
	}


	if (y1 > clipRect->bottom)				// 底边出了clip框
	{
		y1 = y2 = clipRect->bottom;
	}

	dest_addr = dest_buffer + y0 * mempitch;		// 计算内存的起点位置

	// 开始clip水平扫描线，和画线

	// 如果这个时候，点都在clip框内：
	if (x0 > clipRect->left&&x0<clipRect->right&&
		x1>clipRect->left&&x1<clipRect->right&&
		x2>clipRect->left&&x2 < clipRect->right)
	{
		for (int i = y0; i <= y2; ++i, dest_addr += mempitch)		// !注意，是i<=y2 。之前没有加=号:(
		{
			memset((UCHAR*)dest_addr + (unsigned int)xs, color, (unsigned int)(xe - xs + 1));

			xs += dxy_left;
			xe += dxy_right;
		}
	}
	else  // 有的点不在clip框内
	{
		for (int i = y0; i <= y2; ++i, dest_addr += mempitch)
		{
			left = xs, right = xe;

			xs += dxy_left;
			xe += dxy_right;

			if (right > clipRect->right)
			{
				right = clipRect->right;
				if (left > clipRect->right)		// 端点在clip框外，跳过
				{
					//					dest_addr += mempitch;
					continue;
				}
			}
			if (left < clipRect->left)
			{
				left = clipRect->left;
				if (right < clipRect->left)		// 端点在clip框外，跳过
				{
					//					dest_addr += mempitch;
					continue;
				}
			}
			memset((UCHAR*)dest_addr + (unsigned int)left, color, (unsigned int)(right - left + 1));

			//			dest_addr += mempitch;		// for 循环中加过了
		}

	}
}

// 填充平顶三角形，按照逆时针方向作为正方向发送顶点,函数内部调换为：p0是低点，p1是right点，p2是left点
void Draw_Top_Tri(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR* dest_buffer, int mempitch)
{
	float dxy_right,
		dxy_left,
		xs, xe,
		height,
		delta_hegith;
	int tmp_x,
		tmp_y,
		right,
		left;

	UCHAR *dest_addr;

	if (y1 != y2 && y1 != y0)		// p1是低点
	{
		tmp_x = x1;
		tmp_y = y1;
		x1 = x0;
		y1 = y0;

		x0 = tmp_x;
		y0 = tmp_y;
	}
	else if (y2 != y1 && y2 != y0)
	{
		tmp_x = x2;
		tmp_y = y2;
		x2 = x0;
		y2 = y0;

		x0 = tmp_x;
		y0 = tmp_y;
	}

	// 调换是的p1在右边，p2在左边,y值一样，不用换
	if (x1 < x2)
	{
		tmp_x = x2;
		x2 = x1;

		x1 = tmp_x;
	}

	// 填充平底三角形
	height = y2 - y0;
	dxy_left = (x2 - x0)*1.0f / height;
	dxy_right = (x1 - x0)*1.0f / height;
	xs = (float)x0, xe = (float)x0;

	if (y0 > clipRect->bottom)
	{
		delta_hegith = y0 - clipRect->bottom;

		xs -= dxy_left * delta_hegith;
		xe -= dxy_right * delta_hegith;

		y0 = clipRect->bottom;
	}
	if (y1 < clipRect->top)
	{
		y1 = y2 = clipRect->top;
	}

	// 计算起点行
	dest_addr = dest_buffer + y0 * mempitch;

	// 点都在clip框内
	if (x0 > clipRect->left&&x0<clipRect->right&&
		x1>clipRect->left&&x1<clipRect->right&&
		x2>clipRect->left&&x2 < clipRect->right)
	{
		for (int i = y0; i >= y1; --i, dest_addr -= mempitch)
		{

			memset((UCHAR*)dest_addr + (unsigned int)xs, color, (unsigned int)(xe - xs + 1));
			xs -= dxy_left;
			xe -= dxy_right;
		}
	}
	else	// 有的点被clip掉了
	{
		for (int i = y0; i >= y1; --i, dest_addr -= mempitch)
		{
			left = xs;
			right = xe;

			xs -= dxy_left;
			xe -= dxy_right;

			if (left < clipRect->left)
			{
				left = clipRect->left;
				if (right < clipRect->left)
					continue;
			}
			if (right > clipRect->right)
			{
				right = clipRect->right;
				if (left > clipRect->right)
					continue;
			}

			memset((UCHAR*)dest_addr + (unsigned int)left, color, (unsigned int)(right - left + 1));
		}

	}

	//	for (int i = y1; i > y0; --i)
	//	{
	//		Draw_Clip_Line8(default_clip_rect, xs + 0.5, i, xe + 0.5, i, color, dest_buffer, mempitch);
	//		xs -= dxy_left;
	//		xe -= dxy_right;
	//	}
}

// 任意一个三角形。顶点顺序逆时针发送。内部调整为：p0为顶点，p1为左边点，p2为右边点
// 使用分割为两个三角形的方式进行填充
void Draw_Triangle_2D(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2,
	int color, UCHAR *dest_buffer, int mempitch)
{
	// 按y值排序 y0<y1<y2
	int tmp;
	// 是垂直的线或者是水平的线，返回
	if ((x0 == x1 && x1 == x2) || (y0 == y1 && y1 == y2))
	{
		return;
	}

	if (y1 < y2&&y1 < y0)		// y1是顶点,与0点交换
	{
		tmp = y0;
		y0 = y1;
		y1 = tmp;

		tmp = x0;
		x0 = x1;
		x1 = tmp;
		//		SwapInt(y1, y0);
		//		SwapInt(x1, x0);
	}
	else if (y2 < y1&&y2 < y0)	// y2是顶点，与0点交换
	{
		tmp = y0;
		y0 = y2;
		y2 = tmp;

		tmp = x0;
		x0 = x2;
		x2 = tmp;
		//		SwapInt(y2, y0);
		//		SwapInt(x2, x0);
	}

	if (y2 < y1)		// 按y值排序
	{
		tmp = y1;
		y1 = y2;
		y2 = tmp;

		tmp = x1;
		x1 = x2;
		x2 = tmp;
	}


	// 检查是否全部在clip框外
	if (y0 > clipRect->bottom ||												// 在clip框下面
		(y1 < clipRect->top&&y2 < clipRect->top) ||							// 在clip框上面
		(x0 < clipRect->left&&x1 < clipRect->left&&x2 < clipRect->left) ||			// 在clip框左面
		(x0 > clipRect->right&&x1 > clipRect->right&&x2 > clipRect->right)		// 在clip框右面
		)
	{
		return;
	}

	if (y0 == y1 || y0 == y2)		// 是个平顶三角形
	{
		Draw_Top_Tri(clipRect, x0, y0, x1, y1, x2, y2, color, dest_buffer, mempitch);
		return;
	}

	if (y1 == y2)		// 是个平底三角形
	{
		Draw_Bottom_Tri(clipRect, x0, y0, x1, y1, x2, y2, color, dest_buffer, mempitch);
		return;
	}

	// 一般三角形，分割成两个三角形
	// 设Ptemp为临时点，已经知道，Yt=y1,根据相似三角形得：
	// Xt=(x0-x2)*(y1-y2)/(y0-y2)+x2
	float new_x = x2 + (int)((float)(x0 - x2)*(float)(y1 - y2) / (float)(y0 - y2) + 0.5);

	Draw_Bottom_Tri(clipRect, x0, y0, x1, y1, new_x, y1, color, dest_buffer, mempitch);	// P0,P1,Pt
	Draw_Top_Tri(clipRect, x2, y2, new_x, y1, x1, y1, color, dest_buffer, mempitch);			// P2,Pt,P1
	return;
}
VERTEX2DF triPoints[] = { 300,400,300,100,100,200 };
void Test_DrawTriangle_Main()
{
	//	if (!KEYDOWN(VK_SPACE))
	//	{
	//		return;
	//	}
	DDraw_Fill_Surface(lpddsback, 0);
	DDRAW_INIT_STRUCT(ddsd);
	lpddsback->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);

	RECT clip = { 270,190,370,290 };
	//	int x0 = 200; 
	//	int y0 = 240;
	//	int x1 = 600;
	//	int y1 = 350;
	//	int x2 = 420;
	//	int y2 = 120;
	int x0 = rand() % SCREEN_WIDTH - 1;
	int y0 = rand() % SCREEN_HEIGHT - 1;
	int x1 = rand() % SCREEN_WIDTH - 1;
	int y1 = rand() % SCREEN_HEIGHT - 1;
	int x2 = rand() % SCREEN_WIDTH - 1;
	int y2 = rand() % SCREEN_HEIGHT - 1;
	Draw_Triangle_2D(&clip, x0, y0, x1, y1, x2, y2, 255, (UCHAR*)ddsd.lpSurface, ddsd.lPitch);


	POLYGON2D clippoly;
	clippoly.state = 1;
	clippoly.color = 120;
	clippoly.num_verts = 4;
	clippoly.x0 = 0;
	clippoly.y0 = 0;
	clippoly.vlist = new VERTEX2DF[4];
	clippoly.vlist[0].x = clip.left;
	clippoly.vlist[0].y = clip.top;
	clippoly.vlist[1].x = clip.right;
	clippoly.vlist[1].y = clip.top;
	clippoly.vlist[2].x = clip.right;
	clippoly.vlist[2].y = clip.bottom;
	clippoly.vlist[3].x = clip.left;
	clippoly.vlist[3].y = clip.bottom;
	Draw_Polygon2D(default_clip_rect, &clippoly, (UCHAR*)ddsd.lpSurface, ddsd.lPitch);

	POLYGON2D poly;
	poly.state = 1;
	poly.color = 220;
	poly.num_verts = 3;
	poly.x0 = 0;
	poly.y0 = 0;
	poly.vlist = new VERTEX2DF[3];
	poly.vlist[0].x = x0;
	poly.vlist[0].y = y0;
	poly.vlist[1].x = x1;
	poly.vlist[1].y = y1;
	poly.vlist[2].x = x2;
	poly.vlist[2].y = y2;

	Draw_Polygon2D(default_clip_rect, &poly, (UCHAR*)ddsd.lpSurface, ddsd.lPitch);
	lpddsback->Unlock(NULL);

	while (lpddsprimary->Flip(NULL, DDFLIP_WAIT));

}

#pragma endregion demo_8_7

#pragma region demo_8_7/8,定点数函数

void Draw_Top_TriFP(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR *dest_buffer, int mempitch)
{
	FIXPOINT dxy_right,
		dxy_left,
		xs, xe;
	int height,
		delta_hegith;
	int tmp_x,
		tmp_y,
		right,
		left;

	UCHAR *dest_addr;

	// 垂直的线，退出
	if (y0 == y2 && y1 == y2)
	{
		return;
	}

	if (y1 != y2 && y1 != y0)		// p1是低点
	{
		tmp_x = x1;
		tmp_y = y1;
		x1 = x0;
		y1 = y0;

		x0 = tmp_x;
		y0 = tmp_y;
	}
	else if (y2 != y1 && y2 != y0)
	{
		tmp_x = x2;
		tmp_y = y2;
		x2 = x0;
		y2 = y0;

		x0 = tmp_x;
		y0 = tmp_y;
	}

	// 调换是的p1在右边，p2在左边,y值一样，不用换
	if (x1 < x2)
	{
		tmp_x = x2;
		x2 = x1;

		x1 = tmp_x;
	}

	// 填充平底三角形
	height = y2 - y0;
	dxy_left = ((x2 - x0) << FIXP16_SHIFT) / height;
	dxy_right = ((x1 - x0) << FIXP16_SHIFT) / height;
	xs = x0 << FIXP16_SHIFT;
	xe = x0 << FIXP16_SHIFT;

	if (y0 > clipRect->bottom)
	{
		delta_hegith = y0 - clipRect->bottom;

		xs -= dxy_left * delta_hegith;
		xe -= dxy_right * delta_hegith;

		y0 = clipRect->bottom;
	}
	if (y1 < clipRect->top)
	{
		y1 = y2 = clipRect->top;
	}

	// 计算起点行
	dest_addr = dest_buffer + y0 * mempitch;

	// 点都在clip框内
	if (x0 > clipRect->left&&x0<clipRect->right&&
		x1>clipRect->left&&x1<clipRect->right&&
		x2>clipRect->left&&x2 < clipRect->right)
	{
		for (int i = y0; i >= y1; --i, dest_addr -= mempitch)
		{

			// xs和se都是定点数，要先转回成整数
			memset((UCHAR*)dest_addr + ((xs + FIXP16_ROUND_UP) >> FIXP16_SHIFT), color, (((xe - xs + FIXP16_ROUND_UP) >> FIXP16_SHIFT) + 1));
			xs -= dxy_left;
			xe -= dxy_right;
		}
	}
	else	// 有的点被clip掉了
	{
		for (int i = y0; i >= y1; --i, dest_addr -= mempitch)
		{
			//			left = xs;
			//			right = xe;
			left = FIXP_2_INT(xs);
			right = FIXP_2_INT(xe);

			xs -= dxy_left;
			xe -= dxy_right;

			if (left < clipRect->left)
			{
				left = clipRect->left;
				if (right < clipRect->left)
					continue;
			}
			if (right > clipRect->right)
			{
				right = clipRect->right;
				if (left > clipRect->right)
					continue;
			}

			memset((UCHAR*)dest_addr + (unsigned int)left, color, (unsigned int)(right - left + 1));
		}

	}

}

void Draw_Bottom_TriFP(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR *dest_buffer, int mempitch)
{
	FIXPOINT dxy_right, dxy_left,
		xs, xe;
	//	int dxy_right,
	//		dxy_left,
	//	int xs, xe,
	int height, delta_hegith;
	int tmp_x,
		tmp_y,
		right,
		left;

	UCHAR *dest_addr;

	if (y0 == y1 && y1 == y2)
	{
		return;
	}

	// 移动顶点，令p0点是顶点,p1p2是底边
	// 并且，判断令p1点在左边，p2点在右边
	if (y1 != y0 && y1 != y2)
	{
		tmp_y = y0;
		tmp_x = x0;
		y0 = y1;
		x0 = x1;

		y1 = tmp_y;
		x1 = tmp_x;
	}
	else if (y2 != y0 && y2 != y1)
	{
		tmp_y = y0;
		tmp_x = x0;

		y0 = y2;
		x0 = x2;

		y2 = tmp_y;
		x2 = tmp_x;
	}
	// 交换p1点和p2点 （y值是一样的，不用换）
	if (x1 > x2)
	{
		tmp_x = x1;
		x1 = x2;

		x2 = tmp_x;
	}

	height = y1 - y0;

	dxy_left = ((x1 - x0) << FIXP16_SHIFT) / height;
	dxy_right = ((x2 - x0) << FIXP16_SHIFT) / height;

	xs = INT_2_FIXP(x0);
	xe = INT_2_FIXP(x0);

	// 查看clip范围
	if (y0 < clipRect->top)	// 顶点在clip框外，获取新的height和xs xe
	{
		delta_hegith = clipRect->top - y0;

		xs += dxy_left * delta_hegith;		// 新起点和终点
		xe += dxy_right * delta_hegith;

		y0 = clipRect->top;
	}


	if (y1 > clipRect->bottom)				// 底边出了clip框
	{
		y1 = y2 = clipRect->bottom;
	}

	dest_addr = dest_buffer + y0 * mempitch;		// 计算内存的起点位置

	// 开始clip水平扫描线，和画线

	// 如果这个时候，点都在clip框内：
	if (x0 > clipRect->left&&x0<clipRect->right&&
		x1>clipRect->left&&x1<clipRect->right&&
		x2>clipRect->left&&x2 < clipRect->right)
	{
		for (int i = y0; i <= y2; ++i, dest_addr += mempitch)		// !注意，是i<=y2 。之前没有加=号:(
		{
			memset((UCHAR*)dest_addr + ((xs + FIXP16_ROUND_UP) >> FIXP16_SHIFT), color, (((xe - xs) + FIXP16_ROUND_UP) >> FIXP16_SHIFT) + 1);

			xs += dxy_left;
			xe += dxy_right;
		}
	}
	else  // 有的点不在clip框内
	{
		for (int i = y0; i <= y2; ++i, dest_addr += mempitch)
		{
			left = FIXP_2_INT(xs);
			right = FIXP_2_INT(xe);

			xs += dxy_left;
			xe += dxy_right;

			if (right > clipRect->right)
			{
				right = clipRect->right;
				if (left > clipRect->right)		// 端点在clip框外，跳过
				{
					//					dest_addr += mempitch;
					continue;
				}
			}
			if (left < clipRect->left)
			{
				left = clipRect->left;
				if (right < clipRect->left)		// 端点在clip框外，跳过
				{
					//					dest_addr += mempitch;
					continue;
				}
			}
			memset((UCHAR*)dest_addr + left, color, (right - left + 1));

			//			dest_addr += mempitch;		// for 循环中加过了
		}

	}
}

void Draw_TriangleFP_2D(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR *dest_buffer, int mempitch)
{
	// 按y值排序 y0<y1<y2
	int tmp;
	// 是垂直的线或者是水平的线，返回
	if ((x0 == x1 && x1 == x2) || (y0 == y1 && y1 == y2))
	{
		return;
	}

	if (y1 < y2&&y1 < y0)		// y1是顶点,与0点交换
	{
		tmp = y0;
		y0 = y1;
		y1 = tmp;

		tmp = x0;
		x0 = x1;
		x1 = tmp;
		//		SwapInt(y1, y0);
		//		SwapInt(x1, x0);
	}
	else if (y2 < y1&&y2 < y0)	// y2是顶点，与0点交换
	{
		tmp = y0;
		y0 = y2;
		y2 = tmp;

		tmp = x0;
		x0 = x2;
		x2 = tmp;
		//		SwapInt(y2, y0);
		//		SwapInt(x2, x0);
	}

	if (y2 < y1)		// 按y值排序
	{
		tmp = y1;
		y1 = y2;
		y2 = tmp;

		tmp = x1;
		x1 = x2;
		x2 = tmp;
	}


	// 检查是否全部在clip框外
	if (y0 > clipRect->bottom ||												// 在clip框下面
		(y1 < clipRect->top&&y2 < clipRect->top) ||							// 在clip框上面
		(x0 < clipRect->left&&x1 < clipRect->left&&x2 < clipRect->left) ||			// 在clip框左面
		(x0 > clipRect->right&&x1 > clipRect->right&&x2 > clipRect->right)		// 在clip框右面
		)
	{
		return;
	}

	if (y0 == y1 || y0 == y2)		// 是个平顶三角形
	{
		Draw_Top_TriFP(clipRect, x0, y0, x1, y1, x2, y2, color, dest_buffer, mempitch);
		return;
	}

	if (y1 == y2)		// 是个平底三角形
	{
		Draw_Bottom_TriFP(clipRect, x0, y0, x1, y1, x2, y2, color, dest_buffer, mempitch);
		return;
	}

	// 一般三角形，分割成两个三角形
	// 设Ptemp为临时点，已经知道，Yt=y1,根据相似三角形得：
	// Xt=(x0-x2)*(y1-y2)/(y0-y2)+x2
	float new_x = x2 + (int)((float)(x0 - x2)*(float)(y1 - y2) / (float)(y0 - y2) + 0.5);

	Draw_Bottom_TriFP(clipRect, x0, y0, x1, y1, new_x, y1, color, dest_buffer, mempitch);	// P0,P1,Pt
	Draw_Top_TriFP(clipRect, x2, y2, new_x, y1, x1, y1, color, dest_buffer, mempitch);			// P2,Pt,P1
	return;

}

inline void Draw_QuadFP_2D(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, int color, UCHAR *dest_buffer, int mempitch)
{
	Draw_TriangleFP_2D(clipRect, x0, y0, x1, y1, x3, y3, color, dest_buffer, mempitch);
	Draw_TriangleFP_2D(clipRect, x1, y1, x2, y2, x3, y3, color, dest_buffer, mempitch);
}


#pragma endregion

#pragma region demo_8_8,四边形的填充

// 四边形的顶点0,1,2,3顺时针传入，那么两个被分割的三角形为： <0,1,3> <1,2,3>
inline void Draw_Quad_2D(PRECT p_clipRect, int x0, int y0, int x1, int y1,
	int x2, int y2, int x3, int y3,
	int color, UCHAR *dest_buffer, int mempitch)
{

	Draw_Triangle_2D(p_clipRect, x0, y0, x1, y1, x3, y3, color, dest_buffer, mempitch);
	Draw_Triangle_2D(p_clipRect, x1, y1, x2, y2, x3, y3, color, dest_buffer, mempitch);

}

void Test_DrawQuad_Main()
{
	//	if (!KEYDOWN(VK_SPACE))
	//	{
	//		return;
	//	}
	DDraw_Fill_Surface(lpddsback, 0);
	DDRAW_INIT_STRUCT(ddsd);
	lpddsback->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);

	RECT clip = { 100,100,400,400 };
	//	int x0 = rand() % SCREEN_WIDTH;
	//	int y0 = rand() % SCREEN_HEIGHT;
	//	int width = 10 + rand() % 400;
	//	int height = 10 + rand() % 400;
	int x0 = 100;
	int y0 = 100;
	int width = 111;
	int height = 10 + 100;
	//	Draw_Quad_2D(&default_clip_rect, x0, y0, x0 + width, y0, x0 + width, y0 + height, x0, y0 + height, 255, (UCHAR*)ddsd.lpSurface, ddsd.lPitch);
	Draw_QuadFP_2D(&default_clip_rect, x0, y0, x0 + width, y0, x0 + width, y0 + height, x0, y0 + height, 255, (UCHAR*)ddsd.lpSurface, ddsd.lPitch);

	lpddsback->Unlock(NULL);

	while (lpddsprimary->Flip(NULL, DDFLIP_WAIT));

}

#pragma endregion

#pragma region demo_8_9,多边形的光栅化

// 填充多边形
void Draw_Filled_Polygon2D(PRECT clipRect, POLYGON2D_PTR poly, UCHAR *vbuffer, int mempitch)
{
	int startIndex, startY;
	int endIndex, endY;

	// 找到起点index
	startIndex = 0;
	startY = poly->vlist[0].y + poly->y0;;

	// 找到了最大的y值
	endIndex = 0;
	endY = poly->vlist[0].y + poly->y0;

	int tmp;


	for (int i = 1; i < poly->num_verts; ++i)
	{
		tmp = poly->vlist[i].y + poly->y0;
		if (tmp < startY)
		{
			startIndex = i;
			startY = tmp;
		}

		if (tmp > endY)
		{
			endIndex = i;
			endY = tmp;
		}
	}

	int edgeCount = poly->num_verts;

	// 左右两条扫描线，但是现在先不区分左右。
	int start1Index = startIndex;
	int start2Index = startIndex;
	// 1线和2线上的点，作为起点和终点
	float x1, x2;
	int y1end;
	int y2end;
	float x1end, x2end;

	int x1Error = 0, x2Error = 0;

	x1 = x2 = poly->vlist[startIndex].x + poly->x0;
	y1end = y2end = (poly->vlist[startIndex].y + poly->y0);

	UCHAR *dest_buff;
	dest_buff = vbuffer + startY * mempitch;

	// 试着画一条扫描线：
	float dxy1, dxy2;		// 每移动单位y,x的变换距离

	// 开始画扫描线
	while (edgeCount > 0 )
	{
		// 找1线，直到找到合适的
		while (edgeCount > 0 && (x1Error || startY >= y1end))
		{
			int pointIndex = start1Index - 1;
			if (pointIndex < 0)
			{
				pointIndex = poly->num_verts - 1;
			}
			edgeCount--;
			float height = poly->vlist[pointIndex].y - poly->vlist[start1Index].y;
			if (height == 0)	// 是水平线,y值一样
			{
				x1 = poly->vlist[pointIndex].x + poly->x0;
				start1Index = pointIndex;
				continue;		// 继续寻找合适的水平线
			}
			else  // 形成了一条斜线
			{
				dxy1 = (float)((poly->vlist[pointIndex].x - poly->vlist[start1Index].x)) / height;

				x1 = poly->vlist[start1Index].x + poly->x0;		// 将x1置回到线的起点
				y1end = poly->vlist[pointIndex].y + poly->y0;
				x1end = poly->vlist[pointIndex].x + poly->x0;

				start1Index = pointIndex;
				break;	// 找到合适的线了，break
			}
		}

		// 找到2线，直到找到合适的
		while (edgeCount > 0 && (x2Error || startY >= y2end))
		{
			int pointIndex = start2Index + 1;
			if (pointIndex >= poly->num_verts)
			{
				pointIndex = 0;
			}
			edgeCount--;
			float height = poly->vlist[pointIndex].y - poly->vlist[start2Index].y;
			if (height == 0)	// 是水平线
			{
				x2 = poly->vlist[pointIndex].x + poly->x0;
				start2Index = pointIndex;
				continue;		// 继续寻找合适的水平线
			}
			else  // 形成了一条斜线
			{
				dxy2 = ((float)(poly->vlist[pointIndex].x - poly->vlist[start2Index].x)) / height;

				x2 = poly->vlist[start2Index].x + poly->x0;
				y2end = poly->vlist[pointIndex].y + poly->y0;
				x2end = poly->vlist[pointIndex].x + poly->x0;

				start2Index = pointIndex;
				break;	// 找到合适的线了，break
			}
		}

		// 要画线啦！

		x1Error = 0;
		x2Error = 0;
		// 只要不到拐点，就一直向下画线
		// 注意，不要等于号。 等于线当做下一边的开始。(最后的结束的交点会不会少一条线？)
		while (startY < y1end && startY < y2end)
		{
			if (x1 < x2)
			{
				memset(dest_buff + (int)x1, poly->color, (int)(x2 - x1 + 1));
			}
			else
			{
				memset(dest_buff + (int)x2, poly->color, (int)(x1 - x2 + 1));
			}
			dest_buff += mempitch;
			x1 += dxy1;
			x2 += dxy2;
			startY++;		// y下移一格

			// 判断扫描线的x是否超过了线的终点。（由于斜率比较大导致的)
			if (dxy1 > 0)	// 增加的
			{
				if (x1 > x1end)
				{
					x1 = x1end;
					x1Error = 1;
				}
			}
			else if (dxy1 < 0)
			{
				if (x1 < x1end)
				{
					x1 = x1end;
					x1Error = 1;
				}
			}

			if (dxy2 > 0)
			{
				if (x2 > x2end)
				{
					x2 = x2end;
					x2Error = 1;
				}
			}
			else if (dxy2 < 0)
			{
				if (x2 < x2end)
				{
					x2 = x2end;
					x2Error = 1;
				}
			}
			if (x1Error || x2Error)
			{
				break;
			}
		}
	}
}

POLYGON2D fillPoly;
void Test_FillPolygon_Init()
{
	fillPoly.color = 255;
	fillPoly.state = 1;
	fillPoly.x0 = 200;
	fillPoly.y0 = 200;

	fillPoly.num_verts = 4;
	fillPoly.vlist = new VERTEX2DF[4];
	fillPoly.vlist[0].x = -50;
	fillPoly.vlist[0].y = -50;

	fillPoly.vlist[1].x = 101;
	fillPoly.vlist[1].y = -50;

	fillPoly.vlist[2].x = 101;
	fillPoly.vlist[2].y = 101;

	fillPoly.vlist[3].x = -50;
	fillPoly.vlist[3].y = 101;

//	Rotate_Polygon2d_Mat(&fillPoly, 2);

//	fillPoly.num_verts = 11;
//	fillPoly.vlist = ship_vertexs;

}
void Test_FillPolygon_Main()
{
	DDraw_Fill_Surface(lpddsback, 0);
	DDRAW_INIT_STRUCT(ddsd);
	lpddsback->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);

	fillPoly.color = 255;
	Draw_Filled_Polygon2D(&default_clip_rect, &fillPoly, (UCHAR*)ddsd.lpSurface, ddsd.lPitch);

	fillPoly.color = 220;
	Draw_Polygon2D(default_clip_rect, &fillPoly, (UCHAR*)ddsd.lpSurface, ddsd.lPitch);

	Rotate_Polygon2d_Mat(&fillPoly, 1);

	lpddsback->Unlock(NULL);

	while (lpddsprimary->Flip(NULL, DDFLIP_WAIT));

}
#pragma endregion

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	WNDCLASSEX wclass;
	wclass.cbSize = sizeof(WNDCLASSEX);
	wclass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC;
	wclass.lpfnWndProc = WinProc;
	wclass.cbClsExtra = 0;
	wclass.cbWndExtra = 0;
	wclass.hInstance = hInstance;
	wclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wclass.lpszMenuName = NULL;
	wclass.lpszClassName = WINCLASSNAME;
	wclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// 不显示鼠标
//	ShowCursor(false);

	RegisterClassEx(&wclass);
	HWND hwnd;
	if (FULL_SCREEN_MODE)
	{
		if (!(hwnd = CreateWindowEx(
			NULL,
			WINCLASSNAME,
			WNDNAME,
			//		WS_OVERLAPPED,     // 有标题栏
			//		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			WS_POPUP | WS_VISIBLE,
			0, 0,
			SCREEN_WIDTH, SCREEN_HEIGHT,
			NULL,
			NULL,
			hInstance,
			NULL)))
		{
			//		sprintf("error !! create window failed");
			MessageBox(NULL, "Create window Error!", "Error", MB_OK);
			return 0;
		}

	}
	else
	{
		if (!(hwnd = CreateWindowEx(
			NULL,
			WINCLASSNAME,
			WNDNAME,
			//			WS_OVERLAPPED|WS_VISIBLE,     // 不能改变尺寸,没有关闭窗口按钮什么的
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,	// 可以改变尺寸，有关闭窗口按钮
			0, 0,
			SCREEN_WIDTH, SCREEN_HEIGHT,
			NULL,
			NULL,
			hInstance,
			NULL)))
		{
			//		sprintf("error !! create window failed");
			MessageBox(NULL, "Create window Error!", "Error", MB_OK);
			return 0;
		}
	}
	main_window_handle = hwnd;
	if (!FULL_SCREEN_MODE)
	{
		RECT window_dect = { 0,0,SCREEN_WIDTH - 1,SCREEN_HEIGHT - 1 };
		AdjustWindowRectEx(&window_dect,
			GetWindowStyle(main_window_handle),
			GetMenu(main_window_handle) != NULL,
			GetWindowExStyle(main_window_handle));
		MoveWindow(main_window_handle, 0, 0, window_dect.right - window_dect.left, window_dect.bottom - window_dect.top, false);
	}

	//	ShowWindow(hwnd, nShowCmd);


		//	MessageBox(NULL, "Create window ok!", "ok", MB_OK);

	CTime::deltaTime = 0.0;
	CTime::timeSinceStartUp = 0.0;

	Game_Init();

	MSG msg;
	DWORD start_time;
	bool mainRuned = false;
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//			Game_Main();

				// 锁帧方式 3
		if (!mainRuned)
		{
			mainRuned = true;
			start_time = GetTickCount();

			Game_Main();
		}
		else
		{

			DWORD delta = GetTickCount() - start_time;
			if (delta >= MS_PER_FRAME)
			{
				mainRuned = false;
			}
		}

		// 锁帧方式2

//				while (GetTickCount() - start_time < MS_PER_FRAME);


		// 锁帧方式2  线程休眠
//		start_time = GetTickCount();
//		Game_Main();
//		DWORD delta = GetTickCount() - start_time;
//		if (delta < MS_PER_FRAME)
//		{
//			Sleep(MS_PER_FRAME - delta);
//		}
//


	}

	Game_Exit();
	return msg.wParam;
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	switch (msg)
	{
	case WM_KEYDOWN:
	{
		int virtual_code = (int)wparam;
		if (virtual_code == VK_ESCAPE)
		{
			PostQuitMessage(0);
			return 0;
		}
	}
	break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	break;
	//	case WM_MOVE:
	//	case WM_SIZING:
	//	case WM_SIZE:
	//	case  WM_MOVING:
	//	{
	//		//		Game_Main(hwnd);
	//		//		TestPolygon(hwnd);
	//	}
	//	break;
	//	case WM_CLOSE:
	//		PostQuitMessage(0);
	//		return 0;
	//		break;
	case WM_PAINT:
	{
		hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		return 0;
	}
	break;
	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}


int Game_Main(void *params, int num_parms)
{
	if (is_window_closed)
	{
		return 0;
	}
	if (KEYDOWN(VK_ESCAPE))
	{
		//		SendMessage(main_window_handle, WM_CLOSE, 0, 0);
		PostMessage(main_window_handle, WM_CLOSE, 0, 0);
		is_window_closed = 1;
	}
	On_GameMain();
	return 1;
}
int Game_Init(void *params, int num_parms)
{
	if (FAILED(DirectDrawCreateEx(NULL, (void **)&lpdd, IID_IDirectDraw7, NULL)))
	{
		return 0;
	}


	if (FULL_SCREEN_MODE)
	{
		lpdd->SetCooperativeLevel(main_window_handle,
			DDSCL_FULLSCREEN |
			DDSCL_ALLOWMODEX |
			DDSCL_EXCLUSIVE |
			DDSCL_ALLOWREBOOT);

		lpdd->SetDisplayMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, 0, 0);// 全屏模式下设置DisplayMode
	}
	else
	{
		lpdd->SetCooperativeLevel(main_window_handle, DDSCL_NORMAL);
	}


	DDRAW_INIT_STRUCT(ddsd);
	if (FULL_SCREEN_MODE)
	{
		ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;

		ddsd.dwBackBufferCount = 1;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE
			| DDSCAPS_COMPLEX | DDSCAPS_FLIP;	// 后备表面，设置复杂表面和页面切换功能

	}
	else	// 窗口模式无法进行页面交换。所以不用创建复杂表面
	{
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	}


	lpdd->CreateSurface(&ddsd, &lpddsprimary, NULL);

	if (FULL_SCREEN_MODE)	// 全面模式创建后备缓冲
	{
		// 为主表面附加后备表面
		ddsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;

		if (FAILED(lpddsprimary->GetAttachedSurface(&ddsd.ddsCaps, &lpddsback)))
		{
			// 如果创建失败....
			return 0;
		}
	}

	// 创建表面完成

	DDRAW_INIT_STRUCT(ddpixelformat);

	lpddsprimary->GetPixelFormat(&ddpixelformat);

	if (ddpixelformat.dwRGBBitCount == 8)
	{
		palette[0].peRed = 0;
		palette[0].peGreen = 0;
		palette[0].peBlue = 0;
		palette[0].peFlags = PC_NOCOLLAPSE;

		palette[1].peRed = 0;
		palette[1].peGreen = 0;
		palette[1].peBlue = 0;
		palette[1].peFlags = PC_NOCOLLAPSE;

		palette[255].peRed = 255;
		palette[255].peGreen = 255;
		palette[255].peBlue = 255;
		palette[255].peFlags = PC_NOCOLLAPSE;

		for (int i = 1; i < 255; ++i)
		{
			palette[i].peRed = rand() % 256;
			palette[i].peGreen = rand() % 256;
			palette[i].peBlue = rand() % 256;
			//		palette[i].peRed = 255;
			//		palette[i].peGreen = 255;
			//		palette[i].peBlue = 255;
			palette[i].peFlags = PC_NOCOLLAPSE;
		}


		if (FAILED(lpdd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_ALLOW256 | DDPCAPS_INITIALIZE,
			palette, &lpddpal, NULL)))
		{
			return 0;
		}

		if (FAILED(lpddsprimary->SetPalette(lpddpal)))
		{
			return 0;
		}

	}	// end if BPP==8

	Win_Clipper_Init();

	Init_LookTable();

	On_GameInit();
	return 1;
}

void TestDrawPixels()
{
	DDRAW_INIT_STRUCT(ddsd);
	//	memset(&ddsd, 0, sizeof(ddsd));
	//	ddsd.dwSize = sizeof(ddsd);

	lpddsprimary->Lock(NULL, &ddsd,
		DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);

	int mempitch = ddsd.lPitch;

	UCHAR *video_buffer = (UCHAR *)ddsd.lpSurface;

	//	for(int i=0;i<1000;++i)
	//	{
	//		UCHAR color = rand() % 256;
	//		int x = rand() % WINDOW_W;
	//		int y = rand() % WINDOW_H;
	//
	//		video_buffer[x + y * mempitch] = color;
	//	}
	for (int i = 0; i < 1000; ++i)
	{
		UCHAR red = rand() % 256;
		UCHAR green = rand() % 256;
		UCHAR blue = rand() % 256;
		int x = rand() % SCREEN_WIDTH;
		int y = rand() % SCREEN_HEIGHT;
		Plot16BIT565(x, y, red, green, blue, (USHORT *)ddsd.lpSurface, ddsd.lPitch);
	}

	lpddsprimary->Unlock(NULL);
}
int Game_Exit(void *params, int num_parms)
{
	if (double_buffer)
	{
		delete double_buffer;
		double_buffer = NULL;
	}
	if (lpddpal)
	{
		lpddpal->Release();
		lpddpal = NULL;
	}
	if (lpddsback)
	{
		lpddsback->Release();
		lpddsback = NULL;
	}
	if (lpddsbackground)	//离屏缓冲
	{
		lpddsbackground->Release();
		lpddsbackground = NULL;
	}
	if (lpddsprimary)
	{
		lpddsprimary->Release();
		lpddsprimary = NULL;
	}
	if (lpddclipper != NULL)
	{
		lpddclipper->Release();
		lpddclipper = NULL;
	}
	if (lpdd)
	{
		lpdd->Release();
		lpdd = NULL;
	}
	TestBitmapUnload();
	return 1;
}

void TestPixel(HWND hwnd)
{
	HDC hdc = GetDC(hwnd);
	RECT rect;
	GetClientRect(hwnd, &rect);

	int win_w = rect.right;
	int win_h = rect.bottom;

	for (int i = 0, imax = (win_w + win_h) * 10; i < imax; ++i)
	{
		int x = rand() % win_w;
		int y = rand() % win_h;
		COLORREF  color = RGB(rand() % 255, rand() % 255, rand() % 255);
		SetPixel(hdc, x, y, color);
	}

	ReleaseDC(hwnd, hdc);

}


void TestLine(HWND hwnd)
{
	HDC hdc = GetDC(hwnd);
	RECT rect;
	GetClientRect(hwnd, &rect);

	int win_w = rect.right;
	int win_h = rect.bottom;

	for (int i = 0, imax = (win_w + win_h) / 10; i < imax; ++i)
	{
		COLORREF  color = RGB(rand() % 255, rand() % 255, rand() % 255);
		HPEN hpen = CreatePen(PS_SOLID, 4, color);
		HPEN old_hpen;
		old_hpen = (HPEN)SelectObject(hdc, hpen);
		int x = rand() % win_w;
		int y = rand() % win_h;
		if (i == 0)
		{
			MoveToEx(hdc, x, y, NULL);
		}
		else
		{
			LineTo(hdc, x, y);
		}
		SelectObject(hdc, old_hpen);
		DeleteObject(hpen);
	}


	ReleaseDC(hwnd, hdc);

}

void TestRectangle(HWND hwnd)
{
	HDC hdc = GetDC(hwnd);
	RECT rect;
	GetClientRect(hwnd, &rect);

	int win_w = rect.right;
	int win_h = rect.bottom;

	for (int i = 0, imax = (win_w + win_h) / 10; i < imax; ++i)
	{
		COLORREF  color = RGB(rand() % 255, rand() % 255, rand() % 255);
		//		HPEN hpen = CreatePen(PS_SOLID, 4, color);
		//		HPEN old_hpen;
		//		old_hpen = (HPEN)SelectObject(hdc, hpen);

		HBRUSH hbrush = CreateSolidBrush(color);
		//		HBRUSH hbrush = CreateHatchBrush(HS_CROSS, color);
		//		HBRUSH hbrush = CreateHatchBrush(HS_BDIAGONAL, color);
		HBRUSH old_brush = (HBRUSH)SelectObject(hdc, hbrush);
		int x = rand() % win_w;
		int y = rand() % win_h;
		Rectangle(hdc, x, y, x + rand() % 200, y + rand() % 200);
		//		SelectObject(hdc, old_hpen);
		//		DeleteObject(hpen);
		SelectObject(hdc, old_brush);
		DeleteObject(hbrush);
	}


	ReleaseDC(hwnd, hdc);


}


void TestEllipse(HWND hwnd)
{
	HDC hdc = GetDC(hwnd);
	RECT rect;
	GetClientRect(hwnd, &rect);

	int win_w = rect.right;
	int win_h = rect.bottom;

	for (int i = 0, imax = (win_w + win_h) / 10; i < imax; ++i)
	{
		COLORREF  color = RGB(rand() % 255, rand() % 255, rand() % 255);
		//		HPEN hpen = CreatePen(PS_SOLID, 4, color);
		//		HPEN old_hpen;
		//		old_hpen = (HPEN)SelectObject(hdc, hpen);

		HBRUSH hbrush = CreateSolidBrush(color);
		//		HBRUSH hbrush = CreateHatchBrush(HS_CROSS, color);
		//		HBRUSH hbrush = CreateHatchBrush(HS_BDIAGONAL, color);
		HBRUSH old_brush = (HBRUSH)SelectObject(hdc, hbrush);
		int x = rand() % win_w;
		int y = rand() % win_h;

		Ellipse(hdc, x, y, x + rand() % 200, y + rand() % 200);
		//		Rectangle(hdc, x, y, x + rand() % 200, y + rand() % 200);
		//		SelectObject(hdc, old_hpen);
		//		DeleteObject(hpen);
		SelectObject(hdc, old_brush);
		DeleteObject(hbrush);
	}


	ReleaseDC(hwnd, hdc);


}
COLORREF RandomColor()
{
	COLORREF  color = RGB(rand() % 255, rand() % 255, rand() % 255);
	return color;

}
COLORREF GetColorRef(int r, int g, int b)
{
	COLORREF  color = RGB(r, g, b);
	return color;

}

int x, y, w = 150, h = 150;
COLORREF color = RGB(rand() % 255, rand() % 255, rand() % 255);
int xSym = 1, ySym = 1;
void TestMoveingEllipse(HWND hwnd)
{
	PAINTSTRUCT ps;

	HDC hdc = GetDC(hwnd);


	RECT rect;
	GetClientRect(hwnd, &rect);

	InvalidateRect(hwnd, &rect, true);

	int speed = 25;

	int win_w = rect.right;
	int win_h = rect.bottom;
	x += xSym * speed;
	y += ySym * speed;
	if (x > (win_w - w))
	{
		//		x = win_w;
		xSym = -1;
		color = RandomColor();

	}
	else if (x <= 0)
	{
		xSym = 1;
		color = RandomColor();
	}
	if (y > (win_h - h))
	{
		//		y = win_h;
		ySym = -1;
		color = RandomColor();
	}
	else if (y <= 0)
	{
		ySym = 1;
		color = RandomColor();
	}
	HBRUSH hbr = CreateSolidBrush(color);
	HBRUSH old_hbr = (HBRUSH)SelectObject(hdc, hbr);
	hdc = BeginPaint(hwnd, &ps);

	Ellipse(hdc, x, y, x + w, y + h);


	EndPaint(hwnd, &ps);

	SelectObject(hdc, old_hbr);
	DeleteObject(hbr);

	ReleaseDC(hwnd, hdc);

}


COLORREF polygonColor = RandomColor();

void TestPolygon(HWND hwnd)
{
	HDC hdc = GetDC(hwnd);
	HBRUSH hbr = CreateSolidBrush(polygonColor);
	HBRUSH old_hbr;
	POINT poly[3] = { POINT{x = 100,y = 100},POINT{x = 100,y = 200},POINT{x = 200,y = 100} };

	old_hbr = (HBRUSH)SelectObject(hdc, hbr);
	Polygon(hdc, poly, 3);

	SelectObject(hdc, old_hbr);
	ReleaseDC(hwnd, hdc);

}

static COLORREF textColor = RGB(255, 10, 202);
void TestTextOut(HWND hwnd)
{
	HDC hdc = GetWindowDC(hwnd);
	POINT point;

	PAINTSTRUCT ps;
	RECT rect;
	GetClientRect(hwnd, &rect);
	InvalidateRect(hwnd, &rect, true);

	GetCursorPos(&point);

	if (point.x < 0)
	{
		point.x = 0;
	}
	if (point.x > rect.right)
	{
		point.x = rect.right - 100;
	}
	if (point.y < 0)
	{
		point.y = 0;
	}

	if (point.y > rect.bottom)
	{
		point.y = rect.bottom - 1000;
	}

	LPCTSTR str = "Hello world!!";

	COLORREF old_color = SetTextColor(hdc, textColor);
	COLORREF old_bkColor = SetBkColor(hdc, RandomColor());
	int old_bkMode = SetBkMode(hdc, TRANSPARENT);

	BeginPaint(hwnd, &ps);

	//	TextOut(hdc, point.x, point.y, str, strlen(str));

	RECT strRect = RECT();
	strRect.left = point.x;
	strRect.right = point.x + 500;
	strRect.bottom = point.y + 500;
	strRect.top = point.y;
	DrawText(hdc, str, strlen(str), &strRect, DT_CENTER);

	EndPaint(hwnd, &ps);

	SetTextColor(hdc, old_color);
	SetBkColor(hdc, old_bkColor);
	SetBkMode(hdc, old_bkMode);

	ReleaseDC(hwnd, hdc);
}

void TestDraw(HWND hwnd)
{
	//	LPDIRECTDRAW lpdd;
	//	LPDIRECTDRAW7 lpdd7;
	//	DirectDrawCreate(NULL, &lpdd, NULL);

	//	lpdd->QueryInterface(IID_IDirectDraw7, &lpdd7);
	//	lpdd->Release();
	//	lpdd = NULL;
	//
	//	// use lpdd7
	//
	//	lpdd7->Release();
	//	lpdd7 = NULL;

	//	DirectDrawCreateEx(NULL, &lpdd, IID_IDirectDraw7, NULL);

}
void On_GameInit()
{
	Test_FillPolygon_Init();
	//	Test_matrix_Init();
	//	Test_Draw_Polygon_Init();
		//	Test_DrawLine_Init();
			//	Test_dd_gdi_Main();
			//	Test_win_Init();
			//		InitAttachClip();

			//	if (!double_buffer)
			//	{
			//		double_buffer = new UCHAR[SCREEN_WIDTH*SCREEN_HEIGHT];
			//	}

				//	TestInitBackSurfaceColor16BIT();

				//		TestInitHappyFace();
			//	TestBitMapLoad();

			//	Test_Sprite_Anim_Init();
			//	Test_light_256_Init();
			//	Test_dd_gdi_Init();
}
void On_GameMain()
{
	// MOUSE_MOVED  鼠标左键
	// MOUSE_EVENT  鼠标右键
	if (KEYDOWN(MOUSE_MOVED))
	{
		mouseDown = true;
		lastMousePoint = mousePoint;
		GetCursorPos(&mousePoint);
		//		PostMessage(main_window_handle, WM_CLOSE, 0, 0);
		//		return;
	}
	if (KEYUP(MOUSE_MOVED))
	{
		lastMousePoint = mousePoint;
		GetCursorPos(&mousePoint);
		mouseDown = false;
	}

	Test_FillPolygon_Main();
//	Test_DrawQuad_Main();
	//	Test_DrawTriangle_Main();
		//	Test_matrix_Main();
		//	Test_Draw_Polygon2D_Main();
		//	Test_Draw_Mouse_Pixel_Blt_Main();
		//	Test_Draw_Mouse_Pixel_Main();
			//	Test_DrawClipLine_Main();
				//		Test_DrawLine_Main();
						//	Test_win_Main();
						//	Test_dd_gdi_Main();
						//	Test_light_256_Main();
							//		Test_Sprite_Anim_Main();
									//	switch (SCREEN_BPP)
									//	{
									//	case 8:
							//				TestBitmap8Main();
									//		break;
									//	case 16:
									//		TestBitMap16Main();
									//		break;
									//	case 24:	// 系统不支持24位。会补一个alpha，成32位。
									////		TestBitMap24Main();
									//		break;
									//	case 32:
									//		TestBitMap32Main();
									//		break;
									//	}
										//	TestBitmapMain();
											//	TestClipperHappyFace();
												//	TestGameMainHappyFace();
									//					TestBlitCopyOnPrimarySurface();
										//				TestBlitCopy();
												//		TestBlitFast();
									//						TestBlit();
												//			TestBackBuffer();
														//	TestDoubleBuffering();
									//						TestDrawPixels();

}


