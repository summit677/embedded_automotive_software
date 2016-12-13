
#ifndef CPU_NUMID
#define EE_SINGLE_CPU
#elif (CPU_NUMID == 0)
#define EE_MASTER_CPU
#endif

/* Relocate CPU1 & CPU2 INTTAB and TRAPTAB */
#if (CPU_NUMID == 1)
#define INTTAB  0x80200000
#elif (CPU_NUMID == 2)
#define INTTAB  0x80300000
#endif

/* Needed to redefine memories */
#define __BMHD0_CONFIG __BMHD_GENERATE
#define __BMHD1_CONFIG __BMHD_GENERATE
#define __REDEFINE_ON_CHIP_ITEMS

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
	memory dspr0 (tag="on-chip")
	{
		mau = 8;
		type = reserved ram;
		size = 112k;
		priority = 2;
		map (dest=bus:tc0:fpi_bus, dest_offset=0xd0000000, size=112k, priority=8);
		map (dest=bus:sri, dest_offset=0x70000000, size=112k);
	}
	memory dspr1 (tag="on-chip")
	{
		mau = 8;
		type = reserved ram;
		size = 120k;
		priority = 2;
		map (dest=bus:tc1:fpi_bus, dest_offset=0xd0000000, size=120k, priority=8);
		map (dest=bus:sri, dest_offset=0x60000000, size=120k);
	}
	memory dspr2 (tag="on-chip")
	{
		mau = 8;
		type = reserved ram;
		size = 120k;
		priority = 2;
		map (dest=bus:tc2:fpi_bus, dest_offset=0xd0000000, size=120k, priority=8);
		map (dest=bus:sri, dest_offset=0x50000000, size=120k);
	}
	memory pspr0 (tag="on-chip")
	{
		mau = 8;
		type = reserved rom;
		size = 24k;
		map (dest=bus:tc0:fpi_bus, dest_offset=0xc0000000, size=24k, priority=8);
		map (dest=bus:sri, dest_offset=0x70100000, size=24k);
	}
	memory pspr1 (tag="on-chip")
	{
		mau = 8;
		type = reserved rom;
		size = 32k;
		map (dest=bus:tc1:fpi_bus, dest_offset=0xc0000000, size=32k, priority=8);
		map (dest=bus:sri, dest_offset=0x60100000, size=32k);
	}
	memory pspr2 (tag="on-chip")
	{
		mau = 8;
		type = reserved rom;
		size = 32k;
		map (dest=bus:tc2:fpi_bus, dest_offset=0xc0000000, size=32k, priority=8);
		map (dest=bus:sri, dest_offset=0x50100000, size=32k);
	}
	memory pflash_startup (tag="on-chip")
	{
		mau = 8;
		type = reserved rom;
		size = 1k;
		map cached (dest=bus:sri, dest_offset=0x80000000, size=1k);
		map not_cached (dest=bus:sri, dest_offset=0xa0000000, size=1k, reserved);
	}
	memory pflash_abs18_tc0 (tag="on-chip")
	{
		mau = 8;
		type = reserved rom;
		size = 7k;
		map cached (dest=bus:sri, dest_offset=0x80000400, size=7k);
		map not_cached (dest=bus:sri, dest_offset=0xa0000400, size=7k, reserved);
	}
	memory pflash_abs18_tc1 (tag="on-chip")
	{
		mau = 8;
		type = reserved rom;
		size = 4k;
		map cached (dest=bus:sri, dest_offset=0x80002000, size=4k);
		map not_cached (dest=bus:sri, dest_offset=0xa0002000, size=4k, reserved);
	}
	memory pflash_abs18_tc2 (tag="on-chip")
	{
		mau = 8;
		type = reserved rom;
		size = 4k;
		map cached (dest=bus:sri, dest_offset=0x80003000, size=4k);
		map not_cached (dest=bus:sri, dest_offset=0xa0003000, size=4k, reserved);
	}
	memory pflash0_tc0 (tag="on-chip")
	{
		mau = 8;
		type = reserved rom;
		size = 1984k;
		priority = 8;
		map cached(dest=bus:sri, dest_offset=0x80004000, size=1984k);
		map not_cached(dest=bus:sri, dest_offset=0xa0004000, size=1984k, reserved);
	}
	memory pflash1_tc1 (tag="on-chip")
	{
		mau = 8;
		type = reserved rom;
		size = 1M;
		priority = 8;
		map cached (dest=bus:sri, dest_offset=0x80200000, size=1M);
		map not_cached(dest=bus:sri, dest_offset=0xa0200000, size=1M, reserved);
	}
	memory pflash1_tc2 (tag="on-chip")
	{
		mau = 8;
		type = reserved rom;
		size = 1M;
		priority = 8;
		map cached(dest=bus:sri, dest_offset=0x80300000, size=1M);
		map not_cached(dest=bus:sri, dest_offset=0xa0300000, size=1M, reserved);
	}
	memory brom (tag="on-chip")
	{
		mau = 8;
		type = reserved rom;
		size = 32k;
		map cached(dest=bus:sri, dest_offset=0x8fff8000, size=32k);
		map not_cached(dest=bus:sri, dest_offset=0xafff8000, size=32k, reserved);
	}
	memory dflash0 (tag="on-chip")
	{
		mau = 8;
		type = reserved nvram;
		size = 384k+16k;
		map (dest=bus:sri, src_offset=0, dest_offset=0xaf000000, size=384k);
		map (dest=bus:sri, src_offset=384k, dest_offset=0xaf100000, size=16k);
	}
	memory dflash1 (tag="on-chip")
	{
		mau = 8;
		type = reserved nvram;
		size = 64k;
		map (dest=bus:sri, dest_offset=0xaf110000, size=64k);
	}
	memory lmuram (tag="on-chip")
	{
		mau = 8;
		type = reserved ram;
		size = 32k;
		priority = 2;
		map cached(dest=bus:sri, dest_offset=0x90000000, size=32k);
		map not_cached(dest=bus:sri, dest_offset=0xb0000000, size=32k, reserved);
	}

#ifdef EE_MASTER_CPU

	section_layout :tc0:linear {
		// "data" group
		group (run_addr=mem:dspr0) {
			select (attributes=-x+w);
		}
		/* "const data" group*/
		group(run_addr=mem:pflash_abs18_tc0) {
			select (attributes=-w);
		}
	}
	section_layout :tc0:abs18 {
		/* "data" group*/
		group (run_addr=mem:dspr0) {
			select (attributes=-x+w);
		}
		/* "const data" group*/
		group(run_addr=mem:pflash_abs18_tc0) {
			select (attributes=-w);
		}
	}
	section_layout :vtc:abs18 {
		group(run_addr=mem:pflash_abs18_tc0) {
			select (attributes=-w);
		}
		/* "data" group */
		group (run_addr=mem:dspr0) {
			select (attributes=-x+w);
		}
	}
#endif /* EE_MASTER_CPU */

#if (CPU_NUMID == 1)
	section_layout :tc1:linear {
		// "data" group
		group (run_addr=mem:dspr1) {
			select (attributes=-x+w);
		}
		// "const data" group
		group(run_addr=mem:pflash_abs18_tc1) {
			select (attributes=-w);
		}
	}
	section_layout :tc1:abs18 {
		/* "data" group */
		group (run_addr=mem:dspr1) {
			select (attributes=-x+w);
		}
		/* "const data" group */
		group(run_addr=mem:pflash_abs18_tc1) {
			select (attributes=-w);
		}
	}
	section_layout :vtc:abs18 {
		group(run_addr=mem:pflash_abs18_tc1) {
			select (attributes=-w);
		}
		/* "data" group */
		group (run_addr=mem:dspr1) {
			select (attributes=-x+w);
		}
	}
#endif /* CPU_NUMID == 1 */

#if (CPU_NUMID == 2)
	section_layout :tc2:linear {
		/* "data" group */
		group (run_addr=mem:dspr2) {
			select (attributes=-x+w);
		}
		/* "const data" group */
		group(run_addr=mem:pflash_abs18_tc2) {
			select (attributes=-w);
		}
	}
	section_layout :tc2:abs18 {
		/* "data" group */
		group (run_addr=mem:dspr2) {
			select (attributes=-x+w);
		}
		/* "const data" group */
		group(run_addr=mem:pflash_abs18_tc2) {
			select (attributes=-w);
		}
	}
	section_layout :vtc:abs18 {
		group(run_addr=mem:pflash_abs18_tc2) {
			select (attributes=-w);
		}
		/* "data" group */
		group (run_addr=mem:dspr2) {
			select (attributes=-x+w);
		}
	}
#endif /* CPU_NUMID == 2 */

	section_layout :vtc:linear {

		group (ordered, run_addr=mem:pflash_startup) {
			select "ee_kernel_start";
		}
#ifdef EE_MASTER_CPU
		// "stacks" group
		group (run_addr=mem:dspr0) {
			select "*.ustack_tc0";
			select "*.istack_tc0";
		}

		group ee_kernel_stack (run_addr=mem:dspr0) {
			select "ee_kernel_stack";
		}

		/* "shared data" group */
		group ee_shared_data(run_addr=mem:lmuram/not_cached[0 .. 8k]) {
			select "ee_mcglobald";
		}

		/* "data" group */
		group (run_addr=mem:dspr0) {
			select (attributes=-x+w);
		}
		group (ordered, contiguous, align = 8, run_addr=mem:pflash0_tc0) {
			section "ee_mcglobalt"(size=1k, attributes=r, fill=0) {
				select "ee_mcglobalt";
				select "*.ErrorHook";
				select "*.StartupHook";
				select "*.ShutdownHook";
				//select "*.PreTaskHook";
				//select "*.PostTaskHook";
				select "*.ProtectionHook";
			}
			select (attributes=-w);
		}
		group(run_addr=mem:pflash0_tc0) {
			select (attributes=-w);
		}
#endif /* EE_MASTER_CPU */
		"EE_tc2Yx_cpu1_start" := EE_TC2YX_CPU1_START_ADDR;
		"EE_tc2Yx_cpu2_start" := EE_TC2YX_CPU2_START_ADDR;
//"ee_stacks_table" := "ee_kernel_stack";

#if (CPU_NUMID == 1)
		/* "stacks" group */
		group (run_addr=mem:dspr1) {
			select "*.ustack_tc1";
			select "*.istack_tc1";
			select (attributes=-x+w);/* "data" group */

		}
		/* "start-up code" groups */
		group (ordered, run_addr=EE_TC2YX_CPU1_START_ADDR) {
			select "EE_tc2Yx_cpu1_start";
		}

		group(run_addr=mem:pflash1_tc1) {
			select (attributes=-w);
		}
#endif /* CPU_NUMID == 1 */
#if (CPU_NUMID == 2)
		// "stacks" group
		group (run_addr=mem:dspr2) {
			select "*.ustack_tc2";
			select "*.istack_tc2";
		}
		// "start-up code" group
		group (ordered, run_addr=EE_TC2YX_CPU2_START_ADDR) {
			select "EE_tc2Yx_cpu2_start";
		}
		// "data" group
		group (run_addr=mem:dspr2)
		{
			select (attributes=-x+w);
		}
		group(run_addr=mem:pflash1_tc2) {
			select (attributes=-w);
		}
#endif /* CPU_NUMID == 2 */
#ifndef EE_MASTER_CPU
#include "../shared_sym.lsl"
#endif /* !EE_MASTER_CPU */
	}

	/* with  __REDEFINE_ON_CHIP_ITEMS and it must be included now */
#include "memdef_04_mcs.lsl"
} /* derivative ee_tc27x */

