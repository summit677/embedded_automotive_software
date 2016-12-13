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

