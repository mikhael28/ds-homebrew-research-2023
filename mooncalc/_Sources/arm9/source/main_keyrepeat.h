
static u32 KeyRepeatLastKey;
static bool KeyRepeatFlag;
static u32 KeyRepeatCount;
static bool KeyRepeatFirstFlag;

static void KeyRepeat_Flash(void)
{
  KeyRepeatLastKey=0;
  KeyRepeatFlag=false;
  KeyRepeatFirstFlag=false;
}

static u32 KeyRepeat_On(u32 NowKey)
{
  if(NowKey!=KeyRepeatLastKey) KeyRepeatFlag=false;
  KeyRepeatLastKey=NowKey;
  
  if(KeyRepeatFlag==false){
    KeyRepeatFlag=true;
    KeyRepeatCount=GlobalINI.KeyRepeat.DelayCount;
    KeyRepeatFirstFlag=true;
    }else{
    if(KeyRepeatCount==0){
      KeyRepeatCount=GlobalINI.KeyRepeat.RateCount;
      }else{
      NowKey=0;
    }
  }
  
  return(NowKey);
}

static u32 KeyRepeat_Proc(u32 NowKey,u32 VsyncCount)
{
  KeyRepeatFirstFlag=false;
  
  if(KeyRepeatFlag==true){
    if(KeyRepeatCount<=VsyncCount){
      KeyRepeatCount=0;
      }else{
      KeyRepeatCount-=VsyncCount;
    }
  }
  
  if(NowKey==0){
    KeyRepeat_Flash();
    }else{
    NowKey=KeyRepeat_On(NowKey);
  }
  
  return(NowKey);
}

static bool KeyRepeat_GetFirstFlag(void)
{
  return(KeyRepeatFirstFlag);
}
