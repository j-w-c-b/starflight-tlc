#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "Game.h"
#include "Events.h"
#include "ScrollBox.h"
#include "ModeMgr.h"

ALLEGRO_DEBUG_CHANNEL("ScrollBox")

ScrollBox::ScrollBox::ScrollBox(ALLEGRO_FONT *Font, ScrollBoxType initScrollBoxType, 
    int X, int Y,	int Width, int Height, int EventID) :
		sbX(X),
		sbY(Y),
		sbWidth(Width),
		sbHeight(Height),
		sbSelectedItem(-1),
		sbLines(20),
		sbWindowClipY(0),
		sbLeftPad(0),
		sbTopPad(0),
		eventID(EventID),
		sbScrollBoxType(initScrollBoxType),
		sbDragging(false),
		sbDrawBar(true),
		sbIsOverUp(false),
		sbIsOverDown(false),
		sbIsOverBar(false),
		sbListItemSelected(false),
		sbRedraw(true),
		sbHighlight(false),
		sbNormal(NULL),
		sbHover(NULL),
		sbSelected(NULL),
		sbScrollBar(NULL),
		sbFont(Font),
		sbLinkedBox(NULL),
		sbParent(NULL)
{
	sbUpRect = AREA(Width - 16, 0, Width-1, 15);
	sbDownRect = AREA(Width - 16, Height - 16, Width-1, Height-1);
	sbScrollRect = AREA(Width - 16, 16, Width-1, 31);
	sbTrackRect = AREA(Width - 16, 0, Width-1, Height-1);
	sbScrollAreaRect = AREA(sbUpRect.left, sbUpRect.top, sbDownRect.right, sbDownRect.bottom);
	sbTextAreaRect = AREA(0, 0, Width - 16, Height);
	sbTextAreaWidth = sbWidth - 16;
	sbScrollBarPos = sbScrollRect.top;
	sbScrollBarMin = sbScrollRect.top;
	sbScrollBarMax = Height - (sbDownRect.bottom - sbDownRect.top) - (sbScrollRect.bottom - sbScrollRect.top) - 2;
	sbScrollBarHeight = sbScrollRect.bottom - sbScrollRect.top;
	sbScrollStart = sbScrollBarPos;
	sbFontHeight = al_get_font_line_height(sbFont);
	if (sbLines * sbFontHeight < sbHeight)
		sbBuffer = al_create_bitmap(sbWidth, sbHeight);
	else
		sbBuffer = al_create_bitmap(sbWidth, sbLines * sbFontHeight);

    ColorItemBorder = al_map_rgb(40, 40, 40);
    ColorControls = al_map_rgb(64, 64, 64);
	ColorBackground = al_map_rgb(32, 32, 32);
	ColorHover = al_map_rgb(128, 128, 128);
	ColorSelectedHighlight = al_map_rgb(64, 64, 128);
	ColorSelectedBackground = al_map_rgb(32, 32, 64);
	ColorSelectedText = al_map_rgb(128, 128, 128);

	sbScrollIncrement = (float)((sbLines * sbFontHeight)-sbHeight) / (sbScrollBarMax - sbScrollBarMin);

	if (sbScrollBoxType == SB_LIST)
	{
		sbNormal = al_create_bitmap(sbWidth - 16, sbFontHeight);
		sbHover = al_create_bitmap(sbWidth - 16, sbFontHeight);
		sbSelected = al_create_bitmap(sbWidth - 16, sbFontHeight);
        PaintNormalImage();
        PaintHoverImage();
        PaintSelectedImage();
		ListBoxItem tempLBI;
		tempLBI.bHover = sbHover;
		tempLBI.bNormal = sbNormal;
		tempLBI.bSelected = sbSelected;
		tempLBI.selected = false;
		tempLBI.hover = false;
		tempLBI.text.String = "";
		tempLBI.text.Color = al_map_rgb(0,0,0);
		for (int a = 0; a < sbLines; a++)
		{
			sbListBoxItems.push_back(tempLBI);
		}
	}
	else if (sbScrollBoxType == SB_TEXT)
	{
		ColoredString temp;
		temp.Color = al_map_rgb(0, 0, 0);
		temp.String = "";
		for (int a = 0; a < sbLines; a++)
		{
			sbTextLines.push_front(temp);
		}
	}
}

void ScrollBox::ScrollBox::PaintNormalImage()
{
	al_set_target_bitmap(sbNormal);
	al_clear_to_color(BLACK);

	al_draw_rectangle(0, 0, al_get_bitmap_width(sbNormal)-1, al_get_bitmap_height(sbNormal)-1, ColorItemBorder, 1);
	al_draw_filled_rectangle( 1, 1, al_get_bitmap_width(sbNormal) - 2, al_get_bitmap_height(sbNormal) - 2, ColorBackground);
}

void ScrollBox::ScrollBox::PaintHoverImage()
{
	al_set_target_bitmap(sbHover);
	al_clear_to_color(BLACK);

	al_draw_rectangle(0, 0, al_get_bitmap_width(sbHover)-1, al_get_bitmap_height(sbHover)-1, ColorItemBorder, 1);
	al_draw_filled_rectangle( 1, 1, al_get_bitmap_width(sbHover) - 2, al_get_bitmap_height(sbHover) - 2, ColorControls);
}

void ScrollBox::ScrollBox::PaintSelectedImage()
{
	al_set_target_bitmap(sbSelected);
	al_clear_to_color(BLACK);

	al_draw_rectangle(0, 0, al_get_bitmap_width(sbSelected)-1, al_get_bitmap_height(sbSelected)-1, ColorSelectedHighlight, 1);
	al_draw_filled_rectangle( 1, 1, al_get_bitmap_width(sbSelected) - 2, al_get_bitmap_height(sbSelected) - 2, ColorSelectedBackground);
}

ScrollBox::ScrollBox::~ScrollBox()
{
	sbTextLines.clear();
	sbListBoxItems.clear();

	if (sbLinkedBox != NULL)
	{
		delete sbLinkedBox;
		sbLinkedBox = NULL;
	}
	if (sbNormal != NULL)
	{
		al_destroy_bitmap(sbNormal);
		sbNormal = NULL;
	}
	if (sbHover != NULL)
	{
		al_destroy_bitmap(sbHover);
		sbHover = NULL;
	}
	if (sbSelected != NULL)
	{
		al_destroy_bitmap(sbSelected);
		sbSelected = NULL;
	}
	if (sbScrollBar != 0)
	{
		al_destroy_bitmap(sbScrollBar);
		sbScrollBar = NULL;
	}
	if (sbBuffer != 0)
	{
		al_destroy_bitmap(sbBuffer);
		sbBuffer = NULL;
	}
}


std::string ScrollBox::ScrollBox::GetSelectedItem()
{
    std::string text="";
	for (auto &i : sbListBoxItems)
	{
		if (i.selected)
		{
            text = i.text.String;
            break;
        }
    }
    return text;
}

void ScrollBox::ScrollBox::Draw(ALLEGRO_BITMAP *buffer)
{
    al_set_target_bitmap(buffer);
	if (sbLinkedBox != NULL)
		sbLinkedBox->Draw(buffer);
	//Draw strings
	if (sbRedraw)
	{
		//Clear buffer
                al_set_target_bitmap(sbBuffer);
		al_clear_to_color(BLACK);
		int a = 0;
		if (sbScrollBoxType == SB_TEXT)
		{
			for(auto &i : sbTextLines)
			{
                            unsigned char r, g, b;
                            al_unmap_rgb(i.Color, &r, &g, &b);
				al_draw_text(sbFont, i.Color, sbLeftPad, sbFontHeight * a + sbTopPad, 
                    0,i.String.c_str());
				a++;
			}
		}
		else if (sbScrollBoxType == SB_LIST)
		{
            ALLEGRO_COLOR color;
			for( auto &i : sbListBoxItems)
			{
				if (i.selected)
                {
					al_draw_bitmap_region(i.bSelected, 0, 0, sbWidth, sbHeight, 0, sbFontHeight * a, 0);
                    color = ColorSelectedText;
                }
				else if (i.hover)
                {
					al_draw_bitmap_region(i.bHover, 0, 0, sbWidth, sbHeight, 0, sbFontHeight * a, 0);
                    color = i.text.Color;
                }
				else
                {
					al_draw_bitmap_region(i.bNormal, 0, 0, sbWidth, sbHeight, 0, sbFontHeight * a, 0);
                    color = i.text.Color;
                }
                            unsigned char r, g, b;
                            al_unmap_rgb(color, &r, &g, &b);

				al_draw_text(sbFont, color, sbLeftPad, sbFontHeight * a + sbTopPad, 
                    0,i.text.String.c_str());
				a++;
			}
		}
		sbRedraw = false;
	}
	//Draw buffer to screen
        al_set_target_bitmap(buffer);
	al_draw_bitmap_region(sbBuffer, 0, sbWindowClipY, sbWidth - 16, sbHeight, sbX, sbY, 0);
	//Draw buttons to buffer
	if (sbDrawBar)
	{
		drawTrack(buffer);
		drawUpArrow(buffer);
		drawDownArrow(buffer);
		drawScrollBar(buffer);
	}
}

void ScrollBox::ScrollBox::OnMouseClick(int button, int x, int y)
{
	if (sbLinkedBox != NULL)
		sbLinkedBox->OnMouseClick(button, x, y);
	if (isInsideOffset(x, y, sbUpRect) && sbScrollBarPos > sbScrollBarMin)
	{
		sbScrollBarPos--;
		sbScrollRect.top = sbScrollBarPos;
		sbScrollRect.bottom = sbScrollRect.top + sbScrollBarHeight;
		sbWindowClipY = sbScrollIncrement * ((100 * (sbScrollBarPos-sbUpRect.bottom)) / (sbScrollBarMax-sbUpRect.bottom));
		sbRedraw = true;
	}
	else if (isInsideOffset(x, y, sbDownRect) && sbScrollBarPos < sbScrollBarMax)
	{
		sbScrollBarPos++;
		sbScrollRect.top = sbScrollBarPos;
		sbScrollRect.bottom = sbScrollRect.top + sbScrollBarHeight;
		sbWindowClipY = sbScrollIncrement * ((100 * (sbScrollBarPos-sbUpRect.bottom)) / (sbScrollBarMax-sbUpRect.bottom));
		sbRedraw = true;
	}
	else if (sbScrollBoxType == SB_LIST)
	{
		int clickedIndex = (sbWindowClipY + y - sbY)/sbFontHeight;
		bool insideScrollBox = false;
		if (isInsideOffset(x, y, sbTextAreaRect) && !isInsideOffset(x, y, sbScrollAreaRect))
		{
			insideScrollBox = true;
			sbRedraw = true;
		}
		int a = 0;

		/*
		 * Converted to a while because items can be removed from the list in mid-loop by other processes
		 * causing an assert crash.
		 */
		std::list<ListBoxItem>::iterator myIt = sbListBoxItems.begin();
		while ( myIt != sbListBoxItems.end() )
		{
			if (insideScrollBox && clickedIndex == a)
			{
				if((*myIt).selected)//if the one we clicked on is already selected...
				{
					//...deselect it
					(*myIt).selected = false;
					sbListItemSelected = false;
					sbSelectedItem = -1;
					Event e(eventID);
					Game::modeMgr->BroadcastEvent(&e);
					break;
				}
				else//if the one we clicked on isn't selected
				{
					(*myIt).selected = true;
					if (sbListItemSelected)
					{
						int holder = a;
						if (sbSelectedItem > a)
						{
							while (sbSelectedItem > a)
							{
								++myIt;
								++a;
							}
							(*myIt).selected = false;
						}
						else
						{
							while (sbSelectedItem < a)
							{
								--myIt;
								--a;
							}
							(*myIt).selected = false;
						}
						a = holder;
					}
					sbSelectedItem = a;
					sbListItemSelected = true;
					Event e(eventID);
					Game::modeMgr->BroadcastEvent(&e);
					break;
				}
			}

			//a is the selected item
			a++;

			//go to next item in list
			myIt++;
		}
	}
}

void ScrollBox::ScrollBox::OnMouseMove(int x, int y)
{
	mouseX = x;
	mouseY = y;
	if (sbLinkedBox != NULL)
		sbLinkedBox->OnMouseMove(x, y);
	if (isInsideOffset(x, y, sbUpRect))
	{
		if (!sbIsOverUp)
		{
			sbIsOverUp = true;
			sbRedraw = true;
		}
	}
	else
	{
		if (sbIsOverUp)
		{
			sbIsOverUp = false;
			sbRedraw = true;
		}
	}
	if (isInsideOffset(x, y, sbDownRect))
	{
		if (!sbIsOverDown)
		{
			sbIsOverDown = true;
			sbRedraw = true;
		}
	}
	else
	{
		if (sbIsOverDown)
		{
			sbIsOverDown = false;
			sbRedraw = true;
		}
	}
	if (isInsideOffset(x, y, sbScrollRect))
	{
		if (!sbIsOverBar)
		{
			sbIsOverBar = true;
			sbRedraw = true;
		}
	}
	else
	{
		if (sbIsOverBar)
		{
			sbIsOverBar = false;
			sbRedraw = true;
		}
	}

	if (sbScrollBoxType == SB_LIST)
	{
		int temp = (sbWindowClipY + y - sbY)/sbFontHeight;
		bool temp2 = false;
		if (isInsideOffset(x, y, sbTextAreaRect) && !isInsideOffset(x, y, sbScrollAreaRect))
		{
			temp2 = true;
			sbRedraw = true;
			sbHighlight = true;
		}
		int a = 0;
		for(std::list<ListBoxItem>::iterator myIt = sbListBoxItems.begin(); myIt != sbListBoxItems.end(); myIt++)
		{
			if (temp2 && temp == a)
			{
				(*myIt).hover = true;
			}
			else
			{
				(*myIt).hover = false;
				if (sbParent)
					sbParent->setHover(a, false);
				if (sbLinkedBox)
					sbLinkedBox->setHover(a, false);
			}
			a++;
		}
		if (sbHighlight && !temp2)
		{
			sbHighlight = false;
			sbRedraw = true;
		}
	}

	if (sbDragging)
	{
		sbScrollBarPos -= sbScrollStart - y;
		sbScrollStart = y;
		if (sbScrollBarPos < sbScrollBarMin)
			sbScrollBarPos = sbScrollBarMin;
		else if (sbScrollBarPos > sbScrollBarMax)
			sbScrollBarPos = sbScrollBarMax;
		sbScrollRect.top = sbScrollBarPos;
		sbScrollRect.bottom = sbScrollRect.top + sbScrollBarHeight;
		sbWindowClipY = sbScrollIncrement * (sbScrollBarPos-sbScrollBarMin);
		sbRedraw = true;
	}
}

void ScrollBox::ScrollBox::OnMousePressed(int button, int x, int y)
{
	if (sbLinkedBox != NULL)
		sbLinkedBox->OnMousePressed(button, x, y);

	//clicked scroll button?
	if (isInsideOffset(x, y, sbScrollRect))
	{
		sbScrollStart = y;
		sbDragging = true;
		sbRedraw = true;
		if (sbLinkedBox)
		{
			sbLinkedBox->sbScrollStart = y;
			sbLinkedBox->sbDragging = true;
			sbLinkedBox->sbRedraw = true;
		}
	}

}

void ScrollBox::ScrollBox::OnMouseReleased(int button, int x, int y)
{
	if (sbLinkedBox != NULL)
		sbLinkedBox->OnMouseReleased(button, x, y);
	sbDragging = false;
}

void ScrollBox::ScrollBox::OnMouseWheelDown(int x, int y)
{
	if (sbLinkedBox != NULL)
		sbLinkedBox->OnMouseWheelDown(x, y);

	//clicked scroll button?
	if (isInsideOffset(x, y, sbTextAreaRect))
	{
		sbScrollBarPos += 10;

		if (sbScrollBarPos < sbScrollBarMin) sbScrollBarPos = sbScrollBarMin;
		if (sbScrollBarPos > sbScrollBarMax) sbScrollBarPos = sbScrollBarMax;

		sbWindowClipY = sbScrollIncrement * (sbScrollBarPos-sbScrollBarMin);

		sbScrollRect.top = sbScrollBarPos;
		sbScrollRect.bottom = sbScrollRect.top + sbScrollBarHeight;

		if (sbLinkedBox)
		{
			sbLinkedBox->sbScrollBarPos = sbScrollBarPos;
			sbLinkedBox->sbWindowClipY = sbWindowClipY;
		}

		sbRedraw = true;
	}
}

void ScrollBox::ScrollBox::OnMouseWheelUp(int x, int y)
{
	if (sbLinkedBox != NULL)
		sbLinkedBox->OnMouseWheelUp(x, y);

	//clicked scroll button?
	if (isInsideOffset(x, y, sbTextAreaRect))
	{
		sbScrollBarPos -= 10;

		if (sbScrollBarPos < sbScrollBarMin) sbScrollBarPos = sbScrollBarMin;
		if (sbScrollBarPos > sbScrollBarMax) sbScrollBarPos = sbScrollBarMax;

		sbWindowClipY = sbScrollIncrement * (sbScrollBarPos-sbScrollBarMin);

		sbScrollRect.top = sbScrollBarPos;
		sbScrollRect.bottom = sbScrollRect.top + sbScrollBarHeight;

		if (sbLinkedBox)
		{
			sbLinkedBox->sbScrollBarPos = sbScrollBarPos;
			sbLinkedBox->sbWindowClipY = sbWindowClipY;
		}

		sbRedraw = true;
	}
}

bool ScrollBox::ScrollBox::isInsideOffset(int x, int y, AREA area)
{
	if (sbLinkedBox)
	{
		x -= sbLinkedBox->getLinkedX();
		y -= sbLinkedBox->getLinkedY();
	}
	else
	{
		x -= sbX;
		y -= sbY;
	}
	if (x >= area.left && x <= area.right && y >= area.top && y <= area.bottom)
		return true;
	else
		return false;
}

void ScrollBox::ScrollBox::Write(std::string text, ALLEGRO_COLOR color)
{
	ColoredString output = {text, color};

	ScrollBox::Write(output);
}

void ScrollBox::ScrollBox::Write(ColoredString text)
{
	if (sbScrollBoxType == SB_TEXT)
	{
        //wrap to the next line if text won't fit
		if (al_get_text_width(sbFont, text.String.c_str()) > sbTextAreaWidth)
		{
			int startpos = 0, a = 0;
			std::list<int> spacePos;
			std::string::iterator stringIt;
			for (stringIt = text.String.begin(); stringIt != text.String.end(); stringIt++)
			{
				if ((*stringIt) == ' ')
					spacePos.push_back(a);
				a++;
			}
			spacePos.push_back(a);
			std::list<int>::iterator myIt = spacePos.begin();
			while (myIt != spacePos.end())
			{
				while (myIt != spacePos.end() && al_get_text_width(sbFont, text.String.substr(startpos, (*myIt) - startpos).c_str()) < sbTextAreaWidth)
				{
					myIt++;
				}
				if (myIt != spacePos.begin())
					myIt--;
				ColoredString tempCS;
				tempCS.Color = text.Color;
				tempCS.String = text.String.substr(startpos, (*myIt) - startpos);
				sbTextLines.push_back(tempCS);
				startpos = (*myIt)+1;
				myIt++;
			}
            		
		}
		else
			sbTextLines.push_back(text);
		while(sbTextLines.size() > (unsigned int)sbLines)
			sbTextLines.pop_front();
	}
	else if (sbScrollBoxType == SB_LIST)
	{
		std::list<ListBoxItem>::iterator myIt = sbListBoxItems.begin();
		while (myIt != sbListBoxItems.end())
		{
			if ((*myIt).text.String == "")
			{
				(*myIt).text = text;
				break;
			}
			myIt++;
		}
	}

	sbRedraw = true;
}

void ScrollBox::ScrollBox::drawDownArrow(ALLEGRO_BITMAP *buffer)
{
	ALLEGRO_COLOR highlightColor;
	if (sbIsOverDown && !sbDragging)
		highlightColor = ColorHover;
	else
		highlightColor = ColorControls;

	al_set_target_bitmap(buffer);
	al_draw_rectangle(getLinkedX() + sbDownRect.left, getLinkedY() + sbDownRect.top, getLinkedX() + sbDownRect.right, getLinkedY() + sbDownRect.bottom, highlightColor, 1);
	al_draw_filled_rectangle( getLinkedX() + sbDownRect.left+1, getLinkedY() + sbDownRect.top+1, getLinkedX() + sbDownRect.right-1, getLinkedY() + sbDownRect.bottom-1, ColorBackground);
	al_draw_line(getLinkedX() + sbDownRect.left + (sbDownRect.right-sbDownRect.left)/2,
		getLinkedY() + sbDownRect.bottom - (sbDownRect.bottom - sbDownRect.top)/4, getLinkedX() + sbDownRect.left + (sbDownRect.right - sbDownRect.left)/4,
		getLinkedY() + sbDownRect.top + (sbDownRect.bottom - sbDownRect.top)/4, highlightColor, 1);
	al_draw_line(getLinkedX() + sbDownRect.left + (sbDownRect.right - sbDownRect.left)/4,
		getLinkedY() + sbDownRect.top + (sbDownRect.bottom - sbDownRect.top)/4, getLinkedX() + sbDownRect.right - (sbDownRect.right - sbDownRect.left)/4,
		getLinkedY() + sbDownRect.top + (sbDownRect.bottom - sbDownRect.top)/4, highlightColor, 1);
	al_draw_line(getLinkedX() + sbDownRect.right - (sbDownRect.right - sbDownRect.left)/4,
		getLinkedY() + sbDownRect.top + (sbDownRect.bottom - sbDownRect.top)/4, getLinkedX() + sbDownRect.left + (sbDownRect.right-sbDownRect.left)/2,
		getLinkedY() + sbDownRect.bottom - (sbDownRect.bottom - sbDownRect.top)/4, highlightColor, 1);
}

void ScrollBox::ScrollBox::drawScrollBar(ALLEGRO_BITMAP *buffer)
{
	ALLEGRO_COLOR highlightColor;
	if (sbIsOverBar || sbDragging)
		highlightColor = ColorHover;
	else
		highlightColor = ColorControls;

        int half = (sbScrollRect.right - sbScrollRect.left)/2;
	int third = (sbScrollRect.right - sbScrollRect.left)/3;
	al_set_target_bitmap(buffer);
	al_draw_rectangle(getLinkedX() + sbScrollRect.left, getLinkedY() + sbScrollRect.top, 
        getLinkedX() + sbScrollRect.right, getLinkedY() + sbScrollRect.bottom, highlightColor, 1);
	al_draw_filled_rectangle(getLinkedX() + sbScrollRect.left + 1, getLinkedY() + sbScrollRect.top + 1,
		getLinkedX() + sbScrollRect.right - 1, getLinkedY() + sbScrollRect.bottom - 1, ColorBackground);
	al_draw_line(getLinkedX() + sbScrollRect.left + third, getLinkedY() + sbScrollRect.top + half - 2,
		getLinkedX() + sbScrollRect.right - third, getLinkedY() + sbScrollRect.top + half - 2, highlightColor, 1);
	al_draw_line(getLinkedX() + sbScrollRect.left + third, getLinkedY() + sbScrollRect.top + half,
		getLinkedX() + sbScrollRect.right - third, getLinkedY() + sbScrollRect.top + half , highlightColor, 1);
	al_draw_line(getLinkedX() + sbScrollRect.left + third, getLinkedY() + sbScrollRect.top + half + 2,
		getLinkedX() + sbScrollRect.right - third, getLinkedY() + sbScrollRect.top + half + 2, highlightColor, 1);
}

void ScrollBox::ScrollBox::drawTrack(ALLEGRO_BITMAP *buffer)
{
	al_set_target_bitmap(buffer);
	al_draw_rectangle(getLinkedX() + sbTrackRect.left, getLinkedY() + sbTrackRect.top, getLinkedX() + sbTrackRect.right, getLinkedY() + sbTrackRect.bottom, ColorControls, 1);
	al_draw_filled_rectangle( getLinkedX() + sbTrackRect.left + 1, getLinkedY() + sbTrackRect.top + 1, getLinkedX() + sbTrackRect.right - 1, getLinkedY() + sbTrackRect.bottom - 1, ColorBackground);
}

void ScrollBox::ScrollBox::drawUpArrow(ALLEGRO_BITMAP *buffer)
{
	ALLEGRO_COLOR highlightColor;
	if (sbIsOverUp && !sbDragging)
		highlightColor = ColorHover;
	else
		highlightColor = ColorControls;

	al_set_target_bitmap(buffer);
	al_draw_rectangle(getLinkedX() + sbUpRect.left, getLinkedY() + sbUpRect.top, getLinkedX() + sbUpRect.right, getLinkedY() + sbUpRect.bottom, highlightColor, 1);
	al_draw_filled_rectangle( getLinkedX() + sbUpRect.left+1, getLinkedY() + sbUpRect.top+1, getLinkedX() + sbUpRect.right-1, getLinkedY() + sbUpRect.bottom-1, ColorBackground);
	al_draw_line(getLinkedX() + sbUpRect.left + (sbUpRect.right-sbUpRect.left)/2,
		getLinkedY() + sbUpRect.top + (sbUpRect.bottom - sbUpRect.top)/4, getLinkedX() + sbUpRect.left + (sbUpRect.right - sbUpRect.left)/4,
		getLinkedY() + sbUpRect.bottom - (sbUpRect.bottom - sbUpRect.top)/4, highlightColor, 1);
	al_draw_line(getLinkedX() + sbUpRect.left + (sbUpRect.right - sbUpRect.left)/4,
		getLinkedY() + sbUpRect.bottom - (sbUpRect.bottom - sbUpRect.top)/4, getLinkedX() + sbUpRect.right - (sbUpRect.right - sbUpRect.left)/4,
		getLinkedY() + sbUpRect.bottom - (sbUpRect.bottom - sbUpRect.top)/4, highlightColor, 1);
	al_draw_line(getLinkedX() + sbUpRect.right - (sbUpRect.right - sbUpRect.left)/4,
		getLinkedY() + sbUpRect.bottom - (sbUpRect.bottom - sbUpRect.top)/4, getLinkedX() + sbUpRect.left + (sbUpRect.right-sbUpRect.left)/2,
		getLinkedY() + sbUpRect.top + (sbUpRect.bottom - sbUpRect.top)/4, highlightColor, 1);
}

void ScrollBox::ScrollBox::ScrollToBottom()
{
	if (sbLinkedBox != NULL)
		sbLinkedBox->ScrollToBottom();
	sbScrollBarPos = sbScrollBarMax;
	sbScrollRect.top = sbScrollBarPos;
	sbScrollRect.bottom = sbScrollRect.top + sbScrollBarHeight;
	sbWindowClipY = sbScrollIncrement * (sbScrollBarPos-sbScrollBarMin);
}

void ScrollBox::ScrollBox::ScrollToTop()
{
	if (sbLinkedBox != NULL)
		sbLinkedBox->ScrollToTop();
	sbScrollBarPos = sbScrollBarMin;
	sbScrollRect.top = sbScrollBarPos;
	sbScrollRect.bottom = sbScrollRect.top + sbScrollBarHeight;
	sbWindowClipY = sbScrollIncrement * (sbScrollBarPos-sbScrollBarMin);
}

void ScrollBox::ScrollBox::LinkBox(ScrollBox *scrollBox)
{
	if (sbLinkedBox == NULL)
	{
		sbLinkedBox = scrollBox;
		scrollBox->DrawScrollBar(false);
		scrollBox->SetParent(this);
		sbTextAreaRect = AREA(0, 0, scrollBox->getLinkedWidth() - 16, scrollBox->getLinkedHeight());
		sbUpRect = AREA(scrollBox->getLinkedWidth() - 16, 0, scrollBox->getLinkedWidth()-1, 15);
		sbDownRect = AREA(scrollBox->getLinkedWidth() - 16, scrollBox->getLinkedHeight() - 16, scrollBox->getLinkedWidth()-1, scrollBox->getLinkedHeight()-1);
		sbScrollRect = AREA(scrollBox->getLinkedWidth() - 16, 16, scrollBox->getLinkedWidth()-1, 31);
		sbTrackRect = AREA(scrollBox->getLinkedWidth() - 16, 0, scrollBox->getLinkedWidth()-1, scrollBox->getLinkedHeight()-1);
		sbScrollAreaRect = AREA(sbUpRect.left, sbUpRect.top, sbDownRect.right, sbDownRect.bottom);
	}
	else
		sbLinkedBox->LinkBox(scrollBox);

}

void ScrollBox::ScrollBox::setLines(int lines)
{
   if (sbLinkedBox != NULL)
      sbLinkedBox->setLines(lines);

	sbLines = lines;
	al_destroy_bitmap(sbBuffer);
	if (sbLines * sbFontHeight < sbHeight)
		sbBuffer = al_create_bitmap(sbWidth, sbHeight);
	else
		sbBuffer = al_create_bitmap(sbWidth, sbLines * sbFontHeight);
	sbScrollIncrement = (float)((sbLines * sbFontHeight)-sbHeight) / (sbScrollBarMax - sbScrollBarMin);
	if (sbScrollBoxType == SB_LIST)
	{
		sbListBoxItems.clear();
		ListBoxItem tempLBI;
		tempLBI.bHover = sbHover;
		tempLBI.bNormal = sbNormal;
		tempLBI.bSelected = sbSelected;
		tempLBI.selected = false;
		tempLBI.hover = false;
		tempLBI.text.String = "";
		tempLBI.text.Color = al_map_rgb(0,0,0);
		for (int a = 0; a < sbLines; a++)
		{
			sbListBoxItems.push_back(tempLBI);
		}
	}
	else if (sbScrollBoxType == SB_TEXT)
	{
		sbTextLines.clear();
		ColoredString temp;
		temp.Color = al_map_rgb(0, 0, 0);
		temp.String = "";
		for (int a = 0; a < sbLines; a++)
		{
			sbTextLines.push_front(temp);
		}
	}
	else
		sbTextLines.clear();
   sbRedraw = true;
}

void ScrollBox::ScrollBox::Clear()
{
   if (sbLinkedBox != NULL)
      sbLinkedBox->Clear();

       if (sbScrollBoxType == SB_LIST)
       {
               sbListBoxItems.clear();
               ListBoxItem tempLBI;
               tempLBI.bHover = sbHover;
               tempLBI.bNormal = sbNormal;
               tempLBI.bSelected = sbSelected;
               tempLBI.selected = false;
               tempLBI.hover = false;
               tempLBI.text.String = "";
               tempLBI.text.Color = al_map_rgb(0,0,0);
               for (int a = 0; a < sbLines; a++)
               {
                       sbListBoxItems.push_back(tempLBI);
               }
			   sbSelectedItem = -1;
			   sbListItemSelected = false;
       }
       else if (sbScrollBoxType == SB_TEXT)
       {
               sbTextLines.clear();
               ColoredString temp;
               temp.Color = al_map_rgb(0, 0, 0);
               temp.String = "";
               for (int a = 0; a < sbLines; a++)
               {
                       sbTextLines.push_front(temp);
               }
       }
	   else
	   {
		   sbTextLines.clear();
	   }

       sbRedraw = true;
}

int ScrollBox::ScrollBox::GetX()
{
	return sbX;
}

void ScrollBox::ScrollBox::SetX(int x)
{
	sbX = x;
}

int ScrollBox::ScrollBox::GetY()
{
	return sbY;
}

void ScrollBox::ScrollBox::SetY(int y)
{
	sbY = y;
}

void ScrollBox::ScrollBox::SetSelectedIndex(int index)
{
   if (sbLinkedBox != NULL)
      sbLinkedBox->SetSelectedIndex(index);

   sbSelectedItem = index;

   if (sbSelectedItem < -1)
      sbSelectedItem = -1;

   if (sbSelectedItem >= sbLines)
      sbSelectedItem = -1;

   if (index < 0)
   {
      // clear any selected items
		for (auto &i : sbListBoxItems)
		{
			if(i.selected)
			{
				i.selected = false;
				sbListItemSelected = false;
            sbRedraw = true;
         }
      }
   }
}

void ScrollBox::ScrollBox::SetParent(ScrollBox *parent)
{
	sbParent = parent;
}

void ScrollBox::ScrollBox::setHover(int index, bool TrueOrFalse)
{
	int a = 0;
	for (auto &i : sbListBoxItems)
	{
		if (index == a)
		{
			i.hover = TrueOrFalse;
			if (sbParent)
				sbParent->setHover(index, TrueOrFalse);
			return;
		}
		a++;
	}
}
