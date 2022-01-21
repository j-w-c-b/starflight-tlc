#include <cmath>
#include <sstream>

#include <allegro5/allegro.h>

#include "TexturedSphere.h"

using namespace std;

TexturedSphere::TexturedSphere(int tex_size) {
    TEX_SIZE = tex_size;
    MAP_SIZE = 256;
    ASPECT_RATIO = 1.04;
    coord_transform_table = NULL;
    screen2sphere_table = NULL;
    tex_table = NULL;
    source_bmp = NULL;
    textureWasSet = false;
    InitSphereLookupTables();
}

TexturedSphere::~TexturedSphere() {
    // only destroy ALLEGRO_BITMAP if it was loaded, not passed
    // a passed texture should be destroyed by the caller
    if (!textureWasSet) {
        if (source_bmp != NULL)
            al_destroy_bitmap(source_bmp);
    }

    if (coord_transform_table != NULL)
        free(coord_transform_table);
    if (screen2sphere_table != NULL)
        free(screen2sphere_table);
    if (tex_table != NULL)
        free(tex_table);
}

bool
TexturedSphere::SetTexture(ALLEGRO_BITMAP *new_texture) {
    // destroy ALLEGRO_BITMAP if it was previously created
    if (source_bmp != NULL) {
        al_destroy_bitmap(source_bmp);
        source_bmp = NULL;
    }

    if (new_texture == NULL)
        return false;
    else {
        source_bmp = new_texture;
        textureWasSet = true; // do not destroy passed texture
    }

    // assuming texture is valid, then generate the map
    CreateTextureTable(source_bmp);

    return true;
}

void
TexturedSphere::Spherical2Cartesian(
    int alpha, int beta, double *x, double *y, double *z) {
    /* Convert to radians */
    double alpha1 = (double)alpha * 2 * M_PI / MAP_SIZE;
    double beta1 = (double)(beta - MAP_SIZE / 2) * M_PI / MAP_SIZE;

    /* Convert to Cartesian */
    *x = cos(alpha1) * cos(beta1);
    *y = sin(beta1);
    *z = sin(alpha1) * cos(beta1);
}

void
TexturedSphere::Cartesian2Sphere(
    double x, double y, double z, int *alpha, int *beta) {
    double beta1, alpha1;

    /* convert to Spherical Coordinates */
    beta1 = asin(y);
    if (fabs(cos(beta1)) > 0.0) { // we'll be dividing by cos(beta1)
        double w = x / cos(beta1);
        if (w > 1)
            w = 1;
        if (w < -1)
            w = -1; // Check bounds
        alpha1 = acos(w);
        if (z / cos(beta1) <
            0) // Check for wrapping around top/bottom of sphere
            alpha1 = 2 * M_PI - alpha1;
    } else
        alpha1 = 0;

    /* Convert to texture coordinates */
    *alpha = (int)(alpha1 / (M_PI * 2) * MAP_SIZE);
    *beta = (int)(beta1 / M_PI * MAP_SIZE + MAP_SIZE / 2);

    /* 'Clip' the texture coordinates */
    if (*alpha < 0)
        *alpha = 0;
    if (*alpha >= MAP_SIZE)
        *alpha = MAP_SIZE - 1;
    if (*beta < 0)
        *beta = 0;
    if (*beta >= MAP_SIZE)
        *beta = MAP_SIZE - 1;
}

// Do not call unless source ALLEGRO_BITMAP has been loaded first!
// unsigned short for 16 bit, can use int for 32 bit, char for 8 bit
void
TexturedSphere::CreateTextureTable(ALLEGRO_BITMAP *bmp) {
    int x, y;
    ALLEGRO_COLOR p;
    tex_table = (ALLEGRO_COLOR *)malloc(
        (TEX_SIZE * (TEX_SIZE + 1) * sizeof(ALLEGRO_COLOR)));

    int new_bitmap_flags = al_get_new_bitmap_flags();
    al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
    ALLEGRO_BITMAP *mem_bmp = al_clone_bitmap(bmp);
    al_set_new_bitmap_flags(new_bitmap_flags);
    int width = al_get_bitmap_width(mem_bmp);
    int height = al_get_bitmap_height(mem_bmp);

    // jjh-> working on getting the spherical image on the screen to match the
    // surface image in the aux screen.  Initially, the spherical image was
    // upside down.  with the new code added below, it's now right-side up, and
    // no longer a mirror image on longitude.

    // Probably a more eleagant way to do this, but this works for now.  old
    // code fills tex_table left-to-right, top-to-bottom. new code still pulls
    // pixel that way, but fills tex_table bottom-to-top and right-to-left.

    for (int i = 0; i < TEX_SIZE;
         i++) // i controls the column.  i = 0 starts in column 0
    {         // j controls the row. j = 0 starts row 0
        int testj = TEX_SIZE;           // testj starts row at 255
        int testi = (TEX_SIZE - 1) - i; // testi starts column at 255
        for (int j = 0; j < TEX_SIZE + 1; j++) {
            x = i * width / TEX_SIZE; // i and j dictate which pixel is pulled
                                      // from the texture.
            y = j * height /
                TEX_SIZE; // so don't change them to solve the problem.
            p = al_get_pixel(mem_bmp, x, y);
            // map 2D coords into 1D array
            tex_table[testj * TEX_SIZE + testi] = p;
            testj--;
        }
    }
    al_destroy_bitmap(mem_bmp);
}

void
TexturedSphere::InitSphereLookupTables() {
    int alpha, beta; // Spherical Coordinates
    int i, j;
    double x, y, z; // Cartesian coordinates

    coord_transform_table =
        (int *)malloc((TEX_SIZE * (TEX_SIZE + 1) * sizeof(int)));

    screen2sphere_table = (int *)malloc(TEX_SIZE * sizeof(int));

    /* Compute the Lookup Table for the Switching between
        the Coordinate-Systems */
    for (j = 0; j < TEX_SIZE + 1; j++) {
        for (i = 0; i < TEX_SIZE; i++) {
            /* Convert Spherical Coord. to Cartesian Coord. */
            Spherical2Cartesian(i, j, &x, &y, &z);

            /* Convert Cartesian Coord. to Spherical Coord.
            Notice it's not x,y,z but x,z,y */
            Cartesian2Sphere(x, z, y, &alpha, &beta);

            /* lower order of bits occupied by alpha,
            upper order shifted by TEX_SIZE occupied by beta */
            coord_transform_table[i + j * TEX_SIZE] = alpha + beta * TEX_SIZE;
        }
    }

    /* Compute the Lookup Table that is used to convert the 2D Screen
    Coordinates to the initial Spherical Coordinates */
    for (i = 0; i < TEX_SIZE; i++) {
        screen2sphere_table[i] =
            (int)(acos((double)(i - TEX_SIZE / 2 + 1) * 2 / TEX_SIZE) *
                  TEX_SIZE / M_PI);
        screen2sphere_table[i] %= TEX_SIZE;
    }
}

// phi, theta, psi must be 0-255 due to lookup table
void
TexturedSphere::Draw(ALLEGRO_BITMAP *dest,
                     int phi,
                     int theta,
                     int psi,
                     int radius,
                     int center_x,
                     int center_y) {
    int x, y; // current Pixel-Position
    int xr;   // Half Width of Sphere (pixels) in current scanline
    int alpha_beta2, alpha_beta3;
    int old_bitmap_flags = al_get_new_bitmap_flags();
    al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
    ALLEGRO_BITMAP *memory_bmp = al_clone_bitmap(dest);
    al_set_new_bitmap_flags(old_bitmap_flags);
    /* spherical coordinates of the 2nd and 3rd rotated system
        (the 2 coordinates are stored in a single integer)       */

    al_set_target_bitmap(memory_bmp);
    /* For all Scanlines ... */
    for (y = -radius + 1; y < radius; y++) {
        int xinc, xscaled; // auxiliary variables
        int beta1, alpha1; // initial spherical coordinates
        /* compute the Width of the Sphere in this Scanline */
        xr = (int)(sqrt((double)(radius * radius - y * y)) *
                   ASPECT_RATIO); // Can be turned into fixed point
        if (xr == 0)
            xr = 1;

        /* compute the first Spherical Coordinate beta */
        beta1 = screen2sphere_table[(y + radius) * TEX_SIZE / (2 * radius)] *
                TEX_SIZE;

        // x << y == x * 2^y
        // xinc = (TEX_SIZE * 65535) / (2*xr);
        xinc = 16776960 / (2 * xr);
        xscaled = 0;

        /* For all Pixels in this Scanline ... */
        for (x = -xr; x < xr; x++) {
            /* compute the second Spherical Coordinate alpha */
            // x >> y == x / 2^y
            // xscaled / 2^16
            // xscaled / 65536
            alpha1 = screen2sphere_table[xscaled / 65536] / 2;

            xscaled += xinc;

            alpha1 = alpha1 + phi;
            /* Rotate Texture in the first Coordinate-System (alpha,beta);
                Switch to the next Coordinate-System and rotate there       */
            alpha_beta2 = coord_transform_table[beta1 + alpha1] + theta;
            /* the same Procedure again ... */
            alpha_beta3 = coord_transform_table[alpha_beta2] + psi;

            /* draw the Pixel */
            al_put_pixel(x + center_x, y + center_y, tex_table[alpha_beta3]);
        }
    }
    al_set_target_bitmap(dest);
    al_draw_bitmap(memory_bmp, 0, 0, 0);
    al_destroy_bitmap(memory_bmp);
}
