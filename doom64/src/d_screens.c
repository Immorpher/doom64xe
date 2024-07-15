/* D_screens.c */

#include "i_main.h"
#include "doomdef.h"
#include "r_local.h"
#include "st_main.h"

// [Immorpher] Used for selecting the proper graphic to show for each episode
char* EpisodeGraphic()
{
	if ((gamemap >= LOSTLEVEL) && (gamemap < BETALEVEL)) {
		return "LOST";
	}
	else if ((gamemap >= BETALEVEL) && (gamemap < BONUSLEVEL)) {
		return "BETA";
	}
	else if (gamemap >= BONUSLEVEL || gamemap == 0) {
		return "BONUS";
	}
	else {
		return "EVIL";
	}
}

int D_RunDemo(char *name, skill_t skill, int map) // 8002B2D0
{
  int lump;
  int exit;

  demo_p = Z_Alloc(16000, PU_STATIC, NULL);

  lump = W_GetNumForName(name);
  W_ReadLump(lump, demo_p, dec_d64);
  exit = G_PlayDemoPtr(skill, map);
  Z_Free(demo_p);

  return exit;
}

extern int DefaultConfiguration[5][13];
int D_TitleMap(void) // 8002B358
{
  int exit;

  D_OpenControllerPak();

  demo_p = Z_Alloc(16000, PU_STATIC, NULL);
  bzero(demo_p, 16000);
  D_memcpy(demo_p, DefaultConfiguration[0], 13*sizeof(int));
  exit = G_PlayDemoPtr(sk_medium, 33);
  Z_Free(demo_p);

  return exit;
}

int D_WarningTicker(void) // 8002B3E8
{
    if ((gamevbls < gametic) && !(gametic & 7))
        MenuAnimationTic = (MenuAnimationTic + 1) & 7;
    return 0;
}

void D_DrawWarning(void) // 8002B430
{
    I_ClearFrame();

    gDPPipeSync(GFX1++);
    gDPSetCycleType(GFX1++, G_CYC_FILL);
    gDPSetRenderMode(GFX1++,G_RM_NOOP,G_RM_NOOP2);
    gDPSetColorImage(GFX1++, G_IM_FMT_RGBA, G_IM_SIZ_32b, SCREEN_WD, OS_K0_TO_PHYSICAL(cfb[vid_side]));
    gDPSetFillColor(GFX1++, GPACK_RGBA5551(0,0,0,1) << 16 | GPACK_RGBA5551(0,0,0,1));
    gDPFillRectangle(GFX1++, 0, 0, SCREEN_WD-1, SCREEN_HT-1);

    if (MenuAnimationTic & 1)
        ST_DrawString(-1,  30, "WARNING!", 0xc00000ff);

    ST_DrawString(-1,  60, "nintendo 64 controller", 0xffffffff);
    ST_DrawString(-1,  80, "is not connected.", 0xffffffff);
    ST_DrawString(-1, 120, "please turn off your", 0xffffffff);
    ST_DrawString(-1, 140, "nintendo 64 system.", 0xffffffff);
    ST_DrawString(-1, 180, "plug in your nintendo 64", 0xffffffff);
    ST_DrawString(-1, 200, "controller and turn it on.", 0xffffffff);

    I_DrawFrame();
}

int D_LegalTicker(void) // 8002B5F8
{
    if ((ticon - last_ticon) >= 150) // 5 * TICRATE
    {
        text_alpha -= 8;
        if (text_alpha < 0)
        {
            text_alpha = 0;
            return 8;
        }
    }
    return 0;
}

void D_DrawSplash(void) // 8002B644
{
    I_ClearFrame();

    gDPPipeSync(GFX1++);
    gDPSetCycleType(GFX1++, G_CYC_FILL);
    gDPSetRenderMode(GFX1++,G_RM_NOOP,G_RM_NOOP2);
    gDPSetColorImage(GFX1++, G_IM_FMT_RGBA, G_IM_SIZ_32b, SCREEN_WD, OS_K0_TO_PHYSICAL(cfb[vid_side]));
    gDPSetFillColor(GFX1++, GPACK_RGBA5551(0,0,0,1) << 16 | GPACK_RGBA5551(0,0,0,1));
    gDPFillRectangle(GFX1++, 0, 0, SCREEN_WD-1, SCREEN_HT-1);

    M_DrawBackground(0, 0, text_alpha, "SPLASH");

    if (FilesUsed > -1) {
        ST_DrawString(-1, 200, "hold \x8d to manage pak", text_alpha | 0xffffff00);
    }

    I_DrawFrame();
}

int D_NoPakTicker(void) // 8002B7A0
{
    if ((ticon - last_ticon) >= 240) // 8 * TICRATE
        return 8;

    return 0;
}

void D_DrawNoPak(void) // 8002B7F4
{
    I_ClearFrame();

    gDPPipeSync(GFX1++);
    gDPSetCycleType(GFX1++, G_CYC_FILL);
    gDPSetRenderMode(GFX1++,G_RM_NOOP,G_RM_NOOP2);
    gDPSetColorImage(GFX1++, G_IM_FMT_RGBA, G_IM_SIZ_32b, SCREEN_WD, OS_K0_TO_PHYSICAL(cfb[vid_side]));
    gDPSetFillColor(GFX1++, GPACK_RGBA5551(0,0,0,1) << 16 | GPACK_RGBA5551(0,0,0,1));
    gDPFillRectangle(GFX1++, 0, 0, SCREEN_WD-1, SCREEN_HT-1);
	
	M_DrawBackground(0, 0, text_alpha, "SPLASH");

    ST_DrawString(-1,  40, "no controller pak.", 0xffffffff);
    ST_DrawString(-1,  60, "your game cannot", 0xffffffff);
    ST_DrawString(-1,  80, "be saved.", 0xffffffff);
    ST_DrawString(-1, 120, "please turn off your", 0xffffffff);
    ST_DrawString(-1, 140, "nintendo 64 system", 0xffffffff);
    ST_DrawString(-1, 160, "before inserting a", 0xffffffff);
    ST_DrawString(-1, 180, "controller pak.", 0xffffffff);

    I_DrawFrame();
}

void D_DrawNoMemory(void)
{
    I_ClearFrame();

    gDPPipeSync(GFX1++);
    gDPSetCycleType(GFX1++, G_CYC_FILL);
    gDPSetRenderMode(GFX1++,G_RM_NOOP,G_RM_NOOP2);
    gDPSetColorImage(GFX1++, G_IM_FMT_RGBA, G_IM_SIZ_32b, SCREEN_WD, OS_K0_TO_PHYSICAL(cfb[vid_side]));
    gDPSetFillColor(GFX1++, GPACK_RGBA5551(0,0,0,1) << 16 | GPACK_RGBA5551(0,0,0,1));
    gDPFillRectangle(GFX1++, 0, 0, SCREEN_WD-1, SCREEN_HT-1);
	
	M_DrawBackground(0, 0, text_alpha, "SPLASH");

    ST_DrawString(-1,  20, "no expansion pak.", 0xffffffff);
    ST_DrawString(-1,  40, "complex levels outside", 0xffffffff);
    ST_DrawString(-1,  60, "the original campaign", 0xffffffff);
    ST_DrawString(-1,  80, "may not be rendered", 0xffffffff);
    ST_DrawString(-1, 100, "properly or crash.", 0xffffffff);
    ST_DrawString(-1, 140, "please turn off your", 0xffffffff);
    ST_DrawString(-1, 160, "nintendo 64 system", 0xffffffff);
    ST_DrawString(-1, 180, "before inserting an", 0xffffffff);
    ST_DrawString(-1, 200, "expansion pak.", 0xffffffff);

    I_DrawFrame();
}

void D_DrawLoadScreen(int textalpha)
{
	if (demoplayback || runintroduction) // No loading screens for demos and introductions
		return;

    I_ClearFrame();

    gDPPipeSync(GFX1++);
    gDPSetCycleType(GFX1++, G_CYC_FILL);
    gDPSetRenderMode(GFX1++,G_RM_NOOP,G_RM_NOOP2);
    gDPSetColorImage(GFX1++, G_IM_FMT_RGBA, G_IM_SIZ_32b, SCREEN_WD, OS_K0_TO_PHYSICAL(cfb[vid_side]));
    gDPSetFillColor(GFX1++, GPACK_RGBA5551(0,0,0,1) << 16 | GPACK_RGBA5551(0,0,0,1));
    gDPFillRectangle(GFX1++, 0, 0, SCREEN_WD-1, SCREEN_HT-1);
	
	M_DrawBackground(63, 25, textalpha>>1, EpisodeGraphic());

	ST_DrawString(-1, 100, "Entering", PACKRGBA(textalpha,0,0,textalpha));
	ST_DrawString(-1, 120, MapInfo[gamemap].name, PACKRGBA(textalpha,0,0,textalpha));

    I_DrawFrame();
}

void D_SplashScreen(void) // 8002B988
{
    /* */
	/* Check if the n64 control is connected */
	/* if not connected, it will show the Warning screen */
	/* */
    if ((gamepad_bit_pattern & 1) == 0) {
        MiniLoop(NULL, NULL,D_WarningTicker,D_DrawWarning);
    }

    /* */
    /* Check if the n64 controller Pak is connected */
    /* */
    I_CheckControllerPak();

    /* */
    /* if not connected, it will show the NoPak screen */
    /* */
    if (FilesUsed < 0) {
        last_ticon = 0;
        MiniLoop(NULL, NULL, D_NoPakTicker, D_DrawNoPak);
    }

    /* */
    /* Check if expansion pak is connected if >8MB memory */
    /* */
    if (osGetMemSize() < 0x800000)
    {
        MiniLoop(NULL, NULL, D_NoPakTicker, D_DrawNoMemory);
    }

    /* */
    /* show the legals screen */
    /* */

    text_alpha = 0xff;
    last_ticon = 0;
    MiniLoop(NULL, NULL, D_LegalTicker, D_DrawSplash);
}

static int cred_step;   // 800B2210
static int cred1_alpha; // 800B2214
static int cred_next;   // 800B2218

int D_Credits(void) // 8002BA34
{
    int exit;

    cred_next = 0;
    cred1_alpha = 0;
    cred_step = 0;
    exit = MiniLoop(NULL, NULL, D_CreditTicker, D_CreditDrawer);

    return exit;
}

int D_CreditTicker(void) // 8002BA88
{
    if ((cred_next == 0) || (cred_next == 1))
    {
        if (cred_step == 0)
        {
            cred1_alpha += 8;
            if (cred1_alpha >= 255)
            {
                cred1_alpha = 0xff;
                cred_step = 1;
            }
        }
        else if (cred_step == 1)
        {
            if (((u32)ticbuttons[0] >> FRACBITS) != 0)
                cred_step = 2;
        }
        else
        {
			cred_next += 1;
			cred1_alpha = 0;
			cred_step = 0;
        }
    }
    else if (cred_next == 2)
        return ga_exitdemo;

    return ga_nothing;
}

void D_CreditDrawer(void) // 8002BBE4
{
    I_ClearFrame();

    gDPPipeSync(GFX1++);
    gDPSetCycleType(GFX1++, G_CYC_FILL);
    gDPSetRenderMode(GFX1++,G_RM_NOOP,G_RM_NOOP2);
    gDPSetColorImage(GFX1++, G_IM_FMT_RGBA, G_IM_SIZ_32b, SCREEN_WD, OS_K0_TO_PHYSICAL(cfb[vid_side]));

    if (cred_next == 0)
    {

        M_DrawBackground(0, 0, cred1_alpha, "OGCRED");
    }
    else
    {
        if ((cred_next == 1) || (cred_next == 2))
        {

            M_DrawBackground(0, 0, cred1_alpha, "XECRED");
        }
    }

    I_DrawFrame();
}

void D_OpenControllerPak(void) // 8002BE28
{
    unsigned int oldbuttons;

    oldbuttons = I_GetControllerData();

    if (((oldbuttons & 0xffff0000) == PAD_START) && (I_CheckControllerPak() == 0))
    {
        MenuCall = M_ControllerPakDrawer;
        linepos = 0;
        cursorpos = 0;

        MiniLoop(M_FadeInStart, M_MenuClearCall, M_ScreenTicker, M_MenuGameDrawer);
        I_WIPE_FadeOutScreen();
    }
    return;
}
