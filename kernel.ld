OUTPUT_FORMAT(binary)
OUTPUT_ARCH(i386)

ENTRY(setup_start)

SECTIONS {
    . = 0x8000;

    _kernel_start = .;

    .text : { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    .bss : { *(.bss) }

    _kernel_end = .;
}