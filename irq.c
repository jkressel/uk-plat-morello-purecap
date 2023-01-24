/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Authors: Santiago Pagani <santiagopagani@gmail.com>
 * 			John A. Kressel <jkressel.apps@gmail.com>
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

#include <stdlib.h>
#include <uk/plat/irq.h>
#include <uk/print.h>
#include <uk/essentials.h>
#include <morello/irq.h>
#include <morello/time.h>
#include <morello/morello_info.h>
#include <arm/time.h>

static irq_handler_func_t irq_handlers[IRQS_MAX];

struct __a64regs exception_regs_a64;

int ukplat_irq_register(unsigned long irq, irq_handler_func_t func, void *arg __unused)
{
	switch (irq) {
		case IRQ_ID_ARM_GENERIC_TIMER:
			break;
		default:
			// Unsupported IRQ
			uk_pr_crit("ukplat_irq_register: Unsupported IRQ\n");
			return -1;
	}

	irq_handlers[irq] = func;
	return 0;
}

int ukplat_irq_init(struct uk_alloc *a __unused)
{
	for (unsigned int i = 0; i < IRQS_MAX; i++) {
		irq_handlers[i] = NULL;
	}
	// *DISABLE_BASIC_IRQS = 0xFFFFFFFF;
	// *DISABLE_IRQS_1 = 0xFFFFFFFF;
	// *DISABLE_IRQS_2 = 0xFFFFFFFF;
	disable_irq();
	irq_vector_init();
	enable_irq();
	return 0;
}

void show_invalid_entry_message(int type)
{
	printf("IRQ: %d\n", type);
}

void dump_registers(int type, struct __a64regs *regs) {
	printf("IRQ: %d\n", type);
	printf("x0: 0x%x\n", regs->x0);
	printf("x1: 0x%x\n", regs->x1);
	printf("x2: 0x%x\n", regs->x2);
	printf("x3: 0x%x\n", regs->x3);
	printf("x4: 0x%x\n", regs->x4);
	printf("x5: 0x%x\n", regs->x5);
	printf("x6: 0x%x\n", regs->x6);
	printf("x7: 0x%x\n", regs->x7);
	printf("x8: 0x%x\n", regs->x8);
	printf("x9: 0x%x\n", regs->x9);
	printf("x10: 0x%x\n", regs->x10);
	printf("x11: 0x%x\n", regs->x11);
	printf("x12: 0x%x\n", regs->x12);
	printf("x13: 0x%x\n", regs->x13);
	printf("x14: 0x%x\n", regs->x14);
	printf("x15: 0x%x\n", regs->x15);
	printf("x16: 0x%x\n", regs->x16);
	printf("x17: 0x%x\n", regs->x17);
	printf("x18: 0x%x\n", regs->x18);
	printf("x19: 0x%x\n", regs->x19);
	printf("x20: 0x%x\n", regs->x20);
	printf("x21: 0x%x\n", regs->x21);
	printf("x22: 0x%x\n", regs->x22);
	printf("x23: 0x%x\n", regs->x23);
	printf("x24: 0x%x\n", regs->x24);
	printf("x25: 0x%x\n", regs->x25);
	printf("x26: 0x%x\n", regs->x26);
	printf("x27: 0x%x\n", regs->x27);
	printf("x28: 0x%x\n", regs->x28);
	printf("fp: 0x%x\n", regs->x29);
	printf("lr: 0x%x\n", regs->x30);
	printf("sp: 0x%x\n", regs->x31);
	printf("esr_el1: 0x%x\n", regs->esr_el1);
	printf("elr_el1: 0x%x\n", regs->elr_el1);
	printf("spsr_el1: 0x%x\n", regs->spsr_el1);
	printf("cctlr_el1: 0x%x\n", regs->cctlr);
	printf("cpacr_el1: 0x%x\n", regs->cpacr_el1);
	printf("far_el1: 0x%x\n", regs->far_el1);
}

void show_invalid_entry_message_el1_sync(uint64_t esr_el, uint64_t far_el)
{
	uk_pr_debug("ESR_EL1: %lx, FAR_EL1: %lx, SCTLR_EL1:%lx\n", esr_el, far_el, get_sctlr_el1());
}

void ukplat_irq_handle(void)
{

	if ((get_el0(cntv_ctl) & GT_TIMER_IRQ_STATUS) && irq_handlers[IRQ_ID_ARM_GENERIC_TIMER]) {
		irq_handlers[IRQ_ID_ARM_GENERIC_TIMER](NULL);
		return;
	}

	uk_pr_crit("ukplat_irq_handle: Unhandled IRQ\n");
	while(1);
}
