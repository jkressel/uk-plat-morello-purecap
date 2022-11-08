/*
 * Copyright (C) 2018, bzt (bztsrc@github), https://github.com/bztsrc/raspi3-tutorial
 * Copyright (c) 2018, Sergey Matyukevich <https://github.com/s-matyukevich/raspberry-pi-os>
 *           (c) 2020, Santiago Pagani <santiagopagani@gmail.com>
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
#include <morello/mm.h>
#include <morello/mmu.h>
#include <uk/config.h>

.section ".text.boot"
.global _start
_start:
    // read cpu id, stop slave cores
//	ldr		x0, =0x3F003000
//	ldr		w10, [x0, #4]
//	ldr		w11, [x0, #8]
//	ldr		w12, [x0, #4]
//	ldr		w13, [x0, #8]
//    mrs     x1, mpidr_el1
//    and     x1, x1, #3
//    cbz     x1, master
b master

// If the cpu id is > 0, hang here
hang:  wfe
    b       hang

// Continue if cpu id == 0
master:
mov        x0, #0
        msr        cptr_el3, x0
//	ldr		x0, =SCTLR_EL1_VALUE_MMU_DISABLED
//	msr		sctlr_el1, x0

//	ldr		x0, =SCTLR_EL2_VALUE
//	msr		sctlr_el2, x0






 MRS   x0, MIDR_EL1
 MSR   VPIDR_EL2, x0
 MRS   x0, MPIDR_EL1
 MSR   VMPIDR_EL2, x0
 
 
 // Set VMID
 // ---------
 // Although we are not using stage 2 translation, NS.EL1 still cares
 // about the VMID
 MSR   VTTBR_EL2, xzr


 // Set SCTLRs for EL1/2 to safe values
 // ------------------------------------
 MSR   SCTLR_EL2, xzr
 MSR   SCTLR_EL1, xzr




// Disable coprocessor traps
	ldr		x0, =CPACR_EL1_VALUE
ldr x1, =(3 << 20)
orr x0, x1, x0
	msr		cpacr_el1, x0

	ldr		x0, =(HCR_EL2_VALUE | 1 << 4 | 1 << 3)
	msr		hcr_el2, x0

	// Set the SPSR state to restore when returning from EL2 to EL1
	ldr		x0, =(0x05)
	msr		spsr_el3, x0

	adr		x0, el1_entry
	msr		elr_el3, x0

//	ldr		x0, =0x40000040
//	ldr		w14, [x0]
//	orr		w14, w14, #8
//	str		w14, [x0]

	eret

el1_entry:
        //setup sp
//        msr             SPSel, #0
//        ldr             x1, =__stack_end
//        and             x1, x1, #-16
//        mov             sp, x1
//        sub sp, sp, #8
//        str xzr, [sp]
//        add sp, sp, #8





//	bl 	create_page_tables
//        ldr    x0, =(0x80000000)
//        add    sp, x0, #0

//	adrp	x0, _pagetables				
//	msr		ttbr1_el1, x0		
//	msr		ttbr0_el1, x0

//	ldr		x0, =(TCR_VALUE)		
//	msr		tcr_el1, x0

//	ldr		x0, =(MAIR_VALUE)
//	msr		mair_el1, x0


 LDR   x0, =_pagetables
 MSR   TTBR0_EL1, x0          


 // Set up memory attributes
 // -------------------------
 // This equates to:
 // 0 = b01000100 = Normal, Inner/Outer Non-Cacheable
 // 1 = b11111111 = Normal, Inner/Outer WB/WA/RA
 // 2 = b00000000 = Device-nGnRnE
 MOV   x0, #0x000000000000FF44
 MSR   MAIR_EL1, x0



 // Set up TCR_EL1
 // ---------------
 MOV   x0, #0x19             // T0SZ=0b011001 Limits VA space to 39 bits
 ORR   x0, x0, #(0x1 << 8)   // IGRN0=0b01  Walks to TTBR0 are Inner WB/WA
 ORR   x0, x0, #(0x1 << 10)  // OGRN0=0b01  Walks to TTBR0 are Outer WB/WA
 ORR   x0, x0, #(0x3 << 12)  // SH0=0b11   Inner Shareable
 ORR   x0, x0, #(0x1 << 23)  // EPD1=0b1   Disable table walks from TTBR1
                             // TBI0=0b0
                             // TG0=0b00   4KB granule for TTBR0
                             // A1=0     TTBR0 contains the ASID
                             // AS=0     8-bit ASID
                             // IPS=0     32-bit IPA space
 MSR   TCR_EL1, x0


// TLBI  ALLE1
 DSB   SY
 ISB


LDR   x1, =_pagetables


  // [0]: 0x0000,0000 - 0x3FFF,FFFF
  LDR      x0, =0x00600000000000409          // Entry template
                                             // Don't need to OR in address, as it is 0
                                             // AP=0b00, EL1 RW, EL0 No Access
  STR      x0, [x1]
  
  // [1]: 0x4000,0000 - 0x7FFF,FFFF
  LDR      x0, =0x00600000000000409          // Entry template
  ORR      x0, x0, #0x40000000               // 'OR' template with base physical address
                                             // AP=0b00, EL1 RW, EL0 No Access
  STR      x0, [x1, #8]

  // [2]: 0x8000,0000 - 0xBFFF,FFFF (DRAM on the VE and Base Platform)
  LDR      x0, =0x00000000000000405            // Entry template
  ORR      x0, x0, #0x80000000               // 'OR' template with base physical address
                                             // AP=0b00, EL1 RW, EL0 No Access
  STR      x0, [x1, #16]

    // [3]: 0xC0000000 - 0xFFFF,FFFF (DRAM on the VE and Base Platform)
  LDR      x0, =0x00000000000000405            // Entry template
  ORR      x0, x0, #0xC0000000               // 'OR' template with base physical address
                                             // AP=0b00, EL1 RW, EL0 No Access
  STR      x0, [x1, #24]

  DSB      SY


	ldr		x4, =_libmorelloplat_entry



//	ldr		x5, =SCTLR_EL1_VALUE_MMU_ENABLED
//	msr		sctlr_el1, x5



 	MOV      x0, #(1 << 0)                     // M=1           Enable the stage 1 MMU
  	ORR      x0, x0, #(1 << 2)                 // C=1           Enable data and unified caches
  	ORR      x0, x0, #(1 << 12)                // I=1           Enable instruction fetches to allocate into unified caches
                                             // A=0           Strict alignment checking disabled
                                             // SA=0          Stack alignment checking disabled
                                             // WXN=0         Write permission does not imply XN
                                             // EE=0          EL3 data accesses are little endian
  MSR      SCTLR_EL1, x0
  ISB

  //
  // MMU is now enabled
  //

  NOP
  NOP
  NOP
  NOP


        msr             SPSel, #1
        ldr             x1, =__stack_end
        and             x1, x1, #-16
        mov             sp, x1



clear_bss_start:
	// Clear bss
//	ldr     x1, =__bss_start
//	ldr     w2, =__bss_size
clear_bss_loop:
//	cbz     w2, clear_bss_done
//	str     xzr, [x1], #8
//	sub     w2, w2, #1
//	cbnz    w2, clear_bss_loop
clear_bss_done:

  //      msr             SPSel, #1
  //      ldr             x1, =__stack_end
   //     and             x1, x1, #-16
   //     mov             sp, x1

jump_to_C:
// Recover initial timer value
	mov		w0, w10
	mov		w1, w11
	mov		w2, w12
	mov		w3, w13
//	b      _libmorelloplat_entry
	br 		x4

	// As a failsafe, we also hang the main core
    b       hang





	.macro	create_pgd_entry, tbl, virt, tmp1, tmp2
	create_table_entry \tbl, \virt, PGD_SHIFT, \tmp1, \tmp2
	create_table_entry \tbl, \virt, PUD_SHIFT, \tmp1, \tmp2
	.endm

	.macro	create_table_entry, tbl, virt, shift, tmp1, tmp2
	lsr	\tmp1, \virt, #\shift
	and	\tmp1, \tmp1, #PTRS_PER_TABLE - 1			// table index
	add	\tmp2, \tbl, #PAGE_SIZE
	orr	\tmp2, \tmp2, #MM_TYPE_PAGE_TABLE	
	str	\tmp2, [\tbl, \tmp1, lsl #3]
	add	\tbl, \tbl, #PAGE_SIZE					// next level table page
	.endm

	.macro	create_block_map, tbl, phys, start, end, flags, tmp1
	lsr	\start, \start, #SECTION_SHIFT
	and	\start, \start, #PTRS_PER_TABLE - 1			// table index
	lsr	\end, \end, #SECTION_SHIFT
	and	\end, \end, #PTRS_PER_TABLE - 1				// table end index
	lsr	\phys, \phys, #SECTION_SHIFT
	mov	\tmp1, #\flags
	orr	\phys, \tmp1, \phys, lsl #SECTION_SHIFT			// table entry
9999:	str	\phys, [\tbl, \start, lsl #3]				// store the entry
	add	\start, \start, #1					// next entry
	add	\phys, \phys, #SECTION_SIZE				// next block
	cmp	\start, \end
	b.ls	9999b
	.endm

create_page_tables:
	mov	x29, x30						// save return address

	adrp	x0, _pagetables
	mov	x1, #PG_DIR_SIZE
	bl 	memzero

	adrp	x0, _pagetables
	mov		x1, #VA_START 
	create_pgd_entry x0, x1, x2, x3

	/* Mapping kernel and init stack*/
	ldr 	x1,= (0x0)											// start mapping from physical offset 0
	mov 	x2, #VA_START									// first virtual address
	ldr	x3, =(VA_START + 0x3FFFFFFF)		// last virtual address
	create_block_map x0, x1, x2, x3, MMU_DEVICE_FLAGS, x4

		/* Mapping kernel and init stack*/
	ldr 	x1,= (0x40000000)											// start mapping from physical offset 0
	ldr 	x2, =(VA_START + 0x40000000)									// first virtual address
	ldr	x3, =(VA_START + 0x3FFFFFFF)		// last virtual address
	create_block_map x0, x1, x2, x3, MMU_DEVICE_FLAGS, x4

	/* Mapping device memory*/
	ldr 	x1, =(0x80000000)					// start mapping from device base address 
	ldr 	x2, =(VA_START + 0x80000000)				// first virtual address
	ldr	x3, =(VA_START + 0xBFFFFFFF)	// last virtual address
	create_block_map x0, x1, x2, x3, MMU_DEVICE_FLAGS, x4

	mov	x30, x29						// restore return address
	ret
