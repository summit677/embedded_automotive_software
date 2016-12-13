/*
 * Name: ee_tc27x_gnu_memory_cpu0.x
 *
 * Description: TC27x memory description for cpu0 in multicore
 * environment
 */

/*
 * Global
 */

OUTPUT_FORMAT("elf32-tricore")
OUTPUT_ARCH(tricore)
ENTRY(_START)

__TRICORE_DERIVATE_MEMORY_MAP__ = 0x260;

  /* Local Data RAM (DSPR) */
__DMI_DSPR_BEGIN = 0x70000000;

  /* CSA list size */
__CSA_SIZE = DEFINED (__CSA_SIZE) ? __CSA_SIZE : 8k ;
  /* User stack Size */
__USTACK_SIZE = DEFINED (__USTACK_SIZE) ? __USTACK_SIZE : 2k;
  /* Interrupt Stack Size */
__ISTACK_SIZE = DEFINED (__ISTACK_SIZE) ? __ISTACK_SIZE : 0k;
  /* Heap size */
__HEAP_SIZE = DEFINED (__HEAP_SIZE) ? __HEAP_SIZE : 0;

MEMORY
{
  /* - 0x(8)A0000000--0x(8)A000001F ABM Header Start */
  /* - 0x(8)A0000020 Boot Address */
  /* - 0x(A)80000020--0x(A)80017FFF Code Application Pattern */

  /* - Program Flash Memory for startup (PFLASH0) */
  PMU_PFLASH_STARTUP (rx!p): org = 0x80000000, len = 1k

  /* - Program Flash Memory for 18 bit absolute addressing (PFLASH0) */
  PMU_PFLASH_ABS18  (rx!p): org = 0x80000400, len = 7k
  /* - Program Flash Memory (PFLASH0). I take 16k for absolute 18 bit
       addressing and for pflash_startup space to be divided between
       the 3 cores */
  PMU_PFLASH (rx!p):  org = 0x80004000, len = 984k

  /* - Boot ROM (BROM) */
  BROM (rx!p):  org = 0x8FFF8000, len = 32k

  /* - Scratch-Pad RAM (PSPR) */
  PMI_PSPR (wx!p):  org = 0x70100000, len = 16k
  /* - Local Data Scratch-Pad RAM (DSPR) */
  DMI_DSPR (w!xp): org = 0x70000000, len = 72k
  /* - Global Data RAM Doesn't exist I'll use DMI_DSPR1 */
  LMU_SRAM (w!xp):  org = 0x60000000, len = 32k
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
_. = ASSERT ((__TRICORE_DERIVATE_MEMORY_MAP__ == __TRICORE_DERIVATE_NAME__), "Using wrong Memory Map. This Map is for TC27x");
*/
/* INSERT BEFORE .startup */
