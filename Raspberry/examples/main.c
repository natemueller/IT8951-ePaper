#include "../lib/Config/DEV_Config.h"
#include "../lib/GUI/GUI_BMPfile.h"
#include "../lib/e-Paper/EPD_IT8951.h"
#include "../lib/GUI/GUI_Paint.h"

#include <math.h>

#include <stdlib.h>     //exit()
#include <signal.h>     //signal()

#define Enhance false

#define USE_Factory_Test false

#define USE_Normal_Demo true

#define USE_Touch_Panel false

UWORD VCOM = 2510;

IT8951_Dev_Info Dev_Info;
UWORD Panel_Width;
UWORD Panel_Height;
UDOUBLE Init_Target_Memory_Addr;

void Handler(int signo){
  EPD_IT8951_Sleep();
  DEV_Module_Exit();
  exit(0);
}

int main(int argc, char *argv[])
{
  signal(SIGINT, Handler);

  if (argc != 3) {
    Debug("epd -2.51 /path/to/image.bmp\n");
    exit(1);
  }

  if(DEV_Module_Init() != 0) {
    return -1;
  }

  double temp;
  sscanf(argv[1],"%lf",&temp);
  VCOM = (UWORD)(fabs(temp)*1000);
  Dev_Info = EPD_IT8951_Init(VCOM);

  //get some important info from Dev_Info structure
  Panel_Width = Dev_Info.Panel_W;
  Panel_Height = Dev_Info.Panel_H;
  Init_Target_Memory_Addr = Dev_Info.Memory_Addr_L | (Dev_Info.Memory_Addr_H << 16);

	EPD_IT8951_Clear_Refresh(Dev_Info, Init_Target_Memory_Addr, INIT_Mode);

  UDOUBLE Imagesize = Panel_Width * 0.5 * Panel_Height;
  UBYTE *Refresh_Frame_Buf = NULL;
  if ((Refresh_Frame_Buf = (UBYTE *)malloc(Imagesize)) == NULL) {
    Debug("Failed to apply for black memory...\r\n");
    return -1;
  }

  Paint_NewImage(Refresh_Frame_Buf, Panel_Width, Panel_Height, 0, BLACK);
  Paint_SelectImage(Refresh_Frame_Buf);
  Paint_SetRotate(ROTATE_0);
  Paint_SetMirroring(MIRROR_HORIZONTAL);
  Paint_SetBitsPerPixel(4);

  GUI_ReadBmp(argv[2], 0, 0);

  EPD_IT8951_4bp_Refresh(Refresh_Frame_Buf, 0, 0, Panel_Width, Panel_Height, true, Init_Target_Memory_Addr, false);

  free(Refresh_Frame_Buf);
  EPD_IT8951_Sleep();
  DEV_Module_Exit();
  return 0;
}
