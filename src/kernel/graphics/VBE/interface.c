#include "interface.h"

#define LATERAL_TERMINAL_BORDER_SIZE 110
#define VERTICAL_TERMINAL_BORDER_SIZE 110

#define TERMINAL_WIDTH SCREEN_WIDTH-2*LATERAL_TERMINAL_BORDER_SIZE
#define TERMINAL_HEIGHT SCREEN_HEIGHT-2*VERTICAL_TERMINAL_BORDER_SIZE
#define TERMINAL_PADDING_Y 6
#define TERMINAL_PADDING_X 3
#define TERMINAL_HEADER_WIDTH TERMINAL_WIDTH
#define TERMINAL_HEADER_HEIGHT CHAR_HEIGHT+2*TERMINAL_PADDING_Y
#define TERMINAL_HEADER_TEXT_Y VERTICAL_TERMINAL_BORDER_SIZE+TERMINAL_PADDING_Y
#define TERMINAL_HEADER_TEXT_X LATERAL_TERMINAL_BORDER_SIZE+TERMINAL_PADDING_X

#define TEXT_START_X LATERAL_TERMINAL_BORDER_SIZE+TERMINAL_PADDING_X
#define TEXT_START_Y VERTICAL_TERMINAL_BORDER_SIZE+TERMINAL_HEADER_HEIGHT+TERMINAL_PADDING_Y


#define BANNER_X LATERAL_TERMINAL_BORDER_SIZE // Banner must be above the terminal
#define BANNER_Y VERTICAL_TERMINAL_BORDER_SIZE-68

#define CURSOR_BITMAP_INDEX 63

u32 char_color=RED;

extern u32 flags;

u16 cursor_x=TEXT_START_X;
u16 cursor_y=TEXT_START_Y;

u16 input_start_x=TEXT_START_X;
u16 input_start_y=TEXT_START_Y;

enum VBE_window current_window=MENU_WINDOW;


static inline void VBE_put_header_text(char *str,u32 color){
    u16 header_cursor_x=TERMINAL_HEADER_TEXT_X;
    u16 header_cursor_y=TERMINAL_HEADER_TEXT_Y;
    for (u16 i=0;i<strlen(str);i++){
            if (header_cursor_x<=LATERAL_TERMINAL_BORDER_SIZE+TERMINAL_WIDTH/CHAR_WIDTH*CHAR_WIDTH-CHAR_WIDTH)
            VBE_put_char(str[i],header_cursor_x,header_cursor_y,color);
            header_cursor_x+=CHAR_WIDTH;
        }
}


static inline void VBE_put_string_8x(char *str,u32 color){
    u16 banner_cursor_x=BANNER_X;
    u16 banner_cursor_y=BANNER_Y;
    for (u32 i=0;i<strlen(str);i++){
        if (banner_cursor_x<=LATERAL_TERMINAL_BORDER_SIZE+TERMINAL_WIDTH){
            for (u8 row = 0; row < CHAR_WIDTH; row++) {
                for (u8 col = 0; col < CHAR_HEIGHT; col++) {
                    if (font8x8_basic[str[i]-32][row] & (1 << col)) {
                        for (int x=0;x<8;x++){
                            for (int y=0;y<8;y++){
                                VBE_plot_pixel_32bpp(8*col+banner_cursor_x+x,8*row+banner_cursor_y+y,color);
                            }
                        }
                    }
                }
            }
        }
        banner_cursor_x+=CHAR_WIDTH*8;
    }
}

static inline void VBE_scroll_down(){
    for (int y=input_start_y;y<SCREEN_HEIGHT-VERTICAL_TERMINAL_BORDER_SIZE-CHAR_HEIGHT;y++){
        for (int x=input_start_x;x<SCREEN_WIDTH-LATERAL_TERMINAL_BORDER_SIZE-TERMINAL_PADDING_X;x++){
            u32* pixel=(u8*)fb + pitch * y + 4 * x;
            u32* pixel_down=(u8*)fb + pitch * (y+CHAR_HEIGHT) + 4 * x;
            *pixel=*pixel_down;
            pixel_down=NULL;
        }
    }
}

static inline void VBE_put_banner(){
    VBE_put_string_8x("BasicOS",YELLOW);
}

static inline void VBE_put_cursor(){
    VBE_put_char('_',cursor_x,cursor_y,SILVER);
}


void VBE_put_char_terminal(char c){
    if (c!=10){
    VBE_delete_char(cursor_x,cursor_y);
    VBE_put_char(c,cursor_x,cursor_y,char_color);
    cursor_x+=CHAR_WIDTH;
    if (cursor_x>=LATERAL_TERMINAL_BORDER_SIZE+TERMINAL_WIDTH/8*8-CHAR_WIDTH){
        cursor_x=TEXT_START_X;
        if (cursor_y>=(VERTICAL_TERMINAL_BORDER_SIZE+TERMINAL_HEIGHT-CHAR_HEIGHT*2-TERMINAL_PADDING_Y)){
            VBE_scroll_down();
        }
        else cursor_y+=CHAR_HEIGHT;
    }
    VBE_put_cursor();
    }
}

void VBE_set_newline(){
    VBE_delete_char(cursor_x,cursor_y);
        cursor_x=TEXT_START_X;
        if (cursor_y>=(VERTICAL_TERMINAL_BORDER_SIZE+TERMINAL_HEIGHT-CHAR_HEIGHT*2-TERMINAL_PADDING_Y)){
            VBE_scroll_down();
        } else{
            cursor_y+=CHAR_HEIGHT;
        }
    VBE_put_cursor();
}

static inline void VBE_change_char_color(u32 new_color){
    char_color=new_color;
}


void static inline VBE_put_terminal(u32 color){
    for (u16 x=LATERAL_TERMINAL_BORDER_SIZE;x<(SCREEN_WIDTH-LATERAL_TERMINAL_BORDER_SIZE);x++){
        VBE_plot_pixel_32bpp(x,VERTICAL_TERMINAL_BORDER_SIZE,color);
        VBE_plot_pixel_32bpp(x,VERTICAL_TERMINAL_BORDER_SIZE+TERMINAL_HEADER_HEIGHT,color);
        VBE_plot_pixel_32bpp(x,VERTICAL_TERMINAL_BORDER_SIZE+TERMINAL_HEIGHT,color);
    }
    
    for (u16 y=VERTICAL_TERMINAL_BORDER_SIZE;y<(SCREEN_HEIGHT-VERTICAL_TERMINAL_BORDER_SIZE);y++){
        VBE_plot_pixel_32bpp(LATERAL_TERMINAL_BORDER_SIZE,y,color);
        VBE_plot_pixel_32bpp(LATERAL_TERMINAL_BORDER_SIZE+TERMINAL_WIDTH,y,color);
    }
    VBE_put_header_text(" TERMINAL (version 0.1)",LIME);
}



void  VBE_put_string_terminal(char *str){
    for (u32 i=0;i<strlen(str);i++){
            VBE_put_char_terminal(str[i]);
        }
}


void VBE_delete_char_terminal(){
    if ((cursor_y<=input_start_y) & (cursor_x<=input_start_x)){
    }
    else if ((cursor_y>=input_start_y) & (cursor_x<=input_start_x)){
            VBE_delete_char(cursor_x,cursor_y);
            cursor_y-=CHAR_HEIGHT;
            cursor_x=LATERAL_TERMINAL_BORDER_SIZE+TERMINAL_PADDING_X+TERMINAL_WIDTH/CHAR_WIDTH*CHAR_WIDTH-CHAR_WIDTH;
            cursor_x-=CHAR_WIDTH;
            VBE_delete_char(cursor_x,cursor_y);
        }
    else if ((cursor_y>=input_start_y) & (cursor_x>=input_start_x)){
        VBE_delete_char(cursor_x,cursor_y);
        cursor_x-=CHAR_WIDTH;
        VBE_delete_char(cursor_x,cursor_y);
    }
    else if ((cursor_y<=input_start_y) & (cursor_x>=input_start_x)){
        VBE_delete_char(cursor_x,cursor_y);
        cursor_x-=CHAR_WIDTH;
        VBE_delete_char(cursor_x,cursor_y);
    }
    VBE_put_cursor();
}



static inline void VBE_delete_terminal_text(){
    for (int y=TEXT_START_Y;y<SCREEN_HEIGHT-VERTICAL_TERMINAL_BORDER_SIZE-CHAR_HEIGHT;y++){
        for (int x=TEXT_START_X;x<SCREEN_WIDTH-LATERAL_TERMINAL_BORDER_SIZE-TERMINAL_PADDING_X;x++){
            u32* pixel=(u8*)fb + pitch * y + 4 * x;
            *pixel=BLACK;
        }
    }
}

static inline void VBE_delete_header_text(){
    for (int y=TERMINAL_HEADER_TEXT_Y;y<VERTICAL_TERMINAL_BORDER_SIZE+TERMINAL_HEADER_HEIGHT;y++){
        for (int x=TERMINAL_HEADER_TEXT_X;x<LATERAL_TERMINAL_BORDER_SIZE+TERMINAL_WIDTH;x++){
            u32* pixel=(u8*)fb + pitch * y + 4 * x;
            *pixel=BLACK;
        }
    }
}


static inline void VBE_initialize_main_window(){
    VBE_put_banner();
    VBE_put_terminal(SILVER);
}


void VBE_switch_menu_window(){
    current_window=MENU_WINDOW;
    VBE_delete_terminal_text();
    VBE_delete_header_text();
    VBE_put_header_text(" Main Menu",LIME);
    cursor_x=TEXT_START_X;
    cursor_y=TEXT_START_Y;
    char_color=LIME;
    VBE_set_newline();
    VBE_put_string_terminal(" Play piano: CTRL+P");
    VBE_set_newline();
    VBE_set_newline();
    VBE_put_string_terminal(" Print image: CTRL+I");
    VBE_set_newline();
    VBE_set_newline();
    VBE_put_string_terminal(" Text input: CTRL+T");
    VBE_delete_char(cursor_x,cursor_y);
}

void VBE_switch_text_input_window(){
    current_window=TEXT_INPUT_WINDOW;
    VBE_delete_terminal_text();
    VBE_delete_header_text();
    VBE_put_header_text(" Text Input",LIME);
    char_color=FUCHSIA;
    cursor_x=TEXT_START_X;
    cursor_y=TEXT_START_Y;
    VBE_put_cursor();
}

void VBE_switch_piano_window(){
    current_window=PIANO_WINDOW;
    VBE_delete_terminal_text();
    VBE_delete_header_text();
    VBE_put_header_text(" Piano (12 notes) ",LIME);
    VBE_put_piano();
}

void VBE_switch_image_window(){
    current_window=IMAGE_WINDOW;
    VBE_delete_terminal_text();
    VBE_delete_header_text();
    VBE_put_header_text(" Bird image",LIME);
    VBE_print_picture();
}

void VBE_quit(){
    VBE_switch_menu_window();
}


void VBE_test_interface(){
    if (flags>>12==1 && bpp==32){
    VBE_initialize_main_window();
    VBE_switch_menu_window();
    VBE_switch_piano_window();
    VBE_quit();
    VBE_switch_text_input_window();
    VBE_put_string_terminal("ab");
    VBE_delete_char_terminal();
    VBE_delete_char_terminal();
    VBE_delete_char_terminal();
    VBE_put_char_terminal('z');
    VBE_switch_image_window();
    }
}
