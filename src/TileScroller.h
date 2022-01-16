#ifndef _TILESCROLLER_H
#define _TILESCROLLER_H 1

#include <allegro5/allegro.h>
#include <stdlib.h>
#include "Point2D.h"

#define MAX_SCROLL_SIZE 2500

class TileScrollerTiles
{
    public:
        TileScrollerTiles(ALLEGRO_BITMAP *bitmap, int tile_width, int tile_height, int columns, int rows);
        void draw(int tile, int x, int y);

    private:
        ALLEGRO_BITMAP *tiles;
        int tile_width, tile_height, columns, rows;
};

class TileScroller 
{
private:
   // windowwidth x windowheight canvas that we draw into
   ALLEGRO_BITMAP *scrollbuffer;
   // set of tiles we use
   ALLEGRO_BITMAP *tiles;

   // array containing which tile number to use for the entire area being
   // scrolled
   short tiledata[MAX_SCROLL_SIZE][MAX_SCROLL_SIZE];

   // Size of the tiles located within the tiles bitmap.
   int tilewidth, tileheight;
   // The layout of the individual tiles within the tiles bitmap. The tiles
   // bitmap is a rectangle containing individual tiles of the above sizes
   // without padding. Tiles are numbered from the top left corner across
   // each row.
   int columns, rows;
   // The dimensions of the scrollable area in units based on the tile size.
   // This should be used as constraints on the tiledata access.
   int tilesacross, tilesdown;
   // The value used as the top-left origin of the scroll buffer. These are in
   // pixel coordinates, not tiles.
   float scrollx, scrolly;
   // Size of the canvas.
   int windowwidth,windowheight;

   // We allocate the scrollbuffer dynamically, so copying one of these
   // objects or assigning one would cause problems with the destructor.
   TileScroller(const TileScroller&) = delete;
   TileScroller &operator=(const TileScroller&) = delete;
   
public:
   ~TileScroller();
   TileScroller();

   bool createScrollBuffer(int width,int height);
   void resetTiles();

   void setTile(int col, int row, short value);
   short getTile(int col, int row);

   void setTileImageColumns(int c) { this->columns = c; }

   void setTileImageRows(int r) { this->rows = r; }

   void setTileSize(int w,int h) { this->tilewidth = w; this->tileheight = h; }
   int getTileWidth() { 
		return tilewidth; 
	}
	int getTileHeight() { 
		return this->tileheight; 
	}

   void setRegionSize(int w,int h);
   int getTilesAcross() { return this->tilesacross; }
   int getTilesDown() { return this->tilesdown; }


   void setTileImage(ALLEGRO_BITMAP *image);

   void setScrollPosition(float x,float y);
   void setScrollPosition(Point2D p);
   float getScrollX() { return this->scrollx; }
   float getScrollY() { return this->scrolly; }

   void updateScrollBuffer();
   void drawScrollWindow(ALLEGRO_BITMAP *dest, int x, int y, int width, int height);
};


#endif

