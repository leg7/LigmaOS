#include "PIT_8254.h"
#include <library/types.h>
#include <architecture/x86/io.h>
#include <architecture/x86/32/irq.h>
#include <stdio.h>

enum : u8
{
	// Used to set the 16 bit reload value
	IO_PORT_CHANNEL_0 = 0x40,
	IO_PORT_CHANNEL_1 = 0x41, // Apparently unusable
	IO_PORT_CHANNEL_2 = 0x42,

	// Read-only
	IO_PORT_MODE      = 0x43,
	IO_PORT_COMMAND   = IO_PORT_MODE,
};

enum operating_mode //: u8
{
	OPERATING_MODE_0, // interrupt on terminal count
	OPERATING_MODE_1, // hardware re-triggerable one-shot
	OPERATING_MODE_2, // rate generator
	OPERATING_MODE_3, // square wave generator
	OPERATING_MODE_4, // software triggered strobe
	OPERATING_MODE_5, // hardware triggered strobe
};

enum access_mode //: u8
{
	ACCESS_MODE_LATCH_COUNT,
	ACCESS_MODE_LOBYTE_ONLY,
	ACCESS_MODE_HIBYTE_ONLY,
	ACCESS_MODE_LOBYTE_HIBYTE,
};

enum channel //:u8
{
	CHANNEL_0,
	CHANNEL_1,
	CHANNEL_2,
	CHANNEL_READ_BACK,
};

union mode_command_byte
{
	struct {
		bool BCD_mode : 1; // Binary coded decimal, false = binary mode
		enum operating_mode operating_mode : 3;
		enum access_mode access_mode : 2;
		enum channel channel : 2;
	};
	u8 byte;
};

constexpr u32 BASE_FREQUENCY = 1'193'182;

static inline void mode_set(union mode_command_byte m)
{
	x86_out_8(IO_PORT_MODE, m.byte);
}

void PIT_8254_initialize(void)
{
	// TODO: Make set channel 0 to tick at 44100 to play audio files
	// union mode_command_byte const b = {
	// 	.BCD_mode = false,
	// 	.operating_mode = OPERATING_MODE_3,
	// 	.access_mode = ACCESS_MODE_LOBYTE_HIBYTE,
	// 	.channel = CHANNEL_0,
	// };
	//
	// u16 const frequency = 44100;
}

void PIT_8254_frequency_play(u16 const frequency)
{
	union mode_command_byte const b = {
		.BCD_mode = false,
		.operating_mode = OPERATING_MODE_3,
		.access_mode = ACCESS_MODE_LOBYTE_HIBYTE,
		.channel = CHANNEL_2,
	};
	x86_out_8(IO_PORT_MODE, b.byte);

	u16 const divider = BASE_FREQUENCY / frequency;
	x86_out_8(IO_PORT_CHANNEL_2, divider);
	x86_out_8(IO_PORT_CHANNEL_2, divider >> 8);

	// enable sound
	u8 const tmp = x86_in_8(0x61);
  	if (tmp != (tmp | 3)) {
 		x86_out_8(0x61, tmp | 3);
 	}
}

void PIT_8254_frequency_stop(void)
{
	x86_out_8(0x61, x86_in_8(0x61) & 0xFC);
}

void PIT_8254_IRQ_0_handler(struct ISR_parameters const *p)
{
	// printf("bazinga\n");
}
