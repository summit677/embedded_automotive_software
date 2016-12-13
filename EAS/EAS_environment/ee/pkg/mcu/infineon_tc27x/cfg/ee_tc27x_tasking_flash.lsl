// TASKING VX-toolset for TriCore
// Eclipse project linker script file
// 

#ifndef CPU_NUMID
#define EE_SINGLE_CPU
#elif (CPU_NUMID == 0)
#define EE_MASTER_CPU
#endif

#ifdef EE_SINGLE_CPU

#include "tc27x_singlecore.lsl"

//Place Stack in DSPR0
section_layout mpe:vtc:linear
{
    // "stacks" group
    group (run_addr=mem:mpe:dspr0)
    {
        select "*.ustack_tc0";
        select "*.istack_tc0";
    }

    // "data" group
    group (run_addr=mem:mpe:dspr0 | mem:mpe:lmuram)
    {
        select (attributes=-x+w);
    }
}

section_layout mpe:vtc:abs18
{
    // "data" group
    group (run_addr=mem:mpe:dspr0 | mem:mpe:lmuram)
    {
        select (attributes=-x+w);
    }
}
#elif (!defined EE_TC27X_LOCK_STEP)

/* Relocate CPU1 & CPU2 INTTAB and TRAPTAB */
#if (CPU_NUMID == 1)
#define INTTAB  0x80200000
#elif (CPU_NUMID == 2)
#define INTTAB  0x80300000
#endif

/* Needed to redefine memories */
#define  __REDEFINE_ON_CHIP_ITEMS
#include "tc27x.lsl"

/* Master CPU DFLASH partition (not used I Will Redefine Memories Layout) */
// #define MASTER_CPU_DFLASH mem:mpe:dflash0[0 .. 512k] | [0xAF100000 .. 0xAF104000] | mem:mpe:dflash1[0 .. 32k]
// #define CPU1_DFLASH mem:mpe:dflash0[512k .. 768k] | mem:mpe:dflash1[32k .. 48k]
// #define CPU2_DFLASH mem:mpe:dflash0[768k .. 1M] | mem:mpe:dflash1[48k .. 64k]

/* CPU1 and CPU2 start addresses (BASE of CORE pflash + sizeof(INTTAB) (0x2000) + sizeof(TRAPTAB) (0x100) */
#define EE_TC2YX_CPU1_START_ADDR    0x80206300
#define EE_TC2YX_CPU2_START_ADDR    0x80306300

// Specify a multi-core processor environment (mpe)
processor mpe
{
    derivative = ee_tc27x;
}

derivative ee_tc27x extends tc27x {

    memory dspr0 // Data Scratch Pad Ram
    {
        mau = 8;
        size = 112k;
        type = ram;
        priority = 2;
        map (dest=bus:tc0:fpi_bus, dest_offset=0xd0000000, size=112k, priority=8);
        map (dest=bus:sri, dest_offset=0x70000000, size=112k);
    }

    memory pspr0 // Program Scratch Pad Ram
    {
        mau = 8;
        size = 24k;
        type = rom; // Workaround for TASKING behaviour, it Should be RAM
        map (dest=bus:tc0:fpi_bus, dest_offset=0xc0000000, size=24k, priority=8);
        map (dest=bus:sri, dest_offset=0x70100000, size=24k);
    }

    memory dspr1 // Data Scratch Pad Ram
    {
        mau = 8;
        size = 120k;
        type = ram;
        priority = 2;
        map (dest=bus:tc1:fpi_bus, dest_offset=0xd0000000, size=120k, priority=8);
        map (dest=bus:sri, dest_offset=0x60000000, size=120k);
    }

    memory pspr1 // Program Scratch Pad Ram
    {
        mau = 8;
        size = 24k;
        type = rom; // Workaround for TASKING behaviour, it Should be RAM
        map (dest=bus:tc1:fpi_bus, dest_offset=0xc0000000, size=24k, priority=8);
        map (dest=bus:sri, dest_offset=0x60100000, size=24k);
    }

    memory dspr2 // Data Scratch Pad Ram
    {
        mau = 8;
        size = 120k;
        type = ram;
        priority = 2;
        map (dest=bus:tc2:fpi_bus, dest_offset=0xd0000000, size=120k, priority=8);
        map (dest=bus:sri, dest_offset=0x50000000, size=120k);
    }

    memory pspr2 // Program Scratch Pad Ram
    {
        mau = 8;
        size = 24k;
        type = rom; // Workaround for TASKING behaviour, it Should be RAM
        map (dest=bus:tc2:fpi_bus, dest_offset=0xc0000000, size=24k, priority=8);
        map (dest=bus:sri, dest_offset=0x50100000, size=24k);
    }

    memory brom
    {
        mau = 8;
        size = 32k;
        type = reserved rom;
        map     cached (dest=bus:sri, dest_offset=0x8fff8000,           size=32k);
        map not_cached (dest=bus:sri, dest_offset=0xafff8000, reserved, size=32k);
    }

    /* This "small piece of flash" is "reserved" for start-up code (_START and EE_tc2Yx_start (tc0))
       and have to be seen by all the cores (Withount linking start-up code the linker strip away
       everything from the ELF) */
    memory pflash_startup
    {
        mau = 8;
        size = 1k;
        type = reserved rom;
        //map     cached (dest=bus:sri, dest_offset=0x801ff000,           size=1k);
        //map not_cached (dest=bus:sri, dest_offset=0xa01ff000, reserved, size=1k);
        map     cached (dest=bus:sri, dest_offset=0x80000000,           size=1k);
        map not_cached (dest=bus:sri, dest_offset=0xa0000000, reserved, size=1k);
    }

#ifdef EE_MASTER_CPU
    /* Begin of Section 8 divided by all the three cores for absolute 18 bit addressing */
    memory pflash_abs18
    {
        mau = 8;
        size = 7k;
        type = rom;
        map     cached (dest=bus:sri, dest_offset=0x80000400,           size=7k);
        map not_cached (dest=bus:sri, dest_offset=0xa0000400, reserved, size=7k);
    }

    /* From pflash0 (2M) I take 16k for absolute 18 bit addressing and for pflash_startup
       space to be divided between the 3 cores */
    memory pflash
    {
        mau = 8;
        size = 1984k;
        type = rom;
        priority = 8;
        map     cached (dest=bus:sri, dest_offset=0x80004000,           size=1984k);
        map not_cached (dest=bus:sri, dest_offset=0xa0004000, reserved, size=1984k);
    }

    memory dflash
    {
        mau = 8;
        size = 512k+16k+32k;
        type = reserved nvram;
        map (dest=bus:sri, dest_offset=0xaf000000, size=512k);
        map (dest=bus:sri, dest_offset=0xaf100000, size=16k);
        map (dest=bus:sri, dest_offset=0xaf110000, size=32k);
    }

    memory lmuram
    {
        mau = 8;
        size = 16k;
        type = ram;
        map     cached (dest=bus:sri, dest_offset=0x90000000,           size=16k);
        map not_cached (dest=bus:sri, dest_offset=0xb0000000, reserved, size=16k);
    }

#elif (CPU_NUMID == 1)
    /* Begin of Section 8 divided by all the three cores for absolute 18 bit addressing */
    memory pflash_abs18
    {
        mau = 8;
        size = 4k;
        type = rom;
        map     cached (dest=bus:sri, dest_offset=0x80002000,           size=4k);
        map not_cached (dest=bus:sri, dest_offset=0xa0002000, reserved, size=4k);
    }

    memory pflash
    {
        mau = 8;
        size = 1M;
        type = rom;
        priority = 8;
        map     cached (dest=bus:sri, dest_offset=0x80200000,           size=1M);
        map not_cached (dest=bus:sri, dest_offset=0xa0200000, reserved, size=1M);
    }

    memory dflash
    {
        mau = 8;
        size = 256k+16k;
        type = reserved nvram;
        map (dest=bus:sri, dest_offset=0xaf080000, size=256k);
        map (dest=bus:sri, dest_offset=0xaf118000, size=16k);
    }

    memory lmuram
    {
        mau = 8;
        size = 8k;
        type = ram;
        map     cached (dest=bus:sri, dest_offset=0x90004000,           size=8k);
        map not_cached (dest=bus:sri, dest_offset=0xb0004000, reserved, size=8k);
    }

#elif (CPU_NUMID == 2)
    /* Begin of Section 8 divided by all the three cores for absolute 18 bit addressing */
    memory pflash_abs18
    {
        mau = 8;
        size = 4k;
        type = rom;
        map     cached (dest=bus:sri, dest_offset=0x80003000,           size=4k);
        map not_cached (dest=bus:sri, dest_offset=0xa0003000, reserved, size=4k);
    }

    memory pflash
    {
        mau = 8;
        size = 1M;
        type = rom;
        priority = 8;
        map     cached (dest=bus:sri, dest_offset=0x80300000,           size=1M);
        map not_cached (dest=bus:sri, dest_offset=0xa0300000, reserved, size=1M);
    }

    memory dflash
    {
        mau = 8;
        size = 256k+16k;
        type = reserved nvram;
        map (dest=bus:sri, dest_offset=0xaf0c0000, size=256k);
        map (dest=bus:sri, dest_offset=0xaf11c000, size=16k);
    }

    memory lmuram
    {
        mau = 8;
        size = 8k;
        type = ram;
        map     cached (dest=bus:sri, dest_offset=0x90006000,           size=8k);
        map not_cached (dest=bus:sri, dest_offset=0xb0006000, reserved, size=8k);
    }
#endif /* EE_MASTER_CPU */
#ifdef EE_MASTER_CPU
    section_layout :tc0:linear
    {
        // "data" group
        group (run_addr=mem:dspr0)
        {
            select (attributes=-x+w);
        }
    }

    section_layout :tc0:abs18
    {
        // "data" group
        group (run_addr=mem:dspr0)
        {
            select (attributes=-x+w);
        }

        // "const data" group
        group(run_addr=mem:pflash_abs18) {
            select (attributes=-w);
        }

    }
#endif /* EE_MASTER_CPU */
#if (CPU_NUMID == 1)
    section_layout :tc1:linear
    {
        // "data" group
        group (run_addr=mem:dspr1)
        {
            select (attributes=-x+w);
        }
    }

    section_layout :tc1:abs18
    {
        // "data" group
        group (run_addr=mem:dspr1)
        {
            select (attributes=-x+w);
        }

        // "const data" group
        group(run_addr=mem:pflash_abs18) {
            select (attributes=-w);
        }
    }
#endif /* CPU_NUMID == 1 */
#if (CPU_NUMID == 2)
    section_layout :tc2:linear
    {
        // "data" group
        group (run_addr=mem:dspr2)
        {
            select (attributes=-x+w);
        }
    }

    section_layout :tc2:abs18
    {
        // "data" group
        group (run_addr=mem:dspr2)
        {
            select (attributes=-x+w);
        }

        // "const data" group
        group(run_addr=mem:pflash_abs18) {
            select (attributes=-w);
        }
    }
#endif /* CPU_NUMID == 2 */
    section_layout :vtc:linear
    {
        group (ordered, run_addr=mem:pflash_startup)
        {
            select "ee_kernel_start";
        }

#ifdef EE_MASTER_CPU

        "EE_tc2Yx_cpu1_start" := EE_TC2YX_CPU1_START_ADDR;
        "EE_tc2Yx_cpu2_start" := EE_TC2YX_CPU2_START_ADDR;

        // "stacks" group
        group (run_addr=mem:dspr0)
        {
            select "*.ustack_tc0";
            select "*.istack_tc0";
        }

        // "shared data" group
        group ee_shared_data(run_addr=mem:lmuram/not_cached[0 .. 8k])
        {
            select "ee_mcglobald";
        }

        // "data" group
        group (run_addr=mem:dspr0 | mem:lmuram/cached)
        {
            select (attributes=-x+w);
        }
#endif /* EE_MASTER_CPU */

#if (CPU_NUMID == 1)

        // "stacks" group
        group (run_addr=mem:dspr1)
        {
            select "*.ustack_tc1";
            select "*.istack_tc1";
        }

        // "start-up code" groups
        group (ordered, run_addr=EE_TC2YX_CPU1_START_ADDR)
        {
            select "*.EE_tc2Yx_cpu1_start";
        }

        // "data" group
        group (run_addr=mem:dspr1 | mem:lmuram/cached)
        {
            select (attributes=-x+w);
        }
#endif /* CPU_NUMID == 1 */

#if (CPU_NUMID == 2)

        // "stacks" group
        group (run_addr=mem:dspr2)
        {
            select "*.ustack_tc2";
            select "*.istack_tc2";
        }

        // "start-up code" group
        group (ordered, run_addr=EE_TC2YX_CPU2_START_ADDR)
        {
            select "*.EE_tc2Yx_cpu2_start";
        }

        // "data" group
        group (run_addr=mem:dspr2 | mem:lmuram/cached)
        {
            select (attributes=-x+w);
        }
#endif /* CPU_NUMID == 2 */

        group(run_addr=mem:pflash) {
            select (attributes=-w);
        }
#ifndef EE_MASTER_CPU
#include "../shared_sym.lsl"
#endif /* !EE_MASTER_CPU */
    }

    section_layout :vtc:abs18 {
        group(run_addr=mem:pflash_abs18) {
            select (attributes=-w);
        }
    }

/* with  __REDEFINE_ON_CHIP_ITEMS and it must be included now */
#include "memdef_04_mcs.lsl"
} /* derivative ee_tc27x */

#else /* !EE_TC27X_LOCK_STEP */
/* TODO: Add Lock Step Support */
#endif /* !EE_SINGLE_CPU */

