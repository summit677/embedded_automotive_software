/*
 * Name: ee_tc27x_gnu_memory_singlecore.x
 *
 * Description: TC27x memory description (only CPU0)
 */

/*
 * Global
 */

OUTPUT_FORMAT("elf32-tricore")
OUTPUT_ARCH(tricore)
ENTRY(_START)

 __TRICORE_DERIVATE_MEMORY_MAP__ = 0x270;

/* Start-up Program Flash Memory */
__PMU_PFLASH_STARTUP_BEGIN = 0x80000000;
__PMU_PFLASH_STARTUP_SIZE  = 1k;
/* Program Flash Memory (PFLASH0) */
__PMU_PFLASH_BEGIN = 0x80004000;
__PMU_PFLASH_SIZE  = 1984k;
/* Program Flash Memory (PFLASH1) */
__PMU_PFLASH1_BEGIN = 0x80200000;
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
__PMI_PSPR_SIZE = 24K;
/* Local Data RAM (DSPR) */
__DMI_DSPR_BEGIN = 0xD0000000;
__DMI_DSPR_SIZE = 112K;
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
  /* - Data Flash Memory (DFLASH0) */
  PMU_DFLASH0 (r!xp):  org = 0xAF000000, len = 1M
  /* - Data Flash Memory (DFLASH0_1) */
  PMU_DFLASH0_1 (r!xp):  org = 0xAF100000, len = 16K
  /* - Data Flash Memory (DFLASH1) */
  PMU_DFLASH1 (r!xp):  org = 0xAF110000, len = 64K
  /* - Boot ROM (BROM) */
  BROM (rx!p):  org = 0x8FFF8000, len = 32K

  /* - Scratch-Pad RAM (PSPR) */
  PMI_PSPR          (wx!p):  org = 0xC0000000, len = 24k

  /* - Local Data Scratch-Pad RAM (DSPR) */
  DMI_DSPR (w!xp): org = 0xD0000000, len = 112K
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
_. = ASSERT ((__TRICORE_DERIVATE_MEMORY_MAP__ == __TRICORE_DERIVATE_NAME__), "Using wrong Memory Map. This Map is for TC27x");
*/
/* INSERT BEFORE .startup */
