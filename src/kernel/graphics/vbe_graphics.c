#include "vbe_graphics.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

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

#define CURSOR_BITMAP_INDEX 63

static enum basic_color{
    WHITE=0x00FFFFFF,
    SILVER=0x00C0C0C0,
    GRAY=0x808080,
    BLACK=0x000000,
    RED=0xFF0000,
    MAROON=0x800000,
    YELLOW=0xFFFF00,
    OLIVE=0x808000,
    LIME=0x00FF00,
    GREEN=0x008000,
    AQUA=0x00FFFF,
    TEAL=0x008080,
    BLUE=0x0000FF,
    NAVY=0x000080,
    FUCHSIA=0xFF00FF,
    PURPLE=0x800080,
};

extern int font8x8_basic[95][8];
extern void* fb;
extern u32 pitch;
extern u32 flags;

u16 cursor_x=TEXT_START_X;
u16 cursor_y=TEXT_START_Y;

u16 input_start_x=TEXT_START_X;
u16 input_start_y=TEXT_START_Y;

u32 char_color=RED;

bool writing_mode=0;

char file_text[1024];
int file_text_index=0;


print_file_text(){
    u16 x=cursor_x+8;
    u16 y=cursor_y;
    for (u16 i=0;i<file_text_index;i++){
        put_char(file_text[i],x,y,FUCHSIA);
        x+=8;
    }
}
static inline void plot_pixel_32bpp(u32 x,u32 y,u32 color){
    if ((x<=SCREEN_WIDTH) & (y<=SCREEN_HEIGHT)){
        u32 *pixel = fb + pitch * y + 4 * x;
        *pixel = color;
    }
}

void put_char(char c,u16 x,u16 y,u32 color){
    u8 row, col;
    for (row = 0; row < CHAR_WIDTH; row++) {
        for (col = 0; col < CHAR_HEIGHT; col++) {
            if (font8x8_basic[c-32][row] & (1 << col)) {
                plot_pixel_32bpp(col+x,row+y,color);
            }
        }
    }
}

static inline void delete_char(){
    for (u8 row = 0; row < CHAR_WIDTH; row++) {
        for (u8 col = 0; col < CHAR_HEIGHT; col++) {
            plot_pixel_32bpp(col+cursor_x,row+cursor_y,BLACK);
        }
    }
}

static inline void put_header_text(char *str,u32 color){
    u16 header_cursor_x=TEXT_START_X;
    u16 header_cursor_y=TERMINAL_HEADER_TEXT_Y;
    for (u16 i=0;i<strlen(str);i++){
            if (header_cursor_x<=LATERAL_TERMINAL_BORDER_SIZE+TERMINAL_WIDTH/CHAR_WIDTH*CHAR_WIDTH-CHAR_WIDTH)
            put_char(str[i],header_cursor_x,header_cursor_y,color);
            header_cursor_x+=CHAR_WIDTH;
        }
}



void put_string_8x(char *str,u32 color){
    u16 banner_cursor_x=BANNER_X;
    u16 banner_cursor_y=BANNER_Y;
    for (u32 i=0;i<strlen(str);i++){
        if (banner_cursor_x<=LATERAL_TERMINAL_BORDER_SIZE+TERMINAL_WIDTH){
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
        }
        banner_cursor_x+=CHAR_WIDTH*8;
    }
}

void put_banner(){
    put_string_8x("BasicOS",YELLOW);
}


void put_char_terminal(char c){
    if (writing_mode==1){
        file_text[file_text_index]=c;
        file_text_index++;
    }
    if (c!=10){
    delete_char();
    put_char(c,cursor_x,cursor_y,char_color);
    cursor_x+=CHAR_WIDTH;
    if (cursor_x>=LATERAL_TERMINAL_BORDER_SIZE+TERMINAL_WIDTH/8*8-CHAR_WIDTH){
        cursor_x=TEXT_START_X;
        if (cursor_y>=(VERTICAL_TERMINAL_BORDER_SIZE+TERMINAL_HEIGHT-CHAR_HEIGHT*2-TERMINAL_PADDING_Y)){
            scroll_down();
        }
        else cursor_y+=CHAR_HEIGHT;
    }
    put_cursor();
    }
}

void set_newline(){
    delete_char();
        cursor_x=TEXT_START_X;
        if (cursor_y>=(VERTICAL_TERMINAL_BORDER_SIZE+TERMINAL_HEIGHT-CHAR_HEIGHT*2-TERMINAL_PADDING_Y)){
            scroll_down();
        } else{
            cursor_y+=CHAR_HEIGHT;
        }
    put_cursor();
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
    put_header_text(" TERMINAL (version 0.1)",LIME);
}

void put_cursor(){
    put_char('_',cursor_x,cursor_y,SILVER);
}


void put_string_terminal(char *str){
    for (u32 i=0;i<strlen(str);i++){
            put_char_terminal(str[i]);
        }
}





void delete_char_terminal(){
    if (writing_mode==1){
        file_text_index--;
    }
    if (cursor_y<=input_start_y & cursor_x<=input_start_x){
    }
    else if (cursor_y>=input_start_y & cursor_x<=input_start_x){
            delete_char();
            cursor_y-=CHAR_HEIGHT;
            cursor_x=LATERAL_TERMINAL_BORDER_SIZE+TERMINAL_PADDING_X+TERMINAL_WIDTH/CHAR_WIDTH*CHAR_WIDTH-CHAR_WIDTH;
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
    for (int y=input_start_y;y<SCREEN_HEIGHT-VERTICAL_TERMINAL_BORDER_SIZE-CHAR_HEIGHT;y++){
        for (int x=input_start_x;x<SCREEN_WIDTH-LATERAL_TERMINAL_BORDER_SIZE-TERMINAL_PADDING_X;x++){
            u32* pixel=fb + pitch * y + 4 * x;
            u32* pixel_down=fb + pitch * (y+CHAR_HEIGHT) + 4 * x;
            *pixel=*pixel_down;
            pixel_down=NULL;
        }
    }
}

void delete_terminal_text(){
    for (int y=TEXT_START_Y;y<SCREEN_HEIGHT-VERTICAL_TERMINAL_BORDER_SIZE-CHAR_HEIGHT;y++){
        for (int x=TEXT_START_X;x<SCREEN_WIDTH-LATERAL_TERMINAL_BORDER_SIZE-TERMINAL_PADDING_X;x++){
            u32* pixel=fb + pitch * y + 4 * x;
            *pixel=BLACK;
        }
    }
}

void delete_header_text(){
    for (int y=TERMINAL_HEADER_TEXT_Y;y<VERTICAL_TERMINAL_BORDER_SIZE+TERMINAL_HEADER_HEIGHT;y++){
        for (int x=TERMINAL_HEADER_TEXT_X;x<LATERAL_TERMINAL_BORDER_SIZE+TERMINAL_WIDTH;x++){
            u32* pixel=fb + pitch * y + 4 * x;
            *pixel=BLACK;
        }
    }
}


void initialize_main_window(){
    put_banner();
    put_terminal(SILVER);
}


void switch_menu_window(){
    writing_mode=0;
    delete_terminal_text();
    delete_header_text();
    put_header_text("Main Menu",LIME);
    cursor_x=TEXT_START_X;
    cursor_y=TEXT_START_Y;
    char_color=LIME;
    set_newline();
    put_string_terminal("Write File: W");
    set_newline();
    set_newline();
    put_string_terminal("Check Files: F");
    set_newline();
    set_newline();
    put_string_terminal("Select and print image: I");
    delete_char();
}

void switch_file_write_window(){
    delete_terminal_text();
    delete_header_text();
    put_header_text("File writing mode: Start with the file name ( <=11 characters )",LIME);
    cursor_x=TEXT_START_X;
    cursor_y=TEXT_START_Y;
    put_string_terminal("CTRL+Q: quit   CTRL+S: save");
    delete_char();
    change_char_color(RED);
    input_start_y=TEXT_START_Y+2*CHAR_HEIGHT;
    cursor_x=TEXT_START_X;
    cursor_y=input_start_y;
    put_cursor();
    writing_mode=1;
}

void switch_file_check_window(){
    writing_mode=0;
    delete_terminal_text();
    delete_header_text();
    put_header_text("Files",LIME);
}

void quit(){
    file_text_index=0;
    switch_menu_window();
}

void save_file(){
    switch_menu_window();
}


void test_interface(){
    initialize_main_window();
    switch_menu_window();
    
    
}













