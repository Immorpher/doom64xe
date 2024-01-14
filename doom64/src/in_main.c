
/* in_main.c -- intermission */

#include "doomdef.h"
#include "st_main.h"
#include "r_local.h"

extern int nextmap;

#define T_NULL	    ""

#define MI_TXT00	"Bonus Pak" // bonus hub!
#define MI_TXT01	"Staging Area"
#define MI_TXT02	"The Terraformer"
#define MI_TXT03	"Main Engineering"
#define MI_TXT04	"Holding Area"
#define MI_TXT05	"Tech Center"
#define MI_TXT06	"Alpha Quadrant"
#define MI_TXT07	"Research Lab"
#define MI_TXT08	"Final Outpost"
#define MI_TXT09	"Even Simpler"
#define MI_TXT10	"The Bleeding"
#define MI_TXT11	"Terror Core"
#define MI_TXT12	"Altar of Pain"
#define MI_TXT13	"Dark Citadel"
#define MI_TXT14	"Eye of the Storm"
#define MI_TXT15	"Dark Entries"
#define MI_TXT16	"Blood Keep"
#define MI_TXT17	"Watch Your Step"
#define MI_TXT18	"Spawned Fear"
#define MI_TXT19	"The Spiral"
#define MI_TXT20	"Breakdown"
#define MI_TXT21	"Pitfalls"
#define MI_TXT22	"Burnt Offerings"
#define MI_TXT23	"Unholy Temple"
#define MI_TXT24	"No Escape"
#define MI_TXT25	"Cat And Mouse"
#define MI_TXT26	"HardCore"
#define MI_TXT27	"Playground"
#define MI_TXT28	"The Absolution"
#define MI_TXT29	"Outpost Omega"
#define MI_TXT30	"The Lair"
#define MI_TXT31	"In The Void"
#define MI_TXT32	"Hectic"
#define MI_TXT33	"Title"
#define MI_TXT34	"Plant Ops" // Lost levels
#define MI_TXT35	"Evil Sacrifice"
#define MI_TXT36	"Cold Grounds"
#define MI_TXT37	"Wretched Vats"
#define MI_TXT38	"Thy Glory"
#define MI_TXT39	"Final Judgement"
#define MI_TXT40	"Panic"

// Beta 64
#define MI_TXT41	"Penumbra"
#define MI_TXT42	"Hydroplant"
#define MI_TXT43	"Signal Processing"
#define MI_TXT44	"Disposal Center"
#define MI_TXT45	"Refinery"
#define MI_TXT46	"Gamma Lab"
#define MI_TXT47	"Sector R"
#define MI_TXT48	"Ultima Porta"
#define MI_TXT49	"Profane Paradise"
#define MI_TXT50	"Pain Warren"
#define MI_TXT51	"City of the Damned"
#define MI_TXT52	"Mines"
#define MI_TXT53	"Lair of the Blind"
#define MI_TXT54	"The Dungeons"
#define MI_TXT55	"Abhorred Sanctuary"
#define MI_TXT56	"Abaddon Fortress"
#define MI_TXT57	"Simplicity"
#define MI_TXT58	"Inferni Carceris"
#define MI_TXT59	"Umbra"
#define MI_TXT60	"Unearthly Spire"
#define MI_TXT61	"Putrid Cloister"
#define MI_TXT62	"Tartarus"
#define MI_TXT63	"Crimson Court"
#define MI_TXT64	"Nowhere to Hide"
#define MI_TXT65	"The Nest"
#define MI_TXT66	"Hubris" // Fun level
#define MI_TXT67	"Folly" // Fun level
#define MI_TXT68	"Regret" // Fun level
#define MI_TXT69	"Rage" // Fun level
#define MI_TXT70	"The Beyond" // Secret level
#define MI_TXT71	"Irradiated" // Secret level
#define MI_TXT72	"Dark Secrets" // Secret level
#define MI_TXT73	"Sorrow" // Fun level

// Bonus Pak Maps
#define MI_TXT129	"Anguish"
#define MI_TXT130	"Wretched"
#define MI_TXT131	"Entryway"
#define MI_TXT132	"Visions of Hostility"
#define MI_TXT133	"Secondary Engineering"
#define MI_TXT134	"Hangar"
#define MI_TXT135	"Fabrication Labs"
#define MI_TXT136	"Switchless"
#define MI_TXT137	"Devolving Being"
#define MI_TXT138	"Ring of Fire"
#define MI_TXT139	"Doomsday Keep"
#define MI_TXT140	"Dark Retrospect"

mapinfo_t MapInfo[] = //8005A478
{
    {MI_TXT00 , 116}, // Bonus Pak hub with menu music
    {MI_TXT01 , 96}, // Doom 64
    {MI_TXT02 , 97},
    {MI_TXT03 , 105},
    {MI_TXT04 , 104},
    {MI_TXT05 , 101},
    {MI_TXT06 , 107},
    {MI_TXT07 , 108},
    {MI_TXT08 , 110},
    {MI_TXT09 , 95},
    {MI_TXT10 , 98},
    {MI_TXT11 , 99},
    {MI_TXT12 , 102},
    {MI_TXT13 , 93},
    {MI_TXT14 , 106},
    {MI_TXT15 , 111},
    {MI_TXT16 , 97},
    {MI_TXT17 , 103},
    {MI_TXT18 , 94},
    {MI_TXT19 , 100},
    {MI_TXT20 , 112},
    {MI_TXT21 , 109},
    {MI_TXT22 , 101},
    {MI_TXT23 , 108},
    {MI_TXT24 , 98},
    {MI_TXT25 , 97},
    {MI_TXT26 , 98},
    {MI_TXT27 , 94},
    {MI_TXT28 , 99},
    {MI_TXT29 , 101},
    {MI_TXT30 , 102},
    {MI_TXT31 , 103},
    {MI_TXT32 , 104},
    {MI_TXT33 , 115},
    {MI_TXT34 , 100}, // Lost Levels
    {MI_TXT35 , 95},
    {MI_TXT36 , 111},
    {MI_TXT37 , 94},
    {MI_TXT38 , 105},
    {MI_TXT39 , 98},
    {MI_TXT40 , 101},
    {MI_TXT41 , 96}, // Beta 64
    {MI_TXT42 , 97},
    {MI_TXT43 , 105},
    {MI_TXT44 , 104},
    {MI_TXT45 , 101},
    {MI_TXT46 , 107},
    {MI_TXT47 , 108},
    {MI_TXT48 , 110},
    {MI_TXT49 , 95},
    {MI_TXT50 , 98},
    {MI_TXT51 , 99},
    {MI_TXT52 , 102},
    {MI_TXT53 , 93},
    {MI_TXT54 , 106},
    {MI_TXT55 , 111},
    {MI_TXT56 , 97},
    {MI_TXT57 , 103},
    {MI_TXT58 , 94},
    {MI_TXT59 , 100},
    {MI_TXT60 , 112},
    {MI_TXT61 , 109},
    {MI_TXT62 , 101},
    {MI_TXT63 , 108},
    {MI_TXT64 , 98},
    {MI_TXT65 , 97},
    {MI_TXT66 , 98},
    {MI_TXT67 , 94},
    {MI_TXT68 , 99},
    {MI_TXT69, 101},
    {MI_TXT70 , 102},
    {MI_TXT71 , 103},
    {MI_TXT72 , 104},
    {MI_TXT73 , 100},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {T_NULL , 0},
    {MI_TXT129 , 106}, // Anguish - Map 129 Bonus Pak Maps begin
    {MI_TXT130 , 99}, // Wretched
    {MI_TXT131 , 104}, // Entryway
    {MI_TXT132 , 97}, // Visions of Hostility
    {MI_TXT133 , 108}, // Secondary Engineering
    {MI_TXT134 , 96}, // Hangar
    {MI_TXT135 , 105}, // Fabrication Labs
    {MI_TXT136 , 94}, // Switchless
    {MI_TXT137 , 110}, // Devolving Being
    {MI_TXT138 , 95}, // Ring of Fire
    {MI_TXT139 , 103}, // Doomsday Keep
    {MI_TXT140 , 107}, // Dark Retrospect
    {T_NULL , 0}
};

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

    if ((time / 60) < 60) {
        sprintf(timetext, "%2.2d:%2.2d", (time / 60), (time % 60));
    }
    else {
        D_memmove(timetext, "--:--");
    }

    nextstage = 0;
	acceleratestage = 0;
	last_ticon = 0;
	text_alpha = 255;

    if ((nextmap > 0) && (nextmap < LASTLEVEL))
	{
		M_EncodePassword(Passwordbuff);
        CurPasswordSlot = 16;
	}

	S_StartMusic(mus_complete);
}

void IN_Stop(void) // 80004DB0
{

    if ((nextmap > 0) && (nextmap < LASTLEVEL))
    {
        if (EnableExpPak) {
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
	int i, c;
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

	if ((gamemap >= LOSTLEVEL) && (gamemap < BETALEVEL)) {
		M_DrawBackground(63, 25, 128, "LOST");
	}
	else if ((gamemap >= BETALEVEL) && (gamemap < BONUSLEVEL)) {
		M_DrawBackground(63, 25, 128, "BETA");
	}
	else if (gamemap >= BONUSLEVEL || gamemap == 0) {
		M_DrawBackground(63, 25, 128, "BONUS");
	}
	else {
		M_DrawBackground(63, 25, 128, "EVIL");
	}

	ST_DrawString(-1, 20, MapInfo[gamemap].name, PACKRGBA(255, 255, 255, text_alpha));

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
        ST_DrawString(-1, 145, "Entering", PACKRGBA(255, 255, 255, text_alpha));
        ST_DrawString(-1, 161, MapInfo[nextmap].name, PACKRGBA(255, 255, 255, text_alpha));

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
