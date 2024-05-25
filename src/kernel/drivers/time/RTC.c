#include "RTC.h"
#include <stdio.h>
#include <architecture/x86/io.h>
#include <architecture/x86/32/irq.h>
/* Consult the IBM PC AT technical reference for more information about
how the chip works */

enum : u8
{
	IO_PORT_SELECT = 0x70,
	IO_PORT_DATA = 0x71,

	// NMI = Non maskable interrupts
	SELECT_NMI_DISABLE = (1 << 7),
};

enum select : u8
{
	SELECT_SECONDS       = 0x00, // 0–59
	SELECT_MINUTES       = 0x02, // 0–59
	// 1–12 in 12-hour mode, highest bit set if pm
	SELECT_HOURS         = 0x04, // 0–23 in 24-hour mode,

	SELECT_WEEKDAY       = 0x06, // 1–7, Sunday = 1
	SELECT_DAY           = 0x07, // 1–31
	SELECT_MONTH         = 0x08, // 1–12
	SELECT_YEAR          = 0x09, // 0–99
	SELECT_CENTURY       = 0x32, // 19–20?

	SELECT_STATUS_A      = 0x0A,
	SELECT_STATUS_B      = 0x0B,
	SELECT_STATUS_C      = 0x0C,
	SELECT_STATUS_D      = 0x0D,

	SELECT_ALARM_SECONDS = 0x01,
	SELECT_ALARM_MINUTES = 0x03,
	SELECT_ALARM_HOURS   = 0x05,
};

union status_a
{
	struct {
		// frequency divider, default (0b0110) (1.024kHz, square wave, 976.562u periodic int)
		// The fastest rate you can select without problems is 3
		// to choose a new rate use this equation: frequency = 32768 >> (rate-1);
		u8 rate_selection : 4;
		u8 _time_base : 3; // don't change this because only the default value (0b010)(32.768kHz) keeps proper time
		bool update_in_progress : 1;
	};
	u8 byte;
};

// All default to false except hours_24_mode
union status_b
{
	struct {
		bool daylight_savings : 1;        // DSE
		bool hours_24_mode : 1;
		bool binary_format : 1;           // DM: as opposed to BCD (binary coded decimal)
		bool square_wave : 1;             // SQWE: must follow setting in status a rate_selection
		bool interrupts_update_ended : 1; // UIE
		bool interrupts_alarm : 1;        // AIE
		bool interrupts_periodic : 1;     // PIE
		u8 _no_idea : 1;
	};
	u8 byte;
};

union status_c
{
	struct {
		u8 _reserved : 4;
		// these flags are read-only
		bool UF : 1;
		bool AF : 1;
		bool PF : 1;
		bool IRQF : 1;
	};
	u8 byte;
};

union status_d
{
	struct {
		u8 _reserved : 7;
		// read-only
		bool battery_good : 1; // VRB
	};
	u8 byte;
};

static inline void select(u8 const r)
{
	x86_out_8(IO_PORT_SELECT, SELECT_NMI_DISABLE | r);
}

static inline u8 read(void)
{
	return x86_in_8(IO_PORT_DATA);
}

static inline u8 get_register(enum select const r)
{
	select(r);
	return read();
}

static inline void write(enum select const r, u8 const c)
{
	select(r);
	x86_out_8(IO_PORT_DATA, c);
}

static inline void acknowledge_interrupt(void)
{
	// read C to clear the irq pin
	get_register(SELECT_STATUS_C);
}

struct RTC_time RTC_TIME;
struct RTC_date RTC_DATE;

void RTC_IRQ_8_handler(struct ISR_parameters const *p)
{
	RTC_TIME.seconds  = get_register(SELECT_SECONDS);
	RTC_TIME.minutes  = get_register(SELECT_MINUTES);
	RTC_TIME.hours_24 = get_register(SELECT_HOURS);

	RTC_DATE.weekday  = get_register(SELECT_WEEKDAY);
	RTC_DATE.day      = get_register(SELECT_DAY);
	RTC_DATE.month    = get_register(SELECT_MONTH);
	// TODO: Check if the RTC century register exists with ACPI
	RTC_DATE.year     = get_register(SELECT_YEAR) + get_register(SELECT_CENTURY) * 100;

	// printf("%u %u/%u/%u\t", RTC_DATE.weekday, RTC_DATE.day, RTC_DATE.month, RTC_DATE.year);
	// printf("%u:%u:%u\n", RTC_TIME.hours_24, RTC_TIME.minutes, RTC_TIME.seconds);

	acknowledge_interrupt();
}

void RTC_initialize(void)
{
	IRQ_disable();

	union status_d d;
	d.byte = get_register(SELECT_STATUS_D);
	if (!d.battery_good) {
		// TODO: log properly
		printf("RTC battery is dead, couldn't initialize\n");
		return;
	}

	union status_a a;
	a.byte = get_register(SELECT_STATUS_A);
	a.rate_selection = 14; // 4hz
	write(SELECT_STATUS_A, a.byte);

	union status_b b = {
		.daylight_savings  = false,
		.hours_24_mode  = true,
		.binary_format  = true,
		.square_wave  = false,
		.interrupts_update_ended  = true,
		.interrupts_alarm  = false,
		.interrupts_periodic  = true,
		._no_idea  = false,
	};

	write(SELECT_STATUS_B, b.byte);

	acknowledge_interrupt();
	IRQ_enable();
}

struct RTC_time* RTC_time_get(void)
{
	return &RTC_TIME;
}

struct RTC_date* RTC_date_get(void)
{
	return &RTC_DATE;
}
