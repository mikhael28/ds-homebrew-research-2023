
#include <nds.h>
#include <math.h>

#include "_const.h"
#include "maindef.h"
#include "_console.h"
#include "_consoleWriteLog.h"

#include "memtool.h"
#undef safemalloc
#define safemalloc safemalloc_chkmem
#include "strtool.h"
#include "unicode.h"
#include "fat2.h"
#include "datetime.h"
#include "shell.h"
#include "glib.h"
#include "procstate.h"
#include "cfont.h"
#include "lngmsg.h"

#include "Calc.h"

// ----------------------------------------------------

extern CFont *pMathFont;

#define CanvasMarginX (4)
#define CanvasMarginY (4)
#define CanvasPadY (4)
#define CanvasTextHeight (glCanvasTextHeight+CanvasPadY)

static u32 CurX,CurY;
static CglCanvas *pCanvas;

static void CanvasInit(CglCanvas *pcan)
{
  pCanvas=pcan;
  CurX=CanvasMarginX;
  CurY=CanvasMarginY;
}

static void CanvasReturn(void)
{
  CurX=CanvasMarginX;
  CurY+=CanvasTextHeight;
}

static void CanvasDraw(const char *pstr)
{
  CglCanvas *pcan=pCanvas;
  
  bool UnderLine=false;
  bool LastSpace=true;
  
  while(1){
    char ch=*pstr++;
    if(ch==0) break;
    if((LastSpace==true)&&(ch==' ')) continue;
    
    LastSpace=false;
    if(ch==' ') LastSpace=true;
    
    bool BoxStart=false,BoxEnd=false;
    if((ch=='{')||(ch=='}')){
      if(ch=='{'){
        UnderLine=true;
        BoxStart=true;
      }
      if(ch=='}'){
        UnderLine=false;
        BoxEnd=true;
      }
      ch=0;
    }
    
    char text[16];
    Calc_GetStrFromChar(text,ch);
    
    u32 TextWidth=0;
    if(text[0]==0){
      TextWidth=2;
      }else{
      TextWidth=pMathFont->GetTextWidth(text);
    }
    if((ScreenWidth-CanvasMarginY)<(CurX+TextWidth)) CanvasReturn();
    
    u32 BoxWidth=TextWidth;
    if(BoxEnd==true) BoxWidth-=2;
    pcan->SetColor(RGB15(31,16,16)|BIT15);
    if(BoxStart==true) pcan->FillBox(CurX,CurY,1,CanvasTextHeight-CanvasPadY+1);
    if(BoxEnd==true) pcan->FillBox(CurX+BoxWidth,CurY,1,CanvasTextHeight-CanvasPadY+1);
    if((UnderLine==true)||(BoxStart==true)||(BoxEnd==true)){
      pcan->SetColor(RGB15(31,24,24)|BIT15);
      pcan->FillBox(CurX,CurY,BoxWidth+1,1);
      pcan->SetColor(RGB15(31,8,8)|BIT15);
      pcan->FillBox(CurX,CurY+CanvasTextHeight-CanvasPadY+1,BoxWidth+1,1);
    }
    
    pMathFont->DrawText(pcan,CurX,CurY,text);
    CurX+=TextWidth;
  }
}

static void CanvasDraw_Return(const char *pstr)
{
  CanvasDraw(pstr);
  CanvasReturn();
}

static void CanvasDraw_ErrorReturnLang(const char *pstr)
{
  CglCanvas *pcan=pCanvas;
  
  pcan->SetColor(RGB15(31,24,24)|BIT15);
  pcan->FillBox(0,CurY,ScreenWidth,CanvasTextHeight-CanvasPadY+1);
  
  pcan->SetFontTextColor(RGB15(0,0,0)|BIT15);
  
  if(Shell_isJPNmode()==true){
    const char *pjpn=pstr;
    while(1){
      const char ch=*pjpn++;
      if(ch==0) break;
      if(ch=='|'){
        pstr=pjpn;
        break;
      }
    }
  }
  
  pCanvas->TextOutUTF8(CurX,CurY+1,pstr);
  CanvasReturn();
}

// ----------------------------------------------------

enum EOpe {EOP_Null,EOP_Start,EOP_End,EOP_Num,EOP_Func,EOP_Add,EOP_Sub,EOP_Mul,EOP_Div};

typedef double FLOAT;

typedef struct {
  EOpe Ope;
  FLOAT Num;
  char Func[4];
} TExpItem;

typedef struct {
  TExpItem Items[256];
  u32 ItemsCount;
} TExp;

static void ExpInit(TExp *pExp)
{
  pExp->ItemsCount=0;
}

static EOpe GetOpeType(const char ch)
{
  if(ch==0) return(EOP_Null);
  
  if(ch=='(') return(EOP_Start);
  if(ch==')') return(EOP_End);
  if((('0'<=ch)&&(ch<='9'))||(ch=='.')) return(EOP_Num);
  if((('a'<=ch)&&(ch<='z'))||(('A'<=ch)&&(ch<='Z'))) return(EOP_Func);
  if(ch=='+') return(EOP_Add);
  if(ch=='-') return(EOP_Sub);
  if(ch=='*') return(EOP_Mul);
  if(ch=='/') return(EOP_Div);
  
  return(EOP_Null);
}

static bool ExpParse(TExp *pExp, const char *psrcstr)
{
  ExpInit(pExp);
  
  EOpe curope=EOP_Null;
  char curstr[256];
  curstr[0]=0;
  
  bool isLast=false;
  
  while(1){
    if(isLast==true) break;
    const char ch=*psrcstr++;
    if(ch==0) isLast=true;
    
    EOpe ope=GetOpeType(ch);
    if(curope==ope){
      char chstr[2]={ch,0};
      strcat(curstr,chstr);
      }else{
      switch(curope){
        case EOP_Null: {
        } break;
        case EOP_Start: {
          for(u32 idx=0;idx<strlen(curstr);idx++){
            TExpItem *pi=&pExp->Items[pExp->ItemsCount++];
            pi->Ope=EOP_Start;
          }
        } break;
        case EOP_End: {
          for(u32 idx=0;idx<strlen(curstr);idx++){
            TExpItem *pi=&pExp->Items[pExp->ItemsCount++];
            pi->Ope=EOP_End;
          }
        } break;
        case EOP_Num: {
          TExpItem *pi=&pExp->Items[pExp->ItemsCount++];
          pi->Ope=EOP_Num;
          pi->Num=atof(curstr);
        } break;
        case EOP_Func: {
          for(u32 idx=0;idx<strlen(curstr);idx++){
            TExpItem *pi=&pExp->Items[pExp->ItemsCount++];
            const char ch=curstr[idx];
            if(ch==FCH_PI){
              pi->Ope=EOP_Num;
              pi->Num=3.14159265;
              }else{
              pi->Ope=EOP_Func;
              pi->Func[0]=ch;
              pi->Func[1]=0;
            }
          }
        } break;
        case EOP_Add: {
          TExpItem *pi=&pExp->Items[pExp->ItemsCount++];
          pi->Ope=EOP_Add;
        } break;
        case EOP_Sub: {
          TExpItem *pi=&pExp->Items[pExp->ItemsCount++];
          pi->Ope=EOP_Sub;
        } break;
        case EOP_Mul: {
          TExpItem *pi=&pExp->Items[pExp->ItemsCount++];
          pi->Ope=EOP_Mul;
        } break;
        case EOP_Div: {
          TExpItem *pi=&pExp->Items[pExp->ItemsCount++];
          pi->Ope=EOP_Div;
        } break;
      }
      curope=ope;
      curstr[0]=ch;
      curstr[1]=0;
    }
  }
  
  return(true);
}

static void ExpShow(TExp *pExp)
{
  _consolePrintf("ExpItems= %d\n",pExp->ItemsCount);
  for(u32 idx=0;idx<pExp->ItemsCount;idx++){
    _consolePrintf("%d: ",idx);
    TExpItem *pi=&pExp->Items[idx];
    switch(pi->Ope){
      case EOP_Null: { _consolePrintf("is NULL!\n"); } break;
      case EOP_Start: { _consolePrintf("EOP_Start: (\n"); } break;
      case EOP_End: { _consolePrintf("EOP_End: )\n"); } break;
      case EOP_Num: { _consolePrintf("EOP_Num: %.2f\n",pi->Num); } break;
      case EOP_Func: { _consolePrintf("EOP_Func: %s\n",pi->Func); } break;
      case EOP_Add: { _consolePrintf("EOP_Add: +\n"); } break;
      case EOP_Sub: { _consolePrintf("EOP_Sub: -\n"); } break;
      case EOP_Mul: { _consolePrintf("EOP_Mul: *\n"); } break;
      case EOP_Div: { _consolePrintf("EOP_Div: /\n"); } break;
    }
  }
}

static void FloatToStr(char *pbuf,u32 buflen,FLOAT Num)
{
  const char *pFormat=NULL;
  switch(ProcState.Main.ShowPrec){
    case TProcState_Main::ESP_D1: pFormat="%.1f"; break;
    case TProcState_Main::ESP_D2: pFormat="%.2f"; break;
    case TProcState_Main::ESP_D3: pFormat="%.3f"; break;
  }
  snprintf(pbuf,buflen,pFormat,Num);
  
  u32 pos=(u32)-1;
  bool FoundDot=false;
  
  u32 len=strlen(pbuf);
  for(u32 idx=0;idx<len;idx++){
    char ch=pbuf[idx];
    if(ch=='.'){
      FoundDot=true;
      pos=idx-1;
      }else{
      if(FoundDot==true){
        if(ch!='0') pos=idx;
      }
    }
  }
  
  if(pos!=(u32)-1) pbuf[pos+1]=0;
}

static const char* ExpGetStr(TExp *pExp)
{
  static char res[256];
  res[0]=0;
  
  for(u32 idx=0;idx<pExp->ItemsCount;idx++){
    TExpItem *pi=&pExp->Items[idx];
    char tmp[256];
    switch(pi->Ope){
      case EOP_Null: { snprintf(tmp,256,"[NULL]"); } break;
      case EOP_Start: { snprintf(tmp,256," ("); } break;
      case EOP_End: { snprintf(tmp,256,") "); } break;
      case EOP_Num: {
        char tmp2[256];
        FloatToStr(tmp2,256,pi->Num);
        snprintf(tmp,256," %s ",tmp2);
      } break;
      case EOP_Func: { snprintf(tmp,256,"%s ",pi->Func); } break;
      case EOP_Add: { snprintf(tmp,256," + "); } break;
      case EOP_Sub: { snprintf(tmp,256," - "); } break;
      case EOP_Mul: { snprintf(tmp,256," * "); } break;
      case EOP_Div: { snprintf(tmp,256," / "); } break;
    }
    strcat(res,tmp);
  }
  
  return(res);
}

static bool ExpCheckBalance(TExp *pExp)
{
  while(0<pExp->ItemsCount){
    TExpItem *pi=&pExp->Items[pExp->ItemsCount-1];
    if(pi->Ope==EOP_Start){
      _consolePrintf("Balance: Delete last opening parenthesis.\n");
      pExp->ItemsCount--;
      }else{
      break;
    }
  }
  
  if(pExp->ItemsCount==0) return(true);
  
  int bl=0;
  
  for(u32 idx=0;idx<pExp->ItemsCount;idx++){
    TExpItem *pi=&pExp->Items[idx];
    switch(pi->Ope){
      case EOP_Start: {
        bl++;
      } break;
      case EOP_End: {
        bl--;
      } break;
      default: {
      } break;
    }
    if(bl<0){
      CanvasDraw_ErrorReturnLang(LNGMSG_Calc_BalanceError_LotOfClose);
      return(false);
    }
  }
  
  if(0<bl){
    _consolePrintf("Balance: Add %d closing parenthesis.\n",bl);
    for(int idx=0;idx<bl;idx++){
      TExpItem *pi=&pExp->Items[pExp->ItemsCount++];
      pi->Ope=EOP_End;
    }
  }
  
  return(true);
}

static u32 FindOpe(TExp *pExp,EOpe ope)
{
  for(u32 idx=0;idx<pExp->ItemsCount;idx++){
    TExpItem *pi=&pExp->Items[idx];
    if(pi->Ope==ope) return(idx);
  }
  return((u32)-1);
}

static void ExpSplit(TExp *pDstExp,const TExp *pSrcExp,u32 StartIndex,u32 EndIndex)
{
  ExpInit(pDstExp);
  
  for(u32 idx=StartIndex;idx<=EndIndex;idx++){
    TExpItem *pi=&pDstExp->Items[pDstExp->ItemsCount++];
    *pi=pSrcExp->Items[idx];
  }
}

static void ExpDelete(TExp *pExp,u32 delidx)
{
  if(pExp->ItemsCount==0){
    _consolePrintf("ERROR! %s:%d\n",__FILE__,__LINE__);
    exit(0);
  }
  
  for(u32 idx=delidx+1;idx<pExp->ItemsCount;idx++){
    pExp->Items[idx-1]=pExp->Items[idx];
  }
  pExp->ItemsCount--;
}

static void ExpAdd(TExp *pExp,u32 insidx,TExpItem *pItem)
{
  if(pExp->ItemsCount!=0){
    for(u32 idx=pExp->ItemsCount-1;idx>=insidx;idx--){
      pExp->Items[idx+1]=pExp->Items[idx];
      if(idx==0) break;
    }
  }
  pExp->ItemsCount++;
  pExp->Items[insidx]=*pItem;
}

static bool ExpCalc(TExp *pExp)
{
  if(pExp->ItemsCount==0) return(false);
  
  u32 StartIndex=(u32)-1,EndIndex=(u32)-1;
  
  if((FindOpe(pExp,EOP_Start)==(u32)-1)||(FindOpe(pExp,EOP_End)==(u32)-1)){
    StartIndex=0;
    EndIndex=pExp->ItemsCount-1;
    }else{
    for(u32 idx=0;idx<pExp->ItemsCount;idx++){
      TExpItem *pi=&pExp->Items[idx];
      switch(pi->Ope){
        case EOP_Start: {
          StartIndex=idx;
        } break;
        case EOP_End: {
          if(StartIndex!=(u32)-1) EndIndex=idx;
        } break;
        default: {
        } break;
      }
      if(EndIndex!=(u32)-1) break;
    }
  }
  
  if((StartIndex==(u32)-1)||(EndIndex==(u32)-1)){
    CanvasDraw_ErrorReturnLang(LNGMSG_Calc_DetectError);
    return(false);
  }
  
  static TExp curexp;
  if(pExp->Items[StartIndex].Ope==EOP_Start){
    ExpSplit(&curexp,pExp,StartIndex+1,EndIndex-1);
    }else{
    ExpSplit(&curexp,pExp,StartIndex,EndIndex);
  }
  
  bool errf=false;
  const char *perrmsg=NULL;
  
#define SetError(errmsg) { errf=true; perrmsg=errmsg; }

  while(1){
    if(curexp.ItemsCount<=1) break;
    u32 idx=FindOpe(&curexp,EOP_Func);
    if(idx!=(u32)-1){
      TExpItem *pi=&curexp.Items[idx];
      if(curexp.ItemsCount<=(idx+1)){ SetError(LNGMSG_Calc_Error_Function_isLast); break; }
      TExpItem *pni=&curexp.Items[idx+1];
      if(pni->Ope!=EOP_Num){ SetError(LNGMSG_Calc_Error_Function_NotNum); break; }
      bool chk=false;
      FLOAT res=0;
      if(pi->Func[0]==FCH_SIN){ chk=true; res=sin(pni->Num); }
      if(pi->Func[0]==FCH_COS){ chk=true; res=cos(pni->Num); }
      if(pi->Func[0]==FCH_TAN){ chk=true; res=tan(pni->Num); }
      if(pi->Func[0]==FCH_EXP){ chk=true; res=exp(pni->Num); }
      if(pi->Func[0]==FCH_LOG){ chk=true; res=log(pni->Num); }
      if(pi->Func[0]==FCH_PI){ SetError(LNGMSG_Calc_Error_Function_CanNotPI); break; }
      if(chk==false){ SetError(LNGMSG_Calc_Error_Function_Unknown); break; }
      ExpDelete(&curexp,idx);
      if(idx<curexp.ItemsCount) ExpDelete(&curexp,idx);
      TExpItem i;
      i.Ope=EOP_Num;
      i.Num=res;
      ExpAdd(&curexp,idx,&i);
      }else{
      u32 mulidx=FindOpe(&curexp,EOP_Mul);
      u32 dividx=FindOpe(&curexp,EOP_Div);
      if((mulidx!=(u32)-1)||(dividx!=(u32)-1)){
        u32 idx;
        if((mulidx==(u32)-1)||(dividx==(u32)-1)){
          if(mulidx!=(u32)-1){
            idx=mulidx;
            }else{
            idx=dividx;
          }
          }else{
          if(mulidx<dividx){
            idx=mulidx;
            }else{
            idx=dividx;
          }
        }
        if(idx==0){ SetError(LNGMSG_Calc_Error_Ope_First); break; }
        if((idx+1)==curexp.ItemsCount){ SetError(LNGMSG_Calc_Error_Ope_Last); break; }
        TExpItem *pbi=&curexp.Items[idx-1];
        TExpItem *pi=&curexp.Items[idx];
        TExpItem *pni=&curexp.Items[idx+1];
        if(pbi->Ope!=EOP_Num){ SetError(LNGMSG_Calc_Error_Ope_BeforeNotNum); break; }
        if(pni->Ope!=EOP_Num){ SetError(LNGMSG_Calc_Error_Ope_AfterNotNum); break; }
        FLOAT res=0;
        if(pi->Ope==EOP_Mul) res=pbi->Num*pni->Num;
        if(pi->Ope==EOP_Div){
          if(pni->Num==0){ SetError(LNGMSG_Calc_Error_Ope_DivisionByZero); break; }
          res=pbi->Num/pni->Num;
        }
        ExpDelete(&curexp,idx+1);
        ExpDelete(&curexp,idx);
        ExpDelete(&curexp,idx-1);
        TExpItem i;
        i.Ope=EOP_Num;
        i.Num=res;
        ExpAdd(&curexp,idx-1,&i);
        }else{
        u32 addidx=FindOpe(&curexp,EOP_Add);
        u32 subidx=FindOpe(&curexp,EOP_Sub);
        if((addidx!=(u32)-1)||(subidx!=(u32)-1)){
          u32 idx;
          if((addidx==(u32)-1)||(subidx==(u32)-1)){
            if(addidx!=(u32)-1){
              idx=addidx;
              }else{
              idx=subidx;
            }
            }else{
            if(addidx<subidx){
              idx=addidx;
              }else{
              idx=subidx;
            }
          }
          if(idx==0){ SetError(LNGMSG_Calc_Error_Ope_First); break; }
          if((idx+1)==curexp.ItemsCount){ SetError(LNGMSG_Calc_Error_Ope_Last); break; }
          TExpItem *pbi=&curexp.Items[idx-1];
          TExpItem *pi=&curexp.Items[idx];
          TExpItem *pni=&curexp.Items[idx+1];
          if(pbi->Ope!=EOP_Num){ SetError(LNGMSG_Calc_Error_Ope_BeforeNotNum); break; }
          if(pni->Ope!=EOP_Num){ SetError(LNGMSG_Calc_Error_Ope_AfterNotNum); break; }
          FLOAT res=0;
          if(pi->Ope==EOP_Add) res=pbi->Num+pni->Num;
          if(pi->Ope==EOP_Sub) res=pbi->Num-pni->Num;
          ExpDelete(&curexp,idx+1);
          ExpDelete(&curexp,idx);
          ExpDelete(&curexp,idx-1);
          TExpItem i;
          i.Ope=EOP_Num;
          i.Num=res;
          ExpAdd(&curexp,idx-1,&i);
          }else{
          break;
        }
      }
    }
  }
  
#undef SetError

  {
    static TExp topexp,srcexp,lastexp;
    ExpInit(&topexp);
    ExpInit(&srcexp);
    ExpInit(&lastexp);
    if(1<=StartIndex) ExpSplit(&topexp,pExp,0,StartIndex-1);
    if(pExp->Items[StartIndex].Ope==EOP_Start){
      ExpSplit(&srcexp,pExp,StartIndex+1,EndIndex-1);
      }else{
      ExpSplit(&srcexp,pExp,StartIndex,EndIndex);
    }
    if((EndIndex+1)<pExp->ItemsCount) ExpSplit(&lastexp,pExp,EndIndex+1,pExp->ItemsCount-1);
    
    char text[256]={0,};
    char tmp[256];
    if(topexp.ItemsCount!=0){
      snprintf(tmp,256,"%s ",ExpGetStr(&topexp));
      strcat(text,tmp);
    }
    snprintf(tmp,256,"{(%s) = ",ExpGetStr(&srcexp));
    strcat(text,tmp);
    snprintf(tmp,256,"%s} ",ExpGetStr(&curexp));
    strcat(text,tmp);
    if(lastexp.ItemsCount!=0){
      snprintf(tmp,256," %s",ExpGetStr(&lastexp));
      strcat(text,tmp);
    }
    CanvasDraw_Return(text);
  }
  
  for(u32 idx=StartIndex;idx<=EndIndex;idx++){
    ExpDelete(pExp,StartIndex);
  }
  
  if(errf==true){
    CanvasDraw_ErrorReturnLang(perrmsg);
    return(false);
  }
  
  ExpAdd(pExp,StartIndex,&curexp.Items[0]);
  
  return(true);
}

static void ExpOptimize(TExp *pExp)
{
#define GetOpe(idx) (pExp->Items[idx].Ope)

//enum EOpe {EOP_Null,EOP_Start,EOP_End,EOP_Num,EOP_Func,EOP_Add,EOP_Sub,EOP_Mul,EOP_Div,EOP_Err};

  while(1){
    if(pExp->ItemsCount==0) break;
    bool Process=false;
    // 先頭に演算子があったら、先頭に0を追加する。
    if(1<=pExp->ItemsCount){
      EOpe Ope=GetOpe(0);
      if((Ope==EOP_Add)||(Ope==EOP_Sub)||(Ope==EOP_Mul)||(Ope==EOP_Div)){
        Process=true;
        TExpItem ei;
        ei.Ope=EOP_Num;
        ei.Num=0;
        ExpAdd(pExp,0,&ei);
      }
    }
    // 数値の後に、閉じ括弧と演算子以外があったら、＊を追加する。
    if(2<=pExp->ItemsCount){
      for(s32 idx=0;idx<pExp->ItemsCount-1;idx++){
        if(GetOpe(idx+0)==EOP_Num){
          EOpe Ope=GetOpe(idx+1);
          if((Ope==EOP_End)||(Ope==EOP_Add)||(Ope==EOP_Sub)||(Ope==EOP_Mul)||(Ope==EOP_Div)){
            }else{
            Process=true;
            TExpItem ei;
            ei.Ope=EOP_Mul;
            ExpAdd(pExp,idx+1,&ei);
            break;
          }
        }
      }
    }
    // 括弧の中身が１つだったら、括弧を削除する。
    if(3<=pExp->ItemsCount){
      for(s32 idx=0;idx<pExp->ItemsCount-2;idx++){
        if((GetOpe(idx+0)==EOP_Start)&&(GetOpe(idx+2)==EOP_End)){
          Process=true;
          ExpDelete(pExp,idx+2);
          ExpDelete(pExp,idx+0);
          break;
        }
      }
    }
    if(Process==false) break;
  }

#undef GetOpe
}

static void ExpDrawBin(u32 Num)
{
  char str[256]={0,};
  
  strcpy(str,"Bin32.(");
  for(s32 idx=31;idx>=0;idx--){
    if((Num&(1<<idx))==0){
      strcat(str,"0");
      }else{
      strcat(str,"1");
    }
    if((idx&3)==0){
      char tmp[16];
      snprintf(tmp,16,"-%02d",idx);
      strcat(str,tmp);
      if((idx&15)!=0){
        strcat(str,", ");
        }else{
        strcat(str,")");
        CanvasDraw_Return(str);
        strcpy(str,"Bin16.(");
      }
    }
  }
  
  str[0]=0;
  for(s32 idx=31;idx>=16;idx--){
    if((Num&(1<<idx))!=0){
      char tmp[16];
      if(str[0]==0){
        snprintf(tmp,16,"%d",idx);
        }else{
        snprintf(tmp,16," %d",idx);
      }
      strcat(str,tmp);
    }
  }
  if(str[0]!=0){
    char tmp[256];
    snprintf(tmp,256,"On32: %s.",str);
    CanvasDraw_ErrorReturnLang(tmp);
  }
  
  str[0]=0;
  for(s32 idx=15;idx>=0;idx--){
    if((Num&(1<<idx))!=0){
      char tmp[16];
      if(str[0]==0){
        snprintf(tmp,16,"%d",idx);
        }else{
        snprintf(tmp,16," %d",idx);
      }
      strcat(str,tmp);
    }
  }
  if(str[0]!=0){
    char tmp[256];
    snprintf(tmp,256,"On16: %s.",str);
    CanvasDraw_ErrorReturnLang(tmp);
  }
}

bool Calc_Draw(CglCanvas *pcan)
{
  CanvasInit(pcan);
  
  char expstr[256];
  strcpy(expstr,ProcState.Main.ExpStr);
  if(expstr[0]==0) return(false);
  
  static TExp Exp;
  if(ExpParse(&Exp,expstr)==false){
    CanvasDraw_ErrorReturnLang(LNGMSG_Calc_ParseError);
    return(true);
  }
  
  if(Exp.ItemsCount==0){
    CanvasDraw_ErrorReturnLang(LNGMSG_Calc_RequestExpression);
    return(true);
  }
  
  if(ExpCheckBalance(&Exp)==false) return(true);
  
  ExpOptimize(&Exp);
  
  char tmp[256];
  snprintf(tmp,256,"%s ",ExpGetStr(&Exp));
  CanvasDraw_Return(tmp);
  
  ExpShow(&Exp);
  
  while(1){
    if(Exp.ItemsCount==0){
      CanvasDraw_ErrorReturnLang(LNGMSG_Calc_RequestExpression);
      return(true);
    }
    if(ExpCalc(&Exp)==false) return(true);
    if(Exp.ItemsCount<=1) break;
    ExpOptimize(&Exp);
  }
  
  if(ProcState.Main.ShowBin==true){
    if(Exp.ItemsCount==1){
      if(Exp.Items[0].Ope==EOP_Num){
        double Num=Exp.Items[0].Num;
        u32 Num32=Num;
        if(0<=Num){
          ExpDrawBin(Num32);
          }else{
          Num=-Num;
          Num32=Num;
          Num32=-Num32;
          ExpDrawBin(Num32);
        }
      }
    }
  }
  
  ExpShow(&Exp);
  
  return(true);
}


