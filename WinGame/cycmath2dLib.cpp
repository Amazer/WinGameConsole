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

int Translate_Polygon2d(POLYGON2D_PTR polygon, int dx, int dy)
{
	if (!polygon)
		return 0;
	polygon->x0 += dx;
	polygon->y0 += dy;
	return 1;
}

// theta «Ω«∂»
int Rotate_Polygon2d(POLYGON2D_PTR polygon, int theta)
{
	if (!polygon)
		return 0;
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
