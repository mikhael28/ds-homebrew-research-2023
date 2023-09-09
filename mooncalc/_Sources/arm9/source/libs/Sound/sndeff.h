#pragma once


extern void Sound_Open(void);
extern void Sound_Close(void);

// èáî‘ïœÇ¶ÇøÇ·É_ÉÅÅB
enum EWAVFN {WAVFN_License,WAVFN_StartJingle,WAVFN_BGM,
             WAVFN_BtnSE,
             WAVFN_Erase,WAVFN_AllClear,
             WAVFN_Box,
             WAVFN_CurMove,
             WAVFN_Func,
             WAVFN_Help,
             WAVFN_Numpad,WAVFN_Ope,
             WAVFN_Setting,
             WAVFN_Start,WAVFN_End,
             WAVFN_HelpBGM,
             };

extern void SoundSE_Start(EWAVFN WAVFN);
extern void SoundBGM_Start(EWAVFN WAVFN,bool isLoop);
extern u32 SoundSE_GetCurrentPlayTimePerVsync(void);
extern u32 SoundBGM_GetCurrentPlayTimePerVsync(void);
extern void SoundSE_Stop(void);
extern void SoundBGM_Stop(void);

extern void SoundSE_IRQVBlankHandler(void);
extern void SoundSE_MainVBlankHandler(void);
