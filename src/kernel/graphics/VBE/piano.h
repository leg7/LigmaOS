#pragma once
#include <library/types.h>
#include "vbe_graphics.h"

#define PIANO_OUTLINE_COLOR BLUE
#define KEY_PRESS_COLOR SILVER
#define PIANO_POSITION_X 230
#define PIANO_POSITION_Y 280

#define WHITE_KEY_WIDTH 80
#define WHITE_KEY_HEIGHT 200
#define BLACK_KEY_WIDTH 40
#define BLACK_KEY_HEIGHT 150

#define BLACK_KEY_COUNT 5
#define WHITE_KEY_COUNT 7

enum key_position
{
    POSITION_C       = PIANO_POSITION_X,
    POSITION_D       = PIANO_POSITION_X+WHITE_KEY_WIDTH,
    POSITION_E       = PIANO_POSITION_X+WHITE_KEY_WIDTH*2,
    POSITION_F       = PIANO_POSITION_X+WHITE_KEY_WIDTH*3,
    POSITION_G       = PIANO_POSITION_X+WHITE_KEY_WIDTH*4,
    POSITION_A       = PIANO_POSITION_X+WHITE_KEY_WIDTH*5,
    POSITION_B       = PIANO_POSITION_X+WHITE_KEY_WIDTH*6,

    POSITION_C_SHARP = PIANO_POSITION_X+WHITE_KEY_WIDTH-(BLACK_KEY_WIDTH/2),
    POSITION_D_SHARP = PIANO_POSITION_X+WHITE_KEY_WIDTH*2-(BLACK_KEY_WIDTH/2),
    POSITION_F_SHARP = PIANO_POSITION_X+WHITE_KEY_WIDTH*4-(BLACK_KEY_WIDTH/2),
    POSITION_G_SHARP = PIANO_POSITION_X+WHITE_KEY_WIDTH*5-(BLACK_KEY_WIDTH/2),
    POSITION_A_SHARP = PIANO_POSITION_X+WHITE_KEY_WIDTH*6-(BLACK_KEY_WIDTH/2)
};


void VBE_put_piano();
void VBE_press_key(enum key_position pos);
void VBE_release_key(enum key_position pos);
