/*
 * Copyright (C) 2018, bzt (bztsrc@github), https://github.com/bztsrc/raspi3-tutorial
 * Copyright (C) 2020, Santiago Pagani <santiagopagani@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#include <morello/sysregs.h>

/* PL011 UART registers */
#define UART0_DR        ((volatile unsigned int*)(0x2A400000+0x0))
#define UART0_FR        ((volatile unsigned int*)(0x2A400000+0x18))
#define UART0_IBRD      ((volatile unsigned int*)(0x2A400000+0x24))
#define UART0_FBRD      ((volatile unsigned int*)(0x2A400000+0x28))
#define UART0_LCRH      ((volatile unsigned int*)(0x2A400000+0x2C))
#define UART0_CR        ((volatile unsigned int*)(0x2A400000+0x30))
#define UART0_IMSC      ((volatile unsigned int*)(0x2A400000+0x38))
#define UART0_ICR       ((volatile unsigned int*)(0x2A400000+0x44))

static char prev_sent_char = '\0';

static void wait_cycles(unsigned int n)
{
    if (n) {
		while (n--) {
			asm volatile("nop");
		}
	}
}

static unsigned int serial_tx_buffer_full(void)
{
	return *UART0_FR&0x20;
}

static unsigned int serial_rx_buffer_empty(void)
{
	return *UART0_FR&0x10;
}

/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void _libmorelloplat_init_serial_console()
{
    register unsigned int r;

    /* initialize UART */
    *UART0_CR = 0;         // turn off UART0

    *UART0_ICR = 0x7FF;    // clear interrupts
    *UART0_IBRD = 2;       // 115200 baud
    *UART0_FBRD = 0xB;
    *UART0_LCRH = 0b11<<5; // 8n1
    *UART0_CR = 0x301;     // enable Tx, Rx, FIFO
}

/**
 * Send a character
 */
void _libmorelloplat_serial_putc(char c)
{
	if ((c == '\n') && (prev_sent_char != '\r'))
		_libmorelloplat_serial_putc('\r');

    // Wait until we can send
    do{
		asm volatile("nop");
	} while (serial_tx_buffer_full());

    // Write the character to the buffer
    *UART0_DR = c;
	prev_sent_char = c;
}

/**
 * Receive a character
 */
int  _libmorelloplat_serial_getc(void)
{
	if (serial_rx_buffer_empty())
		return -1;

    char r;
    r = (char)(*UART0_DR);
    return (int)r;
}
