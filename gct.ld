OUTPUT_FORMAT("elf32-powerpc", "elf32-powerpc", "elf32-powerpc");
OUTPUT_ARCH(powerpc:common);
__entry = 0;
ENTRY(__entry)

MEMORY
{
	code (rwx) : ORIGIN = 0x00000000, LENGTH = 0x01800000
}

SECTIONS
{
	.text : {
		KEEP (*(.c0_start))

		. = ALIGN(4);

		_G_GOTStart = .;
		*(.got2)
		_G_GOTEnd = .;

		_G_FixupStart = .;
		KEEP(*(.fixup))
		_G_FixupEnd = .;

		_G_ImportStart = .;
		*(.gct_import.*)
		_G_ImportEnd = .;

		*(.text*)
		*(.insert*)
		*(.rodata*)
		*(.data*)
		*(.bss*)

		__SDATA_START__ = .;
		*(.sdata*)
		__SDATA_END__ = .;

		__SDATA2_START__ = .;
		*(.sdata2*)
		__SDATA2_END__ = .;

		__SBSS_START__ = .;
		*(.sbss*)
		__SBSS_END__ = .;

		__SBSS2_START__ = .;
		*(.sbss2*)
		__SBSS2_END__ = .;

		. = ALIGN(8);
		_G_C0End = .;

		KEEP (*(.gct_data))
		KEEP (*(.c0_header))
	} > code

	/DISCARD/ : {
		*(.comment*)
		*(.eh_frame*)
		*(.eh_frame_hdr*)
	}
}
