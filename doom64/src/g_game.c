/* G_game.c  */

#include "doomdef.h"
#include "p_local.h"

void G_PlayerReborn (int player);
void G_PlayerReload (int player);

void G_DoReborn (int playernum);

void G_DoLoadLevel (void);


gameaction_t    gameaction;                 // 80063230
skill_t         gameskill;                  // 80063234
unsigned char	gamemap;                    // 80063238
unsigned char	nextmap;				    // 8006323C /* the map to go to after the stats */

//boolean         playeringame[MAXPLAYERS]; //
player_t        players[MAXPLAYERS];        // 80063240
thingdata_t		playdata[MAXPLAYERS]; // [Immorpher] store player data for reload

int             consoleplayer;          /* player taking events and displaying  */
int             displayplayer;          /* view being displayed  */
int             totalkills, totalitems, totalsecret;    /* for intermission  */

//char            demoname[32];
boolean         demoplayback;           // 800633A8
int		        *demo_p = NULL, *demobuffer = NULL;   // 8005A180, 8005a184

//mapthing_t	deathmatchstarts[10], *deathmatch_p;    // 80097e4c, 80077E8C
mapthing_t	playerstarts[MAXPLAYERS];   // 800a8c60

/*
==============
=
= G_DoLoadLevel
=
==============
*/

void G_DoLoadLevel (void) // 80004530
{
	if (((gameaction == ga_restart) || (gameaction == ga_warped)) || (players[0].playerstate == PST_DEAD))
	{
		players[0].playerstate = PST_REBORN;
	}

	P_SetupLevel(gamemap, gameskill);
	gameaction = ga_nothing;
}


/*
==============================================================================

						PLAYER STRUCTURE FUNCTIONS

also see P_SpawnPlayer in P_Mobj
==============================================================================
*/

/*
====================
=
= G_PlayerFinishLevel
=
= Can when a player completes a level
====================
*/

void G_PlayerFinishLevel (int player) // 80004598
{
	player_t *p;
	unsigned char i;

	p = &players[player];

	bzero(p->powers, sizeof (p->powers));
    bzero(p->cards, sizeof (p->cards));
	p->mo->flags &= ~MF_SHADOW; /* cancel invisibility  */
	p->damagecount = 0;                     /* no palette changes  */
	p->bonuscount = 0;
    p->bfgcount = 0;
    p->automapflags = 0;
	for (i = 0; i < NUMMESSAGES; i++) p->messagetic[i] = 0;
}

/*
====================
=
= G_PlayerReborn
=
= Called after a player dies
= almost everything is cleared and initialized
====================
*/

int gobalcheats = 0; // [GEC]

void G_PlayerReborn (int player) // 80004630
{
	player_t *p;

	p = &players[player];
	bzero(p, sizeof(*p));

	p->usedown = p->attackdown = true; // don't do anything immediately
	p->playerstate = PST_LIVE;
	p->health = MAXHEALTH;
	p->readyweapon = p->pendingweapon = wp_pistol;
	p->weaponowned[wp_fist] = true;
	p->weaponowned[wp_pistol] = true;
	p->ammo[am_clip] = 50;
    p->maxammo[am_clip] = maxammo[am_clip];
    p->maxammo[am_shell] = maxammo[am_shell];
    p->maxammo[am_cell] = maxammo[am_cell];
    p->maxammo[am_misl] = maxammo[am_misl];

    p->cheats |= gobalcheats; // [GEC] Apply global cheat codes
}

void G_PlayerReload (int player) // [Immorpher] Reload player's starting state
{
	player_t *p;
	int	i;

	p = &players[player];
	bzero(p, sizeof(*p));

	p->usedown = p->attackdown = true; // don't do anything immediately
	p->playerstate = PST_LIVE;
	p->health = MAXHEALTH;
	p->readyweapon = p->pendingweapon = wp_pistol;
	p->health = playdata[player].health;				
	p->armorpoints = playdata[player].armorpoints;
	p->armortype = playdata[player].armortype;	
	p->artifacts = playdata[player].artifacts;
	p->backpack = playdata[player].backpack;
	for (i = 0; i < NUMWEAPONS; i++) // Store weapon possession
	{
		p->weaponowned[i] = playdata[player].weaponowned[i];
	}
	for (i = 0; i < NUMAMMO; i++) // Store ammo amount
	{
		p->ammo[i] = playdata[player].ammo[i];
		p->maxammo[i] = playdata[player].maxammo[i];
	}

    p->cheats |= gobalcheats; // [GEC] Apply global cheat codes
}

/*============================================================================  */

/*
====================
=
= G_CompleteLevel
=
====================
*/

void G_CompleteLevel (void) // 800046E4
{
	gameaction = ga_completed;
}

/*
====================
=
= G_InitNew
=
====================
*/

extern int ActualConfiguration[13];
mobj_t emptymobj; // 80063158

void G_InitNew (skill_t skill, int map, gametype_t gametype) // 800046F4
{
	//printf ("G_InitNew, skill %d, map %d\n", skill, map);

	/* free all tags except the PU_STATIC tag */
	Z_FreeTags(mainzone, ~PU_STATIC); // (PU_LEVEL | PU_LEVSPEC | PU_CACHE)

	M_ClearRandom ();

/* force players to be initialized upon first level load          */
    players[0].playerstate = PST_REBORN;

/* these may be reset by I_NetSetup */
	gameskill = skill;
	gamemap = map;

	bzero(&emptymobj, sizeof(emptymobj));
	players[0].mo = &emptymobj;	/* for net consistancy checks */

	demoplayback = false;

	BT_DATA[0] = (buttons_t *)ActualConfiguration;
}

/*============================================================================  */

/*
=================
=
= G_RunGame
=
= The game should allready have been initialized or laoded
=================
*/

void G_RunGame (void) // 80004794
{

	while (1)
	{
        /* load a level */
        G_DoLoadLevel ();

		if(runintroduction) { // [Immorpher] run introduction text screen
			MiniLoop(F_StartIntermission, F_StopIntermission, F_TickerIntermission, F_DrawerIntermission);
			runintroduction = false; // [Immorpher] only run it once!
		}

		MiniLoop (P_Start, P_Stop, P_Ticker, P_Drawer);

        if(gameaction == ga_warped)
			continue; /* skip intermission */

        if ((gameaction == ga_died) || (gameaction == ga_restart))
			continue;			/* died, so restart the level */

        if (gameaction == ga_exitdemo)
            return;

        /* run a stats intermission */
		
		if (gamemap != 0) { // [Immorpher] dont run stats at bonus pak hub
			MiniLoop(IN_Start, IN_Stop, IN_Ticker, IN_Drawer);
		}

        if(nextmap ==  9 ||
           nextmap == 29 ||
           nextmap == 30 ||
           nextmap == 31 ||
           nextmap == 32 ||
           nextmap == 49 || // Beta 64 maps start here
           nextmap == 71 ||
           nextmap == 72 ||
           nextmap == 57 ||
           nextmap == 70)
        {
            /* run the intermission if needed */
            MiniLoop(F_StartIntermission, F_StopIntermission, F_TickerIntermission, F_DrawerIntermission);

            if(gameaction == ga_warped)
                continue; /* skip intermission */

            if(gameaction == ga_restart)
                continue;

            if (gameaction == ga_exitdemo)
                return;
        }
		else if(FUNLEVEL(gamemap))
		{
			gameaction = ga_exitdemo;
			return;
		}
        else
        {
            if ((nextmap != gamemap) && FINLEVEL(gamemap)) // check finale maps to run finale
            {
                /* run the finale if needed */
                MiniLoop(F_Start, F_Stop, F_Ticker, F_Drawer);

                if(gameaction == ga_warped)
                    continue; /* skip intermission */

                if(gameaction == ga_restart)
                    continue;
                else
                    return;
            }
        }

        /* Set Next Level */
        gamemap = nextmap;
	}
}

int G_PlayDemoPtr (int skill, int map) // 800049D0
{
	int		exit;
	int		config[13];
	unsigned char	sensitivity;
	unsigned char	deadzone;
	boolean			mercilessdemo;

	demobuffer = demo_p;

	/* copy key configuration */
	D_memcpy(config, ActualConfiguration, sizeof(config));

	/* set new key configuration */
	D_memcpy(ActualConfiguration, demobuffer, sizeof(config));

	/* store player settings */
	sensitivity = M_SENSITIVITY;
	deadzone = PlayDeadzone;
	mercilessdemo = MercilessMode;

	/* settings for demo compatibility */
	M_SENSITIVITY = demobuffer[13] + 27;
	PlayDeadzone = 10;
	MercilessMode = 0;
	

	/* skip analog and key configuration */
	demobuffer += 14;

	/* play demo game */
	G_InitNew (skill, map, gt_single);
	demoplayback = true;
	G_DoLoadLevel ();
	exit = MiniLoop (P_Start, P_Stop, P_Ticker, P_Drawer);
	demoplayback = false;

	/* restore key configuration */
	D_memcpy(ActualConfiguration, config, sizeof(config));

	/* restore player settings */
	M_SENSITIVITY = sensitivity;
	PlayDeadzone = deadzone;
	MercilessMode = mercilessdemo;

	/* free all tags except the PU_STATIC tag */
	Z_FreeTags(mainzone, ~PU_STATIC); // (PU_LEVEL | PU_LEVSPEC | PU_CACHE)

	return exit;
}