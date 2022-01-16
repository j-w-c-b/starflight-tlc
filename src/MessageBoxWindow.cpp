#include "MessageBoxWindow.h"
#include "Button.h"
#include "Label.h"
#include "Game.h"
#include "Events.h"
#include "ModeMgr.h"

using namespace std;

ALLEGRO_BITMAP *MessageBoxWindow::bg = NULL;
ALLEGRO_BITMAP *MessageBoxWindow::bar = NULL;
Button *MessageBoxWindow::button1 = NULL;
Button *MessageBoxWindow::button2 = NULL;


MessageBoxWindow::MessageBoxWindow(
    const string &initheading,
	const string &initText,
	int initX, int initY, 
	int initWidth, int initHeight, 
	ALLEGRO_COLOR initTextColor, 
	bool initCentered) : 
        heading(initheading),
		text(initText),
		x(initX), 
		y(initY), 
		width(initWidth), 
		height(initHeight),
		textColor(initTextColor),
		centered(initCentered), 
		visible(true)
{
	if (bg == NULL)
		bg = al_load_bitmap("data/gui/trans_bg.tga");
	
	if (bar == NULL)
	{
		bar = al_load_bitmap("data/gui/messagebox_bar.bmp");
		al_convert_mask_to_alpha(bar, MASK_COLOR);
	}

	if (button1 == NULL)
	{
		button1 = new Button(
			"data/gui/generic_exit_btn_norm.bmp", 
			"data/gui/generic_exit_btn_over.bmp", 
			"data/gui/generic_exit_btn_over.bmp", 
			x, y,
			EVENT_MOUSEOVER,EVENT_CLOSE,
			g_game->font24,
			"Ok",
			WHITE);
	}

    int top = y - height/2;

	if (centered)
	{
		button1->SetX(x - button1->GetWidth()/2);
		button1->SetY((y + height/2) - (button1->GetHeight() + 7));
		labelText = new Label(text, (x - width/2) + 20, top + 60, width - 34, height - 20, initTextColor, g_game->font24);
	    labelHeading = new Label(heading, (x - width/2) + 20, top + 20, width - 34, height - 20, initTextColor, g_game->font24);
	}
	else
	{
		button1->SetX((x + width/2) - (button1->GetWidth()/2));
		button1->SetY((y + height) - (button1->GetHeight() + 7));
		labelText = new Label(text, x + 20, y + 30, width - 34, height - 20, initTextColor, g_game->font24);
	    labelHeading = new Label(heading, x + 20, y + 10, width - 34, height - 20, initTextColor, g_game->font24);
	}

    labelText->Refresh();
    labelHeading->Refresh();

}

MessageBoxWindow::~MessageBoxWindow()
{
	if(labelText)
		delete labelText;
    if (labelHeading)
        delete labelHeading;
}

//accessors
int MessageBoxWindow::GetX()								const { return x; }
int MessageBoxWindow::GetY()								const { return y; }
int MessageBoxWindow::GetWidth()							const { return width; }
int MessageBoxWindow::GetHeight()							const { return height; }
bool MessageBoxWindow::IsVisible()							const { return visible; }


//mutators
void MessageBoxWindow::SetText(const string &initText)		{ text = initText; labelText->SetText(text); labelText->Refresh(); }
void MessageBoxWindow::SetX(int initX)						{ x = initX; labelText->SetX(x); labelText->Refresh(); }
void MessageBoxWindow::SetY(int initY)						{ y = initY; labelText->SetY(y); labelText->Refresh(); }
void MessageBoxWindow::SetTextColor(ALLEGRO_COLOR initTextColor)		{ textColor = initTextColor; }
void MessageBoxWindow::SetVisible(bool visibility)			{ visible = visibility; }


//other funcs
bool MessageBoxWindow::OnMouseMove(int x, int y)
{
	bool result = false;

	if(button1)
		result = button1->OnMouseMove(x, y);

	if(button2 && !result)
		result = button2->OnMouseMove(x, y);

	return result;
}
bool MessageBoxWindow::OnMouseReleased(int button, int x, int y)
{
	bool result = false;

	if(button1)
		result = button1->OnMouseReleased(button, x, y);

	if(button2 && !result)
		result = button2->OnMouseReleased(button, x, y);

	return result;
}

bool MessageBoxWindow::OnMouseClick(int /*button*/, int x, int y)
{
	bool result = false;

	if(button1)
		result = button1->PtInBtn(x, y);

	if(button2 && !result)
		result = button2->PtInBtn(x, y);

	return result;
}

bool MessageBoxWindow::OnMousePressed(int /*button*/, int x, int y)
{
	bool result = false;

	if(button1)
		result = button1->PtInBtn(x, y);

	if(button2 && !result)
		result = button2->PtInBtn(x, y);

	return result;
}

bool MessageBoxWindow::OnKeyPress(int keyCode)
{
	if (keyCode == ALLEGRO_KEY_ENTER) 
	{
		Event e(EVENT_CLOSE);
		Game::modeMgr->BroadcastEvent(&e);
		return true;
	}
	return false;
}


void MessageBoxWindow::Update(){}

void MessageBoxWindow::Draw()
{

	ALLEGRO_BITMAP *backBuffer = g_game->GetBackBuffer();

	int left;
	int top;

	if(centered)
	{
		left = x - width/2;
		top = y - height/2;
	}
	else
	{
		left = x;
		top = y;
	}

	ALLEGRO_BITMAP *temp = al_create_bitmap(width, height);
        al_set_target_bitmap(temp);
	al_draw_scaled_bitmap(bg, 0, 0, al_get_bitmap_width(bg), al_get_bitmap_height(bg), 0, 0, width, height, 0);
        al_set_target_bitmap(backBuffer);
	al_draw_bitmap(temp, left, top, 0); 

	if(button1)	button1->Run(backBuffer);
	if(button2)	button2->Run(backBuffer);

        al_set_target_bitmap(backBuffer);
	al_draw_scaled_bitmap(bar, 0, 0, al_get_bitmap_width(bar), al_get_bitmap_height(bar), left, top, al_get_bitmap_width(temp), al_get_bitmap_height(bar), 0);
	al_draw_scaled_bitmap(bar, 0, 0, al_get_bitmap_width(bar), al_get_bitmap_height(bar), left, top + al_get_bitmap_height(temp) - al_get_bitmap_height(bar), al_get_bitmap_width(temp), al_get_bitmap_height(bar), 0);

	al_destroy_bitmap(temp);

    if (labelHeading)
        labelHeading->Draw(backBuffer);
	if(labelText)
		labelText->Draw(backBuffer);

}
