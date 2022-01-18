#include <cmath>
#include <iostream>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "TileScroller.h"

using namespace std;

TileSet::TileSet(ALLEGRO_BITMAP *tiles, int width, int height, int columns, int rows) : m_tiles(tiles), m_width(width), m_height(height), m_columns(columns), m_rows(rows)
{
    ALLEGRO_ASSERT(m_tiles != nullptr);
    ALLEGRO_ASSERT(m_width > 0);
    ALLEGRO_ASSERT(m_height > 0);
    ALLEGRO_ASSERT(m_rows > 0);
    ALLEGRO_ASSERT(m_columns > 0);
}

// Draw the given tile to the target bitmap at the position
void
TileSet::draw_tile(short tile, int x, int y) const {
    ALLEGRO_ASSERT(tile >= 0);
    ALLEGRO_ASSERT(tile < (m_columns * m_rows));

    int left = (tile % m_columns) * m_width;
    int top = (tile / m_columns) * m_height;

    al_draw_bitmap_region(
        m_tiles,
        left, top,
        m_width, m_height,
        x * m_width, y * m_height,
        0);
}

TileScroller::TileScroller(const TileSet &t, int tile_max_x, int tile_max_y, int region_width, int region_height, const Point2D &scroll_offset)
        : m_tile_data(tile_max_x * tile_max_y, 0),
        m_tiles(t),
        m_scroll_position(0, 0),
        m_viewport_dimensions(region_width, region_height),
        m_tile_max_x(tile_max_x),
        m_tile_max_y(tile_max_y),
        m_scrollbuffer(nullptr),
        m_dirty(true),
        m_scroll_offset(scroll_offset)
{
    ALLEGRO_ASSERT(m_viewport_dimensions.x > 0);
    ALLEGRO_ASSERT(m_viewport_dimensions.y > 0);
    ALLEGRO_ASSERT(m_tile_max_x > 0);
    ALLEGRO_ASSERT(m_tile_max_y > 0);

    m_scrollbuffer = al_create_bitmap(
        region_width + m_tiles.get_tile_width() * 2,
        region_height + m_tiles.get_tile_height() * 2);

    ALLEGRO_ASSERT(m_scrollbuffer != nullptr);
}

TileScroller::~TileScroller()
{
	if (m_scrollbuffer) 
	{
		al_destroy_bitmap(m_scrollbuffer);
	}
}

void
TileScroller::reset_tiles() 
{
    for (auto &c : m_tile_data) {
        c = 0;
    }
}

void
TileScroller::update_scroll_buffer()
{
    int tile_width = m_tiles.get_tile_width();
    int tile_height = m_tiles.get_tile_height();

    // Tile containing m_scroll_position
    int tile_x = static_cast<int>(m_scroll_position.x) / tile_width;
    int tile_y = static_cast<int>(m_scroll_position.y) / tile_height;

    if (m_scroll_position.x < 0) {
        --tile_x;
    }
    if (m_scroll_position.y < 0) {
        --tile_y;
    }
    // calculate the number of columns and rows
    int columns = static_cast<int>(m_viewport_dimensions.x / tile_width) + 2;
    int rows = static_cast<int>(m_viewport_dimensions.y / tile_height) + 2;

    // draw tiles onto the scroll buffer surface
    al_set_target_bitmap(m_scrollbuffer);
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < columns; x++) {
            short tile = get_tile(tile_x + x, tile_y + y);
            m_tiles.draw_tile(tile, x, y);
        }
    }
    m_dirty = false;
}

void
TileScroller::draw_scroll_window(ALLEGRO_BITMAP *dest, int x, int y, int width, int height)
{
    if (m_dirty) {
        update_scroll_buffer();
    }

    int tile_width = m_tiles.get_tile_width();
    int tile_height = m_tiles.get_tile_height();

    int partial_x = static_cast<int>(m_scroll_position.x) % tile_width;
    int partial_y = static_cast<int>(m_scroll_position.y) % tile_height;

    if (partial_x < 0)
    {
        partial_x = tile_width + partial_x;
    }
    if (partial_y < 0)
    {
        partial_y = tile_height + partial_y;
    }

    al_set_target_bitmap(dest);
    al_draw_bitmap_region(
        m_scrollbuffer,
        partial_x, partial_y,
        width, height,
        x, y,
        0);
}
