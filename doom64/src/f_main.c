/* f_main.c -- finale */

#include "doomdef.h"
#include "p_local.h"
#include "st_main.h"
#include "r_local.h"
#include "texts.h"

extern void P_CheckCheats (void);


//
// Character cast strings F_FINALE.C
//
#define CC_ZOMBIE	"Zombieman"
#define CC_SHOTGUN	"Shotgun Guy"
//#define CC_HEAVY	"Heavy Weapon Dude" // Enemy Removed
#define CC_IMP		"Imp"
#define CC_NIMP		"Nightmare Imp" // New Enemy on Doom64
#define CC_DEMON	"Demon"
#define CC_SPECT	"Spectre"   // New Enemy on Doom64
#define CC_LOST		"Lost Soul"
#define CC_CACO		"Cacodemon"
#define CC_HELL		"Hell Knight"
#define CC_BARON	"Baron Of Hell"
#define CC_ARACH	"Arachnotron"
#define CC_PAIN		"Pain Elemental"
//#define CC_REVEN	"Revenant"  // Enemy Removed
#define CC_MANCU	"Mancubus"
//#define CC_ARCH	"Arch-Vile" // Enemy Removed
//#define CC_SPIDER	"The Spider Mastermind" // Enemy Removed
#define CC_CYBER	"The Cyberdemon"
#define CC_HERO		"Our Hero"

//
// Final DOOM 2 animation
// Casting by id Software.
// in order of appearance
//
typedef struct
{
	char		*name;
	mobjtype_t	type;
} castinfo_t;

static castinfo_t	castorder[] = // 8005A3F0
{
	{ CC_ZOMBIE, MT_POSSESSED1 },// MT_POSSESSED
	{ CC_SHOTGUN, MT_POSSESSED2 },// MT_SHOTGUY
	//{ CC_HEAVY, MT_CHAINGUY },
	{ CC_IMP, MT_IMP1 },// MT_TROOP
	{ CC_NIMP, MT_IMP2 },// MT_TROOP2
	{ CC_DEMON, MT_DEMON1 },// MT_SERGEANT
	{ CC_SPECT, MT_DEMON2 },// MT_SERGEANT2
	{ CC_LOST, MT_SKULL },// MT_SKULL
	{ CC_CACO, MT_CACODEMON },// MT_HEAD
	{ CC_HELL, MT_BRUISER2 },// MT_KNIGHT
	{ CC_BARON, MT_BRUISER1 },// MT_BRUISER
	{ CC_ARACH, MT_BABY },// MT_BABY
	{ CC_PAIN, MT_PAIN },// MT_PAIN
	//{ CC_REVEN, MT_UNDEAD },
	{ CC_MANCU, MT_MANCUBUS },// MT_FATSO
	//{ CC_ARCH, MT_VILE },
	//{ CC_SPIDER, MT_SPIDER },
	{ CC_CYBER, MT_CYBORG },// MT_CYBORG
	{ CC_HERO, MT_PLAYER },// MT_PLAYER
	{ NULL, 0 }
};

typedef enum
{
    F_STAGE_FADEIN_BACKGROUD,
    F_STAGE_DRAWTEXT,
    F_STAGE_SCROLLTEXT,
    F_STAGE_FADEOUT_BACKGROUD,
    F_STAGE_CAST
} finalestage_t;

static int textypos;			// 800631F0
static int textline;			// 800631F4
static char **text;			    // 800631F8
static int textalpha;			// 800631FC


/*
=================
=
= F_StartIntermission
=
=================
*/

void F_StartIntermission(void) // 80002CD0
{
    int introtext;

    /* the intro text shown when starting a game (not the secret level or
       ending texts) plays the newest song in the WSD */
    introtext = (nextmap == 0 || nextmap == 1 || nextmap == LOSTLEVEL ||
                 nextmap == BETALEVEL || nextmap == VEHEMENCELEVEL);

    if (nextmap == 0)
    {
        text = TX_LoadText("INTRO", 4);
        textypos = 20;
    } else if (nextmap == 1)
    {
        text = TX_LoadText("INTRO", 1);
        textypos = 20;
    } else if (nextmap == LOSTLEVEL)
    {
        text = TX_LoadText("INTRO", 2);
        textypos = 20;
    } else if (nextmap == BETALEVEL)
    {
        text = TX_LoadText("INTRO", 3);
        textypos = 20;
    } else if (nextmap == VEHEMENCELEVEL)
    {
        text = TX_LoadText("INTRO", 5);
        textypos = 20;
    } else if (nextmap == 9)
    {
        text = TX_LoadText("END", 1);
        textypos = 15;
    }
    else if (nextmap == 29)
    {
        text = TX_LoadText("END", 2);
        textypos = 43;
    }
    else if (nextmap == 30)
    {
        text = TX_LoadText("END", 3);
        textypos = 43;
    }
    else if (nextmap == 31)
    {
        text = TX_LoadText("END", 4);
        textypos = 43;
    }
    else if (nextmap == 32)
    {
        text = TX_LoadText("END", 5);
        textypos = 71;
    }
	else if (nextmap == 49) // Beta 64
    {
        text = TX_LoadText("END", 8);
        textypos = 15;
    }
    else if (nextmap == 72)
    {
        text = TX_LoadText("END", 9);
        textypos = 15;
    }
    else if (nextmap == 71)
    {
        text = TX_LoadText("END", 10);
        textypos = 15;
    }
    else if (nextmap == 57)
    {
        text = TX_LoadText("END", 11);
        textypos = 15;
    }
    else if (nextmap == 70)
    {
        text = TX_LoadText("END", 12);
        textypos = 15;
    }

    DrawerStatus = 2;
    textline = 0;
    textalpha = 0;
	
	if (introtext)
		S_StartMusic(MUS_INTROTEXT_SEQ); // intro text music
	else
		S_StartMusic(mus_title); // Play menu music for intermission
}

/*
=================
=
= F_StopIntermission
=
=================
*/

void F_StopIntermission(int exit) // 80002E14
{
	S_StopMusic(); // stop intermission music
    TX_FreeText();
    gamepaused = false;
    DrawerStatus = 0;
    I_WIPE_FadeOutScreen();
}

/*
=================
=
= F_TickerIntermission
=
=================
*/

int F_TickerIntermission(void) // 80002E44
{
	unsigned int buttons, oldbuttons, exit;

	gameaction = ga_nothing;
	P_CheckCheats();

	exit = gameaction;
	if (!gamepaused)
	{
	    buttons = ticbuttons[0] & 0xffff0000;
        oldbuttons = oldticbuttons[0] & 0xffff0000;

	    exit = ga_nothing;

        if(*text[textline])
        {
            textalpha += 8;
            if (textalpha > 255)
            {
                textalpha = 0;
                textline++;
            }
        }
        else if ((buttons != oldbuttons) && (buttons & (ALL_CBUTTONS|ALL_TRIG|PAD_A|PAD_B)))
        {
            exit = ga_exit;
        }
		
		 // Speed up text intermission by pressing buttons
		if (buttons & (ALL_CBUTTONS|ALL_TRIG|PAD_A|PAD_B))
        {
            textalpha = 256;
        }
	}

	return exit;
}

/*
=================
=
= F_DrawerIntermission
=
=================
*/

void F_DrawerIntermission(void) // 80002F14
{
    unsigned char i;
	short ypos;
    I_ClearFrame();

    gDPPipeSync(GFX1++);
    gDPSetCycleType(GFX1++, G_CYC_FILL);
    gDPSetRenderMode(GFX1++,G_RM_NOOP,G_RM_NOOP2);
    gDPSetColorImage(GFX1++, G_IM_FMT_RGBA, G_IM_SIZ_32b, SCREEN_WD, OS_K0_TO_PHYSICAL(cfb[vid_side]));
    // Fill borders with black
    gDPSetFillColor(GFX1++, GPACK_RGBA5551(0,0,0,0) << 16 | GPACK_RGBA5551(0,0,0,0)) ;
    gDPFillRectangle(GFX1++, 0, 0, SCREEN_WD-1, SCREEN_HT-1);

	M_DrawBackground(63, 25, 128, EpisodeGraphic());

    ypos = textypos;
    for(i = 0; i < textline; i++)
    {
		if(runintroduction) {
			ST_Message(20, ypos, text[i], 0xffffffff);
			ypos += 10;
		} else {
			ST_DrawString(-1, ypos, text[i], 0xffffffff);
			ypos += 14;
		}
    }
	
	if(runintroduction) {
		ST_Message(20, ypos, text[i], PACKRGBA(255, textalpha, textalpha, textalpha));
	} else {
		ST_DrawString(-1, ypos, text[i], PACKRGBA(255, textalpha, textalpha, textalpha));
	}

    if (MenuCall)
    {
        M_DrawOverlay(0, 0, 320, 240, 96);
        MenuCall();
    }

    I_DrawFrame();
}

static finalestage_t	finalestage;	// 80063200
static int				castnum;		// 80063204
static int				casttics;		// 80063208
static state_t         *caststate;		// 8006320C
static boolean			castdeath;		// 80063210
static int				castframes;		// 80063214
static int				castonmelee;	// 80063218
static int              castrotation;   // 8006321C
static int              castfadein;     // 80063220
static int              fadeinout;      // 80063224

/*
=================
=
= F_Start/Cast_Start
=
=================
*/

void F_Start(void) // 8000313C
{
	DrawerStatus = 3;
	finalestage = F_STAGE_FADEIN_BACKGROUD;
	fadeinout = 0;
	textypos = 15;
	textline = 0;
	textalpha = 0;
	text = TX_LoadText("END", gamemap == 65 ? 13 : gamemap == 39 ? 7 : 6);
	castnum = 0;
	caststate = &states[mobjinfo[castorder[castnum].type].seestate];
	casttics = states[mobjinfo[castorder[castnum].type].seestate].tics;
	castdeath = false;
	castframes = 0;
	castonmelee = 0;
	castrotation = 0;
	castfadein = 0;

	S_StartMusic(mus_final);
}

/*
=================
=
= F_Stop/Cast_Stop
=
=================
*/

void F_Stop(int exit) // 80003220
{
    TX_FreeText();
    gamepaused = false;
    DrawerStatus = 0;
    S_StopMusic();
    I_WIPE_FadeOutScreen();
}

/*
=================
=
= F_Ticker/Cast_Ticker
=
=================
*/

int F_Ticker(void) // 80003258
{
    unsigned int buttons, oldbuttons;
	int	st, sfx;

	buttons = ticbuttons[0] = M_ButtonResponder(ticbuttons[0]);
	oldbuttons = oldticbuttons[0] & 0xffff0000;

	gameaction = ga_nothing;
	P_CheckCheats();

	if (gamepaused != 0)
	{
		return gameaction;
	}

    switch(finalestage)
    {
        case F_STAGE_FADEIN_BACKGROUD:
            fadeinout += 6;
            if (fadeinout > 160)
            {
                fadeinout = 160;
                finalestage = F_STAGE_DRAWTEXT;
            }
            break;

        case F_STAGE_DRAWTEXT:
            if (*text[textline])
            {
                textalpha += 8;
                if (textalpha > 255)
                {
                    textalpha = 0;
                    textline++;
                }
            }
            else if ((buttons != oldbuttons) && (buttons & (ALL_CBUTTONS|ALL_TRIG|PAD_A|PAD_B)))
            {
                finalestage = F_STAGE_SCROLLTEXT;
            }
			
			// Speed up text intermission by pressing buttons
			if (buttons & (ALL_CBUTTONS|ALL_TRIG|PAD_A|PAD_B))
			{
				textalpha = 256;
			}
		
            break;

        case F_STAGE_SCROLLTEXT:
            textypos -= 1;
            if (textypos < -200)
            {
                finalestage = F_STAGE_FADEOUT_BACKGROUD;
            }
            break;

        case F_STAGE_FADEOUT_BACKGROUD:
            fadeinout -= 6;
            if (fadeinout < 0)
            {
                fadeinout = 0;
                finalestage = F_STAGE_CAST;
            }
            break;

        case F_STAGE_CAST:
            fadeinout += 6;
            if (fadeinout > 128)
            {
                fadeinout = 128;
            }

            if (castdeath == false)
			{
			    if (buttons != oldbuttons)
			    {
			        if (buttons & PAD_LEFT)
                    {
                        castrotation += 1;
                        if (castrotation > 7) {
                            castrotation = 0;
                        }
                    }
                    else if (buttons & PAD_RIGHT)
                    {
                        castrotation -= 1;
                        if (castrotation < 0) {
                            castrotation = 7;
                        }
                    }
                    else if (buttons & (ALL_CBUTTONS|ALL_TRIG|PAD_A|PAD_B))
                    {
                        S_StartSound(NULL, sfx_shotgun); // sfx_shotgn

                        /* go into death frame */
                        if (mobjinfo[castorder[castnum].type].deathsound)
                            S_StartSound(NULL, mobjinfo[castorder[castnum].type].deathsound);

                        caststate = &states[mobjinfo[castorder[castnum].type].deathstate];
                        castframes = 0;
                        castdeath = true;

                        if(castorder[castnum].type == MT_CYBORG) {
                            casttics = 10;
                        }
                        else {
                            casttics = caststate->tics;
                        }

                    }
			    }
			}

			if (gametic > gamevbls)
			{
                if (castfadein < 192) {
                    castfadein += 2;
                }

                /* advance state*/
                if (--casttics > 0)
                    return ga_nothing;  /* not time to change state yet */

				if (castdeath && caststate->nextstate == S_000) // S_NULL
				{
					/* switch from deathstate to next monster */
					castrotation = 0;
					castnum++;
					castfadein = 0;
					castdeath = false;

					if (castorder[castnum].name == NULL)
						castnum = 0;

					if (mobjinfo[castorder[castnum].type].seesound)
						S_StartSound(NULL, mobjinfo[castorder[castnum].type].seesound);

					caststate = &states[mobjinfo[castorder[castnum].type].seestate];
					castframes = 0;
				}

				st = caststate->nextstate;
				caststate = &states[st];

				if (castdeath == false)
                {
                    castframes++;

                    if (castframes == 12)
                    {   /* go into attack frame */
                        if (castonmelee)
                            caststate = &states[mobjinfo[castorder[castnum].type].meleestate];
                        else
                            caststate = &states[mobjinfo[castorder[castnum].type].missilestate];

                        castonmelee ^= 1;

                        if (caststate == &states[S_000]) // S_NULL
                        {
                            if (castonmelee)
                                caststate = &states[mobjinfo[castorder[castnum].type].meleestate];
                            else
                                caststate = &states[mobjinfo[castorder[castnum].type].missilestate];
                        }
                    }

                    if (((castframes == 20) && (castorder[castnum].type == MT_MANCUBUS)) ||
                          castframes == 24 || caststate == &states[S_001])//S_PLAY
                    {
                        caststate = &states[mobjinfo[castorder[castnum].type].seestate];
                        castframes = 0;
                    }
                }

				casttics = caststate->tics;
				if (casttics == -1)
					casttics = TICRATE;

                /* sound hacks.... */
                st = ((int)caststate - (int)states) / sizeof(state_t);
                switch (st)
                {
                    case S_007: // S_PLAY_ATK2
                        sfx = sfx_sht2fire; // sfx_dshtgn
                        break;

                    case S_055: // S_SARG_ATK2
                        sfx = sfx_sargatk; // sfx_sgtatk
                        break;

                    case S_084: // S_FATT_ATK8
                    case S_086: // S_FATT_ATK5
                    case S_088: // S_FATT_ATK2
                        sfx = sfx_bdmissile; // sfx_firsht
                        break;

                    case S_109: // S_POSS_ATK2
                        sfx = sfx_pistol;
                        break;

                    case S_138: // S_SPOS_ATK2
                        sfx = sfx_shotgun; // sfx_shotgn
                        break;

                    case S_166:   // S_TROO_ATK3
                        sfx = sfx_scratch; // sfx_claw
                        break;

                    case S_169: // S_TROO_ATK
                    case S_199: // S_HEAD_ATK2
                    case S_222: // S_BOSS_ATK2
                    case S_243: // S_BOS2_ATK2
                        sfx = sfx_bdmissile; // sfx_firsht
                        break;

                    case S_261: // S_SKULL_ATK2
                        sfx = sfx_skullatk; // sfx_sklatk
                        break;

                    case S_288: // S_BSPI_ATK2
                        sfx = sfx_plasma; // sfx_plasma
                        break;

                    case S_307: // S_CYBER_ATK2
                    case S_309: // S_CYBER_ATK4
                    case S_311: // S_CYBER_ATK6
                        sfx = sfx_missile; // sfx_rlaunc
                        break;

                    case S_328: // S_PAIN_ATK3
                        sfx = sfx_skullatk; // sfx_sklatk
                        break;

                    //case S_VILE_ATK2:
                        //sfx = sfx_vilatk;
                        //break;

                    //case S_SKEL_FIST2:
                        //sfx = sfx_skeswg;
                        //break;

                    //case S_SKEL_FIST4:
                        //sfx = sfx_skepch;
                        //break;

                    //case S_SKEL_MISS2:
                        //sfx = sfx_skeatk;
                        //break;

                    //case S_CPOS_ATK2:
                    //case S_CPOS_ATK3:
                    //case S_CPOS_ATK4:
                        //sfx = sfx_pistol;
                        //break;

                    //case S_SPID_ATK2:
                    //case S_SPID_ATK3:
                        //sfx = sfx_pistol;
                        //break;

                    default:
                        sfx = 0;
                        break;
                }

                if (sfx)
                    S_StartSound(NULL, sfx);
			}

            break;

        default:
            break;
    }

	return ga_nothing;
}

/*
=================
=
= F_Drawer/Cast_Drawer
=
=================
*/
void F_Drawer(void) // 800039DC
{
	unsigned char i;
	int type, alpha;
	short ypos;

	I_ClearFrame();

    gDPPipeSync(GFX1++);
    gDPSetCycleType(GFX1++, G_CYC_FILL);
    gDPSetRenderMode(GFX1++,G_RM_NOOP,G_RM_NOOP2);
    gDPSetColorImage(GFX1++, G_IM_FMT_RGBA, G_IM_SIZ_32b, SCREEN_WD, OS_K0_TO_PHYSICAL(cfb[vid_side]));
    // Fill borders with black
    gDPSetFillColor(GFX1++, GPACK_RGBA5551(0,0,0,0) << 16 | GPACK_RGBA5551(0,0,0,0)) ;
    gDPFillRectangle(GFX1++, 0, 0, SCREEN_WD-1, SCREEN_HT-1);

    switch(finalestage)
    {
        case F_STAGE_FADEIN_BACKGROUD:
        case F_STAGE_FADEOUT_BACKGROUD:
			if (gamemap == 39)
				M_DrawBackground(0, 0, fadeinout, "FINLOST");
			else if (gamemap == 65)
				M_DrawBackground(0, 0, fadeinout, "FINBETA");
			else
				M_DrawBackground(0, 0, fadeinout, "FINAL");
            break;

        case F_STAGE_DRAWTEXT:
        case F_STAGE_SCROLLTEXT:
			if (gamemap == 39)
				M_DrawBackground(0, 0, fadeinout, "FINLOST");
			else if (gamemap == 65)
				M_DrawBackground(0, 0, fadeinout, "FINBETA");
			else
				M_DrawBackground(0, 0, fadeinout, "FINAL");

            ypos = textypos;
            for(i = 0; i < textline; i++)
            {
				if (ypos >= 16)
                    alpha = 0xff;
                else if (ypos > 0)
                    alpha = ypos<<4;
                else
                    alpha = 0;
				
                ST_DrawString(-1, ypos, text[i], PACKRGBA(255, alpha, alpha, alpha));
                ypos += 14;
            }

            ST_DrawString(-1, ypos, text[i], PACKRGBA(255, textalpha, textalpha, textalpha));
            break;

        case F_STAGE_CAST:
			M_DrawBackground(63, 25, 128, EpisodeGraphic());

            type = castorder[castnum].type;

            if (type == MT_DEMON2){
                alpha = 48;
            }
            else {
                alpha = mobjinfo[type].alpha;
            }

            BufferedDrawSprite(type, caststate, castrotation,
                               PACKRGBA(castfadein, castfadein, castfadein, alpha), 160, 190);

            ST_DrawString(-1, 208, castorder[castnum].name, 0xC00000ff);
            break;

        default:
            break;
    }

	if (MenuCall)
    {
        M_DrawOverlay(0, 0, 320, 240, 96);
        MenuCall();
    }

    I_DrawFrame();
}

void BufferedDrawSprite(int type, state_t *state, int rotframe, int color, int xpos, int ypos) // 80003D1C
{
    spritedef_t     *sprdef;
	spriteframe_t   *sprframe;
	int			    lump;
	boolean		    flip;

	spriteN64_t *data;
	byte *paldata;
	byte *src;

	int compressed;
    int tileh;
    int tilew;
    int height;
    int width;
    int width2;
    int tiles;
    int xoffs;
    int yoffs;

    int tilecnt = 0;
    int dsdx;
    int spos;
    int tpos;
    int x1;
    int y1;
    int xh;
    int yh;

    // draw the current frame in the middle of the screen
    sprdef = &sprites[state->sprite];
    sprframe = &sprdef->spriteframes[state->frame & FF_FRAMEMASK];
    lump = sprframe->lump[rotframe];
    flip = (boolean)sprframe->flip[rotframe];

	gDPPipeSync(GFX1++);
	gDPSetCycleType(GFX1++, G_CYC_1CYCLE);
	gDPSetTexturePersp(GFX1++, G_TP_NONE);
	gDPSetTextureLUT(GFX1++, G_TT_RGBA16);
	gDPSetAlphaCompare(GFX1++, G_AC_THRESHOLD);
	gDPSetBlendColor(GFX1++, 0, 0, 0, 0);
	gDPSetCombineMode(GFX1++, G_CC_D64COMB04, G_CC_D64COMB04);

	gDPSetPrimColorD64(GFX1++, 0, 0, color);

    if ((color & 255) < 255) {
        gDPSetRenderMode(GFX1++, G_RM_XLU_SURF_CLAMP, G_RM_XLU_SURF2_CLAMP);
    }
    else {
        gDPSetRenderMode(GFX1++, G_RM_TEX_EDGE, G_RM_TEX_EDGE2);
    }


    data = W_CacheLumpNum(lump, PU_CACHE);

    compressed = data->compressed;
    tileh = data->tileheight;
    width = data->width;
    height = data->height;
    tiles = data->tiles;
    xoffs = data->xoffs;
    yoffs = data->yoffs;

    src = data->texels;

    if (compressed < 0)
    {
        width2 = (width + 7) & ~7;
        tilew = tileh * width2;

        if (data->cmpsize & 1)
        {
            paldata = (byte *)(W_CacheLumpNum(((mobjinfo[type].palette + lump) -
                                    (data->cmpsize >> 1)), PU_CACHE)) + 8;
        }
        else
        {
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
    else
    {
        width2 = (width + 15) & ~15;
        tilew = tileh * width2;

        if (tilew < 0) {
            tilew = tilew + 1;
        }

        tilew >>= 1;

        /* Load Palette Data (16 colors) */
        gDPSetTextureImage(GFX1++, G_IM_FMT_RGBA, G_IM_SIZ_16b , 1, (src + data->cmpsize));

        gDPTileSync(GFX1++);
        gDPSetTile(GFX1++, G_IM_FMT_RGBA, G_IM_SIZ_4b, 0, 256, G_TX_LOADTILE, 0, 0, 0, 0, 0, 0, 0);

        gDPLoadSync(GFX1++);
        gDPLoadTLUTCmd(GFX1++, G_TX_LOADTILE, 15);

        gDPPipeSync(GFX1++);
    }

    if (!flip)
    {
        x1 = (xpos - xoffs) << 2;
        xh = (x1 + (width << 2));
        spos = 0;
        dsdx = 1;
    }
    else
    {
        xh = (xpos + xoffs) << 2;
        x1 = (xh - (width << 2));
        spos = (width - 1);
        dsdx = 63;
    }

    y1 = (ypos - yoffs) << 2;

    if (tiles > 0)
    {
        do
        {
            if (tileh < height)
                tpos = tileh;
            else
                tpos = height;

            if (compressed < 0)
            {
                /* Load Image Data (8bit) */
                gDPSetTextureImage(GFX1++, G_IM_FMT_CI, G_IM_SIZ_16b , 1, src);
                gDPSetTile(GFX1++, G_IM_FMT_CI, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, 0, 0, 0, 0, 0, 0);

                gDPLoadSync(GFX1++);
                gDPLoadBlock(GFX1++, G_TX_LOADTILE, 0, 0, ((width2 * tpos + 1) >> 1) - 1, 0);

                gDPPipeSync(GFX1++);
                gDPSetTile(GFX1++, G_IM_FMT_CI, G_IM_SIZ_8b, ((width2 + 7) >> 3), 0,
                               G_TX_RENDERTILE , 0, 0, 0, 0, 0, 0, 0);

                gDPSetTileSize(GFX1++, G_TX_RENDERTILE, 0, 0, ((width2 - 1) << 2), (tpos - 1) << 2);
            }
            else
            {
                /* Load Image Data (4bit) */
                gDPSetTextureImage(GFX1++, G_IM_FMT_CI, G_IM_SIZ_16b , 1, src);
                gDPSetTile(GFX1++, G_IM_FMT_CI, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, 0, 0, 0, 0, 0, 0);

                gDPLoadSync(GFX1++);
                gDPLoadBlock(GFX1++, G_TX_LOADTILE, 0, 0, ((width2 * tpos + 3) >> 2) - 1, 0);

                gDPPipeSync(GFX1++);
                gDPSetTile(GFX1++, G_IM_FMT_CI, G_IM_SIZ_4b, (((width2>>1) + 7) >> 3), 0,
                           G_TX_RENDERTILE , 0, 0, 0, 0, 0, 0, 0);

                gDPSetTileSize(GFX1++, G_TX_RENDERTILE, 0, 0, ((width2 - 1) << 2), (tpos - 1) << 2);
            }

            yh = (y1 + (tpos << 2));

            gSPTextureRectangle(GFX1++,
                                x1, y1,
                                xh, yh,
                                G_TX_RENDERTILE,
                                (spos << 5), (0 << 5),
                                (dsdx << 10), (1 << 10));

            height -= tpos;

            src += tilew;
            y1 = yh;
            tilecnt += 1;
        } while (tilecnt != tiles);
    }

    globallump = -1;
}
