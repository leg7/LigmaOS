#include "vbe_graphics.h"

extern font8x8_basic[95][8];
extern void* fb;
extern u32 pitch;
extern u32 flags;

u16 cursor_x=TERMINAL_HEADER_TEXT_X;
u16 cursor_y=TERMINAL_HEADER_TEXT_Y;

u32 input_start_y=TEXT_START_Y;
u32 input_start_x=TEXT_START_X;

u32 char_color=YELLOW;

bool line_is_new=0;





void plot_pixel_32bpp(u32 x,u32 y,u32 color){
    if ((x<=SCREEN_WIDTH) & (y<=SCREEN_HEIGHT)){
        u32 *pixel = fb + pitch * y + 4 * x;
        *pixel = color;
    }
}

void put_banner(char *str,u32 color){
    
    u32 banner_cursor_x=BANNER_X;
    u32 banner_cursor_y=BANNER_Y;
    for (u32 i=0;i<strlen(str);i++){
        for (u8 row = 0; row < CHAR_WIDTH; row++) {
            for (u8 col = 0; col < CHAR_HEIGHT; col++) {
                if (font8x8_basic[str[i]-32][row] & (1 << col)) {
                    for (int x=0;x<8;x++){
                        for (int y=0;y<8;y++){
                            plot_pixel_32bpp(8*col+banner_cursor_x+x,8*row+banner_cursor_y+y,color);
                        }
                    }
                }
            }
        }
        banner_cursor_x+=64;
    }
    
}


void put_char(char c){
    if (c==10){
        cursor_y+=CHAR_WIDTH;
    } else {
        u8 row, col;
        for (row = 0; row < CHAR_WIDTH; row++) {
            for (col = 0; col < CHAR_HEIGHT; col++) {
                if (font8x8_basic[c-32][row] & (1 << col)) {
                    plot_pixel_32bpp(col+cursor_x,row+cursor_y,char_color);
                }
            }
        }
        if (cursor_x>=(LATERAL_TERMINAL_BORDER_SIZE+TERMINAL_WIDTH-CHAR_WIDTH*2)){
            input_start_x-=CHAR_WIDTH;
            cursor_x=input_start_x;
            cursor_y+=CHAR_HEIGHT;
            line_is_new=0;
        }
        else{
            cursor_x+=CHAR_WIDTH;
        }
        if (cursor_y>=(VERTICAL_TERMINAL_BORDER_SIZE+TERMINAL_HEIGHT-CHAR_HEIGHT-TERMINAL_PADDING_Y)){
            scroll_down();
        }
    }
}

void put_string(char *str){
    for (u32 i=0;i<strlen(str);i++){
            put_char(str[i]);
        }
}

void change_char_color(u32 new_color){
    char_color=new_color;
}


void put_terminal(u32 color){
    for (u16 x=LATERAL_TERMINAL_BORDER_SIZE;x<(SCREEN_WIDTH-LATERAL_TERMINAL_BORDER_SIZE);x++){
        plot_pixel_32bpp(x,VERTICAL_TERMINAL_BORDER_SIZE,color);
        plot_pixel_32bpp(x,VERTICAL_TERMINAL_BORDER_SIZE+TERMINAL_HEADER_HEIGHT,color);
        plot_pixel_32bpp(x,VERTICAL_TERMINAL_BORDER_SIZE+TERMINAL_HEIGHT,color);
    }
    
    for (u16 y=VERTICAL_TERMINAL_BORDER_SIZE;y<(SCREEN_HEIGHT-VERTICAL_TERMINAL_BORDER_SIZE);y++){
        plot_pixel_32bpp(LATERAL_TERMINAL_BORDER_SIZE,y,color);
        plot_pixel_32bpp(LATERAL_TERMINAL_BORDER_SIZE+TERMINAL_WIDTH,y,color);
    }
    
    change_char_color(LIME);
    put_string(" TERMINAL (version 0.1)");
    cursor_x=TEXT_START_X;
    cursor_y=TEXT_START_Y;
    cursor_x-=CHAR_WIDTH;
    put_char('$');
    
}

void put_cursor(){
    u32 temp_color=char_color;
    change_char_color(SILVER);
    u8 row, col;
    for (row = 0; row < CHAR_WIDTH; row++) {
        for (col = 0; col < CHAR_HEIGHT; col++) {
            if (font8x8_basic[CURSOR_BITMAP_INDEX][row] & (1 << col)) {
                plot_pixel_32bpp(col+cursor_x,row+cursor_y,char_color);
            }
        }
    }
    change_char_color(temp_color);
}

void delete_char(){
    for (u8 row = 0; row < CHAR_WIDTH; row++) {
        for (u8 col = 0; col < CHAR_HEIGHT; col++) {
            plot_pixel_32bpp(col+cursor_x,row+cursor_y,BLACK);
        }
    }
    
}

void put_char_terminal(char c){
    delete_char();
    put_char(c);
    put_cursor();
}

void put_string_terminal(char *str){
    for (u32 i=0;i<strlen(str);i++){
            put_char_terminal(str[i]);
        }
}



void set_newline(){
    line_is_new=1;
    delete_char();
    cursor_x=TEXT_START_X-CHAR_WIDTH;
    cursor_y+=CHAR_HEIGHT;
    u32 temp_char_color=char_color;
    char_color=LIME;
    put_char('$');
    char_color=temp_char_color;
    put_cursor();
    input_start_y=cursor_y;
}



void delete_char_terminal(){
    if (cursor_y<=input_start_y & cursor_x<=input_start_x){
    }
    else if (cursor_y>=input_start_y & cursor_x<=input_start_x){
            delete_char();
            cursor_y-=CHAR_HEIGHT;
            if (cursor_y==input_start_y){
                line_is_new=1;
            }
            cursor_x=LATERAL_TERMINAL_BORDER_SIZE+TERMINAL_WIDTH-TERMINAL_PADDING_X;
            cursor_x-=CHAR_WIDTH;
            delete_char();
        }
    else if (cursor_y>=input_start_y & cursor_x>=input_start_x){
        delete_char();
        cursor_x-=CHAR_WIDTH;
        delete_char();
    }
    else if (cursor_y<=input_start_y & cursor_x>=input_start_x){
        delete_char();
        cursor_x-=CHAR_WIDTH;
        delete_char();
    }
    put_cursor();
}


void scroll_down(){
    cursor_y-=CHAR_HEIGHT;
    for (int y=TEXT_START_Y;y<LATERAL_TERMINAL_BORDER_SIZE+TERMINAL_HEIGHT-TEXT_START_Y;y++){
        for (int x=TEXT_START_X-CHAR_WIDTH;x<VERTICAL_TERMINAL_BORDER_SIZE+TERMINAL_WIDTH;x++){
            u32* pixel=fb + pitch * y + 4 * x;
            u32* pixel_down=fb + pitch * (y+CHAR_HEIGHT) + 4 * x;
            if ((pixel!=NULL) & (pixel_down!=NULL)){
                *pixel=*pixel_down;
                pixel_down=NULL;
            }
        }
    }
}


void put_main_window(){
    
    put_banner("BasicOS",YELLOW);
    put_terminal(SILVER);
    change_char_color(RED);
    put_cursor();
    put_string_terminal("Command_1");
    set_newline();
    put_string_terminal("Command_2");
  
   
   
    
    
    
    
    
    
    
}












