/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gc_proc_def_utils.h>
#include <gc_declare_proc.h>
#include <elf.h>
#include <stdio.h>
#include <string.h>

static const unsigned char expected[EI_PAD] = {
  ELFMAG0,
  ELFMAG1,
  ELFMAG2,
  ELFMAG3,
  ELFCLASS32,
  ELFDATA2MSB,
  EV_CURRENT,
  ELFOSABI_SYSV,
  0
};


/*
 * Basically we're going to find the GC_PROC_DEF_SECTION section
 * in the ELF file and return a pointer to it.  The only things in that
 * section are gc_proc_def's
 */
bool 
gcpd_find_table(spe_program_handle_t *handle,
		struct gc_proc_def **table, int *nentries, uint32_t *ls_addr)
{
  if (!handle || !table || !nentries)
    return false;

  *table = 0;
  *nentries = 0;
  
  Elf32_Ehdr *ehdr = (Elf32_Ehdr *)handle->elf_image;
  if (!ehdr){
    fprintf(stderr, "gcpd: No ELF image has been loaded\n");
    return false;
  }

  // quick check that we're looking at a SPE EXEC object

  if (memcmp(ehdr->e_ident, expected, EI_PAD) != 0){
    fprintf(stderr, "gcpd: invalid ELF header\n");
    return false;
  }

  if (ehdr->e_machine != 0x17){		// confirm machine type (EM_SPU)
    fprintf(stderr, "gcpd: not an SPE ELF object\n");
    return false;
  }

  if (ehdr->e_type != ET_EXEC){
    fprintf(stderr, "gcpd: invalid SPE ELF type.\n");
    fprintf(stderr, "gcpd: SPE type %d != %d\n", ehdr->e_type, ET_EXEC);
    return false;
  }

  // find the section header table

  Elf32_Shdr *shdr;
  Elf32_Shdr *sh;

  if (ehdr->e_shentsize != sizeof (*shdr)){
    fprintf(stderr, "gcpd: invalid section header format.\n");
    return false;
  }

  if (ehdr->e_shnum == 0){
    fprintf(stderr, "gcpd: no section headers in file.\n");
    return false;
  }

  shdr = (Elf32_Shdr *) ((char *)ehdr + ehdr->e_shoff);
  char *str_table = (char *)ehdr + shdr[ehdr->e_shstrndx].sh_offset;

  // traverse the sections looking for GC_PROC_DEF_SECTION
  
  for (sh = shdr; sh < &shdr[ehdr->e_shnum]; sh++){
    if (0){
      fprintf(stderr, "section name: %s (start: 0x%04x, size: 0x%04x)\n",
	      str_table + sh->sh_name, sh->sh_offset, sh->sh_size);
    }

    if (strcmp(GC_PROC_DEF_SECTION, str_table+sh->sh_name) == 0){
      *table = (struct gc_proc_def *)((char *)ehdr + sh->sh_offset);
      if (sh->sh_size % (sizeof(struct gc_proc_def)) != 0){
	fprintf(stderr, "gcpd: %s section has invalid format\n", GC_PROC_DEF_SECTION);
	return false;
      }
      *nentries = sh->sh_size / sizeof(struct gc_proc_def);
      *ls_addr = sh->sh_addr;
      return true;
    }
  }

  return false;
}
