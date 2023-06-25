OUTPUT_ARCH (mips)

/* Starts a segment */
#define BEGIN_SEG(name, addr) \
    _##name##SegmentStart = ADDR(.name); \
    _##name##SegmentRomStart = __romPos; \
    .name addr : AT(__romPos)

/* Ends a segment */
#define END_SEG(name) \
    _##name##SegmentEnd = ALIGN(ADDR(.name) + SIZEOF(.name),0x10); \
    _##name##SegmentRomEnd = _##name##SegmentEnd - _##name##SegmentStart + _##name##SegmentRomStart; \
    __romPos = _##name##SegmentRomEnd; \
    . = _##name##SegmentEnd;

/* Starts a noload segment which is intended to contain variables which do not have data yet. */
#define BEGIN_NOLOAD(name) \
    _##name##SegmentBssStart = ADDR(.name.noload); \
    .name.noload (NOLOAD) :

/* Ends a noload segment */
#define END_NOLOAD(name) \
    _##name##SegmentBssEnd = ALIGN(ADDR(.name.noload) + SIZEOF(.name.noload),0x10); \
    _##name##SegmentBssSize = _##name##SegmentBssEnd - _##name##SegmentBssStart; \
    . = _##name##SegmentBssEnd;

SECTIONS
{
   __romPos = 0;

   _RomStart = 0x1000;
   _RomSize = _RomStart;

   BEGIN_SEG(boot, 0x04000000)
   {
      BUILD_DIR/asm/rom_header.o(.text); /* ROM Header */
      BUILD_DIR/boot.6102.o(.data); /* CIC 6102 bootcode */
   }
   END_SEG(boot)

   _RomSize = (2147484752 - 0x80000400) + _RomStart;

   BEGIN_SEG(code, 0x80000400) SUBALIGN(16)
   {
      BUILD_DIR/asm/entry.o(.start);
      BUILD_DIR/src*.o(.text);

      */libultra_rom.a:*.o(.text);
      */libgcc.a:*.o(.text);

      /usr/lib/n64/PR/rspboot.o(.text);
      /usr/lib/n64/PR/gspL3DEX2.fifo.o(.text);
      /usr/lib/n64/PR/gspF3DEX2.NoN.fifo.o(.text);
      /usr/lib/n64/PR/aspMain.o(.text);

      /* data */
      BUILD_DIR/src*.o(.data*);

      */libultra_rom.a:*.o(.data*);
      
      /usr/lib/n64/PR/rspboot.o(.data*);
      /usr/lib/n64/PR/gspL3DEX2.fifo.o(.data*);
      /usr/lib/n64/PR/gspF3DEX2.NoN.fifo.o(.data*);
      /usr/lib/n64/PR/aspMain.o(.data*);

      /* rodata */
      BUILD_DIR/src*.o(.*rodata*);

      */libultra_rom.a:*.o(.*rodata*);
      */libgcc.a:*.o(.*rodata*);
   }
   END_SEG(code)
   
   _RomSize += (_codeSegmentEnd - _codeSegmentStart);
   
   BEGIN_NOLOAD(code)
   {
      BUILD_DIR/src*.o(.*bss*);

      */libultra_rom.a:*.o(COMMON);
      */libultra_rom.a:*.o(.scommon);
      */libultra_rom.a:*.o(.*bss*);
   }
   END_NOLOAD(code)

   BEGIN_NOLOAD(cfb) 
   { 
      BUILD_DIR/cfb.o(.bss*);
   } 
   END_NOLOAD(cfb) 

   BEGIN_NOLOAD(audio_heap) 
   { 
      BUILD_DIR/audio_heap.o(.bss*);
   } 
   END_NOLOAD(audio_heap) 

   BEGIN_NOLOAD(mem_heap) 
   { 
      BUILD_DIR/mem_heap.o(.bss*);
   } 
   END_NOLOAD(mem_heap) 

   BEGIN_SEG(doom64_wad,_RomSize) {
      "Data/DOOM64.WAD.o"
   }   
   END_SEG(doom64_wad)
   _RomSize += SIZEOF(.doom64_wad); 

   BEGIN_SEG(doom64_wmd,_RomSize) {
      "Data/DOOM64.WMD.o"
   }   
   END_SEG(doom64_wmd)
   _RomSize += SIZEOF(.doom64_wmd); 

   BEGIN_SEG(doom64_wsd,_RomSize) {
      "Data/DOOM64.WSD.o"
   }   
   END_SEG(doom64_wsd)
   _RomSize += SIZEOF(.doom64_wsd); 

   BEGIN_SEG(doom64_wdd,_RomSize) {
      "Data/DOOM64.WDD.o"
   }   
   END_SEG(doom64_wdd)

   /* Discard everything not specifically mentioned above. */
   /DISCARD/ :
   {
      *(*);
   }
}

