/* m_main.c -- menu routines */

#include "doomdef.h"
#include "r_local.h"
#include "st_main.h"

extern void P_RefreshBrightness(void);

//intermission
int DrawerStatus;

#define CT_TXT00	"default: %d"
#define CT_TXT01	"right"
#define CT_TXT02	"left"
#define CT_TXT03	"forward"
#define CT_TXT04	"backward"
#define CT_TXT05	"attack"
#define CT_TXT06	"use"
#define CT_TXT07	"map"
#define CT_TXT08	"speed"
#define CT_TXT09	"strafe on"
#define CT_TXT10	"strafe left"
#define CT_TXT11	"strafe right"
#define CT_TXT12	"weapon backward"
#define CT_TXT13	"weapon forward"
#define CT_TXT14    "autorun: %s"

char *ControlText[] =   //8007517C
{
    CT_TXT14, 
    CT_TXT00, CT_TXT01, CT_TXT02, CT_TXT03, CT_TXT04,
	CT_TXT05, CT_TXT06, CT_TXT07, CT_TXT08, CT_TXT09,
	CT_TXT10, CT_TXT11, CT_TXT12, CT_TXT13
};

#define M_TXT00	"Control Pad"
#define M_TXT01	"Volume"
#define M_TXT02	"Display"
#define M_TXT03	"Password"
#define M_TXT04	"Main Menu"
#define M_TXT05	"Restart Level"
#define M_TXT06	"\x90 Return"
#define M_TXT07 "Music Volume"
#define M_TXT08 "Sound Volume"
#define M_TXT09 "Brightness"
#define M_TXT10 "Resume"
#define M_TXT11 "Options"
#define M_TXT12 "Default" // Default Audio
#define M_TXT13 "Default" // Default Display
#define M_TXT14 "New Game"
#define M_TXT15 "Be Gentle!"
#define M_TXT16 "Bring it on!"
#define M_TXT17 "I own Doom!"
#define M_TXT18 "Watch me die!"
#define M_TXT19 "Hardcore!"
#define M_TXT20 "Yes"
#define M_TXT21 "No"
#define M_TXT22 "Features"
#define M_TXT23 "WARP TO LEVEL"
#define M_TXT24 "INVULNERABLE"
#define M_TXT25 "HEALTH BOOST"
#define M_TXT26 "SECURITY KEYS"
#define M_TXT27 "WEAPONS"
#define M_TXT28 "Exit"
#define M_TXT29 "DEBUG"
#define M_TXT30 "TEXTURE TEST"
#define M_TXT31 "WALL BLOCKING"
#define M_TXT32 "Center Video"
#define M_TXT33 "Messages:"
#define M_TXT34 "Status Bar:"
#define M_TXT35 "LOCK MONSTERS"
#define M_TXT36 "SCREENSHOT"
#define M_TXT37 "MAP EVERYTHING"
#define M_TXT38 "MACRO PEEK"
#define M_TXT39 "MUSIC TEST"
#define M_TXT40 "WARP TO FUN"
#define M_TXT41 "Control Stick"
#define M_TXT42 "Default"
#define M_TXT43 "Sensitivity"
#define M_TXT44 "Manage Pak"
#define M_TXT45 "Do not use Pak"
#define M_TXT46 "Try again"
#define M_TXT47 "Create game note"

// Doom 64 RE options

#define M_TXT48 "COLORS"     // [GEC] NEW CHEAT CODE
#define M_TXT49 "FULL BRIGHT"   // [GEC] NEW CHEAT CODE
#define M_TXT50 "Filtering:"   // Now Menu feature

#define M_TXT51 "Absolution"

// Early CE Menu Texts

#define M_TXT52 "Lost Levels"
#define M_TXT53 "Artifacts"
#define M_TXT54 "Skill"
#define M_TXT55 "Load Game"
#define M_TXT56 "Blood Style:"
#define M_TXT57 "Colored HUD:"

// [Immorpher] Additional Texts
#define M_TXT58 "Video"
#define M_TXT59 "Motion Bob:" // Motion bob scaler
#define M_TXT60 "Map Stats:" // Automap stats
#define M_TXT61 "Bonus Pak" // Bonus level hub
#define M_TXT62 "Beta 64" // Antnee's Beta 64
#define M_TXT63 "WARP TO FINAL" // Final map warp
#define M_TXT64 "Credits" // Credits
#define M_TXT65 "Deadzone:" // Analog stick deadzone
#define M_TXT66 "Flash Level:" // Flash brightness
#define M_TXT67 "Default" // Video default settings

const char *MenuText[] =   // 8005ABA0
{
    M_TXT00, M_TXT01, M_TXT02, M_TXT03, M_TXT04,
    M_TXT05, M_TXT06, M_TXT07, M_TXT08, M_TXT09,
    M_TXT10, M_TXT11, M_TXT12, M_TXT13, M_TXT14,
    M_TXT15, M_TXT16, M_TXT17, M_TXT18, M_TXT19,
    M_TXT20, M_TXT21, M_TXT22, M_TXT23, M_TXT24,
    M_TXT25, M_TXT26, M_TXT27, M_TXT28, M_TXT29,
    M_TXT30, M_TXT31, M_TXT32, M_TXT33, M_TXT34,
    M_TXT35, M_TXT36, M_TXT37, M_TXT38, M_TXT39,
    M_TXT40, M_TXT41, M_TXT42, M_TXT43, M_TXT44,
    M_TXT45, M_TXT46, M_TXT47, M_TXT48, M_TXT49,
	M_TXT50, M_TXT51, M_TXT52, M_TXT53, M_TXT54,
    M_TXT55, M_TXT56, M_TXT57, M_TXT58, M_TXT59,
	M_TXT60, M_TXT61, M_TXT62, M_TXT63, M_TXT64,
	M_TXT65, M_TXT66, M_TXT67
};

const menuitem_t Menu_Title[3] = // 8005A978
{
    { 14, 115, 170 },   // New Game
    { 55, 115, 190 },   // Load Game
	{ 11, 115, 210 },   // Options
};

const menuitem_t Menu_TitleNoSave[3] =
{
    { 14, 115, 170 },   // New Game
    {  3, 115, 190},    // Password
	{ 11, 115, 210 },   // Options
};

menuitem_t Menu_Skill[6] = // 8005A990
{
    { 15, 102, 70 },    // Be Gentle!
    { 16, 102, 90},    // Bring it on!
    { 17, 102, 110},    // I own Doom!
    { 18, 102, 130},    // Watch me die!
    { 19, 102, 150},    // Be merciless!
    { 6, 102, 180},    	// Return
};

const menuitem_t Menu_Episode[5] =
{
    { 51, 102, 80 },    // Doom 64
    { 52, 102, 100},    // The Lost Levels
    { 62, 102, 120},    // Beta 64
    { 61, 102, 140},    // Bonus Pak
    { 6, 102, 170},    	// Return
};

menuitem_t Menu_Options[8] = // 8005A9C0
{
    {  0, 102, 60 },    // Control Pad
    { 41, 102, 80 },    // Control Stick
    {  1, 102, 100},    // Volume
    {  58, 102, 120},    // Video
    {  2, 102, 140},    // Display
    {  3, 102, 160},    // Password
    { 64, 102, 180},    // Credits
    {  6, 102, 200},    // Return
};

const menuitem_t Menu_Volume[4] = // 8005AA08
{
    {  7, 102, 60 },    // Music Volume
    {  8, 102, 100},    // Sound Volume
    { 12, 102, 140},    // Default Volume
    {  6, 102, 160},    // Return
};

const menuitem_t Menu_ControlStick[4] = // 8005AA38
{
    { 43, 102, 70 },    // Sensitivity
    { 65, 102, 110},    // Deadzone
    { 42, 102, 130},    // Defaults
    {  6, 102, 150},    // Return
};

const menuitem_t Menu_Video[7] =
{
    {  9, 102, 60 },    // Brightness
    { 32, 102, 100 },    // Center Display
    { 50, 102, 120},    // Filtering
    { 59, 102, 140},    // Motion Bob
    { 66, 102, 160},    // Flash Level
    { 67, 102, 180},    // Video default
    {  6, 102, 200},    // Return
};

const menuitem_t Menu_Display[7] = // 8005AA5C
{
    { 33, 102, 60},    // Messages
    { 34, 102, 80},    // Status Bar
	{ 60, 102, 100},    // Map Stats
    { 56, 102, 120},    // Blood Color
    { 57, 102, 140},    // Colored HUD
    { 13, 102, 160},    // Default Display
    {  6, 102, 180},    // Return
};

const menuitem_t Menu_Game[5] = // 8005AAA4
{
    { 11, 122, 80 },    // Options
    {  4, 122, 100},    // Main Menu
    {  5, 122, 120},    // Restart Level
    { 55, 122, 140},    // Load Game
    { 22, 122, 160},    // Features
};

const menuitem_t Menu_GameNoSave[4] =
{
    { 11, 122, 80 },    // Options
    {  4, 122, 100},    // Main Menu
    {  5, 122, 120},    // Restart Level
    { 22, 122, 140},    // Features
};

const menuitem_t Menu_Quit[2] = // 8005AAD4
{
    { 20, 142, 100},    // Yes
    { 21, 142, 120},    // No
};

const menuitem_t Menu_DeleteNote[2] = // 8005AAEC
{
    { 20, 142, 100},    // Yes
    { 21, 142, 120},    // No
};

const menuitem_t Menu_ControllerPakBad[2] = // 8005AB04
{
    { 46, 120, 100},    // Try again
    { 45, 120, 120},    // Do not use Pak
};

const menuitem_t Menu_ControllerPakFull[3] = // 8005AB1C
{
    { 44, 110, 90 },    // Manage Pak
    { 47, 110, 110},    // Create game note
    { 45, 110, 130},    // Do not use Pak
};

const menuitem_t Menu_CreateNote[3] = // 8005AB40
{
    { 20, 110, 90 },    // Yes
    { 45, 110, 110},    // Do not use Pak
    { 44, 110, 130},    // Manage Pak
};

#define MAXFEATURES 13
const menuitem_t Menu_Features[MAXFEATURES] = // 8005AB64
{
    { 23, 40, 50},      // WARP TO LEVEL
    { 24, 40, 60},      // INVULNERABLE
    { 25, 40, 70},      // HEALTH BOOST
    { 27, 40, 80},      // WEAPONS
    { 37, 40, 90},      // MAP EVERYTHING
    //
    { 53, 40, 100},      // ARTIFACTS
    { 54, 40, 110},      // SKILL
    //
    { 26, 40, 120},      // SECURITY KEYS
    { 31, 40, 130},      // WALL BLOCKING
    { 35, 40, 140},      // LOCK MONSTERS
    { 39, 40, 150},      // MUSIC TEST
    //
    { 48, 40, 160},      // COLORS [GEC] NEW CHEAT CODE
    { 49, 40, 170},      // FULL BRIGHT [GEC] NEW CHEAT CODE


};

menudata_t MenuData[8]; // 800A54F0
short MenuAnimationTic;   // 800a5570
short cursorpos;          // 800A5574
int m_vframe1;          // 800A5578
const menuitem_t *MenuItem;   // 800A5578
short itemlines;          // 800A5580
menufunc_t MenuCall;    // 800A5584

unsigned char linepos;            // 800A5588
int text_alpha_change_value;    // 800A558C
unsigned char MusicID;            // 800A5590
unsigned char m_actualmap;        // 800A5594
int last_ticon;         // 800A5598

skill_t startskill;     // 800A55A0
unsigned char startmap;           // 800A55A4
boolean EnableMemPak;       // 800A55A8

//-----------------------------------------

unsigned char MenuIdx = 0;                // 8005A7A4
short text_alpha = 255;           // 8005A7A8
unsigned char ConfgNumb = 0;              // 8005A7AC
char Display_X = 0;              // 8005A7B0
char Display_Y = 0;              // 8005A7B4
boolean enable_messages = true; // 8005A7B8
boolean enable_statusbar = true;// 8005A7BC
char SfxVolume = 100;             // 8005A7C0
char MusVolume = 100;             // 8005A7C4
char brightness = 60;             // 8005A7C8
char PlayDeadzone = 10;			// Analog stick deadzone for the gameplay
char M_SENSITIVITY = 27;          // Analog stick sensitivity
int	 MotionBob = 0x100003;		// Video motion bob - 16 pixels of bob is 0x100000
char FlashLevel = 0;				// Flash reduction parameter, 0 is maximum flash brightness
boolean FeaturesUnlocked = true; // 8005A7D0
boolean runintroduction = false; // [Immorpher] New introduction sequence which gets turned on only when starting a new campaign
char TextureFilter = 0;
char Autorun = 0;
byte SavedConfig[16];
char BloodStyle = 3; // Blood style: red, green, dust, combo
boolean ColoredHUD = 1; // Hud color
boolean ShowStats = 1; // Automap stats

int TempConfiguration[13] = // 8005A80C
{
    PAD_LEFT, PAD_RIGHT, PAD_UP, PAD_DOWN,
    PAD_LEFT_C, PAD_RIGHT_C, PAD_UP_C, PAD_DOWN_C,
    PAD_L_TRIG, PAD_R_TRIG, PAD_A, PAD_B, PAD_Z_TRIG
};

int ActualConfiguration[13] = // 8005A840
{
    PAD_RIGHT, PAD_LEFT, PAD_UP, PAD_DOWN,
    PAD_Z_TRIG,
    PAD_RIGHT_C, PAD_UP_C, PAD_LEFT_C, PAD_DOWN_C,
    PAD_L_TRIG, PAD_R_TRIG, PAD_A, PAD_B
};

int DefaultConfiguration[6][13] = // 8005A840
{
    // Default 1
    {
        PAD_RIGHT, PAD_LEFT, PAD_UP, PAD_DOWN,
        PAD_Z_TRIG,
        PAD_RIGHT_C, PAD_UP_C, PAD_LEFT_C, PAD_DOWN_C,
        PAD_L_TRIG, PAD_R_TRIG, PAD_A, PAD_B
    },

    // Default 2
    {
        PAD_RIGHT, PAD_LEFT, PAD_UP, PAD_DOWN,
        PAD_Z_TRIG,
        PAD_RIGHT_C, PAD_UP_C,
        PAD_R_TRIG, PAD_L_TRIG,
        PAD_A, PAD_DOWN_C,
        PAD_B, PAD_LEFT_C
    },

    // Default 3
    {
        PAD_RIGHT, PAD_LEFT, PAD_UP, PAD_DOWN,
        PAD_Z_TRIG,
        PAD_UP_C, PAD_UP,
        PAD_R_TRIG,
        PAD_DOWN, PAD_LEFT_C, PAD_RIGHT_C,
        PAD_A, PAD_B
    },

    // Default 4
    {
        PAD_RIGHT_C, PAD_LEFT_C,
        PAD_UP, PAD_DOWN,
        PAD_Z_TRIG,
        PAD_UP, PAD_UP_C,
        PAD_L_TRIG,
        PAD_DOWN_C, PAD_LEFT, PAD_RIGHT,
        PAD_A, PAD_B
    },

    // Default 5
    {
        PAD_RIGHT, PAD_LEFT, PAD_UP, PAD_DOWN,
        PAD_A, PAD_RIGHT_C, PAD_UP_C, PAD_DOWN_C,
        PAD_Z_TRIG, PAD_L_TRIG, PAD_R_TRIG,
        PAD_B, PAD_LEFT_C
    },
	
    // Modern Controller
    {
        PAD_RIGHT, PAD_LEFT, PAD_UP_C, PAD_DOWN_C,
        PAD_Z_TRIG,
        PAD_B, PAD_A, PAD_UP, PAD_DOWN,
        PAD_LEFT_C, PAD_RIGHT_C, PAD_L_TRIG, PAD_R_TRIG
	}
};

//-----------------------------------------

void M_EncodeConfig(void)
{
    unsigned char i;
    int controlKey[13];


	SavedConfig[3] = (PlayDeadzone>>1) & 0x7; //0-7 - 3 bits
	SavedConfig[3] += (ConfgNumb & 0x7) << 3; //0-5 - is this needed? - 3 bits
    SavedConfig[3] += (BloodStyle & 0x3) << 6; //0-3 - 2 bits

    SavedConfig[4] = MusVolume & 0x7F; //0-127 - 7 bits
    SavedConfig[4] += (FeaturesUnlocked & 0x1) << 7; // 1 bit
    
    SavedConfig[5] = SfxVolume & 0x7F; //0-127 - 7 bits
    SavedConfig[5] += (enable_messages & 0x1) << 7; // 1 bit

    SavedConfig[6] = brightness & 0x7F; //0-127 - 7 bits
    SavedConfig[6] += (enable_statusbar & 0x1) << 7; // 1 bit

    SavedConfig[7] = M_SENSITIVITY & 0x7F; //0-127 - 7 bits
	SavedConfig[7] += (ShowStats & 0x1) << 7; // 1 bit

    for (i = 0; i < 13; i++)
    {
        if (ActualConfiguration[i] == PAD_A)
        {
            controlKey[i] = 0;
        }
        else if (ActualConfiguration[i] == PAD_B)
        {
            controlKey[i] = 1;
        }
        else if (ActualConfiguration[i] == PAD_DOWN)
        {
            controlKey[i] = 2;
        }
        else if (ActualConfiguration[i] == PAD_DOWN_C)
        {
            controlKey[i] = 3;
        }
        else if (ActualConfiguration[i] & PAD_L_TRIG)
        {
            controlKey[i] = 4;
        }
        else if (ActualConfiguration[i] == PAD_LEFT)
        {
            controlKey[i] = 5;
        }
        else if (ActualConfiguration[i] == PAD_LEFT_C)
        {
            controlKey[i] = 6;
        }
        else if (ActualConfiguration[i] == PAD_R_TRIG)
        {
            controlKey[i] = 7;
        }
        else if (ActualConfiguration[i] == PAD_RIGHT)
        {
            controlKey[i] = 8;
        }
        else if (ActualConfiguration[i] == PAD_RIGHT_C)
        {
            controlKey[i] = 9;
        }
        else if (ActualConfiguration[i] == PAD_START)
        {
            controlKey[i] = 10;
        }
        else if (ActualConfiguration[i] == PAD_UP)
        {
            controlKey[i] = 11;
        }
        else if (ActualConfiguration[i] == PAD_UP_C)
        {
            controlKey[i] = 12;
        }
        else if (ActualConfiguration[i] == PAD_Z_TRIG)
        {
            controlKey[i] = 13;
        }
    }

    SavedConfig[8] = controlKey[0] & 0xF;
    SavedConfig[8] += (controlKey[1] & 0xF) << 4;

    SavedConfig[9] = controlKey[2] & 0xF;
    SavedConfig[9] += (controlKey[3] & 0xF) << 4;

    SavedConfig[10] = controlKey[4] & 0xF;
    SavedConfig[10] += (controlKey[5] & 0xF) << 4;

    SavedConfig[11] = controlKey[6] & 0xF;
    SavedConfig[11] += (controlKey[7] & 0xF) << 4;

    SavedConfig[12] = controlKey[8] & 0xF;
    SavedConfig[12] += (controlKey[9] & 0xF) << 4;

    SavedConfig[13] = controlKey[10] & 0xF;
    SavedConfig[13] += (controlKey[11] & 0xF) << 4;

    SavedConfig[14] = controlKey[12] & 0xF;
    SavedConfig[14] += (TextureFilter & 0x3) << 4; //0-2
    SavedConfig[14] += (Autorun & 0x3) << 6; //0-2
    
    SavedConfig[15] = (MotionBob/0x24925) & 0x7; //0-7 - 3 bits
	SavedConfig[15] += (FlashLevel & 0x7) << 3; //0-7 - 3 bits
    SavedConfig[15] += (ColoredHUD & 0x1) << 6; // 0-1 - 1bit - 1 bit left here
}

void M_DecodeConfig()
{
    unsigned char i;
    int controlKey[13];
	
    PlayDeadzone = (SavedConfig[3] & 0x7)<<1;
    ConfgNumb = (SavedConfig[3] >> 3) & 0x7;
    BloodStyle = (SavedConfig[3] >> 6) & 0x3;

    MusVolume = SavedConfig[4] & 0x7F;
    FeaturesUnlocked = (SavedConfig[4] >> 7) & 0x1;

    SfxVolume = SavedConfig[5] & 0x7F;
    enable_messages = (SavedConfig[5] >> 7) & 0x1;

    brightness = SavedConfig[6] & 0x7F;
    enable_statusbar = (SavedConfig[6] >> 7) & 0x1;

    M_SENSITIVITY = SavedConfig[7] & 0x7F;
	ShowStats = (SavedConfig[7] >> 7) & 0x1;

    controlKey[0] = SavedConfig[8] & 0xF;
    controlKey[1] = (SavedConfig[8] >> 4) & 0xF;
    
    controlKey[2] = SavedConfig[9] & 0xF;
    controlKey[3] = (SavedConfig[9] >> 4) & 0xF;

    controlKey[4] = SavedConfig[10] & 0xF;
    controlKey[5] = (SavedConfig[10] >> 4) & 0xF;

    controlKey[6] = SavedConfig[11] & 0xF;
    controlKey[7] = (SavedConfig[11] >> 4) & 0xF;

    controlKey[8] = SavedConfig[12] & 0xF;
    controlKey[9] = (SavedConfig[12] >> 4) & 0xF;

    controlKey[10] = SavedConfig[13] & 0xF;
    controlKey[11] = (SavedConfig[13] >> 4) & 0xF;

    controlKey[12] = SavedConfig[14] & 0xF;

    for (i = 0; i < 13; i++)
    {
        if (controlKey[i] == 0)
        {
            ActualConfiguration[i] = PAD_A;
        }
        else if (controlKey[i] == 1)
        {
            ActualConfiguration[i] = PAD_B;
        }
        else if (controlKey[i] == 2)
        {
            ActualConfiguration[i] = PAD_DOWN;
        }
        else if (controlKey[i] == 3)
        {
            ActualConfiguration[i] = PAD_DOWN_C;
        }
        else if (controlKey[i] == 4)
        {
            ActualConfiguration[i] = PAD_L_TRIG;
        }
        else if (controlKey[i] == 5)
        {
            ActualConfiguration[i] = PAD_LEFT;
        }
        else if (controlKey[i] == 6)
        {
            ActualConfiguration[i] = PAD_LEFT_C;
        }
        else if (controlKey[i] == 7)
        {
            ActualConfiguration[i] = PAD_R_TRIG;
        }
        else if (controlKey[i] == 8)
        {
            ActualConfiguration[i] = PAD_RIGHT;
        }
        else if (controlKey[i] == 9)
        {
            ActualConfiguration[i] = PAD_RIGHT_C;
        }
        else if (controlKey[i] == 10)
        {
            ActualConfiguration[i] = PAD_START;
        }
        else if (controlKey[i] == 11)
        {
            ActualConfiguration[i] = PAD_UP;
        }
        else if (controlKey[i] == 12)
        {
            ActualConfiguration[i] = PAD_UP_C;
        }
        else if (controlKey[i] == 13)
        {
            ActualConfiguration[i] = PAD_Z_TRIG;
        }
    }

    TextureFilter = (SavedConfig[14] >> 4) & 0x3;
    Autorun = (SavedConfig[14] >> 6) & 0x3;
	
    MotionBob = (SavedConfig[15] & 0x7)*0x24925; //0-7
    FlashLevel = (SavedConfig[15] >> 3) & 0x7; //0-7
    ColoredHUD = (SavedConfig[15] >> 6) & 0x1; //0-1

	// Set audio volumes
    wess_master_mus_vol_set(MusVolume);
	wess_master_sfx_vol_set(SfxVolume);
	
	P_RefreshBrightness(); // [Immorpher] - refresh brightness for new video options
}

int M_RunTitle(void) // 80007630
{
    int exit;
    boolean hasPak = false;
    boolean hasSave = false;

    hasPak = I_CheckControllerPak() == 0;
    hasSave = I_ReadPakFile() == 0;
    DrawerStatus = 0;
    startskill = sk_easy;
    startmap = 1;
    MenuIdx = 0;
    MenuItem =  hasPak && hasSave ? Menu_Title : Menu_TitleNoSave;
    MenuCall = M_MenuTitleDrawer;
    text_alpha = 0;
    itemlines = 3;
    cursorpos = 0;
    last_ticon = 0;

    S_StartMusic(mus_title);

    exit = MiniLoop(M_FadeInStart, M_MenuClearCall, M_MenuTicker, M_MenuGameDrawer);
    I_WIPE_FadeOutScreen();
    S_StopMusic();

    if (exit == ga_timeout)
        return ga_timeout;

    G_InitNew(startskill, startmap, ga_nothing);
    G_RunGame();

    return 0;
}

int M_ControllerPak(void) // 80007724
{
    int exit;
    int ret;
    boolean PakBad;

    PakBad = false;

    while(1)
    {
        ret = I_CheckControllerPak();

        if ((ret != PFS_ERR_NOPACK) && (ret != PFS_ERR_ID_FATAL))
            PakBad = true;

        if(ret == 0)
        {
            ret = I_ReadPakFile();

            // Free Pak_Data
            if (Pak_Data)
            {
                Z_Free(Pak_Data);
                Pak_Data = NULL;
            }

            if(ret == 0)
            {
                exit = ga_nothing;
                break;
            }

            // Create Memory Pak Note
            MenuItem = Menu_CreateNote;
            itemlines = 3;
            MenuCall = M_MenuTitleDrawer;
            cursorpos = 0;

            MiniLoop(M_FadeInStart, NULL, M_MenuTicker, M_MenuGameDrawer);
            M_FadeOutStart(8);

            if (cursorpos != 0)
            {
                exit = ga_exit;
                break;
            }

            // Check Memory and Files Used on Memory Pak
            if ((Pak_Memory > 0) && (FilesUsed != 16))
            {
                if (I_CreatePakFile() != 0)
                    goto ControllerPakBad;

                exit = ga_nothing;
                break;
            }

            // Show Memory Pak Full
            MenuItem = Menu_ControllerPakFull;
            itemlines = 3;
            MenuCall = M_MenuTitleDrawer;
            cursorpos = 0;

            MiniLoop(M_FadeInStart, NULL, M_MenuTicker, M_MenuGameDrawer);
            M_FadeOutStart(8);

            if (cursorpos != 1)
            {
                exit = ga_exit;
                break;
            }
        }
        else
        {
            if (PakBad == false)
            {
                exit = ga_exit;
                break;
            }

            // Show Memory Pak Bad
        ControllerPakBad:
            MenuItem = Menu_ControllerPakBad;
            itemlines = 2;
            MenuCall = M_MenuTitleDrawer;
            cursorpos = 0;

            MiniLoop(M_FadeInStart, NULL, M_MenuTicker, M_MenuGameDrawer);
            M_FadeOutStart(8);

            if (cursorpos != 0)
            {
                exit = ga_exit;
                break;
            }
        }
    }

    return exit;
}

int M_ButtonResponder(int buttons) // 80007960
{
    int sensitivity;
    int NewButtons;

    /* Copy Default Buttons */
    NewButtons = (buttons);

    /* Analyze Analog Stick (up / down) */
    sensitivity = (int)((buttons) << 24) >> 24;

    if (sensitivity <= -MenuDeadzone)
        NewButtons |= PAD_DOWN;
    else if (sensitivity >= MenuDeadzone)
        NewButtons |= PAD_UP;

    /* Analyze Analog Stick (left / right) */
    sensitivity = (int)(((buttons & 0xff00) >> 8) << 24) >> 24;

    if (sensitivity <= -MenuDeadzone)
        NewButtons |= PAD_LEFT;
    else if (sensitivity >= MenuDeadzone)
        NewButtons |= PAD_RIGHT;

    return NewButtons & 0xffff0000;
}

void M_AlphaInStart(void) // 800079E0
{
    text_alpha = 0;
    text_alpha_change_value = 20;
}

void M_AlphaOutStart(void) // 800079F8
{
    text_alpha = 255;
    text_alpha_change_value = -20;
}

int M_AlphaInOutTicker(void) // 80007A14
{
    if ((gamevbls < gametic) && ((gametic & 3U) == 0)) {
        MenuAnimationTic = (MenuAnimationTic + 1) & 7;
    }

    text_alpha += text_alpha_change_value;
    if (text_alpha_change_value < 0)
    {
        if (text_alpha < 0)
        {
            text_alpha = 0;
            return 8;
        }
    }
    else
    {
        if ((text_alpha_change_value > 0) && (text_alpha >= 256))
        {
            text_alpha = 255;
            return 8;
        }
    }

    return 0;
}

void M_FadeInStart(void) // 80007AB4
{
    MiniLoop(M_AlphaInStart, NULL, M_AlphaInOutTicker, M_MenuGameDrawer);
}

void M_FadeOutStart(int exitmode) // 80007AEC
{
    if (exitmode == 8)
        MiniLoop(M_AlphaOutStart, NULL, M_AlphaInOutTicker, M_MenuGameDrawer);
}

void M_SaveMenuData(void) // 80007B2C
{
    menudata_t *mdat;

    // Save Actual Menu Page
    mdat = &MenuData[MenuIdx];
    MenuIdx += 1;

    mdat->menu_item  = MenuItem;
    mdat->item_lines = itemlines;
    mdat->menu_call  = MenuCall;
    mdat->cursor_pos = cursorpos;

    // Start Menu Fade Out
    MiniLoop(M_AlphaOutStart, NULL, M_AlphaInOutTicker, M_MenuGameDrawer);
}

void M_RestoreMenuData(boolean alpha_in) // 80007BB8
{
    menudata_t *mdat;

    // Restore Previous Save Menu Page
    MenuIdx -= 1;
    mdat = &MenuData[MenuIdx];

    MenuItem  = mdat->menu_item;
    itemlines = mdat->item_lines;
    MenuCall  = mdat->menu_call;
    cursorpos = mdat->cursor_pos;

    // Start Menu Fade In
    if (alpha_in)
        MiniLoop(M_AlphaInStart, NULL, M_AlphaInOutTicker, M_MenuGameDrawer);
}

void M_MenuGameDrawer(void) // 80007C48
{
    if (DrawerStatus == 1) {
        P_Drawer();
    }
    else if (DrawerStatus == 2) {
        F_DrawerIntermission();
    }
    else if (DrawerStatus == 3) {
        F_Drawer();
    }
    else
    {
        I_ClearFrame();

        gDPPipeSync(GFX1++);
        gDPSetCycleType(GFX1++, G_CYC_FILL);
        gDPSetRenderMode(GFX1++,G_RM_NOOP,G_RM_NOOP2);
        gDPSetColorImage(GFX1++, G_IM_FMT_RGBA, G_IM_SIZ_32b, SCREEN_WD, OS_K0_TO_PHYSICAL(cfb[vid_side]));
        gDPSetFillColor(GFX1++, GPACK_RGBA5551(0,0,0,1) << 16 | GPACK_RGBA5551(0,0,0,1));
        gDPFillRectangle(GFX1++, 0, 0, SCREEN_WD-1, SCREEN_HT-1);
		
		M_DrawBackground(0, 0, 255, "TITLEBG"); // draw star background
		
        if ((MenuItem != Menu_Title && MenuItem != Menu_TitleNoSave) || MenuCall != M_MenuTitleDrawer) {
            M_DrawOverlay(0, 0, 320, 240, text_alpha>>2);
        }
		else {
			M_DrawBackground(40, 15, (7*text_alpha)>>3, "TITLE"); // draw title
		}

        MenuCall();
        I_DrawFrame();
    }
}

extern mobj_t mobjhead;
extern mapthing_t *spawnlist;   // 800A5D74
extern int spawncount;          // 800A5D78
extern int gobalcheats; // [GEC]

int M_MenuTicker(void) // 80007E0C
{
    unsigned int buttons, oldbuttons;
    int exit;
    int truebuttons;
    int ret;
    unsigned char i;
    mobj_t *m;

    /* animate skull */
    if ((gamevbls < gametic) && ((gametic & 3U) == 0))
        MenuAnimationTic = (MenuAnimationTic + 1) & 7;

    buttons = M_ButtonResponder(ticbuttons[0]);
    oldbuttons = oldticbuttons[0] & 0xffff0000;

    /* exit menu if button press */
    if (buttons != 0)
        last_ticon = ticon;

    /* exit menu if time out */
    if ((MenuItem == Menu_Title || MenuItem == Menu_TitleNoSave) && ((ticon - last_ticon) >= 900)) // 30 * TICRATE
    {
        exit = ga_timeout;
    }
    else
    {
        /* check for movement */
        if (!(buttons & (PAD_Z_TRIG|ALL_JPAD)))
            m_vframe1 = 0;
        else
        {
            m_vframe1 = m_vframe1 - vblsinframe[0];
            if (m_vframe1 <= 0)
            {
                m_vframe1 = 0xf; // TICRATE / 2

                if (buttons & PAD_DOWN)
                {
                    cursorpos += 1;

                    if (cursorpos >= itemlines)
                        cursorpos = 0;

                    S_StartSound(NULL, sfx_switch1);
                }
                else if (buttons & PAD_UP)
                {
                    cursorpos -= 1;

                    if (cursorpos < 0)
                        cursorpos = itemlines-1;

                    S_StartSound(NULL, sfx_switch1);
                }
            }
        }

        if ((buttons & PAD_START) && !(oldticbuttons[0] & PAD_START))
        {
            if ((MenuItem == Menu_Title) ||
                (MenuItem == Menu_TitleNoSave) ||
                (MenuItem == Menu_ControllerPakBad) ||
                (MenuItem == Menu_CreateNote) ||
                (MenuItem == Menu_ControllerPakFull))
            {
                return ga_nothing;
            }
            else
            {
                if (MenuIdx != 0)
                    S_StartSound(NULL, sfx_pistol);

                return ga_exit;
            }
        }
        else
        {
                truebuttons = (0 < (buttons ^ oldbuttons));

                if (truebuttons)
                    truebuttons = (0 < (buttons & ALL_BUTTONS));

                switch(MenuItem[cursorpos].casepos)
                {

                case 0: // Control Pad
                    if (truebuttons)
                    {
                        S_StartSound(NULL, sfx_pistol);
                        M_SaveMenuData();

                        MenuCall = M_ControlPadDrawer;
                        cursorpos = 0;
                        linepos = 0;

                        MiniLoop(M_FadeInStart,M_FadeOutStart,M_ControlPadTicker,M_MenuGameDrawer);
                        M_RestoreMenuData(true);
                        return ga_nothing;
                    }
                    break;

                case 1: // Volume
                    if (truebuttons)
                    {
                        S_StartSound(NULL, sfx_pistol);
                        M_SaveMenuData();

                        MenuItem = Menu_Volume;
                        itemlines = 4;
                        MenuCall = M_VolumeDrawer;
                        cursorpos = 0;

                        MiniLoop(M_FadeInStart,M_FadeOutStart,M_MenuTicker,M_MenuGameDrawer);
                        M_RestoreMenuData(true);
                        return ga_nothing;
                    }
                    break;

                case 2: // Display
                    if (truebuttons)
                    {
                        S_StartSound(NULL, sfx_pistol);
                        M_SaveMenuData();

                        MenuItem = Menu_Display;
                        itemlines = 7;
                        MenuCall = M_DisplayDrawer;
                        cursorpos = 0;

                        MiniLoop(M_FadeInStart,M_FadeOutStart,M_MenuTicker,M_MenuGameDrawer);
                        M_RestoreMenuData(true);
                        return ga_nothing;
                    }
                    break;

                case 3: // Password
                    if (truebuttons)
                    {
                        S_StartSound(NULL, sfx_pistol);
                        M_SaveMenuData();

                        MenuCall = M_PasswordDrawer;
                        exit = MiniLoop(M_PasswordStart,M_PasswordStop,M_PasswordTicker,M_MenuGameDrawer);

                        if (exit == ga_exit)
                        {
                            M_RestoreMenuData(true);
                            return ga_nothing;
                        }

                        return exit;
                    }
                    break;

                case 4: // Main Menu
                    if (truebuttons)
                    {
                        S_StartSound(NULL, sfx_pistol);
                        M_SaveMenuData();

                        MenuItem = Menu_Quit;
                        itemlines = 2;
                        MenuCall = M_MenuTitleDrawer;
                        cursorpos = 1;

                        exit = MiniLoop(M_FadeInStart,M_FadeOutStart,M_MenuTicker,M_MenuGameDrawer);
                        M_RestoreMenuData((exit == ga_exit));
                        if (exit == ga_exit) {
                            return ga_nothing;
                        }

                        return ga_exitdemo;//ga_exitdemo;
                    }
                    break;

                case 5: // [Immorpher] Updated restart level so player can change difficulty at will
                    if (truebuttons)
                    {
						
                        S_StartSound(NULL, sfx_pistol);
                        M_SaveMenuData();

                        MenuItem = Menu_Skill;
                        itemlines = 6;
                        MenuCall = M_MenuTitleDrawer;
                        cursorpos = gameskill;  // Set default to current difficulty

                        exit = MiniLoop(M_FadeInStart, M_FadeOutStart, M_MenuTicker, M_MenuGameDrawer);

                        if (exit == ga_exit && cursorpos == 5) { // [Immorpher] 5th to exit menu
							M_RestoreMenuData((exit == ga_exit));
                            return ga_nothing;
						}
						
                        gameskill = cursorpos;
						
						startmap = gamemap;
						startskill = gameskill;
						
						return ga_warped;
                    }
                    break;

                case 6: // Return
                    if (truebuttons)
                    {
                        S_StartSound(NULL, sfx_pistol);
                        return ga_exit;
                    }
                    break;

                case 7: // Music Volume
                    if (buttons & ALL_FORWARD)
                    {
                        if (MusVolume < 127)
                        {
							MusVolume += 1;
                            wess_master_mus_vol_set(MusVolume);
                            S_StartSound(NULL, sfx_secmove);
                            return ga_nothing;
                        }
                    }
                    else if (buttons & ALL_BACK)
                    {
                        if (MusVolume > 0)
                        {
							MusVolume -= 1;
                            wess_master_mus_vol_set(MusVolume);
							S_StartSound(NULL, sfx_secmove);
							return ga_nothing;
                        }
                    }
                    break;

                case 8: // Sound Volume
                    if (buttons & ALL_FORWARD)
                    {
                        if (SfxVolume < 127)
                        {
							SfxVolume += 1;
                            wess_master_sfx_vol_set(SfxVolume);
							S_StartSound(NULL, sfx_secmove);
							return ga_nothing;
                        }
                    }
                    else if (buttons & ALL_BACK)
                    {
                        if (SfxVolume > 0)
                        {
							SfxVolume -= 1;
                            wess_master_sfx_vol_set(SfxVolume);
                            S_StartSound(NULL, sfx_secmove);
                            return ga_nothing;
                        }
                    }
                    break;

                case 9: // Brightness
                    if (buttons & ALL_FORWARD)
                    {
                        if (brightness < 127)
                        {
							brightness += 1;
                            P_RefreshBrightness();
                            S_StartSound(NULL, sfx_secmove);
                            return ga_nothing;
                        }
                    }
                    else if (buttons & ALL_BACK)
                    {
                        if (brightness > 0)
                        {
							brightness -= 1;
                            P_RefreshBrightness();
                            S_StartSound(NULL, sfx_secmove);
                            return ga_nothing;
                        }
                    }
                    break;

                case 11: // Options
                    if (truebuttons)
                    {
                        S_StartSound(NULL, sfx_pistol);
                        M_SaveMenuData();

                        MenuItem = Menu_Options;
                        itemlines = 8;
                        MenuCall = M_MenuTitleDrawer;
                        cursorpos = 0;

                        exit = MiniLoop(M_FadeInStart, M_FadeOutStart, M_MenuTicker, M_MenuGameDrawer);
                        M_RestoreMenuData((exit == ga_exit));

                        if (exit == ga_exit)
                            return ga_nothing;

                        return exit;
                    }
                    break;

                case 12: // Default Audio
                    if (truebuttons)
                    {
                        S_StartSound(NULL, sfx_switch2);

                        SfxVolume = 100;
                        MusVolume = 100;

                        wess_master_mus_vol_set(MusVolume);
                        wess_master_sfx_vol_set(SfxVolume);

                        return ga_nothing;
                    }
                    break;

                case 13: // Default Display
                    if (truebuttons)
                    {
                        S_StartSound(NULL, sfx_switch2);

                        enable_messages = true;
                        enable_statusbar = true;

                        BloodStyle = 3;
                        ColoredHUD = 1;
						ShowStats = 1;
						
                        return ga_nothing;
                    }
                    break;

                case 14: // New Game
                    if (truebuttons)
                    {

                        S_StartSound(NULL, sfx_pistol);
                        M_SaveMenuData();

                        // Check ControllerPak
                        EnableMemPak = (M_ControllerPak() == 0);

                        MenuItem = Menu_Episode;
                        itemlines = 5;
                        MenuCall = M_MenuTitleDrawer;
                        cursorpos = 0;

                        exit = MiniLoop(M_FadeInStart, M_MenuClearCall, M_MenuTicker, M_MenuGameDrawer);
                        M_RestoreMenuData((exit == ga_exit));
                        
                        if (exit == ga_exit)
                            return ga_nothing;

                        return exit;
                    }
                    break;

                case 51: // DOOM 64
                    if (truebuttons)
                    {
                        startmap = MenuItem[cursorpos].casepos == 52 ? 34 : 1;
                        
                        S_StartSound(NULL, sfx_pistol);
                        M_SaveMenuData();

                        MenuItem = Menu_Skill;
                        itemlines = 6;
                        MenuCall = M_MenuTitleDrawer;
                        cursorpos = 2;

                        exit = MiniLoop(M_FadeInStart, M_MenuClearCall, M_MenuTicker, M_MenuGameDrawer);
                        M_RestoreMenuData((exit == ga_exit));
                        
                        if (exit == ga_exit)
                            return ga_nothing;
						
						nextmap = 1; // [Immorpher] For running introduction for Doom 64
						runintroduction = true; // [Immorpher] turn introduction on

                        return exit;
                    }
                    break;
                case 52: // The Lost Levels
                    if (truebuttons)
                    {
                        startmap = MenuItem[cursorpos].casepos == 52 ? 34 : 1;
                        
                        S_StartSound(NULL, sfx_pistol);
                        M_SaveMenuData();

                        MenuItem = Menu_Skill;
                        itemlines = 6;
                        MenuCall = M_MenuTitleDrawer;
                        cursorpos = 2;

                        exit = MiniLoop(M_FadeInStart, M_MenuClearCall, M_MenuTicker, M_MenuGameDrawer);
                        M_RestoreMenuData((exit == ga_exit));
                        
                        if (exit == ga_exit)
                            return ga_nothing;
						
						nextmap = LOSTLEVEL; // [Immorpher] For running introduction for Lost Levels
						runintroduction = true; // [Immorpher] turn introduction on
						
                        return exit;
                    }
                    break;

                case 15: // Be Gentle!
                case 16: // Bring it on!
                case 17: // I own Doom!
                case 18: // Watch me die!
				case 19: // Nightmare!
                
                    if (truebuttons)
                    {
                        startskill = MenuItem[cursorpos].casepos - 15;
                        S_StartSound(NULL, sfx_pistol);
                        return ga_restart;
                    }
                    break;

                case 20: // Yes
                case 46: // Try again
                case 47: // Create game note
                    if (truebuttons)
                    {
                        S_StartSound(NULL, sfx_pistol);
                        return ga_exitdemo; //ga_exitdemo;
                    }
                    break;

                case 21: // No
                case 45: // Do not use Pak
                    if (truebuttons)
                    {
                        S_StartSound(NULL, sfx_pistol);
                        return ga_exit;
                    }
                    break;

                case 22: // Features
                    if (truebuttons)
                    {
                        S_StartSound(NULL, sfx_pistol);
                        M_SaveMenuData();

                        players[0].cheats &= 0xffff1fff;

                        MenuItem = Menu_Features;
                        itemlines = MAXFEATURES;
                        MenuCall = M_FeaturesDrawer;
                        cursorpos = 0;
                        m_actualmap = gamemap;

                        exit = MiniLoop(M_FadeInStart,M_FadeOutStart,M_MenuTicker,M_MenuGameDrawer);
                        M_RestoreMenuData((exit == 8));

                        if (exit == 8)
                            return ga_nothing;

                        return exit;
                    }
                    break;

                case 23: // WARP TO LEVEL
                    if (buttons ^ oldbuttons)
                    {
                        if (buttons & PAD_LEFT)
                        {
							if (m_actualmap > 0)
							{
								m_actualmap -= 1;
								S_StartSound(NULL, sfx_switch2);
							}
							return ga_nothing;
                        }
                        else if (buttons & PAD_RIGHT)
                        {
							m_actualmap += 1;
							if (m_actualmap > LASTLEVEL)
							{
								m_actualmap = LASTLEVEL;
							}
							else
							{
								S_StartSound(NULL, sfx_switch2);
							}
							return ga_nothing;
                        }
                        else if (buttons & ALL_CBUTTONS)
                        {
                            gamemap = m_actualmap;
                            startmap = m_actualmap;
                            return ga_warped;
                        }
                    } else if (m_actualmap > LASTLEVEL) { // [Immorpher] If at bonus pak reset to hub
						m_actualmap = 0;
					}
                    break;

                case 24: // INVULNERABLE
                    if (((gamemap != 32) & truebuttons))
                    {
                        players[0].cheats ^= CF_GODMODE;
                        S_StartSound(NULL, sfx_switch2);
                        return ga_nothing;
                    }
                    break;

                case 25: // HEALTH BOOST
                    if (truebuttons)
                    {
                        players[0].cheats |= CF_HEALTH;
                        players[0].health = 100;
                        players[0].mo->health = 100;
                        S_StartSound(NULL, sfx_switch2);
                        return ga_nothing;
                    }
                    break;

                case 26: // SECURITY KEYS
                    /* Not available in the release code */
                    /*
                    Reconstructed code based on Psx Doom
                    */
                    if (truebuttons)
                    {
                        players[0].cheats |= CF_ALLKEYS;

                        for (m = mobjhead.next; m != &mobjhead; m = m->next)
                        {
                            switch (m->type)
                            {
                            case MT_ITEM_BLUECARDKEY:
                                players[0].cards[it_bluecard] = true;
                                break;
                            case MT_ITEM_REDCARDKEY:
                                players[0].cards[it_redcard] = true;
                                break;
                            case MT_ITEM_YELLOWCARDKEY:
                                players[0].cards[it_yellowcard] = true;
                                break;
                            case MT_ITEM_YELLOWSKULLKEY:
                                players[0].cards[it_yellowskull] = true;
                                break;
                            case MT_ITEM_REDSKULLKEY:
                                players[0].cards[it_redskull] = true;
                                break;
                            case MT_ITEM_BLUESKULLKEY:
                                players[0].cards[it_blueskull] = true;
                                break;
                            default:
                                break;
                            }
                        }

                        for (i = 0; i < spawncount; i++)
                        {
                            switch (spawnlist[i].type)
                            {
                            case 5:
                                players[0].cards[it_bluecard] = true;
                                break;
                            case 13:
                                players[0].cards[it_redcard] = true;
                                break;
                            case 6:
                                players[0].cards[it_yellowcard] = true;
                                break;
                            case 39:
                                players[0].cards[it_yellowskull] = true;
                                break;
                            case 38:
                                players[0].cards[it_redskull] = true;
                                break;
                            case 40:
                                players[0].cards[it_blueskull] = true;
                                break;
                            default:
                                break;
                            }
                        }

                        S_StartSound(NULL, sfx_switch2);
                        return ga_nothing;
                    }
                    break;

                case 27: // WEAPONS
                    if (truebuttons)
                    {
                        players[0].cheats |= CF_WEAPONS;

                        for(i = 0; i < NUMWEAPONS; i++) {
                            players[0].weaponowned[i] = true;
                        }

                        for(i = 0; i < NUMAMMO; i++) {
                            players[0].ammo[i] = players[0].maxammo[i];
                        }

                        S_StartSound(NULL, sfx_switch2);
                        return ga_nothing;
                    }
                    break;

                case 28: // Exit
                    /* nothing special */
                    break;

                case 29: // DEBUG
                    /* Not available in the release code */
                    if (truebuttons)
                    {
                        players[0].cheats ^= CF_DEBUG;
                        S_StartSound(NULL, sfx_switch2);
                        return ga_nothing;
                    }
                    break;

                case 30: // TEXTURE TEST
                    /* Not available in the release code */
                    if (truebuttons)
                    {
                        players[0].cheats ^= CF_TEX_TEST;
                        S_StartSound(NULL, sfx_switch2);
                        return ga_nothing;
                    }
                    break;

                case 31: // WALL BLOCKING
                    /* Not available in the release code */
                    /*
                    In my opinion it must have been the NOCLIP cheat code
                    */
                    if (truebuttons)
                    {
                        players[0].cheats ^= CF_WALLBLOCKING;
                        players[0].mo->flags ^= MF_NOCLIP;
                        S_StartSound(NULL, sfx_switch2);
                        return ga_nothing;
                    }
                    break;

                case 32: // Center Display
                    if (truebuttons)
                    {
                        S_StartSound(NULL, sfx_pistol);
                        M_SaveMenuData();

                        MenuCall = M_CenterDisplayDrawer;

                        MiniLoop(M_FadeInStart,M_FadeOutStart,M_CenterDisplayTicker,M_MenuGameDrawer);
                        M_RestoreMenuData(true);

                        return ga_nothing;
                    }
                    break;

                case 33: // Messages
                    if (truebuttons)
                    {
                        S_StartSound(NULL, sfx_switch2);
                        enable_messages ^= true;
						return ga_nothing;
                    }
                    break;

                case 34: // Status Bar
                    if (truebuttons)
                    {
                        S_StartSound(NULL, sfx_switch2);
                        enable_statusbar ^= true;
						return ga_nothing;
                    }
                    break;

                case 56: // Blood Style
                    if ((buttons ^ oldbuttons) && (buttons & ALL_FORWARD))
                    {
						BloodStyle += 1;
						S_StartSound(NULL, sfx_switch2);
						
                        if (BloodStyle > 3)
						{
							BloodStyle = 0;
                        }
						
						return ga_nothing;
                    }
                    else if ((buttons ^ oldbuttons) && (buttons & ALL_BACK))
                    {
						S_StartSound(NULL, sfx_switch2);
						if (BloodStyle == 0)
						{
							BloodStyle = 3;
                        }
						else
						{
							BloodStyle -= 1;
						}
						return ga_nothing;
                    }
                    break;

                case 57: // Cross Color
                    if (truebuttons)
                    {
                        S_StartSound(NULL, sfx_switch2);
                        ColoredHUD ^= true; // Temporarily disabled will be replaced with palette swap
						return ga_nothing;
                    }
                    break;

                case 35: // LOCK MONSTERS
                    /* Not available in the release code */
                    /*
                    Reconstructed code based on Doom 64 Ex
                    */
                    if (truebuttons)
                    {
                        players[0].cheats ^= CF_LOCKMOSTERS;
                        S_StartSound(NULL, sfx_switch2);
                        return ga_nothing;
                    }
                    break;

                case 36: // SCREENSHOT
                    /* Not available in the release code */
                    if (truebuttons)
                    {
                        players[0].cheats ^= CF_SCREENSHOT;
                        S_StartSound(NULL, sfx_switch2);
                        return ga_nothing;
                    }
                    break;

                case 37: // MAP EVERYTHING
                    if (truebuttons)
                    {
                        players[0].cheats ^= CF_ALLMAP;
                        S_StartSound(NULL, sfx_switch2);
                        return ga_nothing;
                    }
                    break;

                case 38: // MACRO PEEK
                    /* Not available in the release code */
                    if (truebuttons)
                    {
                        players[0].cheats ^= CF_MACROPEEK;
                        S_StartSound(NULL, sfx_switch2);
                        return ga_nothing;
                    }
                    break;

                case 39: // MUSIC TEST
                    /* Not available in the release code */
                    /*
                    Reconstructed code in my interpretation
                    */
                    if (buttons ^ oldbuttons)
                    {
                        if (buttons & PAD_LEFT)
                        {
                            MusicID -= 1;
                            if (MusicID > 0)
                            {
                                S_StartSound(NULL, sfx_switch2);
                                return ga_nothing;
                            }
                            MusicID = 1;
                        }
                        else if (buttons & PAD_RIGHT)
                        {
                            MusicID += 1;
                            if (MusicID < 25)
                            {
                                S_StartSound(NULL, sfx_switch2);
                                return ga_nothing;
                            }
                            MusicID = 24;
                        }
                        else if (buttons & ALL_CBUTTONS)
                        {
                            S_StopMusic();
                            S_StartMusic(MusicID+(NUMSFX-1));
                            return ga_nothing;
                        }
                    }
                    break;

                case 41: // Control Stick
                    if (truebuttons)
                    {
                        S_StartSound(NULL, sfx_pistol);
                        M_SaveMenuData();

                        MenuItem = Menu_ControlStick;
                        itemlines = 4;
                        MenuCall = M_ControlStickDrawer;
                        cursorpos = 0;

                        MiniLoop(M_FadeInStart, M_FadeOutStart, M_MenuTicker, M_MenuGameDrawer);
                        M_RestoreMenuData(true);

                        return ga_nothing;
                    }
                    break;

                case 42: // Default Control Stick
                    if (truebuttons)
                    {
                        S_StartSound(NULL, sfx_switch2);
                        M_SENSITIVITY = 27;
						PlayDeadzone = 10;
                    }
                    break;

                case 43: // Sensitivity
                    if (buttons & ALL_FORWARD)
                    {
                        if (M_SENSITIVITY < 127)
						{
							M_SENSITIVITY += 1;
							S_StartSound(NULL, sfx_secmove);
							return ga_nothing;
                        }
                    }
                    else if (buttons & ALL_BACK)
                    {
                       if (M_SENSITIVITY > 0)
						{
							M_SENSITIVITY -= 1;
							S_StartSound(NULL, sfx_secmove);
							return ga_nothing;
                        }
                    }
                    break;

                case 44: // Manage Pak
                    if (truebuttons)
                    {
                        S_StartSound(NULL, sfx_pistol);
                        M_SaveMenuData();

                        MenuCall = M_ControllerPakDrawer;
                        linepos = 0;
                        cursorpos = 0;

                        exit = MiniLoop(M_FadeInStart, M_FadeOutStart, M_ScreenTicker, M_MenuGameDrawer);
                        M_RestoreMenuData((exit == 8));

                        if (exit == 8)
                            return ga_nothing;

                        return exit;
                    }
                    break;

                case 48: // COLORS [GEC] NEW CHEAT CODE
                    if (truebuttons)
                    {
                    players[0].cheats ^= CF_NOCOLORS;
                    gobalcheats ^= CF_NOCOLORS;
                    P_RefreshBrightness();
                    S_StartSound(NULL, sfx_switch2);
                    return ga_nothing;
                    }
                    break;

                case 49: // FULL BRIGHT [GEC] NEW CHEAT CODE
                    if (truebuttons)
                    {
                        players[0].cheats ^= CF_FULLBRIGHT;
                        gobalcheats ^= CF_FULLBRIGHT;
                        P_RefreshBrightness();
                        S_StartSound(NULL, sfx_switch2);
                        return ga_nothing;
                    }
                    break;

                case 50: // FILTER [GEC] NEW CHEAT CODE
                    if (truebuttons)
                    {
                        TextureFilter += TextureFilter < 2 ? 1 : -2;
                        S_StartSound(NULL, sfx_switch2);
                        return ga_nothing;
                    }
                    break;

                case 53: // ARTIFACTS
                    if (truebuttons)
                    {
                        players[0].artifacts |= 4;
                        players[0].artifacts |= 2;
                        players[0].artifacts |= 1;

                        S_StartSound(NULL, sfx_switch2);
                        return ga_nothing;
                    }
                    break;

                case 54: // SKILL
                    if (buttons ^ oldbuttons)
                    {
                        if (buttons & ALL_FORWARD)
                        {
                            if (gameskill > sk_baby || startskill > sk_baby)
                            {
                                startskill -= 1;
                                gameskill -= 1;
                                S_StartSound(NULL, sfx_switch2);
                                return ga_nothing;
                            }
                        }
                        else if (buttons & ALL_BACK)
                        {
                            if (gameskill < sk_nightmare || startskill < sk_nightmare)
                            {
                                startskill += 1;
                                gameskill += 1;
                                S_StartSound(NULL, sfx_switch2);
                                return ga_nothing;
                            }
                        }
                    }
                    break;

                case 55: // LOAD GAME
                    if (truebuttons)
                    {
                        S_StartSound(NULL, sfx_pistol);
                        M_SaveMenuData();

                        ret = I_CheckControllerPak();
                        exit = ga_exit;

                        if (ret == 0)
                        {
                            if (I_ReadPakFile() == 0)
                            {
                                EnableMemPak = 1;
                                MenuCall = M_LoadPakDrawer;
                                exit = MiniLoop(M_LoadPakStart,M_LoadPakStop,M_LoadPakTicker,M_MenuGameDrawer);
                            }
                            else
                                exit = ga_exit;
                        }

                        if (exit == ga_exit)
                        {
                            M_RestoreMenuData(true);
                            return ga_nothing;
                        }

                        if (EnableMemPak != 0)
                        {
                            return exit;
                        }

                        EnableMemPak = (M_ControllerPak() == 0);
                        return exit;
                    }
					break;
					
				case 58: // Video
					if (truebuttons)
					{
						S_StartSound(NULL, sfx_pistol);
						M_SaveMenuData();
                        MenuItem = Menu_Video;
						itemlines = 7;
						MenuCall = M_VideoDrawer;
						cursorpos = 0;

						MiniLoop(M_FadeInStart,M_FadeOutStart,M_MenuTicker,M_MenuGameDrawer);
						M_RestoreMenuData(true);
						return ga_nothing;
					}
					break;
									
				case 59: // Motion bob
                    if ((buttons ^ oldbuttons) && (buttons & ALL_FORWARD))
                    {
						S_StartSound(NULL, sfx_switch2);
                        if (MotionBob >= 0x100003)
							MotionBob = 0;
						else
							MotionBob += 0x24925;
						
						return ga_nothing;
                    }
                    else if ((buttons ^ oldbuttons) && (buttons & ALL_BACK))
                    {
						S_StartSound(NULL, sfx_switch2);
						if (MotionBob < 0x24925)
							MotionBob = 0x100003;
                        else
							MotionBob -= 0x24925;
						
						return ga_nothing;
                    }
                    break;

				case 60: // Show Stats
					if (truebuttons)
					{
						S_StartSound(NULL, sfx_switch2);
						ShowStats ^= true;
						
						return ga_nothing;
					}
					break;
					
                case 61: // Bonus Pak
                    if (truebuttons)
                    {
                        startmap = MenuItem[cursorpos].casepos == 61 ? 0 : 1;
                        
                        S_StartSound(NULL, sfx_pistol);
                        M_SaveMenuData();

                        MenuItem = Menu_Skill;
                        itemlines = 6;
                        MenuCall = M_MenuTitleDrawer;
                        cursorpos = 2;

                        exit = MiniLoop(M_FadeInStart, M_MenuClearCall, M_MenuTicker, M_MenuGameDrawer);
                        M_RestoreMenuData((exit == ga_exit));
                        
                        if (exit == ga_exit)
                            return ga_nothing;
						
						nextmap = 0; // [Immorpher] For running introduction for Bonus Pak
						runintroduction = true; // [Immorpher] turn introduction on

                        return exit;
                    }
                    break;
					
				case 62: // Beta 64
                    if (truebuttons)
                    {
                        startmap = MenuItem[cursorpos].casepos == 62 ? 41 : 1;
                        
                        S_StartSound(NULL, sfx_pistol);
                        M_SaveMenuData();

                        MenuItem = Menu_Skill;
                        itemlines = 6;
                        MenuCall = M_MenuTitleDrawer;
                        cursorpos = 2;

                        exit = MiniLoop(M_FadeInStart, M_MenuClearCall, M_MenuTicker, M_MenuGameDrawer);
                        M_RestoreMenuData((exit == ga_exit));
                        
                        if (exit == ga_exit)
                            return ga_nothing;

						nextmap = BETALEVEL; // [Immorpher] For running introduction for Lost Levels
						runintroduction = true; // [Immorpher] turn introduction on
						
                        return exit;
                    }
                    break;

				case 64: // Credits
                    if (truebuttons)
                    {
                        S_StartSound(NULL, sfx_pistol);
                        M_SaveMenuData();
                        exit = D_Credits();
						M_RestoreMenuData(true);
                        return ga_nothing;
                    }
                    break;
					
				case 65: // Deadzone
                    if ((buttons ^ oldbuttons) && (buttons & ALL_FORWARD))
                    {
						PlayDeadzone += 2;
						S_StartSound(NULL, sfx_switch2);
						
                        if (PlayDeadzone > 14)
						{
							PlayDeadzone = 0;
                        }
						
						return ga_nothing;
                    }
                    else if ((buttons ^ oldbuttons) && (buttons & ALL_BACK))
                    {
						PlayDeadzone -= 2;
						S_StartSound(NULL, sfx_switch2);
						if (PlayDeadzone < 0)
						{
							PlayDeadzone = 14;
                        }
						return ga_nothing;
                    }
                    break;
					
				case 66: // Flash Level - behind the scenes it works in opposite direction than typical
                    if ((buttons ^ oldbuttons) && (buttons & ALL_FORWARD))
                    {
						FlashLevel -= 1;
						S_StartSound(NULL, sfx_switch2);
                        if (FlashLevel < 0)
						{
							FlashLevel = 7;
                        }
						return ga_nothing;
                    }
                    else if ((buttons ^ oldbuttons) && (buttons & ALL_BACK))
                    {
						FlashLevel += 1;
						S_StartSound(NULL, sfx_switch2);
						if (FlashLevel > 7)
						{
							FlashLevel = 0;
                        }
						return ga_nothing;
                    }
                    break;		

				case 67: // Default Video
                    if (truebuttons)
                    {
                        S_StartSound(NULL, sfx_switch2);
                        Display_X = 0;
                        Display_Y = 0;
                        brightness = 60;
                        I_MoveDisplay(0,0);
                        P_RefreshBrightness();
                        TextureFilter = 0;
						MotionBob = 0x100003;
						FlashLevel = 0;
                        return ga_nothing;
                    }
                    break;
					
                }
            exit = ga_nothing;
        }
    }

    return exit;
}

void M_MenuClearCall(void) // 80008E6C
{
    MenuCall = NULL;
}

void M_MenuTitleDrawer(void) // 80008E7C
{
    const menuitem_t *item;
    unsigned char i;

    if (MenuItem == Menu_Game || MenuItem == Menu_GameNoSave)
    {
        ST_DrawString(-1, 20, "Pause", text_alpha | 0xff000000);
            ST_DrawString(-1, 200, "press \x8d to resume", text_alpha | 0xffffff00);
        }
        else if (MenuItem == Menu_Skill)
        {
            ST_DrawString(-1, 20, "Choose Your Skill...", text_alpha | 0xff000000);
        }
        else if (MenuItem == Menu_Episode)
        {
            ST_DrawString(-1, 20, "Choose Campaign", text_alpha | 0xff000000);
        }
        else if (MenuItem == Menu_Options)
        {
            ST_DrawString(-1, 20, "Options", text_alpha | 0xff000000);
        }
        else if (MenuItem == Menu_Quit)
        {
            ST_DrawString(-1, 20, "Quit Game?", text_alpha | 0xff000000);
        }
        else if (MenuItem == Menu_DeleteNote)
        {
            ST_DrawString(-1, 20, "Delete Game Note?", text_alpha | 0xff000000);
        }
        else if (MenuItem == Menu_ControllerPakBad)
        {
            ST_DrawString(-1, 20, "Memory Pak Bad", text_alpha | 0xff000000);
        }
        else if (MenuItem == Menu_ControllerPakFull)
        {
            ST_DrawString(-1, 20, "Memory Pak Full", text_alpha | 0xff000000);
        }
        else if (MenuItem == Menu_CreateNote)
        {
            ST_DrawString(-1, 20, "Create Game Note?", text_alpha | 0xff000000);
        }

        item = MenuItem;
        for(i = 0; i < itemlines; i++)
    {
        ST_DrawString(item->x, item->y, MenuText[item->casepos], text_alpha | 0xff000000);
        item++;
    }

    ST_DrawSymbol(MenuItem[0].x -37, MenuItem[cursorpos].y -9, MenuAnimationTic + 70, text_alpha | 0xffffff00);
}

void M_FeaturesDrawer(void) // 800091C0
{
    char *text, textbuff[32];
    const menuitem_t *item;
    unsigned char i;

    ST_DrawString(-1, 20, "Features", text_alpha | 0xff000000);
    item = MenuItem;

    for(i = 0; i < itemlines; i++)
    {
        if ((item->casepos == 23) && FUNLEVEL(m_actualmap))
        {
            /* Show "WARP TO FUN" text */
            ST_Message(item->x, item->y, MenuText[40], text_alpha | 0xffffff00);
        } else if ((item->casepos == 23) && FINLEVEL(m_actualmap))
        {
            /* Show "WARP TO FINAL" text */
            ST_Message(item->x, item->y, MenuText[63], text_alpha | 0xffffff00);
        }
        else
        {
            /* Show "WARP TO LEVEL" text */
            ST_Message(item->x, item->y, MenuText[item->casepos], text_alpha | 0xffffff00);
        }

        text = textbuff;
        switch(item->casepos)
        {
            case 23: /* WARP TO LEVEL */
                sprintf(textbuff, "%s", MapInfo[m_actualmap].name);
                break;
            case 24: /* INVULNERABLE */
                text = (!(players[0].cheats & CF_GODMODE)) ? "OFF": "ON";
                break;
            case 25: /* HEALTH BOOST */
                text = (!(players[0].cheats & CF_HEALTH)) ? "-" : "100%";
                break;
            case 26: /* SECURITY KEYS */
                text = (!(players[0].cheats & CF_ALLKEYS)) ? "-" : "100%";
                break;
            case 27: /* WEAPONS */
                text = (!(players[0].cheats & CF_WEAPONS)) ? "-" : "100%";
                break;
            case 28: /* Exit */
                break;
            case 29: /* DEBUG */
                text = (!(players[0].cheats & CF_DEBUG)) ? "OFF": "ON";
                break;
            case 30: /* TEXTURE TEST */
                text = (!(players[0].cheats & CF_TEX_TEST)) ? "OFF": "ON";
                break;
            case 31: /* WALL BLOCKING */
                text = (!(players[0].mo->flags & MF_NOCLIP)) ? "ON": "OFF";
                break;
            case 35: /* LOCK MONSTERS */
                text = (!(players[0].cheats & CF_LOCKMOSTERS)) ? "OFF": "ON";
                break;
            case 36: /* SCREENSHOT */
                text = (!(players[0].cheats & CF_SCREENSHOT)) ? "OFF": "ON";
                break;
            case 37: /* MAP EVERYTHING */
                text = (!(players[0].cheats & CF_ALLMAP)) ? "OFF": "ON";
                break;
            case 38: /* MACRO PEEK */
                text = ((players[0].cheats & CF_MACROPEEK)) ? "ON": "OFF";
                break;
            case 39: /* MUSIC TEST */
                sprintf(textbuff, "%d", MusicID);
                break;

            // [GEC] NEW CHEAT CODES
            case 48: /* COLORS */
                text = (!(players[0].cheats & CF_NOCOLORS)) ? "ON": "OFF";
                break;

            case 49: /* FULL BRIGHT */
                text = (!(players[0].cheats & CF_FULLBRIGHT)) ? "OFF": "ON";
                break;

            case 53: /* ARTIFACTS */
                text = (!(players[0].artifacts & 1 && players[0].artifacts & 2 && players[0].artifacts & 4)) ? "-" : "100%";
                break;

            case 54: /* SKILL */
                switch (gameskill)
                {
                    case 4:
                        text = M_TXT19;
                        break;
                    case 3:
                        text = M_TXT18;
                        break;
                    case 2:
                        text = M_TXT17;
                        break;
                    case 1:
                        text = M_TXT16;
                        break;
                    case 0:
                    default:
                        text = M_TXT15;
                        break;
                }
                break;

            default:
                text = "NOT IMPLEMENTED";
                break;
        }

        ST_Message(item->x + 130, item->y, text, text_alpha | 0xffffff00);
        item++;
    }

    ST_DrawSymbol(MenuItem->x -10, MenuItem[cursorpos].y -1, 78, text_alpha | 0xffffff00);
}

void M_VolumeDrawer(void) // 800095B4
{
    const menuitem_t *item;
    unsigned char i;

    ST_DrawString(-1, 20, "Volume", text_alpha | 0xff000000);
    item = Menu_Volume;

    for(i = 0; i < itemlines; i++)
    {
        ST_DrawString(item->x, item->y, MenuText[item->casepos], text_alpha | 0xff000000);
        item++;
    }

    ST_DrawSymbol(MenuItem->x - 37, MenuItem[cursorpos].y - 9, MenuAnimationTic + 70, text_alpha | 0xffffff00);

    ST_DrawSymbol(102, 80, 68, text_alpha | 0xffffff00);
    ST_DrawSymbol(((101*MusVolume)>>7) + 103, 80, 69, text_alpha | 0xffffff00);

    ST_DrawSymbol(102, 120, 68, text_alpha | 0xffffff00);
    ST_DrawSymbol(((101*SfxVolume)>>7) + 103, 120, 69, text_alpha | 0xffffff00);
}

void M_ControlStickDrawer(void) // 80009738
{
    const menuitem_t *item;
    unsigned char i, casepos;

    ST_DrawString(-1, 20, "Control Stick", text_alpha | 0xff000000);

    item = Menu_ControlStick;

    for(i = 0; i < itemlines; i++)
    {
		casepos = item->casepos;
		if (casepos == 65) { // Deadzone
            ST_DrawNumber(item->x + 110, item->y, PlayDeadzone>>1, 0, text_alpha | 0xff000000);
        }
		
        ST_DrawString(item->x, item->y, MenuText[item->casepos], text_alpha | 0xff000000);
        item++;
    }

    ST_DrawSymbol(MenuItem->x - 37, MenuItem[cursorpos].y - 9, MenuAnimationTic + 70, text_alpha | 0xffffff00);

    ST_DrawSymbol(102,90,68,text_alpha | 0xffffff00);
    ST_DrawSymbol(((101*M_SENSITIVITY)>>7) + 103, 90, 69, text_alpha | 0xffffff00);
}

void M_VideoDrawer(void) // [Immorpher] Video menu for additional options
{
    char *text;
    const menuitem_t *item;
    unsigned char i, casepos;

    ST_DrawString(-1, 20, "Video", text_alpha | 0xff000000);

    item = Menu_Video;

    for(i = 0; i < itemlines; i++)
    {
        casepos = item->casepos;

        if (casepos == 50) // Texture filtering
        {
            switch (TextureFilter)
            {
                case 2:
                    text = "Sky";
                    break;
                case 1:
                    text = "Off";
                    break;
                case 0:
                default:
                    text = "On";
                    break;
            }
        }
        else
        {
            text = NULL;
        }

        if (text)
            ST_DrawString(item->x + 130, item->y, text, text_alpha | 0xff000000);
		
		if (casepos == 59) { // draw motion bob number
			ST_DrawNumber(item->x + 140, item->y, MotionBob/0x24925, 0, text_alpha | 0xff000000);
		}
		
		if (casepos == 66) { // draw flash level number
			ST_DrawNumber(item->x + 140, item->y, 7 - FlashLevel, 0, text_alpha | 0xff000000);
		}

        ST_DrawString(item->x, item->y, MenuText[casepos], text_alpha | 0xff000000);

        item++;
    }

	// Brightness slider
    ST_DrawSymbol(102, 80, 68, text_alpha | 0xffffff00);
    ST_DrawSymbol(((101*brightness)>>7) + 103, 80, 69, text_alpha | 0xffffff00);

    ST_DrawSymbol(Menu_Video[0].x - 37, Menu_Video[cursorpos].y - 9, MenuAnimationTic + 70, text_alpha | 0xffffff00);
}

void M_DisplayDrawer(void) // 80009884
{
    char *text;
    const menuitem_t *item;
    unsigned char i, casepos;

    ST_DrawString(-1, 20, "Display", text_alpha | 0xff000000);

    item = Menu_Display;

    for(i = 0; i < 7; i++)
    {
        casepos = item->casepos;

        if (casepos == 33) // Messages:
        {
            text = enable_messages ? "On" : "Off";
        }
        else if (casepos == 34) // Status Bar:
        {
            text = enable_statusbar ? "On" : "Off";
        }
		else if (casepos == 60) // Show automap stats
        {
            text = ShowStats ? "On" : "Off";
        }
        else if (casepos == 56) // BloodStyle
        {
			switch (BloodStyle)
            {
                case 3:
                    text = "Combo";
                    break;
                case 2:
                    text = "Dust";
                    break;
                case 1:
                    text = "Green";
                    break;
                case 0:
                default:
                    text = "Red";
                    break;
            }
        }
        else if (casepos == 57) // Colored HUD
        {
            text = ColoredHUD ? "On" : "Off";
        }
        else
        {
            text = NULL;
        }

        if (text)
            ST_DrawString(item->x + 140, item->y, text, text_alpha | 0xff000000);

        ST_DrawString(item->x, item->y, MenuText[casepos], text_alpha | 0xff000000);

        item++;
    }

    ST_DrawSymbol(Menu_Display[0].x - 37, Menu_Display[cursorpos].y - 9, MenuAnimationTic + 70, text_alpha | 0xffffff00);
}

void M_DrawBackground(int x, int y, int color, char *name) // 80009A68
{
    int width, height;
    int yh, xh, t;
    int offset;
    byte *data;

    data = (byte *)W_CacheLumpName(name, PU_CACHE);

    gDPPipeSync(GFX1++);
    gDPSetCycleType(GFX1++, G_CYC_1CYCLE);

    gDPSetTextureLUT(GFX1++, G_TT_RGBA16);
    gDPSetTexturePersp(GFX1++, G_TP_NONE);

    gDPSetAlphaCompare(GFX1++, G_AC_THRESHOLD);

    gDPSetBlendColor(GFX1++, 0, 0, 0, 0);
    gDPSetCombineMode(GFX1++, G_CC_D64COMB03, G_CC_D64COMB03);

    if (color == 0xff)
    {
        gDPSetRenderMode(GFX1++, G_RM_TEX_EDGE, G_RM_TEX_EDGE2);
    }
    else
    {
        gDPSetRenderMode(GFX1++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    }

    gDPSetPrimColorD64(GFX1++, 0, 0, color);

    width = ((gfxN64_t*)data)->width;
    height = ((gfxN64_t*)data)->height;

    // Load Palette Data
    offset = (width * height);
    offset = (offset + 7) & ~7;
    gDPSetTextureImage(GFX1++, G_IM_FMT_RGBA, G_IM_SIZ_16b ,
                        1, data + offset + sizeof(gfxN64_t));

    gDPTileSync(GFX1++);
    gDPSetTile(GFX1++, G_IM_FMT_RGBA, G_IM_SIZ_4b, 0, 256, G_TX_LOADTILE, 0, 0, 0, 0, 0, 0, 0);

    gDPLoadSync(GFX1++);
    gDPLoadTLUTCmd(GFX1++, G_TX_LOADTILE, 255);

    gDPPipeSync(GFX1++);

    xh = (width + 7) & ~7;

    t = 0;
    while (height != 0)
    {
        if ((2048 / xh) < height)
            yh = (2048 / xh);
        else
            yh = height;

        // Load Image Data
        gDPSetTextureImage(GFX1++, G_IM_FMT_CI, G_IM_SIZ_8b ,
                        width, data + sizeof(gfxN64_t));

         // Clip Rectangle From Image
        gDPSetTile(GFX1++, G_IM_FMT_CI, G_IM_SIZ_8b,
                        (width + 7) / 8, 0, G_TX_LOADTILE, 0, 0, 0, 0, 0, 0, 0);

        gDPLoadSync(GFX1++);
        gDPLoadTile(GFX1++, G_TX_LOADTILE,
                    (0 << 2), (t << 2),
                    ((width - 1) << 2), (((t + yh) - 1) << 2));

        gDPPipeSync(GFX1++);
        gDPSetTile(GFX1++, G_IM_FMT_CI, G_IM_SIZ_8b,
                    (width + 7) / 8, 0, G_TX_RENDERTILE, 0, 0, 0, 0, 0, 0, 0);

        gDPSetTileSize(GFX1++, G_TX_RENDERTILE,
                       (0 << 2), (t << 2),
                       ((width - 1) << 2), (((t + yh) - 1) << 2));

        gSPTextureRectangle(GFX1++,
            (x << 2), (y << 2),
            ((width + x) << 2), ((yh + y) << 2),
            G_TX_RENDERTILE,
            (0 << 5), (t << 5),
            (1 << 10), (1 << 10));

        height -= yh;
        t += yh;
        y += yh;
    }
	
    gDPPipeSync(GFX1++);
    gDPSetCycleType(GFX1++, G_CYC_2CYCLE);
	
    globallump = -1;
}

void M_DrawOverlay(int x, int y, int w, int h, int color) // 80009F58
{
    I_CheckGFX();

    gDPPipeSync(GFX1++);

    gDPSetCycleType(GFX1++, G_CYC_1CYCLE);

    gDPSetTextureLUT(GFX1++, G_TT_RGBA16);
    gDPSetTexturePersp(GFX1++, G_TP_NONE);

    gDPSetAlphaCompare(GFX1++, G_AC_THRESHOLD);

    gDPSetBlendColor(GFX1++, 0, 0, 0, 0);
    gDPSetCombineMode(GFX1++, G_CC_D64COMB05, G_CC_D64COMB05);
    gDPSetRenderMode(GFX1++, G_RM_XLU_SURF, G_RM_XLU_SURF2);

    gDPSetPrimColorD64(GFX1++, 0, 0, color);

    gDPFillRectangle(GFX1++, x, y, w, h);
    globallump = -1;
}

int M_ScreenTicker(void) // 8000A0F8
{
    int exit;
    unsigned int buttons;
    unsigned int oldbuttons;
    OSPfsState *fState;

    if ((FilesUsed == -1) && (I_CheckControllerPak() == 0))
    {
        cursorpos = 0;
        linepos = 0;
    }

    if ((gamevbls < gametic) && ((gametic & 3U) == 0))
        MenuAnimationTic = (MenuAnimationTic + 1) & 7;

    buttons = M_ButtonResponder(ticbuttons[0]);
    oldbuttons = oldticbuttons[0] & 0xffff0000;

    if (!(buttons & ALL_JPAD))
    {
        m_vframe1 = 0;
    }
    else
    {
        m_vframe1 -= vblsinframe[0];

        if (m_vframe1 <= 0)
        {
            m_vframe1 = 0xf; // TICRATE/2

            if (buttons & PAD_DOWN)
            {
                cursorpos += 1;

                if (cursorpos < 16)
                    S_StartSound(NULL, sfx_switch1);
                else
                    cursorpos = 15;

                if ((linepos + 5) < cursorpos)
                    linepos += 1;
            }
            else if (buttons & PAD_UP)
            {
                cursorpos -= 1;

                if (cursorpos < 0)
                    cursorpos = 0;
                else
                    S_StartSound(NULL, sfx_switch1);

                if(cursorpos < linepos)
                    linepos -= 1;
            }
        }
    }

    if (!(buttons ^ oldbuttons) || !(buttons & PAD_START))
    {
        if (buttons ^ oldbuttons)
        {
            if(buttons == PAD_A)
            {
                fState = &FileState[cursorpos];

                if(fState->file_size != 0)
                {
                    S_StartSound(NULL, sfx_pistol);
                    M_SaveMenuData();

                    MenuItem = Menu_DeleteNote;
                    itemlines = 2;
                    MenuCall = M_MenuTitleDrawer;
                    cursorpos = 1;
                    MiniLoop(M_FadeInStart, NULL, M_MenuTicker, M_MenuGameDrawer);

                    M_FadeOutStart(8);
                    if (cursorpos == 0)
                    {
                        if (I_DeletePakFile(cursorpos) == 0)
                        {
                            fState->file_size = 0;
                        }
                        else
                        {
                            FilesUsed = -1;
                        }
                    }
                    M_RestoreMenuData(true);
                }
            }
        }
        exit = 0;
    }
    else
    {
        S_StartSound(NULL, sfx_pistol);
        exit = 8;
    }
    return exit;
}

void M_ControllerPakDrawer(void) // 8000A3E4
{
    byte idx;
    unsigned char i,j;
    OSPfsState *fState;
    char buffer [32];
    char *tmpbuf;

    ST_DrawString(-1, 20, "Memory Pak", text_alpha | 0xff000000);

    if (FilesUsed == -1)
    {
        if ((MenuAnimationTic & 2) != 0)
            ST_DrawString(-1, 114, "Memory Pak removed!", text_alpha | 0xff000000);

        ST_DrawString(-1, 210, "press \x8d to exit", text_alpha | 0xffffff00);
    }
    else
    {
        fState = &FileState[linepos];

        for(i = linepos; i < (linepos + 6); i++)
        {
            if (fState->file_size == 0)
            {
                D_memmove(buffer, "no save");
            }
            else
            {
                tmpbuf = buffer;

                for(j = 0; j < 16; j++)
                {
                    idx = (byte) fState->game_name[j];
                    if(idx == 0)
                        break;

                    tmpbuf[0] = Pak_Table[idx];
                    tmpbuf++;
                }

                idx = (byte) fState->ext_name[0];
                if (idx != 0)
                {
                    tmpbuf[0] = '.';
                    tmpbuf[1] = Pak_Table[idx];
                    tmpbuf += 2;
                }

                *tmpbuf = '\0';
            }

            ST_DrawString(60, (i - linepos) * 15 + 60, buffer, text_alpha | 0xff000000);

            fState++;
        }

        if (linepos != 0)
        {
            ST_DrawString(60, 45, "\x8F more...", text_alpha | 0xffffff00);
        }

        if ((linepos + 6) < 16)
        {
            ST_DrawString(60, 150, "\x8E more...", text_alpha | 0xffffff00);
        }

        sprintf(buffer, "pages used: %d   free: %d", FileState[cursorpos].file_size >> 8, Pak_Memory);

        ST_DrawString(-1, 170, buffer, text_alpha | 0xff000000);
        ST_DrawSymbol(23, (cursorpos - linepos) * 15 + 51, MenuAnimationTic + 70, text_alpha | 0xffffff00);

        ST_DrawString(-1, 200, "press \x8d to exit", text_alpha | 0xffffff00);
        ST_DrawString(-1, 215, "press \x8a to delete", text_alpha | 0xffffff00);
    }
}

void M_SavePakStart(void) // 8000A6E8
{
    unsigned char i;
    int ret;
    int size;

    cursorpos = 0;
    linepos = 0;
    last_ticon = 0;

    ret = I_CheckControllerPak();
    if (ret == 0)
    {
        if (I_ReadPakFile() == 0)
        {
            size = Pak_Size / 32;

            i = 0;
            if (size != 0)
            {
                do
                {
                    if (Pak_Data[i * 32] == 0)
                        break;

                    i++;
                } while (i != size);
            }

            if (i < size)
            {
                cursorpos = i;

                if (!(size < (i+6)))
                    linepos = i;
                else
                    linepos = (size-6);
            }
        }
    }
    else
    {
        FilesUsed = -1;
    }
}

void M_SavePakStop(void) // 8000A7B4
{
    S_StartSound(NULL, sfx_pistol);

    if (Pak_Data)
    {
        Z_Free(Pak_Data);
        Pak_Data = NULL;
    }
}

int M_SavePakTicker(void) // 8000A804
{
    unsigned int buttons;
    unsigned int oldbuttons;
    int size;

    if ((gamevbls < gametic) && ((gametic & 3U) == 0)) {
        MenuAnimationTic = (MenuAnimationTic + 1) & 7;
    }

    buttons = M_ButtonResponder(ticbuttons[0]);
    oldbuttons = oldticbuttons[0] & 0xffff0000;

    if ((buttons != oldbuttons) && (buttons & PAD_START)) {
        return ga_exit;
    }

    if (FilesUsed == -1)
    {
        if (I_CheckControllerPak()) {
            return ga_nothing;
        }

        if (I_ReadPakFile()) {
            FilesUsed = -1;
            return ga_nothing;
        }

        cursorpos = 0;
        linepos = 0;
    }

    if (!(buttons & ALL_JPAD)) {
        m_vframe1 = 0;
    }
    else
    {
        m_vframe1 -= vblsinframe[0];

        if (m_vframe1 <= 0)
        {
            m_vframe1 = 0xf; // TICRATE/2

            if (buttons & PAD_DOWN)
            {
                cursorpos += 1;

                size = (Pak_Size / 32) - 1;

                if (size < cursorpos)
                    cursorpos = size;
                else
                    S_StartSound(NULL, sfx_switch1);


                if ((linepos + 5) < cursorpos)
                    linepos += 1;
            }
            else if (buttons & PAD_UP)
            {
                cursorpos -= 1;

                if (cursorpos < 0)
                    cursorpos = 0;
                else
                    S_StartSound(NULL, sfx_switch1);

                if(cursorpos < linepos)
                    linepos -= 1;
            }
        }
    }

    if (last_ticon == 0)
    {
        if ((buttons != oldbuttons) && (buttons == PAD_A))
        {

            // save configuration
            M_EncodeConfig();
            D_memcpy(&Pak_Data[cursorpos * 32], &SavedConfig, 16);
			
            // save the next password data in text format
            D_memcpy(&Pak_Data[(cursorpos * 32) + 16], &Passwordbuff, 16);

            if (I_SavePakFile(File_Num, PFS_WRITE, Pak_Data, Pak_Size) == 0) {
                last_ticon = ticon;
            }
            else
            {
                FilesUsed = -1;
                if (Pak_Data)
                {
                    Z_Free(Pak_Data);
                    Pak_Data = NULL;
                }
            }
        }
    }
    else if ((ticon - last_ticon) >= 60) // 2 * TICRATE
    {
        return ga_exit;
    }

    return ga_nothing;
}

void M_SavePakDrawer(void) // 8000AB44
{
    unsigned char i;
    char buffer[36];
    byte savedata[16];
    int leveltxt, skilltxt;

    I_ClearFrame();

    gDPPipeSync(GFX1++);
    gDPSetCycleType(GFX1++, G_CYC_FILL);
    gDPSetRenderMode(GFX1++,G_RM_NOOP,G_RM_NOOP2);
    gDPSetColorImage(GFX1++, G_IM_FMT_RGBA, G_IM_SIZ_32b, SCREEN_WD, OS_K0_TO_PHYSICAL(cfb[vid_side]));
    // Fill borders with black
    gDPSetFillColor(GFX1++, GPACK_RGBA5551(0,0,0,0) << 16 | GPACK_RGBA5551(0,0,0,0)) ;
    gDPFillRectangle(GFX1++, 0, 0, SCREEN_WD-1, SCREEN_HT-1);

	M_DrawBackground(63, 25, 128, EpisodeGraphic());

    ST_DrawString(-1, 20, "Memory Pak", text_alpha | 0xff000000);

    if (FilesUsed == -1)
    {
        if (MenuAnimationTic & 2)
        {
            ST_DrawString(-1, 100, "Memory Pak removed!", 0xc00000ff);
            ST_DrawString(-1, 120, "Game cannot be saved.", 0xc00000ff);
        }

        ST_DrawString(-1, 210, "press \x8d to exit", text_alpha | 0xffffff00);
    }
    else
    {
        for(i = linepos; i < (linepos + 6); i++)
        {
			leveltxt = skilltxt = 0;
			D_memcpy(savedata, &Pak_Data[(i * 32) + 16], 16);
			if (M_DecodePassword((byte*)&savedata, &leveltxt, &skilltxt, 0) == 0)  {
                D_memmove(buffer, "no save");
            }
			else
			{
				switch (skilltxt)
				{
					case 4:
						sprintf(buffer, "Level: %02d Skill: %s", leveltxt, M_TXT19);
						break;
					case 3:
						sprintf(buffer, "Level: %02d Skill: %s", leveltxt, M_TXT18);
						break;
					case 2:
						sprintf(buffer, "Level: %02d Skill: %s", leveltxt, M_TXT17);
						break;
					case 1:
						sprintf(buffer, "Level: %02d Skill: %s", leveltxt, M_TXT16);
						break;
					case 0:
						sprintf(buffer, "Level: %02d Skill: %s", leveltxt, M_TXT15);
						break;
					default:
						sprintf(buffer, "Level: %02d Skill: %d", leveltxt, skilltxt);
						break;
				}
			}
				

            ST_DrawString(23, (i - linepos) * 15 + 65, buffer, text_alpha | 0xff000000);
        }

        if (linepos != 0) {
            ST_DrawString(23, 50, "\x8f more...", text_alpha | 0xffffff00);
        }

        if ((linepos + 6) <= ((Pak_Size >> 5) - 1)) {
            ST_DrawString(23, 155, "\x8e more...", text_alpha | 0xffffff00);
        }

        ST_DrawSymbol(11, (cursorpos - linepos) * 15 + 66, 78, text_alpha | 0xffffff00);

        ST_DrawString(-1, 195, "press \x8d to exit", text_alpha | 0xffffff00);
        ST_DrawString(-1, 210, "press \x8a to save", text_alpha | 0xffffff00);
    }

    I_DrawFrame();
}

void M_LoadPakStart(void) // 8000AEEC
{
    unsigned char i;
    int size;

    cursorpos = 0;
    linepos = 0;

    size = Pak_Size / 32;

    i = 0;
    if (size != 0)
    {
        do
        {
            if (Pak_Data[i * 32])
                break;

            i++;
        } while (i != size);
    }

    if (i < size)
    {
        cursorpos = i;

        if (!(size < (i+6)))
            linepos = i;
        else
            linepos = (size-6);
    }

    M_FadeInStart();
}

void M_LoadPakStop(void) // 8000AF8C
{
    S_StartSound(NULL, sfx_pistol);
    M_FadeOutStart(ga_exit);

    if (Pak_Data)
    {
        Z_Free(Pak_Data);
        Pak_Data = NULL;
    }
}

int M_LoadPakTicker(void) // 8000AFE4
{
    unsigned int buttons;
    unsigned int oldbuttons;
    int size;
    int skill;
    int levelnum;
    int exit;

    if ((gamevbls < gametic) && ((gametic & 3U) == 0)) {
        MenuAnimationTic = (MenuAnimationTic + 1) & 7;
    }

    buttons = M_ButtonResponder(ticbuttons[0]);
    oldbuttons = oldticbuttons[0] & 0xffff0000;

    if (!(buttons & ALL_JPAD))
    {
        m_vframe1 = 0;
    }
    else
    {
        m_vframe1 -= vblsinframe[0];

        if (m_vframe1 <= 0)
        {
            m_vframe1 = 0xf; // TICRATE/2

            if (buttons & PAD_DOWN)
            {
                cursorpos += 1;

                size = (Pak_Size / 32) - 1;

                if (size < cursorpos)
                    cursorpos = size;
                else
                    S_StartSound(NULL, sfx_switch1);

                if ((linepos + 5) < cursorpos)
                    linepos += 1;

            }
            else if (buttons & PAD_UP)
            {
                cursorpos -= 1;

                if (cursorpos < 0)
                    cursorpos = 0;
                else
                    S_StartSound(NULL, sfx_switch1);

                if(cursorpos < linepos)
                    linepos -= 1;
            }
        }
    }

    if (!(buttons ^ oldbuttons) || !(buttons & PAD_START))
    {
        if (!(buttons ^ oldbuttons) || buttons != PAD_A)
        {
            exit = ga_nothing;
        }
        else
        {
            // load the password data in text format
            D_memcpy(&Passwordbuff, &Pak_Data[((cursorpos * 32) + 16)], 16);

            if (M_DecodePassword(Passwordbuff, &levelnum, &skill, 0) == 0)
            {
                CurPasswordSlot = 0;
                exit = ga_exit;
            }
            else
            {
				// load configuration
				D_memcpy(&SavedConfig, &Pak_Data[cursorpos * 32], 16);
				M_DecodeConfig();
			
                doPassword = true;
                CurPasswordSlot = 16;

                startmap = gamemap = levelnum;
                startskill = gameskill = skill;

                exit = ga_warped;
            }
        }
    }
    else
    {
        exit = ga_exit;
    }

    return exit;
}

void M_LoadPakDrawer(void) // 8000B270
{
    unsigned char i;
    char buffer[32];
    byte savedata[16];
    int leveltxt, skilltxt;

    ST_DrawString(-1, 20, "Memory Pak", text_alpha | 0xff000000);

    for(i = linepos; i < (linepos + 6); i++)
    {
        if (FilesUsed == -1) {
            D_memmove(buffer, "-");
        }
        else {
			leveltxt = skilltxt = 0;
			D_memcpy(savedata, &Pak_Data[(i * 32) + 16], 16);
			if (M_DecodePassword((byte*)&savedata, &leveltxt, &skilltxt, 0) == 0)  {
                D_memmove(buffer, "no save");
            }
			else
			{
				M_DecodePassword((byte*)&savedata, &leveltxt, &skilltxt, 0);
				switch (skilltxt)
				{
					case 4:
						sprintf(buffer, "Level: %02d Skill: %s", leveltxt, M_TXT19);
						break;
					case 3:
						sprintf(buffer, "Level: %02d Skill: %s", leveltxt, M_TXT18);
						break;
					case 2:
						sprintf(buffer, "Level: %02d Skill: %s", leveltxt, M_TXT17);
						break;
					case 1:
						sprintf(buffer, "Level: %02d Skill: %s", leveltxt, M_TXT16);
						break;
					case 0:
						sprintf(buffer, "Level: %02d Skill: %s", leveltxt, M_TXT15);
						break;
					default:
						sprintf(buffer, "Level: %02d Skill: %d", leveltxt, skilltxt);
						break;
				}
			}
        }

        ST_DrawString(23, (i - linepos) * 15 + 65, buffer, text_alpha | 0xff000000);
    }

    if (linepos != 0) {
        ST_DrawString(23, 50, "\x8f more...", text_alpha | 0xffffff00);
    }

    if ((linepos + 6) <= ((Pak_Size >> 5) - 1)) {
        ST_DrawString(23, 155, "\x8e more...", text_alpha | 0xffffff00);
    }

    ST_DrawSymbol(11, (cursorpos - linepos) * 15 + 66, 78, text_alpha | 0xffffff00);

    ST_DrawString(-1, 195, "press \x8D to exit", text_alpha | 0xffffff00);
    ST_DrawString(-1, 210, "press \x8a to load", text_alpha | 0xffffff00);
}

int M_CenterDisplayTicker(void) // 8000B4C4
{
    unsigned int buttons, oldbuttons;
    int exit;

    buttons = M_ButtonResponder(ticbuttons[0]);
    oldbuttons = oldticbuttons[0] & 0xffff0000;

    if ((buttons == oldbuttons) || !(buttons & PAD_START))
    {
        if (buttons & PAD_LEFT)
        {
            Display_X -= 1;
            if (Display_X < -16)
                Display_X = -16;
        }
        else if (buttons & PAD_RIGHT)
        {
            Display_X += 1;
            if (Display_X > 24)
                Display_X = 24;
        }

        if (buttons & PAD_UP)
        {
            Display_Y -= 1;
            if (Display_Y < -20)
                Display_Y = -20;
        }
        else if (buttons & PAD_DOWN)
        {
            Display_Y += 1;
            if (Display_Y > 12)
                Display_Y = 12;
        }

        if (buttons & ALL_JPAD)
            I_MoveDisplay(Display_X, Display_Y);

        exit = 0;
    }
    else
    {
        S_StartSound(NULL, sfx_pistol);
        exit = 8;
    }

    return exit;
}

void M_CenterDisplayDrawer(void) // 8000B604
{
    ST_DrawString(-1, 20, "Center Display", text_alpha | 0xff000000);
    ST_DrawString(-1, 114, "use control pad to adjust", text_alpha | 0xffffff00);
    ST_DrawString(-1, 210, "press \x8d to exit", text_alpha | 0xffffff00);
}

int M_ControlPadTicker(void) // 8000B694
{
    unsigned int buttons;
    unsigned int oldbuttons;
    int exit;
    unsigned char i = 0;

    if ((gamevbls < gametic) && ((gametic & 3U) == 0)) {
        MenuAnimationTic = (MenuAnimationTic + 1) & 7;
    }

    buttons = M_ButtonResponder(ticbuttons[0] & 0xffff);

    if (!(buttons & ALL_JPAD))
    {
        m_vframe1 = 0;
    }
    else
    {
        m_vframe1 = m_vframe1 - vblsinframe[0];
        if (m_vframe1 <= 0)
        {
            m_vframe1 = 0xf; // TICRATE / 2

            if (buttons & PAD_DOWN)
            {
                cursorpos += 1;
                if (cursorpos < 15)
                    S_StartSound(NULL, sfx_switch1);
                else
                    cursorpos = 14;

                if (cursorpos > (linepos + 6))
                    linepos += 1;
            }
            else
            {
                if (buttons & PAD_UP)
                {
                    cursorpos -= 1;
                    if (cursorpos < 0)
                        cursorpos = 0;
                    else
                        S_StartSound(NULL, sfx_switch1);

                    if (cursorpos < linepos)
                        linepos -= 1;
                }
            }
        }
    }

    buttons = ticbuttons[0] & 0xffff0000;
    oldbuttons = oldticbuttons[0] & 0xffff0000;

    if (buttons & PAD_START)
    {
        S_StartSound(NULL, sfx_pistol);
        exit = 8;
    }
    else
    {
        if (buttons == oldbuttons)
            exit = 0;
        else
        {
            if (cursorpos == 0)
            {
                if (!(buttons & (PAD_UP|PAD_LEFT)))
                {
                    if (buttons & (PAD_DOWN|PAD_RIGHT|ALL_BUTTONS))
                    {
                        Autorun += 1;
                        if (Autorun > 2) Autorun = 0;
                    }
                }
                else
                {
                    Autorun -= 1;
                    if (Autorun < 0) Autorun = 2;
                }
                if ((buttons & (ALL_BUTTONS|ALL_JPAD)) != 0)
                {
                    S_StartSound(NULL, sfx_switch2);
                    return 0;
                }
            }
            else if (cursorpos == 1) // Set Default Configuration
            {
                if (!(buttons & (PAD_UP|PAD_LEFT)))
                {
                    if (buttons & (PAD_DOWN|PAD_RIGHT|ALL_BUTTONS))
                    {
                        ConfgNumb += 1;
                        if(ConfgNumb > 5)
                            ConfgNumb = 0;
                    }
                }
                else
                {
                    if (ConfgNumb == 0)
                        ConfgNumb = 5;
					else
						ConfgNumb -= 1;
                }

                if ((buttons & (ALL_BUTTONS|ALL_JPAD)) != 0)
                {
                    D_memcpy(ActualConfiguration, DefaultConfiguration[ConfgNumb], (13 * sizeof(int)));
                    S_StartSound(NULL, sfx_switch2);
                    return 0;
                }
            }
            else // Set Custom Configuration
            {
                for (i = 0; i < 13; i++)
                {
                    if ((buttons & TempConfiguration[i]) != 0)
                    {
                        ActualConfiguration[cursorpos - 2] = TempConfiguration[i];
                        S_StartSound(NULL,sfx_switch2);
                        return 0;
                    }
                }
            }
            exit = 0;
        }
    }
    return exit;
}

void M_ControlPadDrawer(void) // 8000B988
{
    unsigned char i, lpos;
    char **text;
    char buffer [44];

    ST_DrawString(-1, 20, "Control Pad", text_alpha | 0xff000000);

    if (linepos < (linepos + 6))
    {
        text = &ControlText[linepos];
        lpos = linepos;
        do
        {
            if (lpos == 0)
            {
                switch (Autorun)
                {
                    case 2:
                        sprintf(buffer, *text, "Toggle");
                        break;
                    case 1:
                        sprintf(buffer, *text, "On");
                        break;
                    case 0:
                    default:
                        sprintf(buffer, *text, "Off");
                        break;
                }   
            }
            else if (lpos >= 1)
            {
                if (lpos > 1)
                {
                    if(lpos != cursorpos || ((ticon & 8U) == 0))
                    {
                        for (i = 0; i < 13; i++)
                        {
                            if (ActualConfiguration[lpos - 2] == TempConfiguration[i])
                            {
                                break;
                            }
                        }
                        ST_DrawSymbol(60, ((lpos - linepos) * 18) + 59, i + 80, text_alpha | 0xffffff00);
                    }
                }
                sprintf(buffer, *text, ConfgNumb + 1);
            }

            ST_DrawString(80, ((lpos - linepos) * 18) + 59, buffer, text_alpha | 0xff000000);

            lpos += 1;
            text += 1;
        } while (lpos < (linepos + 7));
    }

    if (linepos != 0) {
        ST_DrawString(80, 41, "\x8f more...", text_alpha | 0xffffff00);
    }

    if ((linepos + 6) < 14) {
        ST_DrawString(80, 185, "\x8e more...", text_alpha | 0xffffff00);
    }

    ST_DrawSymbol(23,(cursorpos - linepos) * 0x12 + 0x3b - 9, MenuAnimationTic + 0x46, text_alpha | 0xffffff00);

    ST_DrawString(-1, 210, "press \x8d to exit", text_alpha | 0xffffff00);
}
