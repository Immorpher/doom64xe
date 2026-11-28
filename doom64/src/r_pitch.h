/*
 * r_pitch.h - visibility tests for a view that is tilted up or down (vertical look)
 *
 * The normal BSP code works out which screen columns a wall or a box covers from
 * the horizontal angle alone, which is only right while the view is level. Once the
 * view is tilted, the column of a point also depends on its height, so walls near the
 * sides were thrown away although they were on screen.
 *
 * These helpers do the tests in the tilted view instead. A point is given as
 * lateral x, forward f (both along the level view direction) and height h above the
 * eye. With the view tilted up by an angle a (s = sin a, c = cos a):
 *
 *     depth    z = c * f + s * h
 *     vertical y = c * h - s * f
 *
 * and the point is on screen when |x| < z and |y| < 0.75 * z (the 90 x 73.7 degree
 * projection) and z > 8 (near plane). Everything is 16.16 fixed point integers, no floating
 * point (it is slow on the N64 CPU), and plain C with no engine types so it can be tested on
 * its own. Nothing here is used while the view is level.
 *
 * All columns returned are on the safe side: ranges a visibility test looks at are never
 * smaller than the real ones, and ranges that are marked as covered never larger.
 */
#ifndef R_PITCH_H
#define R_PITCH_H

#define RP_INL static inline __attribute__((always_inline))

#define RP_ONE    65536
#define RP_NEAR   (8 * RP_ONE)    /* near plane of the projection */
#define RP_NEARM  (7 * RP_ONE)    /* a point nearer than this is behind it, 1 unit of slack */
#define RP_MARGIN (2 * RP_ONE)    /* slack on every side so truncated vertices stay safe */

#define RP_LEFT   1
#define RP_RIGHT  2
#define RP_BEHIND 4
#define RP_TOP    8
#define RP_BOTTOM 16

typedef struct
{
    int x, z, y;
    int out;        /* RP_ flags for the planes this point is outside of */
} rp_pt_t;

/*
 * (a * b) >> 16 in 32 bits: a is a sine or cosine (at most 1.0, 16.16) cut to 14 bits and b a distance
 * cut to a quarter of a unit. The product of the two is then at most 2^31 and needs no 64 bit multiply
 * or shift (slow on the N64 CPU). The error is under half a unit for anything within 8000 units, which
 * the margins below cover; farther away it grows by 1 unit per 16000.
 */
#define RP_MUL(a, b) (((a) >> 2) * ((b) >> 14))

RP_INL void RP_Point(rp_pt_t *p, int s, int c, int x, int f, int h)
{
    int z = RP_MUL(c, f) + RP_MUL(s, h);
    int y = RP_MUL(c, h) - RP_MUL(s, f);
    int z75 = (z >> 1) + (z >> 2) + RP_MARGIN;  /* 0.75 * z plus the slack */
    int zm = z + RP_MARGIN;
    int out = 0;

    p->x = x;
    p->z = z;
    p->y = y;

    if (x < -zm)       out |= RP_LEFT;
    if (x > zm)        out |= RP_RIGHT;
    if (z < RP_NEARM)  out |= RP_BEHIND;
    if (y > z75)       out |= RP_TOP;
    if (y < -z75)      out |= RP_BOTTOM;
    p->out = out;
}

/*
 * 160 * x / z rounded toward zero, in whole columns from the centre, for z in front of the near
 * plane. Both are cut to 1/256 unit so the product stays inside 32 bits. Off screen values are
 * clamped; they only matter as being far off to one side.
 */
RP_INL int RP_Col(int x, int z)
{
    int xs = x >> 8;
    int zs = z >> 8;
    int q;

    if (zs < 1)
        zs = 1;
    q = (xs * 160) / zs;
    if (q < -1000) q = -1000;
    if (q >  1000) q =  1000;
    return q;
}

/*
 * Columns a set of points can reach, from only their lateral range [xmin,xmax] and depth range
 * [zmin,zmax] (all in front of the near plane): two divides instead of one per point.
 */
RP_INL void RP_ColRange(int xmin, int xmax, int zmin, int zmax, int *a, int *b)
{
    *a = 160 + RP_Col(xmin, (xmin < 0) ? zmin : zmax) - 2;
    *b = 160 + RP_Col(xmax, (xmax < 0) ? zmax : zmin) + 2;
}

/*
 * A wall from (x1,f1) to (x2,f2), drawn between heights hlo and hhi (relative to the
 * eye). The first point must be the left one on screen, which the facing test of the
 * caller gives.
 *
 * Returns 0 when the wall can not be on screen. Otherwise returns 1 and
 *   [*a, *b]   the columns it might touch (inclusive), to see if any are still open
 *   [*ia, *ib) the columns it surely covers over its whole height, which may be marked
 *              solid when canocclude is set, between occ_lo and occ_hi. Empty when
 *              *ia >= *ib. Walls that cross the near plane do not occlude.
 */
RP_INL int RP_Seg(int s, int c,
                  int x1, int f1, int x2, int f2,
                  int hlo, int hhi,
                  int canocclude, int occ_lo, int occ_hi,
                  int *a, int *b, int *ia, int *ib)
{
    rp_pt_t p[4];
    int zmin, zmax, xmin, xmax;
    int i;

    *ia = 0;
    *ib = 0;

    RP_Point(&p[0], s, c, x1, f1, hlo);
    RP_Point(&p[1], s, c, x1, f1, hhi);
    RP_Point(&p[2], s, c, x2, f2, hhi);
    RP_Point(&p[3], s, c, x2, f2, hlo);

    if (p[0].out & p[1].out & p[2].out & p[3].out)
        return 0;

    zmin = zmax = p[0].z;
    for (i = 1; i < 4; i++)
    {
        if (p[i].z < zmin) zmin = p[i].z;
        if (p[i].z > zmax) zmax = p[i].z;
    }

    if (zmin < RP_NEAR)
    {
        /* crosses the near plane: could be anywhere, and does not occlude */
        *a = 0;
        *b = 319;
        return 1;
    }

    if (canocclude)
    {
        rp_pt_t q[4];
        const rp_pt_t *qq;
        int same = (occ_lo == hlo && occ_hi == hhi);

        if (same)
        {
            qq = p;     /* one sided wall: same corners */
        }
        else
        {
            RP_Point(&q[0], s, c, x1, f1, occ_lo);
            RP_Point(&q[1], s, c, x1, f1, occ_hi);
            RP_Point(&q[2], s, c, x2, f2, occ_hi);
            RP_Point(&q[3], s, c, x2, f2, occ_lo);
            qq = q;
        }

        if (qq[0].z >= RP_NEAR && qq[1].z >= RP_NEAR && qq[2].z >= RP_NEAR && qq[3].z >= RP_NEAR)
        {
            /* the left edge slants on screen, so only columns right of both its ends
               and left of both ends of the right edge are covered at every height */
            int l0 = RP_Col(qq[0].x, qq[0].z);
            int l1 = RP_Col(qq[1].x, qq[1].z);
            int r1 = RP_Col(qq[2].x, qq[2].z);
            int r0 = RP_Col(qq[3].x, qq[3].z);
            int L = (l0 > l1) ? l0 : l1;
            int R = (r0 < r1) ? r0 : r1;

            *ia = 160 + L + 2;
            *ib = 160 + R - 2;

            if (same)
            {
                /* all four columns are known: the outer range comes for free */
                int umin = (l0 < l1) ? l0 : l1;
                int umax = (l0 > l1) ? l0 : l1;

                if (r0 < umin) umin = r0;
                if (r1 < umin) umin = r1;
                if (r0 > umax) umax = r0;
                if (r1 > umax) umax = r1;
                *a = 160 + umin - 2;
                *b = 160 + umax + 2;
                return 1;
            }
        }
    }

    xmin = (x1 < x2) ? x1 : x2;
    xmax = (x1 < x2) ? x2 : x1;
    RP_ColRange(xmin, xmax, zmin, zmax, a, b);
    return 1;
}

/*
 * A box from corner points given as view space (lateral, forward) pairs, with heights
 * between hlo and hhi (relative to the eye). Returns 0 when it can not be on screen.
 * Otherwise returns 1 and *a, *b as for RP_Seg (the whole width when the box reaches
 * the near plane).
 */
RP_INL int RP_Box(int s, int c, const int xy[4][2], int hlo, int hhi, int *a, int *b)
{
    rp_pt_t p[8];
    int all, i;
    int xmin, xmax, zmin, zmax;

    all = -1;
    for (i = 0; i < 4; i++)
    {
        RP_Point(&p[i],     s, c, xy[i][0], xy[i][1], hlo);
        RP_Point(&p[i + 4], s, c, xy[i][0], xy[i][1], hhi);
    }

    xmin = xmax = p[0].x;
    zmin = zmax = p[0].z;
    for (i = 0; i < 8; i++)
    {
        all &= p[i].out;
        if (p[i].x < xmin) xmin = p[i].x;
        if (p[i].x > xmax) xmax = p[i].x;
        if (p[i].z < zmin) zmin = p[i].z;
        if (p[i].z > zmax) zmax = p[i].z;
    }

    if (all)
        return 0;

    if (zmin < RP_NEAR)
    {
        *a = 0;
        *b = 319;
        return 1;
    }

    RP_ColRange(xmin, xmax, zmin, zmax, a, b);
    return 1;
}

/*
 * Visibility only, without any columns: 0 when the wall / box can not be on screen. Occlusion is
 * tracked by the bearing of the walls instead (r_phase1.c), so no screen columns are needed.
 */
RP_INL int RP_SegVis(int s, int c, int x1, int f1, int x2, int f2, int hlo, int hhi)
{
    rp_pt_t p;
    int all;

    /* stops as soon as one point is inside the screen, or two points are outside different planes */
    RP_Point(&p, s, c, x1, f1, hlo);  all = p.out;
    if (!all) return 1;
    RP_Point(&p, s, c, x2, f2, hhi);  all &= p.out;
    if (!all) return 1;
    RP_Point(&p, s, c, x1, f1, hhi);  all &= p.out;
    if (!all) return 1;
    RP_Point(&p, s, c, x2, f2, hlo);  all &= p.out;
    return all == 0;
}

RP_INL int RP_BoxVis(int s, int c, const int xy[4][2], int hlo, int hhi)
{
    rp_pt_t p;
    int all, i;
    int chl, shl, chh, shh;

    /* the first point may already be on screen, which is the common case */
    RP_Point(&p, s, c, xy[0][0], xy[0][1], hlo);  all = p.out;
    if (!all) return 1;

    /* the products with the two heights are the same for every corner: once, not per point */
    chl = RP_MUL(c, hlo);  shl = RP_MUL(s, hlo);
    chh = RP_MUL(c, hhi);  shh = RP_MUL(s, hhi);

    for (i = 0; i < 4; i++)
    {
        int x = xy[i][0];
        int cf = RP_MUL(c, xy[i][1]);
        int sf = RP_MUL(s, xy[i][1]);
        int z, y, zm, z75, out;

        z = cf + shh;  y = chh - sf;
        zm = z + RP_MARGIN;  z75 = (z >> 1) + (z >> 2) + RP_MARGIN;
        out = 0;
        if (x < -zm) out |= RP_LEFT;
        if (x > zm) out |= RP_RIGHT;
        if (z < RP_NEARM) out |= RP_BEHIND;
        if (y > z75) out |= RP_TOP;
        if (y < -z75) out |= RP_BOTTOM;
        all &= out;
        if (!all) return 1;

        if (i == 0)
            continue;       /* the low point of the first corner was the one tested above */

        z = cf + shl;  y = chl - sf;
        zm = z + RP_MARGIN;  z75 = (z >> 1) + (z >> 2) + RP_MARGIN;
        out = 0;
        if (x < -zm) out |= RP_LEFT;
        if (x > zm) out |= RP_RIGHT;
        if (z < RP_NEARM) out |= RP_BEHIND;
        if (y > z75) out |= RP_TOP;
        if (y < -z75) out |= RP_BOTTOM;
        all &= out;
        if (!all) return 1;
    }
    return 0;
}

#endif /* R_PITCH_H */
