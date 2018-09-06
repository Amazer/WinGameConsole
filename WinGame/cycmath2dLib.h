#pragma once
#ifndef CYC_MATH2D_LIB
#define CYC_MATH2D_LIB

#include "cycTypeLib.h"

//const double PI = 3.141592653;
const double PI = 3.1415926535;
const double DEG2RAD = 0.0175;			// 1�ȵ��ڶ��ٻ���
const double RAD2DEG = 57.2958;			// 1���ȵ��ڶ��ٽǶ�


static float cos_look[360];
static float sin_look[360];

int Init_LookTable();

// ƽ�ƶ����
int Translate_Polygon2d(POLYGON2D_PTR polygon,int dx,int dy);

// ��ת����Σ�theta�ǽǶȣ�
int Rotate_Polygon2d(POLYGON2D_PTR polygon,int theta);
// ��ת����Σ�theta�ǽǶȣ�,���
int Rotate_Polygon2d_Fast(POLYGON2D_PTR polygon,int theta);

// ���Ŷ����
int Scale_Polygon2d(POLYGON2D_PTR poly, float s_x, float s_y);

#endif
