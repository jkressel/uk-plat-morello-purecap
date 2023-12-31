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
#include <uk/arch/limits.h>
#include <uk/plat/common/common.lds.h>

#define RAM_BASE_ADDR	0x80000000
#define DTB_RESERVED_SIZE 0x100000

ENTRY(__start)

SECTIONS
{
    . = RAM_BASE_ADDR;

	/* Place DTB binary at the beginning of the RAM */
	_dtb = .;
	. = . + DTB_RESERVED_SIZE;

	/* Code */
	.text :
	{
		KEEP(*(.text.boot))
                *(.text)
		*(.text.*)
		*(.gnu.linkonce.t*)
	}

	__start_text = ADDR(.text);
	__stop_text = ALIGN(__start_text + SIZEOF(.text), 128);
/* __FLEXOS MARKER__: insert compartment text sections here. */


	. = ALIGN(__PAGE_SIZE);
	EXCEPTION_SECTIONS

	CTORTAB_SECTION

	INITTAB_SECTION

	/* Read-only data */
	. = ALIGN(__PAGE_SIZE);
	.rodata :
	{
		*(.rodata)
		*(.rodata.*)
		*(.gnu.linkonce.r*)
	}
	_erodata = .;

	__start_rodata = ADDR(.rodata);
	__stop_rodata = ALIGN(__start_rodata + SIZEOF(.rodata), 128);



	/* Constructor tables (read-only) */
	. = ALIGN(0x10);
	.preinit_array : {
		PROVIDE_HIDDEN (__preinit_array_begin = .);
		KEEP (*(.preinit_array))
		PROVIDE_HIDDEN (__preinit_array_end = .);
	}

	.init_array : {
		PROVIDE_HIDDEN (__init_array_begin = .);
		KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
		KEEP (*(.init_array .ctors))
		PROVIDE_HIDDEN (__init_array_end = .);
	}

	__start_ctors = ADDR(.preinit_array);
	__stop_ctors = ALIGN(__start_ctors + SIZEOF(.preinit_array) + SIZEOF(.init_array), 128);

		/* Read-write data that is initialized explicitly in code */
	.data :
	{
		*(.data)
		*(.data.*)
		*(.gnu.linkonce.d*)
		PROVIDE(flexos_comp0_alloc = .);
		/* here: enough to store the compartment's allocator
		 * FIXME derive from roundup(sizeof(struct uk_alloc)) */
		. = . + (1000*__PAGE_SIZE);
		. = ALIGN(0x1000);
	}
	_edata = .;

	__start_data = ADDR(.data);
	__stop_data = ALIGN(__start_data + SIZEOF(.data), 128);

. = ALIGN(__PAGE_SIZE);
	__cap_relocs : {
		*(__cap_relocs)
	}

	__start_relocs = ADDR(__cap_relocs);
	__stop_relocs = ALIGN(ADDR(__cap_relocs) + SIZEOF(__cap_relocs), 128);

	/* Read-write data that is not initialized explicitly in code */
	. = ALIGN(__PAGE_SIZE);
    .bss :
	{
		*(.bss)
		*(.bss.*)
        *(COMMON)
		PROVIDE(__stack_begin = .);
                . += 128 * __PAGE_SIZE;
        PROVIDE(__stack_end = .);
    }
	__bss_end = .;

	__start_bss = ADDR(.bss);
	__stop_bss = ALIGN(__start_bss + SIZEOF(.bss), 128);

	. = ALIGN(__PAGE_SIZE);
	TLS_SECTIONS


    . = ALIGN(__PAGE_SIZE);
	. = ALIGN(16);

	. = ALIGN(__PAGE_SIZE);
	_pagetables = .;
	.pagetables (NOLOAD) :
	{
		. += 3 * __PAGE_SIZE;
	}

	_end = .;
	

	. = ALIGN(__PAGE_SIZE);
	.comment       0 : { *(.comment) }
	DEBUG_SYMBOLS

   /DISCARD/ : { *(.gnu*) *(.note*) }
}
__bss_size = (__stop_bss - __start_bss)>>3;