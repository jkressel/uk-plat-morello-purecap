/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Authors: Santiago Pagani <santiagopagani@gmail.com>
 *          John A. Kressel <jkressel.apps@gmail.com>
 *
 * Copyright (c) 2020, NEC Laboratories Europe GmbH, NEC Corporation.
 *                     All rights reserved.
 * Copyright (c) 2022, John A. Kressel <jkressel.apps@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * THIS HEADER MAY NOT BE EXTRACTED OR MODIFIED IN ANY WAY.
 */

#ifndef __MORELLO_IRQ_H__
#define __MORELLO_IRQ_H__

#include <morello/sysregs.h>

#define IRQ_BASIC_PENDING	((volatile __u32 *)(MMIO_BASE+0x0000B200))
#define IRQ_PENDING_1		((volatile __u32 *)(MMIO_BASE+0x0000B204))
#define IRQ_PENDING_2		((volatile __u32 *)(MMIO_BASE+0x0000B208))
#define FIQ_CONTROL			((volatile __u32 *)(MMIO_BASE+0x0000B20C))
#define ENABLE_IRQS_1		((volatile __u32 *)(MMIO_BASE+0x0000B210))
#define ENABLE_IRQS_2		((volatile __u32 *)(MMIO_BASE+0x0000B214))
#define ENABLE_BASIC_IRQS	((volatile __u32 *)(MMIO_BASE+0x0000B218))
#define DISABLE_IRQS_1		((volatile __u32 *)(MMIO_BASE+0x0000B21C))
#define DISABLE_IRQS_2		((volatile __u32 *)(MMIO_BASE+0x0000B220))
#define DISABLE_BASIC_IRQS	((volatile __u32 *)(MMIO_BASE+0x0000B224))

#define IRQS_BASIC_ARM_TIMER_IRQ	(1 << 0)

#define IRQS_1_SYSTEM_TIMER_IRQ_0	(1 << 0)
#define IRQS_1_SYSTEM_TIMER_IRQ_1	(1 << 1)
#define IRQS_1_SYSTEM_TIMER_IRQ_2	(1 << 2)
#define IRQS_1_SYSTEM_TIMER_IRQ_3	(1 << 3)
#define IRQS_1_USB_IRQ				(1 << 9)

#define IRQS_MAX							2
#define IRQ_ID_ARM_GENERIC_TIMER			0
#define IRQ_ID_MORELLO_ARM_SIDE_TIMER			1

void irq_vector_init( void );
void enable_irq( void );
void disable_irq( void );


struct __a64regs {
	uint64_t esr_el1;
    uint64_t elr_el1;
    uint64_t spsr_el1;
    uint64_t cctlr;
    uint64_t cpacr_el1;
    uint64_t far_el1;
    uint64_t x0;
    uint64_t x0_hi;
    uint64_t x1;
    uint64_t x1_hi;
    uint64_t x2;
    uint64_t x2_hi;
    uint64_t x3;
    uint64_t x3_hi;
    uint64_t x4;
    uint64_t x4_hi;
    uint64_t x5;
    uint64_t x5_hi;
    uint64_t x6;
    uint64_t x6_hi;
    uint64_t x7;
    uint64_t x7_hi;
    uint64_t x8;
    uint64_t x8_hi;
    uint64_t x9;
    uint64_t x9_hi;
    uint64_t x10;
    uint64_t x10_hi;
    uint64_t x11;
    uint64_t x11_hi;
    uint64_t x12;
    uint64_t x12_hi;
    uint64_t x13;
    uint64_t x13_hi;
    uint64_t x14;
    uint64_t x14_hi;
    uint64_t x15;
    uint64_t x15_hi;
    uint64_t x16;
    uint64_t x16_hi;
    uint64_t x17;
    uint64_t x17_hi;
    uint64_t x18;
    uint64_t x18_hi;
    uint64_t x19;
    uint64_t x19_hi;
    uint64_t x20;
    uint64_t x20_hi;
    uint64_t x21;
    uint64_t x21_hi;
    uint64_t x22;
    uint64_t x22_hi;
    uint64_t x23;
    uint64_t x23_hi;
    uint64_t x24;
    uint64_t x24_hi;
    uint64_t x25;
    uint64_t x25_hi;
    uint64_t x26;
    uint64_t x26_hi;
    uint64_t x27;
    uint64_t x27_hi;
    uint64_t x28;
    uint64_t x28_hi;
    uint64_t x29;
    uint64_t x29_hi;
    uint64_t x30;
    uint64_t x30_hi;
    uint64_t x31;
/* top of stack page */
};

extern struct __a64regs exception_regs_a64;

#endif /* __MORELLO_IRQ_H__ */
