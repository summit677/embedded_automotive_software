/*
 * Name: ee_tc27x_gnu_memory_cpu0.x
 *
 * Description: TC27x memory description for cpu0 in multicore
 * environment
 */

/*
 * Global
 */

/* Start-up Program Flash Memory */
__PMU_PFLASH_STARTUP_BEGIN = 0x80000000;
__PMU_PFLASH_STARTUP_SIZE  = 1k;

/* Absolute 18bit addressing data */
__PMU_PFLASH_ABS18_BEGIN  = 0x80000400;
__PMU_PFLASH_ABS18_SIZE   = 7k;

/*Program Flash Memory (PFLASH0)*/
__PMU_PFLASH_BEGIN = 0x80004000;
__PMU_PFLASH_SIZE  = 1984k;

/*Data Flash Memory (DFLASH0)*/
__PMU_DFLASH0_BEGIN = 0xAF000000;
__PMU_DFLASH0_SIZE = 512k;
/*Data Flash Memory (DFLASH0_1)*/
__PMU_DFLASH0_1_BEGIN = 0xAF100000;
__PMU_DFLASH0_1_SIZE = 16K;

/*Data Flash Memory (DFLASH1)*/
__PMU_DFLASH1_BEGIN = 0xAF110000;
__PMU_DFLASH1_SIZE = 32K;

/*Boot ROM (BROM)*/
__BROM_BEGIN = 0x8FFF8000;
__BROM_SIZE = 32K;

/*Scratch-Pad RAM (PSPR)*/
__PMI_PSPR_BEGIN = 0x70100000;
__PMI_PSPR_SIZE = 24K;
/*Local Data RAM (DSPR)*/
__DMI_DSPR_BEGIN = 0x70000000;
__DMI_DSPR_SIZE = 112K;

/*Global Data RAM (LMU)*/
__LMU_SRAM_BEGIN = 0xB0000000;
__LMU_SRAM_SIZE = 16K;

/* CSA list size (I cannot handle default value: it must to be passed by
   command line) */
/* __CSA_SIZE = (CSA_SIZE != 0) ? __CSA_SIZE : 8k; */
/* User stack Size (I cannot handle default value: it must to be passed by
   command line) */ 
/* __USTACK_SIZE = (USTACK_SIZE != 0) ? __USTACK_SIZE : 2K; */
/* Interrupt Stack Size -Unused- */
/* __ISTACK_SIZE = (ISTACK_SIZE != 0) ? __ISTACK_SIZE : 0K; */
__ISTACK_SIZE = 0K;
/* Heap size -Unused- */
__HEAP_SIZE = 0;

MEMORY
{
  /* - 0x(8)A0000000--0x(8)A000001F ABM Header Start */
  /* - 0x(8)A0000020 Boot Address */
  /* - 0x(A)80000020--0x(A)80017FFF Code Application Pattern */

  /* - Program Flash Memory for startup (PFLASH0) */
  PMU_PFLASH_STARTUP: org = 0x80000000, len = 1k

  /* - Program Flash Memory for 18 bit absolute addressing (PFLASH0) */
  PMU_PFLASH_ABS18: org = 0x80000400, len = 7k
  /* - Program Flash Memory (PFLASH0). I take 16k for absolute 18 bit
       addressing and for pflash_startup space to be divided between
       the 3 cores */
  PMU_PFLASH:  org = 0x80004000, len = 1984k
  /* - Data Flash Memory (DFLASH0) */
  PMU_DFLASH0:  org = 0xAF000000, len = 512k
  /* - Data Flash Memory (DFLASH0_1) */
  PMU_DFLASH0_1:  org = 0xAF100000, len = 16K
  /* - Data Flash Memory (DFLASH1) */
  PMU_DFLASH1:  org = 0xAF110000, len = 32K

  /* - Boot ROM (BROM) */
  BROM:  org = 0x8FFF8000, len = 32K

  /* - Scratch-Pad RAM (PSPR) (0x2500 + 0x3B00 = 24k) */
  PMI_PSPR_STARTUP:  org = 0x70100000, len = 0x2500
  PMI_PSPR:  org = 0x70102500, len = 0x3B00
  /* - Local Data Scratch-Pad RAM (DSPR) */
  DMI_DSPR: org = 0x70000000, len = 112K
  /* - Global Data RAM */
  LMU_SRAM:  org = 0xB0000000, len = 16K
}

