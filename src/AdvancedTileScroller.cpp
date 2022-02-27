#include <allegro5/allegro.h>

#include "AdvancedTileScroller.h"
#include "Game.h"
#include "Util.h"

using namespace std;

AdvancedTileScroller::AdvancedTileScroller(
    int TilesAcross,
    int TilesDown,
    int TileWidth,
    int TileHeight)
    : scrollbuffer(0), tiles(0), tileData(0), pointData(0),
      tileWidth(TileWidth), tileHeight(TileHeight), tilesAcross(TilesAcross),
      tilesDown(TilesDown), scrollX(0.0f), scrollY(0.0f), windowWidth(0),
      windowHeight(0) {
    tileData = new ALLEGRO_BITMAP *[TilesAcross * TilesDown];
    pointData = new data[(TilesAcross + 1) * (TilesDown + 1)];
    ResetTiles();
    ResetPointData();
}

AdvancedTileScroller::~AdvancedTileScroller() { this->Destroy(); }

void
AdvancedTileScroller::Destroy() {
    if (scrollbuffer) {
        al_destroy_bitmap(scrollbuffer);
        scrollbuffer = nullptr;
    }

    for (int i = 0; i < (int)tiles.size(); ++i) {
        if (tiles[i] != nullptr) {
            // if tile images were loaded from ALLEGRO_BITMAP file, then we must
            // free them but if a pointer to a data file ALLEGRO_BITMAP was
            // used, do not free the memory!

            delete tiles[i];
            tiles[i] = nullptr;
        }
    }
    tiles.clear();

    ClearTileImageCache();
    delete[] pointData;
    delete[] tileData;
}

void
AdvancedTileScroller::ResetTiles() {
    for (int x = 0; x < tilesAcross * tilesDown; ++x) {
        tileData[x] = NULL;
    }
}

void
AdvancedTileScroller::ResetPointData() {
    for (int x = 0; x < (tilesAcross + 1) * (tilesDown + 1); ++x) {
        pointData[x] = 0;
    }
}

void
AdvancedTileScroller::ClearTileImageCache() {
    for (auto cacheIt : tileImageCache) {
        al_destroy_bitmap(cacheIt.second);
    }
    tileImageCache.clear();
}

bool
AdvancedTileScroller::LoadTileSet(
    shared_ptr<ALLEGRO_BITMAP> tileImage,
    int Variations,
    bool GroundNavigation,
    bool AirNavigation) {
    if (!tileImage) {
        g_game->message("PlanetSurface: error loading tileSetImage");
        return false;
    }

    tiles.push_back(new AdvancedTileSet(
        tileImage, Variations, GroundNavigation, AirNavigation));

    return true;
}

bool
AdvancedTileScroller::GenerateTiles() {
    if (tileImageCache.size() > 0) {
        ClearTileImageCache();
    }

    sfsrand(99);

    for (int x = 0; x < tilesAcross; ++x) {
        for (int y = 0; y < tilesDown; ++y) // Loop through all the tiles
        {
            tileData[tdIndex(x, y)] =
                GenerateTile(CalcTileBaseType(x, y), x, y);
        }
    }

    return true;
}

ALLEGRO_BITMAP *
AdvancedTileScroller::GenerateTile(int BaseTileSet, int TileX, int TileY) {
    cache_key key = 0;
    int pdIndexes[4];
    int var = sfrand() & (ALLEGRO_FLIP_HORIZONTAL | ALLEGRO_FLIP_VERTICAL);
    pdIndexes[0] = pdIndex(TileX, TileY);
    pdIndexes[1] = pdIndex(TileX, TileY + 1);
    pdIndexes[2] = pdIndex(TileX + 1, TileY + 1);
    pdIndexes[3] = pdIndex(TileX + 1, TileY);

    key = BaseTileSet | static_cast<cache_key>(pointData[pdIndexes[0]]) << 8
          | static_cast<cache_key>(pointData[pdIndexes[1]]) << 16
          | static_cast<cache_key>(pointData[pdIndexes[2]]) << 24
          | static_cast<cache_key>(pointData[pdIndexes[3]]) << 32
          | static_cast<cache_key>(var) << 40;

    ALLEGRO_BITMAP *tile = FindTile(key);
    if (tile == NULL) {
        tile = al_create_bitmap(tileWidth, tileHeight);
        al_set_target_bitmap(tile);
        al_clear_to_color(al_map_rgba(0, 0, 0, 0));

        al_draw_bitmap_region(
            tiles[BaseTileSet]->getTiles().get(),
            0,
            0,
            tileWidth,
            tileHeight,
            0,
            0,
            var);

        int pdValues[4];
        pdValues[0] = pointData[pdIndexes[0]];
        pdValues[1] = pointData[pdIndexes[1]];
        pdValues[2] = pointData[pdIndexes[2]];
        pdValues[3] = pointData[pdIndexes[3]];

        // Generates a new tile based on the 4 points being examined
        // 0 3
        // 1 2
        // Each of the TileSets contains a base tile at position 0, then
        // 15 other variations on the tile, which only supply the color of
        // the tile to some quadrants of the tile.
        //
        // These loops choose the variant for each of the quadrants which has
        // a higher tile value than the base and overlay it on the base bitmap.
        for (int i = 0; i < 4; ++i) {
            if (pdValues[i] > BaseTileSet) {
                int accessoryType = 1 << i;
                for (int j = i + 1; j < 4; ++j) {
                    if (pdValues[i] == pdValues[j]) {
                        accessoryType += (1 << j);
                        pdValues[j] = BaseTileSet;
                    }
                }

                al_draw_bitmap_region(
                    tiles[pdValues[i]]->getTiles().get(),
                    (accessoryType % 4) * tileWidth,
                    (accessoryType / 4) * tileHeight,
                    tileWidth,
                    tileHeight,
                    0,
                    0,
                    0);
            }
        }
        tileImageCache[key] = tile;
    }

    return tile;
}

AdvancedTileScroller::data
AdvancedTileScroller::CalcTileBaseType(int TileX, int TileY) {
    int pdIndexes[4];
    pdIndexes[0] = pdIndex(TileX, TileY);
    pdIndexes[1] = pdIndex(TileX, TileY + 1);
    pdIndexes[2] = pdIndex(TileX + 1, TileY + 1);
    pdIndexes[3] = pdIndex(TileX + 1, TileY);

    int smallest = 0;
    for (int i = 0; i < 4; ++i) {
        if (pointData[pdIndexes[smallest]] > pointData[pdIndexes[i]]) {
            smallest = i;
        }
    }

    return (pointData[pdIndexes[smallest]] < (int)tiles.size())
               ? pointData[pdIndexes[smallest]]
               : (int)tiles.size() - 1;
}

ALLEGRO_BITMAP *
AdvancedTileScroller::FindTile(cache_key key) {
    auto it = tileImageCache.find(key);

    if (it != tileImageCache.end()) {
        return it->second;
    }

    return nullptr;
}

int
AdvancedTileScroller::CreateScrollBuffer(int Width, int Height) {
    windowWidth = Width;
    windowHeight = Height;
    if (scrollbuffer) {
        al_destroy_bitmap(scrollbuffer);
    }
    scrollbuffer =
        al_create_bitmap(Width + (tileWidth * 2), Height + (tileHeight * 2));
    return (scrollbuffer != nullptr);
}

/**
 * Fills the scroll buffer with tiles based on current scrollX, scrollY
 **/
void
AdvancedTileScroller::UpdateScrollBuffer() {
    // prevent a crash
    if ((!scrollbuffer))
        return;
    if (tileWidth < 1 || tileHeight < 1)
        return;

    al_set_target_bitmap(scrollbuffer);
    al_clear_to_color(BLUE);
    // calculate starting tile position
    int tilex = (int)scrollX / tileWidth;
    int tiley = (int)scrollY / tileHeight;

    // calculate the number of columns and rows
    int cols = windowWidth / tileWidth;
    int rows = windowHeight / tileHeight;

    // draw tiles onto the scroll buffer surface
    al_set_target_bitmap(scrollbuffer);

    for (int y = 0; y <= rows && y + tiley < tilesDown; ++y) {
        for (int x = 0; x <= cols && x + tilex < tilesAcross; ++x) {
            al_draw_bitmap_region(
                tileData[tdIndex(tilex + x, tiley + y)],
                0,
                0,
                tileWidth,
                tileHeight,
                x * tileWidth,
                y * tileHeight,
                0);
        }
    }
}

/**
 * Draws the portion of the scroll buffer based on the current partial tile
 *scroll position.
 **/
void
AdvancedTileScroller::DrawScrollWindow(
    ALLEGRO_BITMAP *Dest,
    int X,
    int Y,
    int Width,
    int Height) {
    // prevent a crash
    if (tileWidth < 1 || tileHeight < 1)
        return;

    // calculate the partial sub-tile lines to draw using modulus
    int partialx = (int)scrollX % tileWidth;
    int partialy = (int)scrollY % tileHeight;
    al_set_target_bitmap(Dest);

    // draw the scroll buffer to the destination ALLEGRO_BITMAP
    if (scrollbuffer)
        al_draw_bitmap_region(
            scrollbuffer, partialx, partialy, Width, Height, X, Y, 0);
}

bool
AdvancedTileScroller::CheckCollisionbyCoords(int X, int Y, bool Flying) {
    bool result = false;

    ConvertCoordstoNearestPoint(X, Y);

    if (Flying)
        result = tiles[getPointData(X, Y)]->IsAirNavigatable();
    else
        result = tiles[getPointData(X, Y)]->IsGroundNavigatable();

    return !result;
}

void
AdvancedTileScroller::ConvertCoordstoNearestPoint(int &X, int &Y) {
    double x, y = 0;

    x = round(static_cast<double>(X) / tileWidth);
    y = round(static_cast<double>(Y) / tileHeight);

    X = static_cast<int>(x);
    Y = static_cast<int>(y);
}
// vi: ft=cpp
