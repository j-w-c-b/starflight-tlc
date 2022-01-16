
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

    al_draw_multiline_text(alFont, color, 0, 0, al_get_bitmap_width(image), 0, 0, text.c_str());
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
