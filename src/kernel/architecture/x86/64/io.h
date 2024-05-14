#pragma once
#include <types.h>

typedef u16 IO_port_8;
typedef u16 IO_port_16;
typedef u16 IO_port_32;

[[gnu::cdecl]] void x86_64_out_8(const IO_port_8 port, const u8 data);
[[gnu::cdecl]] void x86_64_out_16(const IO_port_16 port, const u16 data);
[[gnu::cdecl]] void x86_64_out_32(const IO_port_32 port, const u32 data);
[[gnu::cdecl]] u8   x86_64_in_8(const IO_port_8 port);
[[gnu::cdecl]] u16  x86_64_in_16(const IO_port_16 port);
[[gnu::cdecl]] u32  x86_64_in_32(const IO_port_32 port);
[[gnu::cdecl]] void x86_panic(void);
