/*
 * Name: ee_tc2Yx_gnu_flash_startup_core1.ld
 * 
 * Description: 
 * Linker rules for startup flash core1 configuration
 */

SECTIONS {
  /*
   * Code sections
   */
  EE_tc2Yx_cpu2_start :
  {
    KEEP(*(*.EE_tc2Yx_cpu2_start))
    . = ALIGN(8);
  } > PMI_PSPR
}

  /* Make sure that EE_tc2Yx_cpu1_start symbol is in the right place. */
  _. = ASSERT (EE_tc2Yx_cpu2_startT == 0x50100000, "EE_tc2Yx_cpu2_start is not in the right place");


