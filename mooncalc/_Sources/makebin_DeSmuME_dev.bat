@echo off
chcp 932
cls

del arm9\MoonCalc_arm9_Data\Release\ObjectCode\ProgramID.o

del MoonCalc.nds
ndstool.exe -c MoonCalc.nds -r7 0x03800000 -e7 0x03800000 -r9 0x02000000 -e9 0x02000000 -7 arm7.bin -9 arm9.bin -b icon.bmp "MoonCalc;Version 0.1;RVCT3.1 [Build 794]"

dir MoonCalc.nds

dldi\dlditool.exe "dldi\mpcf.dldi" "MoonCalc.nds"
rem "C:\ndsdev\emulators\desmume-0.9.7-win32\DeSmuME_dev.exe" --cflash-image=fatimage.ddi "MoonCalc.nds"
"C:\ndsdev\emulators\desmume-0.9.7-win32\DeSmuME_dev.exe" --cflash-path=fatimage "MoonCalc.nds"

pause
dscons_dstt.bat
