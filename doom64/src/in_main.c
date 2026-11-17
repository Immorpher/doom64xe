
/* in_main.c -- intermission */

#include "doomdef.h"
#include "st_main.h"
#include "r_local.h"
#include "texts.h"

extern unsigned char nextmap;


// Beta 64

// Vehemence maps


// Bonus Pak Maps


typedef struct pstats_s
{
	int		killpercent;
	int		itempercent;
	int		secretpercent;
} pstats_t;

int			killvalue, itemvalue, secretvalue; // 800633B8, 800633BC, 800633C0
pstats_t	pstats; // 800633C4

// used to accelerate or skip a stage
int acceleratestage; // 800633B0
int nextstage;       // 800633B4

char timetext[16];  // 800633D0
int start_time;     // 80063390
int end_time;       // 80063394

void IN_Start(void) // 80004AF0
{
	int time;

    killvalue = itemvalue = secretvalue = -1;

    if (totalkills)
        pstats.killpercent = (players[0].killcount * 100) / totalkills;
    else
        pstats.killpercent = 100;

    if (totalitems)
        pstats.itempercent = (players[0].itemcount * 100) / totalitems;
    else
        pstats.itempercent = 100;

    if (totalsecret)
        pstats.secretpercent = (players[0].secretcount * 100) / totalsecret;
    else
        pstats.secretpercent = 100;

    time = (unsigned int)(end_time - start_time) / 60;

    if ((time / 60) < 100) {
        sprintf(timetext, "%02d:%02d", (time / 60), (time % 60));
    }
    else {
        D_memmove(timetext, "99:99");
    }

    nextstage = 0;
	acceleratestage = 0;
	last_ticon = 0;
	text_alpha = 255;

    if ((nextmap > 0) && (nextmap < LASTLEVEL)) // Show password only for normal levels
	{
		M_EncodePassword(Passwordbuff);
        CurPasswordSlot = 16;
	}

	S_StartMusic(mus_complete);
}

void IN_Stop(int exit) // 80004DB0
{

    if ((nextmap > 0) && (nextmap < LASTLEVEL) && !FUNLEVEL(gamemap)) // No memory pak on fun levels
    {
        if (EnableMemPak) {
            MiniLoop(M_SavePakStart,M_SavePakStop,M_SavePakTicker,M_SavePakDrawer);
        }
    }
	
	S_StopMusic();

    I_WIPE_FadeOutScreen();
}

int IN_Ticker(void) // 80004E24
{
	boolean state;
	int buttons, oldbuttons;

    buttons = ticbuttons[0] & 0xffff0000;
    oldbuttons = oldticbuttons[0] & 0xffff0000;

    if ((buttons != oldbuttons) && (buttons & (PAD_A|PAD_B|PAD_START|ALL_TRIG|ALL_CBUTTONS)))
    {
        acceleratestage++;
        if (acceleratestage == 1)
        {
            killvalue = pstats.killpercent;
            itemvalue = pstats.itempercent;
            secretvalue = pstats.secretpercent;
            nextstage = 5;
            last_ticon = 0;
            S_StartSound(NULL, sfx_explode);
        }
        else if (acceleratestage >= 2)
        {
            S_StartSound(NULL, sfx_explode);
            return ga_exit;
        }
    }

    if (last_ticon)
    {
        if ((ticon - last_ticon) <= 90) // 3 * TICRATE
            return ga_nothing;
    }

    state = false;

    switch(nextstage)
    {
        case 0:
            S_StartSound(NULL, sfx_explode);
            nextstage = 1;
            break;

        case 1: // kills
            killvalue += 4;
            if (killvalue > pstats.killpercent)
            {
                S_StartSound(NULL, sfx_explode);
                killvalue = pstats.killpercent;
                last_ticon = ticon;
                nextstage = 2;
            }
            else {
                state = true;
            }
            break;

        case 2: // item
            itemvalue += 4;
            if (itemvalue > pstats.itempercent)
            {
                S_StartSound(NULL, sfx_explode);
                itemvalue = pstats.itempercent;
                last_ticon = ticon;
                nextstage = 3;
            }
            else {
                state = true;
            }
            break;

        case 3: // secret
            secretvalue += 4;
            if (secretvalue > pstats.secretpercent)
            {
                S_StartSound(NULL, sfx_explode);
                secretvalue = pstats.secretpercent;
                last_ticon = ticon;
                nextstage = 4;
            }
            else {
                state = true;
            }
            break;

        case 4:
            S_StartSound(NULL, sfx_explode);
            last_ticon = ticon;
            nextstage = 5;
            break;
    }

    if (!state && (acceleratestage == 0))
    {
        if (nextstage == 5) {
            acceleratestage = 1;
        }
    }

    // Play Sound sfx_pistol
    if (!(gametic & 3) && state)
    {
        S_StartSound(NULL, sfx_pistol);
    }

	return ga_nothing;
}

void IN_Drawer(void) // 80005164
{
	short i;
	int c;
	char password[32];
	char *pbuff;

	I_ClearFrame();

	gDPPipeSync(GFX1++);
	gDPSetCycleType(GFX1++, G_CYC_FILL);
	gDPSetRenderMode(GFX1++, G_RM_NOOP, G_RM_NOOP2);
	gDPSetColorImage(GFX1++, G_IM_FMT_RGBA, G_IM_SIZ_32b, SCREEN_WD, OS_K0_TO_PHYSICAL(cfb[vid_side]));
	// Fill borders with black
    gDPSetFillColor(GFX1++, GPACK_RGBA5551(0,0,0,0) << 16 | GPACK_RGBA5551(0,0,0,0)) ;
    gDPFillRectangle(GFX1++, 0, 0, SCREEN_WD-1, SCREEN_HT-1);

	M_DrawBackground(63, 25, 128, EpisodeGraphic());

	ST_DrawString(-1, 20, TX_MapName(gamemap), PACKRGBA(255, 255, 255, text_alpha));

	ST_DrawString(-1, 36, "Finished", PACKRGBA(255, 255, 255, text_alpha));

    if ((nextstage > 0) && (killvalue > -1))
    {
        ST_DrawString(57, 60, "Kills", PACKRGBA(255, 0, 0, text_alpha));
        ST_DrawString(248, 60, "%", PACKRGBA(255, 0, 0, text_alpha));
        ST_DrawNumber(210, 60, killvalue, 1, PACKRGBA(255, 0, 0, text_alpha));
    }

    if ((nextstage > 1) && (itemvalue > -1))
    {
        ST_DrawString(57, 78, "Items", PACKRGBA(255, 0, 0, text_alpha));
        ST_DrawString(248, 78, "%", PACKRGBA(255, 0, 0, text_alpha));
        ST_DrawNumber(210, 78, itemvalue, 1, PACKRGBA(255, 0, 0, text_alpha));
    }

    if ((nextstage > 2) && (secretvalue > -1))
    {
        ST_DrawString(57, 99, "Secrets", PACKRGBA(255, 0, 0, text_alpha));
        ST_DrawString(248, 99, "%", PACKRGBA(255, 0, 0, text_alpha));
        ST_DrawNumber(210, 99, secretvalue, 1, PACKRGBA(255, 0, 0, text_alpha));
    }

    if ((nextstage > 3))
    {
        ST_DrawString(57, 120, "Time", PACKRGBA(255, 0, 0, text_alpha));
        ST_DrawString(210, 120, timetext, PACKRGBA(255, 0, 0, text_alpha));
    }

	if ((nextstage > 4) && (FINLEVEL(gamemap) || FUNLEVEL(gamemap))) // [Immorpher] Ignore finale and fun levels
	{
		// nothing
	}
	else if (nextstage > 4)
	{
        ST_DrawString(-1, 145, "Approaching", PACKRGBA(255, 255, 255, text_alpha));
        ST_DrawString(-1, 161, TX_MapName(nextmap), PACKRGBA(255, 255, 255, text_alpha));

        pbuff = password;
		for (i = 0; i < 16; i++)
		{
            c = i & 3;
            if ((i < 0) && (c != 0)) {
                c -= 4;
            }
            if (c == 0) {
                *pbuff++ = ' ';
            }

			*pbuff++ = passwordChar[Passwordbuff[i]];
		}
		*pbuff = 0;

		if (nextmap != 0) { // [Immorpher] Dont show password for bonus pak
			ST_DrawString(-1, 187, "Password", PACKRGBA(255, 255, 255, text_alpha));
			ST_DrawString(-1, 203, password, PACKRGBA(255, 255, 255, text_alpha));
		}
	}

	I_DrawFrame();
}
