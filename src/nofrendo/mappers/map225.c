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
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
** Library General Public License for more details.  To obtain a 
** copy of the GNU Library General Public License, write to the Free 
** Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Any permitted reproduction of these routines, in whole or in part,
** must bear this legend.
**
**
** map225.c
**
** Mapper #225 (64 in 1)
**
*/

#include <noftypes.h>
#include <nes_mmc.h>
#include <nes.h>
#include <libsnss.h>
#include <log.h>

/************************/
/* Mapper #225: 64 in 1 */
/************************/
static void map225_init (void)
{
  /* On reset, PRG is set to first 32K and CHR to first 8K */
  mmc_bankrom (32, 0x8000, 0x00);
  mmc_bankvrom (8, 0x0000, 0x00);

  /* Done */
  return;
}

/*******************************************/
/* Mapper #225 write handler ($8000-$FFFF) */
/*******************************************/
static void map225_write (uint32 address, uint8 value)
{
  /* Value written is irrelevant */
  UNUSED (value);
  /* A5-A0 sets 8K CHR page */
  int vbank=(address&0x3f);
  if (address&(1<<14)) vbank|=0x40;
  mmc_bankvrom (8, 0x0000, vbank);

  if (address & (1<<12)) {
     //16K page.
     int bank=((address>>6)&0x3F);
     if (address&(1<<14)) bank|=0x40;
     mmc_bankrom(16, 0x8000, bank);
     mmc_bankrom(16, 0xC000, bank);
  } else {
    //32K page
     int bank=((address>>7)&0x1F);
     if (address&(1<<14)) bank|=0x20;
     mmc_bankrom(32, 0x8000, bank);
  }

  /* A5: mirroring (low = vertical, high = horizontal) */
  if (address & (1<<13)) {
     ppu_mirror(0, 0, 1, 1);
  } else {
     ppu_mirror(0, 1, 0, 1);
  }
  /* Done */
  return;
}

static map_memwrite map225_memwrite [] =
{
   { 0x8000, 0xFFFF, map225_write },
   {     -1,     -1, NULL }
};

mapintf_t map225_intf =
{
   225,                              /* Mapper number */
   "64 in 1 (bootleg)",              /* Mapper name */
   map225_init,                      /* Initialization routine */
   NULL,                             /* VBlank callback */
   NULL,                             /* HBlank callback */
   NULL,                             /* Get state (SNSS) */
   NULL,                             /* Set state (SNSS) */
   NULL,                             /* Memory read structure */
   map225_memwrite,                  /* Memory write structure */
   NULL                              /* External sound device */
};

//225 and 255 are the same, according to https://wiki.nesdev.com/w/index.php/INES_Mapper_255
mapintf_t map255_intf =
{
   255,                              /* Mapper number */
   "64 in 1 (bootleg) (alt)",        /* Mapper name */
   map225_init,                      /* Initialization routine */
   NULL,                             /* VBlank callback */
   NULL,                             /* HBlank callback */
   NULL,                             /* Get state (SNSS) */
   NULL,                             /* Set state (SNSS) */
   NULL,                             /* Memory read structure */
   map225_memwrite,                  /* Memory write structure */
   NULL                              /* External sound device */
};
