#include "doomdef.h"
#include "p_local.h"
#include "st_main.h"

extern void G_PlayerFinishLevel (int player);

boolean		gamepaused = true; // 800A6270

/*
===============================================================================

								THINKERS

All thinkers should be allocated by Z_Malloc so they can be operated on uniformly.  The actual
structures will vary in size, but the first element must be thinker_t.

Mobjs are similar to thinkers, but kept seperate for more optimal list
processing
===============================================================================
*/

thinker_t	thinkercap;	/* both the head and tail of the thinker list */    //80096378
mobj_t		mobjhead;	/* head and tail of mobj list */                    //800A8C74,
//int		    activethinkers;	/* debug count */
//int		    activemobjs;	/* debug count */

/*
===============
=
= P_InitThinkers
=
===============
*/
#if 0
void P_InitThinkers (void)
{
	thinkercap.prev = thinkercap.next  = &thinkercap;
	mobjhead.next = mobjhead.prev = &mobjhead;
}
#endif // 0

/*
===============
=
= P_AddThinker
=
= Adds a new thinker at the end of the list
=
===============
*/

void P_AddThinker (thinker_t *thinker) // 80021770
{
	thinkercap.prev->next = thinker;
	thinker->next = &thinkercap;
	thinker->prev = thinkercap.prev;
	thinkercap.prev = thinker;
}

/*
===============
=
= P_RemoveThinker
=
= Deallocation is lazy -- it will not actually be freed until its
= thinking turn comes up
=
===============
*/

void P_RemoveThinker (thinker_t *thinker) // 8002179C
{
	thinker->function = (think_t)-1;

    if (thinker == macrothinker) { // [D64] New lines
        macrothinker = NULL;
    }
}

/*
===============
=
= P_RunThinkers
=
===============
*/

void P_RunThinkers (void) // 800217C8
{
    thinker_t   *currentthinker, *next;

    currentthinker = thinkercap.next;
    if (thinkercap.next != &thinkercap)
    {
        while (currentthinker != &thinkercap)
        {
            if (currentthinker->function == (think_t)-1)
            {   // time to remove it
                next = currentthinker->next;
                currentthinker->next->prev = currentthinker->prev;
                currentthinker->prev->next = currentthinker->next;
                Z_Free (currentthinker);
            }
            else
            {
                if (currentthinker->function)
                {
                    ((void (*)(void *)) currentthinker->function) (currentthinker);
                }
                // get next pointer after in case the thinker added another one
                next = currentthinker->next;
            }
            currentthinker = next;
        }
    }
}

/*
===================
=
= P_RunMobjLate
=
= Run stuff that doesn't happen every tick
===================
*/
#if 0
void P_RunMobjLate (void)
{
	mobj_t	*mo;
	mobj_t	*next;

	for (mo=mobjhead.next ; mo != &mobjhead ; mo=next)
	{
		next = mo->next;	/* in case mo is removed this time */
		if (mo->latecall)
		{
			mo->latecall(mo);
		}
	}
}
#endif // 0

/*
==============
=
= P_CheckCheats
=
==============
*/

void P_CheckCheats (void) // 8002187C
{
	unsigned int buttons;
	int exit;

	buttons = ticbuttons[0] & 0xffff0000;

	if (!gamepaused)
    {
        if ((buttons & PAD_START) && !(oldticbuttons[0] & PAD_START))
        {
            gamepaused = true;

            S_PauseSound();

            lastticon = ticon;

            MenuCall = M_MenuTitleDrawer;
            cursorpos = 0;

            if (EnableMemPak)
            {
                MenuItem = Menu_Game;
                itemlines = 4;
            }
            else
            {
                MenuItem = Menu_GameNoSave;
                itemlines = 3;
            }

            if (FeaturesUnlocked)
                itemlines++;  // Enable cheat menu

            MenuIdx = 0;
            text_alpha = 255;
            MenuAnimationTic = 0;
        }

        return;
    }

    exit = M_MenuTicker();

    if (exit)
        M_MenuClearCall(ga_nothing);

    if (exit == ga_warped) {
        gameaction = ga_warped;
    }
    else if (exit == ga_exitdemo) {
        gameaction = ga_exitdemo;
    }
    else if (exit == ga_restart) {
        gameaction = ga_restart;
    }
    else if (exit == ga_exit)
    {
        gamepaused = false;
        S_ResumeSound();
        ticon = lastticon;
        ticsinframe = lastticon >> 2;
    }
}

void G_DoReborn (int playernum);//extern

/*
=================
=
= P_Ticker
=
=================
*/

//extern functions
void P_CheckSights (void);
void P_RunMobjBase (void);

int P_Ticker (void)//80021A00
{
	player_t *pl;

	gameaction = ga_nothing;

	//
	// check for pause and cheats
	//
	P_CheckCheats();

	if ((!gamepaused) && (gamevbls < gametic))
	{
	    P_RunThinkers();
		P_CheckSights();
		P_RunMobjBase();

		P_UpdateSpecials();
		P_RunMacros();

		ST_Ticker(); // update status bar
	}

	//ST_DebugPrint("%d",Z_FreeMemory (mainzone));

	//
	// run player actions
	//
	pl = players;

	if (pl->playerstate == PST_RELOAD) {
        gameaction = ga_died;
	}

    AM_Control(pl);
    P_PlayerThink(pl);

	return gameaction; // may have been set to ga_died, ga_completed, or ga_secretexit
}

/*
=============
=
= P_Drawer
=
= draw current display
=============
*/

extern Mtx R_ProjectionMatrix;  // 800A68B8
extern Mtx R_ModelMatrix;       // 8005b0C8

void P_Drawer (void) // 80021AC8
{
	I_ClearFrame();

	gMoveWd(GFX1++, G_MW_CLIP, G_MWO_CLIP_RNX, 1);
	gMoveWd(GFX1++, G_MW_CLIP, G_MWO_CLIP_RNY, 1);
	gMoveWd(GFX1++, G_MW_CLIP, G_MWO_CLIP_RPX, 65535);
	gMoveWd(GFX1++, G_MW_CLIP, G_MWO_CLIP_RPY, 65535);
	gMoveWd(GFX1++, G_MW_PERSPNORM, G_MWO_MATRIX_XX_XY_I, 68);

	// create a projection matrix
    gSPMatrix(GFX1++, OS_K0_TO_PHYSICAL(&R_ProjectionMatrix), G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);

    // create a model matrix
    gSPMatrix(GFX1++, OS_K0_TO_PHYSICAL(&R_ModelMatrix), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);

	if (players[0].automapflags & (AF_LINES|AF_SUBSEC))
    {
        AM_Drawer();
	}
	else
    {
		R_RenderPlayerView();
        //ST_DebugPrint("x %d || y %d", players[0].mo->x >> FRACBITS, players[0].mo->y >> FRACBITS);

        if (demoplayback == false)
            ST_Drawer();
    }

    if (MenuCall)
    {
        M_DrawOverlay(0, 0, 320, 240, 96);
        MenuCall();
    }

    I_DrawFrame();
}

extern void T_FadeInBrightness(fadebright_t *fb);
extern int start_time;  // 80063390
extern int end_time;    // 80063394

void P_Start (void) // 80021C50
{
    fadebright_t *fb;

    DrawerStatus = 1;

	gamepaused = false;
	validcount = 1;

	AM_Start();
	M_ClearRandom();

	/* do a nice little fade in effect */
	fb = Z_Malloc(sizeof(*fb), PU_LEVSPEC, 0);
	P_AddThinker(&fb->thinker);
	fb->thinker.function = T_FadeInBrightness;
	fb->factor = 0;
	
    if (gamemap == 33)  /* Add by default God Mode in player for title  */
    {
        players[0].cheats |= CF_GODMODE;
    }
    else if (gamemap == 32)  /* Remove by default God Mode in player for hectic */
    {
        players[0].cheats &= ~CF_GODMODE;
    }

	/* autoactivate line specials */
	P_ActivateLineByTag(999, players[0].mo);

	start_time = ticon;

    MusicID = MapInfo[gamemap].MusicSeq-(NUMSFX-1);
    S_StartMusic(MapInfo[gamemap].MusicSeq);
}

void P_Stop (int exit) // 80021D58
{
    /* [d64] stop plasma buzz */
	S_StopSound(0, sfx_electric);

    end_time = ticon;
	gamepaused = false;
	DrawerStatus = 0;

	G_PlayerFinishLevel(0);

	/* free all tags except the PU_STATIC tag */
	Z_FreeTags(mainzone, ~PU_STATIC); // (PU_LEVEL | PU_LEVSPEC | PU_CACHE)

    if ((gamemap != 33) || (exit == 8))
        S_StopMusic();

    if ((demoplayback) && (exit == 8))
        I_WIPE_FadeOutScreen();
    else
        I_WIPE_MeltScreen();
    
    if (gamemap == 33 && demoplayback && exit == ga_completed)
    {
        // wait until the music has finished playing.
        // before, it relied on exact map timing (see two conditions above).
        while (S_SoundStatus(MapInfo[gamemap].MusicSeq) == 1)
        {
            //ST_DebugPrint("Waiting %d", exit);
        }
        //ST_DebugPrint("Title Completed");
    }
    S_StopAll();
    //ST_DebugPrint("Stopped %d", exit);
}

