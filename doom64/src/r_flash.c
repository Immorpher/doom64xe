/* r_flash.c -- short lived colored light flashes

   A flash lights the walls, floors and sprites around it by raising their
   vertex colors toward white: each color channel moves up by
   (255 - channel) * strength, where strength falls off with the squared
   distance from the flash and with its age. The RDP multiplies the vertex color
   with the texture as always, so a lit surface never gets brighter than its
   texture. The result is smoothed over each polygon between its vertices.

   Only drawing uses this, so gameplay and demos are not affected. */

#include "doomdef.h"
#include "r_local.h"
#include "r_flash.h"

#define MAXFLASHES	4

typedef struct
{
	void *owner;		/* who started it (refreshing replaces the same owner's flash) */
	int x, y, z;		/* map units */
	int radius;
	int radius2;
	int r, g, b;		/* flash color */
	int life;		/* tics left */
	int priority;		/* see FLASHPRI_* */
	int maxlife;
} flash_t;

static flash_t flashes[MAXFLASHES];
int flash_count;

void R_FlashClear(void)
{
	int i;

	for (i = 0; i < MAXFLASHES; i++)
		flashes[i].life = 0;
	flash_count = 0;
}

static void R_FlashCount(void)
{
	int i, n = 0;

	for (i = 0; i < MAXFLASHES; i++)
	{
		if (flashes[i].life > 0)
			n++;
	}
	flash_count = n;
}

void R_FlashTick(void)
{
	int i;

	for (i = 0; i < MAXFLASHES; i++)
	{
		if (flashes[i].life > 0)
			flashes[i].life--;
	}
	R_FlashCount();
}

void R_FlashAdd(void *owner, int x, int y, int z, int radius, int r, int g, int b, int tics, int priority)
{
	int i, slot = -1;
	flash_t *f;

	/* the same owner's flash first, then a free slot, then the weakest flash
	   (lowest priority, least time left) if it is not more important than this one */
	for (i = 0; i < MAXFLASHES; i++)
	{
		if (flashes[i].life > 0 && flashes[i].owner == owner)
		{
			slot = i;
			if (flashes[i].priority > priority)
				priority = flashes[i].priority;
			break;
		}
	}
	if (slot < 0)
	{
		for (i = 0; i < MAXFLASHES; i++)
		{
			if (flashes[i].life <= 0)
			{
				slot = i;
				break;
			}
			if (slot < 0 || flashes[i].priority < flashes[slot].priority ||
			    (flashes[i].priority == flashes[slot].priority && flashes[i].life < flashes[slot].life))
				slot = i;
		}
		if (flashes[slot].life > 0 && flashes[slot].priority > priority)
			return; /* everything running is more important */
	}

	f = &flashes[slot];
	f->owner = owner;
	f->x = x;
	f->y = y;
	f->z = z;
	f->radius = radius;
	f->radius2 = radius * radius;
	f->r = r;
	f->g = g;
	f->b = b;
	f->life = tics;
	f->maxlife = tics;
	f->priority = priority;
	R_FlashCount();
}

typedef struct
{
	int r, g, b;
	int radius;
	int tics;
} flashpreset_t;

static const flashpreset_t flashpresets[] =
{
	{255, 200, 110, 256, 4},	/* FLASH_ENEMYGUN: pistol and shotgun shots */
	{255, 140,  60, 288, 5},	/* FLASH_FIRE: fireball launches and impacts */
	{110, 170, 255, 288, 5},	/* FLASH_PLASMA */
	{255, 170,  80, 448, 9},	/* FLASH_ROCKET: rocket blast */
	{255, 150,  60, 448, 10},	/* FLASH_BARREL */
	{120, 255, 140, 512, 10},	/* FLASH_BFG */
	{255,  90,  80, 320, 5},	/* FLASH_LASER */
	{255, 190, 100, 352, 7},	/* FLASH_BLAST: other explosions */
	{190,  80, 255, 288, 5},	/* FLASH_NIGHTMARE: purple nightmare fireballs */
	{255, 160,  60, 160, 4},	/* FLASH_SPARK: chainsaw wall sparks */
	{255,  50,  40, 288, 5},	/* FLASH_BARON: red baron fireballs */
	{ 70, 255,  90, 288, 5},	/* FLASH_KNIGHT: green hell knight fireballs */
	{255,  40,  40, 288, 5},	/* FLASH_TRACER: red tracking fireballs */
	{230,  30,  60, 384, 6}		/* FLASH_MOTHER: Mother Demon tracer volley */
};

/* Starts a flash from a gameplay event at map position x, y, z. It follows the Flash Level
   option, flickers with I_Random and is skipped when it is too far away to be seen. */
void R_FlashPreset(void *owner, int x, int y, int z, int kind, int priority)
{
	const flashpreset_t *p = &flashpresets[kind];
	long long dx, dy, reach;
	int k;

	if (FlashLevel >= 7)
		return;

	/* distance from the camera the person is looking through */
	dx = x - (viewx >> FRACBITS);
	dy = y - (viewy >> FRACBITS);
	reach = p->radius + 768;
	if (dx * dx + dy * dy > reach * reach)
		return;

	k = 170 + (I_Random() % 86); /* 170..255 */
	k = k * (7 - FlashLevel) / 7;

	R_FlashAdd(owner, x, y, z, p->radius, p->r * k / 255, p->g * k / 255, p->b * k / 255, p->tics, priority);
}

unsigned int R_FlashColor(unsigned int color, int x, int y, int z)
{
	int i, dx, dy, dz, d2, k;
	int ar = 0, ag = 0, ab = 0;
	int cr, cg, cb;
	flash_t *f;

	for (i = 0; i < MAXFLASHES; i++)
	{
		f = &flashes[i];
		if (f->life <= 0)
			continue;

		dx = x - f->x;
		if (dx > f->radius || dx < -f->radius)
			continue;
		dy = y - f->y;
		if (dy > f->radius || dy < -f->radius)
			continue;
		dz = z - f->z;
		if (dz > f->radius || dz < -f->radius)
			continue;

		d2 = dx * dx + dy * dy + dz * dz;
		if (d2 >= f->radius2)
			continue;

		/* 0..256: falls off with distance squared, fades out with age */
		k = (((f->radius2 - d2) * 256) / f->radius2) * f->life / f->maxlife;

		ar += (k * f->r) >> 8;
		ag += (k * f->g) >> 8;
		ab += (k * f->b) >> 8;
	}

	if ((ar | ag | ab) == 0)
		return color;

	if (ar > 255) ar = 255;
	if (ag > 255) ag = 255;
	if (ab > 255) ab = 255;

	cr = color >> 24;
	cg = (color >> 16) & 255;
	cb = (color >> 8) & 255;

	cr += ((255 - cr) * ar) >> 8;
	cg += ((255 - cg) * ag) >> 8;
	cb += ((255 - cb) * ab) >> 8;

	return ((unsigned int)cr << 24) | ((unsigned int)cg << 16) | ((unsigned int)cb << 8) | (color & 255);
}

unsigned int R_LerpColor(unsigned int a, unsigned int b, int k256)
{
	int s, r = 0, i;

	for (s = 24; s >= 0; s -= 8)
	{
		i = (a >> s) & 255;
		i += ((int)((b >> s) & 255) - i) * k256 / 256;
		r |= i << s;
	}
	return r;
}

/* closest point to (fx, fy) on the segment x1,y1 - x2,y2; num/len2 is the position along it */
static void R_ClosestOnSegment(long long x1, long long y1, long long x2, long long y2,
                               long long fx, long long fy,
                               long long *cx, long long *cy, long long *num, long long *len2)
{
	long long dx = x2 - x1, dy = y2 - y1, n;

	*len2 = dx * dx + dy * dy;
	n = (fx - x1) * dx + (fy - y1) * dy;
	if (*len2 <= 0 || n < 0)
		n = 0;
	else if (n > *len2)
		n = *len2;
	*num = n;
	if (*len2 > 0)
	{
		*cx = x1 + dx * n / *len2;
		*cy = y1 + dy * n / *len2;
	}
	else
	{
		*cx = x1;
		*cy = y1;
	}
}

int R_FlashWallPoint(int x1, int y1, int x2, int y2, int top, int bottom,
                     int *px, int *py, int *pz, int *tfrac, int *hfrac)
{
	int i, found = 0, cz;
	long long cx, cy, num, len2, ex, ey, ez, d2, best = 0x7fffffffffffffffLL;
	flash_t *f;

	for (i = 0; i < MAXFLASHES; i++)
	{
		f = &flashes[i];
		if (f->life <= 0)
			continue;

		R_ClosestOnSegment(x1, y1, x2, y2, f->x, f->y, &cx, &cy, &num, &len2);
		cz = f->z;
		if (cz > top)
			cz = top;
		if (cz < bottom)
			cz = bottom;

		ex = cx - f->x;
		ey = cy - f->y;
		ez = cz - f->z;
		d2 = ex * ex + ey * ey + ez * ez;
		if (d2 < f->radius2 && d2 < best)
		{
			best = d2;
			found = 1;
			*px = (int)cx;
			*py = (int)cy;
			*pz = cz;
			/* the vertex position is a whole map unit, so measure how far along the wall
			   that rounded point really is: its texture coordinate must match it, or the
			   texture would shift around the vertex */
			num = ((long long)(*px - x1)) * (x2 - x1) + ((long long)(*py - y1)) * (y2 - y1);
			if (num < 0)
				num = 0;
			else if (num > len2)
				num = len2;
			*tfrac = len2 > 0 ? (int)(num * 256 / len2) : 0;
			*hfrac = top != bottom ? (top - cz) * 256 / (top - bottom) : 0;
		}
	}
	return found;
}

int R_FlashPlanePoint(void *leafptr, int numverts, int zpos, int *px, int *py)
{
	leaf_t *leaf = (leaf_t *)leafptr;
	int i, j, pos, neg, found = 0;
	long long ax, ay, bx, by, cx, cy, num, len2, cross, ex, ey, ez, d2, d2xy, best = 0x7fffffffffffffffLL;
	long long bestx = 0, besty = 0, ex2, ey2;
	flash_t *f;

	for (i = 0; i < MAXFLASHES; i++)
	{
		f = &flashes[i];
		if (f->life <= 0)
			continue;

		ez = zpos - f->z;
		if (ez >= f->radius || ez <= -f->radius)
			continue;

		/* is the flash straight above/below the polygon (inside the convex outline)? */
		pos = neg = 0;
		for (j = 0; j < numverts; j++)
		{
			ax = leaf[j].vertex->x >> FRACBITS;
			ay = leaf[j].vertex->y >> FRACBITS;
			bx = leaf[(j + 1) % numverts].vertex->x >> FRACBITS;
			by = leaf[(j + 1) % numverts].vertex->y >> FRACBITS;
			cross = (bx - ax) * (f->y - ay) - (by - ay) * (f->x - ax);
			if (cross > 0)
				pos = 1;
			else if (cross < 0)
				neg = 1;
		}

		if (!(pos && neg))
		{
			cx = f->x;
			cy = f->y;
			d2xy = 0;
		}
		else
		{
			/* otherwise the nearest point on the outline */
			d2xy = 0x7fffffffffffffffLL;
			cx = cy = 0;
			for (j = 0; j < numverts; j++)
			{
				ax = leaf[j].vertex->x >> FRACBITS;
				ay = leaf[j].vertex->y >> FRACBITS;
				bx = leaf[(j + 1) % numverts].vertex->x >> FRACBITS;
				by = leaf[(j + 1) % numverts].vertex->y >> FRACBITS;
				R_ClosestOnSegment(ax, ay, bx, by, f->x, f->y, &ex2, &ey2, &num, &len2);
				ex = ex2 - f->x;
				ey = ey2 - f->y;
				if (ex * ex + ey * ey < d2xy)
				{
					d2xy = ex * ex + ey * ey;
					cx = ex2;
					cy = ey2;
				}
			}
		}

		d2 = d2xy + ez * ez;
		if (d2 < f->radius2 && d2 < best)
		{
			best = d2;
			found = 1;
			bestx = cx;
			besty = cy;
		}
	}

	if (found)
	{
		*px = (int)bestx;
		*py = (int)besty;
	}
	return found;
}

unsigned int R_LightenColor(unsigned int color, int k256)
{
	int cr, cg, cb;

	if (k256 > 256)
		k256 = 256;

	cr = color >> 24;
	cg = (color >> 16) & 255;
	cb = (color >> 8) & 255;

	cr += ((255 - cr) * k256) >> 8;
	cg += ((255 - cg) * k256) >> 8;
	cb += ((255 - cb) * k256) >> 8;

	return ((unsigned int)cr << 24) | ((unsigned int)cg << 16) | ((unsigned int)cb << 8) | (color & 255);
}
