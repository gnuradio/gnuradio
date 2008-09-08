OUTPUT_FORMAT("elf32-microblaze", "", "")
/* SEARCH_DIR(/home/jwilliams/tmp/microblaze-toolchain-sources/release/lin/microblaze//lib);*/

ENTRY(_start)

MEMORY {
	flatmem : ORIGIN = 0x0, LENGTH = 0x00008000	/* 32KB */
}

SECTIONS
{
  _TEXT_START_ADDR = DEFINED(_TEXT_START_ADDR) ? _TEXT_START_ADDR : 0x50;		
  .vectors.reset 0x0 : { *(.vectors.reset) } > flatmem
  .vectors.sw_exception 0x8 : { *(.vectors.sw_exception) } > flatmem
  .vectors.interrupt 0x10 : { *(.vectors.interrupt) } > flatmem
  .vectors.hw_exception 0x20 : { *(.vectors.hw_exception) } >flatmem
  . = _TEXT_START_ADDR;
   _ftext  =  .;
  .text : {
    *(.text)
    *(.text.*)
    *(.gnu.linkonce.t.*)
  } > flatmem
   _etext  =  .;
  .init : { KEEP (*(.init))	} > flatmem =0
  .fini : { KEEP (*(.fini))	} > flatmem =0
  PROVIDE (__CTOR_LIST__ = .);
  PROVIDE (___CTOR_LIST__ = .);
  .ctors   : 
  {
    /* gcc uses crtbegin.o to find the start of
       the constructors, so we make sure it is
       first.  Because this is a wildcard, it
       doesn't matter if the user does not
       actually link against crtbegin.o; the
       linker won't look for a file to match a
       wildcard.  The wildcard also means that it
       doesn't matter which directory crtbegin.o
       is in.  */
    KEEP (*crtbegin.o(.ctors))
    /* We don't want to include the .ctor section from
       from the crtend.o file until after the sorted ctors.
       The .ctor section from the crtend file contains the
       end of ctors marker and it must be last */
    KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
  } > flatmem
  PROVIDE (__CTOR_END__ = .);
  PROVIDE (___CTOR_END__ = .);
  PROVIDE (__DTOR_LIST__ = .);
  PROVIDE (___DTOR_LIST__ = .);
   .dtors         :
  {
    KEEP (*crtbegin.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
  } > flatmem
  PROVIDE (__DTOR_END__ = .);
  PROVIDE (___DTOR_END__ = .);
   . = ALIGN(4);
    _frodata = . ;
  .rodata : {
    *(.rodata)
    *(.gnu.linkonce.r.*)
    CONSTRUCTORS; /* Is this needed? */
  } > flatmem
   _erodata = .;
  /* Alignments by 8 to ensure that _SDA2_BASE_ on a word boundary */
  /* Note that .sdata2 and .sbss2 must be contiguous */
  . = ALIGN(8);
   _ssrw = .;
  .sdata2 : {
    *(.sdata2)
    *(.gnu.linkonce.s2.*)
  } > flatmem
  . = ALIGN(4);
  .sbss2 : {
    PROVIDE (__sbss2_start = .);	
    *(.sbss2)
    *(.gnu.linkonce.sb2.*)
    PROVIDE (__sbss2_end = .);
  } > flatmem
  . = ALIGN(8);
   _essrw = .;
   _ssrw_size = _essrw - _ssrw;
   PROVIDE (_SDA2_BASE_ = _ssrw + (_ssrw_size / 2 ));
   . = ALIGN(4);
   _fdata = .;
  .data : {
    *(.data)
    *(.gnu.linkonce.d.*)
    CONSTRUCTORS; /* Is this needed? */
  } > flatmem
   _edata = . ;
   /* Added to handle pic code */
  .got : {  
    *(.got)
  } > flatmem
  .got1 : {  
    *(.got1)
  } > flatmem
  .got2 : {  
    *(.got2)
  } > flatmem
  /* Added by Sathya to handle C++ exceptions */
  .eh_frame : {  
    *(.eh_frame)
  } > flatmem
  .jcr : {  
    *(.jcr)
  } > flatmem
  .gcc_except_table : {  
    *(.gcc_except_table)
  } > flatmem
  /* Alignments by 8 to ensure that _SDA_BASE_ on a word boundary */
  /* Note that .sdata and .sbss must be contiguous */
  . = ALIGN(8);
   _ssro = .;
  .sdata : {
    *(.sdata)
    *(.gnu.linkonce.s.*)
  } > flatmem
  . = ALIGN(4);
  .sbss : {
    PROVIDE (__sbss_start = .);	
    *(.sbss)
    *(.gnu.linkonce.sb.*)
    PROVIDE (__sbss_end = .);
  } > flatmem
  . = ALIGN(8);
   _essro = .;
   _ssro_size = _essro - _ssro;
  PROVIDE (_SDA_BASE_ = _ssro + (_ssro_size / 2 ));
   . = ALIGN(4);
   _fbss = .;
  .bss : {
    PROVIDE (__bss_start = .);	
    *(.bss)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    . = ALIGN(4);  
    PROVIDE (__bss_end = .);
     _heap = .;
     _HEAP_SIZE = DEFINED(_HEAP_SIZE) ? _HEAP_SIZE : 0x0;
     _STACK_SIZE = DEFINED(_STACK_SIZE) ? _STACK_SIZE : 0x800;
     . += _HEAP_SIZE;
     _heap_end = .;
     . += _STACK_SIZE;
     . = ALIGN(8);
     _stack = .;
     _end = .;
  } > flatmem
  .tdata : {  
    *(.tdata)
    *(.gnu.linkonce.td.*)
  } > flatmem
  .tbss : {  
    *(.tbss)
    *(.gnu.linkonce.tb.*)
  } > flatmem
}
