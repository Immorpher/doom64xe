
//Renderer phase 2 - Sky Rendering Routines

#include "doomdef.h"
#include "r_local.h"

#define FIRESKY_WIDTH   64
#define FIRESKY_HEIGHT  64

typedef enum
{
    SKF_CLOUD       = 1,
    SKF_THUNDER     = 2,
    SKF_MOUNTAIN    = 4,
} skyflags_e;

Vtx SkyCloudVertex[4] = // 8005B1D0
{
	{
        { 
            {-300, 120, -160}, 0, {(0 << 6), (0 << 6)}, {0, 0, 0, 0xff} 
        }
    },
	{
        { 
            { 300, 120, -160}, 0, {(0 << 6), (0 << 6)}, {0, 0, 0, 0xff} 
        }
    },
	{
        { 
            { 300,   0, -300}, 0, {(0 << 6), (0 << 6)}, {0, 0, 0, 0xff} 
        }
    },
	{
        { 
            {-300,   0, -300}, 0, {(0 << 6), (0 << 6)}, {0, 0, 0, 0xff} 
        }
    },
};

Vtx SkyFireVertex[4] = // 8005B210
{
	{
        { 
            {-160, 120, -160}, 0, {(0 << 6), (0  << 6)}, {0, 0, 0, 0xff} 
        }
    },
	{
        { 
            { 160, 120, -160}, 0, {(0 << 6), (0  << 6)}, {0, 0, 0, 0xff} 
        }
    },
	{
        { 
            { 160,   0, -160}, 0, {(0 << 6), (64 << 6)}, {0, 0, 0, 0xff} 
        }
    },
	{
        { 
            {-160,   0, -160}, 0, {(0 << 6), (64 << 6)}, {0, 0, 0, 0xff} 
        }
    },
};

fixed_t     FogNear;            // 800A8120
int         FogColor;           // 800A8124
skyfunc_t   R_RenderSKY;        // 800A8130
byte        *SkyFireData[2];    // 800A8140 // Fire data double buffer
byte        *SkyCloudData;      // 800A8148
int         Skyfadeback;        // 800A814C
int         FireSide;           // 800A8150
int         SkyCloudOffsetX;    // 800A8154
int         SkyCloudOffsetY;    // 800A8158
int         ThunderCounter;     // 800A815C
int         LightningCounter;   // 800A8160
int         SkyPicSpace;        // 800A8164
int         SkyPicMount;        // 800A8168
int         SkyCloudColor;      // 800A816C
int         SkyVoidColor;       // 800A8170
int         SkyFlags;           // 800A8174
short		LightningFlash; // new lighting flash to brighten sectors

void R_RenderSpaceSky(void) HOT;
void R_RenderCloudSky(void) HOT;
void R_RenderVoidSky(void) HOT;
void R_RenderEvilSky(void) HOT;
void R_RenderClouds(void) HOT;
void R_RenderSkyPic(int lump, int yoffset) HOT;
void R_RenderFireSky(void) HOT;
void R_CloudThunder(void) HOT;

void R_SetupSky(void) // 80025060
{
    byte *data;

    FogNear = 985;
    FogColor = PACKRGBA(0,0,0,0);
    R_RenderSKY = NULL;
    SkyFlags = 0;
    SkyCloudOffsetX = 0;
    SkyCloudOffsetY = 0;
    ThunderCounter = 180;
    LightningCounter = 0;
    FireSide = 0;
    Skyfadeback = 0;
    SkyPicSpace = W_GetNumForName("SPACE");

    switch(skytexture)
    {
        case 1:
        case 10:
            SkyFlags = (SKF_CLOUD|SKF_THUNDER);
            R_RenderSKY = R_RenderCloudSky;

            SkyCloudData = (byte *)W_CacheLumpName("CLOUD", PU_STATIC);
            SkyCloudColor = PACKRGBA(176,128,255,255); // 0xb080ffff

            if (skytexture == 10)
            {
                SkyPicMount = W_GetNumForName("MOUNTC");
                SkyFlags |= SKF_MOUNTAIN;
            }

            *(int*)SkyCloudVertex[0].v.cn = PACKRGBA(0,0,0,255); // PACKRGBA(0,0,0,255)
            *(int*)SkyCloudVertex[1].v.cn = PACKRGBA(0,0,0,255); // 0xff;
            *(int*)SkyCloudVertex[2].v.cn = PACKRGBA(0,0,21,255); // 0x15ff;
            *(int*)SkyCloudVertex[3].v.cn = PACKRGBA(0,0,21,255); // 0x15ff;
            break;

        case 2:
            SkyFlags = SKF_CLOUD;
            R_RenderSKY = R_RenderCloudSky;

            SkyCloudData = (byte *)W_CacheLumpName("CLOUD", PU_STATIC);
            SkyCloudColor = PACKRGBA(255,48,48,255); // 0xff3030ff;

            *(int*)SkyCloudVertex[0].v.cn = PACKRGBA(16,0,0,255); // 0x100000ff;
            *(int*)SkyCloudVertex[1].v.cn = PACKRGBA(16,0,0,255); // 0x100000ff;
            *(int*)SkyCloudVertex[2].v.cn = PACKRGBA(16,0,0,255); // 0x100000ff;
            *(int*)SkyCloudVertex[3].v.cn = PACKRGBA(16,0,0,255); // 0x100000ff;
            break;

        case 3:
        case 5:
            SkyFlags = SKF_CLOUD;
            R_RenderSKY = R_RenderCloudSky;

            SkyCloudData = (byte *)W_CacheLumpName("CLOUD", PU_STATIC);
            SkyCloudColor = PACKRGBA(208,112,64,255); // 0xd07040ff;

            if (skytexture == 3)
            {
                SkyPicMount = W_GetNumForName("MOUNTB");
                SkyFlags |= SKF_MOUNTAIN;
            }
            else
            {
                FogNear = 975;
                FogColor = PACKRGBA(48,16,8,0); // 0x30100800;
            }

            *(int*)SkyCloudVertex[0].v.cn = PACKRGBA(0,0,0,255); // 0xff;
            *(int*)SkyCloudVertex[1].v.cn = PACKRGBA(0,0,0,255); // 0xff;
            *(int*)SkyCloudVertex[2].v.cn = PACKRGBA(64,16,0,255); // 0x401000ff;
            *(int*)SkyCloudVertex[3].v.cn = PACKRGBA(64,16,0,255); // 0x401000ff;
            break;

        case 4:
        case 9:
		case 20:
            R_RenderSKY = R_RenderFireSky;

            data = W_CacheLumpName("FIRE", PU_LEVEL);
            SkyFireData[0] = (data + 8);
            SkyFireData[1] = Z_Malloc((FIRESKY_WIDTH*FIRESKY_HEIGHT), PU_LEVEL, NULL);

            D_memcpy(SkyFireData[1], SkyFireData[0],(FIRESKY_WIDTH*FIRESKY_HEIGHT));

            if (skytexture == 9)
            {
                *(int*)SkyFireVertex[0].v.cn = PACKRGBA(0,255,0,255); // 0xff00ff;
                *(int*)SkyFireVertex[1].v.cn = PACKRGBA(0,255,0,255); // 0xff00ff;
                *(int*)SkyFireVertex[2].v.cn = PACKRGBA(112,112,0,255); // 0x707000ff;
                *(int*)SkyFireVertex[3].v.cn = PACKRGBA(112,112,0,255); // 0x707000ff;
            }
            else
            {
                *(int*)SkyFireVertex[0].v.cn = PACKRGBA(255,0,0,255); // 0xff0000ff;
                *(int*)SkyFireVertex[1].v.cn = PACKRGBA(255,0,0,255); // 0xff0000ff;
                *(int*)SkyFireVertex[2].v.cn = PACKRGBA(255,96,0,255); // 0xff6000ff;
                *(int*)SkyFireVertex[3].v.cn = PACKRGBA(255,96,0,255); // 0xff6000ff;
			
				if (skytexture == 20)
					{
						SkyPicMount = W_GetNumForName("MOUNTE");
						SkyFlags |= SKF_MOUNTAIN;
					}
            }
            break;

        case 6:
            R_RenderSKY = R_RenderSpaceSky;
            break;

        case 7:
            FogNear = 995;
            R_RenderSKY = R_RenderEvilSky;
            break;

        case 8:
			if (gamemap >= BETALEVEL && gamemap <= LASTLEVEL)
			{
				FogNear = 975;
				SkyFlags = SKF_CLOUD;
				R_RenderSKY = R_RenderCloudSky;

				SkyCloudData = (byte *)W_CacheLumpName("CLOUD", PU_STATIC);
				SkyCloudColor = PACKRGBA(37,0,56,255); // 25 0 38

				*(int*)SkyCloudVertex[0].v.cn = PACKRGBA(0,0,0,255); // 1 0 0
				*(int*)SkyCloudVertex[1].v.cn = PACKRGBA(0,0,0,255); // 1 0 0
				*(int*)SkyCloudVertex[2].v.cn = PACKRGBA(0,0,0,255); // 20 30 0
				*(int*)SkyCloudVertex[3].v.cn = PACKRGBA(0,0,0,255); // 20 30 0;
			}
			else
			{
				R_RenderSKY = R_RenderVoidSky;
				FogNear = 975;
				FogColor = PACKRGBA(0,64,64,0); // 0x404000;
				SkyVoidColor = PACKRGBA(0,56,56,0); // 0x383800;
			}
            break;

        case 11:
			if (gamemap >= BETALEVEL && gamemap <= LASTLEVEL)
			{
				SkyFlags = (SKF_CLOUD|SKF_THUNDER);
				FogNear = 985;
				R_RenderSKY = R_RenderCloudSky;

				SkyCloudData = (byte *)W_CacheLumpName("CLOUD4", PU_STATIC);
				SkyCloudColor = PACKRGBA(0,64,128,255); // 0 40 80

				SkyPicMount = W_GetNumForName("MOUNTA");
				SkyFlags |= SKF_MOUNTAIN;

				*(int*)SkyCloudVertex[0].v.cn = PACKRGBA(0,0,0,255); // 0 0 0
				*(int*)SkyCloudVertex[1].v.cn = PACKRGBA(0,0,0,255); // 0 0 0
				*(int*)SkyCloudVertex[2].v.cn = PACKRGBA(0,0,0,255); // 0 0 0
				*(int*)SkyCloudVertex[3].v.cn = PACKRGBA(0,0,0,255); // 0 0 0
			}
			else
			{
				R_RenderSKY = R_RenderSpaceSky;
				SkyPicMount = W_GetNumForName("MOUNTA");
				SkyFlags |= SKF_MOUNTAIN;
			}
            break;
		
		case 12:
            R_RenderSKY = R_RenderVoidSky;
            FogNear = 950;
            FogColor = PACKRGBA(32,64,0,0); // 20 40 0
            SkyVoidColor = PACKRGBA(64,80,0,0); // 40 50 0
			
            break;
			
        case 13:
            SkyFlags = SKF_CLOUD;
            R_RenderSKY = R_RenderCloudSky;

            SkyCloudData = (byte *)W_CacheLumpName("CLOUD", PU_STATIC);
            SkyCloudColor = PACKRGBA(48,21,0,255); // 30 15 0

			SkyPicMount = W_GetNumForName("MOUNTB");
			SkyFlags |= SKF_MOUNTAIN;

            *(int*)SkyCloudVertex[0].v.cn = PACKRGBA(0,0,0,255); // 0xff;
            *(int*)SkyCloudVertex[1].v.cn = PACKRGBA(0,0,0,255); // 0xff;
            *(int*)SkyCloudVertex[2].v.cn = PACKRGBA(16,5,0,255); // 10 5 0;
            *(int*)SkyCloudVertex[3].v.cn = PACKRGBA(16,5,0,255); // 10 5 0;
            break;
		
        case 14:
            SkyFlags = SKF_CLOUD;
            R_RenderSKY = R_RenderCloudSky;

            SkyCloudData = (byte *)W_CacheLumpName("CLOUD4", PU_STATIC);
            SkyCloudColor = PACKRGBA(96,128,0,255); // 60 80 0

			SkyPicMount = W_GetNumForName("MOUNTD");
			SkyFlags |= SKF_MOUNTAIN;

            *(int*)SkyCloudVertex[0].v.cn = PACKRGBA(1,0,0,255); // 1 0 0
            *(int*)SkyCloudVertex[1].v.cn = PACKRGBA(1,0,0,255); // 1 0 0
            *(int*)SkyCloudVertex[2].v.cn = PACKRGBA(32,48,0,255); // 20 30 0
            *(int*)SkyCloudVertex[3].v.cn = PACKRGBA(32,48,0,255); // 20 30 0;
            break;
			
        case 15:
            SkyFlags = (SKF_CLOUD|SKF_THUNDER);
            R_RenderSKY = R_RenderCloudSky;

            SkyCloudData = (byte *)W_CacheLumpName("CLOUD", PU_STATIC);
            SkyCloudColor = PACKRGBA(53,32,0,255); // 35 20 0

			SkyPicMount = W_GetNumForName("MOUNTB");
			SkyFlags |= SKF_MOUNTAIN;

            *(int*)SkyCloudVertex[0].v.cn = PACKRGBA(0,0,0,255); // 0 0 0
            *(int*)SkyCloudVertex[1].v.cn = PACKRGBA(0,0,0,255); // 0 0 0
            *(int*)SkyCloudVertex[2].v.cn = PACKRGBA(16,5,0,255); // 10 5 0
            *(int*)SkyCloudVertex[3].v.cn = PACKRGBA(16,5,0,255); // 10 5 0;
            break;
			
        case 16:
            R_RenderSKY = R_RenderFireSky;
			FogColor = PACKRGBA(64,16,8,0); // 40 10 8

            data = W_CacheLumpName("FIRE", PU_LEVEL);
            SkyFireData[0] = (data + 8);
            SkyFireData[1] = Z_Malloc((FIRESKY_WIDTH*FIRESKY_HEIGHT), PU_LEVEL, NULL);

            D_memcpy(SkyFireData[1], SkyFireData[0],(FIRESKY_WIDTH*FIRESKY_HEIGHT));

			*(int*)SkyFireVertex[0].v.cn = PACKRGBA(255,0,0,255); // ff 20 0;
			*(int*)SkyFireVertex[1].v.cn = PACKRGBA(255,0,0,255); //ff 20 0;
			*(int*)SkyFireVertex[2].v.cn = PACKRGBA(255,32,0,255); // ff 20 0;
			*(int*)SkyFireVertex[3].v.cn = PACKRGBA(255,32,0,255); // ff 20 0;
            break;
			
		case 17:
            R_RenderSKY = R_RenderVoidSky;
            FogNear = 965;
            FogColor = PACKRGBA(80,7,2,0); // 50 7 2
            SkyVoidColor = PACKRGBA(80,7,2,0); // 50 7 2
            break;
			
		case 18:
            SkyFlags = (SKF_CLOUD|SKF_THUNDER);
            R_RenderSKY = R_RenderCloudSky;
            FogNear = 965;
            FogColor = PACKRGBA(80,7,2,0); // 50 7 2
            SkyCloudData = (byte *)W_CacheLumpName("CLOUD4", PU_STATIC);
            SkyCloudColor = PACKRGBA(48,7,2,0); // 30 7 2

            *(int*)SkyCloudVertex[0].v.cn = PACKRGBA(0,0,0,255); // 0 0 0
            *(int*)SkyCloudVertex[1].v.cn = PACKRGBA(0,0,0,255); // 0 0 0
            *(int*)SkyCloudVertex[2].v.cn = PACKRGBA(80,7,2,255); // 50 7 2
            *(int*)SkyCloudVertex[3].v.cn = PACKRGBA(80,7,2,255); // 50 7 2
            break;
					
        case 19:
			R_RenderSKY = R_RenderSpaceSky;
			SkyPicMount = W_GetNumForName("MOUNTC");
			SkyFlags |= SKF_MOUNTAIN;
            break;
			
		case 21: // Black void sky to help rendering in underground facilities and caves
			FogNear = 975;
            R_RenderSKY = R_RenderVoidSky;
            FogColor = PACKRGBA(0,0,0,0); // 0 0 0
            SkyVoidColor = PACKRGBA(0,0,0,0); // 0 0 0
            break;
    }
}

void R_RenderSpaceSky(void) // 80025440
{

    gDPSetAlphaCompare(GFX1++, G_AC_NONE);
    gDPSetCombineMode(GFX1++, G_CC_D64COMB09, G_CC_D64COMB09);
    gDPSetRenderMode(GFX1++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetPrimColor(GFX1++, 0, (lights[255].rgba >> 8), 0, 0, 0, 255);

    R_RenderSkyPic(SkyPicSpace, 128);

	if (SkyFlags & SKF_THUNDER)
	R_CloudThunder();
	
    if (SkyFlags & SKF_MOUNTAIN)
    {
        gDPPipeSync(GFX1++);
        gDPSetAlphaCompare(GFX1++, G_AC_THRESHOLD);
        gDPSetCombineMode(GFX1++, G_CC_D64COMB10, G_CC_D64COMB10);
        gDPSetRenderMode(GFX1++, G_RM_TEX_EDGE, G_RM_TEX_EDGE2);

        R_RenderSkyPic(SkyPicMount, 170);
    }
}

void R_RenderCloudSky(void) // 800255B8
{
    if (SkyFlags & SKF_CLOUD)
        R_RenderClouds();

    if (SkyFlags & SKF_THUNDER)
        R_CloudThunder();

    if (SkyFlags & SKF_MOUNTAIN)
    {
        gDPPipeSync(GFX1++);
        gDPSetAlphaCompare(GFX1++, G_AC_THRESHOLD);
        gDPSetCombineMode(GFX1++, G_CC_D64COMB10, G_CC_D64COMB10);
        gDPSetRenderMode(GFX1++, G_RM_TEX_EDGE, G_RM_TEX_EDGE2);

        R_RenderSkyPic(SkyPicMount, 170);
    }
}

void R_RenderVoidSky(void) // 800256B4
{

    gDPSetCycleType(GFX1++, G_CYC_FILL);
    gDPSetRenderMode(GFX1++,G_RM_NOOP,G_RM_NOOP2);

    // Fill borders with SkyVoidColor
    gDPSetFillColor(GFX1++, SkyVoidColor);
    gDPFillRectangle(GFX1++, 0, 0, SCREEN_WD-1, SCREEN_HT-1);
	
	if (SkyFlags & SKF_THUNDER)
	R_CloudThunder();
}

void R_RenderEvilSky(void) // 80025738
{
    int color;

    gDPSetPrimColor(GFX1++, 0, ((lights[255].rgba >> 8)  - Skyfadeback), 0, 0, 0, 255);
    gDPSetAlphaCompare(GFX1++, G_AC_NONE);
    gDPSetCombineMode(GFX1++, G_CC_D64COMB09, G_CC_D64COMB09);
    gDPSetRenderMode(GFX1++,G_RM_OPA_SURF,G_RM_OPA_SURF2);

    R_RenderSkyPic(SkyPicSpace, 128);
    R_RenderSkyPic(SkyPicSpace, 240);

    if (Skyfadeback)
    {
        Skyfadeback += 4;

        if (Skyfadeback > 255)
            Skyfadeback = 255;

        if (Skyfadeback > 128)
            color = 128;
        else
            color = Skyfadeback;

        M_DrawBackground(63, 25, color, EpisodeGraphic());
    }
}

void R_RenderClouds(void) // 80025878
{
    int x, y;

    gDPSetCycleType(GFX1++, G_CYC_2CYCLE);
    gDPSetTexturePersp(GFX1++, G_TP_PERSP);
    gDPSetTextureLUT(GFX1++, G_TT_NONE);
    gDPSetCombineMode(GFX1++, G_CC_D64COMB11, G_CC_D64COMB12);
    gDPSetRenderMode(GFX1++,G_RM_OPA_SURF,G_RM_OPA_SURF2);

    gDPSetPrimColorD64(GFX1++, 0, (lights[255].rgba >> 8), SkyCloudColor);

    if (!gamepaused)
    {
        SkyCloudOffsetX = (SkyCloudOffsetX - (viewcos >> 14)) & 16383;
        SkyCloudOffsetY = (SkyCloudOffsetY + (viewsin >> 13)) & 16383;
    }

    x = SkyCloudOffsetX - (viewangle >> ANGLETOFINESHIFT);
    y = SkyCloudOffsetY;

    D_memcpy(VTX1, SkyCloudVertex, sizeof(Vtx)*4);

    VTX1[0].v.tc[0] = x;
    VTX1[1].v.tc[0] = x + (64 << 6);
    VTX1[2].v.tc[0] = x + (64 << 6);
    VTX1[3].v.tc[0] = x;

    VTX1[0].v.tc[1] = y;
    VTX1[1].v.tc[1] = y;
    VTX1[2].v.tc[1] = y + (128 << 6);
    VTX1[3].v.tc[1] = y + (128 << 6);

    gDPSetTextureImage(GFX1++, G_IM_FMT_I, G_IM_SIZ_16b , 1, (SkyCloudData+8));
    gDPSetTile(GFX1++, G_IM_FMT_I, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, 0, 6, 0, 0, 6, 0);

    gDPLoadSync(GFX1++);
    gDPLoadBlock(GFX1++, G_TX_LOADTILE, 0, 0, (((64 * 64) -1) >> 1), 0);

    gDPPipeSync(GFX1++);
    gDPSetTile(GFX1++, G_IM_FMT_I, G_IM_SIZ_8b, 8, 0, G_TX_RENDERTILE, 0, 0, 6, 0, 0, 6, 0);
    gDPSetTileSize(GFX1++, G_TX_RENDERTILE, 0, 0, (63 << 2), (63 << 2));
    gSPTexture(GFX1++, (768 << 6), (512 << 6), 0, G_TX_RENDERTILE, G_ON);

    gSPVertex(GFX1++, VTX1, 4, 0);
    gSP1Triangle(GFX1++, 0, 2, 1, 0);
    gSP1Triangle(GFX1++, 0, 3, 2, 0);

    VTX1 += 4;
}

void R_RenderSkyPic(int lump, int yoffset) // 80025BDC
{
    byte *data;
    byte *src;
    byte *paldata;
    int i;

    int tileh;
    int yl, yh;
    int ang;
    int lrs;

    data = W_CacheLumpNum(lump, PU_CACHE);

    ang = ((0 - (viewangle >> 22)) & 255);
    tileh = ((spriteN64_t*)data)->tileheight;

    src = data + sizeof(spriteN64_t);
    paldata = (src + ((spriteN64_t*)data)->cmpsize);

    gDPSetCycleType(GFX1++, G_CYC_1CYCLE);

    gDPSetTexturePersp(GFX1++, G_TP_NONE);
    gDPSetTextureLUT(GFX1++, G_TT_RGBA16);
    gDPSetBlendColor(GFX1++, 0, 0, 0, 0);

    // Load Palette Data (256 colors)
    gDPSetTextureImage(GFX1++, G_IM_FMT_RGBA, G_IM_SIZ_16b , 1, paldata);

    gDPTileSync(GFX1++);
    gDPSetTile(GFX1++, G_IM_FMT_RGBA, G_IM_SIZ_4b, 0, 256, G_TX_LOADTILE, 0, 0, 0, 0, 0, 0, 0);

    gDPLoadSync(GFX1++);
    gDPLoadTLUTCmd(GFX1++, G_TX_LOADTILE, 255);

    gDPPipeSync(GFX1++);

    lrs = (((tileh << 8) + 1) >> 1) - 1;
    yl = (yoffset - ((spriteN64_t*)data)->height);

    for(i = 0; i < ((spriteN64_t*)data)->tiles; i++)
    {
        yh = (tileh + yl);

        gDPSetTextureImage(GFX1++, G_IM_FMT_CI, G_IM_SIZ_16b , 1, src);
        gDPSetTile(GFX1++, G_IM_FMT_CI, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, 0, 0, 0, 0, 8, 0);

        gDPLoadSync(GFX1++);
        gDPLoadBlock(GFX1++, G_TX_LOADTILE, 0, 0, lrs, 0);

        gDPPipeSync(GFX1++);
        gDPSetTile(GFX1++, G_IM_FMT_CI, G_IM_SIZ_8b, 32, 0, G_TX_RENDERTILE, 0, 0, 0, 0, 0, 8, 0);
        gDPSetTileSize(GFX1++, 0, 0, 0, ((256-1) << 2), ((tileh - 1) << 2));

        gSPTextureRectangle(GFX1++, (0 << 2), (yl << 2),
                                    (320 << 2), (yh << 2),
                                    G_TX_RENDERTILE,
                                    (ang << 5), (0 << 5),
                                    (1 << 10), (1 << 10));
        src += (tileh << 8);
        yl = yh;
    }
}

void R_RenderFireSky(void) // 80025F68
{
    byte *buff;
    byte *src, *srcoffset, *tmpSrc;
    int width, height, rand;
    int pixel, randIdx;
    int ang, t;

    gDPSetCycleType(GFX1++, G_CYC_2CYCLE);
    gDPSetTexturePersp(GFX1++, G_TP_PERSP);
    gDPSetTextureLUT(GFX1++, G_TT_NONE);
    gDPSetCombineMode(GFX1++, G_CC_D64COMB13, G_CC_D64COMB14);
    gDPSetRenderMode(GFX1++,G_RM_OPA_SURF,G_RM_OPA_SURF2);
    gDPSetPrimColor(GFX1++, 0, (lights[255].rgba >> 8), 0, 0, 0, 255);

    if (((gamevbls < gametic) && (gametic & 1)) && (!gamepaused))
    {
        buff = SkyFireData[FireSide];
        D_memcpy(buff, SkyFireData[FireSide ^ 1], (FIRESKY_WIDTH*FIRESKY_HEIGHT));

        rand = (M_Random() & 0xff);
        width = 0;
        src = (buff + FIRESKY_WIDTH);

        do // width
        {
            height = 2;
            srcoffset = (src + width);

            // R_SpreadFire
            pixel = *(byte*)srcoffset;
            if (pixel != 0)
            {
                randIdx = rndtable[rand];
                rand = ((rand + 2) & 0xff);

                tmpSrc = (src + (((width - (randIdx & 3)) + 1) & (FIRESKY_WIDTH-1)));
                *(byte*)(tmpSrc - FIRESKY_WIDTH) = pixel - ((randIdx & 1) << 4);
            }
            else
            {
                *(byte*)(srcoffset - FIRESKY_WIDTH) = 0;
            }

            src += FIRESKY_WIDTH;
            srcoffset += FIRESKY_WIDTH;

            do // height
            {
                height += 2;

                // R_SpreadFire
                pixel = *(byte*)srcoffset;
                if (pixel != 0)
                {
                    randIdx = rndtable[rand];
                    rand = ((rand + 2) & 0xff);

                    tmpSrc = (src + (((width - (randIdx & 3)) + 1) & (FIRESKY_WIDTH-1)));
                    *(byte*)(tmpSrc - FIRESKY_WIDTH) = pixel - ((randIdx & 1) << 4);
                }
                else
                {
                    *(byte*)(srcoffset - FIRESKY_WIDTH) = 0;
                }

                src += FIRESKY_WIDTH;
                srcoffset += FIRESKY_WIDTH;

                // R_SpreadFire
                pixel = *(byte*)srcoffset;
                if (pixel != 0)
                {
                    randIdx = rndtable[rand];
                    rand = ((rand + 2) & 0xff);

                    tmpSrc = (src + (((width - (randIdx & 3)) + 1) & (FIRESKY_WIDTH-1)));
                    *(byte*)(tmpSrc - FIRESKY_WIDTH) = pixel - ((randIdx & 1) << 4);
                }
                else
                {
                    *(byte*)(srcoffset - FIRESKY_WIDTH) = 0;
                }

                src += FIRESKY_WIDTH;
                srcoffset += FIRESKY_WIDTH;

            } while (height != FIRESKY_HEIGHT);

            src -= ((FIRESKY_WIDTH*FIRESKY_HEIGHT) - FIRESKY_WIDTH);
            width++;

        } while (width != FIRESKY_WIDTH);

        FireSide ^= 1;
    }
    else
    {
        buff = SkyFireData[FireSide ^ 1];
    }

    D_memcpy(VTX1, SkyFireVertex, sizeof(Vtx)*4);

    ang = (viewangle >> 22);
    t = ((-ang & 255) << 5);

    VTX1[0].v.tc[0] = t;
    VTX1[1].v.tc[0] = t + 0x2800;
    VTX1[2].v.tc[0] = t + 0x2800;
    VTX1[3].v.tc[0] = t;

    gDPSetTextureImage(GFX1++, G_IM_FMT_I, G_IM_SIZ_16b , 1, buff);
    gDPSetTile(GFX1++, G_IM_FMT_I, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, 0, 0, 0, 0, 6, 0);

    gDPLoadSync(GFX1++);
    gDPLoadBlock(GFX1++, G_TX_LOADTILE, 0, 0, (((64 * 64) -1) >> 1), 256);

    gDPPipeSync(GFX1++);
    gDPSetTile(GFX1++, G_IM_FMT_I, G_IM_SIZ_8b, 8, 0, G_TX_RENDERTILE, 0, 0, 0, 0, 0, 6, 0);
    gDPSetTileSize(GFX1++, G_TX_RENDERTILE, 0, 0, (63 << 2), (63 << 2));
    gSPTexture(GFX1++, (1024 << 6)-1, (512 << 6), 0, G_TX_RENDERTILE, G_ON);

    gSPVertex(GFX1++, VTX1, 4, 0);
    gSP1Triangle(GFX1++, 0, 2, 1, 0);
    gSP1Triangle(GFX1++, 0, 3, 2, 0);

    VTX1 += 4;

    if (SkyFlags & SKF_MOUNTAIN)
    {
        gDPPipeSync(GFX1++);
        gDPSetAlphaCompare(GFX1++, G_AC_THRESHOLD);
        gDPSetCombineMode(GFX1++, G_CC_D64COMB10, G_CC_D64COMB10);
        gDPSetRenderMode(GFX1++, G_RM_TEX_EDGE, G_RM_TEX_EDGE2);

        R_RenderSkyPic(SkyPicMount, 170);
    }

}

void R_CloudThunder(void) // 80026418
{
    int rand;
    if (!(gamepaused))
    {
        ThunderCounter -= vblsinframe[0];

        if(ThunderCounter <= 0)
        {
            if (LightningCounter == 0)
            {
                S_StartSound(NULL, sfx_thndrlow + (M_Random() & 1));
            }
            else if(!(LightningCounter < 6))    // Reset loop after 6 lightning flickers
            {
                rand = (M_Random() & 7);
                ThunderCounter = (((rand << 4) - rand) << 2) + 60;
                LightningCounter = 0;
				LightningFlash = 0; // reset flash
                return;
            }

            if ((LightningCounter & 1) == 0)
            {
				LightningFlash += 30; // get brighter
                *(int*)SkyCloudVertex[0].v.cn = PACKRGBA(LightningFlash,LightningFlash,LightningFlash,0);
                *(int*)SkyCloudVertex[1].v.cn = *(int*)SkyCloudVertex[0].v.cn;
            }
            else
            {
				LightningFlash -= 30; // get darker
                *(int*)SkyCloudVertex[0].v.cn = PACKRGBA(LightningFlash,LightningFlash,LightningFlash,0);
                *(int*)SkyCloudVertex[1].v.cn = *(int*)SkyCloudVertex[0].v.cn;
            }

            ThunderCounter = (M_Random() & 7) + 1; // Do short delay loops for lightning flickers
            LightningCounter += 1;
        }
    }
}

