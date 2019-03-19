#pragma once
// BOB(Billtter Object Engine)
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <iostream>
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <io.h>
#include <fcntl.h>
#include <sys/timeb.h>
#include <time.h>
#include <ddraw.h>

#include "Debug.h"
#include "cyclibdraw.h"

#define BOB_STATE_DEAD 0
#define BOB_STATE_ALIVE 1
#define BOB_STATE_DYING 2

#define BOB_STATE_ANIM_DONE 1	// 动画播放完了

#define MAX_BOB_FRAMES 64
#define MAX_BOB_ANIMATIONS 16

// 动画类型
#define BOB_ATTR_SINGLE_FRAME 1
#define BOB_ATTR_MULTI_FRAME 2
#define BOB_ATTR_MULTI_ANIM 4

// 循环与否
#define BOB_ATTR_ANIM_ONE_SHOT 8	//只播放一次

// 是否可以见
#define BOB_ATTR_VISIBLE 16

// 移动属性  回弹 or 滚动
#define BOB_ATTR_BOUNCE 32		
#define BOB_ATTR_WRAPAROUND 64

#define BOB_ATTR_LOADED 128
#define BOB_ATTR_CLONE 256

typedef struct BOB_TYP
{
	int state; 
	int anim_state;
	int attr;
	float x, y;				// position
	float vx, vy;
	int width, height;
	int width_fill;			// 内部使用变量，用于确定像素为8*x宽的表面
	int bpp;				// 每像素位数
	int counter_1;			
	int counter_2;
	int max_count_1;
	int max_count_2;
	int varsI[16];			// 16个整数栈
	int varsF[16];			// 16个浮点数栈
	int curr_frame;			// 当前动画的帧
	int num_frames;			// 动画的总帧数
	int curr_animation;		// 当前动画的index
	int anim_counter;		// 几帧换一次动画帧的临时数据
	int anim_count_max;		// 几帧换一次动画帧 
	int anim_index;			// 动画元素的索引

	int *animations[MAX_BOB_ANIMATIONS];		// 动画序列. 如果序列中，frame_index = -1 表示动画结束
	LPDIRECTDRAWSURFACE7 images[MAX_BOB_FRAMES];

}BOB,*BOB_PTR;

int Create_BOB(BOB_PTR bob, int x, int y, int width, int height, int num_frames, int attr, 
	int mem_flags = 0, USHORT color_key_value = 0);
int Clone_BOB(BOB_PTR source, BOB_PTR dest);

int Destroy_BOB(BOB_PTR bob);

// 多种bpp通用
// 绘制当前帧的image到dest表面
// 注意，绘制的时候，不能lockSurface,否则不会绘制。(blt的缘故吗)
int Draw_BOB(BOB_PTR bob, LPDIRECTDRAWSURFACE7 dest);
//int Draw_BOB16(BOB_PTR bob, LPDIRECTDRAWSURFACE7 dest);
//int Draw_BOB32(BOB_PTR bob, LPDIRECTDRAWSURFACE7 dest);

//多种bpp通用
int Draw_Scaled_BOB(BOB_PTR bob, int swidth, int sheight, LPDIRECTDRAWSURFACE7 dest);
//int Draw_Scaled_BOB16(BOB_PTR bob, int swidth, int sheight, LPDIRECTDRAWSURFACE7 dest);
//int Draw_Scaled_BOB32(BOB_PTR bob, int swidth, int sheight, LPDIRECTDRAWSURFACE7 dest);

// bob加载某一帧的sprite
int Load_Frame_BOB8(BOB_PTR bob, BITMAP_FILE_PTR bitmap, int frame, int cx, int cy, int mode);
// bob加载某一帧的sprite
int Load_Frame_BOB16(BOB_PTR bob, BITMAP_FILE_PTR bitmap, int frame, int cx, int cy, int mode);
// bob加载某一帧的sprite,bmp最大是24位的
int Load_Frame_BOB24(BOB_PTR bob, BITMAP_FILE_PTR bitmap, int frame, int cx, int cy, int mode);

// 播放动画
// 根据动画类型，切换当前帧curr_frame
int Animate_BOB(BOB_PTR bob);
// 根据当前bob的vx vy速度移动
int Move_BOB(BOB_PTR bob);

// bob加载动画帧
int Load_Animation_BOB(BOB_PTR bob, int anim_index, int num_frames, int *sequence);

// 设置当前动画的位置(不管在不在clip区域内)
int Set_Pos_BOB(BOB_PTR bob, int x, int y);
int Set_Vel_BOB(BOB_PTR bob, int vx, int vy);
// 动画播放速度（speed越大，动画越慢）
int Set_Anim_Speed_BOB(BOB_PTR bob, int speed);
// 当前播放那个动画
int Set_Animation_BOB(BOB_PTR bob, int anim_index);

int Hide_BOB(BOB_PTR bob);
int Show_BOB(BOB_PTR bob);

// bob的碰撞检测:检测两个bob的边界是否重叠
int Collision_BOBS(BOB_PTR bob1, BOB_PTR bob2);


