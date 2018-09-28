#pragma once
#ifndef CYCLIBDRAW
#define CYCLIBDRAW

#pragma region 宏定义
#pragma endregion 宏定义


#define MS_PER_FRAME 33			// 每帧毫秒数

// 调色板相关
#define MAX_COLORS_PALETTE 256	// 调色板项的最大数量

// bitmap相关
#define BITMAP_ID 0x4D42		// bitmap类型id
#define BITMAP_STATE_DEAD 0
#define BITMAP_STATE_ALIVE 1
#define BITMAP_STATE_DYING 2
#define BITMAP_ATTR_LOADED 128

#define BITMAP_EXTRACT_MODE_CELL 0		// 单元格模式(从一整张bitmap中，分隔出image. 认为bitmap中的image单元格之间有1像素的边界)
#define BITMAP_EXTRACT_MODE_ABS 1		// 绝对模式(从一张bitmap中，加载出指定位置的image)

// 像素格式相关
#define DD_PIXEL_FORMAT8        8
#define DD_PIXEL_FORMAT555      15
#define DD_PIXEL_FORMAT565      16
#define DD_PIXEL_FORMAT888      24
#define DD_PIXEL_FORMATALPHA888 32 

// 线段裁剪相关
#define CLIP_CODE_C 0x0000
#define CLIP_CODE_N 0x0008
#define CLIP_CODE_S 0x0004
#define CLIP_CODE_E 0x0002
#define CLIP_CODE_W 0x0001

#define CLIP_CODE_NE 0x000a
#define CLIP_CODE_SE 0x0006
#define CLIP_CODE_NW 0x0009
#define CLIP_CODE_SW 0x0005


/////////////// 定点数相关///////////////
// 定点数16.16;将整体的32数转成16.16的定点数。
// 取部分值的时候，都是32位但是只低16位有效。
#define FIXP16_SHIFT 16					// 需要偏移的位数
#define FIXP16_MAG	65536				// 浮点数->整数要放大的倍数 =pow(2,16)
#define FIXP16_DP_MASK	0x0000ffff		// 定点数 小数部分的mask
#define FIXP16_WP_MASK	0xffff0000		// 定点数 整数部分的mask
#define FIXP16_ROUND_UP 0x00008000		// 定点数表示的0.5，用来避免丢失精度的

// int32 -> fix point 16.16;得到的还是32位. 放大65536倍,并且变为了无符号数？？
#define INT_2_FIXP(n) (n<<FIXP16_SHIFT)
// 定点数p转整数的方法是， (p+补码+1)>>16
#define FIXP_2_INT(p) ((p+FIXP16_ROUND_UP)>>FIXP16_SHIFT)
////////////////////////////////

//按键相关
#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code)&0x8000)?1:0)
#define KEYUP(vk_code) ((GetAsyncKeyState(vk_code)&0x8000)?0:1)


// 颜色格式相关
#define __RGB16BIT555(r,g,b) ((b&31)+((g&31)<<5)+((r&31)<<10))
#define __RGB16BIT565(r,g,b) ((b&31)+((g&63)<<5)+((r&31)<<11))
#define __RGB24BIT(r,g,b) (b+(g<<8)+(r<<16))
#define __RGB32BIT(a,r,g,b) (b+(g<<8)+(r<<16)+(a<<24))

//dx draw 相关
#define DDRAW_INIT_STRUCT(ddstruct) {memset(&ddstruct,0,sizeof(ddstruct)); ddstruct.dwSize=sizeof(ddstruct);}

// 数学相关
// PI 
#define PI         ((float)3.141592654f)
#define PI2        ((float)6.283185307f)
#define PI_DIV_2   ((float)1.570796327f)
#define PI_DIV_4   ((float)0.785398163f) 
#define PI_INV     ((float)0.318309886f) 
#define DEG2RAD		0.0175;			// 1度等于多少弧度
#define RAD2DEG		57.2958;			// 1弧度等于多少角度

#define MIN(a,b) ((a<b)?a:b)
#define MAX(a,b) ((a>b)?a:b)
#define SWAP(a,b,t) {t=a;a=b;b=t;}

#endif


#pragma region 基本类型
typedef unsigned char BYTE;			// 1个字节
typedef unsigned char UCHAR;		// 1个字节
typedef unsigned int  UINT;			// 4个字节
typedef unsigned short USHORT;		// 2个字节
typedef unsigned short UWORD;		// 2个字节
typedef unsigned long UDWORD;		// 4个字节
typedef int FIXPOINT;				// 定点数
#pragma endregion

#pragma region 2d type
// 2d点类型(顶点)
typedef struct VERTEX2DI_TYP
{
	int x, y;		// 顶点

}VERTEX2DI, *VERTEX2DI_PTR;

// 2d点类型(顶点)
typedef struct VERTEX2DF_TYP
{
	float x, y;		// 顶点

}VERTEX2DF, *VERTEX2DF_PTR;

// 2d多边形定义
typedef struct POLYGON2D_TYP
{
	int state;						// 多边形的状态
	int num_verts;					// 顶点个数
	int x0, y0;						// 多边形的中点位置
	int xv, yv;						// 初始速度
	UDWORD color;					// 多边形颜色. 可以是index or palettentry
	VERTEX2DF *vlist;				// 顶点列表

} POLYGON2D, *POLYGON2D_PTR;

// 边界类型
typedef struct BOUND2DF_TYP
{
	float min_x, min_y;
	float max_x, max_y;
}BOUND2DF, *BOUND2DF_PTR;

#pragma endregion 2d type

#pragma region matrix type
// 3x3矩阵
typedef struct MATRIX3X3_TYP
{
	float M[3][3];

}MATRIX3X3, *MATRIX3X3_PTR;


// 1x3矩阵
typedef struct MATRIX1X3_TYP
{
	float M[3];

}MATRIX1X3, *MATRIX1X3_PTR;

// 1x2矩阵
typedef struct MATRIX1X2_TYP
{
	float M[2];

}MATRIX1X2, *MATRIX1X2_PTR;

// 3x2矩阵
typedef struct MATRIX3X2_TYP
{
	float M[3][2];

}MATRIX3X2, *MATRIX3X2_PTR;

#pragma endregion matrix type


#pragma region 图像类型

// 自定义的位图文件类型。
typedef struct BITMAP_FILE_TAG
{
	BITMAPFILEHEADER bitmapfileHeader;		// 位图文件信息
	BITMAPINFOHEADER bitmapInfoHeader;		// 位图信息
	PALETTEENTRY palette[256];				// 调色板，如果有
	UCHAR *buffer;							// 位图像素数据
}BITMAP_FILE, *BITMAP_FILE_PTR;

// 平铺图像块
typedef struct TILE_TYP
{
	int x, y;			// 平铺矩阵中的位置
	int index;			// 在bitmap中的index
	int flags;			// 


}TILE,*TILE_PTR;

// 平铺一整屏幕的图块
typedef struct TILED_IMAGE_TYP
{
	int rows;
	int columns;
	TILE_PTR tiles;

}TILED_IMAGE,*TILED_IMAGE_PTR;

// 存储bitmap像素(图像)
typedef struct BITMAP_IMAGE_TYP
{
	int state;
	int attr;
	int x, y;				// 图像位置
	int width, height;		// 图像的尺寸
	int num_bytes;			// 总字节数
	int bpp;				// 每像素的位数
	UCHAR *buffer;			// bitmpa 的像素

}BITMAP_IMAGE,*BITMAP_IMAGE_PTR;

// blinking light structure
typedef struct BLINKER_TYP
{
	// user sets these
	int color_index;         // index of color to blink
	PALETTEENTRY on_color;   // RGB value of "on" color
	PALETTEENTRY off_color;  // RGB value of "off" color
	int on_time;             // number of frames to keep "on" 
	int off_time;            // number of frames to keep "off"

	// internal member
	int counter;             // counter for state transitions
	int state;               // state of light, -1 off, 1 on, 0 dead
} BLINKER, *BLINKER_PTR;

#pragma endregion  图像类型

#pragma region 函数定义

inline void Mem_Set_USHORT(void *dest, USHORT data, int count);
inline void Mem_Set_UINT(void *dest, UINT data, int count);

#pragma region 随机颜色
extern inline UINT RandomRGB16BIT565();
extern inline UINT RandomRGBA32();
#pragma endregion  随机颜色

#pragma region Draw Pixel 2D
extern void (*Draw_Pixel)(int x, int y, int color, UCHAR *buffer, int mempitch);
extern inline void Draw_Pixel8(int x, int y, int color, UCHAR *buffer, int mempitch);
extern inline void Draw_Pixel16(int x, int y, int color, UCHAR *buffer, int mempitch);
extern inline void Draw_Pixel32(int x, int y, int color, UCHAR *video_buffer, int lpitch);

#pragma endregion 画像素

#pragma region Draw Rectangle (使用了blt)
//  填充Surface矩形区域颜色
extern int Draw_Rectangle(int x1, int y1, int x2, int y2, int color,LPDIRECTDRAWSURFACE7 lpdds);
#pragma endregion

#pragma region 裁剪

// 裁剪，bitmap从（0，0）点开始计算位置
extern void Blit_Clipped(int posX, int posY, int sizeWidth, int sizeHeight, UCHAR *bitmap, UCHAR *video_buffer, int mempitch);
#pragma endregion 裁剪
#pragma region poly  functions
// 平移多边形
int Translate_Polygon2d(POLYGON2D_PTR polygon, int dx, int dy);
// 旋转多边形（theta是角度,为正值的时候顺时针转）
int Rotate_Polygon2d(POLYGON2D_PTR polygon, int theta);
// 旋转多边形（theta是角度,为正值的时候顺时针转）,查表
int Rotate_Polygon2d_Fast(POLYGON2D_PTR polygon, int theta);
// 缩放多边形
int Scale_Polygon2d(POLYGON2D_PTR poly, float s_x, float s_y);
// 使用矩阵平移多边形
int Translate_Polygon2d_Mat(POLYGON2D_PTR poly, int dx, int dy);
// 旋转多边形（theta是角度,为正值的时候顺时针转） 
int Rotate_Polygon2d_Mat(POLYGON2D_PTR poly, int theta);
// 使用矩阵缩放多边形
int Scale_Polygon2d_Mat(POLYGON2D_PTR poly, float s_x, float s_y);

#pragma endregion poly functions

#pragma region matrix functions
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
#pragma endregion matrix functions

#pragma region 数学函数
// 初始化cos sin 表
int Init_LookTable();

#pragma endregion 数学函数


#pragma endregion 函数定义

#pragma region DDRAW 相关函数
// 初始化ddraw
int DDraw_Init(int width, int height, int bpp, int windowed);
int DDraw_Wait_For_Vsync(void);
int DDraw_ShutDown();
int DDraw_CheckWinClient();
int DDraw_Flip(void);
int DDraw_Clipper_Init();
int DDraw_Fill_Surface(LPDIRECTDRAWSURFACE7 lpdds, int color);
int DDraw_Draw_Surface(LPDIRECTDRAWSURFACE7 source, int x, int y, int width, int height, LPDIRECTDRAWSURFACE7 dest, int transparent = 1, float scale = 1.0f);
UCHAR *DDraw_Lock_Surface(LPDIRECTDRAWSURFACE7 lpdds, int *lpitch);
int DDraw_Unlock_Surface(LPDIRECTDRAWSURFACE7 lpdds);

UCHAR *DDraw_Lock_BackSurface(void);
UCHAR *DDRaw_Lock_PrimarySurface(void);
int DDraw_Unlock_BackSurface(void);
int DDraw_Unlock_PrimarySurface(void);

// 根据像素格式初始化函数指针
int DDraw_Init_FunctionPtrs(void);

// 创建离屏表面
LPDIRECTDRAWSURFACE7 DDraw_Create_Surface(int width, int height, int mem_flags = DDSCAPS_VIDEOMEMORY, int color_key = 0);
// 创建一个裁剪器
LPDIRECTDRAWCLIPPER DDraw_Attach_Clipper(LPDIRECTDRAWSURFACE7 lpdds, int num_rects, LPRECT clip_list);

#pragma endregion DDRAW 相关函数  

#pragma region Bitmap 相关
// 上下翻转bitmap内存
int Flip_Bitmap(UCHAR* image, int bytes_per_line, int height);
// 加载bitmap文件
int Load_Bitmap_File(BITMAP_FILE_PTR bitmap, const char* filename);
// 释放内存
int Unload_Bitmap_Flie(BITMAP_FILE_PTR bitmap);

// 将bitmap从cy行，cx个sprite的内容渲染到lpdds
int Scan_Image_Bitmap8(BITMAP_FILE_PTR bitmap,     // bitmap file to scan image data from
	LPDIRECTDRAWSURFACE7 lpdds, // surface to hold data
	int cx, int cy);             // cell to scan image from
int Scan_Image_Bitmap16(BITMAP_FILE_PTR bitmap,     // bitmap file to scan image data from
	LPDIRECTDRAWSURFACE7 lpdds, // surface to hold data
	int cx, int cy);             // cell to scan image from

int Scan_Image_Bitmap24(BITMAP_FILE_PTR bitmap,     // bitmap file to scan image data from
	LPDIRECTDRAWSURFACE7 lpdds, // surface to hold data
	int cx, int cy);             // cell to scan image from

// 给定尺寸在指定位置创建8位16位24位系统内存位图(分配了buffer空间，buffer内容都为0)
int Create_Bitmap(BITMAP_IMAGE_PTR image, int x, int y, int width, int height, int bpp = 8);

int Destroy_Bitmap(BITMAP_IMAGE_PTR image);

int Load_Image_Bitmap8(BITMAP_IMAGE_PTR image, BITMAP_FILE_PTR bitmap, int cx, int cy, int mode);

int Load_Image_Bitmap16(BITMAP_IMAGE_PTR image, BITMAP_FILE_PTR bitmap, int cx, int cy, int mode);

int Load_Image_Bitmap24(BITMAP_IMAGE_PTR image, BITMAP_FILE_PTR bitmap, int cx, int cy, int mode);

// 画image到显存或内存。transparent表示是否拷贝透明颜色。1为不拷贝透明颜色
int Draw_Bitmap_Image8(BITMAP_IMAGE_PTR image, UCHAR * dest_buffer, int lpitch, int transparent);

// 画image到显存或内存。transparent表示是否拷贝透明颜色。1为不拷贝透明颜色
int Draw_Bitmap_Image16(BITMAP_IMAGE_PTR image, UCHAR * dest_buffer, int lpitch, int transparent);

// 画image到显存或内存。transparent表示是否拷贝透明颜色。1为不拷贝透明颜色
int Draw_Bitmap_Image24(BITMAP_IMAGE_PTR image, UCHAR * dest_buffer, int lpitch, int transparent);

// 卷动位图
int Scroll_Bitmap(BITMAP_IMAGE_PTR image, int dx, int dy);

// 拷贝图位图
int Copy_Bitmap(BITMAP_IMAGE_PTR dest_image, int dest_x, int dest_y,
	BITMAP_IMAGE_PTR src_image, int src_x, int src_y,
	int width, int height);

#pragma endregion

#pragma region Draw Text

// 在DirectDraw中画文字 color:RGB(r,g,b)
extern int Draw_Text_GDI_IN_DD(const char * txt, int x, int y, COLORREF color, LPDIRECTDRAWSURFACE7 lpdds);

#pragma endregion

#pragma region Draw Lines

//  画线的函数指针。（不裁剪）
extern int (*Draw_Line)(int x0, int y0, int x1, int y1, int color, UCHAR *vb_start, int lpitch);
// vb_start: video buffer start
extern int Draw_Line8(int x0, int y0, int x1, int y1, int color, UCHAR *vb_start, int lpitch);

// vb_start: video buffer start
extern int Draw_Line16(int x0, int y0, int x1, int y1, int color, UCHAR *vb_start, int lpitch);

extern int Draw_Line32(int x0, int y0, int x1, int y1, int color, UCHAR *vb_start, int lpitch);

// 裁剪线段。返回值为1的时候，没有完全被裁剪掉；返回值为0的时候，完全被裁减掉了
extern int Clip_Line(LPRECT clipRect, int &x0, int &y0, int &x1, int &y1);

extern void (*Draw_Clip_Line)(LPRECT clipRect, int x0, int y0, int x1, int y1, int color, UCHAR *vb_start, int lpitch);
extern void Draw_Clip_Line8(LPRECT clipRect, int x0, int y0, int x1, int y1, int color, UCHAR *vb_start, int lpitch);
extern void Draw_Clip_Line16(LPRECT clipRect, int x0, int y0, int x1, int y1, int color, UCHAR *vb_start, int lpitch);
extern void Draw_Clip_Line32(LPRECT clipRect, int x0, int y0, int x1, int y1, int color, UCHAR *vb_start, int lpitch);

extern void (*HLine)(LPRECT clipRect, int x1,int x2,int y,int color, UCHAR *vbuffer, int lpitch);
extern void (*VLine)(LPRECT clipRect,int y1,int y2,int x,int color, UCHAR *vbuffer, int lpitch);
extern void HLine8(LPRECT clipRect,int x1,int x2,int y,int color, UCHAR *vbuffer, int lpitch);
extern void VLine8(LPRECT clipRect,int y1,int y2,int x,int color, UCHAR *vbuffer, int lpitch);
extern void HLine16(LPRECT clipRect,int x1,int x2,int y,int color, UCHAR *vbuffer, int lpitch);
extern void VLine16(LPRECT clipRect,int y1,int y2,int x,int color, UCHAR *vbuffer, int lpitch);
extern void HLine32(LPRECT clipRect,int x1,int x2,int y,int color, UCHAR *vbuffer, int lpitch);
extern void VLine32(LPRECT clipRect,int y1,int y2,int x,int color, UCHAR *vbuffer, int lpitch);

#pragma endregion

#pragma region Draw Polygon2D

extern int (*Draw_Polygon2D)(LPRECT clipRect, POLYGON2D_PTR poly, UCHAR *vbuffer, int lpitch);
extern int Draw_Polygon2D8(LPRECT clipRect, POLYGON2D_PTR poly, UCHAR *vbuffer, int lpitch);
extern int Draw_Polygon2D16(LPRECT clipRect, POLYGON2D_PTR poly, UCHAR *vbuffer, int lpitch);
extern int Draw_Polygon2D32(LPRECT clipRect, POLYGON2D_PTR poly, UCHAR *vbuffer, int lpitch);

#pragma endregion


#pragma region 三角形光栅化和多边形填充

// 填充平底三角形 ,按照逆时针方向作为正方向发送顶点。函数内部调换为：p0是顶点，p1是left点，p2是right点
void Draw_Bottom_Tri(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR* dest_buffer, int mempitch);

void Draw_Bottom_Tri16(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR* dest_buffer, int mempitch);

void Draw_Bottom_Tri32(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR* dest_buffer, int mempitch);

// 填充平顶三角形，按照逆时针方向作为正方向发送顶点,函数内部调换为：p0是低点，p1是right点，p2是left点
void Draw_Top_Tri(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR* dest_buffer, int mempitch);
void Draw_Top_Tri16(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR* dest_buffer, int mempitch);
void Draw_Top_Tri32(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR* dest_buffer, int mempitch);

// 任意一个三角形。顶点顺序逆时针发送。内部调整为：p0为顶点，p1为左边点，p2为右边点
// 使用分割为两个三角形的方式进行填充
void Draw_Triangle_2D8(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2,
	int color, UCHAR *dest_buffer, int mempitch);

void Draw_Triangle_2D16(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2,
	int color, UCHAR *dest_buffer, int mempitch);

void Draw_Triangle_2D32(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2,
	int color, UCHAR *dest_buffer, int mempitch);


// 定点数的填充平顶三角形
void Draw_Top_TriFP8(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR *dest_buffer, int mempitch);
void Draw_Top_TriFP16(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR *dest_buffer, int mempitch);
void Draw_Top_TriFP32(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR *dest_buffer, int mempitch);

void Draw_Bottom_TriFP8(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR *dest_buffer, int mempitch);
void Draw_Bottom_TriFP16(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR *dest_buffer, int mempitch);
void Draw_Bottom_TriFP32(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR *dest_buffer, int mempitch);

void Draw_TriangleFP_2D8(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR *dest_buffer, int mempitch);
void Draw_TriangleFP_2D16(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR *dest_buffer, int mempitch);
void Draw_TriangleFP_2D32(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR *dest_buffer, int mempitch);

// 定点数，填充四边形
extern inline void Draw_QuadFP_2D(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, int color, UCHAR *dest_buffer, int mempitch);

// 四边形的顶点0,1,2,3顺时针传入，那么两个被分割的三角形为： <0,1,3> <1,2,3>
extern inline void Draw_Quad_2D(PRECT p_clipRect, int x0, int y0, int x1, int y1,
	int x2, int y2, int x3, int y3,
	int color, UCHAR *dest_buffer, int mempitch);

// 填充多边形
extern void (*Draw_Filled_Polygon2D)(PRECT clipRect, POLYGON2D_PTR poly, UCHAR *vbuffer, int mempitch);
extern void Draw_Filled_Polygon2D8(PRECT clipRect, POLYGON2D_PTR poly, UCHAR *vbuffer, int mempitch);
extern void Draw_Filled_Polygon2D16(PRECT clipRect, POLYGON2D_PTR poly, UCHAR *vbuffer, int mempitch);
extern void Draw_Filled_Polygon2D32(PRECT clipRect, POLYGON2D_PTR poly, UCHAR *vbuffer, int mempitch);

#pragma endregion

#pragma region 碰撞检测部分的函数

// 快速计算<x,y>到<0,0>的距离。使用了泰勒展开式。（没有明白）
extern int Fast_Distance_2D(int x, int y);

extern int Find_Bounding_Box_Poly2D(POLYGON2D_PTR poly, BOUND2DF_PTR bound);

#pragma endregion

#pragma region Log 相关
// error functions
extern int Open_Error_File(const char *filename, FILE *fp_override=NULL);
extern int Close_Error_File(void);
extern int Write_Error(const char *string, ...);
#pragma endregion


#pragma region 全局变量

// 错误文件指针
extern FILE *fp_error;
// 错误文件名称
extern char error_filename[80];

extern LPDIRECTDRAW7 lpdd;
extern LPDIRECTDRAWSURFACE7 lpddsprimary;		// 主显示表面(主表面)
extern LPDIRECTDRAWSURFACE7 lpddsback;			// 后备表面(主表面的子表面)
extern LPDIRECTDRAWCLIPPER lpddclipper;			// 裁剪器
extern LPDIRECTDRAWCLIPPER lpddclipperwin;		// 窗口裁剪器

extern LPDIRECTDRAWPALETTE lpddpal;
extern PALETTEENTRY palette[256];

extern DDSURFACEDESC2 ddsd;						// surface描述
extern DDSCAPS2 ddcaps;							// 表面的能力描述
extern DDBLTFX ddbltfx;							// 内存块blitter
extern HRESULT ddrval;							// dd调用的返回值

extern UCHAR *primary_buffer;
extern UCHAR *back_buffer;

extern int primary_lpitch;
extern int back_lpitch;

extern DDPIXELFORMAT ddpixelformat;					// 像素格式

extern RECT default_clipRect;						// 默认的裁剪框(和窗口一样大小)

extern int screen_width;
extern int screen_height;
extern int screen_bpp;
extern int screen_windowed;

extern int win_client_x0;
extern int win_client_y0;

extern int dd_pixel_format;

extern RECT client_rect;							// 用户区域
extern RECT win_client_rect;						// 窗口模式的用户区域

extern float cos_look[361];
extern float sin_look[361];

// rgb颜色
extern void* (*RGBColor)(int r, int g, int b, int a);
extern void* RGBColor8Bit(int r, int g, int b, int a);
extern void* RGBColor16Bit565(int r, int g, int b, int a);
extern void* RGBColor16Bit555(int r, int g, int b, int a);
extern void * RGBAColor32Bit(int r, int g, int b, int a);

extern USHORT(*RGB16Bit)(int r, int g, int b);
extern USHORT RGB16Bit565(int r, int g, int b);
extern USHORT RGB16Bit555(int r, int g, int b);

// draw triangle_2d 的函数指针
extern void(*Draw_Triangle_2D)(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2,
	int color, UCHAR *dest_buffer, int mempitch);

// draw triangle fixed point 2D 的函数指针
extern void(*Draw_TriangleFP_2D)(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2,
	int color, UCHAR *dest_buffer, int mempitch);

#pragma endregion 全局变量