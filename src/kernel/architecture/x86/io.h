#pragma once
#if __x86_64__
#	include <architecture/x86/64/io.h>
#	define x86_out_8(PORT, DATA)	x86_64_out_8(PORT, DATA)
#	define x86_out_16(PORT, DATA)	x86_64_out_16(PORT, DATA)
#	define x86_out_32(PORT, DATA)	x86_64_out_32(PORT, DATA)
#	define x86_in_8(PORT)		x86_64_in_8(PORT)
#	define x86_in_16(PORT)		x86_64_in_16(PORT)
#	define x86_in_32(PORT)		x86_64_in_32(PORT)
#else
#	include <architecture/x86/32/io.h>
#	define x86_out_8(PORT, DATA)	x86_32_out_8(PORT, DATA)
#	define x86_out_16(PORT, DATA)	x86_32_out_16(PORT, DATA)
#	define x86_out_32(PORT, DATA)	x86_32_out_32(PORT, DATA)
#	define x86_in_8(PORT)		x86_32_in_8(PORT)
#	define x86_in_16(PORT)		x86_32_in_16(PORT)
#	define x86_in_32(PORT)		x86_32_in_32(PORT)
#endif

void x86_io_wait(void);
