#pragma once
#define WIN32_LEAN_AND_MEAN
#include "cyclib1.h"
#include "stdlib.h"
void Plot_Pixel32(int x, int y, int alpha, int red, int green, int blue, UINT *video_buffer, int lpitch32)
{
	video_buffer[x + y * lpitch32] = __RGB32BIT(alpha, red, green, blue);
}

void Plot_Pixel32(int x, int y, UINT color, UINT *video_buffer, int lpitch32)
{
	video_buffer[x + y * lpitch32] = color;
}

void Plot8(int x, int y,
	UCHAR color,
	UCHAR *buffer,
	int mempitch)
{
	buffer[x + y * mempitch] = color;
}

void Plot16BIT555(int x, int y,
	UCHAR red,
	UCHAR green,
	UCHAR blue,
	USHORT *buffer,
	int mempitch)
{
	buffer[x + y * (mempitch >> 1)] = __RGB16BIT555(red, green, blue);
}

void Plot16BIT565(int x, int y,
	UCHAR red,
	UCHAR green,
	UCHAR blue,
	USHORT *buffer,
	int mempitch)
{
	buffer[x + y * (mempitch >> 1)] = __RGB16BIT565(red, green, blue);
}
void Plot16BIT565(int x, int y,
	USHORT color,
	USHORT *buffer,
	int mempitch)
{
	buffer[x + y * (mempitch >> 1)] = color;
}
UINT RandomRGB16BIT565()
{
	return __RGB16BIT565(rand() % 255, rand() % 255, rand() % 255);
}

UINT RandomRGBA32()
{

	return __RGB32BIT(rand() % 255, rand() % 255, rand() % 255, rand() % 255);
}


// �ü���bitmap�����Ͻǿ�ʼ����λ��
// posX,posY��Ҫ���Ƶ�bitmap��λ�á�
void Blit_Clipped(int posX, int posY, int sizeWidth, int sizeHeight, UCHAR *bitmap, UCHAR *video_buffer, int mempitch)
{
	// bitmap��ȫ�ڽ�����
	if (posX >= SCREEN_WIDTH || posY >= SCREEN_HEIGHT || (posX + sizeWidth) <= 0 || (posY + sizeHeight) <= 0)
	{
		return;
	}

	// x1,y1,x2,y2Ϊ����Ҫ���Ƶ�bitmap������
	int x1 = posX;					// bitmap ���Ͻ�λ��
	int y1 = posY;
	int x2 = x1 + sizeWidth - 1;	// bitmap ���½�λ��
	int y2 = y1 + sizeHeight - 1;

	if (x1 < 0) x1 = 0;
	if (y1 < 0) y1 = 0;

	if (x2 >= SCREEN_WIDTH) x2 = SCREEN_WIDTH - 1;
	if (y2 >= SCREEN_HEIGHT) y2 = SCREEN_HEIGHT - 1;

	int x_off = x1 - posX;
	int y_off = y1 - posY;

	int dx = x2 - x1 + 1;
	int dy = y2 - y1 + 1;

	// ��video_buffer�еĳ�ʼλ��
	video_buffer += (x1 + y1 * mempitch);
	// Ҫ���Ƶ���bitmap�еĳ�ʼλ��,����bitmap����Ҫ���ƵĲ���
	// (�ƶ�bitmmap�Ļ�����ʼλ��)
	bitmap += (x_off + y_off * sizeWidth);

	UCHAR pixel;
	for (int index_y = 0; index_y < dy; index_y++)
	{
		for (int index_x = 0; index_x < dx; index_x++)
		{
			if ((pixel = bitmap[index_x]))		// ���pixel��ֵ֮����0������false,���ǲ�����
			{
				video_buffer[index_x] = pixel;
			}
		}

		video_buffer += mempitch;
		bitmap += sizeWidth;
	}
}	// end Blit_Clipped
