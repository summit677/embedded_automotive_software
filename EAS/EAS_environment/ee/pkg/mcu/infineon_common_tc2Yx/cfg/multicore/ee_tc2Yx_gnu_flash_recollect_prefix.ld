/*
 * Name: ee_tc2Yx_gnu_flash_recollect_prefix.ld
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
  } > PMU_PFLASH

  /* Erika RTOS Multicore global data */
  ee_mcglobald : ALIGN(8) FLAGS(awlc0)
  {
    PROVIDE(ee_mcglobald_begin = .);
    *(SORT(ee_mcglobald*))
    /* . += 1; */
    . = ALIGN(8);
    PROVIDE(ee_mcglobald_end = .);
  } > LMU_SRAM_NC AT> PMU_PFLASH

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
  } > PMU_PFLASH
}
