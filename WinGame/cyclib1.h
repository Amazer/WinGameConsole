#pragma once
#ifndef CYCLIB1
#define CYCLIB1

typedef unsigned char BYTE;			// 1���ֽ�
typedef unsigned char UCHAR;		// 1���ֽ�
typedef unsigned int  UINT;			// 4���ֽ�
typedef unsigned short USHORT;		// 2���ֽ�
typedef unsigned short UWORD;		// 2���ֽ�
typedef unsigned long UDWORD;		// 4���ֽ�

typedef int FIXPOINT;				// ������

#define __RGB16BIT555(r,g,b) ((b&31)+((g&31)<<5)+((r&31)<<10))
#define __RGB16BIT565(r,g,b) ((b&31)+((g&63)<<5)+((r&31)<<11))
#define __RGB24BIT(r,g,b) (b+(g<<8)+(r<<16))
#define __RGB32BIT(a,r,g,b) (b+(g<<8)+(r<<16)+(a<<24))

#define DDRAW_INIT_STRUCT(ddstruct) {memset(&ddstruct,0,sizeof(ddstruct)); ddstruct.dwSize=sizeof(ddstruct);}

#define SCREEN_WIDTH 640	//640		// ��Ļ���   ����ģʽ,������display mode,ʹ��ϵͳ��ɫ��λ��
#define SCREEN_HEIGHT 480	//480 		// ��Ļ�߶�   

#define WIN_OFFSCREEN_WIDTH 720	//640		����ģʽ�µ������������ 
#define WIN_OFFSCREEN_HEIGHT 720	//480 		

#define SCREEN_BPP 8			// ɫ��λ��  ����ģʽ��������
#define MS_PER_FRAME 15			// ÿ֡������

#define FULL_SCREEN_MODE 1			

#define BITMAP_ID 0x4D42		// bitmap����id
#define MAX_COLORS_PALETTE 256	// ��ɫ������������

#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code)&0x8000)?1:0)
#define KEYUP(vk_code) ((GetAsyncKeyState(vk_code)&0x8000)?0:1)

#define CLIP_CODE_C 0x0000
#define CLIP_CODE_N 0x0008
#define CLIP_CODE_S 0x0004
#define CLIP_CODE_E 0x0002
#define CLIP_CODE_W 0x0001

#define CLIP_CODE_NE 0x000a
#define CLIP_CODE_SE 0x0006
#define CLIP_CODE_NW 0x0009
#define CLIP_CODE_SW 0x0005

/////////////// ���������
// ������16.16;�������32��ת��16.16�Ķ�������
// ȡ����ֵ��ʱ�򣬶���32λ����ֻ��16λ��Ч��
#define FIXP16_SHIFT 16					// ��Ҫƫ�Ƶ�λ��
#define FIXP16_MAG	65536				// ������->����Ҫ�Ŵ�ı��� =pow(2,16)
#define FIXP16_DP_MASK	0x0000ffff		// ������ С�����ֵ�mask
#define FIXP16_WP_MASK	0xffff0000		// ������ �������ֵ�mask
#define FIXP16_ROUND_UP 0x00008000		// ��������ʾ��0.5���������ⶪʧ���ȵ�

// int32 -> fix point 16.16;�õ��Ļ���32λ. �Ŵ�65536��,���ұ�Ϊ���޷���������
#define INT_2_FIXP(n) (n<<FIXP16_SHIFT);

// ������pת�����ķ����ǣ� (p+����+1)>>16
#define FIXP_2_INT(p) ((p+FIXP16_ROUND_UP)>>FIXP16_SHIFT);


////////////////////////////////



extern inline void Plot_Pixel32(int x, int y, int alpha, int red, int green, int blue, UINT *video_buffer, int lpitch32);
extern inline void Plot_Pixel32(int x, int y, UINT color, UINT *video_buffer, int lpitch32);
extern inline void Plot8(int x, int y, UCHAR color, UCHAR *buffer, int mempitch);
extern inline void Plot16BIT555(int x, int y, UCHAR red, UCHAR green, UCHAR blue, USHORT *buffer, int mempitch);
extern inline void Plot16BIT565(int x, int y, UCHAR red, UCHAR green, UCHAR blue, USHORT *buffer, int mempitch);
extern inline void Plot16BIT565(int x, int y, USHORT color, USHORT *buffer, int mempitch);
extern inline UINT RandomRGB16BIT565();
extern inline UINT RandomRGBA32();

// �ü���bitmap�ӣ�0��0���㿪ʼ����λ��
extern void Blit_Clipped(int posX, int posY, int sizeWidth, int sizeHeight, UCHAR *bitmap, UCHAR *video_buffer, int mempitch);

#endif
