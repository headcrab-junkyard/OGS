/// @file

#pragma once

class Color
{
public:
	Color(){SetColor(0);}
	Color(int r, int g, int b, int a = 0){SetColor(r, g, b, a);}
	
	void SetColor(int anColor){SetColor(anColor, anColor, anColor, anColor);}
	
	void SetColor(int r, int g, int b, int a)
	{
		mData[0] = (unsigned char)r;
		mData[1] = (unsigned char)g;
		mData[2] = (unsigned char)b;
		mData[3] = (unsigned char)a;
	};
private:
	unsigned char mData[4];
};