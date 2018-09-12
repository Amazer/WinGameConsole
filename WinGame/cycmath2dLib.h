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


// 2d点类型(顶点)
typedef struct VERTEX2DI_TYP
{
	int x, y;		// 顶点

}VERTEX2DI,*VERTEX2DI_PTR;

// 2d点类型(顶点)
typedef struct VERTEX2DF_TYP
{
	float x, y;		// 顶点

}VERTEX2DF,*VERTEX2DF_PTR;

// 2d多边形定义
typedef struct POLYGON2D_TYP
{
	int state;						// 多边形的状态
	int num_verts;					// 顶点个数
	int x0, y0;						// 多边形的中点位置
	int xv, yv;						// 初始速度
	UDWORD color;					// 多边形颜色. 可以是index or palettentry
	VERTEX2DF *vlist;				// 顶点列表

} POLYGON2D,*POLYGON2D_PTR;

// 3x3矩阵
typedef struct MATRIX3X3_TYP
{
	float M[3][3];

}MATRIX3X3,*MATRIX3X3_PTR;


// 1x3矩阵
typedef struct MATRIX1X3_TYP
{
	float M[3];

}MATRIX1X3,*MATRIX1X3_PTR;

// 1x2矩阵
typedef struct MATRIX1X2_TYP
{
	float M[2];

}MATRIX1X2,*MATRIX1X2_PTR;

// 3x2矩阵
typedef struct MATRIX3X2_TYP
{
	float M[3][2];

}MATRIX3X2,*MATRIX3X2_PTR;

int Init_LookTable();

// 平移多边形
int Translate_Polygon2d(POLYGON2D_PTR polygon,int dx,int dy);

// 旋转多边形（theta是角度,为正值的时候顺时针转）
int Rotate_Polygon2d(POLYGON2D_PTR polygon,int theta);
// 旋转多边形（theta是角度,为正值的时候顺时针转）,查表
int Rotate_Polygon2d_Fast(POLYGON2D_PTR polygon,int theta);

// 缩放多边形
int Scale_Polygon2d(POLYGON2D_PTR poly, float s_x, float s_y);

int Translate_Polygon2d_Mat(POLYGON2D_PTR poly, int dx, int dy);

// 旋转多边形（theta是角度,为正值的时候顺时针转） 
int Rotate_Polygon2d_Mat(POLYGON2D_PTR poly, int theta);

int Scale_Polygon2d_Mat(POLYGON2D_PTR poly, float s_x,float s_y);


// ------矩阵运算
inline int Mat_Init_1X2(MATRIX1X2_PTR mat, float x, float y);

inline int Mat_Init_3X2(MATRIX3X2_PTR mat,
	float m00, float m01,
	float m10, float m11,
	float m20, float m21);
// 矩阵相乘
int Mat_Mul_3X3(MATRIX3X3_PTR ma, MATRIX3X3_PTR mb, MATRIX3X3_PTR mprod);
int Mat_Mul_1X3_3X3(MATRIX1X3_PTR ma, MATRIX3X3_PTR mb, MATRIX1X3_PTR mprod);
int Mat_Mul_1X2_3X2(MATRIX1X2_PTR ma, MATRIX3X2_PTR mb, MATRIX1X2_PTR mprod);

// ------矩阵运算   end


inline int SwapInt(int &a, int &b);
inline int SwapFloat(float &a, float &b);

#endif
