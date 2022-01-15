
#include "Label.h"
#include "GameState.h"
#include "Game.h"
#include "Util.h"

using namespace std;

Label::Label(const string &Text, int X, int Y, int Width, int Height, ALLEGRO_COLOR Color, ALLEGRO_FONT *Font) :
	xPos(X),
	yPos(Y),
	width(Width),
	height(Height),
	color(Color),
	text(Text),
	alFont(Font)
{
	image = al_create_bitmap(Width, Height);
}

Label::~Label()
{
	if (image != NULL)
	{
		al_destroy_bitmap(image);
		image = NULL;
	}
}

void Label::Refresh()
{
    al_set_target_bitmap(image);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));

    //handle wrapping
	if (al_get_text_width(alFont, text.c_str()) > width)
	{
		int startpos = 0, a = 0, h = 0;
		std::list<int> spacePos;
		std::string::iterator stringIt;
		for (auto &i : text)
		{
			if (i == ' ')
				spacePos.push_back(a);
			a++;
		}
		spacePos.push_back(a);
		std::list<int>::iterator myIt = spacePos.begin();
		while (myIt != spacePos.end())
		{
			while (myIt != spacePos.end() && al_get_text_width(alFont, text.substr(startpos, (*myIt) - startpos).c_str()) < width)
			{
				++myIt;
			}
			if (myIt != spacePos.begin())
				--myIt;

			al_draw_text(alFont, color, 0, h, 0,(text.substr(startpos, (*myIt) - startpos)).c_str());
			h += al_get_font_line_height(alFont);
			
			if (h > height) break;

			startpos = (*myIt)+1;
			--myIt;
		}
	}
	else
	{
        //print entire message on one line
		al_draw_text(alFont, color, 0, 0, 0, text.c_str());
	}
}

void Label::Draw(ALLEGRO_BITMAP *Canvas)
{
	al_set_target_bitmap(Canvas);
	al_draw_bitmap(image, xPos, yPos, 0);
}

void Label::SetText(const string &Text)
{
	text = Text;
}

void Label::SetFont(ALLEGRO_FONT *Font)
{
	alFont = Font;
}
