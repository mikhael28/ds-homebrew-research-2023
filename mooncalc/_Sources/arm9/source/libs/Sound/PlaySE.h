#pragma once


static void PlaySE_Sequence(u32 Count,bool AddLong)
{
  if(ProcState.System.ClickSound==false) return;
  
  while(IPC6->ClickSE.Apply==true);
  
  AddLong=false; // Disabled long sound.
  
  IPC6->ClickSE.Volume=strpcmGetAudioVolume64()*2;
  if(GlobalINI.FileList.CarSupplyMode==false) IPC6->ClickSE.Volume/=2;
  IPC6->ClickSE.Count=Count;
  IPC6->ClickSE.AddLong=AddLong;
  IPC6->ClickSE.Apply=true;
}

static void PlaySE_Short(void)
{
  if(ProcState.System.ClickSound==false) return;
  
  while(IPC6->ClickSE.Apply==true);
  
  IPC6->ClickSE.Volume=strpcmGetAudioVolume64()*2;
  if(GlobalINI.FileList.CarSupplyMode==false) IPC6->ClickSE.Volume/=2;
  IPC6->ClickSE.Count=1;
  IPC6->ClickSE.AddLong=false;
  IPC6->ClickSE.Apply=true;
}

static void PlaySE_Long(void)
{
  if(ProcState.System.ClickSound==false) return;
  
  while(IPC6->ClickSE.Apply==true);
  
  IPC6->ClickSE.Volume=strpcmGetAudioVolume64()*2;
  if(GlobalINI.FileList.CarSupplyMode==false) IPC6->ClickSE.Volume/=2;
  IPC6->ClickSE.Count=0;
  IPC6->ClickSE.AddLong=true;
  IPC6->ClickSE.Apply=true;
}

