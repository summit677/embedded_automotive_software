
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

