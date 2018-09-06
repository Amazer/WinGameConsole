#pragma once
#ifndef CYC_TYPE_LIB
#define CYC_TYPE_LIB
#include "cyclib1.h"

// 2d������(����)
typedef struct VERTEX2DI_TYP
{
	int x, y;		// ����

}VERTEX2DI,*VERTEX2DI_PTR;

// 2d������(����)
typedef struct VERTEX2DF_TYP
{
	float x, y;		// ����

}VERTEX2DF,*VERTEX2DF_PTR;
// 2d����ζ���
typedef struct POLYGON2D_TYP
{
	int state;						// ����ε�״̬
	int num_verts;					// �������
	int x0, y0;						// ����ε��е�λ��
	int xv, yv;						// ��ʼ�ٶ�
	UDWORD color;					// �������ɫ. ������index or palettentry
	VERTEX2DF *vlist;				// �����б�

} POLYGON2D,*POLYGON2D_PTR;

#endif
