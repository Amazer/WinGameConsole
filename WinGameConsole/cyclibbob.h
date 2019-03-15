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

#define BOB_STATE_ANIM_DONE 1	// ������������

#define MAX_BOB_FRAMES 64
#define MAX_BOB_ANIMATIONS 16

// ��������
#define BOB_ATTR_SINGLE_FRAME 1
#define BOB_ATTR_MULTI_FRAME 2
#define BOB_ATTR_MULTI_ANIM 4

// ѭ�����
#define BOB_ATTR_ANIM_ONE_SHOT 8	//ֻ����һ��

// �Ƿ���Լ�
#define BOB_ATTR_VISIBLE 16

// �ƶ�����  �ص� or ����
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
	int width_fill;			// �ڲ�ʹ�ñ���������ȷ������Ϊ8*x��ı���
	int bpp;				// ÿ����λ��
	int counter_1;			
	int counter_2;
	int max_count_1;
	int max_count_2;
	int varsI[16];			// 16������ջ
	int varsF[16];			// 16��������ջ
	int curr_frame;			// ��ǰ������֡
	int num_frames;			// ��������֡��
	int curr_animation;		// ��ǰ������index
	int anim_counter;		// ��֡��һ�ζ���֡����ʱ����
	int anim_count_max;		// ��֡��һ�ζ���֡ 
	int anim_index;			// ����Ԫ�ص�����

	int *animations[MAX_BOB_ANIMATIONS];		// ��������. ��������У�frame_index = -1 ��ʾ��������
	LPDIRECTDRAWSURFACE7 images[MAX_BOB_FRAMES];

}BOB,*BOB_PTR;

int Create_BOB(BOB_PTR bob, int x, int y, int width, int height, int num_frames, int attr, 
	int mem_flags = 0, USHORT color_key_value = 0, int bpp = 32);
int Clone_BOB(BOB_PTR source, BOB_PTR dest);

int Destroy_BOB(BOB_PTR bob);

//����bppͨ��
// ���Ƶ�ǰ֡��image��dest����
int Draw_BOB(BOB_PTR bob, LPDIRECTDRAWSURFACE7 dest);
//int Draw_BOB16(BOB_PTR bob, LPDIRECTDRAWSURFACE7 dest);
//int Draw_BOB32(BOB_PTR bob, LPDIRECTDRAWSURFACE7 dest);

//����bppͨ��
int Draw_Scaled_BOB(BOB_PTR bob, int swidth, int sheight, LPDIRECTDRAWSURFACE7 dest);
//int Draw_Scaled_BOB16(BOB_PTR bob, int swidth, int sheight, LPDIRECTDRAWSURFACE7 dest);
//int Draw_Scaled_BOB32(BOB_PTR bob, int swidth, int sheight, LPDIRECTDRAWSURFACE7 dest);

// bob����ĳһ֡��sprite
int Load_Frame_BOB8(BOB_PTR bob, BITMAP_FILE_PTR bitmap, int frame, int cx, int cy, int mode);
// bob����ĳһ֡��sprite
int Load_Frame_BOB16(BOB_PTR bob, BITMAP_FILE_PTR bitmap, int frame, int cx, int cy, int mode);
// bob����ĳһ֡��sprite
int Load_Frame_BOB32(BOB_PTR bob, BITMAP_FILE_PTR bitmap, int frame, int cx, int cy, int mode);

// ���Ŷ���
// ���ݶ������ͣ��л���ǰ֡curr_frame
int Animate_BOB(BOB_PTR bob);
// ���ݵ�ǰbob��vx vy�ٶ��ƶ�
int Move_BOB(BOB_PTR bob);

// bob���ض���֡
int Load_Animation_BOB(BOB_PTR bob, int anim_index, int num_frames, int *sequence);

// ���õ�ǰ������λ��(�����ڲ���clip������)
int Set_Pos_BOB(BOB_PTR bob, int x, int y);
int Set_Vel_BOB(BOB_PTR bob, int vx, int vy);
// ���������ٶȣ�speedԽ�󣬶���Խ����
int Set_Anim_Speed_BOB(BOB_PTR bob, int speed);
// ��ǰ�����Ǹ�����
int Set_Animation_BOB(BOB_PTR bob, int anim_index);

int Hide_BOB(BOB_PTR bob);
int Show_BOB(BOB_PTR bob);

// bob����ײ���:�������bob�ı߽��Ƿ��ص�
int Collision_BOBS(BOB_PTR bob1, BOB_PTR bob2);


