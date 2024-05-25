#include "piano.h"
#include <stdio.h>

/*#define PIANO_OUTLINE_COLOR BLUE
#define KEY_PRESS_COLOR SILVER
#define PIANO_POSITION_X 230
#define PIANO_POSITION_Y 280

#define WHITE_KEY_WIDTH 80
#define WHITE_KEY_HEIGHT 200
#define BLACK_KEY_WIDTH 40
#define BLACK_KEY_HEIGHT 150

#define BLACK_KEY_COUNT 5
#define WHITE_KEY_COUNT 7*/


/*static enum key_position{
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
};*/





void VBE_put_piano_outline(){
    for (u16 x=PIANO_POSITION_X;x<PIANO_POSITION_X+WHITE_KEY_WIDTH*WHITE_KEY_COUNT;x++){
        VBE_plot_pixel_32bpp(x,PIANO_POSITION_Y,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(x,PIANO_POSITION_Y+WHITE_KEY_HEIGHT,PIANO_OUTLINE_COLOR);
    }

    for (u16 x=0;x<BLACK_KEY_WIDTH;x++){
        VBE_plot_pixel_32bpp(x+POSITION_C_SHARP,PIANO_POSITION_Y+BLACK_KEY_HEIGHT,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(x+POSITION_D_SHARP,PIANO_POSITION_Y+BLACK_KEY_HEIGHT,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(x+POSITION_F_SHARP,PIANO_POSITION_Y+BLACK_KEY_HEIGHT,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(x+POSITION_G_SHARP,PIANO_POSITION_Y+BLACK_KEY_HEIGHT,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(x+POSITION_A_SHARP,PIANO_POSITION_Y+BLACK_KEY_HEIGHT,PIANO_OUTLINE_COLOR);
    }

    for (u16 y=PIANO_POSITION_Y;y<PIANO_POSITION_Y+WHITE_KEY_HEIGHT;y++){
        VBE_plot_pixel_32bpp(POSITION_C,y,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(POSITION_F,y,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(POSITION_B+WHITE_KEY_WIDTH,y,PIANO_OUTLINE_COLOR);
    }

    for (u16 y=(PIANO_POSITION_Y+BLACK_KEY_HEIGHT);y<PIANO_POSITION_Y+WHITE_KEY_HEIGHT;y++){
        VBE_plot_pixel_32bpp(POSITION_D,y,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(POSITION_E,y,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(POSITION_G,y,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(POSITION_A,y,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(POSITION_B,y,PIANO_OUTLINE_COLOR);
    }

    for (u16 y=PIANO_POSITION_Y;y<PIANO_POSITION_Y+BLACK_KEY_HEIGHT+1;y++){
        VBE_plot_pixel_32bpp(POSITION_C_SHARP,y,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(POSITION_C_SHARP+BLACK_KEY_WIDTH,y,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(POSITION_D_SHARP,y,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(POSITION_D_SHARP+BLACK_KEY_WIDTH,y,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(POSITION_F_SHARP,y,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(POSITION_F_SHARP+BLACK_KEY_WIDTH,y,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(POSITION_G_SHARP,y,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(POSITION_G_SHARP+BLACK_KEY_WIDTH,y,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(POSITION_A_SHARP,y,PIANO_OUTLINE_COLOR);
        VBE_plot_pixel_32bpp(POSITION_A_SHARP+BLACK_KEY_WIDTH,y,PIANO_OUTLINE_COLOR);
    }
}

void fill_in_left_white_key(enum key_position pos,u32 color){  //white key left of black key
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

void fill_in_right_white_key(enum key_position pos,u32 color){ //white key right of black keys
    for (u16 x=pos+(BLACK_KEY_WIDTH/2)+1;x<pos+WHITE_KEY_WIDTH;x++){
        for (u16 y=PIANO_POSITION_Y+1;y<PIANO_POSITION_Y+BLACK_KEY_HEIGHT+1;y++){
            VBE_plot_pixel_32bpp(x,y,color);
        }
    }
    for (u16 x=pos+1;x<pos+WHITE_KEY_WIDTH;x++){
        for (u16 y=PIANO_POSITION_Y+BLACK_KEY_HEIGHT+1;y<PIANO_POSITION_Y+WHITE_KEY_HEIGHT;y++){
            VBE_plot_pixel_32bpp(x,y,color);
        }
    }
}
void fill_in_inbetween_white_key(enum key_position pos,u32 color){ //white key inbetween 2 black keys
    for (u16 x=pos+(BLACK_KEY_WIDTH/2)+1;x<pos+WHITE_KEY_WIDTH-(BLACK_KEY_WIDTH/2);x++){
        for (u16 y=PIANO_POSITION_Y+1;y<PIANO_POSITION_Y+BLACK_KEY_HEIGHT+1;y++){
            VBE_plot_pixel_32bpp(x,y,color);
        }
    }
    for (u16 x=pos+1;x<pos+WHITE_KEY_WIDTH;x++){
        for (u16 y=PIANO_POSITION_Y+BLACK_KEY_HEIGHT+1;y<PIANO_POSITION_Y+WHITE_KEY_HEIGHT;y++){
            VBE_plot_pixel_32bpp(x,y,color);
        }
    }
}


void fill_in_black_key(enum key_position pos,u32 color){
        for (u16 x=pos+1;x<pos+BLACK_KEY_WIDTH;x++){
            for (u16 y=PIANO_POSITION_Y+1;y<PIANO_POSITION_Y+BLACK_KEY_HEIGHT;y++){
                VBE_plot_pixel_32bpp(x,y,color);
            }
        }
}


void fill_in_key(enum key_position pos, u32 color){
	switch(pos) {
		case POSITION_C: fill_in_left_white_key     (POSITION_C, color) ; break;
		case POSITION_D: fill_in_inbetween_white_key(POSITION_D, color) ; break;
		case POSITION_E: fill_in_right_white_key    (POSITION_E, color) ; break;
		case POSITION_F: fill_in_left_white_key     (POSITION_F, color) ; break;
		case POSITION_G: fill_in_inbetween_white_key(POSITION_G, color) ; break;
		case POSITION_A: fill_in_inbetween_white_key(POSITION_A, color) ; break;
		case POSITION_B: fill_in_right_white_key    (POSITION_B, color) ; break;

		case POSITION_C_SHARP: fill_in_black_key(POSITION_C_SHARP, color); break;
		case POSITION_D_SHARP: fill_in_black_key(POSITION_D_SHARP, color); break;
		case POSITION_F_SHARP: fill_in_black_key(POSITION_F_SHARP, color); break;
		case POSITION_G_SHARP: fill_in_black_key(POSITION_G_SHARP, color); break;
		case POSITION_A_SHARP: fill_in_black_key(POSITION_A_SHARP, color); break;

		default: printf("unhandeled case in fill key!\n");
	}
}

void VBE_press_key(enum key_position pos){
    fill_in_key(pos,KEY_PRESS_COLOR);
}

void VBE_release_key(enum key_position pos){
    if ((pos-PIANO_POSITION_X)%WHITE_KEY_WIDTH==0){
    fill_in_key(pos,WHITE);
    } else {
        fill_in_key(pos,BLACK);
    }
}

void VBE_put_piano(){
    VBE_put_piano_outline();
    fill_in_left_white_key(POSITION_C,WHITE);
    fill_in_inbetween_white_key(POSITION_D,WHITE);
    fill_in_right_white_key(POSITION_E,WHITE);
    fill_in_left_white_key(POSITION_F,WHITE);
    fill_in_inbetween_white_key(POSITION_G,WHITE);
    fill_in_inbetween_white_key(POSITION_A,WHITE);
    fill_in_right_white_key(POSITION_B,WHITE);
}





