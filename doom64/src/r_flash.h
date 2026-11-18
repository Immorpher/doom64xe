/* r_flash.h -- short lived colored light flashes (muzzle flashes and the like).
   Render only: nothing here is used by gameplay code or demos. */

#ifndef __R_FLASH_H__
#define __R_FLASH_H__

extern int flash_count;		/* number of active flashes, 0 = no work to do while drawing */

void R_FlashClear(void);
void R_FlashTick(void);		/* once per game tic */

/* Starts (or refreshes) the flash belonging to "owner". x, y, z are map units. */
void R_FlashAdd(void *owner, int x, int y, int z, int radius, int r, int g, int b, int tics);

/* Lights one vertex: returns color (0xRRGGBBAA) brightened toward white
   (multiplicatively, so it can never exceed the texture's own colors). */
unsigned int R_FlashColor(unsigned int color, int x, int y, int z);

/* Surfaces are lit through their vertices only, so a flash sitting in the middle of a
   big polygon would light nothing. These find the point of a wall or floor/ceiling
   nearest to the closest flash that reaches it; the caller adds a vertex there. */
struct leaf_s;
int R_FlashWallPoint(int x1, int y1, int x2, int y2, int top, int bottom,
                     int *px, int *py, int *pz, int *tfrac, int *hfrac);	/* fractions 0..256 */
int R_FlashPlanePoint(void *leaf, int numverts, int zpos, int *px, int *py);
unsigned int R_LerpColor(unsigned int a, unsigned int b, int k256);

#endif
