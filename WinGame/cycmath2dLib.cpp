#pragma once
#include <math.h>
#include "cycmath2dLib.h"

int Init_LookTable()
{
	for (int ang = 0; ang < 360; ++ang)
	{
		float theta = (float)ang*DEG2RAD;
		cos_look[ang] = cos(theta);
		sin_look[ang] = sin(theta);
	}
	return 1;
}

#pragma region Polygon2d相关函数

int Translate_Polygon2d(POLYGON2D_PTR polygon, int dx, int dy)
{
	if (!polygon)
		return 0;
	polygon->x0 += dx;
	polygon->y0 += dy;
	return 1;
}

// theta是角度
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
	if(!poly)
		return 0;
	MATRIX1X2 posMat = {poly->x0,poly->y0};

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
	if(!poly)
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
	if(!poly)
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


#pragma region Matrix 运算

int Mat_Init_1X2(MATRIX1X2_PTR mat,float x,float y)
{
	mat->M[0] = x;
	mat->M[1] = y;
	return 1;
}
int Mat_Init_3X2(MATRIX3X2_PTR mat,
	float m00,float m01,
	float m10,float m11,
	float m20,float m21)
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
			sum += (ma->M[index]*mb->M[index][col]);
		}
		sum += mb->M[index][col];		// 注意，这里要加上[3][col]的数！（平移！）
		mprod->M[col] = sum;
	}
	return 0;
}

#pragma endregion Matrix运算