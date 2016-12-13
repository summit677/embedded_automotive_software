
/*
 * Name: ee_tc2Yx_global_gnu_ram.ld 
 * 
 * Description: 
 * Linker rules for RAM configuration multicore with single elf
 */

/* Slaves' entry points */
EE_tc2Yx_cpu1_start = 0x60102100;
EE_tc2Yx_cpu2_start = 0x50102100;

/* Following sections are global sections */
CORE_ID = GLOBAL ;

SECTIONS
{
  /*
   * Startup code for TriCore
   */
  .startup : FLAGS(rxl)
  {
    KEEP (*(.startup))
  } > PMI_PSPR0

  /*
   * Code sections
   */
  EE_tc2Yx_cpu1_start (0x60102100) :
  {
    KEEP(*(*.EE_tc2Yx_cpu1_start))
  }

  EE_tc2Yx_cpu2_start (0x50102100) :
  {
    KEEP(*(*.EE_tc2Yx_cpu2_start))
  }
}

/* Following sections are CPU0 sections */
CORE_ID = CPU0 ;

SECTIONS
{
  /*
   * Allocate .text and other read-only sections.
   */
  /*
   * Code executed before calling main
   */
  CORE_SEC(.init) :
  {
    *(CORE_SEC(.init))
  } > PMI_PSPR0

  CORE_SEC(.cramtext) :
  {
    *(CORE_SEC(.cramtext))
  } > PMI_PSPR0

  /*
   * Section for trap table
   */
  CORE_SEC(.traptab) : FLAGS(axl)
  {
    KEEP(*(CORE_SEC(.traptab)))   /* Section for trap table */ 
  } > PMI_PSPR0

  /*
   * Section for interrupt table
   */
  CORE_SEC(.inttab) : FLAGS(axl)
  {
    KEEP(*(CORE_SEC(.inttab))) 
  } > PMI_PSPR0

  /*
   * Code sections
   */
  CORE_SEC(.text) : FLAGS(axl)
  {
    *(CORE_SEC(.text))
  } > PMI_PSPR0

  /*
   * C++ exception handling tables.  NOTE: gcc emits .eh_frame
   * sections when compiling C sources with debugging enabled (-g).
   * If you can be sure that your final application consists
   * exclusively of C objects (i.e., no C++ objects), you may use
   * the -R option of the "strip" and "objcopy" utilities to remove
   * the .eh_frame section from the executable.
   */
  CORE_SEC(.eh_frame) : FLAGS(axl)
  {
    *(CORE_SEC(.eh_frame))
  } > PMI_PSPR0

  /* OS Public Code */
  CORE_SEC(.ee_api_text) : ALIGN(8) FLAGS(axl) {
    *(CORE_SEC(.ee_api_text))
  } > PMI_PSPR0

  /* OS Private Code */
  CORE_SEC(.ee_kernel_text) : ALIGN(16) FLAGS(axl) {
    *(CORE_SEC(.ee_kernel_text))
  } > PMI_PSPR0

  /* OS-Applications Private Code */
  CORE_SEC(.ee_apps_text) : ALIGN(16) FLAGS(axl) {
    *(CORE_SEC(.ee_*_text))
  } > PMI_PSPR0

  /*
   * Constructors and destructors.
   */
  CORE_SEC(.ctors) : FLAGS(axl)
  {
    *(CORE_SEC(.ctors))
  } > PMI_PSPR0

  CORE_SEC(.dtors) : FLAGS(axl)
  {
    *(CORE_SEC(.dtors))
  } > PMI_PSPR0

  /*
   * Allocate space for absolute addressable sections; this requires that
   * "int_dspr" starts at a TriCore segment (256M) and points to
   * some RAM area!  If these conditions are not met by your particular
   * hardware setup, you should either not use absolute data, or you
   * must move .zdata*,.zbss*,.bdata*,.bbss* input sections to some appropriate
   * memory area.
   */
  CORE_SEC(.zbss) (NOLOAD) : FLAGS(awz)
  {
    *(CORE_SEC(.zbss))
  } > DMI_DSPR0

  CORE_SEC(.zdata) : FLAGS(awzl)
  {
    *(CORE_SEC(.zdata))
  } > DMI_DSPR0

  /*
   * Not initialised data in section ’.sbss’, addressable by small data area pointer (%a0)
   */
  CORE_SEC(.sbss) (NOLOAD) : FLAGS(aws)
  {
    *(CORE_SEC(.sbss))
  } > DMI_DSPR0

  /*
   * Initialised data in section ’.sdata’, addressable by small data area pointer (%a0)
   */
  CORE_SEC(.sdata) : ALIGN(8)
  {
    *(CORE_SEC(.sdata))
  } > DMI_DSPR0

  /*
   * Storage of write-protected data
   */
  CORE_SEC(.rodata) : FLAGS(arl)
  {
    *(CORE_SEC(.rodata))
  } > DMI_DSPR0 /* DMI_DSPR: Scratch-Pad RAM (DSPR) */ 

  CORE_SEC(.sdata2) : FLAGS(arsl)
  {
    *(CORE_SEC(.sdata2))
  } > DMI_DSPR0

  /*
   * Not Initialised data.
   * Allocate space for BSS sections.
   */
  CORE_SEC(.bss) (NOLOAD) :
  {
    *(CORE_SEC(.bss))
  } > DMI_DSPR0

  /*
   * Initialised data
   */
  CORE_SEC(.data) : ALIGN(8)
  {
    *(CORE_SEC(.data))
  } > DMI_DSPR0

  /*
   * Section for ustack
   */  
  CORE_SEC(.ustack) : ALIGN(8)
  {
    *(CORE_SEC(.ustack))
  } > DMI_DSPR0

  /*
   * Section for istack
   */  
  CORE_SEC(.istack) : ALIGN(8)
  {
    *(CORE_SEC(.istack))
  } > DMI_DSPR0

  /*
   * Section for heap
   */  
  CORE_SEC(.heap) : ALIGN(8)
  {
    *(CORE_SEC(.heap))
  } > DMI_DSPR0

  /*
   * Section for CSA
   */  
  CORE_SEC(.csa) : ALIGN(64)
  {
    *(CORE_SEC(.csa))
  } > DMI_DSPR0

  /* ERIKA API Sections */

  /* ERIKA API public unitializated data section */
  CORE_SEC(.ee_api_bss) : ALIGN(8) {
    *(CORE_SEC(.ee_api_bss))
  } > DMI_DSPR0

  /* ERIKA API public initializated data DATA */
  CORE_SEC(.ee_api_data) : ALIGN(8) {
    *(CORE_SEC(.ee_api_data))
  } > DMI_DSPR0

  /* ERIKA Kernel Sections */

  /* ERIKA Kernel private unitializated data section */
  CORE_SEC(.ee_kernel_bss) : ALIGN(8) {
    *(CORE_SEC(.ee_kernel_bss))
  } > DMI_DSPR0

  /* ERIKA Kernel private initializated data DATA */
  CORE_SEC(.ee_kernel_data) : ALIGN(8) {
    *(CORE_SEC(.ee_kernel_data))
  } > DMI_DSPR0

  /* ERIKA Kenel Stacks (Don't belong to any OS-Applications, special section
     is needed to handle memory protection and stack monitoring) */
  CORE_SEC(.ee_kernel_stack) : ALIGN(8)
  {
    *(CORE_SEC(.ee_kernel_stack))
  } > DMI_DSPR0

  /* Start OS-Application Data Section Symbol. */
  CORE_SEC(.ee_apps_sec) : ALIGN(16) {
    *(CORE_SEC(.ee_*_sec))
  } > DMI_DSPR0
}

/* Following sections are CPU1 sections */
CORE_ID = CPU1 ;

SECTIONS
{
  /*
   * Allocate .text and other read-only sections.
   */
  /*
   * Code executed before calling main
   */
  CORE_SEC(.init) :
  {
    *(CORE_SEC(.init))
  } > PMI_PSPR1

  CORE_SEC(.cramtext) :
  {
    *(CORE_SEC(.cramtext))
  } > PMI_PSPR1

  /*
   * Section for trap table
   */
  CORE_SEC(.traptab) : FLAGS(axl)
  {
    KEEP(*(CORE_SEC(.traptab)))   /* Section for trap table */ 
  } > PMI_PSPR1

  /*
   * Section for interrupt table
   */
  CORE_SEC(.inttab) : FLAGS(axl)
  {
    KEEP(*(CORE_SEC(.inttab))) 
  } > PMI_PSPR1

  /*
   * Code sections
   */
  CORE_SEC(.text) : FLAGS(axl)
  {
    *(CORE_SEC(.text))
  } > PMI_PSPR1

  /*
   * C++ exception handling tables.  NOTE: gcc emits .eh_frame
   * sections when compiling C sources with debugging enabled (-g).
   * If you can be sure that your final application consists
   * exclusively of C objects (i.e., no C++ objects), you may use
   * the -R option of the "strip" and "objcopy" utilities to remove
   * the .eh_frame section from the executable.
   */
  CORE_SEC(.eh_frame) : FLAGS(axl)
  {
    *(CORE_SEC(.eh_frame))
  } > PMI_PSPR1

  /* OS Public Code */
  CORE_SEC(.ee_api_text) : ALIGN(8) FLAGS(axl) {
    *(CORE_SEC(.ee_api_text))
  } > PMI_PSPR1

  /* OS Private Code */
  CORE_SEC(.ee_kernel_text) : ALIGN(16) FLAGS(axl) {
    *(CORE_SEC(.ee_kernel_text))
  } > PMI_PSPR1

  /* OS-Applications Private Code */
  CORE_SEC(.ee_apps_text) : ALIGN(16) FLAGS(axl) {
    *(CORE_SEC(.ee_*_text))
  } > PMI_PSPR1

  /*
   * Constructors and destructors.
   */
  CORE_SEC(.ctors) : FLAGS(axl)
  {
    *(CORE_SEC(.ctors))
  } > PMI_PSPR1

  CORE_SEC(.dtors) : FLAGS(axl)
  {
    *(CORE_SEC(.dtors))
  } > PMI_PSPR1

  /*
   * Allocate space for absolute addressable sections; this requires that
   * "int_dspr" starts at a TriCore segment (256M) and points to
   * some RAM area!  If these conditions are not met by your particular
   * hardware setup, you should either not use absolute data, or you
   * must move .zdata*,.zbss*,.bdata*,.bbss* input sections to some appropriate
   * memory area.
   */
  CORE_SEC(.zbss) (NOLOAD) : FLAGS(awz)
  {
    *(CORE_SEC(.zbss))
  } > DMI_DSPR1

  CORE_SEC(.zdata) : FLAGS(awzl)
  {
    *(CORE_SEC(.zdata))
  } > DMI_DSPR1

  /*
   * Not initialised data in section ’.sbss’, addressable by small data area pointer (%a0)
   */
  CORE_SEC(.sbss) (NOLOAD) : FLAGS(aws)
  {
    *(CORE_SEC(.sbss))
  } > DMI_DSPR1

  /*
   * Initialised data in section ’.sdata’, addressable by small data area pointer (%a0)
   */
  CORE_SEC(.sdata) : ALIGN(8)
  {
    *(CORE_SEC(.sdata))
  } > DMI_DSPR1 /* DMI_DSPR: Local Data RAM (DSPR) */

  /*
   * Storage of write-protected data
   */
  CORE_SEC(.rodata) : FLAGS(arl)
  {
    *(CORE_SEC(.rodata))
  } > DMI_DSPR1

  CORE_SEC(.sdata2) : FLAGS(arsl)
  {
    *(CORE_SEC(.sdata2))
  } > DMI_DSPR1

  /*
   * Not Initialised data.
   * Allocate space for BSS sections.
   */
  CORE_SEC(.bss) (NOLOAD) :
  {
    *(CORE_SEC(.bss))
  } > DMI_DSPR1

  /*
   * Initialised data
   */
  CORE_SEC(.data) : ALIGN(8)
  {
    *(CORE_SEC(.data))
  } > DMI_DSPR1

  /*
   * Section for ustack
   */  
  CORE_SEC(.ustack) : ALIGN(8)
  {
    *(CORE_SEC(.ustack))
  } > DMI_DSPR1

  /*
   * Section for istack
   */  
  CORE_SEC(.istack) : ALIGN(8)
  {
    *(CORE_SEC(.istack))
  } > DMI_DSPR1

  /*
   * Section for heap
   */  
  CORE_SEC(.heap) : ALIGN(8)
  {
    *(CORE_SEC(.heap))
  } > DMI_DSPR1

  /*
   * Section for CSA
   */  
  CORE_SEC(.csa) : ALIGN(64)
  {
    *(CORE_SEC(.csa))
  } > DMI_DSPR1

  /* ERIKA API Sections */

  /* ERIKA API public unitializated data section */
  CORE_SEC(.ee_api_bss) : ALIGN(8) {
    *(CORE_SEC(.ee_api_bss))
  } > DMI_DSPR1

  /* ERIKA API public initializated data DATA */
  CORE_SEC(.ee_api_data) : ALIGN(8) {
    *(CORE_SEC(.ee_api_data))
  } > DMI_DSPR1

  /* ERIKA Kernel Sections */

  /* ERIKA Kernel private unitializated data section */
  CORE_SEC(.ee_kernel_bss) : ALIGN(8) {
    *(CORE_SEC(.ee_kernel_bss))
  } > DMI_DSPR1

  /* ERIKA Kernel private initializated data DATA */
  CORE_SEC(.ee_kernel_data) : ALIGN(8) {
    *(CORE_SEC(.ee_kernel_data))
  } > DMI_DSPR1

  /* ERIKA Kenel Stacks (Don't belong to any OS-Applications, special section
     is needed to handle memory protection and stack monitoring) */
  CORE_SEC(.ee_kernel_stack) : ALIGN(8)
  {
    *(CORE_SEC(.ee_kernel_stack))
  } > DMI_DSPR1

  /* Start OS-Application Data Section Symbol. */
  CORE_SEC(.ee_apps_sec) : ALIGN(16) {
    *(CORE_SEC(.ee_*_sec))
  } > DMI_DSPR1
}

/* Following sections are CPU2 sections */
CORE_ID = CPU2 ;

SECTIONS
{
  /*
   * Allocate .text and other read-only sections.
   */
  /*
   * Code executed before calling main
   */
  CORE_SEC(.init) :
  {
    *(CORE_SEC(.init))
  } > PMI_PSPR2

  CORE_SEC(.cramtext) :
  {
    *(CORE_SEC(.cramtext))
  } > PMI_PSPR2

  /*
   * Section for trap table
   */
  CORE_SEC(.traptab) : FLAGS(axl)
  {
    KEEP(*(CORE_SEC(.traptab)))   /* Section for trap table */ 
  } > PMI_PSPR2

  /*
   * Section for interrupt table
   */
  CORE_SEC(.inttab) : FLAGS(axl)
  {
    KEEP(*(CORE_SEC(.inttab))) 
  } > PMI_PSPR2

  /*
   * Code sections
   */
  CORE_SEC(.text) : FLAGS(axl)
  {
    *(CORE_SEC(.text))
  } > PMI_PSPR2

  /*
   * C++ exception handling tables.  NOTE: gcc emits .eh_frame
   * sections when compiling C sources with debugging enabled (-g).
   * If you can be sure that your final application consists
   * exclusively of C objects (i.e., no C++ objects), you may use
   * the -R option of the "strip" and "objcopy" utilities to remove
   * the .eh_frame section from the executable.
   */
  CORE_SEC(.eh_frame) : FLAGS(axl)
  {
    *(CORE_SEC(.eh_frame))
  } > PMI_PSPR2

  /* OS Public Code */
  CORE_SEC(.ee_api_text) : ALIGN(8) FLAGS(axl) {
    *(CORE_SEC(.ee_api_text))
  } > PMI_PSPR2

  /* OS Private Code */
  CORE_SEC(.ee_kernel_text) : ALIGN(16) FLAGS(axl) {
    *(CORE_SEC(.ee_kernel_text))
  } > PMI_PSPR2

  /* OS-Applications Private Code */
  CORE_SEC(.ee_apps_text) : ALIGN(16) FLAGS(axl) {
    *(CORE_SEC(.ee_*_text))
  } > PMI_PSPR2

  /*
   * Constructors and destructors.
   */
  CORE_SEC(.ctors) : FLAGS(axl)
  {
    *(CORE_SEC(.ctors))
  } > PMI_PSPR2

  CORE_SEC(.dtors) : FLAGS(axl)
  {
    *(CORE_SEC(.dtors))
  } > PMI_PSPR2

  /*
   * Allocate space for absolute addressable sections; this requires that
   * "int_dspr" starts at a TriCore segment (256M) and points to
   * some RAM area!  If these conditions are not met by your particular
   * hardware setup, you should either not use absolute data, or you
   * must move .zdata*,.zbss*,.bdata*,.bbss* input sections to some appropriate
   * memory area.
   */
  CORE_SEC(.zbss) (NOLOAD) : FLAGS(awz)
  {
    *(CORE_SEC(.zbss))
  } > DMI_DSPR2

  CORE_SEC(.zdata) : FLAGS(awzl)
  {
    *(CORE_SEC(.zdata))
  } > DMI_DSPR2

  /*
   * Not initialised data in section ’.sbss’, addressable by small data area pointer (%a0)
   */
  CORE_SEC(.sbss) (NOLOAD) : FLAGS(aws)
  {
    *(CORE_SEC(.sbss))
  } > DMI_DSPR2

  /*
   * Initialised data in section ’.sdata’, addressable by small data area pointer (%a0)
   */
  CORE_SEC(.sdata) : ALIGN(8)
  {
    *(CORE_SEC(.sdata))
  } > DMI_DSPR2

  /*
   * Storage of write-protected data
   */
  CORE_SEC(.rodata) : FLAGS(arl)
  {
    *(CORE_SEC(.rodata))
  } > DMI_DSPR1

  CORE_SEC(.sdata2) : FLAGS(arsl)
  {
    *(CORE_SEC(.sdata2))
  } > DMI_DSPR1

  /*
   * Not Initialised data.
   * Allocate space for BSS sections.
   */
  CORE_SEC(.bss) (NOLOAD) :
  {
    *(CORE_SEC(.bss))
  } > DMI_DSPR2

  /*
   * Initialised data
   */
  CORE_SEC(.data) : ALIGN(8)
  {
    *(CORE_SEC(.data))
  } > DMI_DSPR2

  /*
   * Section for ustack
   */  
  CORE_SEC(.ustack) : ALIGN(8)
  {
    *(CORE_SEC(.ustack))
  } > DMI_DSPR2

  /*
   * Section for istack
   */  
  CORE_SEC(.istack) : ALIGN(8)
  {
    *(CORE_SEC(.istack))
  } > DMI_DSPR2

  /*
   * Section for heap
   */  
  CORE_SEC(.heap) : ALIGN(8)
  {
    *(CORE_SEC(.heap))
  } > DMI_DSPR2

  /*
   * Section for CSA
   */  
  CORE_SEC(.csa) : ALIGN(64)
  {
    *(CORE_SEC(.csa))
  } > DMI_DSPR2

  /* ERIKA API Sections */

  /* ERIKA API public unitializated data section */
  CORE_SEC(.ee_api_bss) : ALIGN(8) {
    *(CORE_SEC(.ee_api_bss))
  } > DMI_DSPR2

  /* ERIKA API public initializated data DATA */
  CORE_SEC(.ee_api_data) : ALIGN(8) {
    *(CORE_SEC(.ee_api_data))
  } > DMI_DSPR2

  /* ERIKA Kernel Sections */

  /* ERIKA Kernel private unitializated data section */
  CORE_SEC(.ee_kernel_bss) : ALIGN(8) {
    *(CORE_SEC(.ee_kernel_bss))
  } > DMI_DSPR2

  /* ERIKA Kernel private initializated data DATA */
  CORE_SEC(.ee_kernel_data) : ALIGN(8) {
    *(CORE_SEC(.ee_kernel_data))
  } > DMI_DSPR2

  /* ERIKA Kenel Stacks (Don't belong to any OS-Applications, special section
     is needed to handle memory protection and stack monitoring) */
  CORE_SEC(.ee_kernel_stack) : ALIGN(8)
  {
    *(CORE_SEC(.ee_kernel_stack))
  } > DMI_DSPR2

  /* Start OS-Application Data Section Symbol. */
  CORE_SEC(.ee_apps_sec) : ALIGN(16) {
    *(CORE_SEC(.ee_*_sec))
  } > DMI_DSPR2
}

/* Following sections are global sections */
CORE_ID = GLOBAL ;

SECTIONS
{
  /* Global section to share code symbol between cores */
  ee_mcglobalt :
  {
    KEEP(*(ee_mcglobalt))
  } > PMI_PSPR0 /* PMI_PSPR: Scratch-Pad RAM (PSPR) */

  /* Erika RTOS Multicore constant global data (in RAM configuration, locator
     rule have to be explicit otherwise they will tied wrongly with a ROM
     memory) */
  ee_mcglobalc :
  {
    KEEP(*(ee_mcglobalc))
  } > DMI_DSPR1 /* DMI_DSPR: Scratch-Pad RAM (DSPR) */

  /* Erika RTOS Multicore global data */
  ee_mcglobald :
  {
    KEEP(*(ee_mcglobald))
  } > LMU_SRAM

  /*
   * DWARF debug sections.
   * Symbols in the DWARF debugging sections are relative to the
   * beginning of the section, so we begin them at 0.
   */
  /*
   * DWARF 1
   */
  .comment         0 : { *(.comment) }
  .debug           0 : { *(.debug) }
  .line            0 : { *(.line) }
  /*
   * GNU DWARF 1 extensions
   */
  .debug_srcinfo   0 : { *(.debug_srcinfo) }
  .debug_sfnames   0 : { *(.debug_sfnames) }
  /*
   * DWARF 1.1 and DWARF 2
   */
  .debug_aranges   0 : { *(.debug_aranges) }
  .debug_pubnames  0 : { *(.debug_pubnames) }
  /*
   * DWARF 2
   */
  .debug_info      0 : { *(.debug_info) }
  .debug_abbrev    0 : { *(.debug_abbrev) }
  .debug_line      0 : { *(.debug_line) }
  .debug_frame     0 : { *(.debug_frame) }
  .debug_str       0 : { *(.debug_str) }
  .debug_loc       0 : { *(.debug_loc) }
  .debug_macinfo   0 : { *(.debug_macinfo) }
  .debug_ranges    0 : { *(.debug_ranges) }
  /*
   * SGI/MIPS DWARF 2 extensions
   */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }
  /*
   * Optional sections that may only appear when relocating.
   */
  /*
   * Optional sections that may appear regardless of relocating.
   */
  .version_info    0 : { *(.version_info) }
  .boffs           0 : { KEEP (*(.boffs)) }
}

