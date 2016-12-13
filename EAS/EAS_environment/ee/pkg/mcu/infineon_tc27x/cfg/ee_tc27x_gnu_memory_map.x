/*
 * Name: ee_tc26x_gnu_memory_map.x
 *
 * Description: TC26x memory description
 */

/*
 * Global
 */

OUTPUT_FORMAT("elf32-tricore")
OUTPUT_ARCH(tricore)
ENTRY(_START)

__TRICORE_DERIVATE_MEMORY_MAP__ = 0x270;

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
  DMI_DSPR0_LOCAL  (w!xp): org = 0xD0000000, len = 112k
  /* - CORE0 Data Scratch-Pad RAM (DSPR) */
  DMI_DSPR0        (w!xp): org = 0x70000000, len = 112k
  /* - LOCAL Program Scratch-Pad RAM (PSPR) */
  PMI_PSPR0_LOCAL  (w!xp): org = 0xC0000000, len = 24k
  /* - CORE0 Scratch-Pad RAM (PSPR) */
  PMI_PSPR0        (w!xp): org = 0x70100000, len = 24k

  /* - CORE1 Local Data Scratch-Pad RAM (DSPR) */
  DMI_DSPR1_LOCAL  (w!xp): org = 0xD0000000, len = 120k
  /* - CORE1 Data Scratch-Pad RAM (DSPR) */
  DMI_DSPR1        (w!xp): org = 0x60000000, len = 120k
  /* - LOCAL Program Scratch-Pad RAM (PSPR) */
  PMI_PSPR1_LOCAL  (w!xp): org = 0xC0000000, len = 24k
  /* - CORE1 Program Scratch-Pad RAM (PSPR) */
  PMI_PSPR1        (w!xp): org = 0x60100000, len = 24k

  /* - CORE2 Local Data Scratch-Pad RAM (DSPR) */
  DMI_DSPR2_LOCAL  (w!xp): org = 0xD0000000, len = 120k
  /* - CORE2 Data Scratch-Pad RAM (DSPR) */
  DMI_DSPR2        (w!xp): org = 0x50000000, len = 120k
  /* - LOCAL Program Scratch-Pad RAM (PSPR) */
  PMI_PSPR2_LOCAL  (w!xp): org = 0xC0000000, len = 24k
  /* - CORE2 Program Scratch-Pad RAM (PSPR) */
  PMI_PSPR2        (w!xp): org = 0x50100000, len = 24k

  /* - Program Flash Memory (PFLASH0) */
  PMU_PFLASH0       (rx!p): org = 0x80000000, len = 2M
  PMU_PFALSH0_NC    (rx!p): org = 0xA0000000, len = 2M

  /* - Program Flash Memory (PFLASH0) -Not used to allocate and sections- */
  PMU_PFLASH1       (rx!p): org = 0x80200000, len = 2M
  PMU_PFALSH1_NC    (rx!p): org = 0xA0200000, len = 2M

  /* - Data Flash Memory (DFLASH0) */
  PMU_DFLASH0       (w!xp): org = 0xAF000000, len = 1M

  /* - Boot ROM (BROM) */
  BROM              (rx!p): org = 0x8FFF8000, len = 32k
  BROM_NC           (rx!p): org = 0xAFFF8000, len = 32k

  /* - Global Data RAM  */
  /* LMU_SRAM          (w!xp): org = 0x90000000, len = 32K XXX EG: I don't known why but if this is defined the linker will use cached memory for common structures */
  LMU_SRAM_NC       (w!xp): org = 0xB0000000, len = 32K

  /* - Emulation Device Memory */
  PMU_EDMEM         (w!xp): org = 0x9F000000, len = 1M
  PMU_EDMEM_NC      (w!xp): org = 0xBF000000, len = 1M
}

/* Map non-cached global memory addresses into cached */
REGION_MAP( GLOBAL , ORIGIN(PMU_PFALSH0_NC), LENGTH(PMU_PFALSH0_NC), ORIGIN(PMU_PFLASH0))
REGION_MAP( GLOBAL , ORIGIN(PMU_PFALSH1_NC), LENGTH(PMU_PFALSH1_NC), ORIGIN(PMU_PFLASH1))
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

