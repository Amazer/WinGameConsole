#pragma once
#ifndef CYCLIBDRAW
#define CYCLIBDRAW

#pragma region �궨��
#pragma endregion �궨��


#define MS_PER_FRAME 33			// ÿ֡������

// ��ɫ�����
#define MAX_COLORS_PALETTE 256	// ��ɫ������������

// bitmap���
#define BITMAP_ID 0x4D42		// bitmap����id
#define BITMAP_STATE_DEAD 0
#define BITMAP_STATE_ALIVE 1
#define BITMAP_STATE_DYING 2
#define BITMAP_ATTR_LOADED 128

#define BITMAP_EXTRACT_MODE_CELL 0		// ��Ԫ��ģʽ(��һ����bitmap�У��ָ���image. ��Ϊbitmap�е�image��Ԫ��֮����1���صı߽�)
#define BITMAP_EXTRACT_MODE_ABS 1		// ����ģʽ(��һ��bitmap�У����س�ָ��λ�õ�image)

// ���ظ�ʽ���
#define DD_PIXEL_FORMAT8        8
#define DD_PIXEL_FORMAT555      15
#define DD_PIXEL_FORMAT565      16
#define DD_PIXEL_FORMAT888      24
#define DD_PIXEL_FORMATALPHA888 32 

// �߶βü����
#define CLIP_CODE_C 0x0000
#define CLIP_CODE_N 0x0008
#define CLIP_CODE_S 0x0004
#define CLIP_CODE_E 0x0002
#define CLIP_CODE_W 0x0001

#define CLIP_CODE_NE 0x000a
#define CLIP_CODE_SE 0x0006
#define CLIP_CODE_NW 0x0009
#define CLIP_CODE_SW 0x0005


/////////////// ���������///////////////
// ������16.16;�������32��ת��16.16�Ķ�������
// ȡ����ֵ��ʱ�򣬶���32λ����ֻ��16λ��Ч��
#define FIXP16_SHIFT 16					// ��Ҫƫ�Ƶ�λ��
#define FIXP16_MAG	65536				// ������->����Ҫ�Ŵ�ı��� =pow(2,16)
#define FIXP16_DP_MASK	0x0000ffff		// ������ С�����ֵ�mask
#define FIXP16_WP_MASK	0xffff0000		// ������ �������ֵ�mask
#define FIXP16_ROUND_UP 0x00008000		// ��������ʾ��0.5���������ⶪʧ���ȵ�

// int32 -> fix point 16.16;�õ��Ļ���32λ. �Ŵ�65536��,���ұ�Ϊ���޷���������
#define INT_2_FIXP(n) (n<<FIXP16_SHIFT)
// ������pת�����ķ����ǣ� (p+����+1)>>16
#define FIXP_2_INT(p) ((p+FIXP16_ROUND_UP)>>FIXP16_SHIFT)
////////////////////////////////

//�������
#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code)&0x8000)?1:0)
#define KEYUP(vk_code) ((GetAsyncKeyState(vk_code)&0x8000)?0:1)


// ��ɫ��ʽ���
#define __RGB16BIT555(r,g,b) ((b&31)+((g&31)<<5)+((r&31)<<10))
#define __RGB16BIT565(r,g,b) ((b&31)+((g&63)<<5)+((r&31)<<11))
#define __RGB24BIT(r,g,b) (b+(g<<8)+(r<<16))
#define __RGB32BIT(a,r,g,b) (b+(g<<8)+(r<<16)+(a<<24))

//dx draw ���
#define DDRAW_INIT_STRUCT(ddstruct) {memset(&ddstruct,0,sizeof(ddstruct)); ddstruct.dwSize=sizeof(ddstruct);}

// ��ѧ���
// PI 
#define PI         ((float)3.141592654f)
#define PI2        ((float)6.283185307f)
#define PI_DIV_2   ((float)1.570796327f)
#define PI_DIV_4   ((float)0.785398163f) 
#define PI_INV     ((float)0.318309886f) 
#define DEG2RAD		0.0175;			// 1�ȵ��ڶ��ٻ���
#define RAD2DEG		57.2958;			// 1���ȵ��ڶ��ٽǶ�

#define MIN(a,b) ((a<b)?a:b)
#define MAX(a,b) ((a>b)?a:b)
#define SWAP(a,b,t) {t=a;a=b;b=t;}

#endif


#pragma region ��������
typedef unsigned char BYTE;			// 1���ֽ�
typedef unsigned char UCHAR;		// 1���ֽ�
typedef unsigned int  UINT;			// 4���ֽ�
typedef unsigned short USHORT;		// 2���ֽ�
typedef unsigned short UWORD;		// 2���ֽ�
typedef unsigned long UDWORD;		// 4���ֽ�
typedef int FIXPOINT;				// ������
#pragma endregion

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


#pragma region ͼ������

// �Զ����λͼ�ļ����͡�
typedef struct BITMAP_FILE_TAG
{
	BITMAPFILEHEADER bitmapfileHeader;		// λͼ�ļ���Ϣ
	BITMAPINFOHEADER bitmapInfoHeader;		// λͼ��Ϣ
	PALETTEENTRY palette[256];				// ��ɫ�壬�����
	UCHAR *buffer;							// λͼ��������
}BITMAP_FILE, *BITMAP_FILE_PTR;

// ƽ��ͼ���
typedef struct TILE_TYP
{
	int x, y;			// ƽ�̾����е�λ��
	int index;			// ��bitmap�е�index
	int flags;			// 


}TILE,*TILE_PTR;

// ƽ��һ����Ļ��ͼ��
typedef struct TILED_IMAGE_TYP
{
	int rows;
	int columns;
	TILE_PTR tiles;

}TILED_IMAGE,*TILED_IMAGE_PTR;

// �洢bitmap����(ͼ��)
typedef struct BITMAP_IMAGE_TYP
{
	int state;
	int attr;
	int x, y;				// ͼ��λ��
	int width, height;		// ͼ��ĳߴ�
	int num_bytes;			// ���ֽ���
	int bpp;				// ÿ���ص�λ��
	UCHAR *buffer;			// bitmpa ������

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

#pragma endregion  ͼ������

#pragma region ��������

inline void Mem_Set_USHORT(void *dest, USHORT data, int count);
inline void Mem_Set_UINT(void *dest, UINT data, int count);

#pragma region �����ɫ
extern inline UINT RandomRGB16BIT565();
extern inline UINT RandomRGBA32();
#pragma endregion  �����ɫ

#pragma region Draw Pixel 2D
extern void (*Draw_Pixel)(int x, int y, int color, UCHAR *buffer, int mempitch);
extern inline void Draw_Pixel8(int x, int y, int color, UCHAR *buffer, int mempitch);
extern inline void Draw_Pixel16(int x, int y, int color, UCHAR *buffer, int mempitch);
extern inline void Draw_Pixel32(int x, int y, int color, UCHAR *video_buffer, int lpitch);

#pragma endregion ������

#pragma region Draw Rectangle (ʹ����blt)
//  ���Surface����������ɫ
extern int Draw_Rectangle(int x1, int y1, int x2, int y2, int color,LPDIRECTDRAWSURFACE7 lpdds);
#pragma endregion

#pragma region �ü�

// �ü���bitmap�ӣ�0��0���㿪ʼ����λ��
extern void Blit_Clipped(int posX, int posY, int sizeWidth, int sizeHeight, UCHAR *bitmap, UCHAR *video_buffer, int mempitch);
#pragma endregion �ü�
#pragma region poly  functions
// ƽ�ƶ����
int Translate_Polygon2d(POLYGON2D_PTR polygon, int dx, int dy);
// ��ת����Σ�theta�ǽǶ�,Ϊ��ֵ��ʱ��˳ʱ��ת��
int Rotate_Polygon2d(POLYGON2D_PTR polygon, int theta);
// ��ת����Σ�theta�ǽǶ�,Ϊ��ֵ��ʱ��˳ʱ��ת��,���
int Rotate_Polygon2d_Fast(POLYGON2D_PTR polygon, int theta);
// ���Ŷ����
int Scale_Polygon2d(POLYGON2D_PTR poly, float s_x, float s_y);
// ʹ�þ���ƽ�ƶ����
int Translate_Polygon2d_Mat(POLYGON2D_PTR poly, int dx, int dy);
// ��ת����Σ�theta�ǽǶ�,Ϊ��ֵ��ʱ��˳ʱ��ת�� 
int Rotate_Polygon2d_Mat(POLYGON2D_PTR poly, int theta);
// ʹ�þ������Ŷ����
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

#pragma region ��ѧ����
// ��ʼ��cos sin ��
int Init_LookTable();

#pragma endregion ��ѧ����


#pragma endregion ��������

#pragma region DDRAW ��غ���
// ��ʼ��ddraw
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

// �������ظ�ʽ��ʼ������ָ��
int DDraw_Init_FunctionPtrs(void);

// ������������
LPDIRECTDRAWSURFACE7 DDraw_Create_Surface(int width, int height, int mem_flags = DDSCAPS_VIDEOMEMORY, int color_key = 0);
// ����һ���ü���
LPDIRECTDRAWCLIPPER DDraw_Attach_Clipper(LPDIRECTDRAWSURFACE7 lpdds, int num_rects, LPRECT clip_list);

#pragma endregion DDRAW ��غ���  

#pragma region Bitmap ���
// ���·�תbitmap�ڴ�
int Flip_Bitmap(UCHAR* image, int bytes_per_line, int height);
// ����bitmap�ļ�
int Load_Bitmap_File(BITMAP_FILE_PTR bitmap, const char* filename);
// �ͷ��ڴ�
int Unload_Bitmap_Flie(BITMAP_FILE_PTR bitmap);

// ��bitmap��cy�У�cx��sprite��������Ⱦ��lpdds
int Scan_Image_Bitmap8(BITMAP_FILE_PTR bitmap,     // bitmap file to scan image data from
	LPDIRECTDRAWSURFACE7 lpdds, // surface to hold data
	int cx, int cy);             // cell to scan image from
int Scan_Image_Bitmap16(BITMAP_FILE_PTR bitmap,     // bitmap file to scan image data from
	LPDIRECTDRAWSURFACE7 lpdds, // surface to hold data
	int cx, int cy);             // cell to scan image from

int Scan_Image_Bitmap24(BITMAP_FILE_PTR bitmap,     // bitmap file to scan image data from
	LPDIRECTDRAWSURFACE7 lpdds, // surface to hold data
	int cx, int cy);             // cell to scan image from

// �����ߴ���ָ��λ�ô���8λ16λ24λϵͳ�ڴ�λͼ(������buffer�ռ䣬buffer���ݶ�Ϊ0)
int Create_Bitmap(BITMAP_IMAGE_PTR image, int x, int y, int width, int height, int bpp = 8);

int Destroy_Bitmap(BITMAP_IMAGE_PTR image);

int Load_Image_Bitmap8(BITMAP_IMAGE_PTR image, BITMAP_FILE_PTR bitmap, int cx, int cy, int mode);

int Load_Image_Bitmap16(BITMAP_IMAGE_PTR image, BITMAP_FILE_PTR bitmap, int cx, int cy, int mode);

int Load_Image_Bitmap24(BITMAP_IMAGE_PTR image, BITMAP_FILE_PTR bitmap, int cx, int cy, int mode);

// ��image���Դ���ڴ档transparent��ʾ�Ƿ񿽱�͸����ɫ��1Ϊ������͸����ɫ
int Draw_Bitmap_Image8(BITMAP_IMAGE_PTR image, UCHAR * dest_buffer, int lpitch, int transparent);

// ��image���Դ���ڴ档transparent��ʾ�Ƿ񿽱�͸����ɫ��1Ϊ������͸����ɫ
int Draw_Bitmap_Image16(BITMAP_IMAGE_PTR image, UCHAR * dest_buffer, int lpitch, int transparent);

// ��image���Դ���ڴ档transparent��ʾ�Ƿ񿽱�͸����ɫ��1Ϊ������͸����ɫ
int Draw_Bitmap_Image24(BITMAP_IMAGE_PTR image, UCHAR * dest_buffer, int lpitch, int transparent);

// ��λͼ
int Scroll_Bitmap(BITMAP_IMAGE_PTR image, int dx, int dy);

// ����ͼλͼ
int Copy_Bitmap(BITMAP_IMAGE_PTR dest_image, int dest_x, int dest_y,
	BITMAP_IMAGE_PTR src_image, int src_x, int src_y,
	int width, int height);

#pragma endregion

#pragma region Draw Text

// ��DirectDraw�л����� color:RGB(r,g,b)
extern int Draw_Text_GDI_IN_DD(const char * txt, int x, int y, COLORREF color, LPDIRECTDRAWSURFACE7 lpdds);

#pragma endregion

#pragma region Draw Lines

//  ���ߵĺ���ָ�롣�����ü���
extern int (*Draw_Line)(int x0, int y0, int x1, int y1, int color, UCHAR *vb_start, int lpitch);
// vb_start: video buffer start
extern int Draw_Line8(int x0, int y0, int x1, int y1, int color, UCHAR *vb_start, int lpitch);

// vb_start: video buffer start
extern int Draw_Line16(int x0, int y0, int x1, int y1, int color, UCHAR *vb_start, int lpitch);

extern int Draw_Line32(int x0, int y0, int x1, int y1, int color, UCHAR *vb_start, int lpitch);

// �ü��߶Ρ�����ֵΪ1��ʱ��û����ȫ���ü���������ֵΪ0��ʱ����ȫ���ü�����
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


#pragma region �����ι�դ���Ͷ�������

// ���ƽ�������� ,������ʱ�뷽����Ϊ�������Ͷ��㡣�����ڲ�����Ϊ��p0�Ƕ��㣬p1��left�㣬p2��right��
void Draw_Bottom_Tri(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR* dest_buffer, int mempitch);

void Draw_Bottom_Tri16(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR* dest_buffer, int mempitch);

void Draw_Bottom_Tri32(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR* dest_buffer, int mempitch);

// ���ƽ�������Σ�������ʱ�뷽����Ϊ�������Ͷ���,�����ڲ�����Ϊ��p0�ǵ͵㣬p1��right�㣬p2��left��
void Draw_Top_Tri(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR* dest_buffer, int mempitch);
void Draw_Top_Tri16(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR* dest_buffer, int mempitch);
void Draw_Top_Tri32(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR* dest_buffer, int mempitch);

// ����һ�������Ρ�����˳����ʱ�뷢�͡��ڲ�����Ϊ��p0Ϊ���㣬p1Ϊ��ߵ㣬p2Ϊ�ұߵ�
// ʹ�÷ָ�Ϊ���������εķ�ʽ�������
void Draw_Triangle_2D8(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2,
	int color, UCHAR *dest_buffer, int mempitch);

void Draw_Triangle_2D16(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2,
	int color, UCHAR *dest_buffer, int mempitch);

void Draw_Triangle_2D32(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2,
	int color, UCHAR *dest_buffer, int mempitch);


// �����������ƽ��������
void Draw_Top_TriFP8(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR *dest_buffer, int mempitch);
void Draw_Top_TriFP16(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR *dest_buffer, int mempitch);
void Draw_Top_TriFP32(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR *dest_buffer, int mempitch);

void Draw_Bottom_TriFP8(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR *dest_buffer, int mempitch);
void Draw_Bottom_TriFP16(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR *dest_buffer, int mempitch);
void Draw_Bottom_TriFP32(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR *dest_buffer, int mempitch);

void Draw_TriangleFP_2D8(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR *dest_buffer, int mempitch);
void Draw_TriangleFP_2D16(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR *dest_buffer, int mempitch);
void Draw_TriangleFP_2D32(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int color, UCHAR *dest_buffer, int mempitch);

// ������������ı���
extern inline void Draw_QuadFP_2D(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, int color, UCHAR *dest_buffer, int mempitch);

// �ı��εĶ���0,1,2,3˳ʱ�봫�룬��ô�������ָ��������Ϊ�� <0,1,3> <1,2,3>
extern inline void Draw_Quad_2D(PRECT p_clipRect, int x0, int y0, int x1, int y1,
	int x2, int y2, int x3, int y3,
	int color, UCHAR *dest_buffer, int mempitch);

// �������
extern void (*Draw_Filled_Polygon2D)(PRECT clipRect, POLYGON2D_PTR poly, UCHAR *vbuffer, int mempitch);
extern void Draw_Filled_Polygon2D8(PRECT clipRect, POLYGON2D_PTR poly, UCHAR *vbuffer, int mempitch);
extern void Draw_Filled_Polygon2D16(PRECT clipRect, POLYGON2D_PTR poly, UCHAR *vbuffer, int mempitch);
extern void Draw_Filled_Polygon2D32(PRECT clipRect, POLYGON2D_PTR poly, UCHAR *vbuffer, int mempitch);

#pragma endregion

#pragma region ��ײ��ⲿ�ֵĺ���

// ���ټ���<x,y>��<0,0>�ľ��롣ʹ����̩��չ��ʽ����û�����ף�
extern int Fast_Distance_2D(int x, int y);

extern int Find_Bounding_Box_Poly2D(POLYGON2D_PTR poly, BOUND2DF_PTR bound);

#pragma endregion

#pragma region Log ���
// error functions
extern int Open_Error_File(const char *filename, FILE *fp_override=NULL);
extern int Close_Error_File(void);
extern int Write_Error(const char *string, ...);
#pragma endregion


#pragma region ȫ�ֱ���

// �����ļ�ָ��
extern FILE *fp_error;
// �����ļ�����
extern char error_filename[80];

extern LPDIRECTDRAW7 lpdd;
extern LPDIRECTDRAWSURFACE7 lpddsprimary;		// ����ʾ����(������)
extern LPDIRECTDRAWSURFACE7 lpddsback;			// �󱸱���(��������ӱ���)
extern LPDIRECTDRAWCLIPPER lpddclipper;			// �ü���
extern LPDIRECTDRAWCLIPPER lpddclipperwin;		// ���ڲü���

extern LPDIRECTDRAWPALETTE lpddpal;
extern PALETTEENTRY palette[256];

extern DDSURFACEDESC2 ddsd;						// surface����
extern DDSCAPS2 ddcaps;							// �������������
extern DDBLTFX ddbltfx;							// �ڴ��blitter
extern HRESULT ddrval;							// dd���õķ���ֵ

extern UCHAR *primary_buffer;
extern UCHAR *back_buffer;

extern int primary_lpitch;
extern int back_lpitch;

extern DDPIXELFORMAT ddpixelformat;					// ���ظ�ʽ

extern RECT default_clipRect;						// Ĭ�ϵĲü���(�ʹ���һ����С)

extern int screen_width;
extern int screen_height;
extern int screen_bpp;
extern int screen_windowed;

extern int win_client_x0;
extern int win_client_y0;

extern int dd_pixel_format;

extern RECT client_rect;							// �û�����
extern RECT win_client_rect;						// ����ģʽ���û�����

extern float cos_look[361];
extern float sin_look[361];

// rgb��ɫ
extern void* (*RGBColor)(int r, int g, int b, int a);
extern void* RGBColor8Bit(int r, int g, int b, int a);
extern void* RGBColor16Bit565(int r, int g, int b, int a);
extern void* RGBColor16Bit555(int r, int g, int b, int a);
extern void * RGBAColor32Bit(int r, int g, int b, int a);

extern USHORT(*RGB16Bit)(int r, int g, int b);
extern USHORT RGB16Bit565(int r, int g, int b);
extern USHORT RGB16Bit555(int r, int g, int b);

// draw triangle_2d �ĺ���ָ��
extern void(*Draw_Triangle_2D)(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2,
	int color, UCHAR *dest_buffer, int mempitch);

// draw triangle fixed point 2D �ĺ���ָ��
extern void(*Draw_TriangleFP_2D)(PRECT clipRect, int x0, int y0, int x1, int y1, int x2, int y2,
	int color, UCHAR *dest_buffer, int mempitch);

#pragma endregion ȫ�ֱ���