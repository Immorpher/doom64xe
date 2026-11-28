
//Renderer phase 3 - World Rendering Routines

#include "doomdef.h"
#include "r_local.h"
#include "r_flash.h"

/* Lightning brightens sky exposed sectors by moving their colors toward white (like the
   weapon flashes, so it multiplies with the texture instead of adding to it). LightningFlash
   runs 0..30 at full Flash Level and already follows that option. The sky itself is untouched. */
#define FLASH_WALL_SPLIT 0 // 1: walls near a flash get an extra vertex at the nearest point (can warp textures); 0: corner lighting only
#define FLASH_MIN_DEPTH 16 // walls nearer than this (map units, along the view direction) get no extra flash vertex
#define R_DRAWNMASK ((fixed_t)0xffff0000) // same rounding the culling uses
#define LIGHTNING_COLOR_BOOST 3 /* LightningFlash * this = how far toward white, out of 256 */
static int lightning_k; /* current sector's boost, 0 = none */
#define R_LIT(c) (lightning_k ? R_LightenColor((c), lightning_k) : (c))

//-----------------------------------//
void R_RenderWorld(subsector_t *sub) HOT;

void R_WallPrep(seg_t *seg) HOT;
void R_RenderWall(seg_t *seg, int flags, int texture, int topHeight, int bottomHeight, int topOffset, int bottomOffset, int topColor, int bottomColor) HOT;
void R_RenderSwitch(seg_t *seg, int texture, int topOffset, int color) HOT;

void R_RenderPlane(leaf_t *leaf, int numverts, int zpos, int texture, int xpos, int ypos, int color) HOT;

void R_RenderThings(subsector_t *sub) HOT;
void R_RenderLaser(mobj_t *thing) HOT;
void R_RenderPSprites(void) HOT;
//-----------------------------------//


/*
 * Texture load command cache.
 *
 * Loading a texture needs the same 13 commands every time.  Building them is a
 * large part of the CPU time of a frame, so the commands for each (texture, mirror)
 * combination are built once into a small static display list and later uses only
 * emit one gSPDisplayList that calls it.  There is one bank per display list
 * buffer (vid_side), so an entry is never rewritten while the graphics task that
 * was built with it can still be running, and an entry that was used in the frame
 * being built is never replaced (that case falls back to emitting the commands inline).
 * An entry is only reused if it was built for the same lump address, so a purged
 * and reloaded texture is rebuilt.
 */
extern int blanklump;

#define TEXDL_ENTRIES 64
#define TEXDL_CMDS    13

typedef struct
{
    int      key;     /* (texture << 3) | kind, 0 = empty */
    byte     *data;   /* lump address the commands were built for */
    u32      frame;   /* NextFrameIdx of the last use */
    int      pad;
    Gfx      dl[TEXDL_CMDS + 1];
} texdl_t;

static texdl_t texdl[2][TEXDL_ENTRIES];

#define TEXDL_HASH(key) ((((u32)(key)) * 2654435761U) >> 26) /* top 6 bits */

static Gfx *R_TexCmdsWall(Gfx *g, byte *src, int texture, int wshift, int hshift, int cms, int cmt)
{
    // Load Image Data
    gDPSetTextureImage(g++, G_IM_FMT_CI, G_IM_SIZ_16b , 1, src);
    gDPSetTile(g++, G_IM_FMT_CI, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, 0, 0, 0, 0, 0, 0);

    gDPLoadSync(g++);
    gDPLoadBlock(g++, G_TX_LOADTILE, 0, 0,
                 (((1 << wshift) * (1 << hshift)) >> 2) - 1, 0);

    gDPPipeSync(g++);
    gDPSetTile(g++, G_IM_FMT_CI, G_IM_SIZ_4b,
               (((1 << wshift) >> 1) + 7) >> 3, 0, G_TX_RENDERTILE, 0,
               cmt, hshift, 0,
               cms, wshift, 0);

    gDPSetTileSize(g++, G_TX_RENDERTILE, 0, 0,
                   ((1 << wshift) - 1) << 2,
                   ((1 << hshift) - 1) << 2);

    // Load Palette Data
    gDPSetTextureImage(g++, G_IM_FMT_RGBA, G_IM_SIZ_16b , 1,
                       src + (1 << ((wshift + hshift + 31) & 31)) + ((texture & 15) << 5));

    gDPTileSync(g++);
    gDPSetTile(g++, G_IM_FMT_RGBA, G_IM_SIZ_4b, 0, 256, G_TX_LOADTILE, 0, 0, 0, 0, 0, 0, 0);

    gDPLoadSync(g++);
    gDPLoadTLUTCmd(g++, G_TX_LOADTILE, 15);

    gDPPipeSync(g++);

    return g;
}

static Gfx *R_TexCmdsPlane(Gfx *g, byte *src, int texture)
{
    // Load Image Data
    gDPSetTextureImage(g++, G_IM_FMT_CI, G_IM_SIZ_16b , 1, src);
    gDPSetTile(g++, G_IM_FMT_CI, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, 0, 0, 0, 0, 0, 0);

    gDPLoadSync(g++);
    gDPLoadBlock(g++, G_TX_LOADTILE, 0, 0,
                 (((1 << 6) * (1 << 6)) >> 2) - 1, 0);

    gDPPipeSync(g++);
    gDPSetTile(g++, G_IM_FMT_CI, G_IM_SIZ_4b,
               (((1 << 6) >> 1) + 7) >> 3, 0, G_TX_RENDERTILE, 0,
               0, 6, 0,
               0, 6, 0);

    gDPSetTileSize(g++, G_TX_RENDERTILE, 0, 0,
                   ((1 << 6) - 1) << 2,
                   ((1 << 6) - 1) << 2);

    // Load Palette Data
    gDPSetTextureImage(g++, G_IM_FMT_RGBA, G_IM_SIZ_16b , 1,
                       src + (1 << (6 + 6 - 1)) + ((texture & 15) << 5));

    gDPTileSync(g++);
    gDPSetTile(g++, G_IM_FMT_RGBA, G_IM_SIZ_4b, 0, 256, G_TX_LOADTILE, 0, 0, 0, 0, 0, 0, 0);

    gDPLoadSync(g++);
    gDPLoadTLUTCmd(g++, G_TX_LOADTILE, 15);

    gDPPipeSync(g++);

    return g;
}

/* RDP cycle mode currently set for the world: 1 = 1-cycle opaque, 0 = normal 2-cycle */
static int world1c;

static void R_World1C(void)
{
    if (!world1c)
    {
        gDPPipeSync(GFX1++);
        gDPSetCycleType(GFX1++, G_CYC_1CYCLE);
        gDPSetCombineMode(GFX1++, G_CC_D64COMB20, G_CC_D64COMB20);
        gDPSetRenderMode(GFX1++, G_RM_FOG_TEX_EDGE_1C, G_RM_FOG_TEX_EDGE_1C);
        world1c = 1;
    }
}

static void R_World2C(void)
{
    if (world1c)
    {
        gDPPipeSync(GFX1++);
        gDPSetCycleType(GFX1++, G_CYC_2CYCLE);
        gDPSetCombineMode(GFX1++, G_CC_D64COMB07, G_CC_D64COMB08);
        gDPSetRenderMode(GFX1++, G_RM_FOG_SHADE_A, G_RM_TEX_EDGE2);
        world1c = 0;
    }
}

#define R_PlaneVisibleT R_PlaneVisible

void R_RenderAll(void) // 80026590
{
    subsector_t *sub;

    world1c = 0; /* R_Render3D starts the world in 2-cycle mode */

    while (endsubsector--, (endsubsector >= solidsubsectors))
    {
        sub = *endsubsector;
        frontsector = sub->sector;
        R_RenderWorld(sub);

        sub->drawindex = 0x7fff;
    }

    R_World2C(); /* weapon sprites etc. expect 2-cycle */
}

void R_RenderWorld(subsector_t *sub) // 80026638
{
    leaf_t *lf;
    seg_t *seg;

    fixed_t xoffset;
    fixed_t yoffset;
    int numverts;
    int i;
	short extralight = 0; // hold the sector light level and thunder flashes if present

    I_CheckGFX();
	
	// Flash sky exposed sectors when there is lightning
	extralight = frontsector->lightlevel;
	lightning_k = 0;
	if (LightningFlash != 0 && (frontsector->ceilingpic == -1 || frontsector->ceilingpic == 87 || frontsector->ceilingpic == 96 || frontsector->ceilingpic == 64 || frontsector->ceilingpic == 396 || frontsector->ceilingpic == 294 || frontsector->ceilingpic == 320)) {
		lightning_k = LightningFlash * LIGHTNING_COLOR_BOOST; // lightning lights the sector colors
	}


	gDPSetPrimColor(GFX1++, 0, extralight, 0, 0, 0, 255);

	/* 1-cycle is only identical to the 2-cycle combiner when PRIM_LOD_FRAC is 0 */
	if (extralight == 0)
		R_World1C();
	else
		R_World2C();

    numverts = sub->numverts;

    /* */
    /* Render Walls */
    /* */
    lf = &leafs[sub->leaf];
    for (i = 0; i < numverts; i++)
    {
        seg = lf->seg;

        if (seg && (seg->flags & 1))
        {
            R_WallPrep(seg);
        }

        lf++;
    }

    /* */
    /* Render Ceilings */
    /* */
    if ((frontsector->ceilingpic != -1) && (viewz < frontsector->ceilingheight) && ((textures[frontsector->ceilingpic] >> 4) != blanklump) &&
        R_PlaneVisibleT(sub, frontsector->ceilingheight))
    {
        if (frontsector->flags & MS_SCROLLCEILING)
        {
            xoffset = frontsector->xoffset;
            yoffset = frontsector->yoffset;
        }
        else
        {
            xoffset = 0;
            yoffset = 0;
        }

        lf = &leafs[sub->leaf];
        R_RenderPlane(lf, numverts, frontsector->ceilingheight >> FRACBITS,
                        textures[frontsector->ceilingpic],
                        xoffset, yoffset,
                        R_LIT(lights[frontsector->colors[0]].rgba));
    }

    /* */
    /* Render Floors */
    /* */
    if ((frontsector->floorpic != -1) && (frontsector->floorheight < viewz) &&
        ((frontsector->flags & MS_LIQUIDFLOOR) || ((textures[frontsector->floorpic] >> 4) != blanklump)) &&
        R_PlaneVisibleT(sub, frontsector->floorheight))
    {
        if (!(frontsector->flags & MS_LIQUIDFLOOR))
        {
            if (frontsector->flags & MS_SCROLLFLOOR)
            {
                xoffset = frontsector->xoffset;
                yoffset = frontsector->yoffset;
            }
            else
            {
                xoffset = 0;
                yoffset = 0;
            }

            lf = &leafs[sub->leaf];
            R_RenderPlane(lf, numverts, frontsector->floorheight >> FRACBITS,
                            textures[frontsector->floorpic],
                            xoffset, yoffset,
                            R_LIT(lights[frontsector->colors[1]].rgba));
        }
        else
        {
            R_World2C();
            gDPPipeSync(GFX1++);
            gDPSetRenderMode(GFX1++, G_RM_FOG_SHADE_A, G_RM_XLU_SURF2);

            if (frontsector->flags & MS_SCROLLFLOOR)
            {
                xoffset = frontsector->xoffset;
                yoffset = frontsector->yoffset;
            }
            else
            {
                xoffset = scrollfrac;
                yoffset = 0;
            }

            //--------------------------------------------------------------
            lf = &leafs[sub->leaf];
            R_RenderPlane(lf, numverts, frontsector->floorheight >> FRACBITS,
                            textures[frontsector->floorpic + 1],
                            xoffset, yoffset,
                            R_LIT(lights[frontsector->colors[1]].rgba));

            //--------------------------------------------------------------
            gDPSetPrimColor(GFX1++, 0, frontsector->lightlevel, 0, 0, 0, 160);

            lf = &leafs[sub->leaf];
            R_RenderPlane(lf, numverts, frontsector->floorheight >> FRACBITS,
                            textures[frontsector->floorpic],
                            -yoffset, xoffset,
                            R_LIT(lights[frontsector->colors[1]].rgba));

            gDPPipeSync(GFX1++);
            gDPSetRenderMode(GFX1++, G_RM_FOG_SHADE_A, G_RM_TEX_EDGE2);
        }
    }


    /* */
    /* Render Things */
    /* */
    R_RenderThings(sub);
}

void R_WallPrep(seg_t *seg) // 80026A44
{
    sector_t *backsector;
	line_t *li;
	side_t *side;
	fixed_t f_ceilingheight;
	fixed_t f_floorheight;
	fixed_t b_ceilingheight;
	fixed_t b_floorheight;
	fixed_t m_top;
	fixed_t m_bottom;
	fixed_t height;
	fixed_t rowoffs;
	int pic;

	unsigned int r1 = 0, g1 = 0, b1 = 0;
	unsigned int r2 = 0, g2 = 0, b2 = 0;
	float rn = 0, gn = 0, bn = 0;
	float scale = 0;
	unsigned int thingcolor = 0;
	unsigned int upcolor = 0;
	unsigned int lowcolor = 0;
	unsigned int topcolor = 0;
	unsigned int bottomcolor = 0;
	unsigned int tmp_upcolor = 0;
	unsigned int tmp_lowcolor = 0;
	int curRowoffset, sideheight, frontheight;

    li = seg->linedef;
    side = seg->sidedef;

    // [GEC] Prevents errors in textures in T coordinates, but is not applied to switches
    curRowoffset = side->rowoffset & (255 << FRACBITS);
	
    f_ceilingheight = frontsector->ceilingheight >> FRACBITS;
    f_floorheight = frontsector->floorheight >> FRACBITS;

    thingcolor = R_LIT(lights[frontsector->colors[2]].rgba);
    upcolor = R_LIT(lights[frontsector->colors[3]].rgba);
    lowcolor = R_LIT(lights[frontsector->colors[4]].rgba);

    if (li->flags & ML_BLENDING)
    {
        r1 = upcolor  >> 24;
        g1 = upcolor  >> 16 & 0xff;
        b1 = upcolor  >> 8 & 0xff;
        r2 = lowcolor >> 24;
        g2 = lowcolor >> 16 & 0xff;
        b2 = lowcolor >> 8 & 0xff;

        tmp_upcolor = upcolor;
        tmp_lowcolor = lowcolor;
    }
    else
    {
        topcolor = thingcolor;
        bottomcolor = thingcolor;
    }

    m_bottom = f_floorheight; // set middle bottom
    m_top = f_ceilingheight;  // set middle top

	backsector = seg->backsector;
	if (backsector) {
		b_floorheight = backsector->floorheight >> 16;
		b_ceilingheight = backsector->ceilingheight >> 16;

		if ((b_ceilingheight < f_ceilingheight) && (backsector->ceilingpic != -1)) {
			height = f_ceilingheight - b_ceilingheight;

			if (li->flags & ML_DONTPEGTOP) {
				rowoffs = (curRowoffset >> 16) + height;
			} else {
				rowoffs = ((height + 127) & -128) + (curRowoffset >> 16);
			}

			if (li->flags & ML_BLENDING) { // Scale gradient to match upper wall
				frontheight = f_ceilingheight - f_floorheight;
				if (!(li->flags & ML_BLENDFULLTOP) && frontheight != 0) {
					sideheight = b_ceilingheight - f_ceilingheight;

					scale = (float)sideheight / (float)frontheight;

					rn = ((float)r1-(float)r2)*scale + (float)r1;
					gn = ((float)g1-(float)g2)*scale + (float)g1;
					bn = ((float)b1-(float)b2)*scale + (float)b1;

					if (!((rn < 256) && (gn < 256) && (bn < 256))) { // Rescale if out of color bounds
						scale = 255.0f;

						if (rn >= gn && rn >= bn) {
							scale /= rn;
						} else if (gn >= rn && gn >= bn) {
							scale /= gn;
						} else {
							scale /= bn;
						}

						rn *= scale;
						gn *= scale;
						bn *= scale;
					}

					tmp_lowcolor = ((int)rn << 24) | ((int)gn << 16) | ((int)bn << 8) | 0xff;
				} 

				if (li->flags & ML_INVERSEBLEND) {
					bottomcolor = tmp_upcolor;
					topcolor = tmp_lowcolor;
				} else {
					topcolor = tmp_upcolor;
					bottomcolor = tmp_lowcolor;
				}

				// clip middle color upper
				upcolor = tmp_lowcolor;
			}

			R_RenderWall(seg, li->flags, textures[side->toptexture],
						 f_ceilingheight, b_ceilingheight,
						 rowoffs - height, rowoffs,
						 topcolor, bottomcolor);

			m_top = b_ceilingheight; // clip middle top height

			if ((li->flags & (ML_CHECKFLOORHEIGHT|ML_SWITCHX08)) == ML_SWITCHX08) {
				if (SWITCHMASK(li->flags) == ML_SWITCHX04) {
					pic = side->bottomtexture;
					rowoffs = side->rowoffset >> 16;
				} else {
					pic = side->midtexture;
					rowoffs = side->rowoffset >> 16;
				}
				R_RenderSwitch(seg, pic, b_ceilingheight + rowoffs + 48, thingcolor);
			}
		}

		if (f_floorheight < b_floorheight) {
			height = f_ceilingheight - b_floorheight;

			if ((li->flags & ML_DONTPEGBOTTOM) == 0) {
				rowoffs = curRowoffset >> 16;
			} else {
				rowoffs = height + (curRowoffset >> 16);
			}

			if (li->flags & ML_BLENDING) { // Scale gradient to match lower wall
				frontheight = f_ceilingheight - f_floorheight;
				if (!(li->flags & ML_BLENDFULLBOTTOM) && frontheight != 0) {
					sideheight = b_floorheight - f_ceilingheight;

					scale = (float)sideheight / (float)frontheight;

					rn = ((float)r1-(float)r2)*scale + (float)r1;
					gn = ((float)g1-(float)g2)*scale + (float)g1;
					bn = ((float)b1-(float)b2)*scale + (float)b1;

					if (!((rn < 256) && (gn < 256) && (bn < 256))) { // Rescale if out of color bounds
						scale = 255.0f;

						if (rn >= gn && rn >= bn) {
							scale /= rn;
						} else if (gn >= rn && gn >= bn) {
							scale /= gn;
						} else {
							scale /= bn;
						}

						rn *= scale;
						gn *= scale;
						bn *= scale;
					}

					tmp_upcolor = ((int)rn << 24) | ((int)gn << 16) | ((int)bn << 8) | 0xff;
				}

				topcolor = tmp_upcolor;
				bottomcolor = lowcolor;

				// clip middle color lower
				lowcolor = tmp_upcolor;
			}

			R_RenderWall(seg, li->flags, textures[side->bottomtexture],
						 b_floorheight, f_floorheight,
						 rowoffs, rowoffs + (b_floorheight - f_floorheight),
						 topcolor, bottomcolor);

			m_bottom = b_floorheight; // clip middle bottom height
			if ((li->flags & (ML_CHECKFLOORHEIGHT|ML_SWITCHX08)) == ML_CHECKFLOORHEIGHT) {
				if (SWITCHMASK(li->flags) == ML_SWITCHX02) {
					pic = side->toptexture;
					rowoffs = side->rowoffset >> 16;
				} else {
					pic = side->midtexture;
					rowoffs = side->rowoffset >> 16;
				}
				R_RenderSwitch(seg, pic, b_floorheight + rowoffs - 16, thingcolor);
			}
		}

		if (!(li->flags & ML_DRAWMASKED)) {
			return;
		}
	}

	height = m_top - m_bottom;

	if (li->flags & ML_DONTPEGBOTTOM) {
		rowoffs = ((height + 127) & -128) + (curRowoffset >> 16);
	} else if (li->flags & ML_DONTPEGTOP) {
		rowoffs = (curRowoffset >> 16) - m_bottom;
	} else {
		rowoffs = (curRowoffset >> 16) + height;
	}

	if (li->flags & ML_BLENDING) {
		topcolor = upcolor;
		bottomcolor = lowcolor;
	}

	R_RenderWall(seg, li->flags, textures[side->midtexture],
				 m_top, m_bottom,
				 rowoffs - height, rowoffs,
				 topcolor, bottomcolor);

	if ((li->flags & (ML_CHECKFLOORHEIGHT|ML_SWITCHX08)) == (ML_CHECKFLOORHEIGHT|ML_SWITCHX08)) {
		if (SWITCHMASK(li->flags) == ML_SWITCHX02) {
			pic = side->toptexture;
			rowoffs = side->rowoffset >> 16;
		} else {
			pic = side->bottomtexture;
			rowoffs = side->rowoffset >> 16;
		}
		R_RenderSwitch(seg, pic, m_bottom + rowoffs + 48, thingcolor);
	}
}


void R_RenderWall(seg_t *seg, int flags, int texture, int topHeight, int bottomHeight,
                  int topOffset, int bottomOffset, int topColor, int bottomColor) // 80027138
{
    int fl_extra = 0, fl_x = 0, fl_y = 0, fl_z = 0, fl_t = 0, fl_h = 0; // flash vertex (see R_FlashWallPoint)
    textureN64_t *data;
    byte *src;
    vertex_t *v1;
    vertex_t *v2;
    int cms, cmt;
    int wshift, hshift;
    int key;
    texdl_t *e;
    Gfx *g;
	
	// [GEC] Prevents errors in textures in S coordinates
    int curTextureoffset = (seg->sidedef->textureoffset + seg->offset) & (255 << FRACBITS);

    /* BLANK is fully transparent, so nothing would be drawn: skip the whole wall */
    if ((texture >> 4) == blanklump)
        return;

    /* vertical look: a piece above or below the screen would only be cut away by the RSP */
    if (!R_WallPieceVisible(seg, topHeight, bottomHeight))
    {
        return;
    }

    /*
     * Keep the T coordinates inside the signed 16-bit vertex range.  A row offset
     * plus a tall wall, or a wall far above or below zero, can push (offset << 5)
     * past +-32768, and if the top and bottom edges land on different sides of
     * the wrap the texture gets stretched across the wall.  Textures repeat with a
     * period that divides 1024, so moving the pair by a multiple of 1024 is invisible.
     */
    {
        int offsetheight = bottomOffset - topOffset;
        topOffset = (topOffset & 1023) - 1024;
        bottomOffset = topOffset + offsetheight;
    }

    if (texture != 16)
    {
        if (flags & ML_HMIRROR) {
            cms = G_TX_MIRROR;
        }
        else {
            cms = G_TX_NOMIRROR;
        }

        if (flags & ML_VMIRROR) {
            cmt = G_TX_MIRROR;
        }
        else {
            cmt = G_TX_NOMIRROR;
        }

        if ((texture != globallump) || (globalcm != (cms | cmt)))
        {
            /*
            In Doom 64 all textures are compressed with the second method (dec_none),
            in the original line it was declared that if a texture was not stored,
            it would be stored from the DOOM64.WAD and decompressed with the Jaguar Doom
            method (dec_jag) which is wrong since all the textures are previously
            loaded from the P_Init function with the second decompression method (dec_d64)
            */
            //data = W_CacheLumpNum(texture >> 4, PU_CACHE); // error decomp mode
            data = W_CacheLumpNum(texture >> 4, PU_CACHE); /* [GEC] FIXED */

            key = (texture << 3) | (cms ? 2 : 0) | (cmt ? 1 : 0);
            e = &texdl[vid_side][TEXDL_HASH(key)];

            if ((e->key == key) && (e->data == (byte *)data))
            {
                e->frame = NextFrameIdx;
                gSPDisplayList(GFX1++, e->dl);
            }
            else
            {
                wshift = data->wshift;
                hshift = data->hshift;

                src = data->texels;

                if (e->frame != NextFrameIdx)
                {
                    g = R_TexCmdsWall(e->dl, src, texture, wshift, hshift, cms, cmt);
                    gSPEndDisplayList(g++);
                    e->key = key;
                    e->data = (byte *)data;
                    e->frame = NextFrameIdx;
                    gSPDisplayList(GFX1++, e->dl);
                }
                else
                {
                    /* entry already used by another texture in this frame: emit inline */
                    GFX1 = R_TexCmdsWall(GFX1, src, texture, wshift, hshift, cms, cmt);
                }
            }

            globallump = texture;
            globalcm = (cms | cmt);
        }

        gSPTexture(GFX1++, (1024 << 6)-1, (1024 << 6)-1, 0, G_TX_RENDERTILE, G_ON);

        v1 = seg->v1;
        v2 = seg->v2;

        // a flash near the middle of a wall: add a vertex at the nearest point so it glows there
        fl_extra = 0;
        if (FLASH_WALL_SPLIT && flash_count)
        {
            // Only when the whole wall is in front of the camera: a corner behind it already
            // projects badly (no near plane clipping), and splitting the wall around the new
            // vertex would warp the texture there. Such walls keep the corner-only lighting.
            long long d1 = ((long long)((v1->x & R_DRAWNMASK) - viewx) * viewcos + (long long)((v1->y & R_DRAWNMASK) - viewy) * viewsin) >> 32;
            long long d2 = ((long long)((v2->x & R_DRAWNMASK) - viewx) * viewcos + (long long)((v2->y & R_DRAWNMASK) - viewy) * viewsin) >> 32;

            if (d1 >= FLASH_MIN_DEPTH && d2 >= FLASH_MIN_DEPTH)
                fl_extra = R_FlashWallPoint(v1->x >> FRACBITS, v1->y >> FRACBITS, v2->x >> FRACBITS, v2->y >> FRACBITS,
                                            topHeight, bottomHeight, &fl_x, &fl_y, &fl_z, &fl_t, &fl_h);
        }

        if (fl_extra)
        {
            gSPVertex(GFX1++, VTX1, 5, 0);
            gSP2Triangles(GFX1++, 4, 0, 1, 1, 4, 1, 2, 1);
            gSP2Triangles(GFX1++, 4, 2, 3, 1, 4, 3, 0, 1);
        }
        else
        {
            gSPVertex(GFX1++, VTX1, 4, 0);
            gSP1Quadrangle(GFX1++, 0, 1, 2, 3, 1);
        }

        // x coordinates
        VTX1[0].v.ob[0] = VTX1[3].v.ob[0] = (signed short)(v1->x >> FRACBITS);
        VTX1[1].v.ob[0] = VTX1[2].v.ob[0] = (signed short)(v2->x >> FRACBITS);

        // y coordinates
        VTX1[0].v.ob[1] = VTX1[1].v.ob[1] = topHeight;
        VTX1[3].v.ob[1] = VTX1[2].v.ob[1] = bottomHeight;

        // z coordinates
        VTX1[0].v.ob[2] = VTX1[3].v.ob[2] = (signed short)-(v1->y >> FRACBITS);
        VTX1[1].v.ob[2] = VTX1[2].v.ob[2] = (signed short)-(v2->y >> FRACBITS);

        // texture s coordinates
        VTX1[0].v.tc[0] = VTX1[3].v.tc[0] = (curTextureoffset >> 11);
        VTX1[1].v.tc[0] = VTX1[2].v.tc[0] = VTX1[0].v.tc[0] + (seg->length << 1);

        // texture t coordinates
        VTX1[0].v.tc[1] = VTX1[1].v.tc[1] = (topOffset << 5);
        VTX1[2].v.tc[1] = VTX1[3].v.tc[1] = (bottomOffset << 5);

        // vertex color
        if (flash_count && R_FlashWallHit(v1->x >> FRACBITS, v1->y >> FRACBITS, v2->x >> FRACBITS, v2->y >> FRACBITS, topHeight, bottomHeight)) // lit by a flash: color each corner by its distance
        {
            int x1 = v1->x >> FRACBITS, y1 = v1->y >> FRACBITS;
            int x2 = v2->x >> FRACBITS, y2 = v2->y >> FRACBITS;

            *(int*)VTX1[0].v.cn = R_FlashColor(topColor, x1, y1, topHeight);
            *(int*)VTX1[1].v.cn = R_FlashColor(topColor, x2, y2, topHeight);
            *(int*)VTX1[2].v.cn = R_FlashColor(bottomColor, x2, y2, bottomHeight);
            *(int*)VTX1[3].v.cn = R_FlashColor(bottomColor, x1, y1, bottomHeight);
        }
        else
        {
            *(int*)VTX1[0].v.cn = *(int*)VTX1[1].v.cn = topColor;
            *(int*)VTX1[2].v.cn = *(int*)VTX1[3].v.cn = bottomColor;
        }

        if (fl_extra)
        {
            VTX1[4].v.ob[0] = (signed short)fl_x;
            VTX1[4].v.ob[1] = (signed short)fl_z;
            VTX1[4].v.ob[2] = (signed short)-fl_y;
            VTX1[4].v.tc[0] = VTX1[0].v.tc[0] + (((VTX1[1].v.tc[0] - VTX1[0].v.tc[0]) * fl_t) >> 8);
            VTX1[4].v.tc[1] = VTX1[0].v.tc[1] + (((VTX1[3].v.tc[1] - VTX1[0].v.tc[1]) * fl_h) >> 8);
            *(int*)VTX1[4].v.cn = R_FlashColor(R_LerpColor(topColor, bottomColor, fl_h), fl_x, fl_y, fl_z);
            VTX1 += 5;
        }
        else
            VTX1 += 4;
    }
}

void R_RenderSwitch(seg_t *seg, int texture, int topOffset, int color) // 80027654
{
    textureN64_t *data;
    byte *src;
    vertex_t *v1;
    vertex_t *v2;
    fixed_t x, y;
    fixed_t sin, cos;
    int wshift = 5, hshift = 5;

    if (texture <= 1) return;

    if (texture != globallump)
    {
        /*
        In Doom 64 all textures are compressed with the second method (dec_none),
        in the original line it was declared that if a texture was not stored,
        it would be stored from the DOOM64.WAD and decompressed with the Jaguar Doom
        method (dec_jag) which is wrong since all the textures are previously
        loaded from the P_Init function with the second decompression method (dec_d64)
        */
        //data = W_CacheLumpNum(firsttex + texture, PU_CACHE); // error decomp mode
        data = W_CacheLumpNum(firsttex + texture, PU_CACHE); /* [GEC] FIXED */

        wshift = data->wshift;
        hshift = data->hshift;

        src = data->texels;

        // Load Image Data
        gDPSetTextureImage(GFX1++, G_IM_FMT_CI, G_IM_SIZ_16b , 1, src);
        gDPSetTile(GFX1++, G_IM_FMT_CI, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, 0, 0, 0, 0, 0, 0);

        gDPLoadSync(GFX1++);
        gDPLoadBlock(GFX1++, G_TX_LOADTILE, 0, 0,
                         (((1 << wshift) * (1 << hshift)) >> 2) - 1, 0);

        gDPPipeSync(GFX1++);
        gDPSetTile(GFX1++, G_IM_FMT_CI, G_IM_SIZ_4b,
                       (((1 << wshift) >> 1) + 7) >> 3, 0, G_TX_RENDERTILE, 0,
                       0, 0, 0,
                       0, 0, 0);

        gDPSetTileSize(GFX1++, G_TX_RENDERTILE, 0, 0,
                           ((1 << wshift) - 1) << 2,
                           ((1 << hshift) - 1) << 2);

        // Load Palette Data
        gDPSetTextureImage(GFX1++, G_IM_FMT_RGBA, G_IM_SIZ_16b , 1,
                            src + (1 << ((wshift + hshift + 31) & 31)));

        gDPTileSync(GFX1++);
        gDPSetTile(GFX1++, G_IM_FMT_RGBA, G_IM_SIZ_4b, 0, 256, G_TX_LOADTILE, 0, 0, 0, 0, 0, 0, 0);

        gDPLoadSync(GFX1++);
        gDPLoadTLUTCmd(GFX1++, G_TX_LOADTILE, 15);

        gDPPipeSync(GFX1++);

        globallump = texture;
    }

    gSPTexture(GFX1++, (512 << 6), (512 << 6), 0, 0, 1);

    gSPVertex(GFX1++, VTX1, 4, 0);
    gSP1Quadrangle(GFX1++, 0, 1, 2, 3, 1);

    v1 = seg->linedef->v1;
    v2 = seg->linedef->v2;

    x = (v1->x + v2->x);
    if (x < 0) {x = x + 1;}

    y = (v1->y + v2->y);
    if (y < 0) {y = y + 1;}

    x >>= 1;
    y >>= 1;

    cos = finecosine[seg->angle >> ANGLETOFINESHIFT] << 1;
    sin = finesine[seg->angle >> ANGLETOFINESHIFT] << 1;

    // x coordinates
    VTX1[0].v.ob[0] = VTX1[3].v.ob[0] = ((x) - (cos << 3) + sin) >> FRACBITS;
    VTX1[1].v.ob[0] = VTX1[2].v.ob[0] = ((x) + (cos << 3) + sin) >> FRACBITS;

    // y coordinates
    VTX1[0].v.ob[1] = VTX1[1].v.ob[1] = topOffset;
    VTX1[3].v.ob[1] = VTX1[2].v.ob[1] = topOffset - 32;

    // z coordinates
    VTX1[0].v.ob[2] = VTX1[3].v.ob[2] = ((-y) + (sin << 3) + cos) >> FRACBITS;
    VTX1[1].v.ob[2] = VTX1[2].v.ob[2] = ((-y) - (sin << 3) + cos) >> FRACBITS;

    // texture s coordinates
    VTX1[0].v.tc[0] = VTX1[3].v.tc[0] = (0 << 6);
    VTX1[1].v.tc[0] = VTX1[2].v.tc[0] = ((1 << wshift) << 6);

    // texture t coordinates
    VTX1[0].v.tc[1] = VTX1[1].v.tc[1] = (0 << 6);
    VTX1[2].v.tc[1] = VTX1[3].v.tc[1] = ((1 << hshift) << 6);

    // vertex color
    *(int*)VTX1[0].v.cn = *(int*)VTX1[1].v.cn = *(int*)VTX1[2].v.cn = *(int*)VTX1[3].v.cn = color;

    VTX1 += 4;
}

void R_RenderPlane(leaf_t *leaf, int numverts, int zpos, int texture, int xpos, int ypos, int color) // 80027B68
{
    int fl_extra = 0, fl_x = 0, fl_y = 0; // flash vertex (see R_FlashPlanePoint)
    int fl; // a flash reaches this polygon
    textureN64_t *data;
    byte *src;
    vertex_t *vrt;
    fixed_t x;
    fixed_t y;
    int idx, i;
    int v00, v01, v02;
    int key;
    texdl_t *e;
    Gfx *g;

    if (texture != globallump)
    {
        /*
        In Doom 64 all textures are compressed with the second method (dec_none),
        in the original line it was declared that if a texture was not stored,
        it would be stored from the DOOM64.WAD and decompressed with the Jaguar Doom
        method (dec_jag) which is wrong since all the textures are previously
        loaded from the P_Init function with the second decompression method (dec_d64)
        */
        //data = W_CacheLumpNum(texture >> 4, PU_CACHE); // error decomp mode
        data = W_CacheLumpNum(texture >> 4, PU_CACHE); /* [GEC] FIXED */

        key = (texture << 3) | 4;
        e = &texdl[vid_side][TEXDL_HASH(key)];

        if ((e->key == key) && (e->data == (byte *)data))
        {
            e->frame = NextFrameIdx;
            gSPDisplayList(GFX1++, e->dl);
        }
        else
        {
            src = data->texels;

            if (e->frame != NextFrameIdx)
            {
                g = R_TexCmdsPlane(e->dl, src, texture);
                gSPEndDisplayList(g++);
                e->key = key;
                e->data = (byte *)data;
                e->frame = NextFrameIdx;
                gSPDisplayList(GFX1++, e->dl);
            }
            else
            {
                GFX1 = R_TexCmdsPlane(GFX1, src, texture);
            }
        }

        globallump = texture;
    }

    fl = flash_count ? R_FlashPlaneHit(leaf, (numverts > 32) ? 32 : numverts, zpos) : 0;

    vrt = leaf->vertex;
    VTX1[0].v.ob[0] = (vrt->x >> FRACBITS);
    VTX1[0].v.ob[1] = zpos;
    VTX1[0].v.ob[2] =-(vrt->y >> FRACBITS);
    VTX1[0].v.tc[0] = ((((vrt->x + xpos) & 0x3f0000U) >> FRACBITS) << 5);
    VTX1[0].v.tc[1] =-((((vrt->y + ypos) & 0x3f0000U) >> FRACBITS) << 5);
    if (fl)
        *(int *)VTX1[0].v.cn = R_FlashColor(color, vrt->x >> FRACBITS, vrt->y >> FRACBITS, zpos);
    else
        *(int *)VTX1[0].v.cn = color;

    x = ((vrt->x + xpos) >> FRACBITS) & -64;
    y = ((vrt->y + ypos) >> FRACBITS) & -64;

    gSPTexture(GFX1++, (1024 << 6)-1, (1024 << 6)-1, 0, G_TX_RENDERTILE, G_ON);

    if (numverts >= 32)
        numverts = 32;

    // a flash over a big polygon: add a vertex at the nearest point so the light shows there
    fl_extra = 0;
    if (fl && numverts < 32)
        fl_extra = R_FlashPlanePoint(leaf, numverts, zpos, &fl_x, &fl_y);

    if (fl_extra)
    {
        // fan of triangles around the extra vertex (index numverts)
        gSPVertex(GFX1++, VTX1, numverts + 1, 0);
        VTX1++;

        for (i = 0; i < numverts; i += 2)
        {
            if (i + 1 < numverts)
            {
                gSP2Triangles(GFX1++, numverts, i, i + 1, 0,
                                      numverts, i + 1, (i + 2) % numverts, 0);
            }
            else
            {
                gSP1Triangle(GFX1++, numverts, i, (i + 1) % numverts, 0);
            }
        }

        idx = numverts; // no regular triangles below
        leaf++;
        numverts--;
        idx = numverts;
    }
    else
    {
    gSPVertex(GFX1++, VTX1, numverts, 0);
    VTX1++;

    if (numverts & 1)
    {
        idx = 2;
        gSP1Triangle(GFX1++, 0, 1, 2, 0);
    }
    else
    {
        idx = 1;
    }

    leaf++;
    numverts--;
    }

    if (idx < numverts)
    {
        v00 = idx + 0;
        v01 = idx + 1;
        v02 = idx + 2;
        do
        {
            gSP2Triangles(GFX1++,
                          v00, v01, v02, 0, // 0, 1, 2
                          v00, v02, 0, 0);  // 0, 2, 0

            v00 += 2;
            v01 += 2;
            v02 += 2;
        } while (v02 < (numverts + 2));
    }

    /*i = 0;
    if (numverts > 0)
    {
        if ((numverts & 3))
        {
            while(i != (numverts & 3))
            {
                vrt = leaf->vertex;
                VTX1[0].v.ob[0] = (vrt->x >> FRACBITS);
                VTX1[0].v.ob[1] = zpos;
                VTX1[0].v.ob[2] =-(vrt->y >> FRACBITS);
                VTX1[0].v.tc[0] = ((((vrt->x + xpos) >> FRACBITS) - x) << 5);
                VTX1[0].v.tc[1] =-((((vrt->y + ypos) >> FRACBITS) - y) << 5);
                *(int *)VTX1[0].v.cn = color;
                VTX1++;
                leaf++;
                i++;
            }
        }

        while(i != numverts)
        {
            vrt = leaf->vertex;
            VTX1[0].v.ob[0] = (vrt->x >> FRACBITS);
            VTX1[0].v.ob[1] = zpos;
            VTX1[0].v.ob[2] =-(vrt->y >> FRACBITS);
            VTX1[0].v.tc[0] = ((((vrt->x + xpos) >> FRACBITS) - x) << 5);
            VTX1[0].v.tc[1] =-((((vrt->y + ypos) >> FRACBITS) - y) << 5);
            *(int *)VTX1[0].v.cn = color;
            VTX1++;
            leaf++;

            vrt = leaf->vertex;
            VTX1[0].v.ob[0] = (vrt->x >> FRACBITS);
            VTX1[0].v.ob[1] = zpos;
            VTX1[0].v.ob[2] =-(vrt->y >> FRACBITS);
            VTX1[0].v.tc[0] = ((((vrt->x + xpos) >> FRACBITS) - x) << 5);
            VTX1[0].v.tc[1] =-((((vrt->y + ypos) >> FRACBITS) - y) << 5);
            *(int *)VTX1[0].v.cn = color;
            VTX1++;
            leaf++;

            vrt = leaf->vertex;
            VTX1[0].v.ob[0] = (vrt->x >> FRACBITS);
            VTX1[0].v.ob[1] = zpos;
            VTX1[0].v.ob[2] =-(vrt->y >> FRACBITS);
            VTX1[0].v.tc[0] = ((((vrt->x + xpos) >> FRACBITS) - x) << 5);
            VTX1[0].v.tc[1] =-((((vrt->y + ypos) >> FRACBITS) - y) << 5);
            *(int *)VTX1[0].v.cn = color;
            VTX1++;
            leaf++;

            vrt = leaf->vertex;
            VTX1[0].v.ob[0] = (vrt->x >> FRACBITS);
            VTX1[0].v.ob[1] = zpos;
            VTX1[0].v.ob[2] =-(vrt->y >> FRACBITS);
            VTX1[0].v.tc[0] = ((((vrt->x + xpos) >> FRACBITS) - x) << 5);
            VTX1[0].v.tc[1] =-((((vrt->y + ypos) >> FRACBITS) - y) << 5);
            *(int *)VTX1[0].v.cn = color;
            VTX1++;
            leaf++;

            i += 4;
        }
    }*/

    for(i = 0; i < numverts; i++)
    {
        vrt = leaf->vertex;
        VTX1[0].v.ob[0] = (vrt->x >> FRACBITS);
        VTX1[0].v.ob[1] = zpos;
        VTX1[0].v.ob[2] =-(vrt->y >> FRACBITS);
        VTX1[0].v.tc[0] = ((((vrt->x + xpos) >> FRACBITS) - x) << 5);
        VTX1[0].v.tc[1] =-((((vrt->y + ypos) >> FRACBITS) - y) << 5);
        if (fl)
            *(int *)VTX1[0].v.cn = R_FlashColor(color, vrt->x >> FRACBITS, vrt->y >> FRACBITS, zpos);
        else
            *(int *)VTX1[0].v.cn = color;
        VTX1++;
        leaf++;
    }

    if (fl_extra)
    {
        VTX1[0].v.ob[0] = fl_x;
        VTX1[0].v.ob[1] = zpos;
        VTX1[0].v.ob[2] =-fl_y;
        VTX1[0].v.tc[0] = (((((fl_x << FRACBITS) + xpos) >> FRACBITS) - x) << 5);
        VTX1[0].v.tc[1] =-(((((fl_y << FRACBITS) + ypos) >> FRACBITS) - y) << 5);
        *(int *)VTX1[0].v.cn = R_FlashColor(color, fl_x, fl_y, zpos);
        VTX1++;
    }
}

void R_RenderThings(subsector_t *sub) // 80028248
{
    spriteN64_t *data;
    byte *src;
    byte *paldata;
    vissprite_t *vissprite_p;

    mobj_t *thing;
    boolean flip;
    int lump;

    int compressed;
    int tileh;
    int tilew;
    int height;
    int width;
    int tiles;
    int color;

    fixed_t xx, yy;
    int xpos1, xpos2;
    int ypos;
    int zpos1, zpos2;
    int spos, tpos;
    int v00, v01, v02, v03;
	short extralight = 0; // hold the sector light level and thunder flashes if present

    // RDP state carried from one sprite to the next, so runs of the same sprite
    // (puffs, blood) only send the triangles. Reset on every call because walls
    // and planes reuse TMEM between subsectors.
    int lastlump = -1;              // lump whose single tile is in TMEM
    int lastsingle = 0;             // that lump was a single tile (TMEM holds all of it)
    int lastpalkey = 0x7fffffff;    // palette currently in TMEM (see palkey below)
    int lastlight = -1;             // prim color currently set
    int lastalpha = -1;
    int texset = 0;                 // gSPTexture sent
    int palkey;
    int needsync;
    int reuse;

    vissprite_p = sub->vissprite;
    if (vissprite_p)
    {
        R_World2C();
        gDPPipeSync(GFX1++);

        if (vissprite_p->thing->flags & MF_RENDERLASER)
        {
            gDPSetRenderMode(GFX1++, G_RM_FOG_SHADE_A, G_RM_RA_OPA_SURF2);
            gDPSetCombineMode(GFX1++, G_CC_D64COMB15, G_CC_D64COMB16);

            do
            {
                I_CheckGFX();
                R_RenderLaser(vissprite_p->thing);

                vissprite_p = vissprite_p->next;
                if(vissprite_p == NULL) {
                    break;
                }

            } while(vissprite_p->thing->flags & MF_RENDERLASER);

            gDPPipeSync(GFX1++);
            gDPSetCombineMode(GFX1++, G_CC_D64COMB07, G_CC_D64COMB08);

            if (vissprite_p == NULL)
            {
                gDPSetRenderMode(GFX1++, G_RM_FOG_SHADE_A, G_RM_TEX_EDGE2);
                return;
            }
        }

        gDPSetRenderMode(GFX1++, G_RM_FOG_SHADE_A, G_RM_XLU_SURF2_CLAMP);

        while (vissprite_p)
        {
            I_CheckGFX();

            thing = vissprite_p->thing;
            lump = vissprite_p->lump;
            flip = vissprite_p->flip;

            if (thing->flags & MF_NIGHTMARE)
            {
                color = 0x20ffffff; //PACKRGBA(32, 255, 255, 255)
            }
            else if (thing->frame & FF_FULLBRIGHT)
            {
                color = 0xffffffff; //PACKRGBA(255, 255, 255, 255)
            }
            else
            {
                color = lights[vissprite_p->sector->colors[2]].rgba;

                // lightning lights sprites standing in sky exposed sectors
                if (LightningFlash != 0 && vissprite_p->sector->ceilingpic == -1)
                    color = R_LightenColor(color, LightningFlash * LIGHTNING_COLOR_BOOST);

                if (flash_count) // lit by a flash, measured at the middle of the sprite
                    color = R_FlashColor(color, thing->x >> FRACBITS, thing->y >> FRACBITS,
                                         (thing->z + (thing->height >> 1)) >> FRACBITS);
            }
			
			// Flash sky exposed SPRITES when there is lightning
			extralight = vissprite_p->sector->lightlevel;

            needsync = 0;
            if ((extralight != lastlight) || (thing->alpha != lastalpha))
            {
                gDPSetPrimColorD64(GFX1++, 0, extralight, thing->alpha);
                lastlight = extralight;
                lastalpha = thing->alpha;
                needsync = 1;
            }

            data = W_CacheLumpNum(lump, PU_CACHE);

            compressed = data->compressed;
            tileh = data->tileheight;
            width = data->width;
            height = data->height;
            tiles = data->tiles << 1;

            spos = width;
            tpos = 0;

            src = data->texels;

            if (flip)
            {
                xx = thing->x + (data->xoffs * viewsin);
                xpos1 = (xx - (width * viewsin)) >> FRACBITS;
                xpos2 = (xx) >> FRACBITS;

                yy = thing->y - (data->xoffs * viewcos);
                zpos1 = -(yy + (width * viewcos)) >> FRACBITS;
                zpos2 = -(yy) >> FRACBITS;
            }
            else
            {
                xx = thing->x - (data->xoffs * viewsin);
                xpos2 = (xx + (width * viewsin)) >> FRACBITS;
                xpos1 = (xx) >> FRACBITS;

                yy = thing->y + (data->xoffs * viewcos);
                zpos2 = -(yy - (width * viewcos)) >> FRACBITS;
                zpos1 = -(yy) >> FRACBITS;
            }

            if (!texset)
            {
                gSPTexture(GFX1++, (512 << 6), (512 << 6), 0, 0, 1);
                texset = 1;
            }

            gSPVertex(GFX1++, VTX1, (tiles+2), 0);

            // The palette is identified by its lump: shared (external) palettes by the
            // palette lump, embedded ones by the sprite lump (negative, so they never
            // collide).
            if ((compressed < 0) && (data->cmpsize & 1))
                palkey = (lump - (data->cmpsize >> 1)) + thing->info->palette;
            else
                palkey = -(lump + 1);

            if (compressed < 0)
            {
                width = (data->width + 7) & ~7;
                tilew = tileh * width;

                if (palkey != lastpalkey)
                {
                    if (data->cmpsize & 1)
                    {
                        paldata = (byte *)(W_CacheLumpNum(palkey, PU_CACHE)) + 8;
                    }
                    else
                    {
                        paldata = (src + data->cmpsize);
                    }

                    // Load Palette Data (256 colors)
                    gDPSetTextureImage(GFX1++, G_IM_FMT_RGBA, G_IM_SIZ_16b , 1, paldata);

                    gDPTileSync(GFX1++);
                    gDPSetTile(GFX1++, G_IM_FMT_RGBA, G_IM_SIZ_4b, 0, 256, G_TX_LOADTILE, 0, 0, 0, 0, 0, 0, 0);

                    gDPLoadSync(GFX1++);
                    gDPLoadTLUTCmd(GFX1++, G_TX_LOADTILE, 255);
                    needsync = 1;
                }
            }
            else
            {
                width = (data->width + 15) & ~15;
                tilew = tileh * width;

                if (tilew < 0) {
                    tilew = tilew + 1;
                }

                tilew >>= 1;

                if (palkey != lastpalkey)
                {
                    // Load Palette Data (16 colors)
                    gDPSetTextureImage(GFX1++, G_IM_FMT_RGBA, G_IM_SIZ_16b , 1, (src + data->cmpsize));

                    gDPTileSync(GFX1++);
                    gDPSetTile(GFX1++, G_IM_FMT_RGBA, G_IM_SIZ_4b, 0, 256, G_TX_LOADTILE, 0, 0, 0, 0, 0, 0, 0);

                    gDPLoadSync(GFX1++);
                    gDPLoadTLUTCmd(GFX1++, G_TX_LOADTILE, 15);
                    needsync = 1;
                }
            }
            lastpalkey = palkey;

            // Same single-tile image as the previous sprite is still in TMEM
            reuse = (lump == lastlump) && lastsingle;

            // Previous triangles must finish before anything they read is changed
            if (needsync || !reuse)
            {
                gDPPipeSync(GFX1++);
            }

            ypos = (thing->z >> FRACBITS) + data->yoffs;

            VTX1[0].v.ob[0] = xpos1;
            VTX1[0].v.ob[1] = ypos;
            VTX1[0].v.ob[2] = zpos1;

            VTX1[1].v.ob[0] = xpos2;
            VTX1[1].v.ob[1] = ypos;
            VTX1[1].v.ob[2] = zpos2;

            VTX1[flip].v.tc[0] = 0;
            VTX1[flip^1].v.tc[0] = (spos << 6);

            VTX1[0].v.tc[1] = VTX1[1].v.tc[1] = (tpos << 6);

            *(int *)VTX1[0].v.cn = *(int *)VTX1[1].v.cn = color;
            VTX1 += 2;

            v03 = 0;
            v00 = 1;
            v01 = 3;
            v02 = 2;

            if (tiles > 0)
            {
                do
                {
                    if (!reuse)
                    {
					gDPSetTextureImage(GFX1++, G_IM_FMT_CI, G_IM_SIZ_16b , 1, src);
					gDPSetTile(GFX1++, G_IM_FMT_CI, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, 0, 0, 0, 0, 0, 0);
					gDPLoadSync(GFX1++);
					
					gDPLoadBlock(GFX1++, G_TX_LOADTILE, 0, 0, (tilew >> 1) - 1, 0);
					gDPPipeSync(GFX1++);
					
                    if (compressed < 0) // Load Image Data (8bit)
					{
                        gDPSetTile(GFX1++, G_IM_FMT_CI, G_IM_SIZ_8b, (width >> 3), 0, G_TX_RENDERTILE , 0, 0, 0, 0, 0, 0, 0);
					}
                    else // Load Image Data (4bit)
					{
						gDPSetTile(GFX1++, G_IM_FMT_CI, G_IM_SIZ_4b, (width >> 4), 0, G_TX_RENDERTILE , 0, 0, 0, 0, 0, 0, 0);
					}
					
					gDPSetTileSize(GFX1++, G_TX_RENDERTILE, 0, tpos << 2, ((width - 1) << 2), (tpos + tileh - 1) << 2);
                    }

                    tpos += tileh;
                    ypos -= tileh;

                    gSP2Triangles(GFX1++, v00, v01, v02, 0, // 1, 3, 2
                                          v00, v02, v03, 0); // 1, 2, 0

                    VTX1[0].v.ob[0] = xpos1;
                    VTX1[0].v.ob[1] = ypos;
                    VTX1[0].v.ob[2] = zpos1;

                    VTX1[1].v.ob[0] = xpos2;
                    VTX1[1].v.ob[1] = ypos;
                    VTX1[1].v.ob[2] = zpos2;

                    VTX1[flip].v.tc[0] = 0;
                    VTX1[flip^1].v.tc[0] = (spos << 6);

                    VTX1[0].v.tc[1] = VTX1[1].v.tc[1] = (tpos << 6);

                    *(int *)VTX1[0].v.cn = *(int *)VTX1[1].v.cn = color;
                    VTX1 += 2;

                    src += tilew;

                    height -= tileh;
                    if (height < tileh) {
                        tileh = height;
                    }

                    v00 += 2;
                    v01 += 2;
                    v02 += 2;
                    v03 += 2;
                } while (v02 < (tiles+2));
            }

            lastlump = lump;
            lastsingle = (data->tiles == 1);

            vissprite_p = vissprite_p->next;

        }

        globallump = -1;

        gDPPipeSync(GFX1++);
        gDPSetRenderMode(GFX1++, G_RM_FOG_SHADE_A, G_RM_TEX_EDGE2);
    }
}

void R_RenderLaser(mobj_t *thing) // 80028CCC
{
    laserdata_t *laserdata;

    laserdata = (laserdata_t *)thing->extradata;

    gSPVertex(GFX1++, (VTX1), 6, 0);

    gSP2Triangles(GFX1++, 0, 2, 3, 1/*flag1*/,
						  0, 1, 2, 2/*flag2*/);

    gSP2Triangles(GFX1++, 0, 3, 5, 2/*flag1*/,
						  3, 4, 5, 2/*flag2*/);

    VTX1[0].v.ob[0] = (laserdata->x1 >> FRACBITS);
    VTX1[0].v.ob[1] = (laserdata->z1 >> FRACBITS);
    VTX1[0].v.ob[2] = -(laserdata->y1 >> FRACBITS);

    VTX1[1].v.ob[0] = ((laserdata->x1 - laserdata->slopey) >> FRACBITS);
    VTX1[1].v.ob[1] = (laserdata->z1 >> FRACBITS);
    VTX1[1].v.ob[2] = (-(laserdata->y1 + laserdata->slopex) >> FRACBITS);

    VTX1[2].v.ob[0] = ((laserdata->x2 - laserdata->slopey) >> FRACBITS);
    VTX1[2].v.ob[1] = (laserdata->z2 >> FRACBITS);
    VTX1[2].v.ob[2] = (-(laserdata->y2 + laserdata->slopex) >> FRACBITS);

    VTX1[3].v.ob[0] = (laserdata->x2 >> FRACBITS);
    VTX1[3].v.ob[1] = (laserdata->z2 >> FRACBITS);
    VTX1[3].v.ob[2] = -(laserdata->y2 >> FRACBITS);

    VTX1[4].v.ob[0] = ((laserdata->x2 + laserdata->slopey) >> FRACBITS);
    VTX1[4].v.ob[1] = (laserdata->z2 >> FRACBITS);
    VTX1[4].v.ob[2] = (-(laserdata->y2 - laserdata->slopex) >> FRACBITS);

    VTX1[5].v.ob[0] = ((laserdata->x1 + laserdata->slopey) >> FRACBITS);
    VTX1[5].v.ob[1] = (laserdata->z1 >> FRACBITS);
    VTX1[5].v.ob[2] = (-(laserdata->y1 - laserdata->slopex) >> FRACBITS);

    *(int *)VTX1[0].v.cn = 0xff0000ff;		// PACKRGBA(255,0,0,255)
    *(int *)VTX1[1].v.cn = 0xff;			// PACKRGBA(0,0,0,255);
    *(int *)VTX1[2].v.cn = 0xff;			// PACKRGBA(0,0,0,255);
    *(int *)VTX1[3].v.cn = 0xff0000ff;		// PACKRGBA(255,0,0,255);
    *(int *)VTX1[4].v.cn = 0xff;			// PACKRGBA(0,0,0,255);
    *(int *)VTX1[5].v.cn = 0xff;			// PACKRGBA(0,0,0,255);

    VTX1 += 6;
}

void R_RenderPSprites(void) // 80028f20
{
    int				i;
	pspdef_t		*psp, *psptmp;
	state_t			*state;
	spritedef_t		*sprdef;
	spriteframe_t	*sprframe;
	int				lump;
	int				flagtranslucent;

	boolean         palloaded;
	spriteN64_t	    *data;
	byte		    *paldata;
	byte		    *src;

	unsigned int    color; // lightning lit weapon color
	int             tilecnt;
	int             tiles;
	int             tileh;
	int             tilew;
	int             width;
	int             height;
	int             width2;
	int             yh;
	int             x, y;
	short extralight = 0; // hold the sector light level and thunder flashes if present

	I_CheckGFX();

	gDPPipeSync(GFX1++);
	gDPSetTexturePersp(GFX1++, G_TP_NONE);
	gDPSetCombineMode(GFX1++, G_CC_D64COMB17, G_CC_D64COMB18);

    psp = &viewplayer->psprites[0];

    flagtranslucent = (viewplayer->mo->flags & MF_SHADOW) != 0;

    psptmp = psp;
    for (i = 0; i < NUMPSPRITES; i++, psptmp++)
    {
        if(flagtranslucent || ((psptmp->state != 0) && (psptmp->alpha < 255)))
        {
            gDPSetRenderMode(GFX1++, G_RM_FOG_SHADE_A, G_RM_XLU_SURF2_CLAMP);
            break;
        }
    }

    palloaded = false;

    for (i = 0; i < NUMPSPRITES; i++, psp++)
	{
		if ((state = psp->state) != 0) /* a null state means not active */
		{
			sprdef = &sprites[state->sprite];
			sprframe = &sprdef->spriteframes[state->frame & FF_FRAMEMASK];
			lump = sprframe->lump[0];

            data = W_CacheLumpNum(lump, PU_CACHE);

            tilecnt = 0;
            tiles = data->tiles;
            width = data->width;
            tileh = data->tileheight;
            width2 = (width + 7) & ~7;
            tilew = tileh * width2;
            height = data->height;
            src = data->texels;

            x = (((psp->sx >> FRACBITS) - data->xoffs) + 160) << 2;
            y = (((psp->sy >> FRACBITS) - data->yoffs) + 239) << 2;
            if (viewplayer->onground)
            {
                x += (quakeviewx >> 20);
                y += (quakeviewy >> 14);
            }

			if (psp->state->frame & FF_FULLBRIGHT)
			{
			    gDPSetPrimColorD64(GFX1, 0, 0, 0xffffff00);		// PACKRGBA(255,255,255,0)
			}
			else
			{
				
				// Flash sky exposed player when there is lightning
				extralight = frontsector->lightlevel;

				color = lights[frontsector->colors[2]].rgba & ~255; // remove alpha value
				if (LightningFlash != 0 && frontsector->ceilingpic == -1)
					color = R_LightenColor(color, LightningFlash * LIGHTNING_COLOR_BOOST);

			    gDPSetPrimColorD64(GFX1, 0, extralight, color);
			}

			// apply alpha value
			if (flagtranslucent)
			{
				GFX1->words.w1 |= 144;
			}
			else
            {
                GFX1->words.w1 |= psp->alpha;
            }
            GFX1++; // continue to next GFX1

            if (!palloaded)
            {
                palloaded = true;

                if (data->cmpsize & 1)
                {
                /* Loads the palette from the first frame of the animation,  */
                /* which uses an odd number to get to the lump */
                    paldata = W_CacheLumpNum((lump - (data->cmpsize >> 1)),
                                             PU_CACHE);

                    paldata = ((spriteN64_t*)paldata)->texels + ((spriteN64_t*)paldata)->cmpsize;
                }
                else
                {
                /* Loads the palette if it is included in the image data */
                    paldata = (src + data->cmpsize);
                }

                /* Load Palette Data (256 colors) */
                gDPSetTextureImage(GFX1++, G_IM_FMT_RGBA, G_IM_SIZ_16b , 1, paldata);

                gDPTileSync(GFX1++);
                gDPSetTile(GFX1++, G_IM_FMT_RGBA, G_IM_SIZ_4b, 0, 256, G_TX_LOADTILE, 0, 0, 0, 0, 0, 0, 0);

                gDPLoadSync(GFX1++);
                gDPLoadTLUTCmd(GFX1++, G_TX_LOADTILE, 255);

                gDPPipeSync(GFX1++);
            }

            if (tiles > 0)
            {
                do
                {
                    /* Load Image Data (8bit) */
                    gDPSetTextureImage(GFX1++, G_IM_FMT_CI, G_IM_SIZ_16b , 1, src);
                    gDPSetTile(GFX1++, G_IM_FMT_CI, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, 0, 0, 0, 0, 0, 0);

                    gDPLoadSync(GFX1++);
                    gDPLoadBlock(GFX1++, G_TX_LOADTILE, 0, 0, ((width2 * tileh + 1) >> 1) - 1, 0);

                    gDPPipeSync(GFX1++);
                    gDPSetTile(GFX1++, G_IM_FMT_CI, G_IM_SIZ_8b, ((width2 + 7) >> 3), 0,
                                   G_TX_RENDERTILE , 0, 0, 0, 0, 0, 0, 0);

                    gDPSetTileSize(GFX1++, G_TX_RENDERTILE, 0, 0, ((width2 - 1) << 2), (tileh - 1) << 2);

                    yh = (tileh << 2) + y;

                    gSPTextureRectangle(GFX1++,
                                    x, y,
                                    (width << 2) + x, yh,
                                    0,
                                    0, 0,
                                    (1 << 10), (1 << 10));

                    height -= tileh;
                    if (height < tileh) {
                        tileh = height;
                    }

                    y = yh;

                    src += tilew;
                    tilecnt += 1;
                } while (tilecnt != (int)tiles);
            }
		}
	}
}
