
/* Needed to redefine memories */
#define __BMHD0_CONFIG __BMHD_GENERATE
#define __BMHD1_CONFIG __BMHD_GENERATE
#define __REDEFINE_ON_CHIP_ITEMS

#include "tc27x_tc0.lsl"

// Specify a multi-core processor environment (mpe)
processor mpe
{
	derivative = ee_tc27x;
}

derivative ee_tc27x extends tc27x {
    memory dspr0 (tag="on-chip")
    {
        mau = 8;
		type = ram;
        size = 112k;
		priority = 2;
		map (dest=bus:tc0:fpi_bus, dest_offset=0xd0000000, size=112k, priority=8);
        map (dest=bus:sri, dest_offset=0x70000000, size=112k);
    }
    memory pspr0 (tag="on-chip")
    {
        mau = 8;
		type = ram;
        size = 24k;
		map (dest=bus:tc0:fpi_bus, dest_offset=0xc0000000, size=24k, priority=8);
        map (dest=bus:sri, dest_offset=0x70100000, size=24k);
    }
    memory dspr1 (tag="on-chip")
    {
        mau = 8;
        type = ram;
        size = 120k;
        map (dest=bus:sri, dest_offset=0x60000000, size=120k);
    }
    memory pspr1 (tag="on-chip")
    {
        mau = 8;
        type = ram;
        size = 32k;
        map (dest=bus:sri, dest_offset=0x60100000, size=32k);
    }
    memory dspr2 (tag="on-chip")
    {
        mau = 8;
        type = ram;
        size = 120k;
        map (dest=bus:sri, dest_offset=0x50000000, size=120k);
    }
    memory pspr2 (tag="on-chip")
    {
        mau = 8;
        type = ram;
        size = 32k;
        map (dest=bus:sri, dest_offset=0x50100000, size=32k);
    }
    memory pflash0 (tag="on-chip")
    {
        mau = 8;
        type = rom;
        size = 2M;
        map cached(dest=bus:sri, dest_offset=0x80000000, size=2M);
        map not_cached(dest=bus:sri, dest_offset=0xa0000000, size=2M, reserved);
    }
    memory pflash1 (tag="on-chip")
    {
        mau = 8;
        type = rom;
        size = 2M;
        map cached (dest=bus:sri, dest_offset=0x80200000, size=2M);
        map not_cached(dest=bus:sri, dest_offset=0xa0200000, size=2M, reserved);
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
        type = ram;
        size = 32k;
        priority = 2;
        map cached(dest=bus:sri, dest_offset=0x90000000, size=32k);
        map not_cached(dest=bus:sri, dest_offset=0xb0000000, size=32k, reserved);
    }

	/* with  __REDEFINE_ON_CHIP_ITEMS and it must be included now */
#include "memdef_04_mcs.lsl"
} /* derivative ee_tc27x */
