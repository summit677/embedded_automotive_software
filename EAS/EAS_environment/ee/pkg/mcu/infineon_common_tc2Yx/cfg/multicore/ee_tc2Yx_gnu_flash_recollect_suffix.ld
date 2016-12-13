/*
 * Name: ee_tc2Yx_gnu_flash_recollect_suffix.ld
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
