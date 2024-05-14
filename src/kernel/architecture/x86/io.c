#include <architecture/x86/io.h>

void x86_io_wait(void)
{
	x86_out_8(0x80, 0);
}
