

#include <PA9.h>       
#include <stdio.h> 
#include "gfx/all_gfx.c"
#include "gfx/all_gfx.h"

char tex2[200];
char tex1[200];
char text[200];
int main(void)
{
	PA_Init();    // Initialise PA_Lib
	PA_InitVBL(); // Initialise un VBL standard
	PA_InitText(0,1);
	PA_InitText(1,1);
	PA_Init16cBg(0, 2); 	PA_Init16cBg(1, 2);
	PA_LoadTiledBg(1, 3, haut);
    PA_LoadTiledBg(0, 3, menu);
u8 timer = 0;
u8 frames = 0;
s32 x = 100;
s32 a = 0;
s32  b = 0;
u8 minute = 1;
u8 seconde = 30;
s32 u = 30;
s32 m = 1;
s32 z = 60;
s32 t = 0;
s32 y = 59;
s32 k = 0;
while (1)
	{     	
sprintf( tex1," Faces: %d ", a );
if(x < 30 ) PA_16cClearZone(1, 0, 0, 100, 192); PA_16cText(1, 1, 175, 255, 191, tex1 , 12, 4, 100);
	
if(Stylus.X > 157 && Stylus.X < 256 && Stylus.Y > 0 && Stylus.Y < 21 && Stylus.Newpress && x < 99)
{
   x = 100;
   PA_DeleteBg(0,3);
   PA_LoadTiledBg(0, 3, menu);
   }
	
//Menu
if(Pad.Held.Start)
   {
   x = 100;
   PA_DeleteBg(0,3);
   PA_LoadTiledBg(0, 3, menu);
   
   }
   if(	Stylus.Y > 128 && Stylus.Y < 160 && Stylus.X > 0 && Stylus.X < 128 && x == 100 && Stylus.Newpress ) 
   {
   PA_DeleteBg(0,3);
   PA_LoadTiledBg(0, 3, toucher2); 
   x = 14;
   a = 2;
   }
   if(	Stylus.Y > 32 && Stylus.Y < 64 && Stylus.X > 0 && Stylus.X < 128 && x == 100 && Stylus.Newpress ) 
   {
   PA_DeleteBg(0,3);
   PA_LoadTiledBg(0, 3, lancer); 
   x = 4;
   a = 4;
   }
   
if(	Stylus.Y > 32 && Stylus.Y < 64 && Stylus.X > 128 && Stylus.X < 256 && x == 100 && Stylus.Newpress ) 
   {
   PA_DeleteBg(0,3);
   PA_LoadTiledBg(0, 3, lancer); 
   x = 6;
   a = 6;
   }

if(	Stylus.Y > 64 && Stylus.Y < 96 && Stylus.X > 0 && Stylus.X < 128 && x == 100 && Stylus.Newpress ) 
   {
   PA_DeleteBg(0,3);
   PA_LoadTiledBg(0, 3, lancer); 
   x = 8;
   a = 8 ;
   }
   
   if(	Stylus.Y > 64 && Stylus.Y < 96 && Stylus.X > 128 && Stylus.X < 256 && x == 100 && Stylus.Newpress ) 
   {
   PA_DeleteBg(0,3);
   PA_LoadTiledBg(0, 3, lancer); 
   x = 10 ;
   a = 10 ;
   }
   
if(	Stylus.Y > 96 && Stylus.Y < 128 && Stylus.X > 0 && Stylus.X < 128 && x == 100 && Stylus.Newpress ) 
   {
   PA_DeleteBg(0,3);
   PA_LoadTiledBg(0, 3, lancer); 
   x = 12;
   a = 12;
   }
if(	Stylus.Y > 96 && Stylus.Y < 128 && Stylus.X > 128 && Stylus.X < 256 && x == 100 && Stylus.Newpress ) 
   {
   PA_DeleteBg(0,3);
   PA_LoadTiledBg(0, 3, lancer); 
   x = 20;
   a = 20;
   }
   
   if(	Stylus.Y > 128 && Stylus.Y < 160 && Stylus.X > 128 && Stylus.X < 256 && x == 100 && Stylus.Newpress ) 
   {
   PA_DeleteBg(0,3);
   PA_LoadTiledBg(0, 3, timer); 
   x = 50;
   a = 0;
   if(t == 1)
   {
   sprintf(tex2,  "Timer: %d m %d s", minute , seconde );
   PA_16cText(1, 120, 175, 255, 191, tex2 , 12, 4, 100);
   }
   }
//timer





sprintf(text,  "%d m %d s", m, u);
 if(x == 50) PA_16cText(0, 95, 120, 255, 191, text , 1, 4, 100);

if (x < 49) PA_16cClearZone(0, 0, 0, 256, 192);
if (x > 51) PA_16cClearZone(0, 0, 0, 256, 192);
if(x == 50)
{
if(Stylus.X > 76 && Stylus.X < 109 && Stylus.Y > 26 && Stylus.Y < 55 && Stylus.Newpress ){ t = 1; b = b + 1;}
if(Stylus.X > 102 && Stylus.X < 117 && Stylus.Y > 95 && Stylus.Y < 109 && Stylus.Newpress  ){ PA_16cClearZone(1, 100, 0, 256, 192);PA_16cClearZone(0, 0, 75, 256, 192); minute = minute + 1; m = m + 1;  }
if(Stylus.X > 141 && Stylus.X < 155 && Stylus.Y > 95 && Stylus.Y < 109 && Stylus.Newpress  ){ PA_16cClearZone(1, 100, 0, 256, 192);PA_16cClearZone(0, 0, 75, 256, 192); seconde = seconde + 1; u = u + 1; }

if(Stylus.X > 102 && Stylus.X < 117 && Stylus.Y > 140 && Stylus.Y < 153 && Stylus.Newpress  ){PA_16cClearZone(1, 100, 0, 256, 192); PA_16cClearZone(0, 0, 75, 256, 192); minute = minute - 1; m = m - 1; }
if(Stylus.X > 141 && Stylus.X < 155 && Stylus.Y > 140 && Stylus.Y < 153 && Stylus.Newpress  ) {PA_16cClearZone(1, 100, 0, 256, 192); PA_16cClearZone(0, 0, 75, 256, 192); seconde = seconde - 1;u = u - 1; }

}
if(Pad.Newpress.A && t == 1) { k = k + 1; }
if(k == 2) k = 0;
if(k == 0){ z = 60; y = 59; }
if(k == 1){ z = -1; y = -1; }

if(b == 1)
{
timer++;
	if (timer == y )
		{
		timer = 1;
		seconde --;
		PA_16cClearZone(1, 100, 0, 256, 192);
		}
		
}



sprintf(tex2,  "Timer: %d m %d s", minute , seconde );
if(b == 1 )PA_16cText(1, 120, 175, 255, 191, tex2 , 12, 4, 100);
if(b == 2) b = 0;
if(b == 0 ) {  seconde = u ; minute = m; PA_16cClearZone(1, 100, 0, 256, 192); }



if(x == 50)
{
if(b == 0 ){ PA_16cClearZone(0, 0, 0, 150, 100); PA_16cText(0, 75, 35, 255, 191, " NO " , 1, 4, 100);}
if(b == 1 ){ PA_16cClearZone(0, 0, 0, 150, 100);  PA_16cText(0, 75, 35, 255, 191, " OK " , 1, 4, 100);}
PA_16cText(0, 185, 35, 255, 191, " A " , 1, 4, 100);
}

if(seconde == 255 ) {PA_16cClearZone(1, 100, 0, 256, 192); seconde = 59 ; minute = minute - 1;}
if(minute == 0 && seconde == 0 ){ PA_16cClearZone(1, 100, 0, 256, 192); x = 150; PA_LoadTiledBg(0, 3, end); t = 0; }
if(x == 150 ) { if(Stylus.Newpress){ x = 100; PA_DeleteBg(0,3);  PA_LoadTiledBg(0, 3, menu); } PA_16cClearZone(1, 0, 0, 256, 192);}
if(minute == 0) z = - 1;

if(x > 49 && t == 0) PA_16cClearZone(1, 0, 0, 256, 192);
if(x > 49 && t == 1) PA_16cClearZone(1, 0, 0, 100, 192);
	
//test

// contition 
	if( x == 14 ) 
	{
	frames++;
	if (frames==7)
		{
		frames = 1;
		}
	}
	
	if( x == 4 )
	{
	frames++;
	if (frames==5)
		{
		frames = 1;
		}
	}
	
	if( x == 6 )
	{
	frames++;
	if (frames==7)
		{
		frames = 1;
		}
	}
	
	if( x == 8 )
	{
	frames++;
	if (frames==9)
		{
		frames = 1;
		}
	}
	
	if( x == 10)
	{
	frames++;
	if (frames==11)
		{
		frames = 1;
		}
	}
	
	if( x == 12 )
	{
	frames++;
	if (frames==13)
		{
		frames = 1;
		}
	}
	
	if( x == 20 )
	{
	frames++;
	if (frames==21)
		{
		frames = 1;
		}
	}
	
//dé

if(Stylus.Newpress && x == 4)
{
if(frames == 1 && x == 4 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, un); } 
if(frames == 2 && x == 4 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, deux); } 
if(frames == 3 && x == 4 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, trois); } 
if(frames == 4 && x == 4 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, quatre); } 

}
if(Stylus.Newpress && x == 6)
{
if(frames == 1 && x == 6 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, un); } 
if(frames == 2 && x == 6 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, deux); } 
if(frames == 3 && x == 6 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, trois); } 
if(frames == 4 && x == 6 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, quatre); } 
if(frames == 5 && x == 6 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, cinq); } 
if(frames == 6 && x == 6 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, six); } 
   }
if(Stylus.Newpress && x == 8)
{
if(frames == 1 && x == 8 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, un); } 
if(frames == 2 && x == 8 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, deux); } 
if(frames == 3 && x == 8 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, trois); } 
if(frames == 4 && x == 8 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, quatre); } 
if(frames == 5 && x == 8 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, cinq); } 
if(frames == 6 && x == 8 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, six); } 
if(frames == 7 && x == 8 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, sept); } 
if(frames == 8 && x == 8 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, huit); } 
   }
   if(Stylus.Newpress && x == 10)
{
if(frames == 1 && x == 10 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, un); } 
if(frames == 2 && x == 10) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, deux); } 
if(frames == 3 && x == 10) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, trois); } 
if(frames == 4 && x == 10) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, quatre); } 
if(frames == 5 && x == 10) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, cinq); } 
if(frames == 6 && x == 10) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, six); } 
if(frames == 7 && x == 10) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, sept); } 
if(frames == 8 && x == 10) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, huit); } 
if(frames == 9 && x == 10) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, neuf); } 
if(frames == 10 && x == 10) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, dix); } 
 
 }
if(Stylus.Newpress && x == 12)
{
if(frames == 1 && x == 12 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, un); } 
if(frames == 2 && x == 12 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, deux); } 
if(frames == 3 && x == 12 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, trois); } 
if(frames == 4 && x == 12 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, quatre); } 
if(frames == 5 && x == 12 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, cinq); } 
if(frames == 6 && x == 12 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, six); } 
if(frames == 7 && x == 12 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, sept); } 
if(frames == 8 && x == 12 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, huit); } 
if(frames == 9 && x == 12 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, neuf); } 
if(frames == 10 && x == 12 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, dix); } 
if(frames == 11 && x == 12 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, onze); } 
if(frames == 12 && x == 12 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, douze); } 
   }   
if(Stylus.Newpress && x == 20)
{
if(frames == 1 && x == 20 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, un); } 
if(frames == 2 && x == 20 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, deux); } 
if(frames == 3 && x == 20 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, trois); } 
if(frames == 4 && x == 20 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, quatre); } 
if(frames == 5 && x == 20 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, cinq); } 
if(frames == 6 && x == 20 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, six); } 
if(frames == 7 && x == 20 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, sept); } 
if(frames == 8 && x == 20 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, huit); } 
if(frames == 9 && x == 20 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, neuf); } 
if(frames == 10 && x == 20 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, dix); } 
if(frames == 11 && x == 20 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, onze); } 
if(frames == 12 && x == 20 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, douze); } 
if(frames == 13 && x == 20 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, treize); } 
if(frames == 14 && x == 20 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, quatorze); } 
if(frames == 15 && x == 20 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, quinze); } 
if(frames == 16 && x == 20 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, seize); } 
if(frames == 17 && x == 20 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, dixsept); } 
if(frames == 18 && x == 20 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, dixhuit); } 
if(frames == 19 && x == 20 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, dixneuf); } 
if(frames == 20 && x == 20 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, vingt); } 
   }
if(Stylus.Newpress && x == 14)
{
if(frames == 1 && x == 14 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, pile); } 
if(frames == 2 && x == 14 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, face); } 
if(frames == 3 && x == 14 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, pile); } 
if(frames == 4 && x == 14 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, face); } 
if(frames == 5 && x == 14 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, pile); } 
if(frames == 6 && x == 14 ) { PA_DeleteBg(1,3); PA_LoadTiledBg(1, 3, face); } 

}	
   

PA_WaitForVBL();	
}        
while (1)
	{
		PA_WaitForVBL();
	}
	
	return 0;
  } // End of main()