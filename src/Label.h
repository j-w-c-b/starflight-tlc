#pragma once

#include <allegro5/allegro.h>
#include <string>
#include <allegro5/allegro_font.h>
#include <vector>
#include <list>

class Label
{
public:
	Label(const std::string &Text, int X, int Y, int Width, int Height, ALLEGRO_COLOR Color, ALLEGRO_FONT *Font);

	~Label();

	void Refresh();
	void Draw(ALLEGRO_BITMAP *Canvas);

	//accessors
	int GetX()							const { return xPos; }
	int GetY()							const { return yPos; }
	int GetWidth()						const { return width; }
	int GetHeight()						const { return height; }
	ALLEGRO_COLOR GetColor()						const { return color; }
	std::string GetText()				const { return text; }
	ALLEGRO_FONT* GetFont()				const { return alFont; }
	
	//mutators
	void SetX(int X)					{ xPos = X; }
	void SetY(int Y)					{ yPos = Y; }
	void Append(const std::string &Text)		{ text += Text; }

	//These are in the cpp so they may be easily changed to automatically call the Refresh function when used
	void SetText(const std::string &Text);
	void SetFont(ALLEGRO_FONT *Font);


protected:
	


private: 

	int					xPos;
	int					yPos;
	int					width;
	int					height;
	ALLEGRO_COLOR					color;
	std::string			text;
	ALLEGRO_FONT			*alFont;
	ALLEGRO_BITMAP				*image;


};
