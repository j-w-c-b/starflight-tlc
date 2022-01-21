#ifndef _SCROLLBOX_H
#define _SCROLLBOX_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <list>
#include <string>

namespace ScrollBox {

enum ScrollBoxType
{
    SB_TEXT,
    SB_LIST
};

struct ColoredString {
    std::string String;
    ALLEGRO_COLOR Color;
};

class ScrollBox {
  private:
    struct AREA {
        int left, top, right, bottom;
        AREA() : left(0), top(0), right(0), bottom(0) {}
        AREA(int X, int Y, int W, int H)
            : left(X), top(Y), right(W), bottom(H) {}
    };
    struct ListBoxItem {
        ALLEGRO_BITMAP *bNormal, *bHover, *bSelected;
        bool selected, hover;
        ColoredString text;
    };

    // private:

    int sbX;
    int sbY;
    int sbWidth;
    int sbHeight;
    int sbSelectedItem;
    int sbScrollBarPos;
    int sbScrollBarMin;
    int sbScrollBarMax;
    int sbLines;
    int sbTextAreaWidth;
    int sbFontHeight;
    int sbScrollStart;
    int sbScrollBarHeight;
    int sbWindowClipY;
    int sbLeftPad;
    int sbTopPad;
    int eventID;
    float sbScrollIncrement;
    ScrollBoxType sbScrollBoxType;
    bool sbDragging;
    bool sbDrawBar;
    bool sbIsOverUp;
    bool sbIsOverDown;
    bool sbIsOverBar;
    bool sbListItemSelected;
    bool sbRedraw;
    bool sbHighlight;
    ALLEGRO_BITMAP *sbNormal;
    ALLEGRO_BITMAP *sbHover;
    ALLEGRO_BITMAP *sbSelected;
    ALLEGRO_BITMAP *sbScrollBar;
    ALLEGRO_BITMAP *sbBuffer;
    AREA sbUpRect;
    AREA sbDownRect;
    AREA sbScrollRect;
    AREA sbTrackRect;
    AREA sbScrollAreaRect;
    AREA sbTextAreaRect;
    std::list<ColoredString> sbTextLines;
    std::list<ListBoxItem> sbListBoxItems;
    ALLEGRO_FONT *sbFont;
    ScrollBox *sbLinkedBox;
    ScrollBox *sbParent;

    int mouseX;
    int mouseY;

  private:
    bool isInsideOffset(int x, int y, AREA area);
    void drawUpArrow(ALLEGRO_BITMAP *buffer);
    void drawDownArrow(ALLEGRO_BITMAP *buffer);
    void drawTrack(ALLEGRO_BITMAP *buffer);
    void drawScrollBar(ALLEGRO_BITMAP *buffer);
    void setHover(int index, bool TrueOrFalse);
    int
    getLinkedWidth() {
        return (sbLinkedBox) ? sbLinkedBox->getLinkedWidth() : sbWidth;
    }
    int
    getLinkedHeight() {
        return (sbLinkedBox) ? sbLinkedBox->getLinkedHeight() : sbHeight;
    }
    int
    getLinkedX() {
        return (sbLinkedBox) ? sbLinkedBox->getLinkedX() : sbX;
    }
    int
    getLinkedY() {
        return (sbLinkedBox) ? sbLinkedBox->getLinkedY() : sbY;
    }

  public:
    ScrollBox(ALLEGRO_FONT *Font,
              ScrollBoxType initScrollBoxType = SB_TEXT,
              int X = 0,
              int Y = 0,
              int Width = 200,
              int Height = 200,
              int EventID = 66);
    ~ScrollBox();
    void Clear();
    void Draw(ALLEGRO_BITMAP *buffer);
    void OnMousePressed(int button, int x, int y);
    void OnMouseReleased(int button, int x, int y);
    void OnMouseMove(int x, int y);
    void OnMouseWheelDown(int x, int y);
    void OnMouseWheelUp(int x, int y);
    void OnMouseClick(int button, int x, int y);
    void Write(std::string text,
               ALLEGRO_COLOR color = al_map_rgb(255, 255, 255));
    void Write(ColoredString String);
    void ScrollToBottom();
    void ScrollToTop();
    void LinkBox(ScrollBox *scrollBox);
    void
    DrawScrollBar(bool TrueOrFalse) {
        sbDrawBar = TrueOrFalse;
    };
    void setLines(int lines);
    int
    getLines() {
        return sbLines;
    }
    int
    GetSelectedIndex() {
        return sbSelectedItem;
    }
    std::string GetSelectedItem();
    void SetSelectedIndex(int index);
    void SetParent(ScrollBox *parent);

    ALLEGRO_COLOR ColorControls;
    ALLEGRO_COLOR ColorBackground;
    ALLEGRO_COLOR ColorItemBorder;
    ALLEGRO_COLOR ColorHover;
    ALLEGRO_COLOR ColorSelectedBackground;
    ALLEGRO_COLOR ColorSelectedHighlight;
    ALLEGRO_COLOR ColorSelectedText;
    void
    SetColorBackground(ALLEGRO_COLOR color) {
        ColorBackground = color;
    };
    void
    SetColorControls(ALLEGRO_COLOR color) {
        ColorControls = color;
    };
    void
    SetColorHover(ALLEGRO_COLOR color) {
        ColorHover = color;
    };
    void
    SetColorSelectedBackground(ALLEGRO_COLOR color) {
        ColorSelectedBackground = color;
    };
    void
    SetColorSelectedHighlight(ALLEGRO_COLOR color) {
        ColorSelectedHighlight = color;
    };
    void
    SetColorItemBorder(ALLEGRO_COLOR color) {
        ColorItemBorder = color;
    };
    void PaintNormalImage();
    void PaintHoverImage();
    void PaintSelectedImage();

    int GetX();
    void SetX(int x);
    int GetY();
    void SetY(int y);
}; // end class ScrollBox

} // end namespace ScrollBox

#endif /* _SCROLL_BOX_H */
