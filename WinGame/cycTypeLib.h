#pragma once
#ifndef CYC_TYPE_LIB
#define CYC_TYPE_LIB
#include "cyclib1.h"

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

#endif
