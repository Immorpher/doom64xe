
//Renderer phase 1 - BSP traversal

#include "doomdef.h"
#include "r_local.h"

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

void	R_RenderBSPNode(int bspnum) HOT;
boolean R_CheckBBox(const fixed_t bspcoord[4]) HOT;
void	R_Subsector(int num) HOT;
void	R_AddLine(seg_t *line) HOT;
void    R_AddSprite(subsector_t *sub) HOT;
void    R_RenderBSPNodeNoClip(int bspnum) HOT;

//
// Kick off the rendering process by initializing the solidsubsectors array and then
// starting the BSP traversal.
//

void R_BSP(void) // 80023F30
{
    int count;
    subsector_t **sub;

    validcount++;
    rendersky = false;

    numdrawsubsectors = 0;
    numdrawvissprites = 0;

    visspritehead = vissprites;

	endsubsector = solidsubsectors; /* Init the free memory pointer */
	bzero(solidcols, 320);

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

static boolean R_RenderBspSubsector(int bspnum)
{
	// Found a subsector?
	if (bspnum & NF_SUBSECTOR) {
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
			bspnum = bsp->children[side];
			if (!R_CheckBBox(bsp->bbox[side])) {
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
		while (!R_CheckBBox(bsp->bbox[side ^ 1]))
		{
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
	byte *solid_cols;
	int vx1, vy1, vx2, vy2, delta;
	int Xstart, Xend;

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
	
	fixed_t vxovery1 = FixedDiv2(vx1, vy1);
	fixed_t vxovery2 = FixedDiv2(vx2, vy2);

	Xstart = (((vxovery1 >> 9) + (vxovery1 >> 11))) + 160;
	Xend   = (((vxovery2 >> 9) + (vxovery2 >> 11))) + 160;

    if (Xstart < 0)
        Xstart = 0;

    if (Xend >= 320)
        Xend = 320;

    solid_cols = &solidcols[Xstart];
    while (Xstart < Xend)
    {
        if (*solid_cols == 0)
            return true;
        solid_cols++;
        Xstart++;
    }

    return false;
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

	if (numdrawsubsectors < 1024)
	{
	    numdrawsubsectors++;

		sub = &subsectors[num];
		sub->drawindex = numdrawsubsectors;

		*endsubsector = sub;//copy subsector
		endsubsector++;

		frontsector = sub->sector;

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
	int         x1, y1, x2, y2, count;
	int        Xstart, Xend, delta;
	byte        *solid_cols;

	line->flags &= ~1;

	vrt = line->v1;
	if (vrt->validcount != validcount)
	{
        x1 = FixedMul(viewsin, (vrt->x - viewx)) - FixedMul(viewcos,(vrt->y - viewy));
        y1 = FixedMul(viewcos, (vrt->x - viewx)) + FixedMul(viewsin,(vrt->y - viewy));

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
        x2 = FixedMul(viewsin, (vrt2->x - viewx)) - FixedMul(viewcos,(vrt2->y - viewy));
        y2 = FixedMul(viewcos, (vrt2->x - viewx)) + FixedMul(viewsin,(vrt2->y - viewy));

        vrt2->vx = x2;
        vrt2->vy = y2;

        vrt2->validcount = validcount;
	}
	else
	{
        x2 = vrt2->vx;
        y2 = vrt2->vy;
	}

	if ((x1 < -y1) && (x2 < -y2))
        return;

    if ((y1 < x1) && (y2 < x2))
        return;

    if ((y1 < ((8*FRACUNIT)+1)) && (y2 < ((8*FRACUNIT)+1)))
        return;

    if ((((x2 >> FRACBITS) * (y1 >> FRACBITS)) - ((x1 >> FRACBITS) * (y2 >> FRACBITS))) <= 0)
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

	fixed_t xovery1 = FixedDiv2(x1, y1);
	fixed_t xovery2 = FixedDiv2(x2, y2);

	Xstart = (((xovery1 >> 9) + (xovery1 >> 11))) + 160;
	Xend   = (((xovery2 >> 9) + (xovery2 >> 11))) + 160;

    if (Xstart < 0)
        Xstart = 0;

    if (Xend >= 320)
        Xend = 320;

    if (Xstart != Xend)
    {
        solid_cols = &solidcols[Xstart];
        count = Xstart;
        while (count < Xend)
        {
            if (*solid_cols == 0)
            {
                line->flags |= 1;
                line->linedef->flags |= ML_MAPPED;
                break;
            }
            solid_cols++;
            count++;
        }

        if (frontsector->ceilingpic == -1) {
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
    byte *data;
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
	fixed_t         tx, tz;
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

            // thing is behind view plane?
            if (tz < MINZ)
                continue;
			
			// [GBA Doom] Too far away. Always draw Cyberdemon. They are big sprites!
			if((tz > SPRMAXZ) && (thing->type != MT_CYBORG) && (thing->type != MT_CYBORG_TITLE))
                continue;

            // too far off the side?
            if (tx > (tz << 1) || tx < -(tz << 1))
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

            data = (byte *)W_CacheLumpNum(lump, PU_CACHE);

            CurSub = sub;
            if (tz < MAXZ)
            {
                if (thing->flags & (MF_CORPSE|MF_SHOOTABLE))
                {
                    x = ((((spriteN64_t*)data)->width >> 1) * viewsin);
                    y = ((((spriteN64_t*)data)->width >> 1) * viewcos);

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
