
#include "component.h"

#define ClientHeight (192)

enum ECompLabels {ECLS_TitleLbl,ECLS_VersionLbl,ECLS_LangLbl,ECLS_ShowPrecLbl,ECLS_VolSELbl,ECLS_VolBGMLbl,ECLSCount};
#define CompLabelsCount (ECLSCount)
static TComponentLabel CompLabels[CompLabelsCount];

enum ECompChecks {ECCS_LangENGRadio,ECCS_LangJPNRadio,
                  ECCS_ShowBinChk,
                  ECCS_ShowPrecD1Radio,ECCS_ShowPrecD2Radio,ECCS_ShowPrecD3Radio,
                  ECCS_VolBGMMuteRadio,ECCS_VolBGMLowRadio,ECCS_VolBGMMidRadio,ECCS_VolBGMHighRadio,
                  ECCS_VolSEMuteRadio,ECCS_VolSELowRadio,ECCS_VolSEMidRadio,ECCS_VolSEHighRadio,
                  ECCSCount};
#define CompChecksCount (ECCSCount)
static TComponentCheck CompChecks[CompChecksCount];

enum ECompButtons {ECBS_CancelBtn,ECBS_OkBtn,ECBSCount};
#define CompButtonsCount (ECBSCount)
static TComponentButton CompButtons[CompButtonsCount];

static CglCanvas *pCompBG;

static void Setting_Redraw(CglCanvas *pcan)
{
  pCompBG->BitBltFullBeta(pcan);
  
  for(u32 idx=0;idx<CompChecksCount;idx++){
    TComponentCheck *pcc=&CompChecks[idx];
    bool *pchk=(bool*)pcc->UserData;
    if(pchk!=NULL) pcc->Checked=*pchk;
  }
  
  {
    const bool isJPNmode=Shell_isJPNmode();
    CompChecks[ECCS_LangENGRadio].Checked=!isJPNmode;
    CompChecks[ECCS_LangJPNRadio].Checked=isJPNmode;
  }
  
  {
    CompChecks[ECCS_ShowPrecD1Radio].Checked=false;
    CompChecks[ECCS_ShowPrecD2Radio].Checked=false;
    CompChecks[ECCS_ShowPrecD3Radio].Checked=false;
    
    switch(ProcState.Main.ShowPrec){
      case TProcState_Main::ESP_D1: CompChecks[ECCS_ShowPrecD1Radio].Checked=true; break;
      case TProcState_Main::ESP_D2: CompChecks[ECCS_ShowPrecD2Radio].Checked=true; break;
      case TProcState_Main::ESP_D3: CompChecks[ECCS_ShowPrecD3Radio].Checked=true; break;
      default: break;
    }
  }
  
  {
    CompChecks[ECCS_VolBGMMuteRadio].Checked=false;
    CompChecks[ECCS_VolBGMLowRadio].Checked=false;
    CompChecks[ECCS_VolBGMMidRadio].Checked=false;
    CompChecks[ECCS_VolBGMHighRadio].Checked=false;
    
    switch(ProcState.System.VolBGM){
      case TProcState_System::EV_Mute: CompChecks[ECCS_VolBGMMuteRadio].Checked=true; break;
      case TProcState_System::EV_Low:  CompChecks[ECCS_VolBGMLowRadio].Checked=true; break;
      case TProcState_System::EV_Mid:  CompChecks[ECCS_VolBGMMidRadio].Checked=true; break;
      case TProcState_System::EV_High: CompChecks[ECCS_VolBGMHighRadio].Checked=true; break;
      default: break;
    }
  }
  
  {
    CompChecks[ECCS_VolSEMuteRadio].Checked=false;
    CompChecks[ECCS_VolSELowRadio].Checked=false;
    CompChecks[ECCS_VolSEMidRadio].Checked=false;
    CompChecks[ECCS_VolSEHighRadio].Checked=false;
    
    switch(ProcState.System.VolSE){
      case TProcState_System::EV_Mute: CompChecks[ECCS_VolSEMuteRadio].Checked=true; break;
      case TProcState_System::EV_Low:  CompChecks[ECCS_VolSELowRadio].Checked=true; break;
      case TProcState_System::EV_Mid:  CompChecks[ECCS_VolSEMidRadio].Checked=true; break;
      case TProcState_System::EV_High: CompChecks[ECCS_VolSEHighRadio].Checked=true; break;
      default: break;
    }
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

static void CB_LangRadio_Click(void *pComponentCheck)
{
  TComponentCheck *pcc=(TComponentCheck*)pComponentCheck;
  const TProcState_System::ELang Lang=(TProcState_System::ELang)pcc->UserData;
  ProcState.System.Lang=Lang;
  ProcState_RequestSave=true;
  
  switch(ProcState.System.Lang){
    case TProcState_System::EL_ENG: Shell_Set_isJPNmode(false); break;
    case TProcState_System::EL_JPN: Shell_Set_isJPNmode(true); break;
  }
  
  MoveNextProc(false);
}

static void CB_CheckBox_Click(void *pComponentCheck)
{
  TComponentCheck *pcc=(TComponentCheck*)pComponentCheck;
  bool *pchk=(bool*)pcc->UserData;
  if(pchk!=NULL) *pchk=!*pchk;
  ProcState_RequestSave=true;
  
  SoundSE_Start(WAVFN_BtnSE);
}

static void CB_ShowPrecRadio_Click(void *pComponentCheck)
{
  TComponentCheck *pcc=(TComponentCheck*)pComponentCheck;
  
  ProcState.Main.ShowPrec=(TProcState_Main::EShowPrec)pcc->UserData;
  ProcState_RequestSave=true;
  
  SoundSE_Start(WAVFN_BtnSE);
}

static void CB_VolBGMRadio_Click(void *pComponentCheck)
{
  TComponentCheck *pcc=(TComponentCheck*)pComponentCheck;
  
  ProcState.System.VolBGM=(TProcState_System::EVolume)pcc->UserData;
  ProcState_RequestSave=true;
  SoundBGM_Start(WAVFN_HelpBGM,true);
}

static void CB_VolSERadio_Click(void *pComponentCheck)
{
  TComponentCheck *pcc=(TComponentCheck*)pComponentCheck;
  
  ProcState.System.VolSE=(TProcState_System::EVolume)pcc->UserData;
  ProcState_RequestSave=true;
  SoundSE_Start(WAVFN_BtnSE);
}

static void CB_CancelBtn_Click(void *pComponentButton)
{
  ProcState_RequestSave=true;
  ProcState=BackupProcState;
  
  MoveNextProc(true);
}

static void CB_OkBtn_Click(void *pComponentButton)
{
  ProcState_RequestSave=true;
  
  MoveNextProc(true);
}

static void CompsInit(void)
{
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
  
  const u32 chkw=24,chkh=14;
  
  // -----------------
  
  const u32 x=4;
  u32 y=8;
  const u32 ChecksHeight=24;
  
  {
    TComponentLabel *pcl=&CompLabels[ECLS_TitleLbl];
    pcl->Visible=true;
    pcl->isTitle=true;
    pcl->pMsgUTF8=GetLangMsg(LNGMSG_Settings_Title);
    pcl->Rect=CreateRect(x,y,0,0);
  }
  {
    TComponentLabel *pcl=&CompLabels[ECLS_VersionLbl];
    pcl->Visible=true;
    pcl->isTitle=true;
    pcl->pMsgUTF8=ROMVERSION;
    pcl->Rect=CreateRect(ScreenWidth-64,y,0,0);
  }
  y+=ChecksHeight+0;
  
  {
    {
      TComponentLabel *pcl=&CompLabels[ECLS_LangLbl];
      pcl->Visible=true;
      pcl->pMsgUTF8=GetLangMsg("Language:");
      pcl->Rect=CreateRect(0,0,0,0);
    }
    
    {
      TComponentCheck *pcc=&CompChecks[ECCS_LangENGRadio];
      pcc->Visible=true;
      pcc->CallBack_Click=CB_LangRadio_Click;
      pcc->UserData=false;
      pcc->pOnIcon=ComponentAlpha_GetSkin(ECSA_RadioOn);
      pcc->pOffIcon=ComponentAlpha_GetSkin(ECSA_RadioOff);
      pcc->pMsgUTF8=GetLangMsg("English");
      pcc->Rect=CreateRect(0,0,chkw,chkh);
    }
    
    {
      TComponentCheck *pcc=&CompChecks[ECCS_LangJPNRadio];
      pcc->Visible=true;
      pcc->CallBack_Click=CB_LangRadio_Click;
      pcc->UserData=true;
      pcc->pOnIcon=ComponentAlpha_GetSkin(ECSA_RadioOn);
      pcc->pOffIcon=ComponentAlpha_GetSkin(ECSA_RadioOff);
      pcc->pMsgUTF8=GetLangMsg("Japanese");
      pcc->Rect=CreateRect(0,0,chkw,chkh);
    }
    
    Component_AutoLocation(x,y,0,&CompLabels[ECLS_LangLbl],&CompChecks[ECCS_LangENGRadio],&CompChecks[ECCS_LangJPNRadio],NULL,NULL);
  }
  y+=ChecksHeight;
  
  {
    {
      TComponentCheck *pcc=&CompChecks[ECCS_ShowBinChk];
      pcc->Visible=true;
      pcc->UserData=(u32)&ProcState.Main.ShowBin;
      pcc->pMsgUTF8=GetLangMsg(LNGMSG_Settings_ShowBinChk);
      pcc->Rect=CreateRect(x,y,chkw,chkh);
    }
  }
  y+=ChecksHeight+2;
  
  {
    {
      TComponentLabel *pcl=&CompLabels[ECLS_ShowPrecLbl];
      pcl->Visible=true;
      pcl->pMsgUTF8=GetLangMsg(LNGMSG_Settings_ShowPrecLbl);
      pcl->Rect=CreateRect(0,0,0,0);
    }
    
    {
      TComponentCheck *pcc=&CompChecks[ECCS_ShowPrecD1Radio];
      pcc->Visible=true;
      pcc->CallBack_Click=CB_ShowPrecRadio_Click;
      pcc->UserData=TProcState_Main::ESP_D1;
      pcc->pOnIcon=ComponentAlpha_GetSkin(ECSA_RadioOn);
      pcc->pOffIcon=ComponentAlpha_GetSkin(ECSA_RadioOff);
      pcc->pMsgUTF8=GetLangMsg(LNGMSG_Settings_ShowPrecD1Radio);
      pcc->Rect=CreateRect(0,0,chkw,chkh);
    }
    
    {
      TComponentCheck *pcc=&CompChecks[ECCS_ShowPrecD2Radio];
      pcc->Visible=true;
      pcc->CallBack_Click=CB_ShowPrecRadio_Click;
      pcc->UserData=TProcState_Main::ESP_D2;
      pcc->pOnIcon=ComponentAlpha_GetSkin(ECSA_RadioOn);
      pcc->pOffIcon=ComponentAlpha_GetSkin(ECSA_RadioOff);
      pcc->pMsgUTF8=GetLangMsg(LNGMSG_Settings_ShowPrecD2Radio);
      pcc->Rect=CreateRect(0,0,chkw,chkh);
    }
    
    {
      TComponentCheck *pcc=&CompChecks[ECCS_ShowPrecD3Radio];
      pcc->Visible=true;
      pcc->CallBack_Click=CB_ShowPrecRadio_Click;
      pcc->UserData=TProcState_Main::ESP_D3;
      pcc->pOnIcon=ComponentAlpha_GetSkin(ECSA_RadioOn);
      pcc->pOffIcon=ComponentAlpha_GetSkin(ECSA_RadioOff);
      pcc->pMsgUTF8=GetLangMsg(LNGMSG_Settings_ShowPrecD3Radio);
      pcc->Rect=CreateRect(0,0,chkw,chkh);
    }
    
    Component_AutoLocation(x,y,0,&CompLabels[ECLS_ShowPrecLbl],&CompChecks[ECCS_ShowPrecD1Radio],&CompChecks[ECCS_ShowPrecD2Radio],&CompChecks[ECCS_ShowPrecD3Radio],NULL);
  }
  y+=ChecksHeight;
  
  {
    {
      TComponentLabel *pcl=&CompLabels[ECLS_VolBGMLbl];
      pcl->Visible=true;
      pcl->pMsgUTF8=GetLangMsg(LNGMSG_Settings_VolBGMLbl);
      pcl->Rect=CreateRect(0,0,0,0);
    }
    
    {
      TComponentCheck *pcc=&CompChecks[ECCS_VolBGMMuteRadio];
      pcc->Visible=true;
      pcc->CallBack_Click=CB_VolBGMRadio_Click;
      pcc->UserData=TProcState_System::EV_Mute;
      pcc->pOnIcon=ComponentAlpha_GetSkin(ECSA_RadioOn);
      pcc->pOffIcon=ComponentAlpha_GetSkin(ECSA_RadioOff);
      pcc->pMsgUTF8=GetLangMsg(LNGMSG_Settings_VolBGMMuteRadio);
      pcc->Rect=CreateRect(0,0,chkw,chkh);
    }
    
    {
      TComponentCheck *pcc=&CompChecks[ECCS_VolBGMLowRadio];
      pcc->Visible=true;
      pcc->CallBack_Click=CB_VolBGMRadio_Click;
      pcc->UserData=TProcState_System::EV_Low;
      pcc->pOnIcon=ComponentAlpha_GetSkin(ECSA_RadioOn);
      pcc->pOffIcon=ComponentAlpha_GetSkin(ECSA_RadioOff);
      pcc->pMsgUTF8=GetLangMsg(LNGMSG_Settings_VolBGMLowRadio);
      pcc->Rect=CreateRect(0,0,chkw,chkh);
    }
    
    {
      TComponentCheck *pcc=&CompChecks[ECCS_VolBGMMidRadio];
      pcc->Visible=true;
      pcc->CallBack_Click=CB_VolBGMRadio_Click;
      pcc->UserData=TProcState_System::EV_Mid;
      pcc->pOnIcon=ComponentAlpha_GetSkin(ECSA_RadioOn);
      pcc->pOffIcon=ComponentAlpha_GetSkin(ECSA_RadioOff);
      pcc->pMsgUTF8=GetLangMsg(LNGMSG_Settings_VolBGMMidRadio);
      pcc->Rect=CreateRect(0,0,chkw,chkh);
    }
    
    {
      TComponentCheck *pcc=&CompChecks[ECCS_VolBGMHighRadio];
      pcc->Visible=true;
      pcc->CallBack_Click=CB_VolBGMRadio_Click;
      pcc->UserData=TProcState_System::EV_High;
      pcc->pOnIcon=ComponentAlpha_GetSkin(ECSA_RadioOn);
      pcc->pOffIcon=ComponentAlpha_GetSkin(ECSA_RadioOff);
      pcc->pMsgUTF8=GetLangMsg(LNGMSG_Settings_VolBGMHighRadio);
      pcc->Rect=CreateRect(0,0,chkw,chkh);
    }
    
    Component_AutoLocation(x,y,0,&CompLabels[ECLS_VolBGMLbl],&CompChecks[ECCS_VolBGMMuteRadio],&CompChecks[ECCS_VolBGMLowRadio],&CompChecks[ECCS_VolBGMMidRadio],&CompChecks[ECCS_VolBGMHighRadio]);
  }
  y+=ChecksHeight;
  
  {
    {
      TComponentLabel *pcl=&CompLabels[ECLS_VolSELbl];
      pcl->Visible=true;
      pcl->pMsgUTF8=GetLangMsg(LNGMSG_Settings_VolSELbl);
      pcl->Rect=CreateRect(0,0,0,0);
    }
    
    {
      TComponentCheck *pcc=&CompChecks[ECCS_VolSEMuteRadio];
      pcc->Visible=true;
      pcc->CallBack_Click=CB_VolSERadio_Click;
      pcc->UserData=TProcState_System::EV_Mute;
      pcc->pOnIcon=ComponentAlpha_GetSkin(ECSA_RadioOn);
      pcc->pOffIcon=ComponentAlpha_GetSkin(ECSA_RadioOff);
      pcc->pMsgUTF8=GetLangMsg(LNGMSG_Settings_VolSEMuteRadio);
      pcc->Rect=CreateRect(0,0,chkw,chkh);
    }
    
    {
      TComponentCheck *pcc=&CompChecks[ECCS_VolSELowRadio];
      pcc->Visible=true;
      pcc->CallBack_Click=CB_VolSERadio_Click;
      pcc->UserData=TProcState_System::EV_Low;
      pcc->pOnIcon=ComponentAlpha_GetSkin(ECSA_RadioOn);
      pcc->pOffIcon=ComponentAlpha_GetSkin(ECSA_RadioOff);
      pcc->pMsgUTF8=GetLangMsg(LNGMSG_Settings_VolSELowRadio);
      pcc->Rect=CreateRect(0,0,chkw,chkh);
    }
    
    {
      TComponentCheck *pcc=&CompChecks[ECCS_VolSEMidRadio];
      pcc->Visible=true;
      pcc->CallBack_Click=CB_VolSERadio_Click;
      pcc->UserData=TProcState_System::EV_Mid;
      pcc->pOnIcon=ComponentAlpha_GetSkin(ECSA_RadioOn);
      pcc->pOffIcon=ComponentAlpha_GetSkin(ECSA_RadioOff);
      pcc->pMsgUTF8=GetLangMsg(LNGMSG_Settings_VolSEMidRadio);
      pcc->Rect=CreateRect(0,0,chkw,chkh);
    }
    
    {
      TComponentCheck *pcc=&CompChecks[ECCS_VolSEHighRadio];
      pcc->Visible=true;
      pcc->CallBack_Click=CB_VolSERadio_Click;
      pcc->UserData=TProcState_System::EV_High;
      pcc->pOnIcon=ComponentAlpha_GetSkin(ECSA_RadioOn);
      pcc->pOffIcon=ComponentAlpha_GetSkin(ECSA_RadioOff);
      pcc->pMsgUTF8=GetLangMsg(LNGMSG_Settings_VolSEHighRadio);
      pcc->Rect=CreateRect(0,0,chkw,chkh);
    }
    
    Component_AutoLocation(x,y,0,&CompLabels[ECLS_VolSELbl],&CompChecks[ECCS_VolSEMuteRadio],&CompChecks[ECCS_VolSELowRadio],&CompChecks[ECCS_VolSEMidRadio],&CompChecks[ECCS_VolSEHighRadio]);
  }
  y+=ChecksHeight;
  
  const u32 w=72;
  const u32 h=24;
  const u32 padx=8;
  y=ScreenHeight-h-8;
  
  {
    TComponentButton *pcb=&CompButtons[ECBS_OkBtn];
    pcb->Visible=true;
    pcb->CallBack_Click=CB_OkBtn_Click;
    pcb->pIcon=ComponentAlpha_GetSkin(ECSA_Ok);
    pcb->pMsgUTF8=GetLangMsg(LNGMSG_Settings_OkBtn);
    pcb->Rect=CreateRect(padx+64+16,y,w,h);
  }
  
  {
    TComponentButton *pcb=&CompButtons[ECBS_CancelBtn];
    pcb->Visible=true;
    pcb->CallBack_Click=CB_CancelBtn_Click;
    pcb->pIcon=ComponentAlpha_GetSkin(ECSA_Cancel);
    pcb->pMsgUTF8=GetLangMsg(LNGMSG_Settings_CancelBtn);
    pcb->Rect=CreateRect(ScreenWidth-padx-w,y,w,h);
  }
  
  // -----------------
  
  pCompBG=new CglCanvas(&MM_Process,NULL,ScreenWidth,ScreenHeight,pf15bit);
  {
    CglB15 *pb15=SkinPack_LoadB15(ESF_settings_b15);
    pb15->pCanvas->BitBltFullBeta(pCompBG);
    if(pb15!=NULL){
      delete pb15; pb15=NULL;
    }
  }
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

static bool deskmf;
static TComponentButton *pPressingButton;

static void CB_MouseDown(s32 x,s32 y)
{
  deskmf=false;
  
  for(u32 idx=0;idx<CompButtonsCount;idx++){
    TComponentButton *pcb=&CompButtons[idx];
    if(ComponentButton_GetIndexFromPos(pcb,x,y)!=-1){
      pPressingButton=pcb;
      pcb->Pressing=true;
      deskmf=true;
      return;
    }
  }
  
  deskmf=true;
}

static void CB_MouseMove(s32 x,s32 y)
{
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
  if(deskmf==false) return;
  
  deskmf=false;
  
  if(pPressingButton!=NULL){
    pPressingButton->Pressing=false;
    
    for(u32 idx=0;idx<CompButtonsCount;idx++){
      TComponentButton *pcb=&CompButtons[idx];
      if(pcb==pPressingButton){
        if(ComponentButton_GetIndexFromPos(pcb,x,y)!=-1){
          ComponentButton_MouseUp(&CompButtons[idx],x,y);
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

