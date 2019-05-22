#pragma once
#define DIKEYDOWN(data,n) (data[n])&0x80)
extern LPDIRECTINPUT8 lpdi;

extern LPDIRECTINPUTDEVICE8 lpdikey;
extern LPDIRECTINPUTDEVICE8 lpdimouse;
extern LPDIRECTINPUTDEVICE8 lpdijoy;

extern GUID joystickGUID;
extern char joyname[80];

extern UCHAR keybord_state[256];
extern DIMOUSESTATE mouse_state;
extern DIJOYSTATE joy_state;
extern int joystick_found;


int DInput_Init(HINSTANCE mainInstance,HWND mainWindowHandle);
void DInput_Shutdown(void);

int DInput_Init_Joystick(int min_x = -256, int max_x = 256, int min_y = -256, int max_y = 256,int dead_zone=1);
int DInput_Init_Mouse(void);
int DInput_Init_Keyboard(void);

int DInput_Read_Jyostick(void);
int DInput_Read_Mouse(void);
int DInput_Read_Keyboard(void);

int DInput_Release_Joystick(void);
int DInput_Release_Mouse(void);
int DInput_Release_Keyboard(void);
