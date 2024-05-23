#include "piano.h"
#include <stdio.h>

#define PIANO_OUTLINE_COLOR RED
#define PIANO_POSITION_X 160
#define PIANO_POSITION_Y 250

#define WHITE_KEY_WIDTH 100
#define WHITE_KEY_HEIGHT 200
#define BLACK_KEY_WIDTH 40
#define BLACK_KEY_HEIGHT 150

#define BLACK_KEY_COUNT 5
#define WHITE_KEY_COUNT 7

static enum key {
    //white keys
    C,
    D,
    E,
    F,
    G,
    A,
    B,
    //black keys
    C_SHARP,
    D_SHARP,
    F_SHARP,
    G_SHARP,
    A_SHARP,
    KEY_COUNT
};


static constexpr u16 key_to_position[KEY_COUNT] = {
    PIANO_POSITION_X,
    PIANO_POSITION_X+WHITE_KEY_WIDTH,
    PIANO_POSITION_X+WHITE_KEY_WIDTH*2,
    PIANO_POSITION_X+WHITE_KEY_WIDTH*3,
    PIANO_POSITION_X+WHITE_KEY_WIDTH*4,
    PIANO_POSITION_X+WHITE_KEY_WIDTH*5,
    PIANO_POSITION_X+WHITE_KEY_WIDTH*6,

    PIANO_POSITION_X+WHITE_KEY_WIDTH-(BLACK_KEY_WIDTH/2),
    PIANO_POSITION_X+WHITE_KEY_WIDTH*2-(BLACK_KEY_WIDTH/2),
    PIANO_POSITION_X+WHITE_KEY_WIDTH*4-(BLACK_KEY_WIDTH/2),
    PIANO_POSITION_X+WHITE_KEY_WIDTH*5-(BLACK_KEY_WIDTH/2),
    PIANO_POSITION_X+WHITE_KEY_WIDTH*6-(BLACK_KEY_WIDTH/2)
};




void VBE_put_piano_outline(){
    for (u16 x=PIANO_POSITION_X;x<PIANO_POSITION_X+7*WHITE_KEY_WIDTH;x++){
        VBE_plot_pixel_32bpp(x,PIANO_POSITION_Y,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(x,PIANO_POSITION_Y+WHITE_KEY_HEIGHT,PIANO_OUTLINE_COLOR);
    }
    
    for (u16 x=0;x<BLACK_KEY_WIDTH;x++){
        VBE_plot_pixel_32bpp(x+key_to_position[C_SHARP],PIANO_POSITION_Y+BLACK_KEY_HEIGHT,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(x+key_to_position[D_SHARP],PIANO_POSITION_Y+BLACK_KEY_HEIGHT,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(x+key_to_position[F_SHARP],PIANO_POSITION_Y+BLACK_KEY_HEIGHT,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(x+key_to_position[G_SHARP],PIANO_POSITION_Y+BLACK_KEY_HEIGHT,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(x+key_to_position[A_SHARP],PIANO_POSITION_Y+BLACK_KEY_HEIGHT,PIANO_OUTLINE_COLOR);
    }
    
    for (u16 y=PIANO_POSITION_Y;y<PIANO_POSITION_Y+WHITE_KEY_HEIGHT;y++){
        VBE_plot_pixel_32bpp(key_to_position[C],y,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(key_to_position[F],y,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(key_to_position[B]+WHITE_KEY_WIDTH,y,PIANO_OUTLINE_COLOR);
    }
    
    for (u16 y=(PIANO_POSITION_Y+BLACK_KEY_HEIGHT);y<PIANO_POSITION_Y+WHITE_KEY_HEIGHT;y++){
        VBE_plot_pixel_32bpp(key_to_position[D],y,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(key_to_position[E],y,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(key_to_position[G],y,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(key_to_position[A],y,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(key_to_position[B],y,PIANO_OUTLINE_COLOR);
    }
    
    for (u16 y=PIANO_POSITION_Y;y<PIANO_POSITION_Y+BLACK_KEY_HEIGHT;y++){
        VBE_plot_pixel_32bpp(key_to_position[C_SHARP],y,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(key_to_position[C_SHARP]+BLACK_KEY_WIDTH,y,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(key_to_position[D_SHARP],y,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(key_to_position[D_SHARP]+BLACK_KEY_WIDTH,y,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(key_to_position[F_SHARP],y,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(key_to_position[F_SHARP]+BLACK_KEY_WIDTH,y,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(key_to_position[G_SHARP],y,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(key_to_position[G_SHARP]+BLACK_KEY_WIDTH,y,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(key_to_position[A_SHARP],y,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(key_to_position[A_SHARP]+BLACK_KEY_WIDTH,y,PIANO_OUTLINE_COLOR);
    }
}

void fill_in_left_white_key(enum key K,u32 color){  //white key left of black key
    if ((K==C) | (K==F)){
        const u16 pos=key_to_position[K];
        for (u16 x=pos+1;x<pos+WHITE_KEY_WIDTH-(BLACK_KEY_WIDTH/2);x++){
            for (u16 y=PIANO_POSITION_Y+1;y<PIANO_POSITION_Y+WHITE_KEY_HEIGHT;y++){
                VBE_plot_pixel_32bpp(x,y,color);
            }
        }
        for (u16 x=pos+WHITE_KEY_WIDTH-(BLACK_KEY_WIDTH/2);x<pos+WHITE_KEY_WIDTH;x++){
            for (u16 y=PIANO_POSITION_Y+BLACK_KEY_HEIGHT+1;y<PIANO_POSITION_Y+WHITE_KEY_HEIGHT;y++){
                VBE_plot_pixel_32bpp(x,y,color);
            }
        }
    }
}

void fill_in_right_white_key(enum key K,u32 color){ //white key right of black keys
    if ((K==E) | (K==B)){
        const u16 pos=key_to_position[K];
        for (u16 x=pos;x<(pos+BLACK_KEY_WIDTH/2);x++){
            for (u16 y=PIANO_POSITION_Y+BLACK_KEY_HEIGHT;y<PIANO_POSITION_Y+WHITE_KEY_HEIGHT;y++){
                VBE_plot_pixel_32bpp(x,y,color);
            }
        }
        for (u16 x=pos+(BLACK_KEY_WIDTH/2);x<pos+WHITE_KEY_WIDTH;x++){
            for (u16 y=PIANO_POSITION_Y;y<PIANO_POSITION_Y+WHITE_KEY_HEIGHT;y++){
                VBE_plot_pixel_32bpp(x,y,color);
            }
        }
    }
}
void fill_in_inbetween_white_key(enum key K,u32 color){ //white key inbetween 2 black keys
    if ((K==D) | (K==G) | (K==A)){
        const u16 pos=key_to_position[K];
        for (u16 x=pos+1;x<(pos+BLACK_KEY_WIDTH/2);x++){
            for (u16 y=PIANO_POSITION_Y+BLACK_KEY_HEIGHT+1;y<PIANO_POSITION_Y+WHITE_KEY_HEIGHT;y++){
                VBE_plot_pixel_32bpp(x,y,color);
            }
        }
        for (u16 x=pos+(BLACK_KEY_WIDTH/2);x<pos+WHITE_KEY_WIDTH-(BLACK_KEY_WIDTH/2);x++){
            for (u16 y=PIANO_POSITION_Y+1;y<PIANO_POSITION_Y+WHITE_KEY_HEIGHT;y++){
                VBE_plot_pixel_32bpp(x,y,color);
            }
        }
        for (u16 x=pos+WHITE_KEY_WIDTH-(BLACK_KEY_WIDTH/2);x<pos+WHITE_KEY_WIDTH;x++){
            for (u16 y=PIANO_POSITION_Y+BLACK_KEY_HEIGHT;y<PIANO_POSITION_Y+WHITE_KEY_HEIGHT;y++){
                VBE_plot_pixel_32bpp(x,y,color);
            }
        }
    }
}


void fill_in_black_key(enum key K,u32 color){
    if (K>=WHITE_KEY_COUNT){
        const u16 pos=key_to_position[K];
        for (u16 x=pos;x<pos+BLACK_KEY_WIDTH;x++){
            for (u16 y=PIANO_POSITION_Y;y<BLACK_KEY_WIDTH;y++){
                VBE_plot_pixel_32bpp(x,y,color);
            }
        }
    }
}

void (*fill_in_key_functions[KEY_COUNT])(enum key K,u32 color)={
    fill_in_left_white_key,
    fill_in_inbetween_white_key,
    fill_in_right_white_key,
    fill_in_left_white_key,
    fill_in_inbetween_white_key,
    fill_in_inbetween_white_key,
    fill_in_right_white_key,
    
    fill_in_black_key,
    fill_in_black_key,
    fill_in_black_key,
    fill_in_black_key,
    fill_in_black_key
};

void fill_in_key(enum key K,u32 color){
    if ((K>=0) & (K<=KEY_COUNT)){
        fill_in_key_functions[K](K,color);
    }
}

void VBE_put_piano(){
    VBE_put_piano_outline();
    fill_in_key(D,WHITE);
}



