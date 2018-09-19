#define WIN32_LEAN_AND_MEAN  

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

#include "cyclibdraw.h"

#pragma region ȫ�ֱ���

// externals///////////////////
// �����ļ�ָ��
FILE *fp_error = NULL;
// �����ļ�����
char error_filename[80];

extern HWND main_window_handle;
extern HINSTANCE main_instance;

// ȫ�ֱ�����ʼ��
float cos_look[361];
float sin_look[361];

LPDIRECTDRAW7 lpdd = NULL;
LPDIRECTDRAWSURFACE7 lpddsprimary = NULL;		// ����ʾ����(������)
LPDIRECTDRAWSURFACE7 lpddsback = NULL;			// �󱸱���(��������ӱ���)
LPDIRECTDRAWCLIPPER lpddclipper = NULL;			// �ü���
LPDIRECTDRAWCLIPPER lpddclipperwin = NULL;		// ���ڲü���

LPDIRECTDRAWPALETTE lpddpal = NULL;
PALETTEENTRY palette[MAX_COLORS_PALETTE];

DDSURFACEDESC2 ddsd;						// surface����
DDSCAPS2 ddcaps;
DDBLTFX ddbltfx;							// �ڴ��blitter
HRESULT ddrval;

UCHAR *primary_buffer;
UCHAR *back_buffer;

int primary_lpitch;
int back_lpitch;

DDPIXELFORMAT ddpixelformat;					// ���ظ�ʽ

RECT default_clipRect;						// Ĭ�ϵĲü���(�ʹ���һ����С)

int screen_width = 0;
int screen_height = 0;
int screen_bpp = 8;
int screen_windowed = 0;

int dd_pixel_format = DD_PIXEL_FORMATALPHA888;

int win_client_x0 = 0;
int win_client_y0 = 0;

RECT win_client_rect;						// ����ģʽ���û�����

RECT client_rect;							// �û�����


///////////// ����ָ�� ///////////////////////////

void* (*RGBColor)(int r, int g, int b, int a) = NULL;

USHORT(*RGB16Bit)(int r, int g, int b) = NULL;

// draw triangle_2d �ĺ���ָ��
void(*Draw_Triangle_2D)(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2,
	int color, UCHAR *dest_buffer, int mempitch);

// draw triangleFixedPoint_2d �ĺ���ָ��
void(*Draw_TriangleFP_2D)(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2,
	int color, UCHAR *dest_buffer, int mempitch);

void(*Draw_Filled_Polygon2D)(PRECT clipRect, POLYGON2D_PTR poly, UCHAR *vbuffer, int mempitch);

int(*Draw_Polygon2D)(LPRECT clipRect, POLYGON2D_PTR poly, UCHAR *vbuffer, int lpitch);

//  ����2DͼԪ��غ���ָ��

int(*Draw_Line)(int x0, int y0, int x1, int y1, int color, UCHAR *vb_start, int lpitch);

void(*Draw_Clip_Line)(LPRECT clipRect, int x0, int y0, int x1, int y1, int color, UCHAR *vb_start, int lpitch);

void(*Draw_Pixel)(int x, int y, int color, UCHAR *buffer, int mempitch);

void(*HLine)(LPRECT clipRect, int x1, int x2, int y, int color, UCHAR *vbuffer, int lpitch);
void(*VLine)(LPRECT clipRect, int y1, int y2, int x, int color, UCHAR *vbuffer, int lpitch);

///////////// ����ָ�� edn///////////////////////////

#pragma endregion ȫ�ֱ���

#pragma region ������

USHORT RGB16Bit565(int r, int g, int b)
{
	r >>= 3; g >> 2; b >> 3;
	return __RGB16BIT565(r, g, b);
}
USHORT RGB16Bit555(int r, int g, int b)
{
	r >>= 3; g >> 3; b >> 3;
	return __RGB16BIT555(r, g, b);
}

void * RGBAColor32Bit(int r, int g, int b, int a)
{
	return (void *)__RGB32BIT(a, r, g, b);
}
void* RGBColor8Bit(int r, int g, int b, int a)
{
	return (void*)r;

}
void* RGBColor16Bit555(int r, int g, int b, int a)
{
	r >>= 3; g >> 3; b >> 3;
	return (void*)__RGB16BIT555(r, g, b);
}
void* RGBColor16Bit565(int r, int g, int b, int a)
{
	r >>= 3; g >> 2; b >> 3;
	return (void*)__RGB16BIT565(r, g, b);
}



inline void Draw_Pixel32(int x, int y, int color, UCHAR *video_buffer, int lpitch)
{
	((UINT*)video_buffer)[x + y * (lpitch >> 2)] = color;
}

inline void Draw_Pixel8(int x, int y, int color, UCHAR *buffer, int mempitch)
{
	buffer[x + y * mempitch] = color;
}

inline void Draw_Pixel16(int x, int y, int color, UCHAR *buffer, int mempitch)
{
	((USHORT*)buffer)[x + y * (mempitch >> 1)] = color;
}

UINT RandomRGB16BIT565()
{
	return __RGB16BIT565(rand() % 255, rand() % 255, rand() % 255);
}

UINT RandomRGBA32()
{

	return __RGB32BIT(rand() % 255, rand() % 255, rand() % 255, rand() % 255);
}

#pragma endregion 

#pragma region Draw Rectangle (ʹ����blt)
int Draw_Rectangle(int x1, int y1, int x2, int y2, int color, LPDIRECTDRAWSURFACE7 lpdds)
{
	DDBLTFX ddbltfx;

	RECT rect;
	rect.top = y1;
	rect.left = x1;
	rect.bottom = y2;
	rect.right = x2;

	DDRAW_INIT_STRUCT(ddbltfx);

	ddbltfx.dwFillColor = color;

	lpdds->Blt(&rect, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);

	return 1;

}
#pragma endregion

#pragma region �ü�
// �ü���bitmap�����Ͻǿ�ʼ����λ��
// posX,posY��Ҫ���Ƶ�bitmap��λ�á�
void Blit_Clipped(int posX, int posY, int sizeWidth, int sizeHeight, UCHAR *bitmap, UCHAR *video_buffer, int mempitch)
{
	// bitmap��ȫ�ڽ�����
	if (posX >= screen_width || posY >= screen_height || (posX + sizeWidth) <= 0 || (posY + sizeHeight) <= 0)
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

	if (x2 >= screen_width) x2 = screen_width - 1;
	if (y2 >= screen_height) y2 = screen_height - 1;

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
#pragma endregion 
#pragma region ��ʼ����
int Init_LookTable()
{
	for (int ang = 0; ang <= 360; ++ang)
	{
		float theta = (float)ang*DEG2RAD;
		cos_look[ang] = cos(theta);
		sin_look[ang] = sin(theta);
	}
	return 1;
}
#pragma endregion

#pragma region Polygon2d��غ���

int Translate_Polygon2d(POLYGON2D_PTR polygon, int dx, int dy)
{
	if (!polygon)
		return 0;
	polygon->x0 += dx;
	polygon->y0 += dy;
	return 1;
}

// theta�ǽǶ�
int Rotate_Polygon2d(POLYGON2D_PTR polygon, int theta)
{
	if (!polygon)
		return 0;
	if (theta < 0)
	{
		theta += 360;
	}
	float rad = theta * DEG2RAD;
	float sinRad = sin(rad);
	float cosRad = cos(rad);
	for (int i = 0; i < polygon->num_verts; ++i)
	{
		float xr = polygon->vlist[i].x*cosRad - polygon->vlist[i].y *sinRad;
		float yr = polygon->vlist[i].x*sinRad + polygon->vlist[i].y*cosRad;

		polygon->vlist[i].x = xr;
		polygon->vlist[i].y = yr;
	}
	return 1;
}

int Rotate_Polygon2d_Fast(POLYGON2D_PTR poly, int theta)
{
	if (!poly)
		return 0;
	for (int curr_vert = 0; curr_vert < poly->num_verts; ++curr_vert)
	{
		float xr = (float)poly->vlist[curr_vert].x*cos_look[theta] -
			(float)poly->vlist[curr_vert].y*sin_look[theta];

		float yr = (float)poly->vlist[curr_vert].x*sin_look[theta] +
			(float)poly->vlist[curr_vert].y*cos_look[theta];

		// store result back
		poly->vlist[curr_vert].x = xr;
		poly->vlist[curr_vert].y = yr;
	}

	return 1;
}

int Scale_Polygon2d(POLYGON2D_PTR poly, float s_x, float s_y)
{
	if (!poly)
		return 0;
	for (int i = 0; i < poly->num_verts; ++i)
	{
		poly->vlist[i].x *= s_x;
		poly->vlist[i].y *= s_y;
	}
	return 1;
}

int Translate_Polygon2d_Mat(POLYGON2D_PTR poly, int dx, int dy)
{
	if (!poly)
		return 0;
	MATRIX1X2 posMat = { poly->x0,poly->y0 };

	MATRIX3X2 transMat;
	Mat_Init_3X2(&transMat, 1, 0, 0, 1, dx, dy);

	MATRIX1X2 resMat;

	Mat_Mul_1X2_3X2(&posMat, &transMat, &resMat);

	poly->x0 = resMat.M[0];
	poly->y0 = resMat.M[1];

	return 1;
}

int Rotate_Polygon2d_Mat(POLYGON2D_PTR poly, int theta)
{
	if (!poly)
		return 0;
	if (theta < 0)
	{
		theta += 360;
	}
	MATRIX3X2 rotaMat;
	Mat_Init_3X2(&rotaMat, cos_look[theta], sin_look[theta], -sin_look[theta], cos_look[theta], 0, 0);

	MATRIX1X2 resMat;
	MATRIX1X2 vertMat;
	for (int i = 0; i < poly->num_verts; ++i)
	{
		Mat_Init_1X2(&vertMat, poly->vlist[i].x, poly->vlist[i].y);
		Mat_Mul_1X2_3X2(&vertMat, &rotaMat, &resMat);

		poly->vlist[i].x = resMat.M[0];
		poly->vlist[i].y = resMat.M[1];
	}
	return 1;
}

int Scale_Polygon2d_Mat(POLYGON2D_PTR poly, float s_x, float s_y)
{
	if (!poly)
		return 0;
	MATRIX3X2 scaleMat;
	Mat_Init_3X2(&scaleMat, s_x, 0, 0, s_y, 0, 0);

	MATRIX1X2 resMat;
	MATRIX1X2 verMat;
	for (int i = 0; i < poly->num_verts; ++i)
	{
		Mat_Init_1X2(&verMat, poly->vlist[i].x, poly->vlist[i].y);
		Mat_Mul_1X2_3X2(&verMat, &scaleMat, &resMat);
		poly->vlist[i].x = resMat.M[0];
		poly->vlist[i].y = resMat.M[1];
	}

	return 1;
}


#pragma endregion polygon2d

#pragma region �����ι�դ���Ͷ�������
// ���ƽ�������� ,������ʱ�뷽����Ϊ�������Ͷ��㡣�����ڲ�����Ϊ��p0�Ƕ��㣬p1��left�㣬p2��right��
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

	// �ƶ����㣬��p0���Ƕ���,p1p2�ǵױ�
	// ���ң��ж���p1������ߣ�p2�����ұ�
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
	// ����p1���p2�� ��yֵ��һ���ģ����û���
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

	// �鿴clip��Χ
	if (y0 < clipRect->top)	// ������clip���⣬��ȡ�µ�height��xs xe
	{
		delta_hegith = clipRect->top - y0;

		xs += dxy_left * delta_hegith;		// �������յ�
		xe += dxy_right * delta_hegith;

		y0 = clipRect->top;
	}


	if (y1 > clipRect->bottom)				// �ױ߳���clip��
	{
		y1 = y2 = clipRect->bottom;
	}

	dest_addr = dest_buffer + y0 * mempitch;		// �����ڴ�����λ��

	// ��ʼclipˮƽɨ���ߣ��ͻ���

	// ������ʱ�򣬵㶼��clip���ڣ�
	if (x0 > clipRect->left&&x0<clipRect->right&&
		x1>clipRect->left&&x1<clipRect->right&&
		x2>clipRect->left&&x2 < clipRect->right)
	{
		for (int i = y0; i <= y2; ++i, dest_addr += mempitch)		// !ע�⣬��i<=y2 ��֮ǰû�м�=��:(
		{
			memset((UCHAR*)dest_addr + (unsigned int)xs, color, (unsigned int)(xe - xs + 1));

			xs += dxy_left;
			xe += dxy_right;
		}
	}
	else  // �еĵ㲻��clip����
	{
		for (int i = y0; i <= y2; ++i, dest_addr += mempitch)
		{
			left = xs, right = xe;

			xs += dxy_left;
			xe += dxy_right;

			if (right > clipRect->right)
			{
				right = clipRect->right;
				if (left > clipRect->right)		// �˵���clip���⣬����
				{
					//					dest_addr += mempitch;
					continue;
				}
			}
			if (left < clipRect->left)
			{
				left = clipRect->left;
				if (right < clipRect->left)		// �˵���clip���⣬����
				{
					//					dest_addr += mempitch;
					continue;
				}
			}
			memset((UCHAR*)dest_addr + (unsigned int)left, color, (unsigned int)(right - left + 1));

			//			dest_addr += mempitch;		// for ѭ���мӹ���
		}

	}
}

void Draw_Bottom_Tri16(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR* dest_buffer, int mempitch)
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

	mempitch = (mempitch >> 1);
	USHORT *dest_addr = (USHORT *)dest_buffer;

	// �ƶ����㣬��p0���Ƕ���,p1p2�ǵױ�
	// ���ң��ж���p1������ߣ�p2�����ұ�
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
	// ����p1���p2�� ��yֵ��һ���ģ����û���
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

	// �鿴clip��Χ
	if (y0 < clipRect->top)	// ������clip���⣬��ȡ�µ�height��xs xe
	{
		delta_hegith = clipRect->top - y0;

		xs += dxy_left * delta_hegith;		// �������յ�
		xe += dxy_right * delta_hegith;

		y0 = clipRect->top;
	}


	if (y1 > clipRect->bottom)				// �ױ߳���clip��
	{
		y1 = y2 = clipRect->bottom;
	}

	dest_addr += y0 * mempitch;		// �����ڴ�����λ��

	// ��ʼclipˮƽɨ���ߣ��ͻ���

	// ������ʱ�򣬵㶼��clip���ڣ�
	if (x0 > clipRect->left&&x0<clipRect->right&&
		x1>clipRect->left&&x1<clipRect->right&&
		x2>clipRect->left&&x2 < clipRect->right)
	{
		for (int i = y0; i <= y2; ++i, dest_addr += mempitch)		// !ע�⣬��i<=y2 ��֮ǰû�м�=��:(
		{
			Mem_Set_USHORT(dest_addr + (UINT)xs, color, (UINT)(xe - xs + 1));
			//			memset((USHORT*)dest_addr + (unsigned int)xs, color, (unsigned int)(xe - xs + 1)*sizeof(USHORT));

			xs += dxy_left;
			xe += dxy_right;
		}
	}
	else  // �еĵ㲻��clip����
	{
		for (int i = y0; i <= y2; ++i, dest_addr += mempitch)
		{
			left = xs, right = xe;

			xs += dxy_left;
			xe += dxy_right;

			if (right > clipRect->right)
			{
				right = clipRect->right;
				if (left > clipRect->right)		// �˵���clip���⣬����
				{
					//					dest_addr += mempitch;
					continue;
				}
			}
			if (left < clipRect->left)
			{
				left = clipRect->left;
				if (right < clipRect->left)		// �˵���clip���⣬����
				{
					//					dest_addr += mempitch;
					continue;
				}
			}

			Mem_Set_USHORT(dest_addr + (UINT)xs, color, (UINT)(right - left + 1));
			//			memset((USHORT*)dest_addr + (unsigned int)left, color, (unsigned int)(right - left + 1)*sizeof(USHORT));

						//			dest_addr += mempitch;		// for ѭ���мӹ���
		}

	}
}

void Draw_Bottom_Tri32(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR* dest_buffer, int mempitch)
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

	mempitch = (mempitch >> 2);
	UINT *dest_addr = (UINT*)dest_buffer;

	// �ƶ����㣬��p0���Ƕ���,p1p2�ǵױ�
	// ���ң��ж���p1������ߣ�p2�����ұ�
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
	// ����p1���p2�� ��yֵ��һ���ģ����û���
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

	// �鿴clip��Χ
	if (y0 < clipRect->top)	// ������clip���⣬��ȡ�µ�height��xs xe
	{
		delta_hegith = clipRect->top - y0;

		xs += dxy_left * delta_hegith;		// �������յ�
		xe += dxy_right * delta_hegith;

		y0 = clipRect->top;
	}


	if (y1 > clipRect->bottom)				// �ױ߳���clip��
	{
		y1 = y2 = clipRect->bottom;
	}

	dest_addr += y0 * mempitch;		// �����ڴ�����λ��

	// ��ʼclipˮƽɨ���ߣ��ͻ���

	// ������ʱ�򣬵㶼��clip���ڣ�
	if (x0 > clipRect->left&&x0<clipRect->right&&
		x1>clipRect->left&&x1<clipRect->right&&
		x2>clipRect->left&&x2 < clipRect->right)
	{
		for (int i = y0; i <= y2; ++i, dest_addr += mempitch)		// !ע�⣬��i<=y2 ��֮ǰû�м�=��:(
		{
			Mem_Set_UINT(dest_addr + (unsigned int)xs, color, (unsigned int)(xe - xs + 1));
			//			memset((UINT*)dest_addr + (unsigned int)xs, color, (unsigned int)(xe - xs + 1)*sizeof(UINT));

			xs += dxy_left;
			xe += dxy_right;
		}
	}
	else  // �еĵ㲻��clip����
	{
		for (int i = y0; i <= y2; ++i, dest_addr += mempitch)
		{
			left = xs, right = xe;

			xs += dxy_left;
			xe += dxy_right;

			if (right > clipRect->right)
			{
				right = clipRect->right;
				if (left > clipRect->right)		// �˵���clip���⣬����
				{
					//					dest_addr += mempitch;
					continue;
				}
			}
			if (left < clipRect->left)
			{
				left = clipRect->left;
				if (right < clipRect->left)		// �˵���clip���⣬����
				{
					//					dest_addr += mempitch;
					continue;
				}
			}
			Mem_Set_UINT(dest_addr + (unsigned int)xs, color, (unsigned int)(right - left + 1));
			//			memset((UINT*)dest_addr + (unsigned int)left, color, (unsigned int)(right - left + 1)*sizeof(UINT));

						//			dest_addr += mempitch;		// for ѭ���мӹ���
		}

	}
}

// ���ƽ�������Σ�������ʱ�뷽����Ϊ�������Ͷ���,�����ڲ�����Ϊ��p0�ǵ͵㣬p1��right�㣬p2��left��
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

	if (y1 != y2 && y1 != y0)		// p1�ǵ͵�
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

	// �����ǵ�p1���ұߣ�p2�����,yֵһ�������û�
	if (x1 < x2)
	{
		tmp_x = x2;
		x2 = x1;

		x1 = tmp_x;
	}

	// ���ƽ��������
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

	// ���������
	dest_addr = dest_buffer + y0 * mempitch;

	// �㶼��clip����
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
	else	// �еĵ㱻clip����
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

void Draw_Top_Tri16(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR* dest_buffer, int mempitch)
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

	mempitch >>= 1;
	USHORT *dest_addr = (USHORT*)dest_buffer;

	if (y1 != y2 && y1 != y0)		// p1�ǵ͵�
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

	// �����ǵ�p1���ұߣ�p2�����,yֵһ�������û�
	if (x1 < x2)
	{
		tmp_x = x2;
		x2 = x1;

		x1 = tmp_x;
	}

	// ���ƽ��������
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

	// ���������
	dest_addr += y0 * mempitch;

	// �㶼��clip����
	if (x0 > clipRect->left&&x0<clipRect->right&&
		x1>clipRect->left&&x1<clipRect->right&&
		x2>clipRect->left&&x2 < clipRect->right)
	{
		for (int i = y0; i >= y1; --i, dest_addr -= mempitch)
		{

			Mem_Set_USHORT(dest_addr + (UINT)xs, color, (UINT)(xe - xs + 1));
			//			memset((USHORT*)dest_addr + (unsigned int)xs, color, (unsigned int)(xe - xs + 1)*sizeof(USHORT));
			xs -= dxy_left;
			xe -= dxy_right;
		}
	}
	else	// �еĵ㱻clip����
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

			Mem_Set_USHORT(dest_addr + (UINT)xs, color, (UINT)(right - left + 1));
			//			memset((USHORT*)dest_addr + (unsigned int)left, color, (unsigned int)(right - left + 1)*sizeof(USHORT));
		}

	}

	//	for (int i = y1; i > y0; --i)
	//	{
	//		Draw_Clip_Line8(default_clip_rect, xs + 0.5, i, xe + 0.5, i, color, dest_buffer, mempitch);
	//		xs -= dxy_left;
	//		xe -= dxy_right;
	//	}
}

inline void Mem_Set_USHORT(void *dest, USHORT data, int count)
{
	// this function fills or sets unsigned 16-bit aligned memory
	// count is number of words

	_asm
	{
		mov edi, dest; edi points to destination memory
		mov ecx, count; number of 16 - bit words to move
		mov ax, data; 16 - bit data
		rep stosw; move data
	} // end asm

} // end Mem_Set_WORD

inline void Mem_Set_UINT(void *dest, UINT data, int count)
{
	// this function fills or sets unsigned 32-bit aligned memory
	// count is number of quads

	_asm
	{
		mov edi, dest; edi points to destination memory
		mov ecx, count; number of 32 - bit words to move
		mov eax, data; 32 - bit data
		rep stosd; move data
	} // end asm

} // end Mem_Set_QUAD
void Draw_Top_Tri32(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR* dest_buffer, int mempitch)
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

	mempitch = (mempitch >> 2);

	UINT *dest_addr = (UINT*)dest_buffer;

	if (y1 != y2 && y1 != y0)		// p1�ǵ͵�
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

	// �����ǵ�p1���ұߣ�p2�����,yֵһ�������û�
	if (x1 < x2)
	{
		tmp_x = x2;
		x2 = x1;

		x1 = tmp_x;
	}

	// ���ƽ��������
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

	// ���������
	dest_addr += y0 * mempitch;

	// �㶼��clip����
	if (x0 > clipRect->left&&x0<clipRect->right&&
		x1>clipRect->left&&x1<clipRect->right&&
		x2>clipRect->left&&x2 < clipRect->right)
	{
		for (int i = y0; i >= y1; --i, dest_addr -= mempitch)
		{

			Mem_Set_UINT(dest_addr + (unsigned int)xs, color, (unsigned int)(xe - xs + 1));
			//			memset((UINT*)dest_addr + (unsigned int)xs, color, (unsigned int)(xe - xs + 1)*sizeof(UINT));
			xs -= dxy_left;
			xe -= dxy_right;
		}
	}
	else	// �еĵ㱻clip����
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

			Mem_Set_UINT(dest_addr + (unsigned int)xs, color, (unsigned int)(right - left + 1));
			//			memset((UINT*)dest_addr + (unsigned int)left, color, (unsigned int)(right - left + 1)*sizeof(UINT));
		}

	}

	//	for (int i = y1; i > y0; --i)
	//	{
	//		Draw_Clip_Line8(default_clip_rect, xs + 0.5, i, xe + 0.5, i, color, dest_buffer, mempitch);
	//		xs -= dxy_left;
	//		xe -= dxy_right;
	//	}
}
// ����һ�������Ρ�����˳����ʱ�뷢�͡��ڲ�����Ϊ��p0Ϊ���㣬p1Ϊ��ߵ㣬p2Ϊ�ұߵ�
// ʹ�÷ָ�Ϊ���������εķ�ʽ�������
void Draw_Triangle_2D8(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2,
	int color, UCHAR *dest_buffer, int mempitch)
{
	// ��yֵ���� y0<y1<y2
	int tmp;
	// �Ǵ�ֱ���߻�����ˮƽ���ߣ�����
	if ((x0 == x1 && x1 == x2) || (y0 == y1 && y1 == y2))
	{
		return;
	}

	if (y1 < y2&&y1 < y0)		// y1�Ƕ���,��0�㽻��
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
	else if (y2 < y1&&y2 < y0)	// y2�Ƕ��㣬��0�㽻��
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

	if (y2 < y1)		// ��yֵ����
	{
		tmp = y1;
		y1 = y2;
		y2 = tmp;

		tmp = x1;
		x1 = x2;
		x2 = tmp;
	}


	// ����Ƿ�ȫ����clip����
	if (y0 > clipRect->bottom ||												// ��clip������
		(y1 < clipRect->top&&y2 < clipRect->top) ||							// ��clip������
		(x0 < clipRect->left&&x1 < clipRect->left&&x2 < clipRect->left) ||			// ��clip������
		(x0 > clipRect->right&&x1 > clipRect->right&&x2 > clipRect->right)		// ��clip������
		)
	{
		return;
	}

	if (y0 == y1 || y0 == y2)		// �Ǹ�ƽ��������
	{
		Draw_Top_Tri(clipRect, x0, y0, x1, y1, x2, y2, color, dest_buffer, mempitch);
		return;
	}

	if (y1 == y2)		// �Ǹ�ƽ��������
	{
		Draw_Bottom_Tri(clipRect, x0, y0, x1, y1, x2, y2, color, dest_buffer, mempitch);
		return;
	}

	// һ�������Σ��ָ������������
	// ��PtempΪ��ʱ�㣬�Ѿ�֪����Yt=y1,�������������εã�
	// Xt=(x0-x2)*(y1-y2)/(y0-y2)+x2
	float new_x = x2 + (int)((float)(x0 - x2)*(float)(y1 - y2) / (float)(y0 - y2) + 0.5);

	Draw_Bottom_Tri(clipRect, x0, y0, x1, y1, new_x, y1, color, dest_buffer, mempitch);	// P0,P1,Pt
	Draw_Top_Tri(clipRect, x2, y2, new_x, y1, x1, y1, color, dest_buffer, mempitch);			// P2,Pt,P1
	return;
}

void Draw_Triangle_2D16(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2,
	int color, UCHAR *dest_buffer, int mempitch)
{
	// ��yֵ���� y0<y1<y2
	int tmp;
	// �Ǵ�ֱ���߻�����ˮƽ���ߣ�����
	if ((x0 == x1 && x1 == x2) || (y0 == y1 && y1 == y2))
	{
		return;
	}

	if (y1 < y2&&y1 < y0)		// y1�Ƕ���,��0�㽻��
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
	else if (y2 < y1&&y2 < y0)	// y2�Ƕ��㣬��0�㽻��
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

	if (y2 < y1)		// ��yֵ����
	{
		tmp = y1;
		y1 = y2;
		y2 = tmp;

		tmp = x1;
		x1 = x2;
		x2 = tmp;
	}


	// ����Ƿ�ȫ����clip����
	if (y0 > clipRect->bottom ||												// ��clip������
		(y1 < clipRect->top&&y2 < clipRect->top) ||							// ��clip������
		(x0 < clipRect->left&&x1 < clipRect->left&&x2 < clipRect->left) ||			// ��clip������
		(x0 > clipRect->right&&x1 > clipRect->right&&x2 > clipRect->right)		// ��clip������
		)
	{
		return;
	}

	if (y0 == y1 || y0 == y2)		// �Ǹ�ƽ��������
	{
		Draw_Top_Tri16(clipRect, x0, y0, x1, y1, x2, y2, color, dest_buffer, mempitch);
		return;
	}

	if (y1 == y2)		// �Ǹ�ƽ��������
	{
		Draw_Bottom_Tri16(clipRect, x0, y0, x1, y1, x2, y2, color, dest_buffer, mempitch);
		return;
	}

	// һ�������Σ��ָ������������
	// ��PtempΪ��ʱ�㣬�Ѿ�֪����Yt=y1,�������������εã�
	// Xt=(x0-x2)*(y1-y2)/(y0-y2)+x2
	float new_x = x2 + (int)((float)(x0 - x2)*(float)(y1 - y2) / (float)(y0 - y2) + 0.5);

	Draw_Bottom_Tri16(clipRect, x0, y0, x1, y1, new_x, y1, color, dest_buffer, mempitch);	// P0,P1,Pt
	Draw_Top_Tri16(clipRect, x2, y2, new_x, y1, x1, y1, color, dest_buffer, mempitch);			// P2,Pt,P1
	return;

}

void Draw_Triangle_2D32(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2,
	int color, UCHAR *dest_buffer, int mempitch)
{
	// ��yֵ���� y0<y1<y2
	int tmp;
	// �Ǵ�ֱ���߻�����ˮƽ���ߣ�����
	if ((x0 == x1 && x1 == x2) || (y0 == y1 && y1 == y2))
	{
		return;
	}

	if (y1 < y2&&y1 < y0)		// y1�Ƕ���,��0�㽻��
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
	else if (y2 < y1&&y2 < y0)	// y2�Ƕ��㣬��0�㽻��
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

	if (y2 < y1)		// ��yֵ����
	{
		tmp = y1;
		y1 = y2;
		y2 = tmp;

		tmp = x1;
		x1 = x2;
		x2 = tmp;
	}


	// ����Ƿ�ȫ����clip����
	if (y0 > clipRect->bottom ||												// ��clip������
		(y1 < clipRect->top&&y2 < clipRect->top) ||							// ��clip������
		(x0 < clipRect->left&&x1 < clipRect->left&&x2 < clipRect->left) ||			// ��clip������
		(x0 > clipRect->right&&x1 > clipRect->right&&x2 > clipRect->right)		// ��clip������
		)
	{
		return;
	}

	if (y0 == y1 || y0 == y2)		// �Ǹ�ƽ��������
	{
		Draw_Top_Tri32(clipRect, x0, y0, x1, y1, x2, y2, color, dest_buffer, mempitch);
		return;
	}

	if (y1 == y2)		// �Ǹ�ƽ��������
	{
		Draw_Bottom_Tri32(clipRect, x0, y0, x1, y1, x2, y2, color, dest_buffer, mempitch);
		return;
	}

	// һ�������Σ��ָ������������
	// ��PtempΪ��ʱ�㣬�Ѿ�֪����Yt=y1,�������������εã�
	// Xt=(x0-x2)*(y1-y2)/(y0-y2)+x2
	float new_x = x2 + (int)((float)(x0 - x2)*(float)(y1 - y2) / (float)(y0 - y2) + 0.5);

	Draw_Bottom_Tri32(clipRect, x0, y0, x1, y1, new_x, y1, color, dest_buffer, mempitch);	// P0,P1,Pt
	Draw_Top_Tri32(clipRect, x2, y2, new_x, y1, x1, y1, color, dest_buffer, mempitch);			// P2,Pt,P1
	return;

}

//void Draw_Triangle_2D32(int x0, int y0, int x1, int y1, int x2, int y2,
//	int color, UCHAR *dest_buffer, int mempitch);

void Draw_Top_TriFP8(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR *dest_buffer, int mempitch)
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

	// ��ֱ���ߣ��˳�
	if (y0 == y2 && y1 == y2)
	{
		return;
	}

	if (y1 != y2 && y1 != y0)		// p1�ǵ͵�
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

	// �����ǵ�p1���ұߣ�p2�����,yֵһ�������û�
	if (x1 < x2)
	{
		tmp_x = x2;
		x2 = x1;

		x1 = tmp_x;
	}

	// ���ƽ��������
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

	// ���������
	dest_addr = dest_buffer + y0 * mempitch;

	// �㶼��clip����
	if (x0 > clipRect->left&&x0<clipRect->right&&
		x1>clipRect->left&&x1<clipRect->right&&
		x2>clipRect->left&&x2 < clipRect->right)
	{
		for (int i = y0; i >= y1; --i, dest_addr -= mempitch)
		{

			// xs��se���Ƕ�������Ҫ��ת�س�����
			memset((UCHAR*)dest_addr + ((xs + FIXP16_ROUND_UP) >> FIXP16_SHIFT), color, (((xe - xs + FIXP16_ROUND_UP) >> FIXP16_SHIFT) + 1));
			xs -= dxy_left;
			xe -= dxy_right;
		}
	}
	else	// �еĵ㱻clip����
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

void Draw_Top_TriFP16(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR *dest_buffer, int mempitch)
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

	mempitch = (mempitch >> 1);
	USHORT *dest_addr = (USHORT*)dest_buffer;

	// ��ֱ���ߣ��˳�
	if (y0 == y2 && y1 == y2)
	{
		return;
	}

	if (y1 != y2 && y1 != y0)		// p1�ǵ͵�
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

	// �����ǵ�p1���ұߣ�p2�����,yֵһ�������û�
	if (x1 < x2)
	{
		tmp_x = x2;
		x2 = x1;

		x1 = tmp_x;
	}

	// ���ƽ��������
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

	// ���������
	dest_addr += y0 * mempitch;

	// �㶼��clip����
	if (x0 > clipRect->left&&x0<clipRect->right&&
		x1>clipRect->left&&x1<clipRect->right&&
		x2>clipRect->left&&x2 < clipRect->right)
	{
		for (int i = y0; i >= y1; --i, dest_addr -= mempitch)
		{

			//			Mem_Set_USHORT(dest_addr + ((xs + FIXP16_ROUND_UP) >> FIXP16_SHIFT), color, (((xe - xs + FIXP16_ROUND_UP) >> FIXP16_SHIFT) + 1));
			Mem_Set_USHORT(dest_addr + FIXP_2_INT(xs), color, FIXP_2_INT((xe - xs)) + 1);
			// xs��se���Ƕ�������Ҫ��ת�س�����
//			memset((UCHAR*)dest_addr + ((xs + FIXP16_ROUND_UP) >> FIXP16_SHIFT), color, (((xe - xs + FIXP16_ROUND_UP) >> FIXP16_SHIFT) + 1));
			xs -= dxy_left;
			xe -= dxy_right;
		}
	}
	else	// �еĵ㱻clip����
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

			Mem_Set_USHORT(dest_addr + FIXP_2_INT(xs), color, FIXP_2_INT((right - left)) + 1);
			//			Mem_Set_USHORT(dest_addr + (unsigned int)left, color, (unsigned int)(right - left + 1));
			//			memset((UCHAR*)dest_addr + (unsigned int)left, color, (unsigned int)(right - left + 1));
		}

	}

}

void Draw_Top_TriFP32(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR *dest_buffer, int mempitch)
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

	mempitch = (mempitch >> 2);
	UINT *dest_addr = (UINT*)dest_buffer;

	// ��ֱ���ߣ��˳�
	if (y0 == y2 && y1 == y2)
	{
		return;
	}

	if (y1 != y2 && y1 != y0)		// p1�ǵ͵�
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

	// �����ǵ�p1���ұߣ�p2�����,yֵһ�������û�
	if (x1 < x2)
	{
		tmp_x = x2;
		x2 = x1;

		x1 = tmp_x;
	}

	// ���ƽ��������
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

	// ���������
	dest_addr += y0 * mempitch;

	// �㶼��clip����
	if (x0 > clipRect->left&&x0<clipRect->right&&
		x1>clipRect->left&&x1<clipRect->right&&
		x2>clipRect->left&&x2 < clipRect->right)
	{
		for (int i = y0; i >= y1; --i, dest_addr -= mempitch)
		{

			//			Mem_Set_UINT(dest_addr + ((xs + FIXP16_ROUND_UP) >> FIXP16_SHIFT), color, (((xe - xs + FIXP16_ROUND_UP) >> FIXP16_SHIFT) + 1));
			Mem_Set_UINT(dest_addr + FIXP_2_INT(xs), color, FIXP_2_INT((xe - xs)) + 1);
			// xs��se���Ƕ�������Ҫ��ת�س�����
//			memset((UCHAR*)dest_addr + ((xs + FIXP16_ROUND_UP) >> FIXP16_SHIFT), color, (((xe - xs + FIXP16_ROUND_UP) >> FIXP16_SHIFT) + 1));
			xs -= dxy_left;
			xe -= dxy_right;
		}
	}
	else	// �еĵ㱻clip����
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

			Mem_Set_UINT(dest_addr + FIXP_2_INT(xs), color, FIXP_2_INT((right - left)) + 1);
			//			Mem_Set_USHORT(dest_addr + (unsigned int)left, color, (unsigned int)(right - left + 1));
			//			memset((UCHAR*)dest_addr + (unsigned int)left, color, (unsigned int)(right - left + 1));
		}

	}

}

void Draw_Bottom_TriFP8(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR *dest_buffer, int mempitch)
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

	// �ƶ����㣬��p0���Ƕ���,p1p2�ǵױ�
	// ���ң��ж���p1������ߣ�p2�����ұ�
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
	// ����p1���p2�� ��yֵ��һ���ģ����û���
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

	// �鿴clip��Χ
	if (y0 < clipRect->top)	// ������clip���⣬��ȡ�µ�height��xs xe
	{
		delta_hegith = clipRect->top - y0;

		xs += dxy_left * delta_hegith;		// �������յ�
		xe += dxy_right * delta_hegith;

		y0 = clipRect->top;
	}


	if (y1 > clipRect->bottom)				// �ױ߳���clip��
	{
		y1 = y2 = clipRect->bottom;
	}

	dest_addr = dest_buffer + y0 * mempitch;		// �����ڴ�����λ��

	// ��ʼclipˮƽɨ���ߣ��ͻ���

	// ������ʱ�򣬵㶼��clip���ڣ�
	if (x0 > clipRect->left&&x0<clipRect->right&&
		x1>clipRect->left&&x1<clipRect->right&&
		x2>clipRect->left&&x2 < clipRect->right)
	{
		for (int i = y0; i <= y2; ++i, dest_addr += mempitch)		// !ע�⣬��i<=y2 ��֮ǰû�м�=��:(
		{
			memset((UCHAR*)dest_addr + ((xs + FIXP16_ROUND_UP) >> FIXP16_SHIFT), color, (((xe - xs) + FIXP16_ROUND_UP) >> FIXP16_SHIFT) + 1);

			xs += dxy_left;
			xe += dxy_right;
		}
	}
	else  // �еĵ㲻��clip����
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
				if (left > clipRect->right)		// �˵���clip���⣬����
				{
					//					dest_addr += mempitch;
					continue;
				}
			}
			if (left < clipRect->left)
			{
				left = clipRect->left;
				if (right < clipRect->left)		// �˵���clip���⣬����
				{
					//					dest_addr += mempitch;
					continue;
				}
			}
			memset((UCHAR*)dest_addr + left, color, (right - left + 1));

			//			dest_addr += mempitch;		// for ѭ���мӹ���
		}

	}
}

void Draw_Bottom_TriFP16(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR *dest_buffer, int mempitch)
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

	mempitch = (mempitch >> 1);
	USHORT *dest_addr = (USHORT *)dest_buffer;

	if (y0 == y1 && y1 == y2)
	{
		return;
	}

	// �ƶ����㣬��p0���Ƕ���,p1p2�ǵױ�
	// ���ң��ж���p1������ߣ�p2�����ұ�
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
	// ����p1���p2�� ��yֵ��һ���ģ����û���
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

	// �鿴clip��Χ
	if (y0 < clipRect->top)	// ������clip���⣬��ȡ�µ�height��xs xe
	{
		delta_hegith = clipRect->top - y0;

		xs += dxy_left * delta_hegith;		// �������յ�
		xe += dxy_right * delta_hegith;

		y0 = clipRect->top;
	}


	if (y1 > clipRect->bottom)				// �ױ߳���clip��
	{
		y1 = y2 = clipRect->bottom;
	}

	dest_addr += y0 * mempitch;		// �����ڴ�����λ��

	// ��ʼclipˮƽɨ���ߣ��ͻ���

	// ������ʱ�򣬵㶼��clip���ڣ�
	if (x0 > clipRect->left&&x0<clipRect->right&&
		x1>clipRect->left&&x1<clipRect->right&&
		x2>clipRect->left&&x2 < clipRect->right)
	{
		for (int i = y0; i <= y2; ++i, dest_addr += mempitch)		// !ע�⣬��i<=y2 ��֮ǰû�м�=��:(
		{
			Mem_Set_USHORT(dest_addr + FIXP_2_INT(xs), color, FIXP_2_INT((xe - xs)) + 1);
			//			memset((UCHAR*)dest_addr + ((xs + FIXP16_ROUND_UP) >> FIXP16_SHIFT), color, (((xe - xs) + FIXP16_ROUND_UP) >> FIXP16_SHIFT) + 1);

			xs += dxy_left;
			xe += dxy_right;
		}
	}
	else  // �еĵ㲻��clip����
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
				if (left > clipRect->right)		// �˵���clip���⣬����
				{
					//					dest_addr += mempitch;
					continue;
				}
			}
			if (left < clipRect->left)
			{
				left = clipRect->left;
				if (right < clipRect->left)		// �˵���clip���⣬����
				{
					//					dest_addr += mempitch;
					continue;
				}
			}
			Mem_Set_USHORT(dest_addr + left, color, (right - left + 1));
			//			memset((UCHAR*)dest_addr + left, color, (right - left + 1));

						//			dest_addr += mempitch;		// for ѭ���мӹ���
		}

	}
}

void Draw_Bottom_TriFP32(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR *dest_buffer, int mempitch)
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

	mempitch = (mempitch >> 2);
	UINT *dest_addr = (UINT*)dest_buffer;

	if (y0 == y1 && y1 == y2)
	{
		return;
	}

	// �ƶ����㣬��p0���Ƕ���,p1p2�ǵױ�
	// ���ң��ж���p1������ߣ�p2�����ұ�
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
	// ����p1���p2�� ��yֵ��һ���ģ����û���
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

	// �鿴clip��Χ
	if (y0 < clipRect->top)	// ������clip���⣬��ȡ�µ�height��xs xe
	{
		delta_hegith = clipRect->top - y0;

		xs += dxy_left * delta_hegith;		// �������յ�
		xe += dxy_right * delta_hegith;

		y0 = clipRect->top;
	}


	if (y1 > clipRect->bottom)				// �ױ߳���clip��
	{
		y1 = y2 = clipRect->bottom;
	}

	dest_addr += y0 * mempitch;		// �����ڴ�����λ��

	// ��ʼclipˮƽɨ���ߣ��ͻ���

	// ������ʱ�򣬵㶼��clip���ڣ�
	if (x0 > clipRect->left&&x0<clipRect->right&&
		x1>clipRect->left&&x1<clipRect->right&&
		x2>clipRect->left&&x2 < clipRect->right)
	{
		for (int i = y0; i <= y2; ++i, dest_addr += mempitch)		// !ע�⣬��i<=y2 ��֮ǰû�м�=��:(
		{
			//			memset((UCHAR*)dest_addr + ((xs + FIXP16_ROUND_UP) >> FIXP16_SHIFT), color, (((xe - xs) + FIXP16_ROUND_UP) >> FIXP16_SHIFT) + 1);
			Mem_Set_UINT(dest_addr + FIXP_2_INT(xs), color, FIXP_2_INT((xe - xs)) + 1);

			xs += dxy_left;
			xe += dxy_right;
		}
	}
	else  // �еĵ㲻��clip����
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
				if (left > clipRect->right)		// �˵���clip���⣬����
				{
					//					dest_addr += mempitch;
					continue;
				}
			}
			if (left < clipRect->left)
			{
				left = clipRect->left;
				if (right < clipRect->left)		// �˵���clip���⣬����
				{
					//					dest_addr += mempitch;
					continue;
				}
			}
			Mem_Set_UINT(dest_addr + left, color, (right - left + 1));
			//			memset((UCHAR*)dest_addr + left, color, (right - left + 1));

						//			dest_addr += mempitch;		// for ѭ���мӹ���
		}

	}
}

void Draw_TriangleFP_2D8(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR *dest_buffer, int mempitch)
{
	// ��yֵ���� y0<y1<y2
	int tmp;
	// �Ǵ�ֱ���߻�����ˮƽ���ߣ�����
	if ((x0 == x1 && x1 == x2) || (y0 == y1 && y1 == y2))
	{
		return;
	}

	if (y1 < y2&&y1 < y0)		// y1�Ƕ���,��0�㽻��
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
	else if (y2 < y1&&y2 < y0)	// y2�Ƕ��㣬��0�㽻��
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

	if (y2 < y1)		// ��yֵ����
	{
		tmp = y1;
		y1 = y2;
		y2 = tmp;

		tmp = x1;
		x1 = x2;
		x2 = tmp;
	}


	// ����Ƿ�ȫ����clip����
	if (y0 > clipRect->bottom ||												// ��clip������
		(y1 < clipRect->top&&y2 < clipRect->top) ||							// ��clip������
		(x0 < clipRect->left&&x1 < clipRect->left&&x2 < clipRect->left) ||			// ��clip������
		(x0 > clipRect->right&&x1 > clipRect->right&&x2 > clipRect->right)		// ��clip������
		)
	{
		return;
	}

	if (y0 == y1 || y0 == y2)		// �Ǹ�ƽ��������
	{
		Draw_Top_TriFP8(clipRect, x0, y0, x1, y1, x2, y2, color, dest_buffer, mempitch);
		return;
	}

	if (y1 == y2)		// �Ǹ�ƽ��������
	{
		Draw_Bottom_TriFP8(clipRect, x0, y0, x1, y1, x2, y2, color, dest_buffer, mempitch);
		return;
	}

	// һ�������Σ��ָ������������
	// ��PtempΪ��ʱ�㣬�Ѿ�֪����Yt=y1,�������������εã�
	// Xt=(x0-x2)*(y1-y2)/(y0-y2)+x2
	float new_x = x2 + (int)((float)(x0 - x2)*(float)(y1 - y2) / (float)(y0 - y2) + 0.5);

	Draw_Bottom_TriFP8(clipRect, x0, y0, x1, y1, new_x, y1, color, dest_buffer, mempitch);	// P0,P1,Pt
	Draw_Top_TriFP8(clipRect, x2, y2, new_x, y1, x1, y1, color, dest_buffer, mempitch);			// P2,Pt,P1
	return;

}

void Draw_TriangleFP_2D16(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR *dest_buffer, int mempitch)
{
	// ��yֵ���� y0<y1<y2
	int tmp;
	// �Ǵ�ֱ���߻�����ˮƽ���ߣ�����
	if ((x0 == x1 && x1 == x2) || (y0 == y1 && y1 == y2))
	{
		return;
	}

	if (y1 < y2&&y1 < y0)		// y1�Ƕ���,��0�㽻��
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
	else if (y2 < y1&&y2 < y0)	// y2�Ƕ��㣬��0�㽻��
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

	if (y2 < y1)		// ��yֵ����
	{
		tmp = y1;
		y1 = y2;
		y2 = tmp;

		tmp = x1;
		x1 = x2;
		x2 = tmp;
	}


	// ����Ƿ�ȫ����clip����
	if (y0 > clipRect->bottom ||												// ��clip������
		(y1 < clipRect->top&&y2 < clipRect->top) ||							// ��clip������
		(x0 < clipRect->left&&x1 < clipRect->left&&x2 < clipRect->left) ||			// ��clip������
		(x0 > clipRect->right&&x1 > clipRect->right&&x2 > clipRect->right)		// ��clip������
		)
	{
		return;
	}

	if (y0 == y1 || y0 == y2)		// �Ǹ�ƽ��������
	{
		Draw_Top_TriFP16(clipRect, x0, y0, x1, y1, x2, y2, color, dest_buffer, mempitch);
		return;
	}

	if (y1 == y2)		// �Ǹ�ƽ��������
	{
		Draw_Bottom_TriFP16(clipRect, x0, y0, x1, y1, x2, y2, color, dest_buffer, mempitch);
		return;
	}

	// һ�������Σ��ָ������������
	// ��PtempΪ��ʱ�㣬�Ѿ�֪����Yt=y1,�������������εã�
	// Xt=(x0-x2)*(y1-y2)/(y0-y2)+x2
	float new_x = x2 + (int)((float)(x0 - x2)*(float)(y1 - y2) / (float)(y0 - y2) + 0.5);

	Draw_Bottom_TriFP16(clipRect, x0, y0, x1, y1, new_x, y1, color, dest_buffer, mempitch);	// P0,P1,Pt
	Draw_Top_TriFP16(clipRect, x2, y2, new_x, y1, x1, y1, color, dest_buffer, mempitch);			// P2,Pt,P1
	return;

}

void Draw_TriangleFP_2D32(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR *dest_buffer, int mempitch)
{
	// ��yֵ���� y0<y1<y2
	int tmp;
	// �Ǵ�ֱ���߻�����ˮƽ���ߣ�����
	if ((x0 == x1 && x1 == x2) || (y0 == y1 && y1 == y2))
	{
		return;
	}

	if (y1 < y2&&y1 < y0)		// y1�Ƕ���,��0�㽻��
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
	else if (y2 < y1&&y2 < y0)	// y2�Ƕ��㣬��0�㽻��
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

	if (y2 < y1)		// ��yֵ����
	{
		tmp = y1;
		y1 = y2;
		y2 = tmp;

		tmp = x1;
		x1 = x2;
		x2 = tmp;
	}


	// ����Ƿ�ȫ����clip����
	if (y0 > clipRect->bottom ||												// ��clip������
		(y1 < clipRect->top&&y2 < clipRect->top) ||							// ��clip������
		(x0 < clipRect->left&&x1 < clipRect->left&&x2 < clipRect->left) ||			// ��clip������
		(x0 > clipRect->right&&x1 > clipRect->right&&x2 > clipRect->right)		// ��clip������
		)
	{
		return;
	}

	if (y0 == y1 || y0 == y2)		// �Ǹ�ƽ��������
	{
		Draw_Top_TriFP32(clipRect, x0, y0, x1, y1, x2, y2, color, dest_buffer, mempitch);
		return;
	}

	if (y1 == y2)		// �Ǹ�ƽ��������
	{
		Draw_Bottom_TriFP32(clipRect, x0, y0, x1, y1, x2, y2, color, dest_buffer, mempitch);
		return;
	}

	// һ�������Σ��ָ������������
	// ��PtempΪ��ʱ�㣬�Ѿ�֪����Yt=y1,�������������εã�
	// Xt=(x0-x2)*(y1-y2)/(y0-y2)+x2
	float new_x = x2 + (int)((float)(x0 - x2)*(float)(y1 - y2) / (float)(y0 - y2) + 0.5);

	Draw_Bottom_TriFP32(clipRect, x0, y0, x1, y1, new_x, y1, color, dest_buffer, mempitch);	// P0,P1,Pt
	Draw_Top_TriFP32(clipRect, x2, y2, new_x, y1, x1, y1, color, dest_buffer, mempitch);			// P2,Pt,P1
	return;

}

inline void Draw_QuadFP_2D(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, int color, UCHAR *dest_buffer, int mempitch)
{
	Draw_TriangleFP_2D(clipRect, x0, y0, x1, y1, x3, y3, color, dest_buffer, mempitch);
	Draw_TriangleFP_2D(clipRect, x1, y1, x2, y2, x3, y3, color, dest_buffer, mempitch);
	//	Draw_TriangleFP_2D8(clipRect, x0, y0, x1, y1, x3, y3, color, dest_buffer, mempitch);
	//	Draw_TriangleFP_2D8(clipRect, x1, y1, x2, y2, x3, y3, color, dest_buffer, mempitch);
}

// �ı��εĶ���0,1,2,3˳ʱ�봫�룬��ô�������ָ��������Ϊ�� <0,1,3> <1,2,3>
inline void Draw_Quad_2D(PRECT p_clipRect, int x0, int y0, int x1, int y1,
	int x2, int y2, int x3, int y3,
	int color, UCHAR *dest_buffer, int mempitch)
{

	Draw_Triangle_2D(p_clipRect, x0, y0, x1, y1, x3, y3, color, dest_buffer, mempitch);
	Draw_Triangle_2D(p_clipRect, x1, y1, x2, y2, x3, y3, color, dest_buffer, mempitch);
	//	Draw_Triangle_2D8(p_clipRect, x0, y0, x1, y1, x3, y3, color, dest_buffer, mempitch);
	//	Draw_Triangle_2D8(p_clipRect, x1, y1, x2, y2, x3, y3, color, dest_buffer, mempitch);

}

#pragma endregion


#pragma region ��ײ��ⲿ�ֵĺ���
// ���ټ���<x,y>��<0,0>�ľ��롣ʹ����̩��չ��ʽ����û�����ף�
int Fast_Distance_2D(int x, int y)
{
	x = abs(x);
	y = abs(y);
	int min = MIN(x, y);

	// ��һ��û�����ס�min��Ϊʲô����ô�õ�̩�ռ�����
	return (x + y - (min >> 1) - (min >> 2) + (min >> 4));
}

int Find_Bounding_Box_Poly2D(POLYGON2D_PTR poly, BOUND2DF_PTR bound)
{
	bound->max_x = bound->max_y = bound->min_x = bound->min_y = 0;
	if (poly->num_verts <= 0)
	{
		return 0;
	}
	for (int i = 0; i < poly->num_verts; ++i)
	{
		if (poly->vlist[i].x < bound->min_x)
		{
			bound->min_x = poly->vlist[i].x;
		}
		if (poly->vlist[i].x > bound->max_x)
		{
			bound->max_x = poly->vlist[i].x;
		}

		if (poly->vlist[i].y < bound->min_y)
		{
			bound->min_y = poly->vlist[i].y;
		}
		if (poly->vlist[i].y > bound->max_y)
		{
			bound->max_y = poly->vlist[i].y;
		}
	}
	return 1;
}

#pragma endregion

#pragma region Matrix ����

int Mat_Init_1X2(MATRIX1X2_PTR mat, float x, float y)
{
	mat->M[0] = x;
	mat->M[1] = y;
	return 1;
}
int Mat_Init_3X2(MATRIX3X2_PTR mat,
	float m00, float m01,
	float m10, float m11,
	float m20, float m21)
{
	mat->M[0][0] = m00; mat->M[0][1] = m01;
	mat->M[1][0] = m10; mat->M[1][1] = m11;
	mat->M[2][0] = m20; mat->M[2][1] = m21;

	return 1;
}
int Mat_Mul_3X3(MATRIX3X3_PTR ma, MATRIX3X3_PTR mb, MATRIX3X3_PTR mprod)
{
	for (int row = 0; row < 3; ++row)
	{
		for (int col = 0; col < 3; ++col)
		{
			float sum = 0;
			sum = ma->M[row][0] * mb->M[0][col] + ma->M[row][1] * mb->M[1][col] + ma->M[row][2] * mb->M[2][col];
			mprod->M[row][col] = sum;
		}
	}
	return 1;
}

int Mat_Mul_1X3_3X3(MATRIX1X3_PTR ma, MATRIX3X3_PTR mb, MATRIX1X3_PTR mprod)
{
	for (int col = 0; col < 3; col++)
	{
		float sum = 0;
		for (int index = 0; index < 3; ++index)
		{

			sum += (ma->M[index] * mb->M[index][col]);
		}
		mprod->M[col] = sum;
	}
	return 1;
}

int Mat_Mul_1X2_3X2(MATRIX1X2_PTR ma, MATRIX3X2_PTR mb, MATRIX1X2_PTR mprod)
{
	for (int col = 0; col < 2; ++col)
	{
		float sum = 0;
		int index = 0;
		for (; index < 2; ++index)
		{
			sum += (ma->M[index] * mb->M[index][col]);
		}
		sum += mb->M[index][col];		// ע�⣬����Ҫ����[3][col]��������ƽ�ƣ���
		mprod->M[col] = sum;
	}
	return 0;
}

int SwapInt(int &a, int &b)
{
	int tmp = a;
	a = b;
	b = tmp;

	return 1;
}

int SwapFloat(float &a, float &b)
{
	float tmp = a;
	a = b;
	b = tmp;
	return 1;
}

#pragma endregion Matrix����


#pragma region Bitmap ���

// ���·�תbitmap�ڴ�
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

// ����bitmap�ļ�
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

	// ��ȡbitmap�ļ�ͷ���ֵ���Ϣ
	_lread(file_handle, &bitmap->bitmapfileHeader, sizeof(BITMAPFILEHEADER));

	// �����ȡ֮�󣬲���bitmap����
	if (bitmap->bitmapfileHeader.bfType != BITMAP_ID)
	{
		_lclose(file_handle);
		printf("load_bitmap_file error! read headinfo failed!!");
		return 0;
	}

	// ��ȡbitmapͷ���ֵ���Ϣ
	_lread(file_handle, &bitmap->bitmapInfoHeader, sizeof(BITMAPINFOHEADER));

	// �����8λģʽ����ȡ��ɫ����Ϣ
	// ���ҵ���rgb˳�򡣶�ȡ��Ϣ��BGR˳��Paletteentry��RGB˳��. G��˳���øı�.
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

	// ��󣬶�ȡ������Ϣ
	// ��λ��ʼλ��
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

		// ��ȡ��buffer��
		_lread(file_handle, bitmap->buffer, bitmap->bitmapInfoHeader.biSizeImage);

	}
	else		// ����λ�����أ�serious problem) (����32λ����)
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

// �ͷ��ڴ�
int Unload_Bitmap_Flie(BITMAP_FILE_PTR bitmap)
{
	if (bitmap->buffer)
	{
		free(bitmap->buffer);
		bitmap->buffer = NULL;
	}

	return 1;
}


// ��bitmap��cy�У�cx��sprite��������Ⱦ��lpdds
int Scan_Image_Bitmap8(BITMAP_FILE_PTR bitmap,     // bitmap file to scan image data from
	LPDIRECTDRAWSURFACE7 lpdds, // surface to hold data
	int cx, int cy)             // cell to scan image from
{
	UCHAR * source_ptr;
	UCHAR * dest_ptr;
	DDSURFACEDESC2 ddsd;
	ddsd.dwSize = sizeof(ddsd);

	lpdds->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);

	// ������surface�еĿ�ʼ��λ��(ƽ�Ƶ����λ�õ�(0,0)��)
	// ��Ϊʹ�õ���ר�����ڼ���һ֡�������ڴ棬ddsd�Ŀ�Ⱥ͸߶ȶ�����ǰ�趨�õ�һ��sprite�Ŀ�Ⱥ͸߶ȣ�72x80��
	// ��1�����صĿհ�
	int gwidth = ddsd.dwWidth;
	int gheight = ddsd.dwHeight;
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

// ��bitmap��cy�У�cx��sprite��������Ⱦ��lpdds
int Scan_Image_Bitmap24(BITMAP_FILE_PTR bitmap,     // bitmap file to scan image data from
	LPDIRECTDRAWSURFACE7 lpdds, // surface to hold data
	int cx, int cy)             // cell to scan image from
{
	DWORD * dest_ptr;
	DDSURFACEDESC2 ddsd;
	DDRAW_INIT_STRUCT(ddsd);

	lpdds->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);

	// ������surface�еĿ�ʼ��λ��(ƽ�Ƶ����λ�õ�(0,0)��)
	// ��Ϊʹ�õ���ר�����ڼ���һ֡�������ڴ棬ddsd�Ŀ�Ⱥ͸߶ȶ�����ǰ�趨�õ�һ��sprite�Ŀ�Ⱥ͸߶ȣ�72x80��
	// ��1�����صĿհ�
	int gwidth = ddsd.dwWidth;
	int gheight = ddsd.dwHeight;
	int bitmapWidth = bitmap->bitmapInfoHeader.biWidth;

	cx = cx * (gwidth + 1) + 1;		// x
	cy = cy * (gheight + 1) + 1;		// y


	// 24λλͼ��һ������ռ3���ֽ�
	dest_ptr = (DWORD *)ddsd.lpSurface;

	int xoff = cx * 3;
	int yoff = cy * bitmapWidth * 3;

	for (int y = 0; y < gheight; ++y)
	{
		for (int x = 0; x < gwidth; ++x)
		{

			int off = xoff + yoff + y * bitmapWidth * 3 + x * 3;

			// ������ת��
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

#pragma endregion

#pragma region Draw Text

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


#pragma endregion

#pragma region Draw Lines

// vb_start: video buffer start
int Draw_Line8(int x0, int y0, int x1, int y1, int color, UCHAR *vb_start, int lpitch)
{
	int dx, dy, dx2, dy2, error;
	int x_inc, y_inc;
	int index;

	vb_start = vb_start + x0 + y0 * lpitch;

	dx = x1 - x0;
	dy = y1 - y0;


	if (dx >= 0)	//  ��dx��Զ����ֵ,x_inc��ʾx�任�ķ���
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

	// �ŵ�dx dy ��Ϊ��֮������λ
	dx2 = dx << 1;
	dy2 = dy << 1;

	if (dx > dy)	// ��x��
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
int Draw_Line16(int x0, int y0, int x1, int y1, int color, UCHAR *vb_start, int lpitch)
{
	int dx, dy, dx2, dy2, error;
	int x_inc, y_inc;
	int index;
	int x = x0, y = y0;

	USHORT * vb_dest = (USHORT*)vb_start;
	lpitch = (lpitch >> 1);
	vb_dest = vb_dest + x0 + y0 * lpitch;

	dx = x1 - x0;
	dy = y1 - y0;


	if (dx >= 0)	//  ��dx��Զ����ֵ,x_inc��ʾx�任�ķ���
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

	if (dx > dy)	// ��x��
	{
		error = dy2 - dx;
		for (index = 0; index <= dx; ++index)
		{
			*vb_dest = color;
			if (error > 0)
			{
				error -= dx2;
				y += y_inc;
				vb_dest = vb_dest + y_inc * lpitch;
			}
			error += dy2;
			x += x_inc;
			vb_dest = vb_dest + x_inc;
		}
	}
	else
	{
		error = dx2 - dy;
		for (index = 0; index <= dy; index++)
		{
			*vb_dest = color;
			if (error >= 0)
			{
				error -= dy2;
				x += x_inc;
				vb_dest = vb_dest + x_inc;
			}
			error += dx2;
			y += y_inc;
			vb_dest = vb_dest + y_inc * lpitch;
		}
	}

	return 1;
}

// vb_start: video buffer start
int Draw_Line32(int x0, int y0, int x1, int y1, int color, UCHAR *vb_start, int lpitch)
{
	int dx, dy, dx2, dy2, error;
	int x_inc, y_inc;
	int index;
	int x = x0, y = y0;

	UINT* vb_dest = (UINT*)vb_start;
	lpitch = (lpitch >> 2);
	vb_dest = vb_dest + x0 + y0 * lpitch;

	dx = x1 - x0;
	dy = y1 - y0;


	if (dx >= 0)	//  ��dx��Զ����ֵ,x_inc��ʾx�任�ķ���
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

	if (dx > dy)	// ��x��
	{
		error = dy2 - dx;
		for (index = 0; index <= dx; ++index)
		{
			*vb_dest = color;
			if (error > 0)
			{
				error -= dx2;
				y += y_inc;
				vb_dest = vb_dest + y_inc * lpitch;
			}
			error += dy2;
			x += x_inc;
			vb_dest = vb_dest + x_inc;
		}
	}
	else
	{
		error = dx2 - dy;
		for (index = 0; index <= dy; index++)
		{
			*vb_dest = color;
			if (error >= 0)
			{
				error -= dy2;
				x += x_inc;
				vb_dest = vb_dest + x_inc;
			}
			error += dx2;
			y += y_inc;
			vb_dest = vb_dest + y_inc * lpitch;
		}
	}

	return 1;
}

// �ü��߶Ρ�����ֵΪ1��ʱ��û����ȫ���ü���������ֵΪ0��ʱ����ȫ���ü�����
int Clip_Line(LPRECT clipRect, int &x0, int &y0, int &x1, int &y1)
{
	int point0_code = CLIP_CODE_C, point1_code = CLIP_CODE_C;

	// line's pointcode
	if (x0 < clipRect->left)
	{
		point0_code |= CLIP_CODE_W;
	}
	else if (x0 > clipRect->right)
	{
		point0_code |= CLIP_CODE_E;
	}

	if (y0 < clipRect->top)
	{
		point0_code |= CLIP_CODE_N;
	}
	else if (y0 > clipRect->bottom)
	{
		point0_code |= CLIP_CODE_S;
	}


	if (x1 < clipRect->left)
	{
		point1_code |= CLIP_CODE_W;
	}
	else if (x1 > clipRect->right)
	{
		point1_code |= CLIP_CODE_E;
	}

	if (y1 < clipRect->top)
	{
		point1_code |= CLIP_CODE_N;
	}
	else if (y1 > clipRect->bottom)
	{
		point1_code |= CLIP_CODE_S;
	}

	// �ж϶˵�ļ��������
	if (point0_code == CLIP_CODE_C && point1_code == CLIP_CODE_C)		// �˵㶼��rect�ڣ������ü�
	{
		return 1;

	}
	if (point0_code & point1_code)		// �˵㶼��ĳһ�ߵ�rect�⣬��ȫ�ü���
	{
		return 0;
	}

	// һ���˵����⣬һ���˵����ڣ���Ҫ�ü�һ����. �ü��õ��µĶ˵�; �����������˵㶼��rect�⣬�����߶λᾭ���ü����򣨶˵�ü��������
	// 2.���¶˵��滻��rect��Ķ˵�

	// 1.������봹ֱ����ˮƽ�ߵĽ���  (������������������Ż��� 1.x1=x0;2.y1=y0)
	// ��ͨ�����m=(y1-y0)/(x1-x0)  y=m*(x-x1)+y1  x=(y-y1)/m+x1

	if (x0 == x1)		// ��һ����ֱ����
	{
		if (point0_code != CLIP_CODE_C)		// point0������
		{
			//			outpoint_index = 0;
			//			inter_x = x0;
			if (point0_code&CLIP_CODE_N)		// ��north
			{
				y0 = clipRect->top;
			}
			else   // ��south
			{
				y0 = clipRect->bottom;
			}
		}
		if (point1_code != CLIP_CODE_C)		// point1������
		{
			//			outpoint_index = 1;
			//			inter_x = x1;
			if (point1_code&CLIP_CODE_N)
			{
				y1 = clipRect->top;
			}
			else
			{
				y1 = clipRect->bottom;
			}
		}
		return 1;
	} // end if  x0==x1

	if (y0 == y1)  //��ˮƽ���ཻ
	{
		if (point0_code != CLIP_CODE_C)		// point0������
		{
			//			outpoint_index = 0;
			//			inter_y = y0;
			if (point0_code&CLIP_CODE_W)		// ��west
			{
				x0 = clipRect->left;
			}
			else   // ��east
			{
				x0 = clipRect->right;
			}
		}
		if (point1_code != CLIP_CODE_C)		// point1������
		{
			//			outpoint_index = 1;
			//			inter_y = y1;
			if (point1_code&CLIP_CODE_W)
			{
				x1 = clipRect->left;
			}
			else
			{
				x1 = clipRect->right;
			}
		}
		return 1;
	} // end if y0==y1

	// һ����� 
	float m = (y1 - y0)*1.0f / (x1 - x0);

	// ��ͨ�����m=(y1-y0)/(x1-x0)  y=m*(x-x1)+y1  x=(y-y1)/m+x1
	switch (point0_code)
	{
	case CLIP_CODE_N:
		y0 = clipRect->top;
		x0 = (y0 - y1) / m + x1;
		break;
	case CLIP_CODE_S:
		y0 = clipRect->bottom;
		x0 = (y0 - y1) / m + x1;
		break;
	case CLIP_CODE_W:
		x0 = clipRect->left;
		y0 = m * (x0 - x1) + y1;
		break;
	case CLIP_CODE_E:
		x0 = clipRect->right;
		y0 = m * (x0 - x1) + y1;
		break;
	case CLIP_CODE_NE:
		// ��N����
		y0 = clipRect->top;		// ���жϽ����Ƿ���N����
		x0 = (y0 - y1) / m + x1;

		if (x0<clipRect->left || x0>clipRect->right)// ������E����
		{
			x0 = clipRect->right;
			y0 = m * (x0 - x1) + y1;
		}
		break;
	case CLIP_CODE_SE:
		// S����
		y0 = clipRect->bottom;
		x0 = (y0 - y1) / m + x1;
		if (x0<clipRect->left || x0>clipRect->right)  // E����
		{
			x0 = clipRect->right;
			y0 = m * (x0 - x1) + y1;
		}
		break;
	case CLIP_CODE_NW:
		// N��
		y0 = clipRect->top;
		x0 = (y0 - y1) / m + x1;
		if (x0<clipRect->left || x0>clipRect->right)
		{
			x0 = clipRect->left;
			y0 = m * (x0 - x1) + y1;
		}
		break;
	case CLIP_CODE_SW:
		// s line
		y0 = clipRect->bottom;
		x0 = (y0 - y1) / m + x1;

		if (x0<clipRect->left || x0>clipRect->right)
		{
			x0 = clipRect->left;
			y0 = m * (x0 - x1) + y1;
		}
		break;
	}

	// ��ͨ�����m=(y1-y0)/(x1-x0)  y=m*(x-x0)+y0  x=(y-y0)/m+x0
	switch (point1_code)
	{
	case CLIP_CODE_N:
		y1 = clipRect->top;
		x1 = (y1 - y0) / m + x0;
		break;
	case CLIP_CODE_S:
		y1 = clipRect->bottom;
		x1 = (y1 - y0) / m + x0;
		break;
	case CLIP_CODE_W:
		x1 = clipRect->left;
		y1 = m * (x1 - x0) + y0;
		break;
	case CLIP_CODE_E:
		x1 = clipRect->right;
		y1 = m * (x1 - x0) + y0;
		break;
	case CLIP_CODE_NE:
		// ��N����
		y1 = clipRect->top;		// ���жϽ����Ƿ���N����
		x1 = (y1 - y0) / m + x0;
		if (x1<clipRect->left || x1>clipRect->right)// ������E����
		{
			x1 = clipRect->right;
			y1 = m * (x1 - x0) + y0;
		}
		break;
	case CLIP_CODE_SE:
		// S����
		y1 = clipRect->bottom;
		x1 = (y1 - y0) / m + x0;
		if (x1<clipRect->left || x1>clipRect->right)  // E����
		{
			x1 = clipRect->right;
			y1 = m * (x1 - x0) + y0;
		}
		break;
	case CLIP_CODE_NW:
		// N��
		y1 = clipRect->top;
		x1 = (y1 - y0) / m + x0;
		if (x1<clipRect->left || x1>clipRect->right)
		{
			x1 = clipRect->left;
			y1 = m * (x1 - x0) + y0;
		}
		break;
	case CLIP_CODE_SW:
		// on S line
		y1 = clipRect->bottom;
		x1 = (y1 - y0) / m + x0;

		if (x1<clipRect->left || x1>clipRect->right)
		{
			x1 = clipRect->left;
			y1 = m * (x1 - x0) + y0;
		}
		break;
	}
	// �����checkһ�¶˵��Ƿ���ȷ. �д���ͷ���0
	if (x0<clipRect->left || x0>clipRect->right
		|| x1<clipRect->left || y1>clipRect->right
		|| y0<clipRect->top || y0>clipRect->bottom
		|| y1<clipRect->top || y1>clipRect->bottom)
	{
		//		printf("clip (%d,%d),(%d,%d)", x0, y0, x1, y1);
		return 0;
	}
	return 1;
}

void Draw_Clip_Line8(LPRECT clipRect, int x0, int y0, int x1, int y1, int color, UCHAR *vb_start, int lpitch)
{
	int cx0 = x0, cy0 = y0, cx1 = x1, cy1 = y1;
	if (Clip_Line(clipRect, cx0, cy0, cx1, cy1))
	{
		Draw_Line8(cx0, cy0, cx1, cy1, color, vb_start, lpitch);
	}
}

void Draw_Clip_Line16(LPRECT clipRect, int x0, int y0, int x1, int y1, int color, UCHAR *vb_start, int lpitch)
{
	int cx0 = x0, cy0 = y0, cx1 = x1, cy1 = y1;
	if (Clip_Line(clipRect, cx0, cy0, cx1, cy1))
	{
		Draw_Line16(cx0, cy0, cx1, cy1, color, vb_start, lpitch);
	}
}

void Draw_Clip_Line32(LPRECT clipRect, int x0, int y0, int x1, int y1, int color, UCHAR *vb_start, int lpitch)
{
	int cx0 = x0, cy0 = y0, cx1 = x1, cy1 = y1;
	if (Clip_Line(clipRect, cx0, cy0, cx1, cy1))
	{
		Draw_Line32(cx0, cy0, cx1, cy1, color, vb_start, lpitch);
	}
}

void HLine8(LPRECT clipRect, int x1, int x2, int y, int color, UCHAR *vbuffer, int lpitch)
{
	if (y<clipRect->top || y>clipRect->bottom)
		return;
	if (x2 < x1)
	{
		SwapInt(x2, x1);
	}
	if (x1 > clipRect->right)
		return;
	if (x2 < clipRect->left)
		return;

	if (x1 < clipRect->left)
		x1 = clipRect->left;
	if (x2 > clipRect->right)
		x2 = clipRect->right;

	memset((UCHAR*)vbuffer + x1 + y * lpitch, color, x2 - x1 + 1);

}
void VLine8(LPRECT clipRect, int y1, int y2, int x, int color, UCHAR *vbuffer, int lpitch)
{
	if (x<clipRect->left || x>clipRect->right)
		return;
	if (y2 < y1)
	{
		SwapInt(y1, y2);
	}

	y1 = (y1 < clipRect->top) ? clipRect->top : y1;
	y2 = (y2 > clipRect->bottom) ? clipRect->bottom : y2;
	vbuffer += x;

	for (int i = y1; i <= y2; ++i)
	{
		*vbuffer = color;
		vbuffer += lpitch;
	}

}
void HLine16(LPRECT clipRect, int x1, int x2, int y, int color, UCHAR *vbuffer, int lpitch)
{
	if (y<clipRect->top || y>clipRect->bottom)
		return;
	if (x2 < x1)
	{
		SwapInt(x2, x1);
	}
	if (x1 > clipRect->right)
		return;
	if (x2 < clipRect->left)
		return;

	if (x1 < clipRect->left)
		x1 = clipRect->left;
	if (x2 > clipRect->right)
		x2 = clipRect->right;

	Mem_Set_USHORT(((USHORT*)vbuffer) + x1 + y * (lpitch >> 1), color, (x2 - x1 + 1));

}
void VLine16(LPRECT clipRect, int y1, int y2, int x, int color, UCHAR *vbuffer, int lpitch)
{
	if (x<clipRect->left || x>clipRect->right)
		return;
	if (y2 < y1)
	{
		SwapInt(y1, y2);
	}

	y1 = (y1 < clipRect->top) ? clipRect->top : y1;
	y2 = (y2 > clipRect->bottom) ? clipRect->bottom : y2;

	USHORT* vb_dest = (USHORT*)vbuffer;
	lpitch = (lpitch >> 1);
	vb_dest += x;
	for (int i = y1; i <= y2; ++i)
	{
		*vb_dest = color;
		vb_dest += lpitch;
	}

}

void HLine32(LPRECT clipRect, int x1, int x2, int y, int color, UCHAR *vbuffer, int lpitch)
{
	if (y<clipRect->top || y>clipRect->bottom)
		return;
	if (x2 < x1)
	{
		SwapInt(x2, x1);
	}
	if (x1 > clipRect->right)
		return;
	if (x2 < clipRect->left)
		return;

	if (x1 < clipRect->left)
		x1 = clipRect->left;
	if (x2 > clipRect->right)
		x2 = clipRect->right;

	Mem_Set_UINT(((UINT*)vbuffer) + x1 + y * (lpitch >> 2), color, (x2 - x1 + 1));

}
void VLine32(LPRECT clipRect, int y1, int y2, int x, int color, UCHAR *vbuffer, int lpitch)
{
	if (x<clipRect->left || x>clipRect->right)
		return;
	if (y2 < y1)
	{
		SwapInt(y1, y2);
	}

	y1 = (y1 < clipRect->top) ? clipRect->top : y1;
	y2 = (y2 > clipRect->bottom) ? clipRect->bottom : y2;

	UINT* vb_dest = (UINT*)vbuffer;
	lpitch = (lpitch >> 2);
	vb_dest += x;
	for (int i = y1; i <= y2; ++i)
	{
		*vb_dest = color;
		vb_dest += lpitch;
	}

}

#pragma endregion 

#pragma region Draw Polygon2D

int Draw_Polygon2D8(LPRECT clipRect, POLYGON2D_PTR poly, UCHAR *vbuffer, int lpitch)
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

int Draw_Polygon2D16(LPRECT clipRect, POLYGON2D_PTR poly, UCHAR *vbuffer, int lpitch)
{
	if (poly->state)
	{
		int i = 0;
		for (; i < poly->num_verts - 1; ++i)
		{
			Draw_Clip_Line16(clipRect, poly->vlist[i].x + poly->x0, poly->vlist[i].y + poly->y0, poly->vlist[i + 1].x + poly->x0, poly->vlist[i + 1].y + poly->y0, poly->color, vbuffer, lpitch);
		}
		Draw_Clip_Line16(clipRect, poly->vlist[i].x + poly->x0, poly->vlist[i].y + poly->y0, poly->vlist[0].x + poly->x0, poly->vlist[0].y + poly->y0, poly->color, vbuffer, lpitch);
		return 1;
	}
	return 0;
}

int Draw_Polygon2D32(LPRECT clipRect, POLYGON2D_PTR poly, UCHAR *vbuffer, int lpitch)
{
	if (poly->state)
	{
		int i = 0;
		for (; i < poly->num_verts - 1; ++i)
		{
			Draw_Clip_Line32(clipRect, poly->vlist[i].x + poly->x0, poly->vlist[i].y + poly->y0, poly->vlist[i + 1].x + poly->x0, poly->vlist[i + 1].y + poly->y0, poly->color, vbuffer, lpitch);
		}
		Draw_Clip_Line32(clipRect, poly->vlist[i].x + poly->x0, poly->vlist[i].y + poly->y0, poly->vlist[0].x + poly->x0, poly->vlist[0].y + poly->y0, poly->color, vbuffer, lpitch);
		return 1;
	}
	return 0;
}

// �������
void Draw_Filled_Polygon2D8(PRECT clipRect, POLYGON2D_PTR poly, UCHAR *vbuffer, int mempitch)
{
	int startIndex, startY;
	int endIndex, endY;

	// �ҵ����index
	startIndex = 0;
	startY = poly->vlist[0].y + poly->y0;;

	// �ҵ�������yֵ
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

	// ��������ɨ���ߣ����������Ȳ��������ҡ�
	int start1Index = startIndex;
	int start2Index = startIndex;
	// 1�ߺ�2���ϵĵ㣬��Ϊ�����յ�
	float x1, x2;
	int y1end;
	int y2end;
	float x1end, x2end;

	int x1Error = 0, x2Error = 0;

	x1 = x2 = poly->vlist[startIndex].x + poly->x0;
	y1end = y2end = (poly->vlist[startIndex].y + poly->y0);

	UCHAR *dest_buff;
	dest_buff = vbuffer + startY * mempitch;

	// ���Ż�һ��ɨ���ߣ�
	float dxy1, dxy2;		// ÿ�ƶ���λy,x�ı任����

	// ��ʼ��ɨ����
	while (edgeCount > 0)
	{
		// ��1�ߣ�ֱ���ҵ����ʵ�
		while (edgeCount > 0 && (x1Error || startY >= y1end))
		{
			int pointIndex = start1Index - 1;
			if (pointIndex < 0)
			{
				pointIndex = poly->num_verts - 1;
			}
			edgeCount--;
			float height = poly->vlist[pointIndex].y - poly->vlist[start1Index].y;
			if (height == 0)	// ��ˮƽ��,yֵһ��
			{
				x1 = poly->vlist[pointIndex].x + poly->x0;
				start1Index = pointIndex;
				continue;		// ����Ѱ�Һ��ʵ�ˮƽ��
			}
			else  // �γ���һ��б��
			{
				dxy1 = (float)((poly->vlist[pointIndex].x - poly->vlist[start1Index].x)) / height;

				x1 = poly->vlist[start1Index].x + poly->x0;		// ��x1�ûص��ߵ����
				y1end = poly->vlist[pointIndex].y + poly->y0;
				x1end = poly->vlist[pointIndex].x + poly->x0;

				start1Index = pointIndex;
				break;	// �ҵ����ʵ����ˣ�break
			}
		}

		// �ҵ�2�ߣ�ֱ���ҵ����ʵ�
		while (edgeCount > 0 && (x2Error || startY >= y2end))
		{
			int pointIndex = start2Index + 1;
			if (pointIndex >= poly->num_verts)
			{
				pointIndex = 0;
			}
			edgeCount--;
			float height = poly->vlist[pointIndex].y - poly->vlist[start2Index].y;
			if (height == 0)	// ��ˮƽ��
			{
				x2 = poly->vlist[pointIndex].x + poly->x0;
				start2Index = pointIndex;
				continue;		// ����Ѱ�Һ��ʵ�ˮƽ��
			}
			else  // �γ���һ��б��
			{
				dxy2 = ((float)(poly->vlist[pointIndex].x - poly->vlist[start2Index].x)) / height;

				x2 = poly->vlist[start2Index].x + poly->x0;
				y2end = poly->vlist[pointIndex].y + poly->y0;
				x2end = poly->vlist[pointIndex].x + poly->x0;

				start2Index = pointIndex;
				break;	// �ҵ����ʵ����ˣ�break
			}
		}

		// Ҫ��������

		x1Error = 0;
		x2Error = 0;
		// ֻҪ�����յ㣬��һֱ���»���
		// ע�⣬��Ҫ���ںš� �����ߵ�����һ�ߵĿ�ʼ��(���Ľ����Ľ���᲻����һ���ߣ�)
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
			startY++;		// y����һ��

			// �ж�ɨ���ߵ�x�Ƿ񳬹����ߵ��յ㡣������б�ʱȽϴ��µ�)
			if (dxy1 > 0)	// ���ӵ�
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

void Draw_Filled_Polygon2D16(PRECT clipRect, POLYGON2D_PTR poly, UCHAR *vbuffer, int mempitch)
{
	int startIndex, startY;
	int endIndex, endY;

	// �ҵ����index
	startIndex = 0;
	startY = poly->vlist[0].y + poly->y0;;

	// �ҵ�������yֵ
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

	// ��������ɨ���ߣ����������Ȳ��������ҡ�
	int start1Index = startIndex;
	int start2Index = startIndex;
	// 1�ߺ�2���ϵĵ㣬��Ϊ�����յ�
	float x1, x2;
	int y1end;
	int y2end;
	float x1end, x2end;

	int x1Error = 0, x2Error = 0;

	x1 = x2 = poly->vlist[startIndex].x + poly->x0;
	y1end = y2end = (poly->vlist[startIndex].y + poly->y0);

	USHORT *dest_buff = (USHORT*)vbuffer;
	mempitch = (mempitch >> 1);
	dest_buff += startY * mempitch;

	// ���Ż�һ��ɨ���ߣ�
	float dxy1, dxy2;		// ÿ�ƶ���λy,x�ı任����

	// ��ʼ��ɨ����
	while (edgeCount > 0)
	{
		// ��1�ߣ�ֱ���ҵ����ʵ�
		while (edgeCount > 0 && (x1Error || startY >= y1end))
		{
			int pointIndex = start1Index - 1;
			if (pointIndex < 0)
			{
				pointIndex = poly->num_verts - 1;
			}
			edgeCount--;
			float height = poly->vlist[pointIndex].y - poly->vlist[start1Index].y;
			if (height == 0)	// ��ˮƽ��,yֵһ��
			{
				x1 = poly->vlist[pointIndex].x + poly->x0;
				start1Index = pointIndex;
				continue;		// ����Ѱ�Һ��ʵ�ˮƽ��
			}
			else  // �γ���һ��б��
			{
				dxy1 = (float)((poly->vlist[pointIndex].x - poly->vlist[start1Index].x)) / height;

				x1 = poly->vlist[start1Index].x + poly->x0;		// ��x1�ûص��ߵ����
				y1end = poly->vlist[pointIndex].y + poly->y0;
				x1end = poly->vlist[pointIndex].x + poly->x0;

				start1Index = pointIndex;
				break;	// �ҵ����ʵ����ˣ�break
			}
		}

		// �ҵ�2�ߣ�ֱ���ҵ����ʵ�
		while (edgeCount > 0 && (x2Error || startY >= y2end))
		{
			int pointIndex = start2Index + 1;
			if (pointIndex >= poly->num_verts)
			{
				pointIndex = 0;
			}
			edgeCount--;
			float height = poly->vlist[pointIndex].y - poly->vlist[start2Index].y;
			if (height == 0)	// ��ˮƽ��
			{
				x2 = poly->vlist[pointIndex].x + poly->x0;
				start2Index = pointIndex;
				continue;		// ����Ѱ�Һ��ʵ�ˮƽ��
			}
			else  // �γ���һ��б��
			{
				dxy2 = ((float)(poly->vlist[pointIndex].x - poly->vlist[start2Index].x)) / height;

				x2 = poly->vlist[start2Index].x + poly->x0;
				y2end = poly->vlist[pointIndex].y + poly->y0;
				x2end = poly->vlist[pointIndex].x + poly->x0;

				start2Index = pointIndex;
				break;	// �ҵ����ʵ����ˣ�break
			}
		}

		// Ҫ��������

		x1Error = 0;
		x2Error = 0;
		// ֻҪ�����յ㣬��һֱ���»���
		// ע�⣬��Ҫ���ںš� �����ߵ�����һ�ߵĿ�ʼ��(���Ľ����Ľ���᲻����һ���ߣ�)
		while (startY < y1end && startY < y2end)
		{
			if (x1 < x2)
			{
				//				memset(dest_buff + (int)x1, poly->color, (int)(x2 - x1 + 1));
				Mem_Set_USHORT(dest_buff + (int)x1, poly->color, (int)(x2 - x1 + 1));
			}
			else
			{
				//				memset(dest_buff + (int)x2, poly->color, (int)(x1 - x2 + 1));
				Mem_Set_USHORT(dest_buff + (int)x2, poly->color, (int)(x1 - x2 + 1));
			}
			dest_buff += mempitch;
			x1 += dxy1;
			x2 += dxy2;
			startY++;		// y����һ��

			// �ж�ɨ���ߵ�x�Ƿ񳬹����ߵ��յ㡣������б�ʱȽϴ��µ�)
			if (dxy1 > 0)	// ���ӵ�
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

void Draw_Filled_Polygon2D32(PRECT clipRect, POLYGON2D_PTR poly, UCHAR *vbuffer, int mempitch)
{
	int startIndex, startY;
	int endIndex, endY;

	// �ҵ����index
	startIndex = 0;
	startY = poly->vlist[0].y + poly->y0;;

	// �ҵ�������yֵ
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

	// ��������ɨ���ߣ����������Ȳ��������ҡ�
	int start1Index = startIndex;
	int start2Index = startIndex;
	// 1�ߺ�2���ϵĵ㣬��Ϊ�����յ�
	float x1, x2;
	int y1end;
	int y2end;
	float x1end, x2end;

	int x1Error = 0, x2Error = 0;

	x1 = x2 = poly->vlist[startIndex].x + poly->x0;
	y1end = y2end = (poly->vlist[startIndex].y + poly->y0);

	UINT *dest_buff = (UINT*)vbuffer;
	mempitch = (mempitch >> 2);
	dest_buff += startY * mempitch;

	// ���Ż�һ��ɨ���ߣ�
	float dxy1, dxy2;		// ÿ�ƶ���λy,x�ı任����

	// ��ʼ��ɨ����
	while (edgeCount > 0)
	{
		// ��1�ߣ�ֱ���ҵ����ʵ�
		while (edgeCount > 0 && (x1Error || startY >= y1end))
		{
			int pointIndex = start1Index - 1;
			if (pointIndex < 0)
			{
				pointIndex = poly->num_verts - 1;
			}
			edgeCount--;
			float height = poly->vlist[pointIndex].y - poly->vlist[start1Index].y;
			if (height == 0)	// ��ˮƽ��,yֵһ��
			{
				x1 = poly->vlist[pointIndex].x + poly->x0;
				start1Index = pointIndex;
				continue;		// ����Ѱ�Һ��ʵ�ˮƽ��
			}
			else  // �γ���һ��б��
			{
				dxy1 = (float)((poly->vlist[pointIndex].x - poly->vlist[start1Index].x)) / height;

				x1 = poly->vlist[start1Index].x + poly->x0;		// ��x1�ûص��ߵ����
				y1end = poly->vlist[pointIndex].y + poly->y0;
				x1end = poly->vlist[pointIndex].x + poly->x0;

				start1Index = pointIndex;
				break;	// �ҵ����ʵ����ˣ�break
			}
		}

		// �ҵ�2�ߣ�ֱ���ҵ����ʵ�
		while (edgeCount > 0 && (x2Error || startY >= y2end))
		{
			int pointIndex = start2Index + 1;
			if (pointIndex >= poly->num_verts)
			{
				pointIndex = 0;
			}
			edgeCount--;
			float height = poly->vlist[pointIndex].y - poly->vlist[start2Index].y;
			if (height == 0)	// ��ˮƽ��
			{
				x2 = poly->vlist[pointIndex].x + poly->x0;
				start2Index = pointIndex;
				continue;		// ����Ѱ�Һ��ʵ�ˮƽ��
			}
			else  // �γ���һ��б��
			{
				dxy2 = ((float)(poly->vlist[pointIndex].x - poly->vlist[start2Index].x)) / height;

				x2 = poly->vlist[start2Index].x + poly->x0;
				y2end = poly->vlist[pointIndex].y + poly->y0;
				x2end = poly->vlist[pointIndex].x + poly->x0;

				start2Index = pointIndex;
				break;	// �ҵ����ʵ����ˣ�break
			}
		}

		// Ҫ��������

		x1Error = 0;
		x2Error = 0;
		// ֻҪ�����յ㣬��һֱ���»���
		// ע�⣬��Ҫ���ںš� �����ߵ�����һ�ߵĿ�ʼ��(���Ľ����Ľ���᲻����һ���ߣ�)
		while (startY < y1end && startY < y2end)
		{
			if (x1 < x2)
			{
				//				memset(dest_buff + (int)x1, poly->color, (int)(x2 - x1 + 1));
				Mem_Set_UINT(dest_buff + (int)x1, poly->color, (int)(x2 - x1 + 1));
			}
			else
			{
				//				memset(dest_buff + (int)x2, poly->color, (int)(x1 - x2 + 1));
				Mem_Set_UINT(dest_buff + (int)x2, poly->color, (int)(x1 - x2 + 1));
			}
			dest_buff += mempitch;
			x1 += dxy1;
			x2 += dxy2;
			startY++;		// y����һ��

			// �ж�ɨ���ߵ�x�Ƿ񳬹����ߵ��յ㡣������б�ʱȽϴ��µ�)
			if (dxy1 > 0)	// ���ӵ�
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

#pragma endregion

#pragma region DDraw��غ���

int DDraw_Init(int width, int height, int bpp, int windowed)
{
	screen_windowed = windowed;
	screen_width = width;
	screen_height = height;
	screen_bpp = bpp;

	default_clipRect = { 0,0,screen_width - 1,screen_height - 1 };
	win_client_rect = client_rect = { 0,0,screen_width ,screen_height };;

	if (FAILED(DirectDrawCreateEx(NULL, (void **)&lpdd, IID_IDirectDraw7, NULL)))
	{
		return 0;
	}

	if (screen_windowed)
	{
		lpdd->SetCooperativeLevel(main_window_handle, DDSCL_NORMAL);
	}
	else
	{
		lpdd->SetCooperativeLevel(main_window_handle,
			DDSCL_FULLSCREEN |
			DDSCL_ALLOWMODEX |
			DDSCL_EXCLUSIVE |
			DDSCL_ALLOWREBOOT);

		lpdd->SetDisplayMode(screen_width, screen_height, screen_bpp, 0, 0);// ȫ��ģʽ������DisplayMode
	}


	DDRAW_INIT_STRUCT(ddsd);

	if (!screen_windowed)
	{
		ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;

		ddsd.dwBackBufferCount = 1;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE
			| DDSCAPS_COMPLEX | DDSCAPS_FLIP;	// �󱸱��棬���ø��ӱ����ҳ���л�����

	}
	else	// ����ģʽ�޷�����ҳ�潻�������Բ��ô������ӱ���
	{
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	}


	lpdd->CreateSurface(&ddsd, &lpddsprimary, NULL);


	// �����������
	DDRAW_INIT_STRUCT(ddpixelformat);

	lpddsprimary->GetPixelFormat(&ddpixelformat);

	screen_bpp = ddpixelformat.dwRGBBitCount;
	dd_pixel_format = ddpixelformat.dwRGBBitCount;


	if (!screen_windowed)	// ȫ��ģʽ�����󱸻���
	{
		// Ϊ�����渽�Ӻ󱸱���
		ddsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
		if (FAILED(lpddsprimary->GetAttachedSurface(&ddsd.ddsCaps, &lpddsback)))
		{
			// �������ʧ��....
			return 0;
		}
	}
	else		// ����ģʽ��Ҳ����һ�����û�������ֻ�ǲ���ʹ���������Flip
	{
		lpddsback = DDraw_Create_Surface(screen_width, screen_height);

	}

	// �����8λ������һ�������ɫ��

	if (screen_bpp == DD_PIXEL_FORMAT8)
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


	DDraw_Init_FunctionPtrs();


	if (screen_windowed)
	{
		DDraw_Fill_Surface(lpddsback, 0);
	}
	else
	{
		DDraw_Fill_Surface(lpddsprimary, 0);
		DDraw_Fill_Surface(lpddsback, 0);
	}


	DDraw_Clipper_Init();

	return 1;

}

int DDraw_ShutDown()
{
	if (lpddpal)
	{
		lpddpal->Release();
		lpddpal = NULL;
	}
	if (lpddclipperwin != NULL)
	{
		lpddclipperwin->Release();
		lpddclipperwin = NULL;
	}
	if (lpddclipper != NULL)
	{
		lpddclipper->Release();
		lpddclipper = NULL;
	}
	if (lpddsback)
	{
		lpddsback->Release();
		lpddsback = NULL;
	}
	if (lpddsprimary)
	{
		lpddsprimary->Release();
		lpddsprimary = NULL;
	}
	if (lpdd)
	{
		lpdd->Release();
		lpdd = NULL;
	}
	return 1;
}

int DDraw_Flip(void)
{
	if (primary_buffer || back_buffer)
		return 0;
	if (!screen_windowed)
	{
		while (FAILED(lpddsprimary->Flip(NULL, DDFLIP_WAIT)));
	}
	else
	{
		if (FAILED(lpddsprimary->Blt(&win_client_rect, lpddsback, NULL, DDBLT_WAIT, NULL)))
			return 0;
	}
	return 1;
}

int DDraw_Wait_For_Vsync(void)
{
	lpdd->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, NULL);
	return 1;
}

int DDraw_CheckWinClient()
{
	if (!screen_windowed)
		return 0;
	GetWindowRect(main_window_handle, &win_client_rect);
	return 1;
}

// ����ģʽ����lpddsprimary�ü���
int DDraw_Clipper_Init()
{
	lpddclipper = DDraw_Attach_Clipper(lpddsback, 1, &client_rect);
	if (screen_windowed)
	{
		if (FAILED(lpdd->CreateClipper(0, &lpddclipperwin, NULL)))
			return 0;
		if (FAILED(lpddclipperwin->SetHWnd(0, main_window_handle)))
			return 0;
		if (FAILED(lpddsprimary->SetClipper(lpddclipperwin)))
			return 0;

	}
	return 1;
}

int DDraw_Fill_Surface(LPDIRECTDRAWSURFACE7 lpdds, int color)
{
	DDRAW_INIT_STRUCT(ddbltfx);
	ddbltfx.dwFillColor = color;
	lpdds->Blt(NULL,
		NULL,
		NULL,
		DDBLT_COLORFILL | DDBLT_WAIT,
		&ddbltfx);
	return 1;
}

int DDraw_Draw_Surface(LPDIRECTDRAWSURFACE7 source, int x, int y, int width, int height, LPDIRECTDRAWSURFACE7 dest, int transparent, float scale)
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


UCHAR *DDraw_Lock_Surface(LPDIRECTDRAWSURFACE7 lpdds, int *lpitch)
{
	if (!lpdds)
		return NULL;
	DDRAW_INIT_STRUCT(ddsd);
	lpdds->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);
	if (lpitch)
	{
		*lpitch = ddsd.lPitch;
	}
	return (UCHAR*)ddsd.lpSurface;
}
int DDraw_Unlock_Surface(LPDIRECTDRAWSURFACE7 lpdds)
{
	if (!lpdds)
		return 0;
	lpdds->Unlock(NULL);
	return 1;
}

UCHAR *DDraw_Lock_BackSurface(void)
{
	if (back_buffer)
	{
		return back_buffer;
	}
	DDRAW_INIT_STRUCT(ddsd);
	lpddsback->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);
	back_buffer = (UCHAR*)ddsd.lpSurface;
	back_lpitch = ddsd.lPitch;
	return back_buffer;

}

UCHAR *DDRaw_Lock_PrimarySurface(void)
{
	if (primary_buffer)
		return primary_buffer;
	DDRAW_INIT_STRUCT(ddsd);
	lpddsprimary->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);
	primary_buffer = (UCHAR *)ddsd.lpSurface;
	primary_lpitch = ddsd.lPitch;
	return primary_buffer;
}
int DDraw_Unlock_BackSurface(void)
{
	if (!back_buffer)
		return 0;
	lpddsback->Unlock(NULL);
	back_buffer = NULL;
	back_lpitch = 0;
	return 1;
}
int DDraw_Unlock_PrimarySurface(void)
{
	if (!primary_buffer)
		return 0;
	lpddsprimary->Unlock(NULL);
	primary_buffer = NULL;
	primary_lpitch = 0;
	return 1;
}

// �������ظ�ʽ��ʼ������ָ��
int DDraw_Init_FunctionPtrs(void)
{
	if (dd_pixel_format == DD_PIXEL_FORMAT8)
	{
		Draw_Triangle_2D = Draw_Triangle_2D8;
		Draw_TriangleFP_2D = Draw_TriangleFP_2D8;
		Draw_Filled_Polygon2D = Draw_Filled_Polygon2D8;

		Draw_Line = Draw_Line8;
		Draw_Clip_Line = Draw_Clip_Line8;

		Draw_Polygon2D = Draw_Polygon2D8;
		Draw_Pixel = Draw_Pixel8;

		HLine = HLine8;
		VLine = VLine8;

		RGBColor = RGBColor8Bit;
	}
	else
		if (dd_pixel_format == DD_PIXEL_FORMAT555)
		{
			RGB16Bit = RGB16Bit555;
			RGBColor = RGBColor16Bit555;
			Draw_Triangle_2D = Draw_Triangle_2D16;
			Draw_TriangleFP_2D = Draw_TriangleFP_2D16;
			Draw_Filled_Polygon2D = Draw_Filled_Polygon2D16;
			Draw_Polygon2D = Draw_Polygon2D16;

			Draw_Line = Draw_Line16;
			Draw_Clip_Line = Draw_Clip_Line16;
			Draw_Pixel = Draw_Pixel16;

			HLine = HLine16;
			VLine = VLine16;
		}
		else if (dd_pixel_format == DD_PIXEL_FORMAT565)
		{
			RGB16Bit = RGB16Bit565;
			RGBColor = RGBColor16Bit565;

			Draw_Triangle_2D = Draw_Triangle_2D16;
			Draw_TriangleFP_2D = Draw_TriangleFP_2D16;
			Draw_Filled_Polygon2D = Draw_Filled_Polygon2D16;
			Draw_Polygon2D = Draw_Polygon2D16;

			Draw_Line = Draw_Line16;
			Draw_Clip_Line = Draw_Clip_Line16;
			Draw_Pixel = Draw_Pixel16;

			HLine = HLine16;
			VLine = VLine16;

		}
		else if (dd_pixel_format == DD_PIXEL_FORMAT888)
		{
			Draw_Triangle_2D = Draw_Triangle_2D32;
			Draw_TriangleFP_2D = Draw_TriangleFP_2D32;
			Draw_Filled_Polygon2D = Draw_Filled_Polygon2D32;
			Draw_Polygon2D = Draw_Polygon2D32;

			RGBColor = RGBAColor32Bit;

			Draw_Line = Draw_Line32;
			Draw_Clip_Line = Draw_Clip_Line32;
			Draw_Pixel = Draw_Pixel32;

			HLine = HLine32;
			VLine = VLine32;
		}
		else if (dd_pixel_format == DD_PIXEL_FORMATALPHA888)
		{
			Draw_Triangle_2D = Draw_Triangle_2D32;
			Draw_TriangleFP_2D = Draw_TriangleFP_2D32;
			Draw_Filled_Polygon2D = Draw_Filled_Polygon2D32;
			Draw_Polygon2D = Draw_Polygon2D32;

			RGBColor = RGBAColor32Bit;

			Draw_Line = Draw_Line32;
			Draw_Clip_Line = Draw_Clip_Line32;
			Draw_Pixel = Draw_Pixel32;

			HLine = HLine32;
			VLine = VLine32;
		}
	return 1;
}

// ������������
LPDIRECTDRAWSURFACE7 DDraw_Create_Surface(int width, int height, int mem_flags, int color_key)
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
			color_key.dwColorSpaceLowValue = 0;				// ��Low��High����͸����
			color_key.dwColorSpaceHighValue = 0;
		}
		else if (ddpixelformat.dwRGBBitCount == 16)
		{
			color_key.dwColorSpaceLowValue = __RGB16BIT565(0, 0, 0);				// ��Low��High����͸����
			color_key.dwColorSpaceHighValue = __RGB16BIT565(0, 0, 0);
		}
		else // 24 or 32 (24λϵͳ��֧�֣���ת��Ϊ32λ)
		{
			color_key.dwColorSpaceLowValue = __RGB32BIT(1, 0, 0, 0);				// ��Low��High����͸����
			color_key.dwColorSpaceHighValue = __RGB32BIT(1, 0, 0, 0);
		}

		lpdds->SetColorKey(DDCKEY_SRCBLT, &color_key);
	} // end if

	return(lpdds);
} // end DDraw_Create_Surface

// ����һ���ü���
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
#pragma endregion

#pragma region Log ���
// error functions
int Open_Error_File(const char *filename, FILE *fp_override)
{
	if (fp_override)
	{
		fp_error = fp_override;
	}
	else
	{
		if ((fp_error = fopen(filename, "w")) == NULL)
		{
			return 0;
		}
	}
	struct _timeb timebuffer;
	char *timeline;
	char timestring[280];

	_ftime(&timebuffer);
	timeline = ctime(&(timebuffer.time));
	sprintf(timestring, "%.19s.%hu, %s", timeline, timebuffer.millitm, &timeline[20]);

	Write_Error("\n== Opening Error Output File (%s) on %s\n", filename, timestring);

	if (!fp_override)
	{
		fclose(fp_error);
		if ((fp_error = fopen(filename, "a+")) == NULL)
		{
			return 0;
		}
	}
	return 1;

}
int Close_Error_File(void)
{
	if (fp_error)
	{

		Write_Error("\n== Closing Error Output File.");

		if (fp_error != stdout || fp_error != stderr)
		{
			fclose(fp_error);
		}
		fp_error = NULL;
		return 1;
	}
	else {
		return 0;
	}
}
int Write_Error(const char *string, ...)
{
	char buffer[256];
	va_list arglist;

	if (!string || !fp_error)
	{
		return 0;
	}

	va_start(arglist, string);
	vsprintf(buffer, string, arglist);
	va_end(arglist);

	fprintf(fp_error, buffer);
	fflush(fp_error);
	return 1;

}

#pragma endregion
