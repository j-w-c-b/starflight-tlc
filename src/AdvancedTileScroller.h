#ifndef ADVANCEDTILESCROLLER_H
#define ADVANCEDTILESCROLLER_H

#include <map>
#include <memory>
#include <vector>

#include <allegro5/allegro.h>

#include "Point2D.h"

#define pdIndex(col, row) (((row) * (tilesAcross + 1)) + (col))
#define tdIndex(col, row) (((row)*tilesAcross) + (col))

struct AdvancedTileSet {
  public:
    bool groundNavigation;
    bool airNavigation;
    int variations;
    std::shared_ptr<ALLEGRO_BITMAP> tiles;

    AdvancedTileSet(
        std::shared_ptr<ALLEGRO_BITMAP> Tiles,
        int Variations,
        bool GroundNavigation = true,
        bool AirNavigation = true)
        : groundNavigation(GroundNavigation), airNavigation(AirNavigation),
          variations(Variations), tiles(Tiles) {}

    ~AdvancedTileSet() {}

    bool IsGroundNavigatable() { return groundNavigation; }
    bool IsAirNavigatable() { return airNavigation; }
    std::shared_ptr<ALLEGRO_BITMAP> getTiles() { return tiles; }
    int getVariations() { return variations; }
};

class AdvancedTileScroller {
  private:
    ALLEGRO_BITMAP *scrollbuffer;
    std::vector<AdvancedTileSet *> tiles;

    std::map<int, ALLEGRO_BITMAP *> tileImageCache;
    std::map<int, ALLEGRO_BITMAP *>::iterator cacheIt;

    ALLEGRO_BITMAP **tileData;
    short *pointData;

    int tileWidth, tileHeight;
    int tilesAcross, tilesDown;
    float scrollX, scrollY;
    int windowWidth, windowHeight;

    ALLEGRO_BITMAP *GenerateTile(int BaseTileSet, int TileX, int TileY);
    short CalcTileBaseType(int TileX, int TileY);
    ALLEGRO_BITMAP *FindTile(int key);

  public:
    AdvancedTileScroller(
        int TilesAcross,
        int TilesDown,
        int TileWidth,
        int TileHeight);
    ~AdvancedTileScroller();

    void Destroy();
    int CreateScrollBuffer(int Width, int Height);
    bool LoadTileSet(
        char *FileName,
        int Variations,
        bool GroundNavigation = true,
        bool AirNavigation = true);
    bool LoadTileSet(
        std::shared_ptr<ALLEGRO_BITMAP> tileImage,
        int Variations,
        bool GroundNavigation = true,
        bool AirNavigation = true);
    bool GenerateTiles();

    void ResetTiles();
    void ResetPointData();
    void ClearTileImageCache();
    void UpdateScrollBuffer();
    void
    DrawScrollWindow(ALLEGRO_BITMAP *Dest, int X, int Y, int Width, int Height);
    bool CheckCollisionbyCoords(int X, int Y, bool Flying = false);
    void ConvertCoordstoNearestPoint(int &X, int &Y);

    // Accessors
    int getTileWidth() const { return tileWidth; }
    int getTileHeight() const { return tileHeight; }
    int getTilesAcross() const { return tilesAcross; }
    int getTilesDown() const { return tilesDown; }
    short getPointData(int Column, int Row) const {
        return pointData[pdIndex(Column, Row)];
    }
    float getScrollX() const { return scrollX; }
    float getScrollY() const { return scrollY; }

    // Mutators
    void setTileSize(int Width, int Height) {
        tileWidth = Width;
        tileHeight = Height;
    }
    void setScrollPosition(float X, float Y) {
        scrollX = X;
        scrollY = Y;
    }
    void setTileImage(ALLEGRO_BITMAP *image, int Column, int Row) {
        tileData[tdIndex(Column, Row)] = image;
    }
    void setPointData(int Column, int Row, short Value) {
        pointData[pdIndex(Column, Row)] = Value;
    }
    void setRegionSize(int Width, int Height) {
        if (Width >= 0 && Width <= 2500)
            tilesAcross = Width;
        if (Height >= 0 && Height <= 2500)
            tilesDown = Height;
    }
};

#endif
// vi: ft=cpp
