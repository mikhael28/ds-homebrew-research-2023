
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <nds.h>

#include "_const.h"
#include "maindef.h"
#include "_console.h"
#include "_consoleWriteLog.h"

#include "plug_ndsrom.h"

#include "memtool.h"
#include "strpcm.h"
#include "lang.h"
#include "procstate.h"
#include "shell.h"
#include "skin.h"
#include "skinpack.h"
#include "component.h"
#include "strtool.h"
#include "inifile.h"

#include "ipc6.h"

#include "arm9tcm.h"
#include "setarm9_reg_waitcr.h"

#include "fat2.h"
#include "io_dldi.h"
#include "zlibhelp.h"
#include "skinpack.h"

#include "sndeff.h"
#include "datetime.h"

// ------------------------------------------------------------------

u32 *pDTCMEND;

CglFont *pCglFontDefault=NULL;

ENextProc NextProc;

bool GBABUS_OwnerARM7=false;
//bool GBABUS_OwnerARM7=true; // for ARM7 debugger.

// ------------------------------------------------------------------

void __StopFatalError__(const char *filename,int linenum,const char *funcname,u32 errnum,const char* format, ...)
{
  _consoleLogResume();
  IPC6->LCDPowerControl=LCDPC_ON_BOTH;
  videoSetModeSub(MODE_2_2D | DISPLAY_BG2_ACTIVE);
  
  _consoleSetLogOutFlag(true);
  
  _consolePrintf("\n%s:%d %s: #%05d: ",filename,linenum,funcname,errnum);
  
  {
    static char strbuf[126+1];
    va_list args;
    va_start( args, format );
    vsnprintf( strbuf, 126, format, args );
    _consolePrint(strbuf);
  }
  
  if(errnum==(u32)-1){
    _consolePrintf("\n OK.\n\n");
    while(1);
  }
  
  _consolePrint("\n     Application halted !!\n");
  _consolePrint("Please refer [/moonshl2/logbuf.txt]\n\n");
  
  while(1);
}

static __attribute__ ((always_inline)) void _ttywrch_tx_waitsend(const u32 data)
{
	while((*((vu8 *)0x0A000001) & 2)!=0);
	*((vu8 *)0x0A000000)=(u8)data;
}

extern "C" {
extern void _ttywrch(int ch);
}

void _ttywrch(int ch)
{
  _ttywrch_tx_waitsend(0xa0);
  _ttywrch_tx_waitsend(2);
  _ttywrch_tx_waitsend((u32)ch);
}

// ------------------------------------------------------

static inline void CP15(void)
{
/*
  CP15_0_0：ID コードレジスタ
  CP15_0_1：キャッシュタイプレジスタ
  CP15_0_2：密結合メモリサイズレジスタ
  CP15_1_0：制御レジスタ
  CP15_2_0：キャッシュコンフィギュレーションレジスタ(Data)
  CP15_2_1：キャッシュコンフィギュレーションレジスタ(Instruction)
  CP15_3_0：ライトバッファ制御レジスタ
  CP15_5_2：アクセスパーミッションレジスタ(Data)
  CP15_5_3：アクセスパーミッションレジスタ(Instruction)
  CP15_6_0-7：保護領域 / ベースサイズレジスタ
  CP15_9_0_0：キャッシュロックダウンレジスタ(Data)
  CP15_9_0_1：キャッシュロックダウンレジスタ(Instruction)
  CP15_9_1_0：密結合メモリ領域レジスタ(Data)
  CP15_9_1_1：密結合メモリ領域レジスタ(Instruction)
  CP15_13_0：トレースプロセス識別レジスタ(Native)
  CP15_13_1：トレースプロセス識別レジスタ(Alias)
  CP15_15_0：テスト状態レジスタ
  CP15_15_1：トレース制御レジスタ
*/
  
  u32 CP15_0_0,CP15_0_1,CP15_0_2,CP15_1_0,CP15_2_0,CP15_2_1,CP15_3_0,CP15_5_2,CP15_5_3;
  u32 CP15_6_0,CP15_6_1,CP15_6_2,CP15_6_3,CP15_6_4,CP15_6_5,CP15_6_6,CP15_6_7;
  u32 CP15_9_0_0,CP15_9_0_1,CP15_9_1_0,CP15_9_1_1,CP15_13_0,CP15_13_1,CP15_15_0,CP15_15_1;
  
  asm{
    MRC p15, 0, CP15_0_0, c0, c0, 0 // ID register
    MRC p15, 0, CP15_0_1, c0, c0, 1 // cache details
    MRC p15, 0, CP15_0_2, c0, c0, 2 // tightly-coupled memory size register
    MRC p15, 0, CP15_1_0, c1, c0, 0 // control register
    MRC p15, 0, CP15_2_0, c2, c0, 0 // data cachable bits
    MRC p15, 0, CP15_2_1, c2, c0, 1 // instruction cachable bits
    MRC p15, 0, CP15_3_0, c3, c0, 0 // data bufferable bits
    MRC p15, 0, CP15_5_2, c5, c0, 2 // data access permission bits
    MRC p15, 0, CP15_5_3, c5, c0, 3 // instruction access permission bits
    MRC p15, 0, CP15_6_0, c6, c0, 0 // protection region and base size register (Region:0)
    MRC p15, 0, CP15_6_1, c6, c1, 0 // protection region and base size register (Region:1)
    MRC p15, 0, CP15_6_2, c6, c2, 0 // protection region and base size register (Region:2)
    MRC p15, 0, CP15_6_3, c6, c3, 0 // protection region and base size register (Region:3)
    MRC p15, 0, CP15_6_4, c6, c4, 0 // protection region and base size register (Region:4)
    MRC p15, 0, CP15_6_5, c6, c5, 0 // protection region and base size register (Region:5)
    MRC p15, 0, CP15_6_6, c6, c6, 0 // protection region and base size register (Region:6)
    MRC p15, 0, CP15_6_7, c6, c7, 0 // protection region and base size register (Region:7)
    MRC p15, 0, CP15_9_0_0, c9, c0, 0 // data lockdown control
    MRC p15, 0, CP15_9_0_1, c9, c0, 1 // instruction lockdown control
    MRC p15, 0, CP15_9_1_0, c9, c1, 0 // data tightly-coupled memory
    MRC p15, 0, CP15_9_1_1, c9, c1, 1 // instruction tightly-coupled memory
    MRC p15, 0, CP15_13_0, c13, c0, 1 // native process ID register
    MRC p15, 0, CP15_13_1, c13, c1, 1 // alias process ID register
    MRC p15, 0, CP15_15_0, c15, c0, 0 // test state register
    MRC p15, 1, CP15_15_1, c15, c1, 0 // trace control register
  }
  
  _consolePrintf("Memory control processor informaion.\n");
  
  _consolePrintf("CP15_0_0:0x%08x ID register.\n",CP15_0_0);
  _consolePrintf("CP15_0_1:0x%08x cache details.\n",CP15_0_1);
  _consolePrintf("CP15_0_2:0x%08x tightly-coupled memory size register.\n",CP15_0_2);
  _consolePrintf("CP15_1_0:0x%08x control register.\n",CP15_1_0);
  _consolePrintf("CP15_2_0:0x%08x data cachable bits.\n",CP15_2_0);
  _consolePrintf("CP15_2_1:0x%08x instruction cachable bits.\n",CP15_2_1);
  _consolePrintf("CP15_3_0:0x%08x data bufferable bits.\n",CP15_3_0);
  _consolePrintf("CP15_5_2:0x%08x data access permission bits.\n",CP15_5_2);
  _consolePrintf("CP15_5_3:0x%08x instruction access permission bits.\n",CP15_5_3);
  _consolePrintf("CP15_6: protection region and base size register.\n");
  _consolePrintf("0:0x%08x 1:0x%08x 2:0x%08x 3:0x%08x 4:0x%08x 5:0x%08x 6:0x%08x 7:0x%08x\n"
                  ,CP15_6_0,CP15_6_1,CP15_6_2,CP15_6_3,CP15_6_4,CP15_6_5,CP15_6_6,CP15_6_7);
  _consolePrintf("CP15_9_0_0:0x%08x data lockdown control.\n",CP15_9_0_0);
  _consolePrintf("CP15_9_0_1:0x%08x instruction lockdown control.\n",CP15_9_0_1);
  _consolePrintf("CP15_9_1_0:0x%08x data tightly-coupled memory.\n",CP15_9_1_0);
  _consolePrintf("CP15_9_1_1:0x%08x instruction tightly-coupled memory.\n",CP15_9_1_1);
  _consolePrintf("CP15_13_0:0x%08x native process ID register.\n",CP15_13_0);
  _consolePrintf("CP15_13_1:0x%08x alias process ID register.\n",CP15_13_1);
  _consolePrintf("CP15_15_0:0x%08x test state register.\n",CP15_15_0);
  _consolePrintf("CP15_15_1:0x%08x trace control register.\n",CP15_15_1);
  
  return;
  
  const u32 tempcnt=32;
  const u32 tempsize=4*1024*1024/tempcnt;
  u32 *ptemp[tempcnt];
  
  for(u32 idx=0;idx<tempcnt;idx++){
    ptemp[idx]=(u32*)safemalloc(&MM_Temp,tempsize);
    if(ptemp[idx]!=NULL){
      _consolePrintf("allocation: 0x%02x:0x%08x.\n",idx,ptemp[idx]);
    }
  }
  
  for(u32 idx=0;idx<tempcnt;idx++){
    if(ptemp[idx]!=NULL){
      _consolePrintf("fill: 0x%02x:0x%08x.\n",idx,ptemp[idx]);
      u32 *p=ptemp[idx];
      for(u32 idx=0;idx<tempsize/4;idx++){
        p[idx]=(u32)&p[idx];
      }
    }
  }
  
  for(u32 idx=0;idx<tempcnt;idx++){
    if(ptemp[idx]!=NULL){
      _consolePrintf("test: 0x%02x:0x%08x.\n",idx,ptemp[idx]);
      u32 *p=ptemp[idx];
      for(u32 idx=0;idx<tempsize/4;idx++){
        if(p[idx]!=(u32)&p[idx]){
          _consolePrintf("Error found!! 0x%08x!=0x%08x.\n",p[idx],(u32)&p[idx]);
          break;
        }
      }
    }
  }
  
  StopFatalError(0,"Memory controller test complited.\n");
}

// ------------------------------------------------------

static void DrawBootWarn(const void *pSrcBuf,u32 SrcSize,u32 palmode)
{
  TZLIBData zd;
  
  zd.DstSize=ScreenWidth*ScreenHeight;
  zd.pDstBuf=(u8*)safemalloc_chkmem(&MM_Temp,zd.DstSize);
  zd.SrcSize=SrcSize;
  zd.pSrcBuf=(u8*)pSrcBuf;
  
  if((zd.pSrcBuf[0]!=0x78)||(zd.pSrcBuf[1]!=0x01)) StopFatalError(10603,"Boot warn screen unknown compress format.\n");

  if(zlibdecompress(&zd)==false) StopFatalError(10604,"Boot warn screen ZLIB decompress error.\n");
  
  u16 pals[4];
  
  if(palmode==0){
    pals[0]=RGB15(12,0,0)|BIT15;
    pals[1]=RGB15(7,0,0)|BIT15;
    pals[2]=RGB15(31,31,31)|BIT15;
    pals[3]=RGB15(0,0,0)|BIT15;
    }else{
    pals[0]=RGB15(24,0,0)|BIT15;
    pals[1]=RGB15(28,26,27)|BIT15;
    pals[2]=RGB15(0,0,0)|BIT15;
    pals[3]=0;
  }    
  
  u16 *psrc=(u16*)zd.pDstBuf;
  u32 *pdst=(u32*)pScreenMain->pViewCanvas->GetVRAMBuf();
  for(u32 idx=0;idx<(ScreenWidth*ScreenHeight)/2;idx++){
    u32 pal16=*psrc++;
    u32 col32=pals[pal16&0xff]|(pals[pal16>>8]<<16);
    *pdst++=col32;
  }
  
  zd.DstSize=0;
  safefree(&MM_Temp,zd.pDstBuf); zd.pDstBuf=NULL;
}

// ----------------------------------------

// ----------------------------------------

// ----------------------------------------

#include "bootwarn_b8zlib.h"

// -----------------------------------------

// ------------------------------------------

static __attribute__ ((noinline)) void main_ins_start1(void)
{
  u32 zero;
  asm {
    mov zero,#0
    MCR p15, 0, zero, c7, c10, 4 // drain write buffer
  }
  
  SetARM9_REG_WaitCR();
  
  REG_POWERCNT = POWER_ALL_2D; // | POWER_SWAP_LCDS; // SWAPするとファイルリストが下
  
  _consoleInitLogFile();
  
  glDefaultMemorySetting();
  
  {
    SUB_BG2_CR = BG_256_COLOR | BG_RS_64x64 | BG_MAP_BASE(8) | BG_TILE_BASE(0) | BG_PRIORITY_1; // Tile16kb Map2kb(64x32)
    
    BG_PALETTE_SUB[(0*16)+0] = RGB15(0,0,0); // unuse (transparent)
    BG_PALETTE_SUB[(0*16)+1] = RGB15(0,0,2) | BIT(15); // BG color
    BG_PALETTE_SUB[(0*16)+2] = RGB15(0,0,0) | BIT(15); // Shadow color
    BG_PALETTE_SUB[(0*16)+3] = RGB15(16,16,16) | BIT(15); // Text color
    
    u16 XDX=(u16)((8.0/6)*0x100);
    u16 YDY=(u16)((8.0/6)*0x100);
    
    SUB_BG2_XDX = XDX;
    SUB_BG2_XDY = 0;
    SUB_BG2_YDX = 0;
    SUB_BG2_YDY = YDY;
    
    SUB_BG2_CX=(u32)1;
    SUB_BG2_CY=(u32)-1;
    
    //consoleInit() is a lot more flexible but this gets you up and running quick
    _consoleInitDefault((u16*)(SCREEN_BASE_BLOCK_SUB(8)), (u16*)(CHAR_BASE_BLOCK_SUB(0)));
  }
  
  {
    extern u32 Image$$ER_DTCM$$Base;
    extern u32 Image$$ER_DTCM$$Length;
    u32 exec_base=(u32)&Image$$ER_DTCM$$Base;
    u32 region_size=(u32)&Image$$ER_DTCM$$Length;
    _consolePrintf("DTCM top=0x%08x,size=0x%04x(%d)byte\n",exec_base,region_size,region_size);
    pDTCMEND=(u32*)(exec_base+region_size);
  }
  
  {
    // setup stack overflow checker
    u32 *p=pDTCMEND;
    for(;p<(u32*)__current_sp();p++){
      *p=(u32)p;
    }
  }
  
  MM_Init();
  
  _consolePrintf("AppName %s %s\n%s\n%s\n",ROMTITLE,ROMVERSION,ROMDATE,ROMENV);
  _consolePrintf("__current pc=0x%08x sp=0x%08x\n\n",__current_pc(),__current_sp());
  PrintFreeMem();
  
  glDefaultClassCreate();
  pCglFontDefault=NULL;
  
  {
    u64 *p=(u64*)NULL;
    *p=NULLSTR64bit;
    _consolePrintf("Test for NULL ptr. [%s]\n",NULL);
  }
  
  DrawBootWarn(bootwarn_b8zlib,bootwarn_b8zlib_Size,1);
  
  _consolePrintf("IPC6=0x%08x %dbyte.\n",IPC6,sizeof(TransferRegion6));
  
  _consolePrintf("DSTT_SDHCFlag= 0x%08x.\n",IPC6->DSTT_SDHCFlag);
  
  SetARM9_REG_WaitCR();
  if(FAT2_InitFiles()==false){
    _consolePrint("FAT2_InitFiles() failed. broken disk or not enogth free area.\n");
    while(1);
  }
  SetARM9_REG_WaitCR();
  
  if(Shell_FAT_fopen_isExists_Root(LogFilename)==true){
    FAT_FILE *pf=Shell_FAT_fopen_Root(LogFilename);
    if(pf!=NULL){
      _consoleSetLogFile(pf);
      FAT2_fclose(pf);
    }
  }
  
  {
    const char *pmsg="Unknown";
    switch(IPC6->DSType){
      case DST_DS: pmsg="DS"; break;
      case DST_DSLite: pmsg="DSLite"; break;
      case DST_DSi: pmsg="DSi"; break;
    }
    _consolePrintf("Boot with %s.\n",pmsg);
  }
  
//  CP15();
}

static __attribute__ ((noinline)) void main_ins_start2(void)
{
}

static __attribute__ ((noinline)) void main_ins_start3(void)
{
  LoadINI();
  VerboseDebugLog=GlobalINI.System.DetailsDebugLog;
  
  if(GlobalINI.DiskAdapter.SlowDiskAccess==false) FAT2_Disabled64kClusterMode();
  
//  _consoleSetLogOutFlag(true);
  
  IPC6->LCDPowerControl=LCDPC_ON_BOTH;
  
  InitInterrupts();
  strpcmSetAudioVolume64(64);
  
  if(false){
    u32 UserLanguage=(u32)-1;
    u32 Timeout=0x10000;
    
    while(UserLanguage==(u32)-1){
      UserLanguage=IPC6->UserLanguage;
      Timeout--;
      if(Timeout==0){
        _consolePrintf("NDS farmware language read error. ARM7CPU stopped...?\n");
        while(1);
      }
    }
    _consolePrintf("NDS farmware language ID : %d\n",UserLanguage);
  }
  
  _consolePrintf("Request RTC data from ARM7.\n");
  IPC6->curtimeFlag=true;
}

static __attribute__ ((noinline)) void main_ins_end(void)
{
  _consolePrint("Check: FileSystem.\n");
  u32 OpenedCount=FAT2_GetOpenedFileHandlesCount();
  if(OpenedCount!=0) StopFatalError(10610,"There is a file handle not closed. [%d]",OpenedCount); 
  
//  FAT2_FreeFiles();
  
  _consolePrint("Free: glDefaultClass.\n");
  glDefaultClassFree();
}

static __attribute__ ((noinline)) void mainloop(void);

#include "plug_ndsrom.h"

int main(void)
{
  REG_IME=0;
  
  main_ins_start1();
  main_ins_start2();
  main_ins_start3();
  
  {
    SetARM9_REG_WaitCR();
    PrintFreeMem();
    mainloop();
    PrintFreeMem();
  }
  
  REG_IME=0;
  
  main_ins_end();
  
  _consolePrint("main terminated.\n");
  
  BootNDSROM("/MOONSHL2/RELOAD.DAT");
  
  return(0);
}

// -------------------------------- mainloop

static __attribute__ ((noinline)) void WaitForVBlank(void)
{
  if(VBlankPassedFlag==false){
    swiWaitForVBlank();
  }
  VBlankPassedFlag=false;
}

// ------------------------------------------------------------

static TCallBack CallBack;

static void CallBackInit(void)
{
  MemSet32CPU(0,&CallBack,sizeof(TCallBack));
}

CODE_IN_ITCM void CallBack_ExecuteVBlankHandler(void)
{
  if(CallBack.VBlankHandler!=NULL) CallBack.VBlankHandler();
  SoundSE_IRQVBlankHandler();
}

TCallBack* CallBack_GetPointer(void)
{
  return(&CallBack);
}

// ------------------------------------------------------------

static bool mf;
static s32 mx,my;

static __attribute__ ((noinline)) void Proc_TouchPad(u32 VsyncCount) 
{
  if(IPC6->RequestUpdateIPC==true) return;
  
  bool tpress;
  s32 tx,ty;
  
  if((IPC6->buttons&IPC_PEN_DOWN)==0){
    tpress=false;
    tx=0;
    ty=0;
    }else{
    tpress=true;
    tx=IPC6->touchXpx;
    ty=IPC6->touchYpx;
  }
  
  IPC6->RequestUpdateIPC=true;
  
  if(tpress==true){
    if(mf==false){
      mf=true;
      if(CallBack.MouseDown!=NULL) CallBack.MouseDown(tx,ty);
      mx=tx;
      my=ty;
      }else{
      s32 dx=abs(mx-tx);
      s32 dy=abs(my-ty);
      if((1<=dx)||(1<=dy)){
        if(CallBack.MouseMove!=NULL) CallBack.MouseMove(tx,ty);
        mx=tx;
        my=ty;
      }
    }
    }else{
    if(mf==true){
      mf=false;
      if(CallBack.MouseUp!=NULL) CallBack.MouseUp(mx,my);
    }
  }
}

#include "main_keyrepeat.h"

static u32 KEYS_Last;
static bool KEYS_PressedLR;
static u32 KEYS_PressStartCount,KEYS_PressSelectCount;
static bool KEYS_HPSwitch_Pressed;

static __attribute__ ((noinline)) void Proc_KeyInput_Init(void)
{
  KEYS_Last=~0;
  KEYS_PressedLR=false;
  KEYS_PressStartCount=0;
  KEYS_PressSelectCount=0;
  KEYS_HPSwitch_Pressed=false;
}

#include "main_savepreview.h"

static __attribute__ ((noinline)) void Proc_KeyInput(u32 VsyncCount)
{
  if(KeyRepeatFlag==true){
    if(KeyRepeatCount<=VsyncCount){
      KeyRepeatCount=0;
      }else{
      KeyRepeatCount-=VsyncCount;
    }
  }
  
  u32 KEYS_Cur=(~REG_KEYINPUT)&0x3ff;
  
  {
    u32 btns=IPC6->buttons;
    
    KEYS_Cur|=(~REG_KEYINPUT)&0x3ff;
    if((btns&IPC_PEN_DOWN)!=0) KEYS_Cur|=KEY_TOUCH;
    if((btns&IPC_X)!=0) KEYS_Cur|=KEY_X;
    if((btns&IPC_Y)!=0) KEYS_Cur|=KEY_Y;
  }
  
  if((KEYS_Cur&(KEY_START|KEY_SELECT))==(KEY_START|KEY_SELECT)){
    if(CallBack.ReqSoftReset!=NULL) CallBack.ReqSoftReset();
  }
  
  {
    const u32 Timeout=60*1;
    if((KEYS_Cur & KEY_START)!=0){
      if(KEYS_PressStartCount<Timeout){
        KEYS_PressStartCount+=VsyncCount;
        if(Timeout<=KEYS_PressStartCount){
          if(CallBack.KeyLongPress!=NULL) CallBack.KeyLongPress(KEY_START);
          KEYS_PressStartCount=Timeout;
        }
      }
      }else{
      u32 singletime=30;
      if((KEYS_PressStartCount!=0)&&(KEYS_PressStartCount<singletime)){
        if(CallBack.KeyPress!=NULL) CallBack.KeyPress(VsyncCount,KEY_START,false);
      }
      KEYS_PressStartCount=0;
    }
    KEYS_Cur&=~KEY_START;
  }
  
  {
    const u32 Timeout=60*1;
    if((KEYS_Cur & KEY_SELECT)!=0){
      if(KEYS_PressSelectCount<Timeout){
        KEYS_PressSelectCount+=VsyncCount;
        if(IPC6->DSType!=DST_DSi){
          if(Timeout<=KEYS_PressSelectCount){
            if(CallBack.SaveScreenCapture!=NULL) CallBack.SaveScreenCapture();
            main_SavePreviewAndHalt();
            StopFatalError((u32)-1,"Saved preview files. successed.\n");
          }
        }
      }
      }else{
      u32 singletime=30;
      if(IPC6->DSType==DST_DSi) singletime=15;
      if((KEYS_PressSelectCount!=0)&&(KEYS_PressSelectCount<singletime)){
        if(CallBack.KeyPress!=NULL) CallBack.KeyPress(VsyncCount,KEY_SELECT,false);
      }
      KEYS_PressSelectCount=0;
    }
    KEYS_Cur&=~KEY_SELECT;
  }
  
  const u32 DupMask=KEY_A|KEY_B|KEY_X|KEY_Y|KEY_UP|KEY_DOWN|KEY_LEFT|KEY_RIGHT;
  
  if((KEYS_Last&DupMask)!=0) KEYS_Cur&=KEYS_Last;
  
  KEYS_Last=KEYS_Cur;
  KEYS_Cur=KeyRepeat_Proc(KEYS_Cur,VsyncCount);
  
  if(8<VsyncCount) VsyncCount=8;
  
  if(KEYS_Cur!=0){
    if(CallBack.KeyPress!=NULL) CallBack.KeyPress(VsyncCount,KEYS_Cur,KeyRepeat_GetFirstFlag());
  }
  
}

static bool Proc_PanelOpened_Last;

static __attribute__ ((noinline)) void Proc_PanelOpened(void)
{
  if(Proc_PanelOpened_Last!=IPC6->PanelOpened){
    Proc_PanelOpened_Last=IPC6->PanelOpened;
    if(Proc_PanelOpened_Last==true){
      if(CallBack.PanelOpen!=NULL) CallBack.PanelOpen();
      }else{
      if(CallBack.PanelClose!=NULL) CallBack.PanelClose();
    }
  }
}

#include "main_Trigger.h"

// ------------------------------------------------------------

EProcFadeEffect ProcFadeEffect;

static const u32 HorizontalFadeStepCount=35;
static const u32 HorizontalFadeStep[HorizontalFadeStepCount]={2,4,4,6,6,8,8,8,8,10,10,10,10,10,10,10,10,10,10,10,10,8,8,8,8,8,8,6,6,6,4,4,4,2,2,};

static const u32 VerticalFadeStepCount=27;
static const u32 VerticalFadeStep[VerticalFadeStepCount]={2,4,5,7,8,8,9,9,9,9,9,9,9,9,9,9,9,9,8,8,7,7,6,5,4,3,2,};

static void ScreenMain_Flip_ProcFadeEffect_ins_WaitForVBlank_And_LoadStreamSoundBGM(void)
{
  SoundSE_MainVBlankHandler();
  while(VBlankPassedFlag==false){
    swiWaitForVBlank();
  }
  VBlankPassedFlag=false;
}

void ScreenMain_Flip_ProcFadeEffect(void)
{
  if(NextProc!=ENP_Loop) return;
  
  if(ProcState.System.EnableFadeEffect==false) ProcFadeEffect=EPFE_None;
  
  u16 *pviewbuf=pScreenMain->pViewCanvas->GetVRAMBuf();
  u16 *pbackbuf=pScreenMain->pBackCanvas->GetVRAMBuf();
  
  switch(ProcFadeEffect){
    case EPFE_None: {
    } break;
    case EPFE_LeftToRight: {
      u32 sx=0;
      for(u32 idx=0;idx<HorizontalFadeStepCount;idx++){
        ScreenMain_Flip_ProcFadeEffect_ins_WaitForVBlank_And_LoadStreamSoundBGM();
        u32 step=HorizontalFadeStep[idx];
        u16 tmpbuf[ScreenWidth];
        for(u32 y=0;y<ScreenHeight;y++){
          u16 *pviewlinebuf=&pviewbuf[y*ScreenWidth];
          u16 *pbacklinebuf=&pbackbuf[y*ScreenWidth];
          MemCopy32CPU(&pviewlinebuf[0],tmpbuf,(ScreenWidth-step)*2);
          MemCopy32CPU(tmpbuf,&pviewlinebuf[step],(ScreenWidth-step)*2);
          MemCopy32CPU(&pbacklinebuf[ScreenWidth-step-sx],&pviewlinebuf[0],step*2);
        }
        sx+=step;
      }
    } break;
    case EPFE_RightToLeft: {
      u32 sx=0;
      for(u32 idx=0;idx<HorizontalFadeStepCount;idx++){
        ScreenMain_Flip_ProcFadeEffect_ins_WaitForVBlank_And_LoadStreamSoundBGM();
        u32 step=HorizontalFadeStep[idx];
        for(u32 y=0;y<ScreenHeight;y++){
          u16 *pviewlinebuf=&pviewbuf[y*ScreenWidth];
          u16 *pbacklinebuf=&pbackbuf[y*ScreenWidth];
          MemCopy32CPU(&pviewlinebuf[step],&pviewlinebuf[0],(ScreenWidth-step)*2);
          MemCopy32CPU(&pbacklinebuf[sx],&pviewlinebuf[ScreenWidth-step],step*2);
        }
        sx+=step;
      }
    } break;
    case EPFE_UpToDown: {
      u32 sy=0;
      for(u32 idx=0;idx<VerticalFadeStepCount;idx++){
        ScreenMain_Flip_ProcFadeEffect_ins_WaitForVBlank_And_LoadStreamSoundBGM();
        u32 step=VerticalFadeStep[idx];
        s32 y;
        y=ScreenHeight-(step*2);
        y=(y/step)*step;
        for(;y>=0;y-=step){
          u16 *pviewlinebuf=&pviewbuf[y*ScreenWidth];
          MemCopy32CPU(&pviewlinebuf[0*ScreenWidth],&pviewlinebuf[step*ScreenWidth],(step*ScreenWidth)*2);
        }
        u16 *pviewlinebuf=&pviewbuf[0*ScreenWidth];
        u16 *pbacklinebuf=&pbackbuf[0*ScreenWidth];
        MemCopy32CPU(&pbacklinebuf[(ScreenHeight-step-sy)*ScreenWidth],&pviewlinebuf[0*ScreenWidth],(step*ScreenWidth)*2);
        sy+=step;
      }
    } break;
    case EPFE_DownToUp: {
      u32 sy=0;
      for(u32 idx=0;idx<VerticalFadeStepCount;idx++){
        ScreenMain_Flip_ProcFadeEffect_ins_WaitForVBlank_And_LoadStreamSoundBGM();
        u32 step=VerticalFadeStep[idx];
        for(u32 y=0;y<ScreenHeight-step;y+=step){
          u16 *pviewlinebuf=&pviewbuf[y*ScreenWidth];
          MemCopy32CPU(&pviewlinebuf[step*ScreenWidth],&pviewlinebuf[0*ScreenWidth],(step*ScreenWidth)*2);
        }
        u16 *pviewlinebuf=&pviewbuf[0*ScreenWidth];
        u16 *pbacklinebuf=&pbackbuf[0*ScreenWidth];
        MemCopy32CPU(&pbacklinebuf[sy*ScreenWidth],&pviewlinebuf[(ScreenHeight-step)*ScreenWidth],(step*ScreenWidth)*2);
        sy+=step;
      }
    } break;
    case EPFE_CrossFade: {
      for(u32 idx=0;idx<=16;idx+=1){
        ScreenMain_Flip_ProcFadeEffect_ins_WaitForVBlank_And_LoadStreamSoundBGM();
        pScreenMain->SetBlendLevel(16-idx);
      }
    } break;
    case EPFE_FastCrossFade: {
      for(u32 idx=0;idx<=16;idx+=2){
        ScreenMain_Flip_ProcFadeEffect_ins_WaitForVBlank_And_LoadStreamSoundBGM();
        pScreenMain->SetBlendLevel(16-idx);
      }
    } break;
  }
  pScreenMain->Flip(true);
  
  if(ProcFadeEffect!=EPFE_None){
    ProcFadeEffect=EPFE_None;
    VBlankPassedCount=1;
  }
}

// ------------------------------------------------------------

static __attribute__ ((noinline)) void Proc_ExternalPowerPresent(void)
{
  static bool LastState;
  
  static bool FirstStart=true;
  if(FirstStart==true){
    FirstStart=false;
    LastState=IPC6->ExternalPowerPresent;
    return;
  }
  
  bool curstate=IPC6->ExternalPowerPresent;
  if(LastState==curstate) return;
  LastState=curstate;
  
  if(curstate==true){
    if(CallBack.ExternalPowerAttach!=NULL) CallBack.ExternalPowerAttach();
    }else{
    if(CallBack.ExternalPowerDetach!=NULL) CallBack.ExternalPowerDetach();
  }
}

// ------------------------------------------------------------

// ------------------------------------------------------------

static __attribute__ ((noinline)) void mainloop_ins_start_ins_LangInitAndLoadFont(void)
{
  Shell_FAT_fopen_LanguageInit();
  
  FAT_FILE *pf=Shell_FAT_fopen_Language_chrglyph();
  if(pf==NULL) StopFatalError(10602,"Not found default font file.\n");
  
  u32 bufsize=FAT2_GetFileSize(pf);
  void *pdummy=(u8*)safemalloc_chkmem(&MM_Temp,(bufsize*2)+(192*1024));
  u8 *pbuf=(u8*)safemalloc_chkmem(&MM_Temp,bufsize);
  
  FAT2_fread_fast(pbuf,1,bufsize,pf);
  FAT2_fclose(pf);
  
  safefree(&MM_Temp,pdummy); pdummy=NULL;
  
  if(pCglFontDefault!=NULL){
    delete pCglFontDefault; pCglFontDefault=NULL;
  }
  pCglFontDefault=new CglFont(&MM_System,(const u8*)pbuf,bufsize);
  
  safefree(&MM_Temp,pbuf); pbuf=NULL;
  
  pScreenMain->pBackCanvas->SetCglFont(pCglFontDefault);
  pScreenMain->pViewCanvas->SetCglFont(pCglFontDefault);
  pScreenMainOverlay->pCanvas->SetCglFont(pCglFontDefault);
  pScreenSub->pCanvas->SetCglFont(pCglFontDefault);
  
  _consolePrintf("Loaded font for your language.\n");
}

static __attribute__ ((noinline)) void mainloop_ins_start(void)
{
  {
    if(disc_isGBACartridge==false){
      _consolePrintf("Boot from GBA cartridge.\n");
      }else{
      _consolePrintf("Boot from NDS card.\n");
    }
    
    mainloop_ins_start_ins_LangInitAndLoadFont();
    
    {
      Lang_Load();
      const char *pfmt=Lang_GetUTF8("DateTimeFormat");
      if(isStrEqual_NoCaseSensitive(pfmt,"YYYYMMDD")==true) Date_SetDateFormat(EDF_YMD);
      if(isStrEqual_NoCaseSensitive(pfmt,"DDMMYYYY")==true) Date_SetDateFormat(EDF_DMY);
      if(isStrEqual_NoCaseSensitive(pfmt,"MMDDYYYY")==true) Date_SetDateFormat(EDF_MDY);
    }
  }
  
  ProcState_Init();
  ProcState_Load();
  switch(ProcState.System.Lang){
    case TProcState_System::EL_ENG: Shell_Set_isJPNmode(false); break;
    case TProcState_System::EL_JPN: Shell_Set_isJPNmode(true); break;
  }
  
  {
    Date_Set24hFormat(ProcState.System.Use24hFormat);
    
    _consolePrintf("Get RTC data from ARM7.\n");
    
    DateTime_ResetNow();
    TDateTime dt=DateTime_GetNow();
    
    TFAT2_TIME ft;
    ft.Year=dt.Date.Year;
    ft.Month=dt.Date.Month;
    ft.Day=dt.Date.Day;
    ft.Hour=dt.Time.Hour;
    ft.Minuts=dt.Time.Min;
    ft.Second=dt.Time.Sec;
    FAT2_SetSystemDateTime(ft);
    
    _consolePrintf("Initialize random seed.\n");
    u32 cnt=((dt.Time.Min*60)+dt.Time.Sec)&0xff;
    for(u32 idx=0;idx<cnt;idx++){
      rand();
    }
  }
  
  Component_Init();
  Component_SetFont(pCglFontDefault);
  
  {
    u32 keys=(~REG_KEYINPUT)&0x3ff;
    if(keys==(KEY_A|KEY_B|KEY_L|KEY_R)){
      ProcState_Clear();
      ProcState_RequestSave=true;
      ProcState_Save();
      ProcState_Load();
      
      pScreenSub->pCanvas->SetFontTextColor(RGB15(31,31,31)|BIT15);
      pScreenSub->pCanvas->TextOutA(8,96,"All settings were initialized.");
      pScreenSub->pCanvas->TextOutA(8,96+16,"Please release all key.");
    }
  }
  
  ApplyCurrentBacklightLevel();
  strpcmSetAudioVolume64(ProcState.System.AudioVolume64);
  
  mf=false;
  mx=0;
  my=0;
  
  KEYS_Last=0;
  KeyRepeatLastKey=0;
  KeyRepeatFlag=false;
  KeyRepeatCount=0;
  KeyRepeatFirstFlag=false;
  
  Sound_Open();
  SkinPack_Init();
  
  PrintFreeMem();

  VBlankPassedFlag=false;
  VBlankPassedCount=0;
  
  _consolePrintf("Set NextProc.\n");
  
  NextProc=ENP_Loop;
  
  MM_Compact();
  if(VerboseDebugLog==true) MM_ShowAllocated(&MM_System);
  MM_CheckOverRange();
  MM_CheckMemoryLeak(&MM_Temp);
  MM_CheckMemoryLeak(&MM_SystemAfter);
  MM_CheckMemoryLeak(&MM_Skin);
  MM_CheckMemoryLeak(&MM_DLLImage);
  MM_CheckMemoryLeak(&MM_DLLSound);
  MM_CheckMemoryLeak(&MM_SKKOSK);
  MM_CheckMemoryLeak(&MM_DLLDPG);
  MM_CheckMemoryLeak(&MM_Skin);
  MM_CheckMemoryLeak(&MM_Process);
  
  if(VerboseDebugLog==true) _consolePrintf("Set skin.\n");
  if(Skin_SetFilename(ProcState.System.SkinFilenameUnicode)==false){
    _consolePrintf("Set default skin.\n");
    StrConvert_Ank2Unicode(DefaultInternalPath "/default.skn",ProcState.System.SkinFilenameUnicode);
    if(Skin_SetFilename(ProcState.System.SkinFilenameUnicode)==false) StopFatalError(10606,"Not found skin package.\n");
  }
  
//  SaveSkinBG();
  
  SetNextProc(ENP_License,EPFE_None);
  
//  SetNextProc(ENP_Settings,EPFE_None);
//  SetNextProc(ENP_Main,EPFE_None);
  
  Proc_PanelOpened_Last=IPC6->PanelOpened;
}

// ------------------------------------------------------------

static __attribute__ ((noinline)) void mainloop_ins_end(void)
{
  _consolePrint("mainloop terminate...\n");
  
  _consolePrint("Free: Skin.\n");
  Skin_Free();
  Skin_CloseFile();
  
  _consolePrint("Free: Lang.\n");
  Lang_Free();
  
  _consolePrint("Free: Font.\n");
  if(pCglFontDefault!=NULL){
    delete pCglFontDefault; pCglFontDefault=NULL;
  }
  
  _consolePrint("Free: Sound.\n");
  Sound_Close();
  
  _consolePrint("Free: SkinPack.\n");
  SkinPack_Free();
  
  _consolePrint("Free: ProcState.\n");
  ProcState_Free();
  
  _consolePrint("Check: Memory.\n");
  MM_Compact();
  MM_CheckOverRange();
  MM_CheckMemoryLeak(&MM_Temp);
  MM_CheckMemoryLeak(&MM_System);
  MM_CheckMemoryLeak(&MM_SystemAfter);
  MM_CheckMemoryLeak(&MM_Skin);
  MM_CheckMemoryLeak(&MM_DLLImage);
  MM_CheckMemoryLeak(&MM_DLLSound);
  MM_CheckMemoryLeak(&MM_DLLDPG);
  MM_CheckMemoryLeak(&MM_SKKOSK);
  MM_CheckMemoryLeak(&MM_Process);
}

// ------------------------------------------------------------

static __attribute__ ((noinline)) void mainloop_ins_loopstart(void)
{
    { // setup stack overflow checker
      u32 *p=pDTCMEND;
      for(;p<(u32*)__current_sp();p++){
        *p=(u32)p;
      }
    }
    
    CallBackInit();
    
    switch(NextProc){
      case ENP_Loop: StopFatalError(10607,"Illigal process error! NextProc==ENP_Loop\n"); break;
      case ENP_License: Skin_Load_License(); ProcLicense_SetCallBack(&CallBack); break;
      case ENP_Main: Skin_Load_Main(); ProcMain_SetCallBack(&CallBack); break;
      case ENP_Help: Skin_Load_Main(); ProcHelp_SetCallBack(&CallBack); break;
      case ENP_Settings: Skin_Load_Settings(); ProcSettings_SetCallBack(&CallBack); break;
      default: StopFatalError(10608,"Unknown process error! NextProc==%d\n",NextProc); break;
    }
    
    { // fast stack overflow checker
      DTCM_StackCheck(-1);
    }
    
    NextProc=ENP_Loop;
    
    { // setup stack overflow checker
      u32 *p=pDTCMEND;
      for(;p<(u32*)__current_sp();p++){
        *p=(u32)p;
      }
    }
    
    if(CallBack.Start!=NULL) CallBack.Start();
    
    { // fast stack overflow checker
      DTCM_StackCheck(1000);
    }
    
    { // setup stack overflow checker
      u32 *p=pDTCMEND;
      for(;p<(u32*)__current_sp();p++){
        *p=(u32)p;
      }
    }
    
    ProcState_Save();
    
    MM_CheckOverRange();
    
    PrintFreeMem();
    
    VBlankPassedFlag=false;
    VBlankPassedCount=0;
    Proc_TouchPad(0);
    Proc_KeyInput_Init();
    Proc_Trigger(true,0);
    
    VBlankPassedCount=0;
}

// ------------------------------------------------------------

static __attribute__ ((noinline)) void mainloop_ins_loopend(void)
{
    TCallBack *pCallBack=CallBack_GetPointer();
    pCallBack->VBlankHandler=NULL;
    
    VBlankPassedFlag=false;
    VBlankPassedCount=0;
    if(CallBack.End!=NULL) CallBack.End();
    
    IPC6->LCDPowerControl=LCDPC_ON_BOTH;
    
    Skin_Free();
    
    MM_Compact();
    MM_CheckOverRange();
    MM_CheckMemoryLeak(&MM_Temp);
    MM_CheckMemoryLeak(&MM_Skin);
    MM_CheckMemoryLeak(&MM_DLLImage);
    MM_CheckMemoryLeak(&MM_DLLDPG);
    MM_CheckMemoryLeak(&MM_Skin);
    MM_CheckMemoryLeak(&MM_Process);
    
    {
      DTCM_StackCheck(1001);
    }
}

// ------------------------------------------------------------

extern bool ReqSoftResetFlag;
bool ReqSoftResetFlag=false;

static __attribute__ ((noinline)) void mainloop(void)
{
  _consolePrint("mainloop.\n");
  
  mainloop_ins_start();
  
  DTCM_StackCheck(0);
  
  _consolePrintf("Start event loop...\n");
  
  while(1){//break;
    strpcm_ExclusivePause=true;
    mainloop_ins_loopstart();
    strpcm_ExclusivePause=false;
    
    while(NextProc==ENP_Loop){
      while(VBlankPassedFlag==false){
        swiWaitForIRQ();
      }
      VBlankPassedFlag=false;
      
      REGIME_Pause();
      u32 vsynccount=VBlankPassedCount;
      VBlankPassedCount=0;
      REGIME_Resume();
      
      if(ReqSoftResetFlag==false){
        if(CallBack.VsyncUpdate!=NULL) CallBack.VsyncUpdate(vsynccount);
      }
      
      SoundSE_MainVBlankHandler();
      
      Proc_TouchPad(vsynccount);
      Proc_KeyInput(vsynccount);
      Proc_PanelOpened();
      Proc_Trigger(false,vsynccount);
      Proc_ExternalPowerPresent();
      
      { // fast stack overflow checker
        DTCM_StackCheck(-1);
      }
      
/*
      if(IPC6->DebugStrFlag!=0){
        pScreenSub->pCanvas->SetColor(RGB15(0,0,0)|BIT15);
        pScreenSub->pCanvas->FillBox(0,0,256,24);
        pScreenSub->pCanvas->SetFontTextColor(RGB15(31,31,31)|BIT15);
        pScreenSub->pCanvas->TextOutA(8,8,(char*)IPC6->DebugStr);
        IPC6->DebugStrFlag=0;
      }
*/
      
    }
    
    strpcm_ExclusivePause=true;
    mainloop_ins_loopend();
    
    if(ReqSoftResetFlag==true) break;
  }
  
  mainloop_ins_end();
  
  _consolePrint("mainloop terminated.\n");
}

