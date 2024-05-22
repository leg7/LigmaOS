#include "PS2_keyboard.h"
#include <architecture/x86/chips/PS2_8042.h>
#include <library/types.h>
#include <stdio.h>

// TODO: check chat scan code set the keyboard is using
// TODO: support scan code sets 1 and 3
// TODO: support sending commands to the controller

// scancodes corresponding to qwerty keys
// keys commented out are special keys that are encoded over multiple bytes
// normal_key_released = normal_pressed code + 0xF0 + normal_pressed_code
enum scancode : u8
{
// row 0
	SCANCODE_ESCAPE          = 0x76,
	SCANCODE_F1              = 0x05,
	SCANCODE_F2              = 0x06,
	SCANCODE_F3              = 0x04,
	SCANCODE_F4              = 0x0C,
	SCANCODE_F5              = 0x03,
	SCANCODE_F6              = 0x0B,
	SCANCODE_F7              = 0x83,
	SCANCODE_F8              = 0x0A,
	SCANCODE_F9              = 0x01,
	SCANCODE_F10             = 0x09,
	SCANCODE_F11             = 0x78,
	SCANCODE_F12             = 0x07,
	// print screen
	SCANCODE_SCROLL_LOCK     = 0x7E,
	// pause

// row 1
	SCANCODE_BACKTICK        = 0x0E,
	SCANCODE_1               = 0x16,
	SCANCODE_2               = 0x1E,
	SCANCODE_3               = 0x26,
	SCANCODE_4               = 0x25,
	SCANCODE_5               = 0x2E,
	SCANCODE_6               = 0x36,
	SCANCODE_7               = 0x3D,
	SCANCODE_8               = 0x3E,
	SCANCODE_9               = 0x46,
	SCANCODE_0               = 0x45,
	SCANCODE_MINUS           = 0x4E,
	SCANCODE_EQUALS          = 0x55,
	SCANCODE_BACKSPACE       = 0x66,
	// insert
	// home
	// page up
	SCANCODE_KEYPAD_NUM_LOCK = 0x77,
	// keypad /
	SCANCODE_KEYPAD_ASTERISK = 0x7C,
	SCANCODE_KEYPAD_MINUS    = 0x7B,

// row 2
	SCANCODE_TAB             = 0x0D,
	SCANCODE_Q               = 0x15,
	SCANCODE_W               = 0x1D,
	SCANCODE_E               = 0x24,
	SCANCODE_R               = 0x2D,
	SCANCODE_T               = 0x2C,
	SCANCODE_Y               = 0x35,
	SCANCODE_U               = 0x3C,
	SCANCODE_I               = 0x43,
	SCANCODE_O               = 0x44,
	SCANCODE_P               = 0x4D,
	SCANCODE_LEFT_BRACKET    = 0x54,
	SCANCODE_RIGHT_BRACKET   = 0x5B,
	SCANCODE_BACKSLASH       = 0x5D,
	// delete
	// end
	// page down
	SCANCODE_KEYPAD_7        = 0x6C,
	SCANCODE_KEYPAD_8        = 0x75,
	SCANCODE_KEYPAD_9        = 0x7D,
	SCANCODE_KEYPAD_PLUS     = 0x79,

// row 3
	SCANCODE_CAPS_LOCK       = 0x58,
	SCANCODE_A               = 0x1C,
	SCANCODE_S               = 0x1B,
	SCANCODE_D               = 0x23,
	SCANCODE_F               = 0x2B,
	SCANCODE_G               = 0x34,
	SCANCODE_H               = 0x33,
	SCANCODE_J               = 0x3B,
	SCANCODE_K               = 0x42,
	SCANCODE_L               = 0x4B,
	SCANCODE_SEMICOLON       = 0x4C,
	SCANCODE_APOSTROPHE      = 0x52,
	SCANCODE_ENTER           = 0x5A,
	SCANCODE_KEYPAD_4        = 0x6B,
	SCANCODE_KEYPAD_5        = 0x73,
	SCANCODE_KEYPAD_6        = 0x74,

// row 4
	SCANCODE_LEFT_SHIFT      = 0x12,
	SCANCODE_Z               = 0x1A,
	SCANCODE_X               = 0x22,
	SCANCODE_C               = 0x21,
	SCANCODE_V               = 0x2A,
	SCANCODE_B               = 0x32,
	SCANCODE_N               = 0x31,
	SCANCODE_M               = 0x3A,
	SCANCODE_COMMA           = 0x41,
	SCANCODE_DOT             = 0x49,
	SCANCODE_SLASH           = 0x4A,
	SCANCODE_RIGHT_SHIFT     = 0x59,
	// cursor up
	SCANCODE_KEYPAD_1        = 0x69,
	SCANCODE_KEYPAD_2        = 0x72,
	SCANCODE_KEYPAD_3        = 0x7A,
	//keypad enter

// row 5
	SCANCODE_LEFT_CONTROL    = 0x14,
	// left gui
	SCANCODE_LEFT_ALT        = 0x11,
	SCANCODE_SPACE           = 0x29,
	// right alt
	// right gui
	// right control
	// cursor left
	// cursor down
	// cursor right
	SCANCODE_KEYPAD_0        = 0x70,
	SCANCODE_KEYPAD_DOT      = 0x71,

// non keyboard
	SCANCODE_RELEASED		 = 0xF0,
	SCANCODE_EXTENDED        = 0xE0,
};

constexpr static union keycode const scancode_to_keycode[UINT8_MAX] = {
	[SCANCODE_ESCAPE]          = { .row = 0, .column = 0 },
	[SCANCODE_F1]              = { .row = 0, .column = 1 },
	[SCANCODE_F2]              = { .row = 0, .column = 2 },
	[SCANCODE_F3]              = { .row = 0, .column = 3 },
	[SCANCODE_F4]              = { .row = 0, .column = 4 },
	[SCANCODE_F5]              = { .row = 0, .column = 5 },
	[SCANCODE_F6]              = { .row = 0, .column = 6 },
	[SCANCODE_F7]              = { .row = 0, .column = 7 },
	[SCANCODE_F8]              = { .row = 0, .column = 8 },
	[SCANCODE_F9]              = { .row = 0, .column = 9 },
	[SCANCODE_F10]             = { .row = 0, .column = 10 },
	[SCANCODE_F11]             = { .row = 0, .column = 11 },
	[SCANCODE_F12]             = { .row = 0, .column = 12 },
	// print screen
	[SCANCODE_SCROLL_LOCK]     = { .row = 0, .column = 14 },
	// pause

	[SCANCODE_BACKTICK]        = { .row = 1, .column = 0 },
	[SCANCODE_1]               = { .row = 1, .column = 1 },
	[SCANCODE_2]               = { .row = 1, .column = 2 },
	[SCANCODE_3]               = { .row = 1, .column = 3 },
	[SCANCODE_4]               = { .row = 1, .column = 4 },
	[SCANCODE_5]               = { .row = 1, .column = 5 },
	[SCANCODE_6]               = { .row = 1, .column = 6 },
	[SCANCODE_7]               = { .row = 1, .column = 7 },
	[SCANCODE_8]               = { .row = 1, .column = 8 },
	[SCANCODE_9]               = { .row = 1, .column = 9 },
	[SCANCODE_0]               = { .row = 1, .column = 10 },
	[SCANCODE_MINUS]           = { .row = 1, .column = 11 },
	[SCANCODE_EQUALS]          = { .row = 1, .column = 12 },
	[SCANCODE_BACKSPACE]       = { .row = 1, .column = 13 },
	// insert
	// home
	// page up
	[SCANCODE_KEYPAD_NUM_LOCK] = { .row = 1, .column = 17 },
	// keypad slash
	[SCANCODE_KEYPAD_ASTERISK] = { .row = 1, .column = 19 },
	[SCANCODE_KEYPAD_MINUS]    = { .row = 1, .column = 20 },

	[SCANCODE_TAB]             = { .row = 2, .column = 0 },
	[SCANCODE_Q]               = { .row = 2, .column = 1 },
	[SCANCODE_W]               = { .row = 2, .column = 2 },
	[SCANCODE_E]               = { .row = 2, .column = 3 },
	[SCANCODE_R]               = { .row = 2, .column = 4 },
	[SCANCODE_T]               = { .row = 2, .column = 5 },
	[SCANCODE_Y]               = { .row = 2, .column = 6 },
	[SCANCODE_U]               = { .row = 2, .column = 7 },
	[SCANCODE_I]               = { .row = 2, .column = 8 },
	[SCANCODE_O]               = { .row = 2, .column = 9 },
	[SCANCODE_P]               = { .row = 2, .column = 10 },
	[SCANCODE_LEFT_BRACKET]    = { .row = 2, .column = 11 },
	[SCANCODE_RIGHT_BRACKET]   = { .row = 2, .column = 12 },
	[SCANCODE_BACKSLASH]       = { .row = 2, .column = 13 },
	// delete
	// end
	// page down
	[SCANCODE_KEYPAD_7]        = { .row = 2, .column = 17 },
	[SCANCODE_KEYPAD_8]        = { .row = 2, .column = 18 },
	[SCANCODE_KEYPAD_9]        = { .row = 2, .column = 19 },
	[SCANCODE_KEYPAD_PLUS]     = { .row = 2, .column = 20 },

	[SCANCODE_CAPS_LOCK]       = { .row = 3, .column = 0 },
	[SCANCODE_A]               = { .row = 3, .column = 1 },
	[SCANCODE_S]               = { .row = 3, .column = 2 },
	[SCANCODE_D]               = { .row = 3, .column = 3 },
	[SCANCODE_F]               = { .row = 3, .column = 4 },
	[SCANCODE_G]               = { .row = 3, .column = 5 },
	[SCANCODE_H]               = { .row = 3, .column = 6 },
	[SCANCODE_J]               = { .row = 3, .column = 7 },
	[SCANCODE_K]               = { .row = 3, .column = 8 },
	[SCANCODE_L]               = { .row = 3, .column = 9 },
	[SCANCODE_SEMICOLON]       = { .row = 3, .column = 10 },
	[SCANCODE_APOSTROPHE]      = { .row = 3, .column = 11 },
	[SCANCODE_ENTER]           = { .row = 3, .column = 12 },
	[SCANCODE_KEYPAD_4]        = { .row = 3, .column = 13 },
	[SCANCODE_KEYPAD_5]        = { .row = 3, .column = 14 },
	[SCANCODE_KEYPAD_6]        = { .row = 3, .column = 15 },

	[SCANCODE_LEFT_SHIFT]      = { .row = 4, .column = 0 },
	[SCANCODE_Z]               = { .row = 4, .column = 1 },
	[SCANCODE_X]               = { .row = 4, .column = 2 },
	[SCANCODE_C]               = { .row = 4, .column = 3 },
	[SCANCODE_V]               = { .row = 4, .column = 4 },
	[SCANCODE_B]               = { .row = 4, .column = 5 },
	[SCANCODE_N]               = { .row = 4, .column = 6 },
	[SCANCODE_M]               = { .row = 4, .column = 7 },
	[SCANCODE_COMMA]           = { .row = 4, .column = 8 },
	[SCANCODE_DOT]             = { .row = 4, .column = 9 },
	[SCANCODE_SLASH]           = { .row = 4, .column = 10 },
	[SCANCODE_RIGHT_SHIFT]     = { .row = 4, .column = 11 },
	// cursor up
	[SCANCODE_KEYPAD_1]        = { .row = 4, .column = 13 },
	[SCANCODE_KEYPAD_2]        = { .row = 4, .column = 14 },
	[SCANCODE_KEYPAD_3]        = { .row = 4, .column = 15 },
	// kepad enter

	[SCANCODE_LEFT_CONTROL]    = { .row = 5, .column = 0 },
	// left gui
	[SCANCODE_LEFT_ALT]        = { .row = 5, .column = 2 },
	[SCANCODE_SPACE]           = { .row = 5, .column = 3 },
	// right alt
	// right gui
	// right control
	// cursor left
	// cursor down
	// cursor right
	[SCANCODE_KEYPAD_0]        = { .row = 5, .column = 10 },
	[SCANCODE_KEYPAD_DOT]      = { .row = 5, .column = 11 },

};

enum scancode_extended : u8
{
	SCANCODE_EXTENDED_MULTIMEDIA_WWW_SEARCH     = 0X10,
	SCANCODE_EXTENDED_RIGHT_ALT                 = 0X11,
	SCANCODE_EXTENDED_RIGHT_CONTROL             = 0X14,
	SCANCODE_EXTENDED_MULTIMEDIA_PREVIOUS_TRACK = 0X15,
	SCANCODE_EXTENDED_MULTIMEDIA_WWW_FAVOURITES = 0X18,
	SCANCODE_EXTENDED_LEFT_GUI                  = 0X1F,
	SCANCODE_EXTENDED_MULTIMEDIA_WWW_REFRESH    = 0X20,
	SCANCODE_EXTENDED_MULTIMEDIA_VOLUME_DOWN    = 0X21,
	SCANCODE_EXTENDED_MULTIMEDIA_MUTE           = 0X23,
	SCANCODE_EXTENDED_RIGHT_GUI                 = 0X27,
	SCANCODE_EXTENDED_MULTIMEDIA_WWW_STOP       = 0X28,
	SCANCODE_EXTENDED_MULTIMEDIA_CALCULATOR     = 0X2B,
	SCANCODE_EXTENDED_APPS                      = 0X2F,
	SCANCODE_EXTENDED_MULTIMEDIA_WWW_FORWARD    = 0X30,
	SCANCODE_EXTENDED_MULTIMEDIA_VOLUME_UP      = 0X32,
	SCANCODE_EXTENDED_MULTIMEDIA_PLAY_PAUSE     = 0X34,
	SCANCODE_EXTENDED_ACPI_POWER                = 0X37,
	SCANCODE_EXTENDED_MULTIMEDIA_WWW_BACK       = 0X38,
	SCANCODE_EXTENDED_MULTIMEDIA_WWW_HOME       = 0X3A,
	SCANCODE_EXTENDED_MULTIMEDIA_STOP           = 0X3B,
	SCANCODE_EXTENDED_ACPI_SLEEP                = 0X3F,
	SCANCODE_EXTENDED_MULTIMEDIA_MY_COMPUTER    = 0X40,
	SCANCODE_EXTENDED_MULTIMEDIA_EMAIL          = 0X48,
	SCANCODE_EXTENDED_KEYPAD_SLASH              = 0X4A,
	SCANCODE_EXTENDED_MULTIMEDIA_NEXT_TRACK     = 0X4D,
	SCANCODE_EXTENDED_MULTIMEDIA_MEDIA_SELECT   = 0X50,
	SCANCODE_EXTENDED_KEYPAD_ENTER              = 0X5A,
	SCANCODE_EXTENDED_ACPI_WAKE                 = 0X5E,
	SCANCODE_EXTENDED_END                       = 0X69,
	SCANCODE_EXTENDED_CURSOR_LEFT               = 0X6B,
	SCANCODE_EXTENDED_HOME                      = 0X6C,
	SCANCODE_EXTENDED_INSERT                    = 0X70,
	SCANCODE_EXTENDED_DELETE                    = 0X71,
	SCANCODE_EXTENDED_CURSOR_DOWN               = 0X72,
	SCANCODE_EXTENDED_CURSOR_RIGHT              = 0X74,
	SCANCODE_EXTENDED_CURSOR_UP                 = 0X75,
	SCANCODE_EXTENDED_PAGE_DOWN                 = 0X7A,
	SCANCODE_EXTENDED_PAGE_UP                   = 0X7D,

	SCANCODE_EXTENDED_PRINT_SCREEN_1 = 0x12,
	SCANCODE_EXTENDED_PRINT_SCREEN_2 = 0x7C,
};

constexpr static union keycode const scancode_extended_to_keycode[UINT8_MAX] = {
	[SCANCODE_EXTENDED_INSERT]                    = { .row = 1, .column = 14 },
	[SCANCODE_EXTENDED_HOME]                      = { .row = 1, .column = 15 },
	[SCANCODE_EXTENDED_PAGE_UP]                   = { .row = 1, .column = 16 },
	[SCANCODE_EXTENDED_KEYPAD_SLASH]              = { .row = 1, .column = 18 },

	[SCANCODE_EXTENDED_DELETE]                    = { .row = 2, .column = 14 },
	[SCANCODE_EXTENDED_END]                       = { .row = 2, .column = 15 },
	[SCANCODE_EXTENDED_PAGE_DOWN]                 = { .row = 2, .column = 16 },

	[SCANCODE_EXTENDED_CURSOR_UP]                 = { .row = 4, .column = 12 },
	[SCANCODE_EXTENDED_KEYPAD_ENTER]              = { .row = 4, .column = 16 },

	[SCANCODE_EXTENDED_LEFT_GUI]                  = { .row = 5, .column = 1 },
	[SCANCODE_EXTENDED_RIGHT_ALT]                 = { .row = 5, .column = 4 },
	[SCANCODE_EXTENDED_RIGHT_GUI]                 = { .row = 5, .column = 5 },
	[SCANCODE_EXTENDED_RIGHT_CONTROL]             = { .row = 5, .column = 6 },
	[SCANCODE_EXTENDED_CURSOR_LEFT]               = { .row = 5, .column = 7 },
	[SCANCODE_EXTENDED_CURSOR_DOWN]               = { .row = 5, .column = 8 },
	[SCANCODE_EXTENDED_CURSOR_RIGHT]              = { .row = 5, .column = 9 },

	[SCANCODE_EXTENDED_MULTIMEDIA_PLAY_PAUSE]     = { .row = 0, .column = 0 },
	[SCANCODE_EXTENDED_MULTIMEDIA_WWW_SEARCH]     = { .row = 0, .column = 0 },
	[SCANCODE_EXTENDED_MULTIMEDIA_PREVIOUS_TRACK] = { .row = 0, .column = 0 },
	[SCANCODE_EXTENDED_MULTIMEDIA_WWW_FAVOURITES] = { .row = 0, .column = 0 },
	[SCANCODE_EXTENDED_MULTIMEDIA_WWW_REFRESH]    = { .row = 0, .column = 0 },
	[SCANCODE_EXTENDED_MULTIMEDIA_VOLUME_DOWN]    = { .row = 0, .column = 0 },
	[SCANCODE_EXTENDED_MULTIMEDIA_MUTE]           = { .row = 0, .column = 0 },
	[SCANCODE_EXTENDED_MULTIMEDIA_WWW_STOP]       = { .row = 0, .column = 0 },
	[SCANCODE_EXTENDED_MULTIMEDIA_CALCULATOR]     = { .row = 0, .column = 0 },
	[SCANCODE_EXTENDED_APPS]                      = { .row = 0, .column = 0 },
	[SCANCODE_EXTENDED_MULTIMEDIA_WWW_FORWARD]    = { .row = 0, .column = 0 },
	[SCANCODE_EXTENDED_MULTIMEDIA_VOLUME_UP]      = { .row = 0, .column = 0 },
	[SCANCODE_EXTENDED_ACPI_POWER]                = { .row = 0, .column = 0 },
	[SCANCODE_EXTENDED_MULTIMEDIA_WWW_BACK]       = { .row = 0, .column = 0 },
	[SCANCODE_EXTENDED_MULTIMEDIA_WWW_HOME]       = { .row = 0, .column = 0 },
	[SCANCODE_EXTENDED_MULTIMEDIA_STOP]           = { .row = 0, .column = 0 },
	[SCANCODE_EXTENDED_ACPI_SLEEP]                = { .row = 0, .column = 0 },
	[SCANCODE_EXTENDED_MULTIMEDIA_MY_COMPUTER]    = { .row = 0, .column = 0 },
	[SCANCODE_EXTENDED_MULTIMEDIA_EMAIL]          = { .row = 0, .column = 0 },
	[SCANCODE_EXTENDED_MULTIMEDIA_NEXT_TRACK]     = { .row = 0, .column = 0 },
	[SCANCODE_EXTENDED_MULTIMEDIA_MEDIA_SELECT]   = { .row = 0, .column = 0 },
	[SCANCODE_EXTENDED_ACPI_WAKE]                 = { .row = 0, .column = 0 },
};

// The scancodes for these are on 4 bytes for print screen and 8 bytes for pause
// so they are defined directly as tokens of the state machine since they are exceptions
union keycode const PRINT_SCREEN_KEYCODE = { .row = 0, .column = 13 };
union keycode const PAUSE_KEYCODE = { .row = 0, .column = 15 };

// TODO: Use a bitmap to compress this down
static bool KEYCODE_IS_PRESSED[UINT8_MAX] = { 0 };

enum state : u8
{
	STATE_TRAP,
	STATE_INITIAL,
	STATE_RELEASED,
	STATE_PAUSE_PRESSED,

	STATE_KEY_PRESSED,
	STATE_KEY_RELEASED,

	STATE_EXTENDED,
	STATE_EXTENDED_PRINT_1,
	STATE_EXTENDED_PRINT_2,

	STATE_EXTENDED_RELEASED,
	STATE_EXTENDED_RELEASED_PRINT_1,
	STATE_EXTENDED_RELEASED_PRINT_2,
	STATE_EXTENDED_RELEASED_PRINT_3,

	STATE_NMEMB
};

// for debugging
const char* state_string[] =
{
	"STATE_TRAP",
	"STATE_INITIAL",
	"STATE_RELEASED",
	"STATE_PAUSE_PRESSED",

	"STATE_KEY_PRESSED",
	"STATE_KEY_RELEASED",

	"STATE_EXTENDED",
	"STATE_EXTENDED_PRINT_1",
	"STATE_EXTENDED_PRINT_2",

	"STATE_EXTENDED_RELEASED",
	"STATE_EXTENDED_RELEASED_PRINT_1",
	"STATE_EXTENDED_RELEASED_PRINT_2",
	"STATE_EXTENDED_RELEASED_PRINT_3",
};

enum token : u8
{
	TOKEN_RELEASED,

	TOKEN_EXTENDED,
	TOKEN_SCANCODE,
	TOKEN_SCANCODE_EXTENDED,

	TOKEN_PAUSE,
	TOKEN_PRINT_SCREEN_1,
	TOKEN_PRINT_SCREEN_2,

	TOKEN_NMEMB,
};

const char* token_string[] =
{
	"TOKEN_RELEASED",

	"TOKEN_EXTENDED",
	"TOKEN_SCANCODE",
	"TOKEN_SCANCODE_EXTENDED",

	"TOKEN_PAUSE",
	"TOKEN_PRINT_SCREEN_1",
	"TOKEN_PRINT_SCREEN_2",

	"TOKEN_NMEMB",
};

// [state][token] = new state to transition to
constexpr enum state state_transition_table[STATE_NMEMB][TOKEN_NMEMB] = {
			   //              RELEASED,                        EXTENDED,           SCANCODE,   SCANCODE_EXTENDED,                    PAUSE,                 PRINT_SCREEN_1,  PRINT_SCREEN_2,
/* TRAP            */ { 0 },
/* INITIAL         */ {           STATE_RELEASED,                  STATE_EXTENDED,  STATE_KEY_PRESSED,                    0,   STATE_PAUSE_PRESSED,                                0,                  0 },
/* REL             */ {                        0,                               0, STATE_KEY_RELEASED,                    0,                     0,                                0,                  0 },
/* PAUSE_PRSD      */ {                        0,                               0, STATE_KEY_RELEASED,                    0,                     0,                                0,                  0 },

// maybe set it to initial_state?
/* K_PRSD          */ {                        0,                               0,                  0,                    0,                     0,                                0,                  0 },
/* K_REL           */ {                        0,                               0,                  0,                    0,                     0,                                0,                  0 }, // same

/* EXT             */ {  STATE_EXTENDED_RELEASED,                               0,                  0,    STATE_KEY_PRESSED,                     0,           STATE_EXTENDED_PRINT_1,                  0 },
/* EXT_PRINT_1     */ {                        0,          STATE_EXTENDED_PRINT_2,                  0,                    0,                     0,                                0,                  0 },
/* EXT_PRINT_2     */ {                        0,                               0,                  0,                    0,                     0,                                0,  STATE_KEY_PRESSED },

/* EXT_REL         */ {                        0,                               0,                  0,   STATE_KEY_RELEASED,                     0,  STATE_EXTENDED_RELEASED_PRINT_1,                  0 },
/* EXT_REL_PRINT_1 */ {                        0, STATE_EXTENDED_RELEASED_PRINT_2,                  0,                    0,                     0,                                0,                  0 },
/* EXT_REL_PRINT_2 */ { STATE_EXTENDED_RELEASED_PRINT_3,                        0,                  0,                    0,                     0,                                0,                  0 },
/* EXT_REL_PRINT_3 */ {                        0,                               0,                  0,                    0,                     0,                                0, STATE_KEY_RELEASED },
};

void PS2_keyboard_IRQ_1_handler(struct ISR_parameters const *p)
{
	static enum state state = STATE_INITIAL;

	// puts("----------------------------------------------------------------\n");
	// printf("%s ---> ", state_string[state]);

	#include <architecture/x86/io.h>
	if (state == STATE_TRAP) {
		printf("PS2 DRIVER BUG!\nKERNEL PANIC");
		x86_panic();
	}


	// TODO: Test if pause works
	if (state == STATE_PAUSE_PRESSED) {
		static s8 i = 0;
		if (i++ == 7) {
			i = 0;
			state = STATE_KEY_PRESSED;
			goto shortcut;
		} else {
			return;
		}
	}

	u8 const scancode = PS2_8042_data_read();

	// determine token
	enum token token;
	if (scancode == SCANCODE_RELEASED) {
		token = TOKEN_RELEASED;
	} else if (scancode == SCANCODE_EXTENDED) {
		token = TOKEN_EXTENDED;
	} else if (scancode == 0xE1) {
		token = TOKEN_PAUSE;
	} else if (state == STATE_EXTENDED && scancode == SCANCODE_EXTENDED_PRINT_SCREEN_1) {
		token = TOKEN_PRINT_SCREEN_1;
	} else if (state == STATE_EXTENDED_PRINT_2 && scancode == SCANCODE_EXTENDED_PRINT_SCREEN_2) {
		token = TOKEN_PRINT_SCREEN_2;
	} else if (state == STATE_EXTENDED || state == STATE_EXTENDED_RELEASED) {
		token = TOKEN_SCANCODE_EXTENDED;
	} else {
		token = TOKEN_SCANCODE;
	}
	state = state_transition_table[state][token];

	// printf("%s\n", state_string[state]);
	// printf("%X\t %s\n", scancode, token_string[token]);

	if (state == STATE_KEY_PRESSED || state == STATE_KEY_RELEASED) {
		union keycode k;

		if (token == TOKEN_SCANCODE) {
			k = scancode_to_keycode[scancode];
		} else if (token == TOKEN_SCANCODE_EXTENDED) {
			k = scancode_extended_to_keycode[scancode];
		} else {
			shortcut:
			k = PAUSE_KEYCODE;
		}

		if (state == STATE_KEY_PRESSED) {
			KEYCODE_IS_PRESSED[k.code] = true;
			// putchar(keycode_to_key[k.row][k.column]);
		} else {
			KEYCODE_IS_PRESSED[k.code] = false;
			KEYCODE_IS_PRESSED[PAUSE_KEYCODE.code] = false;
		}

		state = STATE_INITIAL;
	}
}
