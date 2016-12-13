// TASKING VX-toolset for TriCore
// Eclipse project linker script file
// 

#ifndef CPU_NUMID
#define EE_SINGLE_CPU
#elif (CPU_NUMID == 0)
#define EE_MASTER_CPU
#endif

#define INTTAB0                   0x70100000
#define TRAPTAB0                  0x70102000
#define RESET                     0x70102100
#define EE_TC2YX_START_ADDR       0x70102104

#define INTTAB1                   0x60100000
#define TRAPTAB1                  0x60102000
#define EE_TC2YX_CPU1_START_ADDR  0x60102100

#define INTTAB2                   0x50100000
#define TRAPTAB2                  0x50102000
#define EE_TC2YX_CPU2_START_ADDR  0x50102100


/* Needed to redefine memories */
#define  __REDEFINE_ON_CHIP_ITEMS

#ifdef EE_SINGLE_CPU
#include "tc27x_singlecore.lsl"
#else /* EE_SINGLE_CPU */
#define __BMHD0_CONFIG __BMHD_GENERATE
#define __BMHD1_CONFIG __BMHD_GENERATE
#include "tc27x.lsl"
#endif /* EE_SINGLE_CPU */

// Specify a multi-core processor environment (mpe)
processor mpe
{
    derivative = ee_tc27x;
}

derivative ee_tc27x extends tc27x {

    memory dspr0_as_ROM // Data Scratch Pad Ram (That simulate ROM)
    {
      mau = 8;
      size = 12k;
      type = rom; // Workaround for TASKING behaviour, it Should be RAM
      map (dest=bus:tc0:fpi_bus, dest_offset=0xd0000000, size=12k, priority=8);
      map (dest=bus:sri, dest_offset=0x70000000, size=12k);
    }

    memory dspr0 // Data Scratch Pad Ram
    {
      mau = 8;
      size = 100k;
      type = ram;
      priority = 2;
      //map (dest=bus:tc0:fpi_bus, dest_offset=0xd0000000, size=112k, priority=8);
      //map (dest=bus:sri, dest_offset=0x70000000, size=112k);
      map (dest=bus:tc0:fpi_bus, dest_offset=0xd0003000, size=100k, priority=8);
      map (dest=bus:sri, dest_offset=0x70003000, size=100k);
    }

    memory pspr0 // Program Scratch Pad Ram (faked as ROM)
    {
      mau = 8;
      size = 24k;
      type = rom; // Workaround for TASKING behaviour, it Should be RAM
      map (dest=bus:tc0:fpi_bus, dest_offset=0xc0000000, size=24k, priority=8);
      map (dest=bus:sri, dest_offset=0x70100000, size=24k);
    }

    memory dspr1_as_ROM // Data Scratch Pad Ram (That simulate ROM)
    {
      mau = 8;
      size = 12k;
      type = rom; // Workaround for TASKING behaviour, it Should be RAM
#ifndef EE_SINGLE_CPU
      map (dest=bus:tc1:fpi_bus, dest_offset=0xd0000000, size=12k, priority=8);
#endif /* !EE_SINGLE_CPU */
      map (dest=bus:sri, dest_offset=0x60000000, size=12k);
    }

    memory dspr1 // Data Scratch Pad Ram
    {
      mau = 8;
      size = 108k;
      type = ram;
      priority = 2;
#ifndef EE_SINGLE_CPU
      map (dest=bus:tc1:fpi_bus, dest_offset=0xd0003000, size=108k, priority=8);
#endif /* !EE_SINGLE_CPU */
      map (dest=bus:sri, dest_offset=0x60003000, size=108k);
    }

    memory pspr1 // Program Scratch Pad Ram
    {
      mau = 8;
      size = 24k;
      type = rom; // Workaround for TASKING behaviour, it Should be RAM
#ifndef EE_SINGLE_CPU
      map (dest=bus:tc1:fpi_bus, dest_offset=0xc0000000, size=24k, priority=8);
#endif /* !EE_SINGLE_CPU */
      map (dest=bus:sri, dest_offset=0x60100000, size=24k);
    }

    memory dspr2_as_ROM // Data Scratch Pad Ram (That simulate ROM)
    {
      mau = 8;
      size = 12k;
      type = rom; // Workaround for TASKING behaviour, it Should be RAM
#ifndef EE_SINGLE_CPU
      map (dest=bus:tc2:fpi_bus, dest_offset=0xd0000000, size=12k, priority=8);
#endif /* !EE_SINGLE_CPU */
      map (dest=bus:sri, dest_offset=0x50000000, size=12k);
    }

    memory dspr2 // Data Scratch Pad Ram
    {
      mau = 8;
      size = 108k;
      type = ram;
      priority = 2;
#ifndef EE_SINGLE_CPU
      map (dest=bus:tc2:fpi_bus, dest_offset=0xd0003000, size=108k, priority=8);
#endif /* !EE_SINGLE_CPU */
      map (dest=bus:sri, dest_offset=0x50003000, size=108k);
    }

    memory pspr2 // Program Scratch Pad Ram
    {
      mau = 8;
      size = 24k;
      type = rom; // Workaround for TASKING behaviour, it Should be RAM
#ifndef EE_SINGLE_CPU
      map (dest=bus:tc2:fpi_bus, dest_offset=0xc0000000, size=24k, priority=8);
#endif /* !EE_SINGLE_CPU */
      map (dest=bus:sri, dest_offset=0x50100000, size=24k);
    }
#ifdef EE_SINGLE_CPU
    memory lmuram
    {
      mau = 8;
      size = 32k;
      type = ram;
      map     cached (dest=bus:sri, dest_offset=0x90000000,           size=32k);
      map not_cached (dest=bus:sri, dest_offset=0xb0000000, reserved, size=32k);
    }
#elif defined(EE_MASTER_CPU)
    memory lmuram
    {
        mau = 8;
        size = 16k;
        type = ram;
        map     cached (dest=bus:sri, dest_offset=0x90000000,           size=16k);
        map not_cached (dest=bus:sri, dest_offset=0xb0000000, reserved, size=16k);
    }
#elif (CPU_NUMID == 1)
    memory lmuram
    {
        mau = 8;
        size = 8k;
        type = ram;
        map     cached (dest=bus:sri, dest_offset=0x90004000,           size=8k);
        map not_cached (dest=bus:sri, dest_offset=0xb0004000, reserved, size=8k);
    }
#elif (CPU_NUMID == 2)
    memory lmuram
    {
      mau = 8;
      size = 8k;
      type = ram;
      map     cached (dest=bus:sri, dest_offset=0x90006000,           size=8k);
      map not_cached (dest=bus:sri, dest_offset=0xb0006000, reserved, size=8k);
    }
#endif /* EE_SINGLE_CPU */

    section_layout :vtc:linear
    {
#ifndef EE_SINGLE_CPU
    group (ordered, run_addr=EE_TC2YX_START_ADDR)
      {
        select "ee_kernel_start";
      }
#endif /* !EE_SINGLE_CPU */
#ifdef EE_MASTER_CPU
      "EE_tc2Yx_cpu1_start" := EE_TC2YX_CPU1_START_ADDR;
      "EE_tc2Yx_cpu2_start" := EE_TC2YX_CPU2_START_ADDR;

      // "shared data" group
      group ee_shared_data(run_addr=mem:lmuram/not_cached[0 .. 8k])
      {
        select "ee_mcglobald";
      }
#endif /* EE_MASTER_CPU */
#if defined(EE_SINGLE_CPU) || defined(EE_MASTER_CPU)
      // "data" group
      group (run_addr=mem:dspr0 | mem:lmuram)
      {
        select (attributes=-x+w);
      }

      // "const data" group
      group (run_addr=mem:dspr0_as_ROM)
      {
        select (attributes=-x-w);
      }

      // "code" group
      group (run_addr=mem:pspr0)
      {
        select (attributes=+x);
      }
#endif /* EE_SINGLE_CPU || EE_MASTER_CPU */
#if (CPU_NUMID == 1)
      // "start-up code" groups
      group (ordered, run_addr=EE_TC2YX_CPU1_START_ADDR)
      {
        select "*.EE_tc2Yx_cpu1_start";
      }
      // "data" group
      group (run_addr=mem:dspr1 | mem:lmuram)
      {
        select (attributes=-x+w);
      }

      // "const data" group
      group (run_addr=mem:dspr1_as_ROM)
      {
        select (attributes=-x-w);
      }

      // "code" group
      group (run_addr=mem:pspr1)
      {
        select (attributes=+x);
      }
#endif /* CPU_NUMID == 1 */
#if (CPU_NUMID == 2)
      // "start-up code" group
      group (ordered, run_addr=EE_TC2YX_CPU2_START_ADDR)
      {
        select "*.EE_tc2Yx_cpu2_start";
      }
      // "data" group
      group (run_addr=mem:dspr2 | mem:lmuram)
      {
        select (attributes=-x+w);
      }

      // "const data" group
      group (run_addr=mem:dspr2_as_ROM)
      {
        select (attributes=-x-w);
      }

      // "code" group
      group (run_addr=mem:pspr2)
      {
        select (attributes=+x);
      }
#endif /* CPU_NUMID == 2 */
#if (!defined(EE_SINGLE_CPU)) && (!defined(EE_MASTER_CPU))
#include "../shared_sym.lsl"
#endif /* !EE_SINGLE_CPU && !EE_MASTER_CPU */
    }

    section_layout :vtc:abs18
    {
#if defined(EE_SINGLE_CPU) || defined(EE_MASTER_CPU)
        // "data" group
        group (run_addr=mem:dspr0)
        {
          select (attributes=+w);
        }

        // "const data" group
        group (run_addr=mem:dspr0_as_ROM)
        {
          select (attributes=-w);
        }
#endif /* EE_SINGLE_CPU || EE_MASTER_CPU */
#if (CPU_NUMID == 1)
        // "data" group
        group (run_addr=mem:dspr1)
        {
          select (attributes=+w);
        }

        // "const data" group
        group (run_addr=mem:dspr1_as_ROM)
        {
          select (attributes=-w);
        }
#endif /* CPU_NUMID == 1 */
#if (CPU_NUMID == 2)
        // "data" group
        group (run_addr=mem:dspr2)
        {
          select (attributes=+w);
        }

        // "const data" group
        group (run_addr=mem:dspr2_as_ROM)
        {
          select (attributes=-w);
        }
#endif /* CPU_NUMID == 2 */
    }

#if defined(EE_SINGLE_CPU) || defined(EE_MASTER_CPU)
    section_layout :tc0:linear
    {
        // "data" group
        group (run_addr=mem:dspr0)
        {
          select (attributes=-x+w);
        }

        // "const data" group
        group (run_addr=mem:dspr0_as_ROM)
        {
          select (attributes=-x-w);
        }

        // "code" group
        group (run_addr=mem:pspr0)
        {
          select (attributes=+x);
        }
    }

    section_layout :tc0:abs18
    {
        // "data" group
        group (run_addr=mem:dspr0)
        {
          select (attributes=+w);
        }

        // "const data" group
        group (run_addr=mem:dspr0_as_ROM)
        {
          select (attributes=-w);
        }
    }
#endif /* EE_SINGLE_CPU || EE_MASTER_CPU */
#if (CPU_NUMID == 1)
    section_layout :tc1:linear
    {
        // "data" group
        group (run_addr=mem:dspr1)
        {
          select (attributes=-x+w);
        }

        // "const data" group
        group (run_addr=mem:dspr1_as_ROM)
        {
          select (attributes=-x-w);
        }

        // "code" group
        group (run_addr=mem:pspr1)
        {
          select (attributes=+x);
        }
    }

    section_layout :tc1:abs18
    {
        // "data" group
        group (run_addr=mem:dspr1)
        {
          select (attributes=+w);
        }

        // "const data" group
        group (run_addr=mem:dspr1_as_ROM)
        {
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

        // "const data" group
        group (run_addr=mem:dspr2_as_ROM)
        {
          select (attributes=-x-w);
        }

        // "code" group
        group (run_addr=mem:pspr2)
        {
          select (attributes=+x);
        }
    }

    section_layout :tc2:abs18
    {
        // "data" group
        group (run_addr=mem:dspr2)
        {
          select (attributes=+w);
        }

        // "const data" group
        group (run_addr=mem:dspr2_as_ROM)
        {
          select (attributes=-w);
        }
    }
#endif /* CPU_NUMID == 2 */
} /* derivative ee_tc27x */

