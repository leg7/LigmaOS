#include "PIC_8259A.h"
#include <architecture/x86/io.h>
/*
 * PIC = programmable interrupt controller
 *
 * This implementation is made for the PIIX3 southbridge chipset implementation
 * of the 8259 PIC and it's quirks. To understand this code please read the intel
 * PIIX3 manual section 2.5.3 about the PIC and optionally the 8259A controller manual.
*/

enum : u8 {
	IO_PORT_MASTER_COMMAND = 0x20,
	IO_PORT_MASTER_DATA = 0x21,
	IO_PORT_SLAVE_COMMAND = 0xA0,
	IO_PORT_SLAVE_DATA = 0xA1,

	/*
	 * ICW = initialization command word
	 * OCW = operational control word
	 *
	 * (there are 2-4 of them) and you must use all 4 on the PIIX3.
	 * Consult the documentation to know what each bit of the ICWs does.
	*/
	ICW_1 = 1 & (1 << 4),	            // Require ICW_4 to be sent
	ICW_2_MASTER = 32,			// bits 3-7 will be appended to the IRQ and used to lookup the ISR in the IDT
	ICW_2_SLAVE = 40,
	ICW_3_MASTER = 1 << 3,
	ICW_3_SLAVE = 1 << 1,
	ICW_4 = 1,

	// OCW 2
	COMMAND_EOI = 0x20,

	// OCW 3
	COMMAND_READ_IRQ_REGISTER = 0b10,
	COMMAND_READ_IN_SERVICE_REGISTER = 0b11,
};

void PIC_8259A_initialize(void)
{
	// mask all interrupts while initializing
	x86_out_8(IO_PORT_MASTER_DATA, UINT8_MAX);
	x86_io_wait();
	x86_out_8(IO_PORT_SLAVE_DATA, UINT8_MAX);
	x86_io_wait();

	x86_out_8(IO_PORT_MASTER_COMMAND, ICW_1);
	x86_io_wait();
	x86_out_8(IO_PORT_SLAVE_COMMAND, ICW_1);
	x86_io_wait();

	x86_out_8(IO_PORT_MASTER_DATA, ICW_2_MASTER);
	x86_io_wait();
	x86_out_8(IO_PORT_SLAVE_DATA, ICW_2_SLAVE);
	x86_io_wait();

	x86_out_8(IO_PORT_MASTER_DATA, ICW_3_MASTER);
	x86_io_wait();
	x86_out_8(IO_PORT_SLAVE_DATA, ICW_3_SLAVE);
	x86_io_wait();

	x86_out_8(IO_PORT_MASTER_DATA, ICW_4);
	x86_io_wait();
	x86_out_8(IO_PORT_SLAVE_DATA, ICW_4);
	x86_io_wait();
	x86_out_8(IO_PORT_SLAVE_DATA, ICW_4);

	// unmask all interrupts
	x86_out_8(IO_PORT_MASTER_DATA, 0);
	x86_io_wait();
	x86_out_8(IO_PORT_SLAVE_DATA, 0);
	x86_io_wait();
}

// EOI = end of interrupt
// IRQ = interrupt request
void PIC_8259A_eoi(u8 const irq)
{
	if (irq >= 8) {
		x86_out_8(IO_PORT_SLAVE_COMMAND, COMMAND_EOI);
	}
	x86_out_8(IO_PORT_MASTER_COMMAND, COMMAND_EOI);
}

// returns -1 if the irq to mask is not valid, 0 on success
s32 PIC_8259A_mask(u8 irq)
{
	u8 port;

	if (irq > 15) {
		return -1;
	} else if (irq >= 8) {
		port = IO_PORT_SLAVE_DATA;
		irq -= 8;
	} else {
		port = IO_PORT_MASTER_DATA;
	}

	u8 const mask = x86_in_8(port);
	x86_io_wait();
	x86_out_8(port, mask | (1 << irq));

	return 0;
}

// returns -1 if the irq to mask is not valid, 0 on success
s32 PIC_8259A_unmask(u8 irq)
{
	u8 port;

	if (irq > 15) {
		return -1;
	} else if (irq >= 8) {
		port = IO_PORT_SLAVE_DATA;
		irq -= 8;
	} else {
		port = IO_PORT_MASTER_DATA;
	}

	u8 const mask = x86_in_8(port);
	x86_io_wait();
	x86_out_8(port, mask & ~(1 << irq));

	return 0;
}

u16 PIC_8259A_irq_requests(void)
{
	x86_out_8(IO_PORT_SLAVE_COMMAND, COMMAND_READ_IRQ_REGISTER);
	x86_out_8(IO_PORT_MASTER_COMMAND, COMMAND_READ_IRQ_REGISTER);
	return x86_in_8(IO_PORT_SLAVE_COMMAND) << 8 | x86_in_8(IO_PORT_MASTER_COMMAND);
}

u16 PIC_8259A_irq_in_service(void)
{
	x86_out_8(IO_PORT_SLAVE_COMMAND, COMMAND_READ_IN_SERVICE_REGISTER);
	x86_out_8(IO_PORT_MASTER_COMMAND, COMMAND_READ_IN_SERVICE_REGISTER);
	return x86_in_8(IO_PORT_SLAVE_COMMAND) << 8 | x86_in_8(IO_PORT_MASTER_COMMAND);
}
