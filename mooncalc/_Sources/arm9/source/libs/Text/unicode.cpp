
#include <stdio.h>

#include <NDS.h>

#include "_const.h"
#include "_console.h"
#include "memtool.h"

#include "unicode.h"

bool Unicode_isEqual(const UnicodeChar *s1,const UnicodeChar *s2)
{
  if((s1==0)&&(s2==0)) return(true);
  if((s1==0)||(s2==0)) return(false);
  
  while(*s1==*s2){
    if((*s1==0)||(*s2==0)){
      if((*s1==0)&&(*s2==0)){
        return(true);
        }else{
        return(false);
      }
    }
    s1++;
    s2++;
  }
  return(false);
}

extern bool Unicode_isEqual_NoCaseSensitive(const UnicodeChar *s1,const UnicodeChar *s2)
{
  if((s1==0)&&(s2==0)) return(true);
  if((s1==0)||(s2==0)) return(false);
  
  while(true){
    UnicodeChar uc1=*s1,uc2=*s2;
    
    if(((u32)'A'<=uc1)&&(uc1<=(u32)'Z')) uc1+=0x20;
    if(((u32)'A'<=uc2)&&(uc2<=(u32)'Z')) uc2+=0x20;
    
    if(uc1!=uc2) return(false);
    
    if((*s1==0)||(*s2==0)){
      if((*s1==0)&&(*s2==0)){
        return(true);
        }else{
        return(false);
      }
    }
    s1++;
    s2++;
  }
  return(false);
}

void Unicode_Add(UnicodeChar *s1,const UnicodeChar *s2)
{
  while(*s1!=0){
    s1++;
  }
  while(*s2!=0){
    *s1++=*s2++;
  }
  
  *s1=(UnicodeChar)0;
}

void Unicode_Copy(UnicodeChar *tag,const UnicodeChar *src)
{
  if(src!=NULL){
    while(*src!=0){
      *tag++=*src++;
    }
  }
  
  *tag=(UnicodeChar)0;
}

u32 Unicode_GetLength(const UnicodeChar *s)
{
  u32 len=0;
  
  while(*s!=0){
    len++;
    s++;
  }
  return(len);
}

UnicodeChar* Unicode_AllocateCopy(TMM *pMM,const UnicodeChar *src)
{
  u32 len=0;
  if(src!=NULL) len=Unicode_GetLength(src);
  
  UnicodeChar *ptag=(UnicodeChar*)safemalloc_chkmem(pMM,sizeof(UnicodeChar)*(len+1));
  
  for(u32 idx=0;idx<len;idx++){
    ptag[idx]=src[idx];
  }
  
  ptag[len]=(UnicodeChar)0;
  
  return(ptag);
}

UnicodeChar* Unicode_AllocateCopyFromUTF8(TMM *pMM,const char *psrcstr)
{
  u32 len=0;
  if(psrcstr!=NULL){
    while(psrcstr[len]!=0){
      u32 b0=(byte)psrcstr[len+0];
      if(b0<0x80){
        len+=1;
        }else{
        if((b0&0xe0)==0xc0){ // 0b 110. ....
          len+=2;
          }else{
          if((b0&0xf0)==0xe0){ // 0b 1110 ....
            len+=3;
            }else{
            len+=4;
          }
        }
      }
    }
  }
  
  UnicodeChar *ptag=(UnicodeChar*)safemalloc_chkmem(pMM,sizeof(UnicodeChar)*(len+1));
  
  for(u32 idx=0;idx<len;idx++){
    UnicodeChar uc;
    u32 b0=(byte)psrcstr[0],b1=(byte)psrcstr[1],b2=(byte)psrcstr[2];
    if(b0<0x80){
      uc=b0;
      psrcstr+=1;
      }else{
      if((b0&0xe0)==0xc0){ // 0b 110. ....
        uc=((b0&~0xe0)<<6)+((b1&~0xc0)<<0);
        psrcstr+=2;
        }else{
        if((b0&0xf0)==0xe0){ // 0b 1110 ....
          uc=((b0&~0xf0)<<12)+((b1&~0xc0)<<6)+((b2&~0xc0)<<0);
          psrcstr+=3;
          }else{
          uc=(u16)'?';
          psrcstr+=4;
        }
      }
    }
    ptag[idx]=uc;
  }
  
  ptag[len]=(UnicodeChar)0;
  
  return(ptag);
}

void StrConvert_Ank2Unicode(const char *srcstr,UnicodeChar *dststr)
{
  while(*srcstr!=0){
    *dststr=(UnicodeChar)*srcstr;
    dststr++;
    srcstr++;
  }
  
  *dststr=(UnicodeChar)0;
}

void StrConvert_UTF82Unicode(const char *srcstr,UnicodeChar *dststr)
{
  while(*srcstr!=0){
    u32 b0=(byte)srcstr[0],b1=(byte)srcstr[1],b2=(byte)srcstr[2];
    u32 uc;
    
    if(b0<0x80){
      uc=b0;
      srcstr++;
      }else{
      if((b0&0xe0)==0xc0){ // 0b 110. ....
        uc=((b0&~0xe0)<<6)+((b1&~0xc0)<<0);
        srcstr+=2;
        }else{
        if((b0&0xf0)==0xe0){ // 0b 1110 ....
          uc=((b0&~0xf0)<<12)+((b1&~0xc0)<<6)+((b2&~0xc0)<<0);
          srcstr+=3;
          }else{
          uc=(u16)'?';
          srcstr+=4;
        }
      }
    }
    
    *dststr=uc;
    dststr++;
  }
  
  *dststr=(UnicodeChar)0;
}

const char* StrConvert_Unicode2Ank_Test(const UnicodeChar *srcstr)
{
  static char dststr[512];
  
  if(srcstr==NULL){
    dststr[0]='N';
    dststr[1]='U';
    dststr[2]='L';
    dststr[3]='L';
    dststr[4]=0;
    return(dststr);
  }
  
  u32 idx=0;
  
  while(*srcstr!=0){
    UnicodeChar uc=*srcstr++;
    if((0x20<=uc)&&(uc<0x80)){
      dststr[idx]=uc;
      }else{
      dststr[idx]='?';
    }
    idx++;
    if(idx==255) break;
  }
  dststr[idx]=0;
  
  return(dststr);
}

