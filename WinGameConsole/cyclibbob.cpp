#define WIN32_LEAN_AND_MEAN  
//#include <windows.h>
//#include <windowsx.h>
//#include <mmsystem.h>
//#include <iostream>
//#include <conio.h>
//#include <stdlib.h>
//#include <malloc.h>
//#include <memory.h>
//#include <string.h>
//#include <stdarg.h>
//#include <stdio.h>
//#include <math.h>
//#include <io.h>
//#include <fcntl.h>
//#include <sys/timeb.h>
//#include <time.h>
//#include <ddraw.h>
//#include "Debug.h"
#include "cyclibbob.h"
//#include "cyclibdraw.h"
//

int Create_BOB(BOB_PTR bob, int x, int y, int width, int height, int num_frames,
	int attr, int mem_flags /* = 0 */, USHORT color_key_value /* = 0 */,
	int bpp /* = 32 */)
{

	DDSURFACEDESC2 ddsd;	// 能力描述
	int index;			// 临时变量

	bob->state = BOB_STATE_ALIVE;
	bob->attr = attr;

	bob->anim_state = 0;
	bob->counter_1 = 0;
	bob->counter_2 = 0;
	bob->max_count_1 = 0;
	bob->max_count_2 = 0;

	bob->curr_frame = 0;
	bob->num_frames = num_frames;

	bob->bpp = bpp;
	bob->curr_animation = 0;
	bob->anim_counter = 0;
	bob->anim_index = 0;
	bob->anim_count_max = 0;

	bob->width_fill = 0;
	bob->width = width;
	bob->height = height;
	bob->x = x;
	bob->y = y;
	bob->vx = 0;
	bob->vy = 0;

	for (index = 0; index < MAX_BOB_FRAMES; ++index)
	{
		bob->images[index] = NULL;
	}

	for (index = 0; index < MAX_BOB_ANIMATIONS; ++index)
	{
		bob->animations[index] = NULL;
	}
#if 0
	// bob的宽度是否为8的倍数
	bob->width_fill = ((width % 8 != 0) ? (8 - width % 8) : 0);
	Write_Error("Create BOB:width_fill=%d", bob->width_fill);
#endif

	for (index = 0; index < bob->num_frames; ++index)
	{
		memset(&ddsd, 0, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;

		ddsd.dwWidth = bob->width + bob->width_fill;
		ddsd.dwHeight = bob->height;

		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | mem_flags;

		if (FAILED(lpdd->CreateSurface(&ddsd, &(bob->images[index]), NULL)))
		{
			Write_Error("Create bob surface failed,index:%d", index);
			return 0;
		}
		DDCOLORKEY color_key;
		color_key.dwColorSpaceLowValue = color_key_value;
		color_key.dwColorSpaceHighValue = color_key_value;

		(bob->images[index])->SetColorKey(DDCKEY_SRCBLT, &color_key);
	}

	return 1;
}

int Clone_BOB(BOB_PTR source, BOB_PTR dest)
{
	if ((source && dest) && (source != dest))
	{
		memcpy(dest, source, sizeof(BOB));
		dest->attr |= BOB_ATTR_CLONE;
	}
	else
	{
		Write_Error("Clone_BOB failed");
		return 0;
	}
	return 1;
}

int Destroy_BOB(BOB_PTR bob)
{
	if (!bob)
		return 0;

	int index = 0;

	if (bob->attr&BOB_ATTR_CLONE)
	{
		for (index = 0; index < MAX_BOB_ANIMATIONS; ++index)
		{
			if (bob->animations[index])
			{
				bob->animations[index] = NULL;
			}
		}
		for (index = 0; index < MAX_BOB_FRAMES; ++index)
		{
			if (bob->images[index])
			{
				bob->images[index] = NULL;
			}
		}
	}
	else
	{
		for (index = 0; index < MAX_BOB_ANIMATIONS; ++index)
		{
			if (bob->animations[index])
			{
				free(bob->animations[index]);
//				free(bob->animations[index]);
			}
		}
		for (index = 0; index < MAX_BOB_FRAMES; ++index)
		{
			if (bob->images[index])
			{
				bob->images[index]->Release();
			}
		}
	}
	return 1;
}

//多种bpp通用
int Draw_BOB(BOB_PTR bob, LPDIRECTDRAWSURFACE7 dest)
{
	RECT dest_rect, src_rect;
	if (!bob)
	{
		return 0;
	}
	if (!(bob->attr&BOB_ATTR_VISIBLE))
	{
		return 1;
	}

	dest_rect.left = bob->x;
	dest_rect.top = bob->y;
	dest_rect.right = bob->x + bob->width;
	dest_rect.bottom = bob->y + bob->height;

	src_rect.left = 0;
	src_rect.top = 0;
	src_rect.right = bob->width;
	src_rect.bottom = bob->height;

	if (FAILED(dest->Blt(&dest_rect, bob->images[bob->curr_frame],
		&src_rect, (DDBLT_WAIT | DDBLT_KEYSRC), NULL)))
	{
		return 0;
	}
	return 1;
}

int Draw_Scaled_BOB(BOB_PTR bob, int swidth, int sheight, 
	LPDIRECTDRAWSURFACE7 dest)
{
	RECT dest_rect, src_rect;
	if (!bob)
	{
		return 0;
	}
	if (!(bob->attr&BOB_ATTR_VISIBLE))
	{
		return 1;
	}

	dest_rect.left = bob->x;
	dest_rect.top = bob->y;
	dest_rect.right = bob->x + swidth;
	dest_rect.bottom = bob->y + sheight;

	src_rect.left = 0;
	src_rect.top = 0;
	src_rect.right = bob->width;
	src_rect.bottom = bob->height;

	if (FAILED(dest->Blt(&dest_rect, bob->images[bob->curr_frame],
		&src_rect, (DDBLT_WAIT | DDBLT_KEYSRC), NULL)))
	{
		return 0;
	}
	return 1;

}


// cx cy 是图片的第几列第几行。bitmap中可能是几个sprite放在一起的,
// 当前要加载的是图集中的第cx行中的第cy个
int Load_Frame_BOB8(BOB_PTR bob, BITMAP_FILE_PTR bitmap, 
	int frame, int cx, int cy, int mode)
{
	// cyc explan:要将bitmap draw 到bob的images中
	// 因为image是surface，所以操作时，需要先Lock
	// 将bitmap中的像素 拷贝到 bob的image中
	// 即，进行内存拷贝操作
	DDSURFACEDESC2 ddsd;
	if (!bob)
	{
		return 0;
	}

	if (mode == BITMAP_EXTRACT_MODE_CELL)
	{
		cx = cx * (bob->width + 1) + 1;
		cy = cy * (bob->height + 1) + 1;
	}

	// 8为像素，用UCHAR
	UCHAR *src_ptr, *dest_ptr;

	// 第cy行的第cx个sprite的起始位置
	src_ptr = bitmap->buffer+cy*bitmap->bitmapInfoHeader.biWidth+cx;		// buffer的初始位置

	ddsd.dwSize = sizeof(ddsd);

	// 将进行像素的拷贝.拷贝到image surface中
	(bob->images[frame])->Lock(NULL, &ddsd,
		DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,
		NULL);

	dest_ptr = (UCHAR *)ddsd.lpSurface;
	// 进行逐行像素的拷贝
	for (int i = 0; i < bob->height; ++i) 
	{
		memcpy(dest_ptr, src_ptr, bob->width);
		src_ptr += bitmap->bitmapInfoHeader.biWidth;
		dest_ptr += ddsd.lPitch;
	}

	(bob->images[frame])->Unlock(NULL);

	bob->attr |= BOB_ATTR_LOADED;
	return 1;
}

int Load_Frame_BOB16(BOB_PTR bob, BITMAP_FILE_PTR bitmap, 
	int frame, int cx, int cy, int mode)
{
	// cyc explan:要将bitmap draw 到bob的images中
	// 因为image是surface，所以操作时，需要先Lock
	// 将bitmap中的像素 拷贝到 bob的image中
	// 即，进行内存拷贝操作
	DDSURFACEDESC2 ddsd;
	if (!bob)
	{
		return 0;
	}

	if (mode == BITMAP_EXTRACT_MODE_CELL)
	{
		cx = cx * (bob->width + 1) + 1;
		cy = cy * (bob->height + 1) + 1;
	}

	// 16为像素，用SHORT
	USHORT *src_ptr, *dest_ptr;

	// 第cy行的第cx个sprite的起始位置
	src_ptr = (USHORT *)bitmap->buffer+cy*bitmap->bitmapInfoHeader.biWidth+cx;		// buffer的初始位置

	ddsd.dwSize = sizeof(ddsd);

	// 将进行像素的拷贝.拷贝到image surface中
	(bob->images[frame])->Lock(NULL, &ddsd,
		DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,
		NULL);

	dest_ptr = (USHORT *)ddsd.lpSurface;
	// 进行逐行像素的拷贝
	for (int i = 0; i < bob->height; ++i) 
	{
		// bob->width*2 是因为每个像素用2个字节内存
		memcpy(dest_ptr, src_ptr, bob->width*2);
		src_ptr += bitmap->bitmapInfoHeader.biWidth;
		dest_ptr += (ddsd.lPitch>>1);	// lpitch是字节数。现在每个像素是2个字节。因此内存位置需要总字节数除以2
	}

	(bob->images[frame])->Unlock(NULL);

	bob->attr |= BOB_ATTR_LOADED;
	return 1;
}

int Load_Frame_BOB32(BOB_PTR bob, BITMAP_FILE_PTR bitmap,
	int frame, int cx, int cy, int mode)
{
	// cyc explan:要将bitmap draw 到bob的images中
	// 因为image是surface，所以操作时，需要先Lock
	// 将bitmap中的像素 拷贝到 bob的image中
	// 即，进行内存拷贝操作
	DDSURFACEDESC2 ddsd;
	if (!bob)
	{
		return 0;
	}

	if (mode == BITMAP_EXTRACT_MODE_CELL)
	{
		cx = cx * (bob->width + 1) + 1;
		cy = cy * (bob->height + 1) + 1;
	}

	// 32位像素，用UINT,4个字节
	UINT *src_ptr, *dest_ptr;

	// 第cy行的第cx个sprite的起始位置
	src_ptr = (UINT *)bitmap->buffer+cy*bitmap->bitmapInfoHeader.biWidth+cx;		// buffer的初始位置

	ddsd.dwSize = sizeof(ddsd);

	// 将进行像素的拷贝.拷贝到image surface中
	(bob->images[frame])->Lock(NULL, &ddsd,
		DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,
		NULL);

	dest_ptr = (UINT *)ddsd.lpSurface;
	// 进行逐行像素的拷贝
	for (int i = 0; i < bob->height; ++i) 
	{
		// bob->width*4 是因为每个像素用4个字节内存
		memcpy(dest_ptr, src_ptr, bob->width*4);
		src_ptr += bitmap->bitmapInfoHeader.biWidth;
		// lpitch是字节数。现在每个像素是4个字节。因此内存位置需要总字节数除以4
		dest_ptr += (ddsd.lPitch>>2);	
	}

	(bob->images[frame])->Unlock(NULL);

	bob->attr |= BOB_ATTR_LOADED;
	return 1;

}

// 播放动画
// 根据动画类型，切换当前帧和当前anim_index
int Animate_BOB(BOB_PTR bob)
{
	if (!bob)
	{
		return 0;
	}

	if (bob->attr & BOB_ATTR_SINGLE_FRAME)
	{
		// 如果只有一帧，那么当前帧是0就ok
		bob->curr_frame = 0;
		return 1;
	}

	if (bob->attr &BOB_ATTR_MULTI_FRAME)
	{
		//---
		//bob->anim_counter ++;
		//if(bob->anim_counter>=bob->anim_count_max)
		//	bob->anim_counter =0
		//----

		// 顺便保证，多帧动画只有一个动画
		if (++bob->anim_counter >= bob->anim_count_max)
		{
			bob->anim_counter = 0;

			// 如果有多帧，那么，当前帧++，并且超过了最大帧的值，归零
			if (++bob->curr_frame >= bob->num_frames)
			{
				bob->curr_frame = 0;
			}
		}
	}
	else if (bob->attr&BOB_ATTR_MULTI_ANIM)	// 有多个动画
	{
		// 每anim_count_max次换一次动画帧 
		if (++bob->anim_counter >= bob->anim_count_max)
		{
			bob->anim_counter = 0;

			// 动画帧计数++
			bob->anim_index++;
			bob->curr_frame = bob->animations[bob->curr_animation][bob->anim_index];
			
			// 如果当前帧是-1，表示动画的结束.根据不同类型的动画播放方式处理下一帧应该怎么设置
			if (bob->curr_frame == -1)
			{
				if (bob->attr&BOB_ATTR_ANIM_ONE_SHOT)		// 只播放一次
				{
					bob->attr |= BOB_STATE_ANIM_DONE;
					// 动画帧回退一帧，到最后一帧可以播放的帧(-1是不能播放的)
					bob->anim_index--;
					bob->curr_frame = bob->animations[bob->curr_animation][bob->anim_index];
				}
				else	// 不是只播放一次，即循环播放
				{
					// 从结束标志-1退回到第一帧 
					bob->anim_index=0;
					bob->curr_frame = bob->animations[bob->curr_animation][bob->anim_index];
				}
			}
		}
	}
}

int Move_BOB(BOB_PTR bob)
{
	if (!bob)
	{
		return 0;
	}
	bob->x += bob->vx;
	bob->y += bob->vy;

	//  滚动
	if (bob->attr&BOB_ATTR_WRAPAROUND)
	{
		if (bob->x > default_clipRect.right)
		{
			bob->x = default_clipRect.left-bob->width;
		}
		else if ((bob->x + bob->width) < default_clipRect.left)
		{
			bob->x = default_clipRect.right;
		}

		if (bob->y > default_clipRect.bottom)
		{
			bob->y = default_clipRect.top - bob->height;
		}
		else if (bob->y+bob->height < default_clipRect.top)
		{
			bob->y = default_clipRect.bottom;
		}
	}
	else if (bob->attr&BOB_ATTR_BOUNCE)// 回弹
	{
		if ((bob->x+bob->width >= default_clipRect.right)||bob->x<=default_clipRect.left)
		{
			bob->vx = -bob->vx;
		}
		if (bob->y <= default_clipRect.top || (bob->y+bob->height >= default_clipRect.bottom))
		{
			bob->vy = -bob->vy;
		}
	}
}

int Load_Animation_BOB(BOB_PTR bob, int anim_index, int num_frames, int *sequence)
{
	if (!bob)
		return 0;
	if (anim_index >= MAX_BOB_ANIMATIONS)
		return 0;
	bob->animations[anim_index] = (int *)malloc((num_frames + 1) * sizeof(int));
//	bob->animations[anim_index] = new int[num_frames+1];
	for (int i = 0; i < num_frames; ++i)
	{
		bob->animations[anim_index][i] = sequence[i];
	}
	// 设置结束标志
	bob->animations[anim_index][num_frames] = -1;
	return 1;
}


int Set_Pos_BOB(BOB_PTR bob, int x, int y)
{
	if (!bob)
		return 0;
	bob->x = x;
	bob->y = y;
	return 1;
}

int Set_Vel_BOB(BOB_PTR bob, int vx, int vy)
{
	if (!bob)
		return 0;
	bob->vx = vx;
	bob->vy = vy;
	return 1;
}

int Set_Anim_Speed_BOB(BOB_PTR bob, int speed)
{
	if (!bob)
		return 0;
	bob->anim_count_max = speed;
	return 1;
}
int Set_Animation_BOB(BOB_PTR bob, int anim_index)
{
	if (!bob)
		return 0;
	if (bob->curr_animation != anim_index)
	{
		bob->curr_animation = anim_index;
		bob->anim_index = 0;
	}
	return 1;
}

int Hide_BOB(BOB_PTR bob)
{
	if (!bob)
		return 0;
	RESET_BIT(bob->attr, BOB_ATTR_VISIBLE);
	return 1;
}

int Show_BOB(BOB_PTR bob)
{
	if (!bob)
		return 0;
	SET_BIT(bob->attr, BOB_ATTR_VISIBLE);
	return 1;
}

int Collision_BOBS(BOB_PTR bob1, BOB_PTR bob2)
{
	if (!bob1 || !bob2)
		return 0;
	int width1 = bob1->width >> 1 - bob1->width >> 3;
	int height1 = bob1->height >> 2 - bob1->height >> 3;

	int width2 = bob2->width >> 1 - bob2->width >> 3;
	int height2 = bob2->height >> 1 - bob2->height >> 3;

	int x1 = bob1->x + width1;
	int y1 = bob1->y + height1;
	
	int x2 = bob2->x + width2;
	int y2 = bob2->y + height2;

	int dx = abs(x1-x2);
	int dy = abs(y1-y2);

	if (dx < (width1 + width2) && dy < (height1 + height2))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}