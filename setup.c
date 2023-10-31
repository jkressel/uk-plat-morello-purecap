/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Authors: Santiago Pagani <santiagopagani@gmail.com>
 * 			John A. Kressel <jkressel.apps@gmail.com>
 *
 * Copyright (c) 2020, NEC Laboratories Europe GmbH, NEC Corporation.
 *                     All rights reserved.
 * 			 (c) 2022, John A. Kressel <jkressel.apps@gmail.com>
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

#include <uk/plat/bootstrap.h>
#include <uk/plat/time.h>
#include <arm/cpu.h>
#include <morello/console.h>
#include <morello/time.h>
#include <uk/print.h>
#include <uk/arch/types.h>
#include <morello/setup.h>

//smccc_conduit_fn_t smccc_psci_call;
smcc_psci_callfn_t smcc_psci_call;

void *__getCapRelocsStart();
void *__getCapRelocsEnd();


//static cap relocations
extern char __start_relocs[], __stop_relocs[];

void do_cap_relocs() {
	//ToDO: reduce perms of ddc used to create reloc caps
	void *__capability ddc = __builtin_cheri_global_data_get();
	void * __capability SealingCap = ddc;
	uint64_t PermsMask =~(__ARM_CAP_PERMISSION_COMPARTMENT_ID__  |
                        __ARM_CAP_PERMISSION_BRANCH_SEALED_PAIR__ |
                        __CHERI_CAP_PERMISSION_PERMIT_UNSEAL__ |
                        __CHERI_CAP_PERMISSION_PERMIT_SEAL__);

	ddc = __builtin_cheri_perms_and(ddc, PermsMask);

	for (reloc *Ptr = (reloc *)__getCapRelocsStart(); Ptr < (reloc*)__getCapRelocsEnd(); ++Ptr) {

		uint64_t Location = Ptr->cap_location;
		uint64_t Base = Ptr->base;
		uint64_t Offset = Ptr->offset;
		uint64_t Size = Ptr->size;
		uint64_t Perms = Ptr->permissions;

		void * __capability *__capability Addr;
		void * __capability Cap, * __capability TypeCap;
		/* Executable capabilities have type 1. */
		uint64_t Type =
			(Perms & __CHERI_CAP_PERMISSION_PERMIT_EXECUTE__)  == 0 ? 1 : 0;
		Addr = __builtin_cheri_address_set(ddc, Location);
		Cap = __builtin_cheri_address_set(ddc, Base);
		Cap = __builtin_cheri_bounds_set(Cap, Size);
		Cap = Cap + Offset;
		Cap = __builtin_cheri_perms_and(Cap, ~Perms);
		TypeCap = __builtin_cheri_address_set(SealingCap, Type);
		Cap = __builtin_cheri_seal(Cap, TypeCap);
		*Addr = Cap;


	}
}

void _libmorelloplat_entry()
{
	do_cap_relocs();
    _libmorelloplat_init_console();

	printf("Hello World From Purecap Unikraft!");
	/*
	 * Enter Unikraft
	 */
	//ukplat_entry(0, 0);
}
