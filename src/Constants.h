#ifndef SFTLC_CONSTANTS_H
#define SFTLC_CONSTANTS_H

// DO NOT MODIFY THESE
static constexpr int SCREEN_WIDTH = 1024;
static constexpr int SCREEN_HEIGHT = 768;
static constexpr int GALAXY_WIDTH = 250;
static constexpr int GALAXY_HEIGHT = 220;

// COMMON RGB COLORS
// here's a good source of rgb colors:
// http://www.pitt.edu/~nisg/cis/web/cgi/rgb.html
#define BLACK al_map_rgb(0, 0, 0)
#define GRAY1 al_map_rgb(232, 232, 232)
#define DGRAY al_map_rgb(120, 120, 120)
#define WHITE al_map_rgb(255, 255, 255)
#define BLUE al_map_rgb(0, 0, 255)
#define LTBLUE al_map_rgb(150, 150, 255)
#define SKYBLUE al_map_rgb(0, 216, 255)
#define DODGERBLUE al_map_rgb(30, 144, 255)
#define ROYALBLUE al_map_rgb(39, 64, 139)
#define PURPLE al_map_rgb(212, 72, 255)
#define RED al_map_rgb(255, 0, 0)
#define LTRED al_map_rgb(255, 150, 150)
#define ORANGE al_map_rgb(255, 165, 0)
#define DKORANGE al_map_rgb(255, 140, 0)
#define BRTORANGE al_map_rgb(255, 120, 0)
#define YELLOW al_map_rgb(250, 250, 0)
#define LTYELLOW al_map_rgb(255, 255, 0)
#define GREEN al_map_rgb(0, 255, 0)
#define LTGREEN al_map_rgb(150, 255, 150)
#define PINEGREEN al_map_rgb(80, 170, 80)
#define STEEL al_map_rgb(159, 182, 205)
#define KHAKI al_map_rgb(238, 230, 133)
#define DKKHAKI al_map_rgb(139, 134, 78)

#define GREEN2 al_map_rgb(71, 161, 91)
#define RED2 al_map_rgb(110, 26, 15)
#define YELLOW2 al_map_rgb(232, 238, 106)
#define GOLD al_map_rgb(255, 216, 0)
#define MASK_COLOR al_map_rgb(255, 0, 255)

#define FLUX_SCANNER_ID 2

#endif // SFTLC_CONSTANTS_H
// vi: ft=cpp
