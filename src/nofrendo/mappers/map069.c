/*
** Nofrendo (c) 1998-2000 Matthew Conte (matt@conte.com)
**
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of version 2 of the GNU Library General 
** Public License as published by the Free Software Foundation.
**
** This program is distributed in the hope that it will be useful, 
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the GNU 
** Library General Public License for more details.  To obtain a 
** copy of the GNU Library General Public License, write to the Free 
** Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Any permitted reproduction of these routines, in whole or in part,
** must bear this legend.
**
**
** map69.c
**
** mapper 69 interface
*/

#include <noftypes.h>
#include <nes_mmc.h>
#include <nes.h>
#include <log.h>

static struct
{
	int counter;
	bool ctr_enabled;
	bool enabled;
	int lastcmd;
} irq;

/* mapper 69: Sunsoft FME-7/5A/5B */
static void map69_write(uint32 address, uint8 value)
{
	if ((address&0xE000)==0x8000) {
		 irq.lastcmd=value&0xf;
	} else if ((address&0xE000)==0xA000) {
		int cmd=irq.lastcmd;
		if (cmd<=7) {
			mmc_bankvrom(1, 0x0400*cmd, value);
		} else if (cmd==8) {
			if (value&0x40) {
				//select ram
				mmc_bankram(8, 0x6000, value&0x3F);
			} else {
				mmc_bankrom(8, 0x6000, value&0x3F);
			}
		} else if (cmd>=9 && cmd<=0xB) {
			mmc_bankrom(8, 0x8000+0x2000*(cmd-9), value&0x3F);
		} else if (cmd==0xC) {
			value&=3;
			if (value==0) ppu_mirror(0, 1, 0, 1); //vertical
			if (value==1) ppu_mirror(0, 0, 1, 1); //horiz
			if (value==2) ppu_mirror(0, 0, 0, 0); //NT 0 (1ScA)
			if (value==3) ppu_mirror(1, 1, 1, 1); //NT 1 (1ScB)
		} else if (cmd==0xd) {
			irq.enabled=value&1;
			irq.ctr_enabled=value&0x80;
		} else if (cmd==0xe) {
			irq.counter=(irq.counter&0xff00)|value;
		} else if (cmd==0xf) {
			irq.counter=(irq.counter&0xff)|(value<<8);
		}
	}
}

static void map69_hblank(int vblank) {
	UNUSED(vblank);

	if (irq.ctr_enabled) {
		irq.counter-=113; //cycles/hbl, actually 113.66
		if (irq.counter < 0) {
			if (irq.enabled) nes_irq();
			irq.counter+=0x10000;
		}
	}
}

static map_memwrite map69_memwrite[] = {
	{ 0x8000, 0xFFFF, map69_write },
	{		-1,	  -1, NULL }
};

static void map69_init(void)
{
	mmc_bankrom(8, 0x6000, 0);
	mmc_bankrom(8, 0x8000, 1);
	mmc_bankrom(8, 0xA000, 2);
	mmc_bankrom(8, 0xC000, MMC_LASTBANK);
	mmc_bankrom(8, 0xE000, MMC_LASTBANK);
	mmc_bankvrom(8, 0x0000, 0);

	irq.counter =  0;
	irq.enabled = false;
	irq.ctr_enabled = false;
}

mapintf_t map69_intf = 
{
	69, /* mapper number */
	"Sunsoft FME-7", /* mapper name */
	map69_init, /* init routine */
	NULL, /* vblank callback */
	map69_hblank, /* hblank callback */
	NULL, /* get state (snss) */
	NULL, /* set state (snss) */
	NULL, /* memory read structure */
	map69_memwrite, /* memory write structure */
	NULL
};
