/*
 * Name: ee_tc29x_gnu_flash_slaves_entry_points.ld
 *
 * Description: TC29x RAM CPU slave entry points
 */

/* Slaves' entry points */
EE_tc2Yx_cpu1_start = 0x60100000;
EE_tc2Yx_cpu2_start = 0x50100000;
/*
 * Name: ee_tc29x_gnu_memory_singlecore.x
 *
 * Description: TC29x memory description (only CPU0)
 */

/*
 * Global
 */

OUTPUT_FORMAT("elf32-tricore")
OUTPUT_ARCH(tricore)
ENTRY(_START)

 __TRICORE_DERIVATE_MEMORY_MAP__ = 0x290;

/* Start-up Program Flash Memory */
__PMU_PFLASH_STARTUP_BEGIN = 0x80000000;
__PMU_PFLASH_STARTUP_SIZE  = 1k;
/* Program Flash Memory (PFLASH0) */
__PMU_PFLASH_BEGIN = 0x80004000;
__PMU_PFLASH_SIZE  = 1984k;
/* Program Flash Memory (PFLASH1) */
__PMU_PFLASH1_BEGIN = 0x80200000;
__PMU_PFLASH1_SIZE = 2M;
/* Program Flash Memory (PFLASH2) */
__PMU_PFLASH1_BEGIN = 0x80400000;
__PMU_PFLASH1_SIZE = 2M;
/* Program Flash Memory (PFLASH3) */
__PMU_PFLASH1_BEGIN = 0x80600000;
__PMU_PFLASH1_SIZE = 2M;
/* Data Flash Memory (DFLASH0) */
__PMU_DFLASH0_BEGIN = 0xAF000000;
__PMU_DFLASH0_SIZE = 1M;
/* Data Flash Memory (DFLASH0_1) */
__PMU_DFLASH0_1_BEGIN = 0xAF100000;
__PMU_DFLASH0_1_SIZE = 16K;
/* Data Flash Memory (DFLASH1) */
__PMU_DFLASH1_BEGIN = 0xAF110000;
__PMU_DFLASH1_SIZE = 64K;
/* Boot ROM (BROM) */
__BROM_BEGIN = 0x8FFF8000;
__BROM_SIZE = 32K;
/* Scratch-Pad RAM (PSPR) */
__PMI_PSPR_BEGIN = 0xC0000000;
__PMI_PSPR_SIZE = 32K;
/* Local Data RAM (DSPR) */
__DMI_DSPR_BEGIN = 0xD0000000;
__DMI_DSPR_SIZE = 120K;
/* Local Data RAM (DSPR) */
__LMU_SRAM_BEGIN = 0x90000000;
__LMU_SRAM_SIZE = 32K;

/* CSA list size */
__CSA_SIZE = DEFINED (__CSA_SIZE) ? __CSA_SIZE : 8k ;
/* User stack Size */
__USTACK_SIZE = DEFINED (__USTACK_SIZE) ? __USTACK_SIZE : 2K;
/* Interrupt Stack Size */
__ISTACK_SIZE = DEFINED (__ISTACK_SIZE) ? __ISTACK_SIZE : 0K;
/* Heap size */
__HEAP_SIZE = DEFINED (__HEAP_SIZE) ? __HEAP_SIZE : 0;

MEMORY
{
  /* - 0x(8)A0000000--0x(8)A000001F ABM Header Start */
  /* - 0x(8)A0000020 Boot Address */
  /* - 0x(A)80000020--0x(A)80017FFF Code Application Pattern */
  /* - Program Flash Memory for startup (PFLASH0) */
  PMU_PFLASH_STARTUP (rx!p): org = 0x80000000, len = 1k
  /* ! COMMENTED SUPPORT FOR ABS18 ADRESSING IN FLASH ! */
  /* - Program Flash Memory for 18 bit absolute addressing (PFLASH0) */
  /* PMU_PFLASH_ABS18  (rx!p): org = 0x80000400, len = 15k */
  /* - Program Flash Memory (PFLASH0) I take 16k for absolute 18 bit
       addressing */
  /* PMU_PFLASH  (rx!p):  org = 0x80004000, len = 1984k */
  /* ! END COMMENT ! */
  PMU_PFLASH  (rx!p):  org = 0x80000400, len = 1999k
  /* - Program Flash Memory (PFLASH1) */
  PMU_PFLASH1 (rx!p):  org = 0x80200000, len = 2M
  /* - Program Flash Memory (PFLASH2) */
  PMU_PFLASH2 (rx!p):  org = 0x80400000, len = 2M
  /* - Program Flash Memory (PFLASH3) */
  PMU_PFLASH3 (rx!p):  org = 0x80600000, len = 2M
  /* - Data Flash Memory (DFLASH0) */
  PMU_DFLASH0 (r!xp):  org = 0xAF000000, len = 1M
  /* - Data Flash Memory (DFLASH0_1) */
  PMU_DFLASH0_1 (r!xp):  org = 0xAF100000, len = 16K
  /* - Data Flash Memory (DFLASH1) */
  PMU_DFLASH1 (r!xp):  org = 0xAF110000, len = 64K
  /* - Boot ROM (BROM) */
  BROM (rx!p):  org = 0x8FFF8000, len = 32K

  /* - Scratch-Pad RAM (PSPR) */
  PMI_PSPR          (wx!p):  org = 0xC0000000, len = 32k

  /* - Local Data Scratch-Pad RAM (DSPR) */
  DMI_DSPR (w!xp): org = 0xD0000000, len = 120K
  /* - Global Data RAM */
  LMU_SRAM (w!xp):  org = 0x90000000, len = 32K
}

/*
REGION_ALIAS("DATA_MEM", int_dram)
REGION_ALIAS("CODE_MEM", int_cflash)
REGION_ALIAS("SDATA_MEM", int_dram)
REGION_ALIAS("BSS_MEM", int_dram)
REGION_ALIAS("ZDATA_MEM", int_dram)
REGION_ALIAS("CSA_MEM", int_dram)
 dummy mapping (needed by default linker script)
REGION_ALIAS("PCP_CODE", int_cflash)
REGION_ALIAS("PCP_DATA", int_dflash)
*/

/* the symbol __TRICORE_DERIVATE_NAME__ will be defined in the crt0.S and is
 * tested here to confirm that this memory map and the startup file will
 * fit together
*/
/*
_. = ASSERT ((__TRICORE_DERIVATE_MEMORY_MAP__ == __TRICORE_DERIVATE_NAME__), "Using wrong Memory Map. This Map is for TC29x");
*/
/* INSERT BEFORE .startup */

/*
 * Name: ee_tc2Yx_gnu_ram.ld.tmpl
 * 
 * Description: 
 * Template for linker rules for RAM configuration
 */

SECTIONS
{
  /*
   * Startup code for TriCore
   */
  .startup : FLAGS(rxl)
  {
    BootModeIndex = .;
    PROVIDE(__startup_code_start = .);
    KEEP(*(*.EE_tc2Yx_cpu1_start))
    KEEP(*(*.EE_tc2Yx_cpu2_start))
    KEEP (*(.startup))
    KEEP (*(ee_kernel_start))
    . = ALIGN(8);
    PROVIDE(__startup_code_end = .);
  } > PMI_PSPR /* PMI_PSPR: Scratch-Pad RAM (PSPR) */

  /*
   * Allocate .text and other read-only sections.
   */

  /*
   * Code executed before calling main
   */
  CORE_SEC(.init) : FLAGS(axl)
  {
    *(.init)
    *(.fini)
    . = ALIGN(8);
  } > PMI_PSPR

  CORE_SEC(.cramtext) : FLAGS(axl)
  {
    *(.cramtext)
    *(.cramtext.*)
    . = ALIGN(8);
  } > PMI_PSPR

  /*
   * Section for trap table
   */
  CORE_SEC(.traptab) : ALIGN(32) FLAGS(axl)
  {
    PROVIDE(__traptab_start = .);

    KEEP(*(.traptab))   /*Section for trap table*/ 
    KEEP(*(.traptab.*))
    . = ALIGN(8);
    PROVIDE(__traptab_end = .);
  } > PMI_PSPR /* PMI_PSPR: Scratch-Pad RAM (PSPR) */ 

  /*
   * Section for interrupt table
   */
  CORE_SEC(.inttab) : ALIGN(32) FLAGS(axl)
  {
    PROVIDE(__inttab_start = .);
    KEEP (*(.inttab));
    KEEP (*(.*.inttab.*));
    . = ALIGN(8);
    PROVIDE(__inttab_end = .);
  } > PMI_PSPR /* PMI_PSPR: Scratch-Pad RAM (PSPR) */ 

  /* Global section to share code symbol between cores */
  ee_mcglobalt : ALIGN(8)
  {
    PROVIDE(ee_mcglobalt_begin = .);
    KEEP(*(*.StartupHook))
    KEEP(*(*.ErrorHook))
    KEEP(*(*.ShutdownHook))
    /* KEEP(*(*.PreTaskHook)) */
    /* KEEP(*(*.PostTaskHook)) */
    KEEP(*(*.ProtectionHook))
    KEEP(*(ee_mcglobalt))
    /* . += 1; */
    . = ALIGN(8);
    PROVIDE(ee_mcglobalt_end = .);
  } > PMI_PSPR /* PMI_PSPR: Scratch-Pad RAM (PSPR) */

  CORE_SEC(.text) : FLAGS(axl)
  {
    PROVIDE(__text_start = .);
    *(.text)
    *(.text.*)
    *(.gnu.linkonce.t.*)
    /*
     * .gnu.warning sections are handled specially by elf32.em.
     */
    *(.gnu.warning)
    . = ALIGN(8);
  } > PMI_PSPR /* PMI_PSPR: Scratch-Pad RAM (PSPR) */

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
    *(.gcc_except_table)
    __EH_FRAME_BEGIN__ = . ;
    KEEP (*(.eh_frame))
    __EH_FRAME_END__ = . ;
    *(.jcr)
    . = ALIGN(8);
  } > PMI_PSPR /* PMI_PSPR: Scratch-Pad RAM (PSPR) */

  /* OS Public Code */
  CORE_SEC(.ee_api_text) : ALIGN(8) FLAGS(axl) {
    /* range section have to be aligned to 8 bytes */
    PROVIDE(ee_stext_api = . );
    *(ee_api_text)
    /* ALIGN(8) is needed because Range Addresses have to be aligned
       to 8 byte. */
    . += 1;
    . = ALIGN(8);
    /* End OS-Application Data Section Symbol */
    PROVIDE(ee_etext_api = . );
    /* ALIGN(16) the begin of OS_Application data and the following do the
       trick to move next OS-Application section 16 byte forward (at least) */
    . += 15;
    /* Each PMU_PFLASH region have to be at least 4 ALIGNED to ensure that LMA
       region (Load Memory Address) for data region are valid for __copy_table
     */
    . = ALIGN(8);
  } > PMI_PSPR

  /* OS Private Code */
  CORE_SEC(.ee_kernel_text) : ALIGN(16) FLAGS(axl) {
    /* The begin of the kernel code section is aligned to 16 to
       ensure space requiremente for cross sections access protection.
       (Read the Note: at page 9-5 of
       TC_Architecture_vol1_TC161_TCS_TC16P_TC16E.pdf manual) */
    PROVIDE(ee_stext_kernel = . );
    *(ee_kernel_text)
    /* ALIGN(8) is needed because Range Addresses have to be aligned
       to 8 byte. */
    . += 1;
    . = ALIGN(8);
    /* End OS-Application Data Section Symbol */
    PROVIDE(ee_etext_kernel = . );
    /* ALIGN(16) the begin of OS_Application data and the following do the
       trick to move next OS-Application section 16 byte forward (at least) */
    . += 15;
    /* Each PMU_PFLASH region have to be at least 4 ALIGNED to ensure that LMA
       region (Load Memory Address) for data region are valid for __copy_table
     */
    . = ALIGN(8);
  } > PMI_PSPR

/* APPLICATION SECTIONS CODE GENERATION - BEGIN */
/* APPLICATION SECTIONS CODE GENERATION - END */

  /*
   * Constructors and destructors.
   */
  CORE_SEC(.ctors) : FLAGS(axl)
  {
    __CTOR_LIST__ = . ;
    LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2);
    *(.ctors)
    LONG(0) ;
    __CTOR_END__ = . ;
    . = ALIGN(8);
  } > PMI_PSPR /* PMI_PSPR: Scratch-Pad RAM (PSPR) */

  CORE_SEC(.dtors) : FLAGS(axl)
  {
    __DTOR_LIST__ = . ;
    LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2);
    *(.dtors)
    LONG(0) ;
    __DTOR_END__ = . ;
    . = ALIGN(8);
  } > PMI_PSPR /* PMI_PSPR: Scratch-Pad RAM (PSPR) */

  /*
   * Allocate space for absolute addressable sections; this requires that
   * "int_dspr" starts at a TriCore segment (256M) and points to
   * some RAM area!  If these conditions are not met by your particular
   * hardware setup, you should either not use absolute data, or you
   * must move .zdata*,.zbss*,.bdata*,.bbss* input sections to some appropriate
   * memory area.
   */
  CORE_SEC(.zbss)  (NOLOAD) : FLAGS(awz)
  {
    ZBSS_BASE = . ;
    *(.zbss)
    *(.zbss.*)
    *(.gnu.linkonce.zb.*)
    *(.bbss)
    *(.bbss.*)
    . = ALIGN(8);
    ZBSS_END = . ;
  } > DMI_DSPR

  CORE_SEC(.zdata) : FLAGS(awzl)
  {
    ZDATA_BASE = . ;
    *(.zrodata)
    *(.zrodata.*)
    *(.zdata)
    *(.zdata.*)
    *(.gnu.linkonce.z.*)
    *(.bdata)
    *(.bdata.*)
    . = ALIGN(8);
    ZDATA_END = . ;
  } > DMI_DSPR

  /*
   * We're done now with the text part of the executable.  The
   * following sections are special in that their initial code or
   * data (if any) must also be stored in said text part of an
   * executable, but they "live" at completely different addresses
   * at runtime -- usually in RAM areas.  NOTE: This is not really
   * necessary if you use a special program loader (e.g., a debugger)
   * to load a complete executable consisting of code, data, BSS, etc.
   * into the RAM of some target hardware or a simulator, but it *is*
   * necessary if you want to burn your application into non-volatile
   * memories such as EPROM or FLASH.
   */

  /*
   * Not initialised data in section ’.sbss’, addressable by small data area pointer (%a0)
   */
  CORE_SEC(.sbss) (NOLOAD) : FLAGS(aws)
  {
    PROVIDE(__sbss_start = .);
    *(.sbss)
    _CONFIG_BEGIN_ = .;
    *(.sbss.CONFIG)
    _CONFIG_END_ = .;
    *(.sbss.*)
    *(.gnu.linkonce.sb.*)
    PROVIDE(__sbss_end = .);
    . = ALIGN(8);
  } > DMI_DSPR /* DMI_DSPR: Local Data RAM (DSPR) */

  /*
   * Initialised data in section ’.sdata’, addressable by small data area pointer (%a0)
   */
  CORE_SEC(.sdata) : ALIGN(8)
  {
    SDATA_BASE = . ;
    PROVIDE(__sdata_start = .);
    *(.sdata)
    *(.sdata.*)
    *(.gnu.linkonce.s.*)
    . = ALIGN(8);
  } > DMI_DSPR /* DMI_DSPR: Local Data RAM (DSPR) */

  CORE_SEC(.sdata2) : FLAGS(arsl)
  {
    *(.sdata.rodata)
    *(.sdata.rodata.*)
    . = ALIGN(8);
  } > DMI_DSPR

  _SMALL_DATA2_ = SIZEOF(CORE_SEC(.sdata2)) ? ADDR(CORE_SEC(.sdata2)) + 32k : (ADDR(CORE_SEC(.sdata2)) & 0xF0000000) + 32k ;
  __A1_MEM = _SMALL_DATA2_;

  /* Erika RTOS Multicore constant global data (in RAM configuration, locator
     rule have to be explicit otherwise they will tied wrongly with a ROM
     memory) */
  ee_mcglobalc  : ALIGN(8)
  {
    PROVIDE(ee_mcglobalc_begin = .);
    *(ee_mcglobalc)
    /* . += 1; */
    . = ALIGN(8);
    PROVIDE(ee_mcglobalc_end = .);
  } > DMI_DSPR /* DMI_DSPR: Scratch-Pad RAM (DSPR) */

  /*
   * Storage of write-protected data
   */
  CORE_SEC(.rodata) : FLAGS(arl)
  {
    PROVIDE(__rodata_start = .);

    *(.rodata)
    *(.rodata.*)
    *(.gnu.linkonce.r.*)
    *(.rodata1)
    *(.toc)
/* Maybe TODO: Sections for const of OS-Applications? */
    /*  Clear and copy tables that tell the startup code
        which memory areas to clear and to copy, respectively, are not
        needed in Execution from RAM */
    . = ALIGN(8);
  } > DMI_DSPR /* DMI_DSPR: Scratch-Pad RAM (DSPR) */ 

  /*
   * Not Initialised data.
   * Allocate space for BSS sections.
   */
  CORE_SEC(.bss) (NOLOAD) :
  {
    BSS_BASE = .;
    PROVIDE(__bss_start = .);
    *(.bss)
    *(.bss.*)
    *(.gnu.linkonce.b.*)
    *(COMMON)

    PROVIDE(__bss_end = .);
    . = ALIGN(8);
  } > DMI_DSPR /* DMI_DSPR: Local Data RAM (DSPR) */

  /*
   * Initialised data
   */
  CORE_SEC(.data) : ALIGN(8)
  {
    PROVIDE(__data_start = .);
    DATA_BASE = . ;
    *(.data)
    *(.data.*)
    *(.gnu.linkonce.d.*)
    SORT(CONSTRUCTORS)
    . = ALIGN(8) ;
    DATA_END = . ;
    PROVIDE(__data_end = .);
  } > DMI_DSPR

  /*
   * Section for ustack
   */  
  CORE_SEC(.ustack) : ALIGN(8)
  {
    PROVIDE(__USTACK_BEGIN = .);
    . += __USTACK_SIZE ;
    . = ALIGN(8);
    PROVIDE(__USTACK = .);
    PROVIDE(__USTACK_END = .);
  } > DMI_DSPR

  /*
   * Section for istack
   */  
  CORE_SEC(.istack) : ALIGN(8)
  {
    PROVIDE(__ISTACK_BEGIN = .);
    . += __ISTACK_SIZE ;
    . = ALIGN(8);
    PROVIDE(__ISTACK = .);
    PROVIDE(__ISTACK_END = .);
  } > DMI_DSPR

  /*
   * Section for heap
   */  
  CORE_SEC(.heap) : ALIGN(8)
  {
    PROVIDE(__HEAP_BEGIN = .);
    PROVIDE(__HEAP = .);
    . += __HEAP_SIZE ;
    . = ALIGN(8);
    PROVIDE(__HEAP_END = .);
  } > DMI_DSPR

  /*
   * Section for CSA
   */  
  CORE_SEC(.csa) : ALIGN(64)
  {
    PROVIDE(__CSA_BEGIN = .);
    PROVIDE(__CSA = .);
    . += __CSA_SIZE ;
    . = ALIGN(64);
    PROVIDE(__CSA_END = .);
  } > DMI_DSPR

  /* ERIKA API Sections */

  /* ERIKA API public unitializated data section */
  CORE_SEC(.ee_api_bss) : ALIGN(8) {
    /* begin ERIKA API public unitializated data section symbol */
    PROVIDE(ee_sbss_api = .);
    *(ee_api_bss)
    PROVIDE(ee_ebss_api = .);
    . = ALIGN(8);
  } > DMI_DSPR

  /* ERIKA API public initializated data DATA */
  CORE_SEC(.ee_api_data) : ALIGN(8) {
    PROVIDE(ee_sdata_api = .);
    *(ee_api_data)

    /* ALIGN(8) is needed because Range Addresses have to be aligned
       to 8 byte. */
    . += 1;
    . = ALIGN(8);
    /* End Erika API Data Section Symbol */
    PROVIDE(ee_edata_api = .);

    /* ALIGN(16) the begin of OS_Application data and the following do the
       trick to move next OS-Application section 16 byte forward (at least) */
    . += 15;
    /* Each data region have to be at least 4 ALIGNED to ensure that LMA
       region (Load Memory Address) for other data region are valid for
       __copy_table
     */
    . = ALIGN(8);
  } > DMI_DSPR

  /* ERIKA Kernel Sections */

  /* ERIKA Kernel private unitializated data section */
  CORE_SEC(.ee_kernel_bss) : ALIGN(8) {
    /* begin ERIKA Kernel private unitializated data section symbol */
    PROVIDE(ee_sbss_kernel = .);
    *(ee_kernel_bss)
    PROVIDE(ee_ebss_kernel = .);
    . = ALIGN(8);
  } > DMI_DSPR

  /* ERIKA Kernel private initializated data DATA */
  CORE_SEC(.ee_kernel_data) : ALIGN(8) {
    PROVIDE(ee_sdata_kernel = .);
    *(ee_kernel_data)

    /* ALIGN(8) is needed because Range Addresses have to be aligned
       to 8 byte. */
    . += 1;
    . = ALIGN(8);
    /* End Erika kernel Data Section Symbol */
    PROVIDE(ee_edata_kernel = .);

    /* ALIGN(16) the begin of OS_Application data and the following do the
       trick to move next OS-Application section 16 byte forward (at least) */
    . += 15;
    /* Each data region have to be at least 4 ALIGNED to ensure that LMA
       region (Load Memory Address) for other data region are valid for
       __copy_table
     */
    . = ALIGN(8);
  } > DMI_DSPR

  /* ERIKA Kenel Stacks (Don't belong to any OS-Applications, special section
     is needed to handle memory protection and stack monitoring) */
  CORE_SEC(.ee_kernel_stack) : ALIGN(8)
  {
    PROVIDE(ee_sstack_kernel = .);
    KEEP(*(ee_kernel_stack))
    /* Put the first OS-Application Section 16 forward respect the system
       sections to assure cross sections access protection.
       (Read the Note: at page 9-5 of
        TC_Architecture_vol1_TC161_TCS_TC16P_TC16E.pdf manual)
     */
    . += 15;
    . = ALIGN(8);
    /* each range address have to be 8 byte aligned */
    PROVIDE(ee_estack_kernel = .);
  } > DMI_DSPR

/* APPLICATION DATA SECTIONS GENERATION - BEGIN */
  /* GENERATED APPLICATION DATA SECTIONS - END */

  /* Erika RTOS Multicore global data */
  ee_mcglobald  :
  {
    PROVIDE(ee_mcglobald_begin = .);
    KEEP(*(ee_mcglobald))
    /* . += 1; */
    . = ALIGN(8);
    PROVIDE(ee_mcglobald_end = .);
  } > LMU_SRAM

  /* Special Section Used to Provide RAM and Code Symbols */
  CORE_SEC(.ee_end) (NOLOAD) : {
    PROVIDE(ee_sall_code    = BootModeIndex);
    PROVIDE(ee_eall_code    = ADDR(CORE_SEC(.dtors)) + SIZEOF(CORE_SEC(.dtors)));

    PROVIDE(ee_skernel_ram  = __DMI_DSPR_BEGIN);
    PROVIDE(ee_ekernel_ram  = .);
    PROVIDE(ee_skernel_code = ADDR(CORE_SEC(.ee_kernel_text)));
    PROVIDE(ee_ekernel_code = ADDR(CORE_SEC(.ee_kernel_text)) + SIZEOF(CORE_SEC(.ee_kernel_text)));

    PROVIDE(ee_sapi_const   = ADDR(CORE_SEC(.sdata2)));
    PROVIDE(ee_eapi_const   = ADDR(CORE_SEC(.rodata)) + SIZEOF(CORE_SEC(.rodata)));
    PROVIDE(ee_sapi_ram     = ADDR(CORE_SEC(.ee_api_bss)));
    PROVIDE(ee_eapi_ram     = ADDR(CORE_SEC(.ee_api_data)) + SIZEOF(CORE_SEC(.ee_api_data)));
    PROVIDE(ee_sapi_code    = ADDR(CORE_SEC(.ee_api_text)));
    PROVIDE(ee_eapi_code    = ADDR(CORE_SEC(.ee_api_text)) + SIZEOF(CORE_SEC(.ee_api_text)));
  } > DMI_DSPR

  /* Make sure CSA, stack and heap addresses are properly aligned.  */
  _. = ASSERT ((__CSA_BEGIN & 0x3f) == 0 , "illegal CSA start address") ;
  _. = ASSERT ((__CSA_SIZE & 0x3f) == 0 , "illegal CSA size") ;
  _. = ASSERT ((__ISTACK & 7) == 0 , "ISTACK not doubleword aligned") ;
  _. = ASSERT ((__USTACK & 7) == 0 , "USTACK not doubleword aligned") ;
  _. = ASSERT ((__HEAP_END & 7) == 0 , "HEAP not doubleword aligned") ;

  /* Define a default symbol for address 0.  */
  NULL = DEFINED (NULL) ? NULL : 0 ;

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

