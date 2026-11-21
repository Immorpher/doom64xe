/* r_flash.h -- short lived colored light flashes (muzzle flashes and the like).
   Render only: nothing here is used by gameplay code or demos. */

#ifndef __R_FLASH_H__
#define __R_FLASH_H__

extern int flash_count;		/* number of active flashes, 0 = no work to do while drawing */

void R_FlashClear(void);
void R_FlashTick(void);		/* once per game tic */

/* Starts (or refreshes) the flash belonging to "owner". x, y, z are map units. */
void R_FlashAdd(void *owner, int x, int y, int z, int radius, int r, int g, int b, int tics, int priority);

/* ready made flashes for gameplay events. priority: when all slots are busy a flash only
   replaces one of equal or lower priority (player 3, explosions 2, enemy attacks 1). */
enum { FLASH_ENEMYGUN, FLASH_FIRE, FLASH_PLASMA, FLASH_ROCKET, FLASH_BARREL, FLASH_BFG, FLASH_LASER, FLASH_BLAST, FLASH_NIGHTMARE, FLASH_SPARK, FLASH_BARON, FLASH_KNIGHT, FLASH_TRACER, FLASH_MOTHER, FLASH_PAIN };
#define FLASHPRI_ATTACK		1
#define FLASHPRI_EXPLOSION	2
#define FLASHPRI_PLAYER		3
void R_FlashPreset(void *owner, int x, int y, int z, int kind, int priority);

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
unsigned int R_LightenColor(unsigned int color, int k256);	/* moves every color channel k256/256 of the way to white, alpha kept */

#endif
