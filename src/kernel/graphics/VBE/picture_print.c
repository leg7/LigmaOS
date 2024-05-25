#include "picture_print.h"

#define PICTURE_X  150
#define PICTURE_Y  150

extern const u32 VBE_image[1057056];
extern const u16 VBE_image_height;
extern const u16 VBE_image_width;

void VBE_print_picture(){
    for (u16 x=PICTURE_X;x<PICTURE_X+VBE_image_width;x++){
        for (u16 y=PICTURE_Y;y<PICTURE_Y+VBE_image_height;y++){
            VBE_plot_pixel_32bpp(x,y,VBE_image[(y-PICTURE_Y)*VBE_image_width+(x-PICTURE_X)]);
        }
    }
}
