OUTPUT_FORMAT("elf32-i386")
ENTRY(entry)
LS_Phys = 0x100000;

SECTIONS
{
  .text LS_Phys : AT(LS_Phys)
  {
    PROVIDE(kernel_start_address = ABSOLUTE(.));

    LS_Code = .;
    text_start_address = .;
    . = ALIGN(8);
    multiboot_header_start = .;
    KEEP(*(.mboot))
    multiboot_header_end = .;
    *(.text)
    *(.text.*)
    text_end_address = .;
    *(.gnu.linkonce.r.*)
    ro_data_start_address = .;
    *(.rodata*)
    ro_data_end_address = .;
    . = ALIGN(4096);
  }

  .data  ALIGN(4096) : AT(LS_Phys + (LS_Data - LS_Code))
  {
    LS_Data = .;
    data_start_address = .;
    *(.data)
    *(COMMON)
    data_end_address = .;
    . = ALIGN(4096);
  }

  .bss : AT(LS_Phys + (LS_Bss - LS_Code))
  {
    LS_Bss = .;
    bss_start_address = .;
    . = . + 4096;
    boot_stack_top_address = .;
    *(.bss)
    . = ALIGN(4096);
    bss_end_address = .;
  }
}

multiboot_header_end = multiboot_header_end - multiboot_header_start;
