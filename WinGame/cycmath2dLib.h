#pragma once
#ifndef CYC_MATH2D_LIB
#define CYC_MATH2D_LIB

#include "cycTypeLib.h"

//const double PI = 3.141592653;
const double PI = 3.1415926535;
const double DEG2RAD = 0.0175;			// 1度等于多少弧度
const double RAD2DEG = 57.2958;			// 1弧度等于多少角度


static float cos_look[360];
static float sin_look[360];

int Init_LookTable();

// 平移多边形
int Translate_Polygon2d(POLYGON2D_PTR polygon,int dx,int dy);

// 旋转多边形（theta是角度）
int Rotate_Polygon2d(POLYGON2D_PTR polygon,int theta);
// 旋转多边形（theta是角度）,查表
int Rotate_Polygon2d_Fast(POLYGON2D_PTR polygon,int theta);

// 缩放多边形
int Scale_Polygon2d(POLYGON2D_PTR poly, float s_x, float s_y);

#endif
