#pragma once
#ifndef CYC_MATH2D_LIB
#define CYC_MATH2D_LIB

#include "cycTypeLib.h"

#define MIN(a,b) ((a<b)?a:b)
#define MAX(a,b) ((a>b)?a:b)
#define SWAP(a,b,t) {t=a;a=b;b=t;}

//const double PI = 3.141592653;
const double PI = 3.1415926535;
const double DEG2RAD = 0.0175;			// 1�ȵ��ڶ��ٻ���
const double RAD2DEG = 57.2958;			// 1���ȵ��ڶ��ٽǶ�

static float cos_look[360];
static float sin_look[360];


#pragma region 2d type
// 2d������(����)
typedef struct VERTEX2DI_TYP
{
	int x, y;		// ����

}VERTEX2DI, *VERTEX2DI_PTR;

// 2d������(����)
typedef struct VERTEX2DF_TYP
{
	float x, y;		// ����

}VERTEX2DF, *VERTEX2DF_PTR;

// 2d����ζ���
typedef struct POLYGON2D_TYP
{
	int state;						// ����ε�״̬
	int num_verts;					// �������
	int x0, y0;						// ����ε��е�λ��
	int xv, yv;						// ��ʼ�ٶ�
	UDWORD color;					// �������ɫ. ������index or palettentry
	VERTEX2DF *vlist;				// �����б�

} POLYGON2D, *POLYGON2D_PTR;

// �߽�����
typedef struct BOUND2DF_TYP
{
	float min_x, min_y;
	float max_x, max_y;
}BOUND2DF, *BOUND2DF_PTR;

#pragma endregion 2d type

#pragma region matrix type
// 3x3����
typedef struct MATRIX3X3_TYP
{
	float M[3][3];

}MATRIX3X3, *MATRIX3X3_PTR;


// 1x3����
typedef struct MATRIX1X3_TYP
{
	float M[3];

}MATRIX1X3, *MATRIX1X3_PTR;

// 1x2����
typedef struct MATRIX1X2_TYP
{
	float M[2];

}MATRIX1X2, *MATRIX1X2_PTR;

// 3x2����
typedef struct MATRIX3X2_TYP
{
	float M[3][2];

}MATRIX3X2, *MATRIX3X2_PTR;

#pragma endregion matrix type

////////////////////////////////////// ��������///////////////////////////

// ��ʼ��cos sin ��
int Init_LookTable();

#pragma region poly  functions
// ƽ�ƶ����
int Translate_Polygon2d(POLYGON2D_PTR polygon, int dx, int dy);

// ��ת����Σ�theta�ǽǶ�,Ϊ��ֵ��ʱ��˳ʱ��ת��
int Rotate_Polygon2d(POLYGON2D_PTR polygon, int theta);
// ��ת����Σ�theta�ǽǶ�,Ϊ��ֵ��ʱ��˳ʱ��ת��,���
int Rotate_Polygon2d_Fast(POLYGON2D_PTR polygon, int theta);

// ���Ŷ����
int Scale_Polygon2d(POLYGON2D_PTR poly, float s_x, float s_y);

int Translate_Polygon2d_Mat(POLYGON2D_PTR poly, int dx, int dy);

// ��ת����Σ�theta�ǽǶ�,Ϊ��ֵ��ʱ��˳ʱ��ת�� 
int Rotate_Polygon2d_Mat(POLYGON2D_PTR poly, int theta);

int Scale_Polygon2d_Mat(POLYGON2D_PTR poly, float s_x, float s_y);

#pragma endregion poly functions


#pragma region matrix functions
// ------��������
inline int Mat_Init_1X2(MATRIX1X2_PTR mat, float x, float y);

inline int Mat_Init_3X2(MATRIX3X2_PTR mat,
	float m00, float m01,
	float m10, float m11,
	float m20, float m21);
// �������
int Mat_Mul_3X3(MATRIX3X3_PTR ma, MATRIX3X3_PTR mb, MATRIX3X3_PTR mprod);
int Mat_Mul_1X3_3X3(MATRIX1X3_PTR ma, MATRIX3X3_PTR mb, MATRIX1X3_PTR mprod);
int Mat_Mul_1X2_3X2(MATRIX1X2_PTR ma, MATRIX3X2_PTR mb, MATRIX1X2_PTR mprod);

// ------��������   end
#pragma endregion matrix functions

#endif
