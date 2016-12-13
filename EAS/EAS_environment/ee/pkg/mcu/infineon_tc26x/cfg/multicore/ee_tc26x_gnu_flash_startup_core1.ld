/*
 * Name: ee_tc2Yx_gnu_flash_startup_core1.ld
 * 
 * Description: 
 * Linker rules for startup flash core1 configuration
 */

SECTIONS {
  /*
   * The startup code should be placed where the CPU expects it after a reset,
   * so we try to locate it first, no matter where it appears in the list of
   * objects and libraries (note: because the wildcard pattern doesn't match
   * directories, we'll try to find crt0.o in various (sub)directories).
   */

  /*
  .bmhd_0 (0x80000000) :
  {
    BootModeIndex = .;
    LONG (0x00000000);
    LONG (0xb3590070);
    LONG (0x00000000);
    LONG (0x00000000);
    LONG (0x00000000);
    LONG (0x00000000);
    LONG (0x791eb864);
    LONG (0x86e1479b);
  } */
  /*This section is always required as Boot mode header 0 address absolutely restricted at address 0x80000000*/
  .bmhd0 (0x80000000) : FLAGS(arl)
  {
    BootModeHeader0 = .;
    KEEP (*(.bmhd0))
  } > PMU_PFLASH_STARTUP

  /*This section is always required as user start address absolutely restricted at address 0x80000020*/
  .startup (0x80000020) : FLAGS(rxl)
  {
    BootModeIndex = .;
    PROVIDE(__startup_code_start = .);
    KEEP (*(.startup))
    KEEP (*(ee_kernel_start))
    . = ALIGN(8);
    PROVIDE(__startup_code_end = .);
  } > PMU_PFLASH_STARTUP

  /*
   * Code sections
   */
  EE_tc2Yx_cpu1_start :
  {
    KEEP(*(*.EE_tc2Yx_cpu1_start))
    . = ALIGN(8);
  } > PMU_PFLASH
}

  /* Make sure that _START symbol is in the right place. */
  _. = ASSERT (_START == 0x80000020, "_START is not in the right place");
  /* Make sure that EE_tc2Yx_cpu1_start symbol is in the right place. */
  _. = ASSERT (EE_tc2Yx_cpu1_start == 0x80100000, "EE_tc2Yx_cpu1_start is not in the right place");


