
//Renderer phase 1 - BSP traversal

#include "doomdef.h"
#include "r_local.h"
#include "r_pitch.h"

byte checkcoord[12][4] =				// 8005B110
{
	{ 3, 0, 2, 1 },/* Above,Left */
	{ 3, 0, 2, 0 },/* Above,Center */
	{ 3, 1, 2, 0 },/* Above,Right */
	{ 0, 0, 0, 0 },
	{ 2, 0, 2, 1 },/* Center,Left */
	{ 0, 0, 0, 0 },/* Center,Center */
	{ 3, 1, 3, 0 },/* Center,Right */
	{ 0, 0, 0, 0 },
	{ 2, 0, 3, 1 },/* Below,Left */
	{ 2, 1, 3, 1 },/* Below,Center */
	{ 2, 1, 3, 0 },/* Below,Right */
	{ 0, 0, 0, 0 }
};


/*
 * Screen column of a point from its view-space x and depth y:
 * ((x/y) * 160) + 160, done with the shifts the original used.
 * The division runs on the FPU (about 30 cycles) instead of ddiv (about 70);
 * when y is too small for the float result to stay in range the integer
 * divide is used, so the result never differs by more than float rounding.
 */
static INLINE_ALWAYS int R_ScreenCol(fixed_t x, fixed_t y)
{
    fixed_t xo;

    if (y >= (4*FRACUNIT))
        xo = (fixed_t)(((float)x / (float)y) * 65536.0f);
    else
        xo = FixedDiv2(x, y);

    return (((xo >> 9) + (xo >> 11))) + 160;
}

/*
 * Is any column from a to b (inclusive) still open?
 *
 * Columns are the screen position rounded down, but the RDP draws a pixel when
 * its centre is inside the edge, so something that is only a few columns wide
 * (or only one) can also cover the column on either side of the rounded range.
 * Looking one column further out on both sides keeps R_CheckBBox and R_AddLine
 * from throwing away geometry that is still visible at the edge of a wall in
 * front of it. That geometry was what left one pixel wide cracks, flickering as
 * the view moved.
 */
/*
 * The renderer sends every vertex to the RSP as whole world units (x >> FRACBITS), but
 * the map vertices, especially the ones made when the BSP splits a wall, have a
 * fraction. Culling with the exact position put the edge of a wall up to a unit away
 * from where it is drawn, which near the player is several pixels. A column was then
 * marked as covered that the wall did not cover, and what is behind it was culled.
 * Cut the positions the same way so culling matches what is drawn.
 */
#define R_DRAWNMASK ((fixed_t)0xffff0000)

/*
 * Columns looked at beyond each side of a range, see below. One column hides the
 * cracks (two hid no more). They only show against the sky, so it is 1 while the
 * last frame drew a sky and 0 otherwise, which is the original behaviour.
 */
static int colmargin = 1;

static INLINE_ALWAYS boolean R_ColsOpen(int a, int b)
{
    byte *p;

    a -= colmargin;
    b += colmargin;

    if (a < 0)
        a = 0;
    if (b > 319)
        b = 319;

    p = &solidcols[a];
    while (a <= b)
    {
        if (*p == 0)
            return true;
        p++;
        a++;
    }
    return false;
}

void	R_RenderBSPNode(int bspnum) HOT;
boolean R_CheckBBox(const fixed_t bspcoord[4]) HOT;
void	R_Subsector(int num) HOT;
void	R_AddLine(seg_t *line) HOT;
void    R_AddSprite(subsector_t *sub) HOT;
void    R_RenderBSPNodeNoClip(int bspnum) HOT;

/*
 * Vertical look. While the view is tilted the tests below can not use the horizontal
 * angle only, so R_BSP sets these and the *Pitch versions (r_pitch.h) are used.
 * When the view is level nothing here is used and the original tests run unchanged.
 */
static boolean rp_active;       /* view tilted by vertical look */
static boolean rp_occbox;       /* R_CheckBBox found the box open but off screen: walk it only for the walls that hide things */
static boolean rp_occ;          /* walking a subsector only for the walls that hide things behind it */
static int      rp_lr1, rp_lr2; /* bearings (left, right) of the last box R_CheckBBox found open */

/* Off screen boxes found open, not walked yet: their walls only matter for what is drawn after them in
   the directions they cover (see R_DeferOcc) */
#define RP_MAXDEFER 128
static int      rp_ndef;
static int      rp_defchild[RP_MAXDEFER];
static int      rp_defr1[RP_MAXDEFER], rp_defr2[RP_MAXDEFER];
static boolean  rp_defdone[RP_MAXDEFER];
static fixed_t rp_s, rp_c;      /* sine and cosine of the tilt (up is positive) */
static fixed_t rp_boxlo, rp_boxhi; /* lowest floor and highest ceiling of the level, relative to the eye */
static fixed_t *rp_nodeh;       /* per BSP node and child: lowest floor and highest ceiling inside, 4 per node */
static fixed_t  rp_globallo, rp_globalhi;
static int      rp_cn, rp_cs;   /* node and child side of the box R_CheckBBox is asked about */
static int      rp_stamp;       /* sectorstamp the node heights were computed for */

/*
 * Lowest floor and highest ceiling below every BSP child, so a box is tested only over the
 * heights that are really in it instead of those of the whole level. Done every frame while the
 * view is tilted since sectors move. Nodes are normally stored children first; any child that
 * is not computed yet falls back to the heights of the whole level.
 */
static void R_PitchNodeHeights(void)
{
    int n, side, i;

    if (!rp_nodeh)
    {
        rp_nodeh = (fixed_t *)Z_Malloc(numnodes * 4 * sizeof(fixed_t), PU_LEVEL, &rp_nodeh);
        rp_stamp = 0;   /* new level: compute */
    }

    /* the heights only change when a floor or ceiling moves (counted in p_floor.c) */
    if (rp_stamp == sectorstamp)
        return;
    rp_stamp = sectorstamp;

    {
        int k;
        fixed_t flo = sectors[0].floorheight, fhi = sectors[0].ceilingheight;

        for (k = 1; k < numsectors; k++)
        {
            if (sectors[k].floorheight < flo)   flo = sectors[k].floorheight;
            if (sectors[k].ceilingheight > fhi) fhi = sectors[k].ceilingheight;
        }
        rp_globallo = flo;
        rp_globalhi = fhi;
    }

    for (n = 0; n < numnodes; n++)
    {
        for (side = 0; side < 2; side++)
        {
            int child = nodes[n].children[side];
            fixed_t lo, hi;

            if (child & NF_SUBSECTOR)
            {
                subsector_t *sub = &subsectors[(child == -1) ? 0 : (child & ~NF_SUBSECTOR)];
                seg_t *seg = &segs[sub->firstline];

                lo = sub->sector->floorheight;
                hi = sub->sector->ceilingheight;

                /* wall pieces can reach the heights of the sector on the other side */
                for (i = 0; i < sub->numlines; i++, seg++)
                {
                    sector_t *bs = seg->backsector;

                    if (bs)
                    {
                        if (bs->floorheight < lo)   lo = bs->floorheight;
                        if (bs->ceilingheight > hi) hi = bs->ceilingheight;
                    }
                }
            }
            else if (child < n)
            {
                const fixed_t *c = &rp_nodeh[child * 4];

                lo = (c[0] < c[2]) ? c[0] : c[2];
                hi = (c[1] > c[3]) ? c[1] : c[3];
            }
            else
            {
                lo = rp_globallo;
                hi = rp_globalhi;
            }

            rp_nodeh[n * 4 + side * 2]     = lo;
            rp_nodeh[n * 4 + side * 2 + 1] = hi;
        }
    }
}


/*
 * Occlusion while the view is tilted is kept by the bearing of the walls, the direction they are
 * in as seen from above, instead of by screen column. A wall that closes a sector hides what is
 * behind it in the same direction at every height, so this stays right however the view is
 * tilted, and the walls of a room join up exactly because the end of one wall and the start of
 * the next have the same bearing. (Screen columns of a tilted wall slant with the height, so
 * only part of them could be marked and the gaps between walls let everything behind through.)
 * What is on screen is then decided by the plane tests of r_pitch.h.
 *
 * The covered part of the circle is kept as a sorted list of ranges, like the solid segments of the
 * original Doom renderer, in 32768 units for a full turn (0.011 degrees, 1/16 of a 0.18 degree step),
 * 16384 straight ahead, rising to the right. Ranges that touch or are only a hair apart are joined,
 * and a test looks a hair inside the range it is given, so a wall exactly behind another one
 * (same bearings, only rounding apart) counts as hidden, as it does in the level view.
 */
#define RB_FINE   32768
#define RB_GAP    32        /* gaps up to this are closed (0.35 degrees, about one column at the middle of the screen) */
#define RB_EPS    16        /* a test looks this far inside its range (0.18 degrees) */
#define RB_MAXR   96
static int rb_lo[RB_MAXR], rb_hi[RB_MAXR], rb_n;

/*
 * atan of 0..1 in steps of 1/1024 of the slope, in 1/16 of a bearing step (4096 = 45 degrees). Made
 * from the game's own tangent table the first time it is needed; 2 KB so it stays in the cache.
 */
static unsigned short rb_atan[1025];
static boolean rb_atan_ready;

static void R_BearingInit(void)
{
    int i;

    for (i = 0; i <= 1024; i++)
        rb_atan[i] = (unsigned short)(tantoangle[i << 1] >> 17);
    rb_atan_ready = true;
}

/* n < d: the table index of n / d */
static INLINE_ALWAYS int R_BearingRatio(unsigned int n, unsigned int d)
{
    /* the same shifts as halving until d < 0x200000, in four steps instead of up to eleven */
    if (d >= 0x10000000u) { n >>= 8; d >>= 8; }
    if (d >= 0x1000000u)  { n >>= 4; d >>= 4; }
    if (d >= 0x400000u)   { n >>= 2; d >>= 2; }
    if (d >= 0x200000u)   { n >>= 1; d >>= 1; }
    return (int)((n << 10) / d);
}

/*
 * The bearing of a point seen from the eye: x lateral (right is positive), f forward. 16384 is
 * straight ahead and it rises to the right, 32768 units for a full turn. Accurate to a few units;
 * the same point always gives the same bearing.
 */
static INLINE_ALWAYS int R_BearingAngle(fixed_t x, fixed_t f)
{
    unsigned int ax = (x < 0) ? (unsigned int)-x : (unsigned int)x;
    unsigned int af = (f < 0) ? (unsigned int)-f : (unsigned int)f;
    int m;      /* angle from straight ahead in 1/16 steps, 0..16384 */

    if (ax == 0 && af == 0)
        return RB_FINE / 2;

    if (ax <= af)
        m = rb_atan[R_BearingRatio(ax, af)];                    /* within 45 degrees of ahead or behind */
    else
        m = 8192 - rb_atan[R_BearingRatio(af, ax)];             /* closer to the sides */

    if (f < 0)
        m = 16384 - m;                                          /* behind */

    return ((x >= 0) ? (16384 + m) : (16384 - m)) & (RB_FINE - 1);
}

/* is any point from a to b (inclusive, 0..RB_FINE-1, a <= b) not covered yet? */
static INLINE_ALWAYS boolean R_BearingOpenRange(int a, int b)
{
    int lo = 0, hi = rb_n;

    /* the ranges are sorted and apart: find the first one that ends at or after a */
    while (lo < hi)
    {
        int mid = (lo + hi) >> 1;

        if (rb_hi[mid] < a)
            lo = mid + 1;
        else
            hi = mid;
    }
    if (lo == rb_n)
        return true;
    if (rb_lo[lo] > a)
        return true;
    return rb_hi[lo] < b;
}

/* cover a to b (inclusive, 0..RB_FINE-1, a <= b), joining the ranges it touches */
static void R_BearingCover(int a, int b)
{
    int i, j, k, n = rb_n;

    {
        int lo = 0, hi = n;

        while (lo < hi)     /* the first range that is not clearly left of a */
        {
            int mid = (lo + hi) >> 1;

            if (rb_hi[mid] + RB_GAP < a)
                lo = mid + 1;
            else
                hi = mid;
        }
        i = lo;
    }
    if (i == n || rb_lo[i] > b + RB_GAP)
    {
        if (n >= RB_MAXR)
            return;     /* full: nothing is lost, only less is hidden */
        for (j = n; j > i; j--)
        {
            rb_lo[j] = rb_lo[j - 1];
            rb_hi[j] = rb_hi[j - 1];
        }
        rb_lo[i] = a;
        rb_hi[i] = b;
        rb_n = n + 1;
        return;
    }

    if (rb_lo[i] < a)
        a = rb_lo[i];
    j = i;
    if (rb_hi[j] > b)
        b = rb_hi[j];
    while (j + 1 < n && rb_lo[j + 1] <= b + RB_GAP)
    {
        j++;
        if (rb_hi[j] > b)
            b = rb_hi[j];
    }
    rb_lo[i] = a;
    rb_hi[i] = b;
    k = j - i;
    if (k > 0)
    {
        for (j = i + 1 + k; j < n; j++)
        {
            rb_lo[j - k] = rb_lo[j];
            rb_hi[j - k] = rb_hi[j];
        }
        rb_n = n - k;
    }
}

/*
 * Can anything between the bearings s1 (left) and s2 (right) still be seen? It looks a hair inside the range,
 * or one 0.18 degree step further out on both sides while the last frame showed sky (colmargin, as R_ColsOpen).
 */
static boolean R_BearingOpen(int s1, int s2)
{
    int d = (s2 - s1) & (RB_FINE - 1);
    int m = colmargin ? 16 : 0;
    int a, b;

    if (d >= RB_FINE / 2)
        return true;    /* should not happen, be safe */

    a = s1 + RB_EPS - m;
    b = s1 + d - RB_EPS + m;
    if (a > b)
    {
        a = s1 + (d >> 1) - m;
        b = s1 + (d >> 1) + m;
    }
    if (a < 0)
    {
        a += RB_FINE;
        b += RB_FINE;
    }
    else if (a >= RB_FINE)
    {
        a -= RB_FINE;
        b -= RB_FINE;
    }
    if (b < RB_FINE)
        return R_BearingOpenRange(a, b);
    return R_BearingOpenRange(a, RB_FINE - 1) || R_BearingOpenRange(0, b - RB_FINE);
}

/*
 * Cover the bearings of a wall, from its left end to its right end. Returns false when the wall spans half
 * the circle or more, which can only be a rounding problem for a wall right next to the eye, and nothing may
 * be covered then.
 */
static boolean R_BearingMark(int s1, int s2)
{
    int d = (s2 - s1) & (RB_FINE - 1);
    int b = s1 + d;

    if (d >= RB_FINE / 2)
        return false;
    if (b < RB_FINE)
    {
        R_BearingCover(s1, b);
        return true;
    }
    R_BearingCover(s1, RB_FINE - 1);
    R_BearingCover(0, b - RB_FINE);
    return true;
}

/*
 * A box of the BSP while the view is tilted. 0: hidden, behind walls already in front of it (by bearing, the
 * same for every height) or, tested in the view, nothing of it is on screen and no wall in it can hide anything.
 * 1: open and on screen, draw it. 2: open but off screen (above or below the tilted view). Such a box is not drawn,
 * but its walls are still looked at for what they hide, because in the level view a wall hides everything behind
 * it in its direction whether or not the wall itself is on screen; leaving them out let things show through
 * them (popping in and out as the view was tilted). occ: the box is inside one that is already off screen,
 * so only the bearing matters and 2 is returned when it is open.
 */
static int R_BoxPitch(const fixed_t bspcoord[4], boolean occ)
{
    fixed_t x[2], y[2];
    fixed_t sx[2], cx[2], sy[2], cy[2];
    int xy[4][2];
    int i, boxx, boxy, boxpos;
    int i1, j1, i2, j2, s1, s2;
    boolean op;

    /* standing inside the box */
    if (viewx >= bspcoord[BOXLEFT] && viewx <= bspcoord[BOXRIGHT] &&
        viewy >= bspcoord[BOXBOTTOM] && viewy <= bspcoord[BOXTOP])
    {
        if (!occ)
        {
            rp_lr1 = 0;
            rp_lr2 = RB_FINE - 1;
        }
        return occ ? 2 : 1;
    }

    /* the walls are drawn at whole units */
    x[0] = bspcoord[BOXLEFT]  & R_DRAWNMASK;
    x[1] = bspcoord[BOXRIGHT] & R_DRAWNMASK;
    y[0] = bspcoord[BOXBOTTOM] & R_DRAWNMASK;
    y[1] = bspcoord[BOXTOP]    & R_DRAWNMASK;

    /* the two corners that bound the box as seen from the eye, as the level view test picks them */
    boxx = (viewx < bspcoord[BOXLEFT]) ? 0 : ((viewx <= bspcoord[BOXRIGHT]) ? 1 : 2);
    boxy = (viewy > bspcoord[BOXTOP]) ? 0 : ((viewy >= bspcoord[BOXBOTTOM]) ? 1 : 2);
    boxpos = (boxy << 2) + boxx;
    if (boxpos == 5)
    {
        if (!occ)
        {
            rp_lr1 = 0;
            rp_lr2 = RB_FINE - 1;       /* everything: a box that is looked at from inside */
        }
        return occ ? 2 : 1;
    }

    i1 = (checkcoord[boxpos][0] == BOXRIGHT);
    j1 = (checkcoord[boxpos][1] == BOXTOP);
    i2 = (checkcoord[boxpos][2] == BOXRIGHT);
    j2 = (checkcoord[boxpos][3] == BOXTOP);

    /* the corners in view space: lateral = sin*dx - cos*dy, forward = cos*dx + sin*dy. Only the products of the
       two corners the bearing needs are made now (a hidden box never needs the others) */
    {
        fixed_t d;

        d = x[i1] - viewx;  sx[i1] = FixedMul(viewsin, d);  cx[i1] = FixedMul(viewcos, d);
        d = y[j1] - viewy;  sy[j1] = FixedMul(viewsin, d);  cy[j1] = FixedMul(viewcos, d);
        if (i2 != i1)
        {
            d = x[i2] - viewx;  sx[i2] = FixedMul(viewsin, d);  cx[i2] = FixedMul(viewcos, d);
        }
        if (j2 != j1)
        {
            d = y[j2] - viewy;  sy[j2] = FixedMul(viewsin, d);  cy[j2] = FixedMul(viewcos, d);
        }
    }

    s1 = R_BearingAngle(sx[i1] - cy[j1], cx[i1] + sy[j1]);
    s2 = R_BearingAngle(sx[i2] - cy[j2], cx[i2] + sy[j2]);
    op = R_BearingOpen(s1, s2);
    if (!op)
        return 0;
    if (occ)
        return 2;
    rp_lr1 = s1;
    rp_lr2 = s2;

    /* is any of it on screen: the products of the corners not made yet */
    for (i = 0; i < 2; i++)
    {
        fixed_t dx = x[i] - viewx, dy = y[i] - viewy;

        if (i != i1 && i != i2)
        {
            sx[i] = FixedMul(viewsin, dx);
            cx[i] = FixedMul(viewcos, dx);
        }
        if (i != j1 && i != j2)
        {
            sy[i] = FixedMul(viewsin, dy);
            cy[i] = FixedMul(viewcos, dy);
        }
    }
    for (i = 0; i < 4; i++)
    {
        i1 = (i == 1 || i == 2);
        j1 = (i >= 2);
        xy[i][0] = sx[i1] - cy[j1];
        xy[i][1] = cx[i1] + sy[j1];
    }

    /* heights of the part of the level in this box: bspcoord points into nodes[n].bbox[side] */
    {
        fixed_t lo = rp_boxlo, hi = rp_boxhi;

        if (rp_nodeh)
        {
            lo = rp_nodeh[rp_cn * 4 + rp_cs * 2]     - viewz;
            hi = rp_nodeh[rp_cn * 4 + rp_cs * 2 + 1] - viewz;
        }

        return RP_BoxVis(rp_s, rp_c, xy, lo, hi) ? 1 : 2;
    }
}

static boolean R_CheckBBoxPitch(const fixed_t bspcoord[4])
{
    int r = R_BoxPitch(bspcoord, false);

    rp_occbox = (r == 2);
    return r != 0;
}

/* x1,y1 and x2,y2 are the ends of the line in view space (lateral, forward) */
static void R_AddLinePitch(seg_t *line, fixed_t x1, fixed_t y1, fixed_t x2, fixed_t y2)
{
	sector_t *backsector;
	fixed_t   hlo, hhi;
	int       s1, s2, canocclude;

    /* facing test, the same as for a level view: which side of the line the eye is on */
    if ((((long long)x2 * y1) - ((long long)x1 * y2)) <= 0)
        return;

    /* the bearing of each end, worked out once per frame for the walls that share it */
    {
        vertex_t *v = line->v1;

        if (v->bvalid != validcount)
        {
            v->bstep = R_BearingAngle(x1, y1);
            v->bvalid = validcount;
        }
        s1 = v->bstep;

        v = line->v2;
        if (v->bvalid != validcount)
        {
            v->bstep = R_BearingAngle(x2, y2);
            v->bvalid = validcount;
        }
        s2 = v->bstep;
    }

    if (!R_BearingOpen(s1, s2))
        return;     /* hidden behind walls in front of it */

    backsector = line->backsector;

    /* height the wall is drawn over, relative to the eye */
    hlo = frontsector->floorheight - viewz;
    hhi = frontsector->ceilingheight - viewz;

    canocclude = false;
    if (!(line->linedef->flags & (ML_DONTOCCLUDE|ML_DRAWMASKED)))
    {
        if (!backsector ||
            backsector->ceilingheight <= frontsector->floorheight ||
            backsector->floorheight   >= frontsector->ceilingheight ||
            backsector->floorheight   == backsector->ceilingheight)
            canocclude = true;
    }

    if (backsector)
    {
        fixed_t blo = backsector->floorheight - viewz;
        fixed_t bhi = backsector->ceilingheight - viewz;

        if (blo < hlo) hlo = blo;
        if (bhi > hhi) hhi = bhi;
    }

    if (!rp_occ && RP_SegVis(rp_s, rp_c, x1, y1, x2, y2, hlo, hhi))
    {
        line->flags |= 1;
        line->linedef->flags |= ML_MAPPED;

        if (frontsector->ceilingpic == -1 || frontsector->floorpic == -1) /* a sky floor shows it too */
            rendersky = true;
    }

    if (canocclude)
    {
        R_BearingMark(s1, s2);
    }
}

/*
 * Vertical look: can the floor or ceiling of a subsector be on screen? Looking down, the ceilings are
 * above the screen, and looking up the floors are below it, yet each was still built and sent to
 * the RSP just to be cut away. The polygon is outside the screen when all its corners are outside
 * the same side of the view. Always true while the view is level.
 */
boolean R_PlaneVisible(subsector_t *sub, fixed_t planeheight)
{
    const leaf_t *lf;
    fixed_t h;
    int i, all;
    rp_pt_t p;

    if (!rp_active)
        return true;

    h = planeheight - viewz;
    lf = &leafs[sub->leaf];
    all = -1;

    for (i = 0; i < sub->numverts; i++, lf++)
    {
        vertex_t *v = lf->vertex;

        if (v->validcount != validcount)
        {
            /* a corner that no wall of this subsector went through this frame */
            v->vx = FixedMul(viewsin, ((v->x & R_DRAWNMASK) - viewx)) - FixedMul(viewcos, ((v->y & R_DRAWNMASK) - viewy));
            v->vy = FixedMul(viewcos, ((v->x & R_DRAWNMASK) - viewx)) + FixedMul(viewsin, ((v->y & R_DRAWNMASK) - viewy));
            v->validcount = validcount;
        }

        RP_Point(&p, rp_s, rp_c, v->vx, v->vy, h);
        all &= p.out;
        if (!all)
            return true;
    }

    return false;
}

/*
 * Vertical look: can this piece of a wall (an upper, lower or middle part between two heights, in
 * whole units) be on screen? The whole wall is on screen as far as the sides go, but when the view
 * is tilted the piece itself may be above or below the screen. Always true while the view is level.
 */
boolean R_WallPieceVisible(seg_t *seg, int top, int bottom)
{
    if (!rp_active)
        return true;

    return RP_SegVis(rp_s, rp_c, seg->v1->vx, seg->v1->vy, seg->v2->vx, seg->v2->vy,
                     (bottom << FRACBITS) - viewz, (top << FRACBITS) - viewz);
}

//
// Kick off the rendering process by initializing the solidsubsectors array and then
// starting the BSP traversal.
//

void R_BSP(void) // 80023F30
{
    int count;
    subsector_t **sub;

    validcount++;
    colmargin = rendersky ? 1 : 0; /* rendersky still holds the last frame's value here */
    rendersky = false;

    numdrawsubsectors = 0;
    numdrawvissprites = 0;

    visspritehead = vissprites;

	endsubsector = solidsubsectors; /* Init the free memory pointer */
	bzero(solidcols, 320);

    /* Vertical look: tilted view needs the pitch aware tests */
    rp_active = false;
    if (cameratarget == viewplayer->mo && viewplayer->lookpitch != 0)
    {
        angle_t pa = (angle_t)viewplayer->recoilpitch + viewplayer->lookpitch;
        rp_s = finesine[pa >> ANGLETOFINESHIFT];
        rp_c = finecosine[pa >> ANGLETOFINESHIFT];

        /* per node heights and the level wide range (cached until a plane moves) */
        R_PitchNodeHeights();
        rp_boxlo = rp_globallo - viewz;
        rp_boxhi = rp_globalhi - viewz;
        rp_active = true;

        /* nothing hides anything yet, except what is outside the widest direction the screen
           can show: the corner of the screen has the direction atan(1 / (cos - 0.75 sin)) from
           straight ahead, and everything when that is not positive (looking up or down steeply) */
        if (!rb_atan_ready)
            R_BearingInit();
        rb_n = 0;
        rp_ndef = 0;
        {
            fixed_t den = rp_c - (((rp_s < 0) ? -rp_s : rp_s) >> 1) - (((rp_s < 0) ? -rp_s : rp_s) >> 2);

            if (den > 64)
            {
                int w = (int)(R_PointToAngle2(0, 0, den, 65536) >> 21) + 8; /* + about 1.4 degrees */

                if (w < 1024)
                {
                    R_BearingCover(0, (1024 - w) * 16);
                    R_BearingCover((1024 + w) * 16, RB_FINE - 1);
                }
            }
        }
    }

    if (camviewpitch == 0)
    {
        R_RenderBSPNode(firstnode);  /* Begin traversing the BSP tree for all walls in render range */
    }
    else
    {
        R_RenderBSPNodeNoClip(firstnode);  /* Begin traversing the BSP tree for all walls in render range */
        rendersky = true;
    }


    sub = solidsubsectors;
    count = numdrawsubsectors;
    while(count)
    {
        R_AddSprite(*sub);	/* Render each sprite */
        sub++;				/* Inc the sprite pointer */
        count--;
    }
}

static void R_RenderBSPNodeOcc(int bspnum);

/*
 * An off screen box that is open is not walked at once: what is in it only matters for the things drawn after
 * it that lie in the directions it covers, and most of the time there are none. It is put on a list, and walked
 * (R_ResolveDeferred) when a subsector is about to be drawn whose directions touch it. What is drawn is the same
 * as when every such box is walked at once, because the walls in a box can only hide what is in directions
 * the box covers, and the subsectors it hides things from come after it in the walk either way.
 */
static void R_DeferOcc(int child)
{
#ifdef RP_OCC_IMMEDIATE
    R_RenderBSPNodeOcc(child);
#else
    if (rp_ndef >= RP_MAXDEFER)
    {
        R_RenderBSPNodeOcc(child);  /* list is full: do it the plain way */
        return;
    }
    rp_defchild[rp_ndef] = child;
    rp_defr1[rp_ndef] = rp_lr1;
    rp_defr2[rp_ndef] = rp_lr2;
    rp_defdone[rp_ndef] = false;
    rp_ndef++;
#endif
}

/* do the directions a1..a2 and b1..b2 (left to right, each less than half a turn) come within a hair of each other */
static INLINE_ALWAYS boolean R_BearingsTouch(int a1, int a2, int b1, int b2)
{
    int m = RB_GAP + RB_EPS + 8;
    int da = ((a2 - a1) & (RB_FINE - 1)) + 2 * m;
    int db = (b2 - b1) & (RB_FINE - 1);

    return (((b1 - (a1 - m)) & (RB_FINE - 1)) <= da) || (((a1 - m - b1) & (RB_FINE - 1)) <= db);
}

/* about to draw the subsector whose box covers rp_lr1..rp_lr2: false when the boxes walked for it now hide it */
static boolean R_ResolveDeferred(void)
{
    int i, r1 = rp_lr1, r2 = rp_lr2;
    boolean walked = false;

    for (i = 0; i < rp_ndef; i++)
    {
        if (rp_defdone[i])
            continue;
        if (R_BearingsTouch(rp_defr1[i], rp_defr2[i], r1, r2))
        {
            rp_defdone[i] = true;
            R_RenderBSPNodeOcc(rp_defchild[i]);
            walked = true;
        }
    }
    return !walked || R_BearingOpen(r1, r2);
}

static boolean R_RenderBspSubsector(int bspnum)
{
	// Found a subsector?
	if (bspnum & NF_SUBSECTOR) {
		if (rp_active && rp_ndef && !R_ResolveDeferred())
			return true;
		if (bspnum == -1)
			R_Subsector(0);
		else
			R_Subsector(bspnum & (~NF_SUBSECTOR));
		return true;
	}
	return false;
}

//
// JNMartin's Non-recursive descension through the BSP, classifying nodes according to the
// player's point of view, and render subsectors in view.
//

/*
 * Walk a part of the BSP that is off screen in the tilted view, front to back like the normal walk, only to let
 * its walls hide what is behind them (see R_BoxPitch). Nothing in it is drawn. Recursion is as deep as the tree.
 */
static void R_RenderBSPNodeOcc(int bspnum)
{
	const node_t *bsp;
	subsector_t *sub;
	seg_t *line;
	int count, left, right, side;

	if (bspnum & NF_SUBSECTOR)
	{
		sub = &subsectors[(bspnum == -1) ? 0 : (bspnum & (~NF_SUBSECTOR))];
		frontsector = sub->sector;
		line = &segs[sub->firstline];
		count = sub->numlines;
		rp_occ = true;
		do
		{
			R_AddLine(line);
			++line;
		} while (--count);
		rp_occ = false;
		return;
	}

	bsp = &nodes[bspnum];
	left = (bsp->line.dy >> FRACBITS) * ((viewx - bsp->line.x) >> FRACBITS);
	right = ((viewy - bsp->line.y) >> FRACBITS) * (bsp->line.dx >> FRACBITS);
	side = (right < left) ? 0 : 1;

	if (R_BoxPitch(bsp->bbox[side], true))
		R_RenderBSPNodeOcc(bsp->children[side]);
	if (R_BoxPitch(bsp->bbox[side ^ 1], true))
		R_RenderBSPNodeOcc(bsp->children[side ^ 1]);
}

#define MAX_BSP_DEPTH 256
static int stack[MAX_BSP_DEPTH];
void R_RenderBSPNode(int bspnum)
{
	const node_t *bsp;
	int side = 0;
	int sp = 0;
	int left;
	int right;
	fixed_t dx;
	fixed_t dy;
	while (true) {
		// Front sides.
		while (!R_RenderBspSubsector(bspnum)) {
			if (sp == MAX_BSP_DEPTH)
				break;
			bsp = &nodes[bspnum];
			dx = (viewx - bsp->line.x);
			dy = (viewy - bsp->line.y);
			left = (bsp->line.dy >> FRACBITS) * (dx >> FRACBITS);
			right = (dy >> FRACBITS) * (bsp->line.dx >> FRACBITS);
			if (right < left)
				side = 0;
			else
				side = 1;
			stack[sp++] = bspnum;
			stack[sp++] = side;
			rp_cn = bspnum;
			rp_cs = side;
			bspnum = bsp->children[side];
			if (!R_CheckBBox(bsp->bbox[side])) {
				break;
			}
			if (rp_occbox) {
				R_DeferOcc(bspnum);
				break;
			}
		}
		if (sp == 0) {
			// back at root node and not visible. All done!
			return;
		}
		// Back sides.
		side = stack[--sp];
		bspnum = stack[--sp];
		bsp = &nodes[bspnum];
		// Possibly divide back space.
		// Walk back up the tree until we find
		// a node that has a visible backspace.
		for (;;)
		{
			rp_cn = bspnum;
			rp_cs = side ^ 1;
			if (R_CheckBBox(bsp->bbox[side ^ 1]))
			{
				if (!rp_occbox)
					break;
				R_DeferOcc(bsp->children[side ^ 1]);
			}
			if (sp == 0)
			{
				// back at root node and not visible. All done!
				return;
			}
			// Back side next.
			side = stack[--sp];
			bspnum = stack[--sp];
			bsp = &nodes[bspnum];
		}
		bspnum = bsp->children[side ^ 1];
	}
}

//
// Checks BSP node/subtree bounding box. Returns true if some part of the bbox
// might be visible.
//

boolean R_CheckBBox(const fixed_t bspcoord[4]) // 80024170
{
	byte boxx;
	byte boxy;
	byte boxpos;

	fixed_t x1, y1, x2, y2;
	int vx1, vy1, vx2, vy2, delta;
	int Xstart, Xend;

	rp_occbox = false;
	if (rp_active)
	{
		return R_CheckBBoxPitch(bspcoord);
	}

	// find the corners of the box that define the edges from current viewpoint
	if (viewx < bspcoord[BOXLEFT])
		boxx = 0;
	else if (viewx <= bspcoord[BOXRIGHT])
		boxx = 1;
	else
		boxx = 2;

	if (viewy > bspcoord[BOXTOP])
		boxy = 0;
	else if (viewy >= bspcoord[BOXBOTTOM])
		boxy = 1;
	else
		boxy = 2;

	boxpos = (boxy << 2) + boxx;
	if (boxpos == 5)
		return true;

	x1 = bspcoord[checkcoord[boxpos][0]];
	y1 = bspcoord[checkcoord[boxpos][1]];
	x2 = bspcoord[checkcoord[boxpos][2]];
	y2 = bspcoord[checkcoord[boxpos][3]];

	/* the walls are drawn at whole units, so test the box where they are drawn */
	x1 &= R_DRAWNMASK;
	y1 &= R_DRAWNMASK;
	x2 &= R_DRAWNMASK;
	y2 &= R_DRAWNMASK;

    vx1 = FixedMul(viewsin, x1 - viewx) - FixedMul(viewcos, y1 - viewy);
    vy1 = FixedMul(viewcos, x1 - viewx) + FixedMul(viewsin, y1 - viewy);
    vx2 = FixedMul(viewsin, x2 - viewx) - FixedMul(viewcos, y2 - viewy);
    vy2 = FixedMul(viewcos, x2 - viewx) + FixedMul(viewsin, y2 - viewy);

    if ((vx1 < -vy1) && (vx2 < -vy2))
        return false;

    if ((vy1 < vx1) && (vy2 < vx2))
        return false;

    if ((((vx2 >> FRACBITS) * (vy1 >> FRACBITS)) - ((vx1 >> FRACBITS) * (vy2 >> FRACBITS))) < 2)
        return true;

    if ((vy1 <= 0) && (vy2 <= 0))
        return false;

    if (vx1 < -vy1)
    {
        delta = (vx1 + vy1);
        delta = FixedDiv2(delta, ((delta - vx2) - vy2));
        delta = FixedMul(delta, (vy2 - vy1));

        vy1 += delta;
        vx1 = -vy1;
    }

    if (vy2 < vx2)
    {
        delta = (vx1 - vy1);
        delta = FixedDiv2(delta, ((delta - vx2) + vy2));
        delta = FixedMul(delta, (vy2 - vy1));
        vx2 = delta + vy1;
        vy2 = vx2;
    }
	
	Xstart = R_ScreenCol(vx1, vy1);
	Xend   = R_ScreenCol(vx2, vy2);

    return R_ColsOpen(Xstart, Xend);
}

//
// Determine floor/ceiling planes, add sprites of things in sector,
// draw one or more segments.
//

void R_Subsector(int num) // 8002451C
{
	subsector_t *sub;
	seg_t       *line;
	int          count;

	if (num >= numsubsectors)
	{
		I_Error("R_Subsector: ss %i with numss = %i", num, numsubsectors);
	}

	if (numdrawsubsectors < MAXSUBSECTORS)
	{
	    numdrawsubsectors++;

		sub = &subsectors[num];
		sub->drawindex = numdrawsubsectors;

		*endsubsector = sub;//copy subsector
		endsubsector++;

		frontsector = sub->sector;

		/* tilted view: a sky overhead can be seen with no wall in view at all */
		if (rp_active && (frontsector->ceilingpic == -1 || frontsector->floorpic == -1)) /* or a sky floor below */
			rendersky = true;

		line = &segs[sub->firstline];
		count = sub->numlines;
        do
        {
            R_AddLine(line);	/* Render each line */
            ++line;				/* Inc the line pointer */
        } while (--count);		/* All done? */
	}
}

//
// Clips the given segment and adds any visible pieces to the line list.
//

void R_AddLine(seg_t *line) // 80024604
{
	sector_t    *backsector;
	vertex_t    *vrt, *vrt2;
	int         x1, y1, x2, y2;
	int        Xstart, Xend, delta;
	byte        *solid_cols;

	line->flags &= ~1;

	vrt = line->v1;
	if (vrt->validcount != validcount)
	{
        x1 = FixedMul(viewsin, ((vrt->x & R_DRAWNMASK) - viewx)) - FixedMul(viewcos,((vrt->y & R_DRAWNMASK) - viewy));
        y1 = FixedMul(viewcos, ((vrt->x & R_DRAWNMASK) - viewx)) + FixedMul(viewsin,((vrt->y & R_DRAWNMASK) - viewy));

        vrt->vx = x1;
        vrt->vy = y1;

        vrt->validcount = validcount;
	}
	else
	{
        x1 = vrt->vx;
        y1 = vrt->vy;
	}

	vrt2 = line->v2;
	if (vrt2->validcount != validcount)
	{
        x2 = FixedMul(viewsin, ((vrt2->x & R_DRAWNMASK) - viewx)) - FixedMul(viewcos,((vrt2->y & R_DRAWNMASK) - viewy));
        y2 = FixedMul(viewcos, ((vrt2->x & R_DRAWNMASK) - viewx)) + FixedMul(viewsin,((vrt2->y & R_DRAWNMASK) - viewy));

        vrt2->vx = x2;
        vrt2->vy = y2;

        vrt2->validcount = validcount;
	}
	else
	{
        x2 = vrt2->vx;
        y2 = vrt2->vy;
	}

	if (rp_active)
	{
		R_AddLinePitch(line, x1, y1, x2, y2);
		return;
	}

	if ((x1 < -y1) && (x2 < -y2))
        return;

    if ((y1 < x1) && (y2 < x2))
        return;

    if ((y1 < ((8*FRACUNIT)+1)) && (y2 < ((8*FRACUNIT)+1)))
        return;

    /*
     * Facing test. Cutting the coordinates down to whole units first (as the original
     * did) throws away up to a thousand units^2 for a wall far away, more than the
     * whole value for one seen at a shallow angle, so front facing walls were dropped
     * depending on the exact view position: a one pixel crack that flickered as the
     * player moved. A 32x32 to 64 bit multiply keeps all the bits.
     */
    if ((((long long)x2 * y1) - ((long long)x1 * y2)) <= 0)
        return;

    if (y1 < (8*FRACUNIT))
    {
        delta = FixedDiv2(((8*FRACUNIT) - y1), (y2 - y1));
        delta = FixedMul(delta, (x2 - x1));
        x1 += delta;
        y1 = (8*FRACUNIT);
    }
    else if (y2 < (8*FRACUNIT))
    {
        delta = FixedDiv2(((8*FRACUNIT) - y2), (y1 - y2));
        delta = FixedMul(delta, (x1 - x2));
        x2 += delta;
        y2 = (8*FRACUNIT);
    }

	Xstart = R_ScreenCol(x1, y1);
	Xend   = R_ScreenCol(x2, y2);

    if (Xstart < 0)
        Xstart = 0;

    if (Xend >= 320)
        Xend = 320;

    /* A line inside a single column is kept too, it can still cover a pixel centre */
    if (Xstart <= Xend)
    {
        if (R_ColsOpen(Xstart, Xend))
        {
            line->flags |= 1;
            line->linedef->flags |= ML_MAPPED;
        }

        if (frontsector->ceilingpic == -1 || frontsector->floorpic == -1) {
            rendersky = true;
        }

        if (!(line->linedef->flags & (ML_DONTOCCLUDE|ML_DRAWMASKED)))
        {
            backsector = line->backsector;

            if(!backsector ||
                backsector->ceilingheight <= frontsector->floorheight ||
                backsector->floorheight   >= frontsector->ceilingheight ||
                backsector->floorheight   == backsector->ceilingheight) // New line on Doom 64
            {
                solid_cols = &solidcols[Xstart];
                while (Xstart < Xend)
                {
                    *solid_cols = 1;
                    solid_cols++;
                    Xstart += 1;
                }
            }
        }
    }
}

void R_AddSprite(subsector_t *sub) // 80024A98
{
    spriteN64_t *data;
    mobj_t *thing;
    spritedef_t		*sprdef;
	spriteframe_t	*sprframe;

	subsector_t     *pSub;
	subsector_t     *CurSub;
    vissprite_t     *VisSrpCur, *VisSrpCurTmp;
    vissprite_t     *VisSrpNew;

	angle_t         ang;
	unsigned int    rot;
	boolean         flip;
	int             lump;
	fixed_t         tx, tz, tzp;
	fixed_t         x, y;

    sub->vissprite = NULL;

    for (thing = sub->sector->thinglist; thing; thing = thing->snext)
    {
        if (thing->subsector != sub)
			continue;

        if (numdrawvissprites >= MAXVISSPRITES) // Limit the number of sprites that can be rendered
            break;

        if (thing->flags & MF_RENDERLASER)
        {
            visspritehead->zdistance = MAXINT;
            visspritehead->thing = thing;
            visspritehead->next = sub->vissprite;
            sub->vissprite = visspritehead;

            visspritehead++;
            numdrawvissprites++;
        }
        else
        {
            // transform origin relative to viewpoint
            x = (thing->x - viewx) >> FRACBITS;
            y = (thing->y - viewy) >> FRACBITS;
            tx = ((viewsin * x) - (viewcos * y)) >> FRACBITS;
            tz = ((viewcos * x) + (viewsin * y)) >> FRACBITS;

            tzp = tz;
            if (rp_active)
            {
                /* the view looks out of the player, so its own sprite is never drawn. With a camera
                   action the camera is another thing and the player is drawn as normal. */
                if (thing == cameratarget)
                    continue;

                /* tilted view: depth along the tilted view of the bottom and the top of the thing */
                int h0 = (thing->z - viewz) >> FRACBITS;
                int h1 = h0 + (thing->height >> FRACBITS);
                int z0 = (int)((((long long)rp_c * tz) + ((long long)rp_s * h0)) >> 16);
                int z1 = (int)((((long long)rp_c * tz) + ((long long)rp_s * h1)) >> 16);

                tzp = (z0 > z1) ? z0 : z1;
            }

            // thing is behind view plane?
            if (tzp < MINZ)
                continue;
			
			// [GBA Doom] Too far away. Always draw Cyberdemon. They are big sprites!
			if((tz > SPRMAXZ) && (thing->type != MT_CYBORG) && (thing->type != MT_CYBORG_TITLE))
                continue;

            // too far off the side?
            if (tx > (tzp << 1) || tx < -(tzp << 1))
                continue;
            
            
            sprdef = &sprites[thing->sprite];
            sprframe = &sprdef->spriteframes[thing->frame & FF_FRAMEMASK];

            if (sprframe->rotate != 0)
            {
                ang = R_PointToAngle2(viewx, viewy, thing->x, thing->y);
                rot = ((ang - thing->angle) + ((unsigned int)(ANG45 / 2) * 9)) >> 29;
                lump = sprframe->lump[rot];
                flip = (boolean)(sprframe->flip[rot]);
            }
            else
            {
                lump = sprframe->lump[0];
                flip = (boolean)(sprframe->flip[0]);
            }

            visspritehead->zdistance = tz;
            visspritehead->thing = thing;
            visspritehead->lump = lump;
            visspritehead->flip = flip;
            visspritehead->next = NULL;
            visspritehead->sector = sub->sector;

            data = W_CacheLumpNum(lump, PU_CACHE);

            CurSub = sub;
            if (tz < MAXZ)
            {
                if (thing->flags & (MF_CORPSE|MF_SHOOTABLE))
                {
                    x = ((data->width >> 1) * viewsin);
                    y = ((data->width >> 1) * viewcos);

                    pSub = R_PointInSubsector((thing->x - x), (thing->y + y));
                    if ((pSub->drawindex) && (pSub->drawindex < sub->drawindex)) {
                        CurSub = pSub;
                    }

                    pSub = R_PointInSubsector((thing->x + x), (thing->y - y));
                    if ((pSub->drawindex) && (pSub->drawindex < CurSub->drawindex)) {
                        CurSub = pSub;
                    }
                }
            }

            VisSrpCur = CurSub->vissprite;
            VisSrpNew = NULL;

            if (VisSrpCur)
            {
                VisSrpCurTmp = VisSrpCur;
                while ((VisSrpCur = VisSrpCurTmp, tz < VisSrpCur->zdistance))
                {
                    VisSrpCur = VisSrpCurTmp->next;
                    VisSrpNew = VisSrpCurTmp;

                    if (VisSrpCur == NULL)
                        break;

                    VisSrpCurTmp = VisSrpCur;
                }
            }

            if (VisSrpNew)
            {
                VisSrpNew->next = visspritehead;
            }
            else
            {
                CurSub->vissprite = visspritehead;
            }
            visspritehead->next = VisSrpCur;

            numdrawvissprites++;
            visspritehead++;
        }
    }
}

void R_RenderBSPNodeNoClip(int bspnum) // 80024E64
{
	subsector_t *sub;
	seg_t       *line;
	int          count;
	node_t      *bsp;
	int          side;
	fixed_t	     dx, dy;
	fixed_t	     left, right;

    while(!(bspnum & NF_SUBSECTOR))
    {
        bsp = &nodes[bspnum];

        // Decide which side the view point is on.
        //side = R_PointOnSide(viewx, viewy, bsp);
        dx = (viewx - bsp->line.x);
        dy = (viewy - bsp->line.y);

        left = (bsp->line.dy >> FRACBITS) * (dx >> FRACBITS);
        right = (dy >> FRACBITS) * (bsp->line.dx >> FRACBITS);

        if (right < left)
            side = 1;		/* back side */
        else
            side = 0;		/* front side */

        R_RenderBSPNodeNoClip(bsp->children[side ^ 1]);

        bspnum = bsp->children[side];
    }

    // subsector with contents
    // add all the drawable elements in the subsector

    numdrawsubsectors++;

    sub = &subsectors[bspnum & ~NF_SUBSECTOR];
    sub->drawindex = numdrawsubsectors;

    *endsubsector = sub;//copy subsector
    endsubsector++;

    frontsector = sub->sector;

    line = &segs[sub->firstline];
    count = sub->numlines;
    do
    {
        line->flags |= 1;	/* Render each line */
        ++line;				/* Inc the line pointer */
    } while (--count);		/* All done? */
}
