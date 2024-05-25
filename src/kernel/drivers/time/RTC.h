#pragma once
#include <architecture/x86/32/isr.h>
#include <library/types.h>

// TODO: support unix time
struct RTC_time
{
	volatile u8 seconds, minutes, hours_24;
};

enum RTC_weekday : u8
{
	SUNDAY = 1,
	MONDAY,
	TUESDAY,
	WEDNESDAY,
	THURSDAY,
	FRIDAY,
	SATURDAY,
};

struct RTC_date
{
	volatile enum RTC_weekday weekday;
	volatile u8 day, month;
	volatile u16 year;
};

// Only call this after setting up the IRQ handler because the interrupt will happen immediately
void RTC_initialize(void);
// TODO: If someone were to modify the contents of the structs it would change the system time
// for everyone who uses these functions and possibly have terrible security implications
struct RTC_time* RTC_time_get(void);
struct RTC_date* RTC_date_get(void);

void RTC_IRQ_8_handler(struct ISR_parameters const *p);

// TODO: make a wait function
