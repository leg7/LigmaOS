#pragma once

[[gnu::cdecl]] void IRQ_enable(void);
[[gnu::cdecl]] void IRQ_disable(void);
void IRQ_initialize(void);
