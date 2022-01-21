#ifndef TILESCROLLER_H_
#define TILESCROLLER_H_ 1

#include "Point2D.h"
#include <allegro5/allegro.h>
#include <vector>

/// Tileset for use with the TileScroller
///
/// This class is a bitmap containing a grid of tiles.
class TileSet {
  public:
    /// Constructor
    ///
    /// Objects of this class retain a pointer to the \a bitmap data for the
    /// lifetime of this class. They interpret the bitmap as a grid of
    /// width x height tile images, arranged in a rows x columns grid without
    /// any padding between them.
    ///
    /// Individual tiles are addressed by a tile value, which is interpreted
    /// using the grid layout, with tile 0 being the top left of the grid.
    ///
    /// \param[in] bitmap
    ///     Pointer to the ALLEGRO_BITMAP containg the image.
    /// \param[in] width
    ///     Width in pixels of the individual tiles within the tile set.
    /// \param[in] height
    ///     Height in pixels of the individual tiles within the tile set.
    /// \param[in] columns
    ///     Layout of the tiles within bitmap: the number of tile images in
    ///     each row
    /// \param[in] rows
    ///     Layout of the tiles within bitmap: the number of rows of tile
    ///     images.
    TileSet(
        ALLEGRO_BITMAP *bitmap, int width, int height, int columns, int rows);

    /// Draw a tile to the current target bitmap
    ///
    /// \param[in] tile
    ///     The tile to display. This must be in the range [0, rows x columns).
    /// \param[in] x
    ///     Pixel x coordinate to display the tile at.
    /// \param[in] x
    ///     Pixel y coordinate to display the tile at.
    ///
    /// \return void
    void draw_tile(short tile, int x, int y) const;

    /// Retrieve width
    ///
    /// \return The width of tiles in the tile set.
    inline int
    get_tile_width() const {
        return m_width;
    }

    /// Retrieve height
    ///
    /// \return The height of tiles in the tile set.
    inline int
    get_tile_height() const {
        return m_height;
    }

  protected:
    //! Bitmap containing the tile subimages
    ALLEGRO_BITMAP *m_tiles;
    //! Width of tiles in m_tiles
    int m_width;
    //! Height of tiles in m_tiles
    int m_height;
    //! Number of columns in m_tiles
    int m_columns;
    //! Number of rows of tiles in m_tiles
    int m_rows;
};

/// Bitmap Tile scroller
///
/// This class implements a display of an area consisting of images selected
/// from a TileSet that can be smoothly scrolled over. Typical usage is to
/// call set_tile() to select which tile is visible at each coordinate within
/// the TileScroller, call set_scroll_position() to select which area to
/// display, and then draw_scroll_window() to render the tiles at the desired
/// location to a destination bitmap.
class TileScroller {
  public:
    /// Constructor
    ///
    /// Create a new TileScroller. The tile data will be filled such that
    /// draw_scroll_window() will render the entire area as tile 0 from the
    /// TileSet
    ///
    /// \param[in] t
    ///    A TileSet containing the images to display.
    /// \param[in] max_x
    ///     The maximum horizontal dimension (in tiles) of the tile grid.
    /// \param[in] max_y
    ///     The maximum vertical dimension (in tiles) of the tile grid.
    /// \param[in] region_width
    ///     The width (in pixels) of the bitmap that this object creates from
    ///     the tile data grid and the tile images.
    /// \param[in] region_height
    ///     The height (in pixels) of the bitmap that this object creates from
    ///     the tile data grid and the tile images.
    /// \param[in] scroll_offset
    ///     Offset (in pixels) to shift the display relative to the current
    ///     scroll position. Set this value to have this object treat the values
    ///     passed to set_scroll_position() as other than the top left corner of
    ///     the view.
    TileScroller(const TileSet &t,
                 int max_x,
                 int max_y,
                 int region_width,
                 int region_height,
                 const Point2D &scroll_offset = Point2D(0, 0));
    ~TileScroller();

    //! Set all tile data in this object to use tile 0.
    void reset_tiles();

    /// Set the tile to use at a position
    ///
    /// \param[in] x
    ///     The x coordinate of the scroll area to set. This must be within the
    ///     range [0, max_x].
    /// \param[in] y
    ///     The y coordinate of the scroll area to set. This must be within the
    ///     range [0, max_y].
    /// \param[in] value
    ///     The tile to use at this position.
    /// \return void
    inline void
    set_tile(int x, int y, short value) {
        ALLEGRO_ASSERT(x >= 0 && x < m_tile_max_x);
        ALLEGRO_ASSERT(y >= 0 && y < m_tile_max_y);

        short &datum = m_tile_data[x + m_tile_max_x * y];
        if (datum != value) {
            m_dirty = true;
        }
        datum = value;
    }

    /// Set scroll position
    ///
    /// This sets the scroll position to the given values in tile coordinates.
    /// The position is adjusted by the offset passed to the constructor.
    ///
    /// \param[in] p
    ///     The position in tile coordinates to scoll to.
    /// \return void
    void
    set_scroll_position(const Point2D &p) {
        Point2D new_position(p.x * m_tiles.get_tile_width() - m_scroll_offset.x,
                             p.y * m_tiles.get_tile_height() -
                                 m_scroll_offset.y);
        int tile_width = m_tiles.get_tile_width();
        int tile_height = m_tiles.get_tile_height();

        if (static_cast<int>(new_position.x / tile_width) !=
                static_cast<int>(m_scroll_position.x / tile_width) ||
            static_cast<int>(new_position.y / tile_height) !=
                static_cast<int>(m_scroll_position.y / tile_height)) {
            m_dirty = true;
        }
        m_scroll_position = new_position;
    }

    /// Draw the scroll buffer to a bitmap
    ///
    /// Draws a section of the current scroll buffer to a position the \a dest
    /// bitmap.
    ///
    /// \param[in,out] dest
    ///     A pointer to the bitmap to draw to.
    /// \param[in] x
    ///     The x coordinate (in pixels) to draw to in the bitmap.
    /// \param[in] y
    ///     The y coordinate (in pixels) to draw to in the bitmap.
    /// \param[in] width
    ///     The width (in pixels) of the scroll buffer to draw to the bitmap.
    /// \param[in] height
    ///     The height (in pixels) of the scroll buffer to draw to the bitmap.
    /// \return void
    void draw_scroll_window(
        ALLEGRO_BITMAP *dest, int x, int y, int width, int height);

  private:
    /// Get the tile value at a position
    ///
    /// Get the value previously set by calling set_tile(). If the (x, y)
    /// coordinates are outside the tile area, return 0.
    ///
    /// \param x
    ///     Tile x coordinate.
    /// \param y
    ///     Tile y coordinate.
    /// \return The value of the tile or 0 if the position is out of bounds.
    inline short
    get_tile(int x, int y) {
        if (x < 0 || x >= m_tile_max_x || y < 0 || y >= m_tile_max_y) {
            return 0;
        }
        return m_tile_data[x + m_tile_max_x * y];
    }
    /// Update scroll buffer
    ///
    /// Redraw the tiles to the scroll buffer based on their current values
    /// in m_tile_data at m_scroll_position.
    /// \return void
    void update_scroll_buffer();

    //! Vector containing which tile number to use for the area being scrolled
    std::vector<short> m_tile_data;

    //! Tile graphics
    TileSet m_tiles;

    /// Position in pixel coordinates of the top-left corner of the area to
    /// render in the scroll buffer.
    Point2D m_scroll_position;

    //! Size of the scroll buffer in pixel coordinates.
    Point2D m_viewport_dimensions;

    //! Maximum tile x coordinate.
    int m_tile_max_x;

    //! Maximum tile y coordinate.
    int m_tile_max_y;

    /// Bitmap that contains the visual representation of the tile data at
    /// m_scroll_position
    ALLEGRO_BITMAP *m_scrollbuffer;

    // Flag to call update_scroll_buffer() when draw_scroll_window() is called.
    bool m_dirty;

    /// Scroll offset
    ///
    /// Offset subtracted from the value passed to set_scroll_position() to
    /// allow the application to treat that value a centered position.
    Point2D m_scroll_offset;

    /// Disable copy constructor to prevent accidental misuse of this class.
    TileScroller(const TileScroller &) = delete;

    /// Disable assignment operator to prevent accidental misuse of this class.
    TileScroller &operator=(const TileScroller &) = delete;
};

#endif
