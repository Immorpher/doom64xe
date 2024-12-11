/*
 * ROM header detailed here https://n64brew.dev/wiki/ROM_Header
 * Only the first 0x18 bytes matter to the console.
 */

.byte  0x80, 0x37, 0x12, 0x40   /* PI BSD Domain 1 register - offset 0x00 */
.word  0x0000000F               /* Clockrate setting - offset 0x4 */
.word  __start                  /* Entrypoint boot address offset - 0x08 */

/* SDK Revision */
.word  0x0000144C				/* Entrypoint boot address - offset 0x0C */

.word  0x00000000               /* Checksum 1 (OVERWRITTEN BY MAKEMASK) - offset 0x1 */
.word  0x00000000               /* Checksum 2 (OVERWRITTEN BY MAKEMASK) - continued */
.word  0x00000000               /* Reserved by Nintendo - offset 0x18 */
.word  0x00000000               /* Reserved by Nintendo - continued */
.ascii "DOOM 64 XE 1.3      "   /* Internal ROM name (Max 20 characters) - offset 0x20 */
.byte  0x02               		/* Homebrew Header controller 1 with memory pak - offset 0x34 */
.byte  0xFF               		/* Homebrew Header controller 2 not attached - offset 0x35 */
.byte  0xFF               		/* Homebrew Header controller 3 not attached - offset 0x36 */
.byte  0xFF               		/* Homebrew Header controller 4 not attached - offset 0x37 */
/* Game ID Begins here */
.word  0x0000004E               /* Cartridge Type (N) - offset 0x3B */
.ascii "ED"                     /* ED for Homebrew Cartridge ID (DM originally) - offset 0x3C */
.ascii "A"						/* Region (A for all regions) - continued */
.byte  0x02						/* Homebrew Savetype and region (0x02 = 00000010 in binary) is region free and no save type) - offset 0x3F */