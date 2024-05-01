#include <stdio.h>
#include <architecture/x86/io.h>
#include "PS2_8042.h"

struct {
	const struct {
		IO_port_8 data;	       // read,write
		union {
			IO_port_16 status;  // read (bitmap use enums to access)
			IO_port_16 command; // write
		};
	} IO_ports;

	union {
		struct {
			bool port_1_enable_interrupts : 1;
			bool port_2_enable_interrupts : 1;
			bool system_passed_post : 1;
			u8   _zero_1 : 1;
			bool port_1_enable_clock : 1;
			bool port_2_enable_clock : 1;
			bool port_1_enable_translation : 1;
			u8   _zero_2 : 1;
		};
		u8 byte;
	} config_byte;
} _8042 = {
	.IO_ports = {
		.data = 0x60,
		{ 0x64 }
	},

	.config_byte.byte = 0
};


// masks to apply to the status byte
enum : u8 {
	STATUS_MASK_OUTPUT_FULL             = 1,
	STATUS_MASK_INPUT_FULL              = 1 << 1,
	STATUS_MASK_SYSTEM                  = 1 << 2,
	STATUS_MASK_COMMAND_OR_DATA         = 1 << 3,
	STATUS_MASK_ERROR_TIMEOUT           = 1 << 6,
	STATUS_MASK_ERROR_PARITY            = 1 << 7,
};

enum _8042_command : u8 {
	COMMAND_PORT_1_ENABLE               = 0xAE,
	COMMAND_PORT_1_DISABLE              = 0xAD,
	COMMAND_PORT_1_TEST                 = 0xAB,
	COMMAND_PORT_2_ENABLE               = 0xA8,
	COMMAND_PORT_2_DISABLE              = 0xA7,
	COMMAND_PORT_2_TEST                 = 0xA9,

	COMMAND_CONFIG_BYTE_READ	      = 0x20,
	COMMAND_CONFIG_BYTE_WRITE           = 0x60,

	COMMAND_SELF_TEST                   = 0xAA,
};

enum _8042_command_response : u8 {
	COMMAND_RESPONSE_PORT_2_TEST_PASSED = 0x00,
	COMMAND_RESPONSE_PORT_1_TEST_PASSED = 0x00,

	COMMAND_RESPONSE_SELF_TEST_FAILED   = 0xFC,
	COMMAND_RESPONSE_SELF_TEST_PASSED   = 0x55,
};


static inline void command_send(enum _8042_command const command)
{
	x86_out_8(_8042.IO_ports.command, command);
}

static inline u8 data_read()
{
	return x86_in_8(_8042.IO_ports.data);
}

static inline void data_write(u8 const data)
{
	x86_out_8(_8042.IO_ports.data, data);
}


// TODO: Disable usb legacy support
// TODO: Check if the 8042 controller exists with ACPI
void PS2_8042_initialize(void)
{
	command_send(COMMAND_PORT_1_DISABLE);
	command_send(COMMAND_PORT_1_DISABLE);

	// flush data port in case some data got stuck in it
	data_read();

	// set the config byte
	command_send(COMMAND_CONFIG_BYTE_READ);
	_8042.config_byte.byte = data_read() & 0b1011'1100; // look at the config byte union to understand this
	bool const dual_channel = _8042.config_byte.port_2_enable_clock;
	data_write(_8042.config_byte.byte);
	command_send(COMMAND_CONFIG_BYTE_WRITE);

	// test the config byte
	command_send(COMMAND_SELF_TEST);
	if (data_read() == COMMAND_RESPONSE_SELF_TEST_FAILED) {
		puts("ps2 self test failed\n");
	}

	// test ports
	command_send(COMMAND_PORT_1_TEST);
	u8 const response = data_read();
	if (response != COMMAND_RESPONSE_PORT_1_TEST_PASSED) {
		printf("ps2 port 1 test failed, response byte = %d\n", response);
	}
	if (dual_channel) {
		u8 const response_2 = data_read();
		command_send(COMMAND_PORT_2_TEST);
		if (response_2 != COMMAND_RESPONSE_PORT_2_TEST_PASSED) {
			printf("ps2 port 2 test failed, response byte = %d\n", response_2);
		}
	}

	// enable devices
	_8042.config_byte.port_1_enable_interrupts = true;
	if (dual_channel) {
		_8042.config_byte.port_2_enable_interrupts = true;
	}
	data_write(_8042.config_byte.byte);
	command_send(COMMAND_CONFIG_BYTE_WRITE);

	// reset devices
	// TODO: use a proper timer for a timeout
	while ((x86_in_16(_8042.IO_ports.status) & STATUS_MASK_INPUT_FULL) != false) {
		// wait for it
	}
}
