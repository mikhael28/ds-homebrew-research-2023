#pragma once
#include <screen.h>
#include <bitmap.h>

class SplashScreen :
	public Screen
{
public:
	SplashScreen(const unsigned short *bm);
	~SplashScreen();
	void draw(Rect clipRect);
	void setBitmap(const unsigned short *bm);

private:
	const unsigned short *_bm;
};

