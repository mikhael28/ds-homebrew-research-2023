
#include "component.h"

#define ClientHeight (192)

enum ECompLabels {ECLSCount};
#define CompLabelsCount (ECLSCount)
static TComponentLabel CompLabels[CompLabelsCount];

enum ECompChecks {ECCSCount};
#define CompChecksCount (ECCSCount)
static TComponentCheck CompChecks[CompChecksCount];

enum ECompButtons {ECBS_HelpBtn,ECBS_StartBtn,ECBS_EndBtn,ECBS_BSBtn,ECBS_ClearBtn,ECBS_SettingsBtn,
                   ECBS_DotBtn,ECBS_Num0Btn,ECBS_Num1Btn,ECBS_Num2Btn,ECBS_Num3Btn,ECBS_Num4Btn,ECBS_Num5Btn,ECBS_Num6Btn,ECBS_Num7Btn,ECBS_Num8Btn,ECBS_Num9Btn,
                   ECBS_AddBtn,ECBS_SubBtn,ECBS_MulBtn,ECBS_DivBtn,
                   ECBS_FuncSinBtn,ECBS_FuncCosBtn,ECBS_FuncTanBtn,ECBS_FuncExpBtn,ECBS_FuncLogBtn,ECBS_FuncPIBtn,
                   ECBSCount};
#define CompButtonsCount (ECBSCount)
static TComponentButton CompButtons[CompButtonsCount];

static u32 CurrentPage;

static CglCanvas *pCompBG;

static void Setting_Redraw(CglCanvas *pcan)
{
  pCompBG->BitBltFullBeta(pcan);
  
  for(u32 idx=0;idx<CompChecksCount;idx++){
    TComponentCheck *pcc=&CompChecks[idx];
    bool *pchk=(bool*)pcc->UserData;
    if(pchk!=NULL) pcc->Checked=*pchk;
  }
  
  for(u32 idx=0;idx<CompLabelsCount;idx++){
    CompLabels[idx].pCanvas=pcan;
    ComponentLabel_Draw(&CompLabels[idx]);
  }
  for(u32 idx=0;idx<CompChecksCount;idx++){
    CompChecks[idx].pCanvas=pcan;
    ComponentCheck_Draw(&CompChecks[idx]);
  }
  for(u32 idx=0;idx<CompButtonsCount;idx++){
    TComponentButton *pcb=&CompButtons[idx];
    if((pcb->pSkin==NULL)||(pcb->Pressing==true)){
      pcb->pCanvas=pcan;
      ComponentButton_Draw(pcb);
    }
  }
}

static void CB_NumPadBtn_Click(void *pComponentButton)
{
  TComponentButton *pcb=(TComponentButton*)pComponentButton;
  switch((ECompButtons)pcb->UserData){
    case ECBS_HelpBtn:  {
      SetNextProc(ENP_Help,EPFE_CrossFade);
    } break;
    case ECBS_StartBtn: ExpAddOne('('); break;
    case ECBS_EndBtn:   ExpAddOne(')'); break;
    case ECBS_BSBtn:    ExpDeleteOne(); break;
    case ECBS_ClearBtn: {
      ExpDeleteAll();
      SysMsg_SetMsgLang(LNGMSG_Main_UI_AllClear);
    } break;
    case ECBS_SettingsBtn:  {
      SetNextProc(ENP_Settings,EPFE_FastCrossFade);
    } break;
    case ECBS_DotBtn:   ExpAddOne('.'); break;
    case ECBS_Num0Btn:  ExpAddOne('0'); break;
    case ECBS_Num1Btn:  ExpAddOne('1'); break;
    case ECBS_Num2Btn:  ExpAddOne('2'); break;
    case ECBS_Num3Btn:  ExpAddOne('3'); break;
    case ECBS_Num4Btn:  ExpAddOne('4'); break;
    case ECBS_Num5Btn:  ExpAddOne('5'); break;
    case ECBS_Num6Btn:  ExpAddOne('6'); break;
    case ECBS_Num7Btn:  ExpAddOne('7'); break;
    case ECBS_Num8Btn:  ExpAddOne('8'); break;
    case ECBS_Num9Btn:  ExpAddOne('9'); break;
    case ECBS_AddBtn:   ExpAddOne('+'); break;
    case ECBS_SubBtn:   ExpAddOne('-'); break;
    case ECBS_MulBtn:   ExpAddOne('*'); break;
    case ECBS_DivBtn:   ExpAddOne('/'); break;
  }
}

static void CB_FuncBtn_Click(void *pComponentButton)
{
  TComponentButton *pcb=(TComponentButton*)pComponentButton;
  SoundSE_Start(WAVFN_Func);
  ExpAddOne((char)pcb->UserData);
}

static void CB_CheckBox_Click(void *pComponentCheck)
{
  TComponentCheck *pcc=(TComponentCheck*)pComponentCheck;
  bool *pchk=(bool*)pcc->UserData;
  if(pchk!=NULL) *pchk=!*pchk;
  ProcState_RequestSave=true;
  
  SoundSE_Start(WAVFN_BtnSE);
}

static void CB_VolBGMRadio_Click(void *pComponentCheck)
{
  TComponentCheck *pcc=(TComponentCheck*)pComponentCheck;
  
  ProcState.System.VolBGM=(TProcState_System::EVolume)pcc->UserData;
  ProcState_RequestSave=true;
  JingleBGM_VSyncCount=0;
  SoundBGM_Start(WAVFN_BGM,true);
}

static void CB_VolSERadio_Click(void *pComponentCheck)
{
  TComponentCheck *pcc=(TComponentCheck*)pComponentCheck;
  
  ProcState.System.VolSE=(TProcState_System::EVolume)pcc->UserData;
  ProcState_RequestSave=true;
  SoundSE_Start(WAVFN_BtnSE);
}

static void CompsInit_ins_SetupButton(ECompButtons ECBS,u32 blkx,u32 blky,bool dwf,const char *pCaption,ESkinFiles ESF)
{
  u32 w=32,h=32;
  u32 x=8+(blkx*(w+8)),y=24+8+(blky*(h+8));
  if(4<=blkx) x+=8;
  if(dwf==true) w*=2;
  
  TComponentButton *pcb=&CompButtons[ECBS];
  pcb->Visible=true;
  pcb->CallBack_Click=CB_NumPadBtn_Click;
  pcb->pMsgUTF8=pCaption;
  pcb->pSkin=SkinPack_LoadTGF(ESF);
  pcb->Rect=CreateRect(x,y,w,h);
  pcb->UserData=ECBS;
}

static void CompsInit_ins_SetupFuncButton(ECompButtons ECBS,u32 blkx,u32 blky,const char FCH,const char *pCaption)
{
  u32 w=64,h=32;
  u32 x=8+(blkx*(w+16)),y=48+(blky*(h+24));
  
  TComponentButton *pcb=&CompButtons[ECBS];
  pcb->Visible=true;
  pcb->CallBack_Click=CB_FuncBtn_Click;
  pcb->pMsgUTF8=pCaption;
  pcb->Rect=CreateRect(x,y,w,h);
  pcb->UserData=FCH;
}

static void CompsInit(void)
{
  CurrentPage=(u32)-1;
  
  CglCanvas *pcan=pScreenMain->pBackCanvas;
  
  for(u32 idx=0;idx<CompLabelsCount;idx++){
    ComponentLabel_Init(&CompLabels[idx],pcan);
    TComponentLabel *pcl=&CompLabels[idx];
    pcl->Visible=false;
  }
  for(u32 idx=0;idx<CompChecksCount;idx++){
    ComponentCheck_Init(&CompChecks[idx],pcan);
    TComponentCheck *pcc=&CompChecks[idx];
    pcc->Visible=false;
    pcc->CallBack_Click=CB_CheckBox_Click;
    pcc->UserData=NULL;
    pcc->pOnIcon=ComponentAlpha_GetSkin(ECSA_CheckOn);
    pcc->pOffIcon=ComponentAlpha_GetSkin(ECSA_CheckOff);
  }
  for(u32 idx=0;idx<CompButtonsCount;idx++){
    ComponentButton_Init(&CompButtons[idx],pcan);
    TComponentButton *pcb=&CompButtons[idx];
    pcb->Visible=false;
  }
  
  // -----------------
  
  CompsInit_ins_SetupButton(ECBS_HelpBtn ,0,3,false,"?" ,ESF_btns_help_tgf);
  CompsInit_ins_SetupButton(ECBS_StartBtn,0,0,false,"(" ,ESF_btns_start_tgf);
  CompsInit_ins_SetupButton(ECBS_EndBtn  ,0,1,false,")" ,ESF_btns_end_tgf);
  CompsInit_ins_SetupButton(ECBS_BSBtn   ,5,0,false,"BS",ESF_btns_bs_tgf);
  CompsInit_ins_SetupButton(ECBS_ClearBtn,5,1,false,"CL",ESF_btns_clear_tgf);
  CompsInit_ins_SetupButton(ECBS_DotBtn  ,3,3,false,"." ,ESF_btns_dot_tgf);
  CompsInit_ins_SetupButton(ECBS_Num0Btn ,1,3,true ,"0" ,ESF_btns_num0_tgf);
  CompsInit_ins_SetupButton(ECBS_Num1Btn ,1,2,false,"1" ,ESF_btns_num1_tgf);
  CompsInit_ins_SetupButton(ECBS_Num2Btn ,2,2,false,"2" ,ESF_btns_num2_tgf);
  CompsInit_ins_SetupButton(ECBS_Num3Btn ,3,2,false,"3" ,ESF_btns_num3_tgf);
  CompsInit_ins_SetupButton(ECBS_Num4Btn ,1,1,false,"4" ,ESF_btns_num4_tgf);
  CompsInit_ins_SetupButton(ECBS_Num5Btn ,2,1,false,"5" ,ESF_btns_num5_tgf);
  CompsInit_ins_SetupButton(ECBS_Num6Btn ,3,1,false,"6" ,ESF_btns_num6_tgf);
  CompsInit_ins_SetupButton(ECBS_Num7Btn ,1,0,false,"7" ,ESF_btns_num7_tgf);
  CompsInit_ins_SetupButton(ECBS_Num8Btn ,2,0,false,"8" ,ESF_btns_num8_tgf);
  CompsInit_ins_SetupButton(ECBS_Num9Btn ,3,0,false,"9" ,ESF_btns_num9_tgf);
  CompsInit_ins_SetupButton(ECBS_AddBtn  ,4,2,false,"+" ,ESF_btns_add_tgf);
  CompsInit_ins_SetupButton(ECBS_SubBtn  ,4,3,false,"-" ,ESF_btns_sub_tgf);
  CompsInit_ins_SetupButton(ECBS_MulBtn  ,4,1,false,"*" ,ESF_btns_mul_tgf);
  CompsInit_ins_SetupButton(ECBS_DivBtn  ,4,0,false,"/" ,ESF_btns_div_tgf);
  CompsInit_ins_SetupButton(ECBS_SettingsBtn,5,3,false,"CL",ESF_dummy32_tgf);
  
  // -----------------
  
  CompsInit_ins_SetupFuncButton(ECBS_FuncSinBtn,0,0,FCH_SIN,FSTR_SIN);
  CompsInit_ins_SetupFuncButton(ECBS_FuncCosBtn,1,0,FCH_COS,FSTR_COS);
  CompsInit_ins_SetupFuncButton(ECBS_FuncTanBtn,2,0,FCH_TAN,FSTR_TAN);
  CompsInit_ins_SetupFuncButton(ECBS_FuncExpBtn,0,1,FCH_EXP,FSTR_EXP);
  CompsInit_ins_SetupFuncButton(ECBS_FuncLogBtn,1,1,FCH_LOG,FSTR_LOG);
  CompsInit_ins_SetupFuncButton(ECBS_FuncPIBtn ,2,1,FCH_PI,FSTR_PI);
  
  // -----------------
  
  pCompBG=new CglCanvas(&MM_Process,NULL,ScreenWidth,ScreenHeight,pf15bit);
}

static void CompsFree(void)
{
  if(pCompBG!=NULL){
    delete pCompBG; pCompBG=NULL;
  }
  
  for(u32 idx=0;idx<CompButtonsCount;idx++){
    TComponentButton *pcb=&CompButtons[idx];
    if(pcb->pSkin!=NULL){
      delete pcb->pSkin; pcb->pSkin=NULL;
    }
  }
}

static void CompsSetPage_ins_Label_SetVisible(ECompLabels ECLS,bool Visible)
{
  TComponentLabel *pcb=&CompLabels[ECLS];
  pcb->Visible=Visible;
}

static void CompsSetPage_ins_Check_SetVisible(ECompChecks ECCS,bool Visible)
{
  TComponentCheck *pcb=&CompChecks[ECCS];
  pcb->Visible=Visible;
}

static void CompsSetPage_ins_Button_SetVisible(ECompButtons ECBS,bool Visible)
{
  TComponentButton *pcb=&CompButtons[ECBS];
  pcb->Visible=Visible;
}

static u32 CompsGetPage(void)
{
  return(CurrentPage);
}

static bool CompsSetPage(u32 _CurrentPage)
{
  if(CurrentPage==_CurrentPage) return(false);
  
  CurrentPage=_CurrentPage;
  
  if(CurrentPage==1) SoundSE_Start(WAVFN_Setting);
  
  SetProcFadeEffect(EPFE_FastCrossFade);
  
  // ----------------------------------------
  
  bool fs=false;
  switch(CurrentPage){
    case 0: fs=true; break;
    case 1: fs=false; break;
  }
  
  CompsSetPage_ins_Button_SetVisible(ECBS_HelpBtn ,fs);
  CompsSetPage_ins_Button_SetVisible(ECBS_StartBtn,fs);
  CompsSetPage_ins_Button_SetVisible(ECBS_EndBtn  ,fs);
  CompsSetPage_ins_Button_SetVisible(ECBS_BSBtn   ,fs);
  CompsSetPage_ins_Button_SetVisible(ECBS_ClearBtn,fs);
  CompsSetPage_ins_Button_SetVisible(ECBS_DotBtn  ,fs);
  CompsSetPage_ins_Button_SetVisible(ECBS_Num0Btn ,fs);
  CompsSetPage_ins_Button_SetVisible(ECBS_Num1Btn ,fs);
  CompsSetPage_ins_Button_SetVisible(ECBS_Num2Btn ,fs);
  CompsSetPage_ins_Button_SetVisible(ECBS_Num3Btn ,fs);
  CompsSetPage_ins_Button_SetVisible(ECBS_Num4Btn ,fs);
  CompsSetPage_ins_Button_SetVisible(ECBS_Num5Btn ,fs);
  CompsSetPage_ins_Button_SetVisible(ECBS_Num6Btn ,fs);
  CompsSetPage_ins_Button_SetVisible(ECBS_Num7Btn ,fs);
  CompsSetPage_ins_Button_SetVisible(ECBS_Num8Btn ,fs);
  CompsSetPage_ins_Button_SetVisible(ECBS_Num9Btn ,fs);
  CompsSetPage_ins_Button_SetVisible(ECBS_AddBtn  ,fs);
  CompsSetPage_ins_Button_SetVisible(ECBS_SubBtn  ,fs);
  CompsSetPage_ins_Button_SetVisible(ECBS_MulBtn  ,fs);
  CompsSetPage_ins_Button_SetVisible(ECBS_DivBtn  ,fs);
  CompsSetPage_ins_Button_SetVisible(ECBS_SettingsBtn,fs);
  
  CompsSetPage_ins_Button_SetVisible(ECBS_FuncSinBtn,!fs);
  CompsSetPage_ins_Button_SetVisible(ECBS_FuncCosBtn,!fs);
  CompsSetPage_ins_Button_SetVisible(ECBS_FuncTanBtn,!fs);
  CompsSetPage_ins_Button_SetVisible(ECBS_FuncExpBtn,!fs);
  CompsSetPage_ins_Button_SetVisible(ECBS_FuncLogBtn,!fs);
  CompsSetPage_ins_Button_SetVisible(ECBS_FuncPIBtn, !fs);
  
  // ----------------------------------------
  
  {
    CglB15 *pb15=SkinPack_LoadB15(ESF_mainbg_b15);
    pb15->pCanvas->BitBltFullBeta(pCompBG);
    if(pb15!=NULL){
      delete pb15; pb15=NULL;
    }
  }
  
  switch(CurrentPage){
    case 0: {
      for(u32 idx=0;idx<CompButtonsCount;idx++){
        TComponentButton *pcb=&CompButtons[idx];
        if((pcb->pSkin!=NULL)&&(pcb->Pressing==false)){
          pcb->pCanvas=pCompBG;
          ComponentButton_Draw(pcb);
        }
      }
    } break;
    case 1: {
      u16 *pbuf=pCompBG->GetVRAMBuf();
      for(u32 idx=0;idx<ScreenWidth*ScreenHeight;idx++){
        u16 col=*pbuf;
//        col=((col&0x739C)>>2)+((col&0x6318)>>3); // 37.5%
        col=((col&0x7BDE)>>1)+((col&0x739C)>>2); // 75%
        *pbuf++=col|BIT15;
      }
    } break;
  }
  
  {
    CglTGF *pTGF=SkinPack_LoadTGF(ESF_textbox_tgf);
    pTGF->BitBlt(pCompBG,0,0);
    if(pTGF!=NULL){
      delete pTGF; pTGF=NULL;
    }
  }
  
  return(true);
}

static bool deskmf;
static TComponentButton *pPressingButton;
static bool deskmf_EditBox;
static u32 deskmf_PressCount;

static void CB_MouseDown_PlaySE(TComponentButton *pcb)
{
  ECompButtons ECBS=(ECompButtons)pcb->UserData;
  
  EWAVFN WAVFN=(EWAVFN)-1;
  
  switch(ECBS){
    case ECBS_HelpBtn: {
    } break;
    case ECBS_StartBtn: {
      WAVFN=WAVFN_Start;
    } break;
    case ECBS_EndBtn: {
      WAVFN=WAVFN_End;
    } break;
    case ECBS_BSBtn: {
      WAVFN=WAVFN_Erase;
    } break;
    case ECBS_ClearBtn: {
      WAVFN=WAVFN_AllClear;
    } break;
    case ECBS_DotBtn: case ECBS_Num0Btn: case ECBS_Num1Btn: case ECBS_Num2Btn: case ECBS_Num3Btn: case ECBS_Num4Btn: case ECBS_Num5Btn: case ECBS_Num6Btn: case ECBS_Num7Btn: case ECBS_Num8Btn: case ECBS_Num9Btn: {
      WAVFN=WAVFN_Numpad;
    } break;
    case ECBS_AddBtn: case ECBS_SubBtn: case ECBS_MulBtn: case ECBS_DivBtn: {
      WAVFN=WAVFN_Ope;
    } break;
    case ECBS_SettingsBtn: {
    } break;
    default: {
    } break;
  }
  
  if(WAVFN!=(EWAVFN)-1) SoundSE_Start(WAVFN);
}

static void CB_MouseDown(s32 x,s32 y)
{
  deskmf=false;
  deskmf_PressCount=0;
  
  if(y<30){
    deskmf_EditBox=true;
    ExpMoveCur_Touch(x);
    return;
  }
  
  for(u32 idx=0;idx<CompButtonsCount;idx++){
    TComponentButton *pcb=&CompButtons[idx];
    if(ComponentButton_GetIndexFromPos(pcb,x,y)!=-1){
      pPressingButton=pcb;
      pcb->Pressing=true;
      if((idx==ECBS_HelpBtn)||(idx==ECBS_ClearBtn)||(idx==ECBS_SettingsBtn)){
        }else{
        CB_MouseDown_PlaySE(pcb);
      }
      deskmf=true;
      return;
    }
  }
  
  deskmf=true;
}

static void CB_MouseMove(s32 x,s32 y)
{
  if(deskmf_EditBox==true){
    ExpMoveCur_Touch(x);
    return;
  }
  
  if(deskmf==false) return;
  
  for(u32 idx=0;idx<CompButtonsCount;idx++){
    TComponentButton *pcb=&CompButtons[idx];
    if(pcb==pPressingButton){
      if(ComponentButton_GetIndexFromPos(pcb,x,y)==-1){
        if(pcb->Pressing==true) pcb->Pressing=false;
        }else{
        if(pcb->Pressing==false) pcb->Pressing=true;
      }
    }
  }
}

static void CB_MouseUp(s32 x,s32 y)
{
  deskmf_EditBox=false;
  
  if(deskmf==false) return;
  
  deskmf=false;
  
  if(pPressingButton!=NULL){
    pPressingButton->Pressing=false;
    
    for(u32 idx=0;idx<CompButtonsCount;idx++){
      TComponentButton *pcb=&CompButtons[idx];
      if(pcb==pPressingButton){
        if(ComponentButton_GetIndexFromPos(pcb,x,y)!=-1){
          if((pcb==&CompButtons[ECBS_HelpBtn])||(pcb==&CompButtons[ECBS_ClearBtn])||(pcb==&CompButtons[ECBS_SettingsBtn])){
            if(10<=deskmf_PressCount){
              ComponentButton_MouseUp(&CompButtons[idx],x,y);
            }
            }else{
            ComponentButton_MouseUp(&CompButtons[idx],x,y);
          }
        }
      }
    }
    
    pPressingButton=NULL;
    return;
  }
  
  for(u32 idx=0;idx<CompLabelsCount;idx++){
    if(ComponentLabel_MouseUp(&CompLabels[idx],x,y)==true){}// Setting_Redraw();
  }
  for(u32 idx=0;idx<CompChecksCount;idx++){
    if(ComponentCheck_MouseUp(&CompChecks[idx],x,y)==true){}// Setting_Redraw();
  }
  
}

