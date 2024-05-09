#include "vbe_graphics.h"

struct graphics_info graphics_info_init(struct multiboot_info* multiboot_info){
    struct graphics_info graphics_info;
    graphics_info.cursor_x=8;
    graphics_info.cursor_y=8;
    graphics_info.screen_width=760;
    graphics_info.screen_height=1080;
    u32 color;
    void *fb = (void*)multiboot_info->framebuffer.address;
    
		switch (multiboot_info->framebuffer.type)
		{
			case MULTIBOOT_INFO_FRAMEBUFFER_TYPE_INDEXED:
				{
					unsigned best_distance, distance;
					struct multiboot_color *palette = multiboot_info->framebuffer_palette_address;

					color = 0;
					best_distance = 4*256*256;

					for (u32 i = 0; i < multiboot_info->framebuffer_palette_num_colors; i++)
					{
						distance = (0xff - palette[i].blue) * (0xff - palette[i].blue)
							+ palette[i].red * palette[i].red
							+ palette[i].green * palette[i].green;
						if (distance < best_distance)
						{
							color = i;
							best_distance = distance;
						}
					}
				}
				break;

			case MULTIBOOT_INFO_FRAMEBUFFER_TYPE_RGB:
				color = ((1 << multiboot_info->framebuffer_blue_mask_size) - 1)
					<< multiboot_info->framebuffer_blue_field_position;
				break;

			case MULTIBOOT_INFO_FRAMEBUFFER_TYPE_EGA_TEXT:
				color = '\\' | 0x0100;
				break;

			default:
				color = 0xffffffff;
				break;
		}
		
    graphics_info.color=color;
    graphics_info.fb=fb;
    
    return graphics_info;
}

void plot_diagonal_line(struct multiboot_info * multiboot_info,struct graphics_info graphics_info){
		
    for (u32 i = 0; i < multiboot_info->framebuffer.width && i < multiboot_info->framebuffer.height; i++){
        switch (multiboot_info->framebuffer.bpp){
                case 8:{
                        u8 *pixel = graphics_info.fb + multiboot_info->framebuffer.pitch * i + i;
                        *pixel = graphics_info.color;
                    }break;
                case 15:
                case 16:{
                        u16 *pixel = graphics_info.fb + multiboot_info->framebuffer.pitch * i + 2 * i;
                        *pixel = graphics_info.color;
                    }break;
                case 24:{
                        u32 *pixel = graphics_info.fb + multiboot_info->framebuffer.pitch * i + 3 * i;
                        *pixel = (graphics_info.color & 0xffffff) | (*pixel & 0xff000000);
                    }break;
                case 32:{
                        u32 *pixel = graphics_info.fb + multiboot_info->framebuffer.pitch * i + 4 * i;
                        *pixel = graphics_info.color;
                    }break;
                }
    }
}

void plot_pixel_8bpp(struct multiboot_info * multiboot_info,struct graphics_info graphics_info,u32 x,u32 y){
    if (x<=multiboot_info->framebuffer.width & y<=multiboot_info->framebuffer.height & x>=0 & y>=0){
        u8 *pixel = graphics_info.fb + multiboot_info->framebuffer.pitch * x + y;
        *pixel = graphics_info.color;
    }
}

void plot_pixel_15bpp(struct multiboot_info * multiboot_info,struct graphics_info graphics_info,u32 x,u32 y){
    if (x<=multiboot_info->framebuffer.width & y<=multiboot_info->framebuffer.height & x>=0 & y>=0){
        u16 *pixel = graphics_info.fb + multiboot_info->framebuffer.pitch * x + y;
        *pixel = graphics_info.color;
    }
}

void plot_pixel_16bpp(struct multiboot_info * multiboot_info,struct graphics_info graphics_info,u32 x,u32 y){
    if (x<=multiboot_info->framebuffer.width & y<=multiboot_info->framebuffer.height & x>=0 & y>=0){
        u16 *pixel = graphics_info.fb + multiboot_info->framebuffer.pitch * x + 2 * y;
        *pixel = graphics_info.color;
    }
}

void plot_pixel_24bpp(struct multiboot_info * multiboot_info,struct graphics_info graphics_info,u32 x,u32 y){
    if (x<=multiboot_info->framebuffer.width & y<=multiboot_info->framebuffer.height & x>=0 & y>=0){
        u32 *pixel = graphics_info.fb + multiboot_info->framebuffer.pitch * x + 3 * y;
        *pixel = (graphics_info.color & 0xffffff) | (*pixel & 0xff000000);
    }
}

void plot_pixel_32bpp(struct multiboot_info * multiboot_info,struct graphics_info graphics_info,u32 x,u32 y){
    if (x<=multiboot_info->framebuffer.width & y<=multiboot_info->framebuffer.height & x>=0 & y>=0){
        u32 *pixel = graphics_info.fb + multiboot_info->framebuffer.pitch * x + 4 * y;
        *pixel = graphics_info.color;
    }
}

void switch_color_32bpp(struct graphics_info* graphics_info,enum basic_color color){
    switch (color){
        case WHITE:
            graphics_info->color=0x00FFFFFF;break;
        case SILVER:
            graphics_info->color=0x00C0C0C0;break;
        case GRAY:
            graphics_info->color=0x00808080;break;
        case BLACK:
            graphics_info->color=0x00000000;break;
        case RED:
            graphics_info->color=0x00FF0000;break;
        case MAROON:
            graphics_info->color=0x00800000;break;
        case YELLOW:
            graphics_info->color=0x00FFFF00;break;
        case OLIVE:
            graphics_info->color=0x00808000;break;
        case LIME:
            graphics_info->color=0x0000FF00;break;
        case GREEN:
            graphics_info->color=0x00008000;break;
        case AQUA:
            graphics_info->color=0x0000FFFF;break;
        case TEAL:
            graphics_info->color=0x00008080;break;
        case BLUE:
            graphics_info->color=0x000000FF;break;
        case NAVY:
            graphics_info->color=0x00000080;break;
        case FUCHSIA:
            graphics_info->color=0x00FF00FF;break;
        case PURPLE:
            graphics_info->color=0x00800080;break;
    }
}


u8 get_index_from_char(char c){
    for (u8 i=0;i<=127;i++){
        if (characters[i]==c) return i;
    }
    return -1;
}


void put_char(struct multiboot_info * multiboot_info,struct graphics_info* graphics_info,char c){
    u8 row, col;
    u8 index=get_index_from_char(c);
    for (row = 0; row < 8; row++) {
        for (col = 0; col < 8; col++) {
            if (font8x8_basic[index][row] & (1 << col)) {
                plot_pixel_32bpp(multiboot_info,*graphics_info,row+graphics_info->cursor_y,col+graphics_info->cursor_x);
            }
        }
    }
    if ((760 - graphics_info->cursor_x ) <= 8){
        graphics_info->cursor_x=8;
        if ((graphics_info->screen_height - graphics_info->cursor_y ) > 8) graphics_info->cursor_y+=8;
    } else {
        graphics_info->cursor_x+=8;
    }
    
}

void put_char_banner(struct multiboot_info * multiboot_info,struct graphics_info* graphics_info,char c){
    u8 row, col;
    u8 index=get_index_from_char(c);
    for (row = 0; row < 8; row++) {
        for (col = 0; col < 8; col++) {
            if (font8x8_basic[index][row] & (1 << col)) {
                for (int i=1;i<4;i++){
                    plot_pixel_32bpp(multiboot_info,*graphics_info,4*(row+graphics_info->cursor_y),4*(col+graphics_info->cursor_x));
                    plot_pixel_32bpp(multiboot_info,*graphics_info,4*(row+graphics_info->cursor_y)+i,4*(col+graphics_info->cursor_x));
                    plot_pixel_32bpp(multiboot_info,*graphics_info,4*(row+graphics_info->cursor_y),4*(col+graphics_info->cursor_x)+i);
                    plot_pixel_32bpp(multiboot_info,*graphics_info,4*(row+graphics_info->cursor_y)+i,4*(col+graphics_info->cursor_x)+i);
                }
            }
        }
    }
    
    graphics_info->cursor_x+=16;
}



void put_string(struct multiboot_info * multiboot_info,struct graphics_info graphics_info,char *str){
    for (u32 i=0;i<strlen(str);i++){
        put_char(multiboot_info,&graphics_info,str[i]);
    }
}

void put_banner(struct multiboot_info * multiboot_info,struct graphics_info graphics_info,char *str){
    for (u32 i=0;i<strlen(str);i++){
        put_char_banner(multiboot_info,&graphics_info,str[i]);
    }
}












