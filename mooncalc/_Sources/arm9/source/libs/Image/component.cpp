
#pragma Ospace

#include <NDS.h>

#include "_const.h"
#include "_console.h"

#include "maindef.h"
#include "memtool.h"
#include "lang.h"

#include "component.h"

#include "skin.h"
#include "strtool.h"

#include "ipc6.h"

// ---------------------

static CglFont *pDefaultFont;

void Component_Init(void)
{
  pDefaultFont=NULL;
}

void Component_SetFont(CglFont *pFont)
{
  pDefaultFont=pFont;
}

// ---------------------

static void CTF_SetDefaultFont(CglCanvas *pCanvas)
{
  pCanvas->SetCglFont(pDefaultFont);
}

static void CTF_SetFontTextColor(CglCanvas *pCanvas,u16 col)
{
  pCanvas->SetFontTextColor(col);
}

static int CTF_GetTextWidthUTF8(CglCanvas *pCanvas,const char *pstr)
{
  return(pCanvas->GetTextWidthUTF8(pstr));
}

static void CTF_TextOutUTF8(CglCanvas *pCanvas,int x,int y,const char *pstr)
{
  pCanvas->TextOutUTF8(x,y,pstr);
}

static int CTF_GetTextWidthLang(CglCanvas *pCanvas,const char *pstr)
{
  return(pCanvas->GetTextWidthLang(pstr));
}

static void CTF_TextOutLang(CglCanvas *pCanvas,int x,int y,const char *pstr)
{
  pCanvas->TextOutLang(x,y,pstr);
}

// ---------------------

void ComponentLabel_Init(TComponentLabel *pComponentLabel,CglCanvas *pCanvas)
{
  TComponentLabel *pcl=pComponentLabel;
  
  pcl->CallBack_Click=NULL;
  pcl->pCanvas=pCanvas;
  pcl->Rect=CreateRect(0,0,0,0);
  pcl->Visible=true;
  pcl->Center=false;
  pcl->isTitle=false;
  pcl->TextColor=0;
  pcl->pMsgUTF8="";
}

void ComponentLabel_Draw(TComponentLabel *pComponentLabel)
{
  TComponentLabel *pcl=pComponentLabel;
  
  if(pcl->Visible==false) return;
  
  CTF_SetDefaultFont(pcl->pCanvas);
  if(pcl->TextColor!=0){
    CTF_SetFontTextColor(pcl->pCanvas,pcl->TextColor);
    }else{
    if(pcl->isTitle==false){
      CTF_SetFontTextColor(pcl->pCanvas,ColorTable.Component.Label_Text);
      }else{
      CTF_SetFontTextColor(pcl->pCanvas,ColorTable.Component.TitleLabel_Text);
    }
  }
  
  const char *pmsgUTF8=pcl->pMsgUTF8;
  
  u32 TextWidth=CTF_GetTextWidthLang(pcl->pCanvas,pmsgUTF8);
  u32 TextHeight=glCanvasTextHeight;
  
  if(pcl->Rect.w==0) pcl->Rect.w=TextWidth;
  if(pcl->Rect.h==0) pcl->Rect.h=TextHeight;
  
  TRect r=pcl->Rect;
  
  if(pmsgUTF8[0]!=0){
    if(pcl->Center==false){
      CTF_TextOutLang(pcl->pCanvas,r.x,r.y,pmsgUTF8);
      }else{
      CTF_TextOutLang(pcl->pCanvas,r.x+((r.w-TextWidth)/2),r.y,pmsgUTF8);
    }
  }
}

s32 ComponentLabel_GetWidth(TComponentLabel *pComponentLabel)
{
  TComponentLabel *pcl=pComponentLabel;
  
  if(pcl->Visible==false) return(0);
  
  CTF_SetDefaultFont(pcl->pCanvas);
  CTF_SetFontTextColor(pcl->pCanvas,ColorTable.Component.Label_Text);
  
  const char *pmsgUTF8=pcl->pMsgUTF8;
  
  u32 TextWidth=CTF_GetTextWidthLang(pcl->pCanvas,pmsgUTF8);
  
  if(pcl->Rect.w==0) pcl->Rect.w=TextWidth;
  
  TRect r=pcl->Rect;
  
  s32 w=0;
  
  if(pmsgUTF8[0]!=0){
    if(pcl->Center==false){
      w=CTF_GetTextWidthLang(pcl->pCanvas,pmsgUTF8);
      }else{
      w=r.w;
    }
  }
  
  return(w+2);
}

s32 ComponentLabel_GetIndexFromPos(TComponentLabel *pComponentLabel,s32 mx,s32 my)
{
  TComponentLabel *pcl=pComponentLabel;
  TRect r=pcl->Rect;
  
  if(pcl->Visible==false) return(-1);
  
  mx-=r.x;
  my-=r.y;
  
  if((0<=mx)&&(mx<r.w)){
    if((0<=my)&&(my<r.h)){
      return(0);
    }
  }
  
  return(-1);
}

bool ComponentLabel_MouseUp(TComponentLabel *pComponentLabel,s32 mx,s32 my)
{
  TComponentLabel *pcl=pComponentLabel;
  
  if(pcl->Visible==false) return(false);
  
  if(ComponentLabel_GetIndexFromPos(pcl,mx,my)==-1) return(false);
  
  if(pcl->CallBack_Click!=NULL) pcl->CallBack_Click(pcl);
  
  return(true);
}

// ------------------

void ComponentCheck_Init(TComponentCheck *pComponentCheck,CglCanvas *pCanvas)
{
  TComponentCheck *pcc=pComponentCheck;
  
  pcc->CallBack_Click=NULL;
  pcc->UserData=0;
  pcc->pCanvas=pCanvas;
  pcc->pOnIcon=NULL;
  pcc->pOffIcon=NULL;
  pcc->Checked=false;
  pcc->Rect=CreateRect(0,0,0,0);
  pcc->Visible=true;
  pcc->pMsgUTF8="";
  pcc->TextColor=ColorTable.Component.Check_Text;
}

void ComponentCheck_Draw(TComponentCheck *pComponentCheck)
{
  TComponentCheck *pcc=pComponentCheck;
  
  if(pcc->Visible==false) return;
  
  if(pcc->pOnIcon!=NULL){
    if(pcc->Rect.w==0) pcc->Rect.w=pcc->pOnIcon->GetWidth();
    if(pcc->Rect.h==0) pcc->Rect.h=pcc->pOnIcon->GetHeight();
  }
  
  TRect r=pcc->Rect;
  
  CglTGF *pSrcTGF=NULL;
  if(pcc->Checked==true){
    pSrcTGF=pcc->pOnIcon;
    }else{
    pSrcTGF=pcc->pOffIcon;
  }
  if(pSrcTGF!=NULL){
    s32 x=r.x;
    s32 y=r.y;
    s32 w=pSrcTGF->GetWidth();
    s32 h=pSrcTGF->GetHeight();
    x+=(r.w-w+0)/2;
    y+=(r.h-h+1)/2;
    pSrcTGF->BitBlt(pcc->pCanvas,x,y);
  }
  
  CTF_SetDefaultFont(pcc->pCanvas);
  CTF_SetFontTextColor(pcc->pCanvas,pcc->TextColor);
  
  const char *pmsgUTF8=pcc->pMsgUTF8;
  
  u32 TextX=r.x+r.w+2;
  u32 TextY=r.y;
//  u32 TextWidth=CTF_GetTextWidthLang(pcc->pCanvas,pmsgUTF8);
  u32 TextHeight=glCanvasTextHeight;
  TextY+=(r.h-TextHeight+1)/2;
  
  CTF_TextOutLang(pcc->pCanvas,TextX,TextY,pmsgUTF8);
}

s32 ComponentCheck_GetWidth(TComponentCheck *pComponentCheck)
{
  TComponentCheck *pcc=pComponentCheck;
  
  if(pcc->Visible==false) return(0);
  
  if(pcc->pOnIcon!=NULL){
    if(pcc->Rect.w==0) pcc->Rect.w=pcc->pOnIcon->GetWidth();
  }
  
  TRect r=pcc->Rect;
  
  CTF_SetDefaultFont(pcc->pCanvas);
  
  const char *pmsgUTF8=pcc->pMsgUTF8;
  
  return(r.w+2+CTF_GetTextWidthLang(pcc->pCanvas,pmsgUTF8)+2);
}

static bool ComponentCheck_GetIndexFromPos(TComponentCheck *pComponentCheck,s32 mx,s32 my)
{
  TComponentCheck *pcc=pComponentCheck;
  TRect r=pcc->Rect;
  
  if(pcc->Visible==false) return(false);
  
  CTF_SetDefaultFont(pcc->pCanvas);
  const char *pmsgUTF8=pcc->pMsgUTF8;
  s32 TextWidth;
  if(str_isEmpty(pcc->pMsgUTF8)==true){
    TextWidth=ScreenWidth;
    }else{
    TextWidth=r.w+2+CTF_GetTextWidthLang(pcc->pCanvas,pmsgUTF8);
  }
  
  mx-=r.x;
  my-=r.y;
  
  if((0<=mx)&&(mx<TextWidth)){
    if((0<=my)&&(my<r.h)){
      return(true);
    }
  }
  
  return(false);
}

bool ComponentCheck_MouseUp(TComponentCheck *pComponentCheck,s32 mx,s32 my)
{
  TComponentCheck *pcc=pComponentCheck;
  
  if(pcc->Visible==false) return(false);
  
  if(ComponentCheck_GetIndexFromPos(pcc,mx,my)==false) return(false);
  
  if(pcc->CallBack_Click!=NULL) pcc->CallBack_Click(pcc);
  
  return(true);
}

// ------------------

void ComponentButton_Init(TComponentButton *pComponentButton,CglCanvas *pCanvas)
{
  TComponentButton *pcb=pComponentButton;
  
  pcb->CallBack_Click=NULL;
  pcb->pCanvas=pCanvas;
  pcb->pSkin=NULL;
  pcb->pIcon=NULL;
  pcb->DrawFrame=true;
  pcb->Pressing=false;
  pcb->Rect=CreateRect(0,0,0,0);
  pcb->pMsgUTF8="";
  pcb->Visible=true;
  pcb->NormalTextColor=ColorTable.Component.Button_NormalText;
  if(pcb->NormalTextColor==(RGB15(0,0,0)|BIT15)) pcb->NormalTextColor=RGB15(0,0,1)|BIT15;
  pcb->PressTextColor=ColorTable.Component.Button_PressText;
  if(pcb->PressTextColor==(RGB15(0,0,0)|BIT15)) pcb->PressTextColor=RGB15(0,0,1)|BIT15;
  pcb->UserData=0;
}

void ComponentButton_Draw(TComponentButton *pComponentButton)
{
  TComponentButton *pcb=pComponentButton;
  
  if(pcb->Visible==false) return;
  
  if(pcb->pSkin!=NULL){
    CglTGF *pTGF=pcb->pSkin;
    TRect r=pcb->Rect;
    u32 ofs=0;
    u32 h=r.h+8+8;
    if(pcb->Pressing==true) ofs=h;
    pTGF->BitBltLimitY(pcb->pCanvas,r.x-8,r.y-8,h,ofs);
    return;
  }
  
  if(pcb->DrawFrame==true){
    if(pcb->Pressing==false){
      TRect r=pcb->Rect;
      pcb->pCanvas->SetColor(ColorTable.Component.Button_NormalHighlight);
      pcb->pCanvas->DrawBox(r.x-1,r.y-1,r.w+2,r.h+2);
      pcb->pCanvas->SetColor(ColorTable.Component.Button_NormalShadow);
      pcb->pCanvas->DrawBox(r.x,r.y,r.w+1,r.h+1);
      pcb->pCanvas->SetColor(ColorTable.Component.Button_NormalBG);
      pcb->pCanvas->FillBox(r.x,r.y,r.w,r.h);
      }else{
      TRect r=pcb->Rect;
      pcb->pCanvas->SetColor(ColorTable.Component.Button_PressHighlight);
      pcb->pCanvas->DrawBox(r.x-1,r.y-1,r.w+2,r.h+2);
      pcb->pCanvas->SetColor(ColorTable.Component.Button_PressShadow);
      pcb->pCanvas->DrawBox(r.x,r.y,r.w+1,r.h+1);
      pcb->pCanvas->SetColor(ColorTable.Component.Button_PressBG);
      pcb->pCanvas->FillBox(r.x,r.y,r.w,r.h);
    }
  }
  
  CTF_SetDefaultFont(pcb->pCanvas);
  if(pcb->Pressing==false){
    CTF_SetFontTextColor(pcb->pCanvas,pcb->NormalTextColor);
    }else{
    CTF_SetFontTextColor(pcb->pCanvas,pcb->PressTextColor);
  }
  
  const char *pmsgUTF8=pcb->pMsgUTF8;
  
  TRect r=pcb->Rect;
  
  s32 IconWidth;
  s32 IconHeight;
  if(pcb->pIcon==NULL){
    IconWidth=-8;
    IconHeight=0;
    }else{
    IconWidth=pcb->pIcon->GetWidth();
    IconHeight=pcb->pIcon->GetHeight();
  }
  
  s32 TextWidth=CTF_GetTextWidthLang(pcb->pCanvas,pmsgUTF8);
  s32 TextHeight=glCanvasTextHeight;
  
  s32 BodyWidth=IconWidth;
  if(TextWidth!=0) BodyWidth+=8+TextWidth;
  s32 BodyHeight=IconHeight;
  if(IconHeight<TextHeight) BodyHeight=TextHeight;
  s32 BodyX=r.x+((r.w-BodyWidth)/2);
  s32 BodyY=r.y+((r.h-BodyHeight)/2);
  
  if(pcb->pIcon!=NULL){
    s32 x=BodyX;
    s32 y=BodyY+((BodyHeight-IconHeight)/2);
    pcb->pIcon->BitBlt(pcb->pCanvas,x,y);
  }
  
  CTF_TextOutLang(pcb->pCanvas,BodyX+8+IconWidth,BodyY+((BodyHeight-TextHeight)/2),pmsgUTF8);
}

s32 ComponentButton_GetIndexFromPos(TComponentButton *pComponentButton,s32 mx,s32 my)
{
  TComponentButton *pcb=pComponentButton;
  TRect r=pcb->Rect;
  
  if(pcb->Visible==false) return(-1);
  
  mx-=r.x;
  my-=r.y;
  
  if((0<=mx)&&(mx<r.w)){
    if((0<=my)&&(my<r.h)){
      return(0);
    }
  }
  
  return(-1);
}

bool ComponentButton_MouseUp(TComponentButton *pComponentButton,s32 mx,s32 my)
{
  TComponentButton *pcb=pComponentButton;
  
  if(pcb->Visible==false) return(false);
  
  if(ComponentButton_GetIndexFromPos(pcb,mx,my)==-1) return(false);
  
  if(pcb->CallBack_Click!=NULL) pcb->CallBack_Click(pcb);
  
  return(true);
}

// ------------------

void Component_AutoLocation(u32 x,u32 y0,u32 y1,TComponentLabel *pc0,TComponentCheck *pc1,TComponentCheck *pc2,TComponentCheck *pc3,TComponentCheck *pc4)
{
  u32 w0=ComponentLabel_GetWidth(pc0);
  u32 w1=ComponentCheck_GetWidth(pc1);
  u32 w2=ComponentCheck_GetWidth(pc2);
  u32 w3=0;
  if(pc3!=NULL) w3=ComponentCheck_GetWidth(pc3);
  u32 w4=0;
  if(pc4!=NULL) w4=ComponentCheck_GetWidth(pc4);
  
  pc0->Rect.x=x;
  pc0->Rect.y=y0;
  
  if(y1==0){
    x+=w0;
    pc1->Rect.x=x;
    pc1->Rect.y=y0;
    x+=w1;
    pc2->Rect.x=x;
    pc2->Rect.y=y0;
    x+=w2;
    if(pc3!=NULL){
      pc3->Rect.x=x;
      pc3->Rect.y=y0;
    }
    x+=w3;
    if(pc4!=NULL){
      pc4->Rect.x=x;
      pc4->Rect.y=y0;
    }
    return;
  }
  
  if((x+w1+w2+w3+w4)<ScreenWidth){
    pc1->Rect.x=x;
    pc1->Rect.y=y1;
    pc2->Rect.x=x+w1;
    pc2->Rect.y=y1;
    if(pc3!=NULL){
      pc3->Rect.x=x+w1+w2;
      pc3->Rect.y=y1;
    }
    if(pc4!=NULL){
      pc4->Rect.x=x+w1+w2+w3;
      pc4->Rect.y=y1;
    }
    return;
  }
  
  if((x+w2+w3+w4)<ScreenWidth){
    pc1->Rect.x=x+w0;
    pc1->Rect.y=y0;
    pc2->Rect.x=x;
    pc2->Rect.y=y1;
    if(pc3!=NULL){
      pc3->Rect.x=x+w2;
      pc3->Rect.y=y1;
    }
    if(pc4!=NULL){
      pc4->Rect.x=x+w2+w3;
      pc4->Rect.y=y1;
    }
    return;
  }
  
  if((x+w3+w4)<ScreenWidth){
    pc1->Rect.x=x+w0;
    pc1->Rect.y=y0;
    pc2->Rect.x=x+w0+w1;
    pc2->Rect.y=y0;
    if(pc3!=NULL){
      pc3->Rect.x=x;
      pc3->Rect.y=y1;
    }
    if(pc4!=NULL){
      pc4->Rect.x=x+w3;
      pc4->Rect.y=y1;
    }
    return;
  }
  
  pc1->Rect.x=x+w0;
  pc1->Rect.y=y0;
  pc2->Rect.x=x+w0+w1;
  pc2->Rect.y=y0;
  if(pc3!=NULL){
    pc3->Rect.x=x+w0+w1+w2;
    pc3->Rect.y=y0;
  }
  if(pc4!=NULL){
    pc4->Rect.x=x;
    pc4->Rect.y=y1;
  }
}

// ------------------

