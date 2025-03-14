/* p_misc.c */

#include "doomdef.h"
#include "p_local.h"
#include "r_local.h"

extern void P_AddSectorSpecial(sector_t* sector);

/*
==============================================================================

							   AIM CAMERA

==============================================================================
*/

#define CAMAIMANGLE ANG5

void T_AimCamera(aimcamera_t *camera) // 8000DE60
{
    angle_t delta;
    angle_t newangle;
    angle_t angle;

    if((cameratarget != camera->viewmobj) || (cameratarget == players[0].mo))
    {
        P_RemoveThinker(&camera->thinker);
        return;
    }

    /* adjust angle */
    newangle = R_PointToAngle2(cameratarget->x, cameratarget->y, players[0].mo->x, players[0].mo->y);
    angle  = cameratarget->angle;
    delta = (newangle - angle);

    if ((delta < CAMAIMANGLE) || (delta > -CAMAIMANGLE)) {
        cameratarget->angle = newangle;
    }
    else if (delta < ANG180) {
        cameratarget->angle = angle + CAMAIMANGLE;
    }
    else {
        cameratarget->angle = angle - CAMAIMANGLE;
    }
}

int P_SetAimCamera(line_t *line, boolean aim) // 8000DF20
{
    aimcamera_t *camera;
    mobj_t *mo;

	for (mo = mobjhead.next; mo != &mobjhead; mo = mo->next)
	{
		if((line->tag != mo->tid))
			continue;   /* not matching the tid */

		if(line->tag == cameratarget->tid)
			continue; /* skip if cameratarget matches tag */

		cameratarget = mo;
		if ((cameratarget != players[0].mo) && (aim))
		{
			mo->angle = R_PointToAngle2(mo->x, mo->y, players[0].mo->x, players[0].mo->y);

			camera = Z_Malloc(sizeof(*camera), PU_LEVSPEC, NULL);
			P_AddThinker(&camera->thinker);
			camera->thinker.function = T_AimCamera;
			camera->viewmobj = mo;
		}

		return 1;
	}

    return 0;
}

/*
==============================================================================

                          EV_SpawnTrapMissile

==============================================================================
*/

int EV_SpawnTrapMissile(line_t *line, mobj_t *target, mobjtype_t type) // 8000E02C
{
    mobj_t* mo;
    mobj_t* th;
    int ok;

    angle_t angle;
    fixed_t x, y;

    ok = 0;
    for(mo = mobjhead.next; mo != &mobjhead; mo = mo->next)
    {
        if(mo->type != MT_DEST_PROJECTILE)
            continue;   /* not a dart projector */

        if((line->tag != mo->tid))
            continue;   /* not matching the tid */

        ok = 1;

        angle = mo->angle;
        angle >>= ANGLETOFINESHIFT;
        x = finecosine[angle];
        y = finesine[angle];

        if(type == MT_PROJ_TRACER)
        {
            th = P_SpawnMissile(mo, target,
                FixedMul(mo->radius, x),
                FixedMul(mo->radius, y),
                (32*FRACUNIT), MT_PROJ_TRACER);
            th->x = (th->x + th->momx);
            th->y = (th->y + th->momy);
            th->tracer = target;
        }
        else if(type == MT_PROJ_DART)
        {
            th = P_SpawnMissile(mo, NULL, 
                FixedMul(mo->radius, x), 
                FixedMul(mo->radius, y), 
                0, MT_PROJ_DART);
        }
    }

    return ok;
}

/*
==============================================================================

							   EXIT DELAY

==============================================================================
*/

/*
================================================================================
=
= P_SpawnDelayTimer
=
= Exclusive Psx Doom / Doom 64
=
===============================================================================
*/

void P_SpawnDelayTimer(int tics, void(*action)()) // 8000E160
{
	delay_t *timer;

	timer = Z_Malloc(sizeof(*timer), PU_LEVSPEC, NULL);
	P_AddThinker(&timer->thinker);
	timer->thinker.function = T_CountdownTimer;
	timer->tics = tics;
	timer->finishfunc = action;
}

/*
================================================================================
=
= T_CountdownTimer
=
= Exclusive Psx Doom / Doom 64
=
===============================================================================
*/

void T_CountdownTimer(delay_t *timer) // 8000E1CC
{
    if((--timer->tics) <= 0)
	{
	    if (timer->finishfunc)
            timer->finishfunc();

		P_RemoveThinker(&timer->thinker);
	}
}

/*
================================================================================
=
= P_ExitLevel
=
= Exclusive Psx Doom / Doom 64
=
===============================================================================
*/

void P_ExitLevel(void) // 8000E220
{
	nextmap = gamemap + 1;
	P_SpawnDelayTimer(15, G_CompleteLevel);
}

/*
================================================================================
=
= P_SecretExitLevel
=
= Exclusive Psx Doom / Doom 64
=
===============================================================================
*/

void P_SecretExitLevel(int map) // 8000E25C
{
    int delaytics;

    if (gamemap == 0) // [Immorpher] Exit quickly to bonus levels
		delaytics = 5;
    else if FINLEVEL(gamemap) // Longer delay after final victory
        delaytics = 120;
	else // Normal maps have this exit delay
        delaytics = 15;

    nextmap = map;
    P_SpawnDelayTimer(delaytics, G_CompleteLevel);
}

/*
====================================================================

                        TELEPORTATION
 The teleportation functions for some reason, were in the middle of
 the other functions in this file, I move them in the corresponding
 file p_telept.c

====================================================================
*/

//void P_Telefrag (mobj_t *thing, fixed_t x, fixed_t y) // 8000E29C
//int	EV_Teleport( line_t *line, mobj_t *thing ) // 8000E3A0
//int	EV_SilentTeleport( line_t *line, mobj_t *thing ) // 8000E5C0

/* --------------------------------------------------------------------------------- */

int P_ModifyLine(line_t *line, int tag, int type) // [Immorpher] Combined modify line action
{
    line_t *line1, *line2;
    int i;

    line2 = lines;
    for (i = 0; i < numlines; i++, line2++)
    {
        if (line2->tag == tag) break;
    }

    if (i < numlines)
    {
        line1 = lines;
        for (i = 0; i < numlines; i++, line1++)
        {
            if (line->tag == line1->tag)
            {
				switch(type)
				{
				case mods_flags:
					if (line1->flags & ML_TWOSIDED)
					{
						line1->flags = (line2->flags | ML_TWOSIDED);
					}
					else
					{
						line1->flags = line2->flags;
						line1->flags &= ~ML_TWOSIDED;
					}
					break;
				case mods_special:
					line1->special = line2->special;
					break;
				case mods_texture:
					sides[line1->sidenum[0]].toptexture = sides[line2->sidenum[0]].toptexture;
					sides[line1->sidenum[0]].bottomtexture = sides[line2->sidenum[0]].bottomtexture;
					sides[line1->sidenum[0]].midtexture = sides[line2->sidenum[0]].midtexture;
					if (line1->flags & ML_TWOSIDED && line2->flags & ML_TWOSIDED) {
						sides[line1->sidenum[1]].toptexture = sides[line2->sidenum[1]].toptexture;
						sides[line1->sidenum[1]].bottomtexture = sides[line2->sidenum[1]].bottomtexture;
						sides[line1->sidenum[1]].midtexture = sides[line2->sidenum[1]].midtexture;
					}
					break;
				default:
					break;
				}
            }
        }

        return 1;
    }

    return 0;
}

int P_ModifySector(line_t *line, int tag, int type) // 8000E928
{
    sector_t *sec1, *sec2;
    int secnum;
    int rtn;

    secnum = P_FindSectorFromLineTag(tag, 0);

    if(secnum == -1)
        return 0;

    sec2 = &sectors[secnum];

    secnum = -1;
    rtn = 0;

    while((secnum = P_FindSectorFromLineTag(line->tag, secnum)) >= 0)
    {
        sec1 = &sectors[secnum];
        rtn = 1;

        switch(type)
        {
        case mods_flags:
            sec1->flags = sec2->flags;
            break;
        case mods_special:
            if (sec1->special != sec2->special)
            {
                sec1->special = sec2->special;
                P_AddSectorSpecial(sec1);
            }
            break;
        case mods_texture:
            sec1->ceilingpic = sec2->ceilingpic;
            sec1->floorpic = sec2->floorpic;
            break;
        case mods_lights:
            sec1->colors[0] = sec2->colors[0];
            sec1->colors[1] = sec2->colors[1];
            sec1->colors[2] = sec2->colors[2];
            sec1->colors[3] = sec2->colors[3];
            sec1->colors[4] = sec2->colors[4];
            break;
        default:
            break;
        }
    }

    return rtn;
}

void T_FadeThinker(fade_t *fade) // 8000EACC
{
    mobj_t *mo;

    mo = fade->mobj;
    mo->alpha += fade->amount;

    if (fade->amount > 0)
    {
        if (mo->alpha < fade->destAlpha)
            return;

        mo->alpha = fade->destAlpha;
        mo->flags |= fade->flagReserve;
        P_RemoveThinker(&fade->thinker);
    }
    else if ((fade->destAlpha < mo->alpha) == 0)
    {
        mo->alpha = fade->destAlpha;
        P_RemoveThinker(&fade->thinker);

        if (fade->destAlpha == 0)
            P_RemoveMobj(mo);
    }

    /*if (fade->amount <= 0)
    {
        if (mo->alpha <= fade->destAlpha)
        {
            mo->alpha = fade->destAlpha;
            P_RemoveThinker(&fade->thinker);

            if (fade->destAlpha == 0)
                P_RemoveMobj(mo);
        }
    }
    else if (mo->alpha >= fade->destAlpha)
    {
        mo->alpha = fade->destAlpha;
        mo->flags |= fade->flagReserve;
        P_RemoveThinker(&fade->thinker);
    }*/
}

extern mobj_t *P_SpawnMapThing (mapthing_t *mthing);
extern mapthing_t *spawnlist;   // 800A5D74
extern int spawncount;          // 800A5D78

int EV_SpawnMobjTemplate(int tag) // 8000EB8C
{
    mobj_t *mobj;
    fade_t *fade;
    mapthing_t *mthing;
    int i;
    int rtn;

    rtn = 0;
    mthing = spawnlist;
    for (i = 0; i < spawncount; i++, mthing++)
    {
        if(mthing->tid != tag)
            continue; /* find matching tid */

        if(!(mobj = P_SpawnMapThing(mthing)))
            continue; /* setup spawning */

        rtn = 1;
        if (mobj->type == MT_DEMON2)
        {
            mobj->alpha = 48;
            mobj->flags |= MF_SHADOW;
        }
        else
        {
            fade = Z_Malloc (sizeof(*fade), PU_LEVSPEC, NULL);
            P_AddThinker (&fade->thinker);
            fade->thinker.function = T_FadeThinker;
            fade->mobj = mobj;
            fade->amount = 8;
            fade->destAlpha = mobj->alpha;
            fade->flagReserve = mobj->flags & (MF_SPECIAL|MF_SOLID|MF_SHOOTABLE);

            if (mobj->flags & MF_SHOOTABLE)
            {
                mobj->flags &= ~MF_SHOOTABLE;
            }
            else
            {
                mobj->flags &= ~(MF_SPECIAL|MF_SOLID|MF_SHOOTABLE);
            }
            mobj->alpha = 0;
        }

        S_StartSound(mobj, sfx_spawn);
    }

    return rtn;
}

int EV_FadeOutMobj(int tag) // 8000ED08
{
    fade_t *fade;
    mobj_t *mo;
    int rtn;

    rtn = 0;

    for (mo=mobjhead.next ; mo != &mobjhead ; mo=mo->next)
    {
        if(tag != mo->tid)
            continue;   /* not matching the tid */

        rtn = 1;
        mo->flags &= ~(MF_SPECIAL|MF_SOLID|MF_SHOOTABLE);

        fade = Z_Malloc (sizeof(*fade), PU_LEVSPEC, 0);
        P_AddThinker (&fade->thinker);
        fade->thinker.function = T_FadeThinker;
        fade->mobj = mo;
        fade->amount = -8;
        fade->destAlpha = 0;
    }

    return rtn;
}

void T_Quake(quake_t *quake) // 8000EDE8
{
    if((--quake->tics) == 0)
    {
        S_StopSound(NULL, sfx_quake);
        quakeviewy = 0;
        quakeviewx = 0;
        P_RemoveThinker(&quake->thinker);
        return;
    }

    quakeviewy = (((P_Random() & 1) << 18) - (2*FRACUNIT));
    quakeviewx = (((P_Random() & 1) << 24) - (128*FRACUNIT));
}

void P_SpawnQuake(int tics) // 8000EE7C
{
    quake_t *quake;

    quake = Z_Malloc (sizeof(*quake), PU_LEVSPEC, 0);
    P_AddThinker (&quake->thinker);
    quake->thinker.function = T_Quake;
    quake->tics = tics;

    S_StartSound(NULL, sfx_quake);
}

int P_RandomLineTrigger(line_t *line,mobj_t *thing) // 8000EEE0
{
    line_t *li;
    int line_idx[16];
    int i, count;

    if (activemacro)
        return 0;

    count = 0;

    li = lines;
    for(i = 0; ((i < numlines)&&(count < 16)); i++, li++)
    {
        /* special 240 -> Execute random line trigger */
        if(((line->tag == li->tag) && (li != line)) && (SPECIALMASK(li->special) != 240))
        {
            line_idx[count] = i;
            count++;
        }
    }

    if(!count)
        return 0;

    return P_UseSpecialLine(&lines[line_idx[P_Random()%count]], thing);
}

#define CAMMOVESPEED    164
#define CAMTRACEANGLE   ANG1

void T_MoveCamera(movecamera_t *camera) // 8000F014
{
    angle_t delta;
    angle_t newangle;
    angle_t angle;
    int     dist;
    mobj_t  *mo;

    /* adjust angle */
    newangle = R_PointToAngle2(cameratarget->x, cameratarget->y, camera->x, camera->y);
    angle = cameratarget->angle;
    delta = newangle - angle;
    if ((delta < CAMTRACEANGLE) || (delta > -CAMTRACEANGLE)) {
        cameratarget->angle = newangle;
    }
    else if (delta < ANG180) {
        cameratarget->angle = angle + CAMTRACEANGLE;
    }
    else {
        cameratarget->angle = angle - CAMTRACEANGLE;
    }

    /* adjust pitch */
    dist = P_AproxDistance(cameratarget->x - camera->x, cameratarget->y - camera->y);
    if(dist >= (48*FRACUNIT))
    {
        newangle = R_PointToAngle2(0, cameratarget->z, dist, camera->z);
        angle = camviewpitch;
        delta = newangle - angle;
        if ((delta < CAMTRACEANGLE) || (delta > -CAMTRACEANGLE)) {
            camviewpitch = newangle;
        }
        else if (delta < ANG180) {
            camviewpitch = angle + CAMTRACEANGLE;
        }
        else {
            camviewpitch = angle - CAMTRACEANGLE;
        }
    }

    /* adjust camera position */

    camera->tic++;
    if (camera->tic < CAMMOVESPEED)
    {
        cameratarget->x = cameratarget->x + camera->slopex;
        cameratarget->y = cameratarget->y + camera->slopey;
        cameratarget->z = cameratarget->z + camera->slopez;
        return;
    }

    /* jump to next camera spot */
    for(mo = mobjhead.next; mo != &mobjhead; mo = mo->next)
    {
        if(camera->current != mo->tid)
            continue; /* must match tid */

        if(mo->type != MT_CAMERA)
            continue; /* not a camera */

        camera->slopex = (mo->x - cameratarget->x) / CAMMOVESPEED;
        camera->slopey = (mo->y - cameratarget->y) / CAMMOVESPEED;
        camera->slopez = (mo->z - cameratarget->z) / CAMMOVESPEED;

        camera->tic = 0;
        camera->current++;
        return;
    }
	
	// [kex] remove thinker after its done
	P_RemoveThinker(&camera->thinker);
}

void P_SetMovingCamera(line_t *line) // 8000F2F8
{
    movecamera_t *camera;
    mobj_t *mo;

    camera = Z_Malloc (sizeof(*camera), PU_LEVSPEC, 0);
    P_AddThinker (&camera->thinker);
    camera->thinker.function = T_MoveCamera;

    for(mo = mobjhead.next; mo != &mobjhead; mo = mo->next)
    {
        if(mo->tid != line->tag)
            continue; /* not matching the tid */

        if(mo->type != MT_CAMERA)
            continue; /* not a camera */

        /* setup moving camera */
        camera->x = mo->x;
        camera->y = mo->y;
        camera->z = mo->z;
		camera->slopex = 0;
		camera->slopey = 0;
		camera->slopez = 0;
        camera->tic = CAMMOVESPEED;
        camera->current = mo->tid + 1;

        /* set camera target */
        mo->angle = cameratarget->angle;
        mo->x = cameratarget->x;
        mo->y = cameratarget->y;
        cameratarget = mo;
        return;
    }
}

void P_RefreshBrightness(void) // 8000f410
{
    int factor = 100;
	unsigned char i;
	float curve, scale;

	scale = (float)brightness/127.0f;
	scale /= 256-(float)lightmax[2*brightness]; // 256 to prevent divide by 0
	
	for (i = 1; i < 255; i++) { // [Immorpher] New brightness adjustment by "tracing out the circle"
		
		curve = (float)i*scale;
		if (curve > 1)
			curve = 1;
		
		curve = (float)i*(1-curve) + curve*(float)lightmax[i];
		lightcurve[i] = (char)curve;
	}
	
    if (factor < infraredFactor) {
        factor = infraredFactor;
    }
	
    if (DrawerStatus == 1) // Doom 64 1.1 bug fix found by Nova
        P_SetLightFactor(factor);
	
}

extern maplights_t *maplights;     // 800A5EA4

void P_SetLightFactor(int lightfactor) // 8000F458
{
    register u32 fpstat, fpstatset;

    int l_flt;
    light_t *light;
    maplights_t *maplight;
    int base_r, base_g, base_b;
    int r, g, b;
    int h, s, v;
    int factor;
    int i;

    maplight = maplights;
    light = lights;
    for(i = 0; i < numlights; i++)
    {
        if (i > 255)
        {
            LightGetHSV(maplight->r, maplight->g, maplight->b, &h, &s, &v);
            maplight++;
            factor = v;
        }
        else
        {
            factor = i;
        }

        // fetch the current floating-point control/status register
        fpstat = __osGetFpcCsr();
        // enable round to negative infinity for floating point
        fpstatset = (fpstat | FPCSR_RM_RM) ^ 2;
        // _Disable_ unimplemented operation exception for floating point.
        __osSetFpcCsr(fpstatset);

        l_flt = (float)factor * ((float)lightfactor / 100.0);

        v = (int)l_flt;
        
		// ensure proper bounds
        if (v > 255)
            v = 255;
		else if  (v < 0)
			v = 0;
		
		v = lightcurve[v]; // [Immorpher] - Non-linearly boost brightness

        if (i > 255)
        {
            LightGetRGB(h, s, v, &r, &g, &b);
            base_r = r;
            base_g = g;
            base_b = b;
            /*base_r = maplight->r;
            base_g = maplight->g;
            base_b = maplight->b;
            maplight++;*/
        }
        else
        {
            base_r = v;
            base_g = v;
            base_b = v;

           /* base_r = i;
            base_g = i;
            base_b = i;*/
        }

        // [GEC] New Cheat Codes
        if (players[0].cheats & CF_FULLBRIGHT)
        {
            base_r = 255;
            base_g = 255;
            base_b = 255;
        }
        else if (players[0].cheats & CF_NOCOLORS)
        {
            base_r = v & 255;
            base_g = v & 255;
            base_b = v & 255;
        }

        light->rgba = PACKRGBA(base_r, base_g, base_b, 255);
        light++;
    }
}

void T_FadeInBrightness(fadebright_t *fb) // 8000f610
{
    fb->factor += 2;
    if (fb->factor >= (100))
    {
        fb->factor = (100);
        P_RemoveThinker(&fb->thinker);
    }

    P_SetLightFactor(fb->factor);
}

int P_ModifyMobjFlags(int tid, int flags) // 8000F674
{
    mobj_t *mo;
    int ok;

    ok = 0;
    for(mo = mobjhead.next; mo != &mobjhead; mo = mo->next)
    {
        if(mo->tid != tid)
            continue; /* not matching the tid */

        ok = 1;
        mo->flags &= ~flags;
    }

    return ok;
}

int P_AlertTaggedMobj(int tid, mobj_t *activator) // 8000F6C4
{
    mobj_t  *mo;
    state_t *st;
    int     ok;

    ok = 0;
    for(mo = mobjhead.next; mo != &mobjhead; mo = mo->next)
    {
        if(mo->tid != tid)
            continue; /* not matching the tid */

        if(mo->info->seestate == S_000)
            continue;

        mo->threshold = 0;
        mo->target = activator;
        ok = 1;

        st = &states[mo->info->seestate];

        mo->state       = st;
        mo->tics        = st->tics;
        mo->sprite      = st->sprite;
        mo->frame       = st->frame;
    }

    return ok;
}

void T_MobjExplode(mobjexp_t *exp) // 8000F76C
{
    fixed_t x;
    fixed_t y;
    fixed_t z;
    mobj_t *mo;

    if((--exp->delay) <= 0)
    {
        exp->delay = exp->delaydefault;

        x = (((P_Random() - P_Random()) << 14) + exp->mobj->x);
        y = (((P_Random() - P_Random()) << 14) + exp->mobj->y);
        z = (((P_Random() - P_Random()) << 14) + exp->mobj->z);

        mo = P_SpawnMobj(x, y, z + (exp->mobj->info->height >> 1), MT_EXPLOSION2);

        if((exp->lifetime & 1))
            S_StartSound(mo, sfx_explode);

        if((--exp->lifetime) == 0)
        {
            P_RemoveThinker(&exp->thinker);
        }
    }
}
