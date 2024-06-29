#define	SCREEN_HT	240
#define	SCREEN_WD	320
#define MEM_SIZE() osMemSize
#define CFB_SIZE SCREEN_WD*SCREEN_HT*2*sizeof(u32)

#define BASE_RAM_ADDR ((byte*)K0BASE)
#define BASE_RAM_END() (BASE_RAM_ADDR + MEM_SIZE())

#define CFB0_ADDR() ((byte*)(BASE_RAM_ADDR + (MEM_SIZE()>>1)))
#define CFB1_ADDR() ((byte*)(BASE_RAM_END() - CFB_SIZE))
#define CFB(i) ((i) ? CFB1_ADDR() : CFB0_ADDR())