/*
 * Name: ee_tc29x_gnu_memory_map.x
 *
 * Description: TC29x memory description
 */

/*
 * Global
 */

OUTPUT_FORMAT("elf32-tricore")
OUTPUT_ARCH(tricore)
ENTRY(_START)

__TRICORE_DERIVATE_MEMORY_MAP__ = 0x290;

/* Local RAM start addresses */
__DMI_DSPR_BEGIN_CPU0_ = 0x70000000;
__DMI_DSPR_BEGIN_CPU1_ = 0x60000000;
__DMI_DSPR_BEGIN_CPU2_ = 0x50000000;

/* CSA list size */
__CSA_SIZE = DEFINED (__CSA_SIZE) ? __CSA_SIZE : 8k ;
/* User stack Size */
__USTACK_SIZE = DEFINED (__USTACK_SIZE) ? __USTACK_SIZE : 2K;
/* Interrupt Stack Size */
__ISTACK_SIZE = DEFINED (__ISTACK_SIZE) ? __ISTACK_SIZE : 0K;
/* Heap size */
__HEAP_SIZE = DEFINED (__HEAP_SIZE) ? __HEAP_SIZE : 0;

/* Notes:
    Local means respect executing core
    NC Means not cacheable memory/segment
 */
MEMORY
{
  /* - CORE0 Local Data Scratch-Pad RAM (DSPR) */
  DMI_DSPR0_LOCAL  (w!xp): org = 0xD0000000, len = 120k
  /* - CORE0 Data Scratch-Pad RAM (DSPR) */
  DMI_DSPR0        (w!xp): org = 0x70000000, len = 120k
  /* - LOCAL Program Scratch-Pad RAM (PSPR) */
  PMI_PSPR0_LOCAL  (w!xp): org = 0xC0000000, len = 32k
  /* - CORE0 Scratch-Pad RAM (PSPR) */
  PMI_PSPR0        (w!xp): org = 0x70100000, len = 32k

  /* - CORE1 Local Data Scratch-Pad RAM (DSPR) */
  DMI_DSPR1_LOCAL  (w!xp): org = 0xD0000000, len = 240k
  /* - CORE1 Data Scratch-Pad RAM (DSPR) */
  DMI_DSPR1        (w!xp): org = 0x60000000, len = 240k
  /* - LOCAL Program Scratch-Pad RAM (PSPR) */
  PMI_PSPR1_LOCAL  (w!xp): org = 0xC0000000, len = 32k
  /* - CORE1 Program Scratch-Pad RAM (PSPR) */
  PMI_PSPR1        (w!xp): org = 0x60100000, len = 32k

  /* - CORE2 Local Data Scratch-Pad RAM (DSPR) */
  DMI_DSPR2_LOCAL  (w!xp): org = 0xD0000000, len = 240k
  /* - CORE2 Data Scratch-Pad RAM (DSPR) */
  DMI_DSPR2        (w!xp): org = 0x50000000, len = 240k
  /* - LOCAL Program Scratch-Pad RAM (PSPR) */
  PMI_PSPR2_LOCAL  (w!xp): org = 0xC0000000, len = 32k
  /* - CORE2 Program Scratch-Pad RAM (PSPR) */
  PMI_PSPR2        (w!xp): org = 0x50100000, len = 32k

  /* - Program Flash Memory (PFLASH0) */
  PMU_PFLASH0       (rx!p): org = 0x80000000, len = 2M
  PMU_PFALSH0_NC    (rx!p): org = 0xA0000000, len = 2M

  /* - Program Flash Memory (PFLASH1) -Not used to allocate and sections- */
  PMU_PFLASH1       (rx!p): org = 0x80200000, len = 2M
  PMU_PFALSH1_NC    (rx!p): org = 0xA0200000, len = 2M
  
  /* - Program Flash Memory (PFLASH2) -Not used to allocate and sections- */
  PMU_PFLASH2       (rx!p): org = 0x80400000, len = 2M
  PMU_PFALSH2_NC    (rx!p): org = 0xA0400000, len = 2M
  
  /* - Program Flash Memory (PFLASH3) -Not used to allocate and sections- */
  PMU_PFLASH3       (rx!p): org = 0x80600000, len = 2M
  PMU_PFALSH3_NC    (rx!p): org = 0xA0600000, len = 2M

  /* - Data Flash Memory (DFLASH0) */
  PMU_DFLASH0       (w!xp): org = 0xAF000000, len = 1M

  /* - Boot ROM (BROM) */
  BROM              (rx!p): org = 0x8FFF8000, len = 32k
  BROM_NC           (rx!p): org = 0xAFFF8000, len = 32k

  /* - Global Data RAM  */
  /* LMU_SRAM          (w!xp): org = 0x90000000, len = 32K XXX EG: I don't known why but if this is defined the linker will use cached memory for common structures */
  LMU_SRAM_NC       (w!xp): org = 0xB0000000, len = 32K

  /* - Emulation Device Memory */
  PMU_EDMEM         (w!xp): org = 0x9F000000, len = 2M
  PMU_EDMEM_NC      (w!xp): org = 0xBF000000, len = 2M
}

/* Map non-cached global memory addresses into cached */
REGION_MAP( GLOBAL , ORIGIN(PMU_PFALSH0_NC), LENGTH(PMU_PFALSH0_NC), ORIGIN(PMU_PFLASH0))
REGION_MAP( GLOBAL , ORIGIN(PMU_PFALSH1_NC), LENGTH(PMU_PFALSH1_NC), ORIGIN(PMU_PFLASH1))
REGION_MAP( GLOBAL , ORIGIN(PMU_PFALSH2_NC), LENGTH(PMU_PFALSH2_NC), ORIGIN(PMU_PFLASH2))
REGION_MAP( GLOBAL , ORIGIN(PMU_PFALSH3_NC), LENGTH(PMU_PFALSH3_NC), ORIGIN(PMU_PFLASH3))
REGION_MAP( GLOBAL , ORIGIN(BROM_NC), LENGTH(BROM_NC), ORIGIN(BROM))
REGION_MAP( GLOBAL , ORIGIN(PMU_EDMEM_NC), LENGTH(PMU_EDMEM_NC), ORIGIN(PMU_EDMEM))
/* REGION_MAP( GLOBAL , ORIGIN(LMU_SRAM_NC), LENGTH(LMU_SRAM_NC), ORIGIN(LMU_SRAM)) */

/* Map local memory address to a global address */ 
REGION_MAP( CPU0 , ORIGIN(DMI_DSPR0_LOCAL), LENGTH(DMI_DSPR0_LOCAL), ORIGIN(DMI_DSPR0))
REGION_MAP( CPU1 , ORIGIN(DMI_DSPR1_LOCAL), LENGTH(DMI_DSPR1_LOCAL), ORIGIN(DMI_DSPR1))
REGION_MAP( CPU2 , ORIGIN(DMI_DSPR2_LOCAL), LENGTH(DMI_DSPR2_LOCAL), ORIGIN(DMI_DSPR2))

/* Linker Script Aliases */
REGION_ALIAS( "PMU_PFLASH", PMU_PFLASH0)

REGION_ALIAS( "PMI_PSPR_CPU0_", PMI_PSPR0)
REGION_ALIAS( "DMI_DSPR_CPU0_", DMI_DSPR0)

REGION_ALIAS( "PMI_PSPR_CPU1_", PMI_PSPR1)
REGION_ALIAS( "DMI_DSPR_CPU1_", DMI_DSPR1)

REGION_ALIAS( "PMI_PSPR_CPU2_", PMI_PSPR2)
REGION_ALIAS( "DMI_DSPR_CPU2_", DMI_DSPR2)

/*
 * Name: ee_tc2Yx_gnu_ram_startup.ld
 * 
 * Description: 
 * Linker rules for startup Flash configuration
 */

CORE_ID = GLOBAL ;

SECTIONS {
  /*
   * The startup code should be placed where the CPU expects it after a reset,
   * so we try to locate it first, no matter where it appears in the list of
   * objects and libraries (note: because the wildcard pattern doesn't match
   * directories, we'll try to find crt0.o in various (sub)directories).
   */

  .startup : FLAGS(rxlc0)
  {
    BootModeIndex = .;
    PROVIDE(__startup_code_start = .);
    KEEP(*(*.startup))
    KEEP (*(ee_kernel_start))
    . = ALIGN(8);
    PROVIDE(__startup_code_end = .);
  } > DMI_DSPR1

}

/*
 * Name: ee_tc2Yx_gnu_ram_recollect_prefix.ld
 * 
 * Description: 
 * Linker rules end rules to recollect all sections in internal Flash
 * configuration
 */

CORE_ID = GLOBAL ;

SECTIONS
{
  /* Global section to share code symbol between cores */
  ee_mcglobalt : ALIGN(8) FLAGS(axlc0)
  {
    PROVIDE(ee_mcglobalt_begin = .);
    KEEP(*(ee_mcglobalt))
    /* . += 1; */
    . = ALIGN(8);
    PROVIDE(ee_mcglobalt_end = .);
  } > PMI_PSPR1

  /* Erika RTOS Multicore global data */
  ee_mcglobald : ALIGN(8) FLAGS(awlc0)
  {
    PROVIDE(ee_mcglobald_begin = .);
    *(SORT(ee_mcglobald*))
    /* . += 1; */
    . = ALIGN(8);
    PROVIDE(ee_mcglobald_end = .);
  } > LMU_SRAM_NC

  /* Erika Constant Shared Data */
  ee_mcglobalc : ALIGN(8) FLAGS(arlc0)
  {
    PROVIDE(ee_mcglobalc_begin = .);
    *(SORT(ee_mcglobalc*))
    /* . += 1; */
    . = ALIGN(8);
    PROVIDE(ee_mcglobalc_end = .);
    /* make sure that ee_mcglobald_copy_table do not coincide with the last
       const data.
       ALIGN(8) because range addresses have to be 8 bytes aligned */
    . += 1;
    . = ALIGN(8);
    PROVIDE(ee_mcglobald_copy_table = .) ;
    LONG(LOADADDR(ee_mcglobald)); LONG(0 + ADDR(ee_mcglobald)); LONG(SIZEOF(ee_mcglobald));
    LONG(-1);                     LONG(-1);                     LONG(-1);
  } > DMI_DSPR1
}

CORE_ID = CPU0;
/*
 * Name: ee_tc2Yx_gnu_ram_recollect.ld.tmpl
 * 
 * Description: 
 * Template for linker rules to recollect all sections in internal Flash
 * configuration
 */

SECTIONS
{
  /*
   * Allocate .text and other read-only sections.
   */

  /*
   * Code executed before calling main
   */
  CORE_SEC(.init) : ALIGN(8) FLAGS(ax)
  {
    KEEP(*(SORT(CORE_SEC(.init*))))
    . = ALIGN(8);
  } > CORE_SYM(PMI_PSPR)

  CORE_SEC(.fini)    : ALIGN(8) FLAGS(axl)
  {
    KEEP(*(SORT(CORE_SEC(.fini*))))
  } > CORE_SYM(PMI_PSPR)

  CORE_SEC(.cramtext) : ALIGN(8)
  {
    KEEP(*(SORT(CORE_SEC(.cramtext*))))
    . = ALIGN(8);
  } > CORE_SYM(PMI_PSPR)

  /*
   * Section for trap table
   */
  CORE_SEC(.traptab) : ALIGN(32) FLAGS(ax)
  {
    KEEP(*(CORE_SEC(.traptab*)))  /* Section for trap table */
    . = ALIGN(8);
  } > CORE_SYM(PMI_PSPR)

  /*
   * Section for interrupt table
   */
  CORE_SEC(.inttab) : ALIGN(32) FLAGS(ax)
  {
    KEEP (*(CORE_SEC(.inttab*)));
    . = ALIGN(8);
  } > CORE_SYM(PMI_PSPR)


  CORE_SEC(.text) : ALIGN(8) FLAGS(axl)
  {
    *(CORE_SEC(.text*))
    . = ALIGN(8);
  } > CORE_SYM(PMI_PSPR)

  /* OS Public Code */
  CORE_SEC(.ee_api_text) : ALIGN(8) FLAGS(axl) {
    /* range section have to be aligned to 8 bytes */
    PROVIDE(CORE_SYM(ee_stext_api) = . );
    *(CORE_SEC(.ee_api_text*))
    /* ALIGN(8) is needed because Range Addresses have to be aligned
       to 8 byte. */
    . += 1;
    . = ALIGN(8);
    /* End OS-Application Data Section Symbol */
    PROVIDE(CORE_SYM(ee_etext_api) = . );
    /* ALIGN(16) the begin of OS_Application data and the following do the
       trick to move next OS-Application section 16 byte forward (at least) */
    . += 15;
    /* Each PMU_PFLASH region have to be at least 4 ALIGNED to ensure that LMA
       region (Load Memory Address) for data region are valid for __copy_table
     */
    . = ALIGN(8);
  } > CORE_SYM(PMI_PSPR)

  /* OS Private Code */
  CORE_SEC(.ee_kernel_text) : ALIGN(16) FLAGS(axl) {
    /* The begin of the kernel code section is aligned to 16 to
       ensure space requiremente for cross sections access protection.
       (Read the Note: at page 9-5 of
       TC_Architecture_vol1_TC161_TCS_TC16P_TC16E.pdf manual) */
    PROVIDE(CORE_SYM(ee_stext_kernel) = . );
    *(CORE_SEC(.ee_kernel_text*))
    /* ALIGN(8) is needed because Range Addresses have to be aligned
       to 8 byte. */
    . += 1;
    . = ALIGN(8);
    /* End OS-Application Data Section Symbol */
    PROVIDE(CORE_SYM(ee_etext_kernel) = . );
    /* ALIGN(16) the begin of OS_Application data and the following do the
       trick to move next OS-Application section 16 byte forward (at least) */
    . += 15;
    /* Each PMU_PFLASH region have to be at least 4 ALIGNED to ensure that LMA
       region (Load Memory Address) for data region are valid for __copy_table
     */
    . = ALIGN(8);
  } > CORE_SYM(PMI_PSPR)

  /* GENERATED APPLICATION CODE SECTIONS - BEGIN */
  /* GENERATED APPLICATION CODE SECTIONS - END */

  /*
   * C++ exception handling tables.  NOTE: gcc emits .eh_frame
   * sections when compiling C sources with debugging enabled (-g).
   * If you can be sure that your final application consists
   * exclusively of C objects (i.e., no C++ objects), you may use
   * the -R option of the "strip" and "objcopy" utilities to remove
   * the .eh_frame section from the executable.
   */
  CORE_SEC(.gcc_except_table) : ALIGN(8) FLAGS(axl)
  {
    *(CORE_SEC(.gcc_except_table*))
  } > CORE_SYM(PMI_PSPR)

  CORE_SEC(.eh_frame) : FLAGS(axl)
  {
    CORE_SYM(__EH_FRAME_BEGIN__) = . ;
    *(CORE_SEC(.eh_frame*))
    CORE_SYM(__EH_FRAME_END__) = . ;
  } > CORE_SYM(PMI_PSPR)

  CORE_SEC(.jcr) : FLAGS(axl)
  {
    *(CORE_SEC(.jcr*))
    . = ALIGN(8);
  } > CORE_SYM(PMI_PSPR)

  /*
   * Constructors and destructors.
   */
  CORE_SEC(.ctors) : FLAGS(axl)
  {
    CORE_SYM(__CTOR_LIST__) = . ;
    LONG((CORE_SYM(__CTOR_END__) - CORE_SYM(__CTOR_LIST__)) / 4 - 2);
    /* gcc uses crtbegin.o to find the start of
       the constructors, so we make sure it is
       first.  Because this is a wildcard, it
       doesn't matter if the user does not
       actually link against crtbegin.o; the
       linker won't look for a file to match a
       wildcard.  The wildcard also means that it
       doesn't matter which directory crtbegin.o
       is in.  */
    KEEP (*crtbegin.o(.ctors))
    /* We don't want to include the .ctor section from
       from the crtend.o file until after the sorted ctors.
       The .ctor section from the crtend file contains the
       end of ctors marker and it must be last */
    KEEP (*(EXCLUDE_FILE (*crtend.o ) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    LONG(0);
    CORE_SYM(__CTOR_END__) = .;
    LONG(0) ;
    CORE_SYM(__CTOR_END__) = . ;
    . = ALIGN(8);
  } > CORE_SYM(PMI_PSPR)

  CORE_SEC(.dtors) : FLAGS(axl)
  {
    CORE_SYM(__DTOR_LIST__) = . ;
    LONG((CORE_SYM(__DTOR_END__) - CORE_SYM(__DTOR_LIST__)) / 4 - 2);
    KEEP (*crtbegin.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o ) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    LONG(0) ;
    CORE_SYM(__DTOR_END__) = . ;
    . = ALIGN(8);
  } > CORE_SYM(PMI_PSPR)

  CORE_SEC(.sdata2) : ALIGN(8)
  {
    *(CORE_SEC(.sdata2*))
    . = ALIGN(8);
  } > CORE_SYM(DMI_DSPR)

  CORE_SYM(_SMALL_DATA2_) = SIZEOF(CORE_SEC(.sdata2)) ? ADDR(CORE_SEC(.sdata2)) + 32k : (ADDR(CORE_SEC(.sdata2)) & 0xF0000000) + 32k ;
  CORE_SYM(__A1_MEM) = CORE_SYM(_SMALL_DATA2_);

  /*
   * Storage of write-protected data
   */
  CORE_SEC(.rodata) : FLAGS(arl)
  {
    PROVIDE(CORE_SYM(__rodata_start) = .);
    KEEP(*(SORT(CORE_SEC(.rodata*))))

    . = ALIGN(4) ;
    PROVIDE(CORE_SYM(ee_stacks_table) = .) ;
    LONG(0 + ADDR(CORE_SEC(.ee_kernel_stack)));      LONG(SIZEOF(CORE_SEC(.ee_kernel_stack)));
/* GENERATED STACKS - BEGIN */
/* GENERATED STACKS TABLE - END */
    LONG(-1);                 LONG(-1);
    /* ALIGN(8) because range addresses have to be 8 bytes aligned */
    . = ALIGN(8);
  } > CORE_SYM(DMI_DSPR)

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
    PROVIDE(CORE_SYM(ZBSS_BASE) = .);
    *(CORE_SEC(.zbss*))
    . = ALIGN(8);
    PROVIDE(CORE_SYM(ZBSS_END) = .);
  } > CORE_SYM(DMI_DSPR)

  CORE_SEC(.zdata) : FLAGS(awzl)
  {
    PROVIDE(CORE_SYM(ZDATA_BASE) = .);
    *(CORE_SEC(.zdata*))
    . = ALIGN(8);
    PROVIDE(CORE_SYM(ZDATA_END) = .);
  } > CORE_SYM(DMI_DSPR)

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
    PROVIDE(CORE_SYM(__sbss_start) = .);
    *(CORE_SEC(.sbss*))
    PROVIDE(CORE_SYM(__sbss_end) = .);
    . = ALIGN(8);
  } > CORE_SYM(DMI_DSPR)

  /*
   * Initialised data in section ’.sdata’, addressable by small data area pointer (%a0)
   */
  CORE_SEC(.sdata) : ALIGN(8) FLAGS(awls)
  {
    PROVIDE(CORE_SYM(SDATA_BASE) = .);
    PROVIDE(CORE_SYM(__sdata_start) = .);
    *(CORE_SEC(.sdata*))
    . = ALIGN(8);
    PROVIDE(CORE_SYM(SDATA_END) = .);
    PROVIDE(CORE_SYM(__sdata_end) = .);
  } > CORE_SYM(DMI_DSPR)

  CORE_SYM(_SMALL_DATA_) = (SIZEOF(CORE_SEC(.sbss)) + SIZEOF(CORE_SEC(.sdata))) ? ADDR(CORE_SEC(.sbss)) + 32k : (ADDR(CORE_SEC(.sbss)) & 0xF0000000) ;
  CORE_SYM(__A0_MEM) = CORE_SYM(_SMALL_DATA_) ;

  /*
   * Not Initialised data.
   * Allocate space for BSS sections.
   */
  CORE_SEC(.bss) (NOLOAD) : FLAGS(aw)
  {
    PROVIDE(CORE_SYM(BSS_BASE) = .);
    PROVIDE(CORE_SYM(__bss_start) = .);
    *(CORE_SEC(.bss*))
    PROVIDE(CORE_SYM(__bss_end) = .);
    . = ALIGN(8);
  } > CORE_SYM(DMI_DSPR)

  /*
   * Initialised data
   */
  CORE_SEC(.data) : ALIGN(8) FLAGS(awl)
  {
    PROVIDE(CORE_SYM(DATA_BASE) = .);
    PROVIDE(CORE_SYM(__data_start) = .);
    *(CORE_SEC(.data*))
    . = ALIGN(8) ;
    PROVIDE(CORE_SYM(DATA_END) = .);
    PROVIDE(CORE_SYM(__data_end) = .);
  } > CORE_SYM(DMI_DSPR)

  /*
   * Section for ustack
   */  
  CORE_SEC(.ustack) : ALIGN(8) FLAGS(awl)
  {
    PROVIDE(CORE_SYM(__USTACK_BEGIN) = .);
    . += __USTACK_SIZE ;
    . = ALIGN(8);
    CORE_SYM(__USTACK) = . ;
    PROVIDE(CORE_SYM(__USTACK_END) = .);
  } > CORE_SYM(DMI_DSPR)

  /*
   * Section for istack
   */  
  CORE_SEC(.istack) : ALIGN(8) FLAGS(awl)
  {
    PROVIDE(CORE_SYM(__ISTACK_BEGIN) = .);
    . += __ISTACK_SIZE ;
    . = ALIGN(8);
    CORE_SYM(__ISTACK) = . ;
    PROVIDE(CORE_SYM(__ISTACK_END) = .);
  } > CORE_SYM(DMI_DSPR)

  /*
   * Section for heap
   */  
  CORE_SEC(.heap) : ALIGN(8) FLAGS(awl)
  {
    PROVIDE(CORE_SYM(__HEAP_BEGIN) = .);
    PROVIDE(CORE_SYM(__HEAP) = .);
    . += __HEAP_SIZE ;
    . = ALIGN(8);
    CORE_SYM(__HEAP_END) = . ;
  } > CORE_SYM(DMI_DSPR)

  /*
   * Section for CSA
   */  
  CORE_SEC(.csa) : ALIGN(64) FLAGS(awl)
  {
    CORE_SYM(__CSA_BEGIN) = .;
    PROVIDE(CORE_SYM(__CSA) = .);
    . += __CSA_SIZE ;
    . = ALIGN(64);
    PROVIDE(CORE_SYM(__CSA_END) = .);
  } > CORE_SYM(DMI_DSPR)

  /* ERIKA API Sections */

  /* ERIKA API public unitializated data section */
  CORE_SEC(.ee_api_bss) :  ALIGN(8) FLAGS(aw)
  {
    /* begin ERIKA API public unitializated data section symbol */
    PROVIDE(CORE_SYM(ee_sbss_api) = .);
    *(CORE_SEC(*.ee_api_bss*))
    PROVIDE(CORE_SYM(ee_ebss_api) = .);
    . = ALIGN(8);
  } > CORE_SYM(DMI_DSPR)

  /* ERIKA API public initializated data DATA */
  CORE_SEC(.ee_api_data) : ALIGN(8) FLAGS(awl)
  {
    PROVIDE(CORE_SYM(ee_sdata_api) = .);
    *(CORE_SEC(.ee_api_data*))

    /* ALIGN(8) is needed because Range Addresses have to be aligned
       to 8 byte. */
    . += 1;
    . = ALIGN(8);
    /* End Erika API Data Section Symbol */
    PROVIDE(CORE_SYM(ee_edata_api) = .);
    /* ALIGN(16) the begin of OS_Application data and the following do the
       trick to move next OS-Application section 16 byte forward (at least) */
    . += 15;
    /* Each data region have to be at least 4 ALIGNED to ensure that LMA
       region (Load Memory Address) for other data region are valid for
       __copy_table */
    . = ALIGN(8);
  } > CORE_SYM(DMI_DSPR)

  /* ERIKA Kernel Sections */

  /* ERIKA Kernel private unitializated data section */
  CORE_SEC(.ee_kernel_bss) : ALIGN(8) FLAGS(aw)
  {
    /* begin ERIKA Kernel private unitializated data section symbol */
    PROVIDE(CORE_SYM(ee_sbss_kernel) = .);
    *(CORE_SEC(.ee_kernel_bss*))
    PROVIDE(CORE_SYM(ee_ebss_kernel) = .);
  } > CORE_SYM(DMI_DSPR)

  /* ERIKA Kernel private initializated data DATA */
  CORE_SEC(.ee_kernel_data) : ALIGN(8) FLAGS(awl)
  {
    PROVIDE(CORE_SYM(ee_sdata_kernel) = .);
    *(CORE_SEC(.ee_kernel_data*))

    /* ALIGN(8) is needed because Range Addresses have to be aligned
       to 8 byte. */
    . += 1;
    . = ALIGN(8);
    /* End Erika Kernel Data Section Symbol */
    PROVIDE(CORE_SYM(ee_edata_kernel) = .);

    /* ALIGN(16) the begin of OS_Application data and the following do the
       trick to move next OS-Application section 16 byte forward (at least) */
    . += 15;
    /* Each data region have to be at least 4 ALIGNED to ensure that LMA
       region (Load Memory Address) for other data region are valid for
       __copy_table */
    . = ALIGN(8);
  } > CORE_SYM(DMI_DSPR)

  /* ERIKA Kenel Stacks (Don't belong to any OS-Applications, special section
     is needed to handle memory protection and stack monitoring) */
  CORE_SEC(.ee_kernel_stack) : ALIGN(8) FLAGS(awl)
  {
    PROVIDE(CORE_SYM(ee_sstack_kernel) = .);
    *(CORE_SEC(.ee_kernel_stack*))
    /* Put the first OS-Application Section 16 forward respect the system
       sections to assure cross sections access protection.
       (Read the Note: at page 9-5 of
        TC_Architecture_vol1_TC161_TCS_TC16P_TC16E.pdf manual) */
    . += 15;
    . = ALIGN(8);
    /* each range address have to be 8 byte aligned */
    PROVIDE(CORE_SYM(ee_estack_kernel) = .);
  } > CORE_SYM(DMI_DSPR)

  /* GENERATED APPLICATION DATA SECTIONS - BEGIN */
  /* GENERATED APPLICATION DATA SECTIONS - END */

  /* Special Section Used to Provide RAM, Flash an code Symbols */
  CORE_SEC(ee_end) (NOLOAD) : FLAGS(a) {
    PROVIDE(CORE_SYM(ee_sall_code)    = ADDR(CORE_SEC(.init)));
    PROVIDE(CORE_SYM(ee_eall_code)    = ADDR(CORE_SEC(.dtors)) + SIZEOF(CORE_SEC(.dtors)));

    PROVIDE(CORE_SYM(ee_skernel_ram)  = CORE_SYM(__DMI_DSPR_BEGIN));
    PROVIDE(CORE_SYM(ee_ekernel_ram)  = .);
    PROVIDE(CORE_SYM(ee_skernel_code) = ADDR(CORE_SEC(.ee_kernel_text)));
    PROVIDE(CORE_SYM(ee_ekernel_code) = ADDR(CORE_SEC(.ee_kernel_text)) + SIZEOF(CORE_SEC(.ee_kernel_text)));
    PROVIDE(CORE_SYM(ee_sapi_const)   = ADDR(CORE_SEC(.sdata2)));
    PROVIDE(CORE_SYM(ee_eapi_const)   = ADDR(CORE_SEC(.rodata)) + SIZEOF(CORE_SEC(.rodata)));

    PROVIDE(CORE_SYM(ee_sapi_ram)     = ADDR(CORE_SEC(.ee_api_bss)));
    PROVIDE(CORE_SYM(ee_eapi_ram)     = ADDR(CORE_SEC(.ee_api_data)) + SIZEOF(CORE_SEC(.ee_api_data)));
    PROVIDE(CORE_SYM(ee_sapi_code)    = ADDR(CORE_SEC(.ee_api_text)));
    PROVIDE(CORE_SYM(ee_eapi_code)    = ADDR(CORE_SEC(.ee_api_text)) + SIZEOF(CORE_SEC(.ee_api_text)));
    /* Each PMU_PFLASH region have to be at least 4 ALIGNED to ensure that LMA
       region (Load Memory Address) for data region are valid for __copy_table
    */
    . = ALIGN(4);
  } > CORE_SYM(DMI_DSPR) AT> PMU_PFLASH

  /* Make sure CSA, stack and heap addresses are properly aligned. */
  _. = ASSERT ((CORE_SYM(__CSA_BEGIN) & 0x3f) == 0 , "illegal CSA start address") ;
  _. = ASSERT ((__CSA_SIZE & 0x3f) == 0 , "illegal CSA size") ;
  _. = ASSERT ((CORE_SYM(__ISTACK) & 7) == 0 , "ISTACK not doubleword aligned") ;
  _. = ASSERT ((CORE_SYM(__USTACK) & 7) == 0 , "USTACK not doubleword aligned") ;
  _. = ASSERT ((CORE_SYM(__HEAP_END) & 7) == 0 , "HEAP not doubleword aligned") ;
}


CORE_ID = CPU1;
/*
 * Name: ee_tc2Yx_gnu_ram_recollect.ld.tmpl
 * 
 * Description: 
 * Template for linker rules to recollect all sections in internal Flash
 * configuration
 */

SECTIONS
{
  /*
   * Allocate .text and other read-only sections.
   */

  /*
   * Code executed before calling main
   */
  CORE_SEC(.init) : ALIGN(8) FLAGS(ax)
  {
    KEEP(*(SORT(CORE_SEC(.init*))))
    . = ALIGN(8);
  } > CORE_SYM(PMI_PSPR)

  CORE_SEC(.fini)    : ALIGN(8) FLAGS(axl)
  {
    KEEP(*(SORT(CORE_SEC(.fini*))))
  } > CORE_SYM(PMI_PSPR)

  CORE_SEC(.cramtext) : ALIGN(8)
  {
    KEEP(*(SORT(CORE_SEC(.cramtext*))))
    . = ALIGN(8);
  } > CORE_SYM(PMI_PSPR)

  /*
   * Section for trap table
   */
  CORE_SEC(.traptab) : ALIGN(32) FLAGS(ax)
  {
    KEEP(*(CORE_SEC(.traptab*)))  /* Section for trap table */
    . = ALIGN(8);
  } > CORE_SYM(PMI_PSPR)

  /*
   * Section for interrupt table
   */
  CORE_SEC(.inttab) : ALIGN(32) FLAGS(ax)
  {
    KEEP (*(CORE_SEC(.inttab*)));
    . = ALIGN(8);
  } > CORE_SYM(PMI_PSPR)


  CORE_SEC(.text) : ALIGN(8) FLAGS(axl)
  {
    *(CORE_SEC(.text*))
    . = ALIGN(8);
  } > CORE_SYM(PMI_PSPR)

  /* OS Public Code */
  CORE_SEC(.ee_api_text) : ALIGN(8) FLAGS(axl) {
    /* range section have to be aligned to 8 bytes */
    PROVIDE(CORE_SYM(ee_stext_api) = . );
    *(CORE_SEC(.ee_api_text*))
    /* ALIGN(8) is needed because Range Addresses have to be aligned
       to 8 byte. */
    . += 1;
    . = ALIGN(8);
    /* End OS-Application Data Section Symbol */
    PROVIDE(CORE_SYM(ee_etext_api) = . );
    /* ALIGN(16) the begin of OS_Application data and the following do the
       trick to move next OS-Application section 16 byte forward (at least) */
    . += 15;
    /* Each PMU_PFLASH region have to be at least 4 ALIGNED to ensure that LMA
       region (Load Memory Address) for data region are valid for __copy_table
     */
    . = ALIGN(8);
  } > CORE_SYM(PMI_PSPR)

  /* OS Private Code */
  CORE_SEC(.ee_kernel_text) : ALIGN(16) FLAGS(axl) {
    /* The begin of the kernel code section is aligned to 16 to
       ensure space requiremente for cross sections access protection.
       (Read the Note: at page 9-5 of
       TC_Architecture_vol1_TC161_TCS_TC16P_TC16E.pdf manual) */
    PROVIDE(CORE_SYM(ee_stext_kernel) = . );
    *(CORE_SEC(.ee_kernel_text*))
    /* ALIGN(8) is needed because Range Addresses have to be aligned
       to 8 byte. */
    . += 1;
    . = ALIGN(8);
    /* End OS-Application Data Section Symbol */
    PROVIDE(CORE_SYM(ee_etext_kernel) = . );
    /* ALIGN(16) the begin of OS_Application data and the following do the
       trick to move next OS-Application section 16 byte forward (at least) */
    . += 15;
    /* Each PMU_PFLASH region have to be at least 4 ALIGNED to ensure that LMA
       region (Load Memory Address) for data region are valid for __copy_table
     */
    . = ALIGN(8);
  } > CORE_SYM(PMI_PSPR)

  /* GENERATED APPLICATION CODE SECTIONS - BEGIN */
  /* GENERATED APPLICATION CODE SECTIONS - END */

  /*
   * C++ exception handling tables.  NOTE: gcc emits .eh_frame
   * sections when compiling C sources with debugging enabled (-g).
   * If you can be sure that your final application consists
   * exclusively of C objects (i.e., no C++ objects), you may use
   * the -R option of the "strip" and "objcopy" utilities to remove
   * the .eh_frame section from the executable.
   */
  CORE_SEC(.gcc_except_table) : ALIGN(8) FLAGS(axl)
  {
    *(CORE_SEC(.gcc_except_table*))
  } > CORE_SYM(PMI_PSPR)

  CORE_SEC(.eh_frame) : FLAGS(axl)
  {
    CORE_SYM(__EH_FRAME_BEGIN__) = . ;
    *(CORE_SEC(.eh_frame*))
    CORE_SYM(__EH_FRAME_END__) = . ;
  } > CORE_SYM(PMI_PSPR)

  CORE_SEC(.jcr) : FLAGS(axl)
  {
    *(CORE_SEC(.jcr*))
    . = ALIGN(8);
  } > CORE_SYM(PMI_PSPR)

  /*
   * Constructors and destructors.
   */
  CORE_SEC(.ctors) : FLAGS(axl)
  {
    CORE_SYM(__CTOR_LIST__) = . ;
    LONG((CORE_SYM(__CTOR_END__) - CORE_SYM(__CTOR_LIST__)) / 4 - 2);
    /* gcc uses crtbegin.o to find the start of
       the constructors, so we make sure it is
       first.  Because this is a wildcard, it
       doesn't matter if the user does not
       actually link against crtbegin.o; the
       linker won't look for a file to match a
       wildcard.  The wildcard also means that it
       doesn't matter which directory crtbegin.o
       is in.  */
    KEEP (*crtbegin.o(.ctors))
    /* We don't want to include the .ctor section from
       from the crtend.o file until after the sorted ctors.
       The .ctor section from the crtend file contains the
       end of ctors marker and it must be last */
    KEEP (*(EXCLUDE_FILE (*crtend.o ) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    LONG(0);
    CORE_SYM(__CTOR_END__) = .;
    LONG(0) ;
    CORE_SYM(__CTOR_END__) = . ;
    . = ALIGN(8);
  } > CORE_SYM(PMI_PSPR)

  CORE_SEC(.dtors) : FLAGS(axl)
  {
    CORE_SYM(__DTOR_LIST__) = . ;
    LONG((CORE_SYM(__DTOR_END__) - CORE_SYM(__DTOR_LIST__)) / 4 - 2);
    KEEP (*crtbegin.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o ) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    LONG(0) ;
    CORE_SYM(__DTOR_END__) = . ;
    . = ALIGN(8);
  } > CORE_SYM(PMI_PSPR)

  CORE_SEC(.sdata2) : ALIGN(8)
  {
    *(CORE_SEC(.sdata2*))
    . = ALIGN(8);
  } > CORE_SYM(DMI_DSPR)

  CORE_SYM(_SMALL_DATA2_) = SIZEOF(CORE_SEC(.sdata2)) ? ADDR(CORE_SEC(.sdata2)) + 32k : (ADDR(CORE_SEC(.sdata2)) & 0xF0000000) + 32k ;
  CORE_SYM(__A1_MEM) = CORE_SYM(_SMALL_DATA2_);

  /*
   * Storage of write-protected data
   */
  CORE_SEC(.rodata) : FLAGS(arl)
  {
    PROVIDE(CORE_SYM(__rodata_start) = .);
    KEEP(*(SORT(CORE_SEC(.rodata*))))

    . = ALIGN(4) ;
    PROVIDE(CORE_SYM(ee_stacks_table) = .) ;
    LONG(0 + ADDR(CORE_SEC(.ee_kernel_stack)));      LONG(SIZEOF(CORE_SEC(.ee_kernel_stack)));
/* GENERATED STACKS - BEGIN */
/* GENERATED STACKS TABLE - END */
    LONG(-1);                 LONG(-1);
    /* ALIGN(8) because range addresses have to be 8 bytes aligned */
    . = ALIGN(8);
  } > CORE_SYM(DMI_DSPR)

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
    PROVIDE(CORE_SYM(ZBSS_BASE) = .);
    *(CORE_SEC(.zbss*))
    . = ALIGN(8);
    PROVIDE(CORE_SYM(ZBSS_END) = .);
  } > CORE_SYM(DMI_DSPR)

  CORE_SEC(.zdata) : FLAGS(awzl)
  {
    PROVIDE(CORE_SYM(ZDATA_BASE) = .);
    *(CORE_SEC(.zdata*))
    . = ALIGN(8);
    PROVIDE(CORE_SYM(ZDATA_END) = .);
  } > CORE_SYM(DMI_DSPR)

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
    PROVIDE(CORE_SYM(__sbss_start) = .);
    *(CORE_SEC(.sbss*))
    PROVIDE(CORE_SYM(__sbss_end) = .);
    . = ALIGN(8);
  } > CORE_SYM(DMI_DSPR)

  /*
   * Initialised data in section ’.sdata’, addressable by small data area pointer (%a0)
   */
  CORE_SEC(.sdata) : ALIGN(8) FLAGS(awls)
  {
    PROVIDE(CORE_SYM(SDATA_BASE) = .);
    PROVIDE(CORE_SYM(__sdata_start) = .);
    *(CORE_SEC(.sdata*))
    . = ALIGN(8);
    PROVIDE(CORE_SYM(SDATA_END) = .);
    PROVIDE(CORE_SYM(__sdata_end) = .);
  } > CORE_SYM(DMI_DSPR)

  CORE_SYM(_SMALL_DATA_) = (SIZEOF(CORE_SEC(.sbss)) + SIZEOF(CORE_SEC(.sdata))) ? ADDR(CORE_SEC(.sbss)) + 32k : (ADDR(CORE_SEC(.sbss)) & 0xF0000000) ;
  CORE_SYM(__A0_MEM) = CORE_SYM(_SMALL_DATA_) ;

  /*
   * Not Initialised data.
   * Allocate space for BSS sections.
   */
  CORE_SEC(.bss) (NOLOAD) : FLAGS(aw)
  {
    PROVIDE(CORE_SYM(BSS_BASE) = .);
    PROVIDE(CORE_SYM(__bss_start) = .);
    *(CORE_SEC(.bss*))
    PROVIDE(CORE_SYM(__bss_end) = .);
    . = ALIGN(8);
  } > CORE_SYM(DMI_DSPR)

  /*
   * Initialised data
   */
  CORE_SEC(.data) : ALIGN(8) FLAGS(awl)
  {
    PROVIDE(CORE_SYM(DATA_BASE) = .);
    PROVIDE(CORE_SYM(__data_start) = .);
    *(CORE_SEC(.data*))
    . = ALIGN(8) ;
    PROVIDE(CORE_SYM(DATA_END) = .);
    PROVIDE(CORE_SYM(__data_end) = .);
  } > CORE_SYM(DMI_DSPR)

  /*
   * Section for ustack
   */  
  CORE_SEC(.ustack) : ALIGN(8) FLAGS(awl)
  {
    PROVIDE(CORE_SYM(__USTACK_BEGIN) = .);
    . += __USTACK_SIZE ;
    . = ALIGN(8);
    CORE_SYM(__USTACK) = . ;
    PROVIDE(CORE_SYM(__USTACK_END) = .);
  } > CORE_SYM(DMI_DSPR)

  /*
   * Section for istack
   */  
  CORE_SEC(.istack) : ALIGN(8) FLAGS(awl)
  {
    PROVIDE(CORE_SYM(__ISTACK_BEGIN) = .);
    . += __ISTACK_SIZE ;
    . = ALIGN(8);
    CORE_SYM(__ISTACK) = . ;
    PROVIDE(CORE_SYM(__ISTACK_END) = .);
  } > CORE_SYM(DMI_DSPR)

  /*
   * Section for heap
   */  
  CORE_SEC(.heap) : ALIGN(8) FLAGS(awl)
  {
    PROVIDE(CORE_SYM(__HEAP_BEGIN) = .);
    PROVIDE(CORE_SYM(__HEAP) = .);
    . += __HEAP_SIZE ;
    . = ALIGN(8);
    CORE_SYM(__HEAP_END) = . ;
  } > CORE_SYM(DMI_DSPR)

  /*
   * Section for CSA
   */  
  CORE_SEC(.csa) : ALIGN(64) FLAGS(awl)
  {
    CORE_SYM(__CSA_BEGIN) = .;
    PROVIDE(CORE_SYM(__CSA) = .);
    . += __CSA_SIZE ;
    . = ALIGN(64);
    PROVIDE(CORE_SYM(__CSA_END) = .);
  } > CORE_SYM(DMI_DSPR)

  /* ERIKA API Sections */

  /* ERIKA API public unitializated data section */
  CORE_SEC(.ee_api_bss) :  ALIGN(8) FLAGS(aw)
  {
    /* begin ERIKA API public unitializated data section symbol */
    PROVIDE(CORE_SYM(ee_sbss_api) = .);
    *(CORE_SEC(*.ee_api_bss*))
    PROVIDE(CORE_SYM(ee_ebss_api) = .);
    . = ALIGN(8);
  } > CORE_SYM(DMI_DSPR)

  /* ERIKA API public initializated data DATA */
  CORE_SEC(.ee_api_data) : ALIGN(8) FLAGS(awl)
  {
    PROVIDE(CORE_SYM(ee_sdata_api) = .);
    *(CORE_SEC(.ee_api_data*))

    /* ALIGN(8) is needed because Range Addresses have to be aligned
       to 8 byte. */
    . += 1;
    . = ALIGN(8);
    /* End Erika API Data Section Symbol */
    PROVIDE(CORE_SYM(ee_edata_api) = .);
    /* ALIGN(16) the begin of OS_Application data and the following do the
       trick to move next OS-Application section 16 byte forward (at least) */
    . += 15;
    /* Each data region have to be at least 4 ALIGNED to ensure that LMA
       region (Load Memory Address) for other data region are valid for
       __copy_table */
    . = ALIGN(8);
  } > CORE_SYM(DMI_DSPR)

  /* ERIKA Kernel Sections */

  /* ERIKA Kernel private unitializated data section */
  CORE_SEC(.ee_kernel_bss) : ALIGN(8) FLAGS(aw)
  {
    /* begin ERIKA Kernel private unitializated data section symbol */
    PROVIDE(CORE_SYM(ee_sbss_kernel) = .);
    *(CORE_SEC(.ee_kernel_bss*))
    PROVIDE(CORE_SYM(ee_ebss_kernel) = .);
  } > CORE_SYM(DMI_DSPR)

  /* ERIKA Kernel private initializated data DATA */
  CORE_SEC(.ee_kernel_data) : ALIGN(8) FLAGS(awl)
  {
    PROVIDE(CORE_SYM(ee_sdata_kernel) = .);
    *(CORE_SEC(.ee_kernel_data*))

    /* ALIGN(8) is needed because Range Addresses have to be aligned
       to 8 byte. */
    . += 1;
    . = ALIGN(8);
    /* End Erika Kernel Data Section Symbol */
    PROVIDE(CORE_SYM(ee_edata_kernel) = .);

    /* ALIGN(16) the begin of OS_Application data and the following do the
       trick to move next OS-Application section 16 byte forward (at least) */
    . += 15;
    /* Each data region have to be at least 4 ALIGNED to ensure that LMA
       region (Load Memory Address) for other data region are valid for
       __copy_table */
    . = ALIGN(8);
  } > CORE_SYM(DMI_DSPR)

  /* ERIKA Kenel Stacks (Don't belong to any OS-Applications, special section
     is needed to handle memory protection and stack monitoring) */
  CORE_SEC(.ee_kernel_stack) : ALIGN(8) FLAGS(awl)
  {
    PROVIDE(CORE_SYM(ee_sstack_kernel) = .);
    *(CORE_SEC(.ee_kernel_stack*))
    /* Put the first OS-Application Section 16 forward respect the system
       sections to assure cross sections access protection.
       (Read the Note: at page 9-5 of
        TC_Architecture_vol1_TC161_TCS_TC16P_TC16E.pdf manual) */
    . += 15;
    . = ALIGN(8);
    /* each range address have to be 8 byte aligned */
    PROVIDE(CORE_SYM(ee_estack_kernel) = .);
  } > CORE_SYM(DMI_DSPR)

  /* GENERATED APPLICATION DATA SECTIONS - BEGIN */
  /* GENERATED APPLICATION DATA SECTIONS - END */

  /* Special Section Used to Provide RAM, Flash an code Symbols */
  CORE_SEC(ee_end) (NOLOAD) : FLAGS(a) {
    PROVIDE(CORE_SYM(ee_sall_code)    = ADDR(CORE_SEC(.init)));
    PROVIDE(CORE_SYM(ee_eall_code)    = ADDR(CORE_SEC(.dtors)) + SIZEOF(CORE_SEC(.dtors)));

    PROVIDE(CORE_SYM(ee_skernel_ram)  = CORE_SYM(__DMI_DSPR_BEGIN));
    PROVIDE(CORE_SYM(ee_ekernel_ram)  = .);
    PROVIDE(CORE_SYM(ee_skernel_code) = ADDR(CORE_SEC(.ee_kernel_text)));
    PROVIDE(CORE_SYM(ee_ekernel_code) = ADDR(CORE_SEC(.ee_kernel_text)) + SIZEOF(CORE_SEC(.ee_kernel_text)));
    PROVIDE(CORE_SYM(ee_sapi_const)   = ADDR(CORE_SEC(.sdata2)));
    PROVIDE(CORE_SYM(ee_eapi_const)   = ADDR(CORE_SEC(.rodata)) + SIZEOF(CORE_SEC(.rodata)));

    PROVIDE(CORE_SYM(ee_sapi_ram)     = ADDR(CORE_SEC(.ee_api_bss)));
    PROVIDE(CORE_SYM(ee_eapi_ram)     = ADDR(CORE_SEC(.ee_api_data)) + SIZEOF(CORE_SEC(.ee_api_data)));
    PROVIDE(CORE_SYM(ee_sapi_code)    = ADDR(CORE_SEC(.ee_api_text)));
    PROVIDE(CORE_SYM(ee_eapi_code)    = ADDR(CORE_SEC(.ee_api_text)) + SIZEOF(CORE_SEC(.ee_api_text)));
    /* Each PMU_PFLASH region have to be at least 4 ALIGNED to ensure that LMA
       region (Load Memory Address) for data region are valid for __copy_table
    */
    . = ALIGN(4);
  } > CORE_SYM(DMI_DSPR) AT> PMU_PFLASH

  /* Make sure CSA, stack and heap addresses are properly aligned. */
  _. = ASSERT ((CORE_SYM(__CSA_BEGIN) & 0x3f) == 0 , "illegal CSA start address") ;
  _. = ASSERT ((__CSA_SIZE & 0x3f) == 0 , "illegal CSA size") ;
  _. = ASSERT ((CORE_SYM(__ISTACK) & 7) == 0 , "ISTACK not doubleword aligned") ;
  _. = ASSERT ((CORE_SYM(__USTACK) & 7) == 0 , "USTACK not doubleword aligned") ;
  _. = ASSERT ((CORE_SYM(__HEAP_END) & 7) == 0 , "HEAP not doubleword aligned") ;
}


CORE_ID = CPU2;
/*
 * Name: ee_tc2Yx_gnu_ram_recollect.ld.tmpl
 * 
 * Description: 
 * Template for linker rules to recollect all sections in internal Flash
 * configuration
 */

SECTIONS
{
  /*
   * Allocate .text and other read-only sections.
   */

  /*
   * Code executed before calling main
   */
  CORE_SEC(.init) : ALIGN(8) FLAGS(ax)
  {
    KEEP(*(SORT(CORE_SEC(.init*))))
    . = ALIGN(8);
  } > CORE_SYM(PMI_PSPR)

  CORE_SEC(.fini)    : ALIGN(8) FLAGS(axl)
  {
    KEEP(*(SORT(CORE_SEC(.fini*))))
  } > CORE_SYM(PMI_PSPR)

  CORE_SEC(.cramtext) : ALIGN(8)
  {
    KEEP(*(SORT(CORE_SEC(.cramtext*))))
    . = ALIGN(8);
  } > CORE_SYM(PMI_PSPR)

  /*
   * Section for trap table
   */
  CORE_SEC(.traptab) : ALIGN(32) FLAGS(ax)
  {
    KEEP(*(CORE_SEC(.traptab*)))  /* Section for trap table */
    . = ALIGN(8);
  } > CORE_SYM(PMI_PSPR)

  /*
   * Section for interrupt table
   */
  CORE_SEC(.inttab) : ALIGN(32) FLAGS(ax)
  {
    KEEP (*(CORE_SEC(.inttab*)));
    . = ALIGN(8);
  } > CORE_SYM(PMI_PSPR)


  CORE_SEC(.text) : ALIGN(8) FLAGS(axl)
  {
    *(CORE_SEC(.text*))
    . = ALIGN(8);
  } > CORE_SYM(PMI_PSPR)

  /* OS Public Code */
  CORE_SEC(.ee_api_text) : ALIGN(8) FLAGS(axl) {
    /* range section have to be aligned to 8 bytes */
    PROVIDE(CORE_SYM(ee_stext_api) = . );
    *(CORE_SEC(.ee_api_text*))
    /* ALIGN(8) is needed because Range Addresses have to be aligned
       to 8 byte. */
    . += 1;
    . = ALIGN(8);
    /* End OS-Application Data Section Symbol */
    PROVIDE(CORE_SYM(ee_etext_api) = . );
    /* ALIGN(16) the begin of OS_Application data and the following do the
       trick to move next OS-Application section 16 byte forward (at least) */
    . += 15;
    /* Each PMU_PFLASH region have to be at least 4 ALIGNED to ensure that LMA
       region (Load Memory Address) for data region are valid for __copy_table
     */
    . = ALIGN(8);
  } > CORE_SYM(PMI_PSPR)

  /* OS Private Code */
  CORE_SEC(.ee_kernel_text) : ALIGN(16) FLAGS(axl) {
    /* The begin of the kernel code section is aligned to 16 to
       ensure space requiremente for cross sections access protection.
       (Read the Note: at page 9-5 of
       TC_Architecture_vol1_TC161_TCS_TC16P_TC16E.pdf manual) */
    PROVIDE(CORE_SYM(ee_stext_kernel) = . );
    *(CORE_SEC(.ee_kernel_text*))
    /* ALIGN(8) is needed because Range Addresses have to be aligned
       to 8 byte. */
    . += 1;
    . = ALIGN(8);
    /* End OS-Application Data Section Symbol */
    PROVIDE(CORE_SYM(ee_etext_kernel) = . );
    /* ALIGN(16) the begin of OS_Application data and the following do the
       trick to move next OS-Application section 16 byte forward (at least) */
    . += 15;
    /* Each PMU_PFLASH region have to be at least 4 ALIGNED to ensure that LMA
       region (Load Memory Address) for data region are valid for __copy_table
     */
    . = ALIGN(8);
  } > CORE_SYM(PMI_PSPR)

  /* GENERATED APPLICATION CODE SECTIONS - BEGIN */
  /* GENERATED APPLICATION CODE SECTIONS - END */

  /*
   * C++ exception handling tables.  NOTE: gcc emits .eh_frame
   * sections when compiling C sources with debugging enabled (-g).
   * If you can be sure that your final application consists
   * exclusively of C objects (i.e., no C++ objects), you may use
   * the -R option of the "strip" and "objcopy" utilities to remove
   * the .eh_frame section from the executable.
   */
  CORE_SEC(.gcc_except_table) : ALIGN(8) FLAGS(axl)
  {
    *(CORE_SEC(.gcc_except_table*))
  } > CORE_SYM(PMI_PSPR)

  CORE_SEC(.eh_frame) : FLAGS(axl)
  {
    CORE_SYM(__EH_FRAME_BEGIN__) = . ;
    *(CORE_SEC(.eh_frame*))
    CORE_SYM(__EH_FRAME_END__) = . ;
  } > CORE_SYM(PMI_PSPR)

  CORE_SEC(.jcr) : FLAGS(axl)
  {
    *(CORE_SEC(.jcr*))
    . = ALIGN(8);
  } > CORE_SYM(PMI_PSPR)

  /*
   * Constructors and destructors.
   */
  CORE_SEC(.ctors) : FLAGS(axl)
  {
    CORE_SYM(__CTOR_LIST__) = . ;
    LONG((CORE_SYM(__CTOR_END__) - CORE_SYM(__CTOR_LIST__)) / 4 - 2);
    /* gcc uses crtbegin.o to find the start of
       the constructors, so we make sure it is
       first.  Because this is a wildcard, it
       doesn't matter if the user does not
       actually link against crtbegin.o; the
       linker won't look for a file to match a
       wildcard.  The wildcard also means that it
       doesn't matter which directory crtbegin.o
       is in.  */
    KEEP (*crtbegin.o(.ctors))
    /* We don't want to include the .ctor section from
       from the crtend.o file until after the sorted ctors.
       The .ctor section from the crtend file contains the
       end of ctors marker and it must be last */
    KEEP (*(EXCLUDE_FILE (*crtend.o ) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    LONG(0);
    CORE_SYM(__CTOR_END__) = .;
    LONG(0) ;
    CORE_SYM(__CTOR_END__) = . ;
    . = ALIGN(8);
  } > CORE_SYM(PMI_PSPR)

  CORE_SEC(.dtors) : FLAGS(axl)
  {
    CORE_SYM(__DTOR_LIST__) = . ;
    LONG((CORE_SYM(__DTOR_END__) - CORE_SYM(__DTOR_LIST__)) / 4 - 2);
    KEEP (*crtbegin.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o ) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    LONG(0) ;
    CORE_SYM(__DTOR_END__) = . ;
    . = ALIGN(8);
  } > CORE_SYM(PMI_PSPR)

  CORE_SEC(.sdata2) : ALIGN(8)
  {
    *(CORE_SEC(.sdata2*))
    . = ALIGN(8);
  } > CORE_SYM(DMI_DSPR)

  CORE_SYM(_SMALL_DATA2_) = SIZEOF(CORE_SEC(.sdata2)) ? ADDR(CORE_SEC(.sdata2)) + 32k : (ADDR(CORE_SEC(.sdata2)) & 0xF0000000) + 32k ;
  CORE_SYM(__A1_MEM) = CORE_SYM(_SMALL_DATA2_);

  /*
   * Storage of write-protected data
   */
  CORE_SEC(.rodata) : FLAGS(arl)
  {
    PROVIDE(CORE_SYM(__rodata_start) = .);
    KEEP(*(SORT(CORE_SEC(.rodata*))))

    . = ALIGN(4) ;
    PROVIDE(CORE_SYM(ee_stacks_table) = .) ;
    LONG(0 + ADDR(CORE_SEC(.ee_kernel_stack)));      LONG(SIZEOF(CORE_SEC(.ee_kernel_stack)));
/* GENERATED STACKS - BEGIN */
/* GENERATED STACKS TABLE - END */
    LONG(-1);                 LONG(-1);
    /* ALIGN(8) because range addresses have to be 8 bytes aligned */
    . = ALIGN(8);
  } > CORE_SYM(DMI_DSPR)

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
    PROVIDE(CORE_SYM(ZBSS_BASE) = .);
    *(CORE_SEC(.zbss*))
    . = ALIGN(8);
    PROVIDE(CORE_SYM(ZBSS_END) = .);
  } > CORE_SYM(DMI_DSPR)

  CORE_SEC(.zdata) : FLAGS(awzl)
  {
    PROVIDE(CORE_SYM(ZDATA_BASE) = .);
    *(CORE_SEC(.zdata*))
    . = ALIGN(8);
    PROVIDE(CORE_SYM(ZDATA_END) = .);
  } > CORE_SYM(DMI_DSPR)

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
    PROVIDE(CORE_SYM(__sbss_start) = .);
    *(CORE_SEC(.sbss*))
    PROVIDE(CORE_SYM(__sbss_end) = .);
    . = ALIGN(8);
  } > CORE_SYM(DMI_DSPR)

  /*
   * Initialised data in section ’.sdata’, addressable by small data area pointer (%a0)
   */
  CORE_SEC(.sdata) : ALIGN(8) FLAGS(awls)
  {
    PROVIDE(CORE_SYM(SDATA_BASE) = .);
    PROVIDE(CORE_SYM(__sdata_start) = .);
    *(CORE_SEC(.sdata*))
    . = ALIGN(8);
    PROVIDE(CORE_SYM(SDATA_END) = .);
    PROVIDE(CORE_SYM(__sdata_end) = .);
  } > CORE_SYM(DMI_DSPR)

  CORE_SYM(_SMALL_DATA_) = (SIZEOF(CORE_SEC(.sbss)) + SIZEOF(CORE_SEC(.sdata))) ? ADDR(CORE_SEC(.sbss)) + 32k : (ADDR(CORE_SEC(.sbss)) & 0xF0000000) ;
  CORE_SYM(__A0_MEM) = CORE_SYM(_SMALL_DATA_) ;

  /*
   * Not Initialised data.
   * Allocate space for BSS sections.
   */
  CORE_SEC(.bss) (NOLOAD) : FLAGS(aw)
  {
    PROVIDE(CORE_SYM(BSS_BASE) = .);
    PROVIDE(CORE_SYM(__bss_start) = .);
    *(CORE_SEC(.bss*))
    PROVIDE(CORE_SYM(__bss_end) = .);
    . = ALIGN(8);
  } > CORE_SYM(DMI_DSPR)

  /*
   * Initialised data
   */
  CORE_SEC(.data) : ALIGN(8) FLAGS(awl)
  {
    PROVIDE(CORE_SYM(DATA_BASE) = .);
    PROVIDE(CORE_SYM(__data_start) = .);
    *(CORE_SEC(.data*))
    . = ALIGN(8) ;
    PROVIDE(CORE_SYM(DATA_END) = .);
    PROVIDE(CORE_SYM(__data_end) = .);
  } > CORE_SYM(DMI_DSPR)

  /*
   * Section for ustack
   */  
  CORE_SEC(.ustack) : ALIGN(8) FLAGS(awl)
  {
    PROVIDE(CORE_SYM(__USTACK_BEGIN) = .);
    . += __USTACK_SIZE ;
    . = ALIGN(8);
    CORE_SYM(__USTACK) = . ;
    PROVIDE(CORE_SYM(__USTACK_END) = .);
  } > CORE_SYM(DMI_DSPR)

  /*
   * Section for istack
   */  
  CORE_SEC(.istack) : ALIGN(8) FLAGS(awl)
  {
    PROVIDE(CORE_SYM(__ISTACK_BEGIN) = .);
    . += __ISTACK_SIZE ;
    . = ALIGN(8);
    CORE_SYM(__ISTACK) = . ;
    PROVIDE(CORE_SYM(__ISTACK_END) = .);
  } > CORE_SYM(DMI_DSPR)

  /*
   * Section for heap
   */  
  CORE_SEC(.heap) : ALIGN(8) FLAGS(awl)
  {
    PROVIDE(CORE_SYM(__HEAP_BEGIN) = .);
    PROVIDE(CORE_SYM(__HEAP) = .);
    . += __HEAP_SIZE ;
    . = ALIGN(8);
    CORE_SYM(__HEAP_END) = . ;
  } > CORE_SYM(DMI_DSPR)

  /*
   * Section for CSA
   */  
  CORE_SEC(.csa) : ALIGN(64) FLAGS(awl)
  {
    CORE_SYM(__CSA_BEGIN) = .;
    PROVIDE(CORE_SYM(__CSA) = .);
    . += __CSA_SIZE ;
    . = ALIGN(64);
    PROVIDE(CORE_SYM(__CSA_END) = .);
  } > CORE_SYM(DMI_DSPR)

  /* ERIKA API Sections */

  /* ERIKA API public unitializated data section */
  CORE_SEC(.ee_api_bss) :  ALIGN(8) FLAGS(aw)
  {
    /* begin ERIKA API public unitializated data section symbol */
    PROVIDE(CORE_SYM(ee_sbss_api) = .);
    *(CORE_SEC(*.ee_api_bss*))
    PROVIDE(CORE_SYM(ee_ebss_api) = .);
    . = ALIGN(8);
  } > CORE_SYM(DMI_DSPR)

  /* ERIKA API public initializated data DATA */
  CORE_SEC(.ee_api_data) : ALIGN(8) FLAGS(awl)
  {
    PROVIDE(CORE_SYM(ee_sdata_api) = .);
    *(CORE_SEC(.ee_api_data*))

    /* ALIGN(8) is needed because Range Addresses have to be aligned
       to 8 byte. */
    . += 1;
    . = ALIGN(8);
    /* End Erika API Data Section Symbol */
    PROVIDE(CORE_SYM(ee_edata_api) = .);
    /* ALIGN(16) the begin of OS_Application data and the following do the
       trick to move next OS-Application section 16 byte forward (at least) */
    . += 15;
    /* Each data region have to be at least 4 ALIGNED to ensure that LMA
       region (Load Memory Address) for other data region are valid for
       __copy_table */
    . = ALIGN(8);
  } > CORE_SYM(DMI_DSPR)

  /* ERIKA Kernel Sections */

  /* ERIKA Kernel private unitializated data section */
  CORE_SEC(.ee_kernel_bss) : ALIGN(8) FLAGS(aw)
  {
    /* begin ERIKA Kernel private unitializated data section symbol */
    PROVIDE(CORE_SYM(ee_sbss_kernel) = .);
    *(CORE_SEC(.ee_kernel_bss*))
    PROVIDE(CORE_SYM(ee_ebss_kernel) = .);
  } > CORE_SYM(DMI_DSPR)

  /* ERIKA Kernel private initializated data DATA */
  CORE_SEC(.ee_kernel_data) : ALIGN(8) FLAGS(awl)
  {
    PROVIDE(CORE_SYM(ee_sdata_kernel) = .);
    *(CORE_SEC(.ee_kernel_data*))

    /* ALIGN(8) is needed because Range Addresses have to be aligned
       to 8 byte. */
    . += 1;
    . = ALIGN(8);
    /* End Erika Kernel Data Section Symbol */
    PROVIDE(CORE_SYM(ee_edata_kernel) = .);

    /* ALIGN(16) the begin of OS_Application data and the following do the
       trick to move next OS-Application section 16 byte forward (at least) */
    . += 15;
    /* Each data region have to be at least 4 ALIGNED to ensure that LMA
       region (Load Memory Address) for other data region are valid for
       __copy_table */
    . = ALIGN(8);
  } > CORE_SYM(DMI_DSPR)

  /* ERIKA Kenel Stacks (Don't belong to any OS-Applications, special section
     is needed to handle memory protection and stack monitoring) */
  CORE_SEC(.ee_kernel_stack) : ALIGN(8) FLAGS(awl)
  {
    PROVIDE(CORE_SYM(ee_sstack_kernel) = .);
    *(CORE_SEC(.ee_kernel_stack*))
    /* Put the first OS-Application Section 16 forward respect the system
       sections to assure cross sections access protection.
       (Read the Note: at page 9-5 of
        TC_Architecture_vol1_TC161_TCS_TC16P_TC16E.pdf manual) */
    . += 15;
    . = ALIGN(8);
    /* each range address have to be 8 byte aligned */
    PROVIDE(CORE_SYM(ee_estack_kernel) = .);
  } > CORE_SYM(DMI_DSPR)

  /* GENERATED APPLICATION DATA SECTIONS - BEGIN */
  /* GENERATED APPLICATION DATA SECTIONS - END */

  /* Special Section Used to Provide RAM, Flash an code Symbols */
  CORE_SEC(ee_end) (NOLOAD) : FLAGS(a) {
    PROVIDE(CORE_SYM(ee_sall_code)    = ADDR(CORE_SEC(.init)));
    PROVIDE(CORE_SYM(ee_eall_code)    = ADDR(CORE_SEC(.dtors)) + SIZEOF(CORE_SEC(.dtors)));

    PROVIDE(CORE_SYM(ee_skernel_ram)  = CORE_SYM(__DMI_DSPR_BEGIN));
    PROVIDE(CORE_SYM(ee_ekernel_ram)  = .);
    PROVIDE(CORE_SYM(ee_skernel_code) = ADDR(CORE_SEC(.ee_kernel_text)));
    PROVIDE(CORE_SYM(ee_ekernel_code) = ADDR(CORE_SEC(.ee_kernel_text)) + SIZEOF(CORE_SEC(.ee_kernel_text)));
    PROVIDE(CORE_SYM(ee_sapi_const)   = ADDR(CORE_SEC(.sdata2)));
    PROVIDE(CORE_SYM(ee_eapi_const)   = ADDR(CORE_SEC(.rodata)) + SIZEOF(CORE_SEC(.rodata)));

    PROVIDE(CORE_SYM(ee_sapi_ram)     = ADDR(CORE_SEC(.ee_api_bss)));
    PROVIDE(CORE_SYM(ee_eapi_ram)     = ADDR(CORE_SEC(.ee_api_data)) + SIZEOF(CORE_SEC(.ee_api_data)));
    PROVIDE(CORE_SYM(ee_sapi_code)    = ADDR(CORE_SEC(.ee_api_text)));
    PROVIDE(CORE_SYM(ee_eapi_code)    = ADDR(CORE_SEC(.ee_api_text)) + SIZEOF(CORE_SEC(.ee_api_text)));
    /* Each PMU_PFLASH region have to be at least 4 ALIGNED to ensure that LMA
       region (Load Memory Address) for data region are valid for __copy_table
    */
    . = ALIGN(4);
  } > CORE_SYM(DMI_DSPR) AT> PMU_PFLASH

  /* Make sure CSA, stack and heap addresses are properly aligned. */
  _. = ASSERT ((CORE_SYM(__CSA_BEGIN) & 0x3f) == 0 , "illegal CSA start address") ;
  _. = ASSERT ((__CSA_SIZE & 0x3f) == 0 , "illegal CSA size") ;
  _. = ASSERT ((CORE_SYM(__ISTACK) & 7) == 0 , "ISTACK not doubleword aligned") ;
  _. = ASSERT ((CORE_SYM(__USTACK) & 7) == 0 , "USTACK not doubleword aligned") ;
  _. = ASSERT ((CORE_SYM(__HEAP_END) & 7) == 0 , "HEAP not doubleword aligned") ;
}

/*
 * Name: ee_tc2Yx_gnu_ram_recollect_suffix.ld
 * 
 * Description: 
 * Linker rules end rules to recollect all sections in internal Flash
 * configuration
 */

CORE_ID = GLOBAL ;

SECTIONS
{
  /* Define a default symbol for address 0. */
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
