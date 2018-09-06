#pragma once
#ifndef CYCLIB1
#define CYCLIB1

typedef unsigned char BYTE;			// 1个字节
typedef unsigned char UCHAR;		// 1个字节
typedef unsigned int  UINT;			// 4个字节
typedef unsigned short USHORT;		// 2个字节
typedef unsigned short UWORD;		// 2个字节
typedef unsigned long UDWORD;		// 4个字节

#define __RGB16BIT555(r,g,b) ((b&31)+((g&31)<<5)+((r&31)<<10))
#define __RGB16BIT565(r,g,b) ((b&31)+((g&63)<<5)+((r&31)<<11))
#define __RGB24BIT(r,g,b) (b+(g<<8)+(r<<16))
#define __RGB32BIT(a,r,g,b) (b+(g<<8)+(r<<16)+(a<<24))

#define DDRAW_INIT_STRUCT(ddstruct) {memset(&ddstruct,0,sizeof(ddstruct)); ddstruct.dwSize=sizeof(ddstruct);}

#define SCREEN_WIDTH 640	//640		// 屏幕宽度   窗口模式,不设置display mode,使用系统的色彩位深
#define SCREEN_HEIGHT 480	//480 		// 屏幕高度   

#define WIN_OFFSCREEN_WIDTH 720	//640		窗口模式下的离屏缓冲表面 
#define WIN_OFFSCREEN_HEIGHT 720	//480 		

#define SCREEN_BPP 8			// 色彩位深  窗口模式不起作用
#define MS_PER_FRAME 15			// 每帧毫秒数

#define FULL_SCREEN_MODE 1			

#define BITMAP_ID 0x4D42		// bitmap类型id
#define MAX_COLORS_PALETTE 256	// 调色板项的最大数量

#define TEST_BMP_NAME  "bitmap8.bmp";

#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code)&0x8000)?1:0)
#define KEYUP(vk_code) ((GetAsyncKeyState(vk_code)&0x8000)?0:1)

#define CLIP_CODE_C 0x0000
#define CLIP_CODE_N 0x0008
#define CLIP_CODE_S 0x0004
#define CLIP_CODE_E 0x0002
#define CLIP_CODE_W 0x0001

#define CLIP_CODE_NE 0x000a
#define CLIP_CODE_SE 0x0006
#define CLIP_CODE_NW 0x0009
#define CLIP_CODE_SW 0x0005



extern inline void Plot_Pixel32(int x, int y, int alpha, int red, int green, int blue, UINT *video_buffer, int lpitch32);
extern inline void Plot_Pixel32(int x, int y, UINT color, UINT *video_buffer, int lpitch32);
extern inline void Plot8(int x, int y, UCHAR color, UCHAR *buffer, int mempitch);
extern inline void Plot16BIT555(int x, int y, UCHAR red, UCHAR green, UCHAR blue, USHORT *buffer, int mempitch);
extern inline void Plot16BIT565(int x, int y, UCHAR red, UCHAR green, UCHAR blue, USHORT *buffer, int mempitch);
extern inline void Plot16BIT565(int x, int y, USHORT color, USHORT *buffer, int mempitch);
extern inline UINT RandomRGB16BIT565();
extern inline UINT RandomRGBA32();

// 裁剪，bitmap从（0，0）点开始计算位置
extern void Blit_Clipped(int posX, int posY, int sizeWidth, int sizeHeight, UCHAR *bitmap, UCHAR *video_buffer, int mempitch);

#endif
