/* f_main.c -- finale */

#include "doomdef.h"
#include "p_local.h"
#include "st_main.h"
#include "r_local.h"

extern void P_CheckCheats (void);

#define T_NULL	        ""

// [Immorpher] Doom 64 introduction text adapted from Doom 64 Reloaded
#define C_INTR1_TXT01	"Surviving the poison of evil has"
#define C_INTR1_TXT02	"pushed sanity to an edge. The"
#define C_INTR1_TXT03	"doctors' treatments are of little"
#define C_INTR1_TXT04	"help. Nightmares of so many demons"
#define C_INTR1_TXT05	"pouring through will not stop."
#define C_INTR1_TXT06	" "
#define C_INTR1_TXT07	"The outpost at Phobos was left dead"
#define C_INTR1_TXT08	"in a nuclear haze. However, hell"
#define C_INTR1_TXT09	"has reached back in. Something has"
#define C_INTR1_TXT10	"distorted reality and resurrected"
#define C_INTR1_TXT11	"decaying carnage back into hideous"
#define C_INTR1_TXT12	"living tissue."
#define C_INTR1_TXT13	" "
#define C_INTR1_TXT14	"The mutations are devastating. The"
#define C_INTR1_TXT15	"demons have returned even stronger"
#define C_INTR1_TXT16	"and more vicious than before. As"
#define C_INTR1_TXT17	"the only one who has survived the"
#define C_INTR1_TXT18	"horror, the mission is clear..."
#define C_INTR1_TXT19	" "
#define C_INTR1_TXT20	"MERCILESS EXTERMINATION!"

// [Immorpher] Lost Levels introduction text adapted from Doom 64 CE
#define C_INTR2_TXT01	"The mother demon's lifeless stench"
#define C_INTR2_TXT02	"is sharp as a dagger in the mind."
#define C_INTR2_TXT03	"With their mother dead, the demons"
#define C_INTR2_TXT04	"have turned to a state of violence"
#define C_INTR2_TXT05	"and hunger. Nothing can be allowed"
#define C_INTR2_TXT06	"to refocus their rage towards us."
#define C_INTR2_TXT07	" "
#define C_INTR2_TXT08	"Each moment, hell pushes your mind"
#define C_INTR2_TXT09	"to a more primitive state. It is a"
#define C_INTR2_TXT10	"struggle not to join the chaos."
#define C_INTR2_TXT11	"The demonic hordes are endless."
#define C_INTR2_TXT12	"Loss of coherent thought is making"
#define C_INTR2_TXT13	"it difficult to find their origin."
#define C_INTR2_TXT14	" "
#define C_INTR2_TXT15	"Suddenly, you find yourself back in"
#define C_INTR2_TXT16	"a vacated UAC base. The last time"
#define C_INTR2_TXT17	"you were ripped across worlds was by"
#define C_INTR2_TXT18	"the mother demon. Has sanity broke?"
#define C_INTR2_TXT19	"You must return to hell. Your path"
#define C_INTR2_TXT20	"of extermination shall not end!"

// [Immorpher] Beta 64 introduction text adapted from Doom 64 CE
#define C_INTR3_TXT01	"The moon of Io was chosen by the"
#define C_INTR3_TXT02	"U.A.C. to resume portal testing."
#define C_INTR3_TXT03	"We were prepared for any demon who"
#define C_INTR3_TXT04	"entered through the portal. But"
#define C_INTR3_TXT05	"the demons made a ship of flesh"
#define C_INTR3_TXT06	"and iron. They attacked from space"
#define C_INTR3_TXT07	"and I barely stopped the invasion."
#define C_INTR3_TXT08	" "
#define C_INTR3_TXT09	"Everyone I knew died. I was meant"
#define C_INTR3_TXT10	"to go down with the facility. It"
#define C_INTR3_TXT11	"should be me frozen on the surface"
#define C_INTR3_TXT12	"of Io. Instead I remained on a"
#define C_INTR3_TXT13	"research ship observing the moon."
#define C_INTR3_TXT14	" "
#define C_INTR3_TXT15	"Unexpectedly, I received a message"
#define C_INTR3_TXT16	"from Io. It said demons and humans"
#define C_INTR3_TXT17	"alike have returned back from the"
#define C_INTR3_TXT18	"dead. My comrades will not be able"
#define C_INTR3_TXT19	"to rest until I end what the demons"
#define C_INTR3_TXT20	"are doing... I must return to Io!"

// [Immorpher] Bonus Pak Introduction
#define C_INTR4_TXT01	"Of many worlds, the demons infect."
#define C_INTR4_TXT02	"Even after death, their mother"
#define C_INTR4_TXT03	"will resurrect. They bring"
#define C_INTR4_TXT04	"damnation through decimation. The"
#define C_INTR4_TXT05	"demons will relentlessly spawn"
#define C_INTR4_TXT06	"until they bring a new dawn."
#define C_INTR4_TXT07	" "
#define C_INTR4_TXT08	"But there are those who forge a"
#define C_INTR4_TXT09	"trail of destruction. They are"
#define C_INTR4_TXT10	"the tide that brings retribution."
#define C_INTR4_TXT11	"Any ordeal they will suffer, as"
#define C_INTR4_TXT12	"long as their weapons are allowed"
#define C_INTR4_TXT13	"to thunder."
#define C_INTR4_TXT14	" "
#define C_INTR4_TXT15	"The demons will have fear, because"
#define C_INTR4_TXT16	"you can be anywhere through here."
#define C_INTR4_TXT17	"Your guns will fume as you bring"
#define C_INTR4_TXT18	"the demons their doom. Choose any"
#define C_INTR4_TXT19	"destination, as it is time for"
#define C_INTR4_TXT20	"merciless extermination!"

#define C_END1_TXT01	"You cackle as the"
#define C_END1_TXT02	"familiarity of the"
#define C_END1_TXT03	"situation occurs to you."
#define C_END1_TXT04	"The gateway to the Demons"
#define C_END1_TXT05	"domain was too accessible."
#define C_END1_TXT06	"You realize the demons mock"
#define C_END1_TXT07	"you with their invitation."
#define C_END1_TXT08	"It does not matter..."
#define C_END1_TXT09	"The demons spawn like rats"
#define C_END1_TXT10	"and you have the grade AAA"
#define C_END1_TXT11	"U.A.C. poison they crave."
#define C_END1_TXT12	"Your bloodthirsty scream"
#define C_END1_TXT13	"shatters the teleport haze."
#define C_END1_TXT14	"Once again you find yourself"
#define C_END1_TXT15	"amidst..."

#define C_END2_TXT01	"The vast silence reminds"
#define C_END2_TXT02	"you of the military morgue."
#define C_END2_TXT03	" "
#define C_END2_TXT04	"You knew the installation"
#define C_END2_TXT05	"had a classified level."
#define C_END2_TXT06	" "
#define C_END2_TXT07	"The U.A.C. had some good"
#define C_END2_TXT08	"reason to hide this place."
#define C_END2_TXT09	" "
#define C_END2_TXT10	"You wonder what it"
#define C_END2_TXT11	"could be..."

#define C_END3_TXT01	"You smile."
#define C_END3_TXT02	" "
#define C_END3_TXT03	"What strange place have"
#define C_END3_TXT04	"you stumbled upon?"
#define C_END3_TXT05	" "
#define C_END3_TXT06	"The Demons did not expect"
#define C_END3_TXT07	"you to survive this far."
#define C_END3_TXT08	"You feel their demonic"
#define C_END3_TXT09	"presence waiting for you..."
#define C_END3_TXT10	" "
#define C_END3_TXT11	"Let them taste their guts!"

#define C_END4_TXT01	"You wretch as a strange"
#define C_END4_TXT02	"acrid odor assaults you."
#define C_END4_TXT03	" "
#define C_END4_TXT04	"Death and Demon carcass!"
#define C_END4_TXT05	" "
#define C_END4_TXT06	"No nightmare could have"
#define C_END4_TXT07	"prepared you for this."
#define C_END4_TXT08	" "
#define C_END4_TXT09	"You realize that this"
#define C_END4_TXT10	"place was not meant for"
#define C_END4_TXT11	"living humans."

#define C_END5_TXT01	"Congratulations!"
#define C_END5_TXT02	"You found..."
#define C_END5_TXT03	" "
#define C_END5_TXT04	"HECTIC"
#define C_END5_TXT05	" "
#define C_END5_TXT06	"Only the best will reap"
#define C_END5_TXT07	"its rewards."

#define C_END6_TXT01	"Finally..."
#define C_END6_TXT02	"The mother of all demons"
#define C_END6_TXT03	"is dead!"
#define C_END6_TXT04	" "
#define C_END6_TXT05	"The blood pours from"
#define C_END6_TXT06	"your eyes as you stand"
#define C_END6_TXT07	"in defiance."
#define C_END6_TXT08	" "
#define C_END6_TXT09	"As the only marine to"
#define C_END6_TXT10	"endure the slaughter-"
#define C_END6_TXT11	"you decide to remain"
#define C_END6_TXT12	"in Hell and ensure no"
#define C_END6_TXT13	"demon ever rises again."
#define C_END6_TXT14	" "
#define C_END6_TXT15	"The End."

// Lost Levels

#define C_END7_TXT01    "You had not expected to be"
#define C_END7_TXT02	"torn from Hell so soon after"
#define C_END7_TXT03	"your fateful decision."
#define C_END7_TXT04	"Getting back there was your"
#define C_END7_TXT05	"only concern. The plans of"
#define C_END7_TXT06	"the Sister Ressurector to"
#define C_END7_TXT07	"exterminate you have failed."
#define C_END7_TXT08	" "
#define C_END7_TXT09	"A grim vision takes hold of"
#define C_END7_TXT10	"your mind as the demon"
#define C_END7_TXT11	"carcasses steam in your wake."
#define C_END7_TXT12	"Stretched before you is a"
#define C_END7_TXT13	"path of perpetual torment..."
#define C_END7_TXT14	"A path through DOOM..."

// Beta 64

#define C_END8_TXT01	"The presence of hell is"
#define C_END8_TXT02	"seeping in the facility like"
#define C_END8_TXT03	"blood through arteries."
#define C_END8_TXT04	"It is upturning reality and"
#define C_END8_TXT05	"infecting the dead."
#define C_END8_TXT06	" "
#define C_END8_TXT07	"A stench leaks from the"
#define C_END8_TXT08	"portal. It is from a place"
#define C_END8_TXT09	"not for humans. But I have"
#define C_END8_TXT10	"not felt human for while..."
#define C_END8_TXT11	" "
#define C_END8_TXT12	"Our mistakes have damned"
#define C_END8_TXT13	"this place! I have to go to"
#define C_END8_TXT14	"their world. Let them bring"
#define C_END8_TXT15	"their worst. This ends in hell!"

#define C_END9_TXT01	"What kind of madness is this?"
#define C_END9_TXT02	" "
#define C_END9_TXT03	"I heard rumors of a secret lab,"
#define C_END9_TXT04	"but I did not think the U.A.C."
#define C_END9_TXT05	"would have the audacity."
#define C_END9_TXT06	"the security operations."
#define C_END9_TXT07	" "
#define C_END9_TXT08	"I must find the purpose of this"
#define C_END9_TXT09	"lab. The U.A.C. must not be"
#define C_END9_TXT10	"allowed to keep their secrets."

#define C_END10_TXT01	"I wretch as an acrid fog"
#define C_END10_TXT02	"assaults my eyes. There is no"
#define C_END10_TXT03	"mistaking the odor of a U.A.C."
#define C_END10_TXT04	"weapons facility."
#define C_END10_TXT05	" "
#define C_END10_TXT06	"Why is it so deep in hell and"
#define C_END10_TXT07	"what went wrong? It seems to"
#define C_END10_TXT08	"have been enveloped in this"
#define C_END10_TXT09	"toxic haze for a long time."
#define C_END10_TXT10	" "
#define C_END10_TXT11	"Perhaps they were trying to"
#define C_END10_TXT12	"get back here. What hellish"
#define C_END10_TXT13	"weapon technology could be"
#define C_END10_TXT14	"so important?"

#define C_END11_TXT01	"The death of the demon"
#define C_END11_TXT02	"shook the ground as the"
#define C_END11_TXT03	"world faded away. What"
#define C_END11_TXT04	"has pulled me from reality?"
#define C_END11_TXT05	" "
#define C_END11_TXT06	"An entity feels close. I can"
#define C_END11_TXT07	"feel hate... It hates as much."
#define C_END11_TXT08	"as I do and it wishes for my"
#define C_END11_TXT09	"death."
#define C_END11_TXT10	" "
#define C_END11_TXT11	"I have been dropped in its"
#define C_END11_TXT12	"realm. The smell of demon is"
#define C_END11_TXT13	"everywhere! They wish to"
#define C_END11_TXT14	"end my suffering. Only if it"
#define C_END11_TXT15	"would be that simple!"

#define C_END12_TXT01	"The demons have entered more"
#define C_END12_TXT02	"worlds than we can comprehend."
#define C_END12_TXT03	"This place does not follow the"
#define C_END12_TXT04	"laws of nature."
#define C_END12_TXT05	" "
#define C_END12_TXT06	"A force beyond human"
#define C_END12_TXT07	"understanding suspends this"
#define C_END12_TXT08	"structure in an endless crimson"
#define C_END12_TXT09	"void. A source of demonic"
#define C_END12_TXT10	"power resides here."
#define C_END12_TXT11	" "
#define C_END12_TXT12	"This is either the edge of my"
#define C_END12_TXT13	"sanity, or perhaps a key to"
#define C_END12_TXT14	"fighting the demons..."

#define C_END13_TXT01	"The mother of horrors has"
#define C_END13_TXT02	"been slain! My comrades can"
#define C_END13_TXT03	"now rest in peace."
#define C_END13_TXT04	" "
#define C_END13_TXT05	"For the first time since"
#define C_END13_TXT06	"entering hell a sense of"
#define C_END13_TXT07	"peace permeates. I have"
#define C_END13_TXT08	"found a purpose to live."
#define C_END13_TXT09	" "
#define C_END13_TXT10	"Hell is vast and the demons"
#define C_END13_TXT11	"scavenge like rats. I shall"
#define C_END13_TXT12	"hunt down every weapon and"
#define C_END13_TXT13	"every circuit. I will bring"
#define C_END13_TXT14	"them into the stone age!"

// Introduction Texts

char *introcluster1[] =   // [Immorpher] Doom 64 intro text adapted from Doom 64 Manual and Doom 64 Reloaded
{
    C_INTR1_TXT01,
	C_INTR1_TXT02,
	C_INTR1_TXT03,
	C_INTR1_TXT04,
	C_INTR1_TXT05,
	C_INTR1_TXT06,
	C_INTR1_TXT07,
	C_INTR1_TXT08,
	C_INTR1_TXT09,
	C_INTR1_TXT10,
	C_INTR1_TXT11,
	C_INTR1_TXT12,
	C_INTR1_TXT13,
	C_INTR1_TXT14,
	C_INTR1_TXT15,
	C_INTR1_TXT16,
	C_INTR1_TXT17,
	C_INTR1_TXT18,
	C_INTR1_TXT19,
	C_INTR1_TXT20,
	T_NULL
};

char *introcluster2[] =   // [Immorpher] Lost Levels intro text adapted from Doom 64 CE
{
    C_INTR2_TXT01,
	C_INTR2_TXT02,
	C_INTR2_TXT03,
	C_INTR2_TXT04,
	C_INTR2_TXT05,
	C_INTR2_TXT06,
	C_INTR2_TXT07,
	C_INTR2_TXT08,
	C_INTR2_TXT09,
	C_INTR2_TXT10,
	C_INTR2_TXT11,
	C_INTR2_TXT12,
	C_INTR2_TXT13,
	C_INTR2_TXT14,
	C_INTR2_TXT15,
	C_INTR2_TXT16,
	C_INTR2_TXT17,
	C_INTR2_TXT18,
	C_INTR2_TXT19,
	C_INTR2_TXT20,
	T_NULL
};

char *introcluster3[] =   // [Immorpher] Beta 64 intro text adapted from Doom 64 CE
{
    C_INTR3_TXT01,
	C_INTR3_TXT02,
	C_INTR3_TXT03,
	C_INTR3_TXT04,
	C_INTR3_TXT05,
	C_INTR3_TXT06,
	C_INTR3_TXT07,
	C_INTR3_TXT08,
	C_INTR3_TXT09,
	C_INTR3_TXT10,
	C_INTR3_TXT11,
	C_INTR3_TXT12,
	C_INTR3_TXT13,
	C_INTR3_TXT14,
	C_INTR3_TXT15,
	C_INTR3_TXT16,
	C_INTR3_TXT17,
	C_INTR3_TXT18,
	C_INTR3_TXT19,
	C_INTR3_TXT20,
	T_NULL
};

char *introcluster4[] =   // [Immorpher] Bonus Pak intro text
{
    C_INTR4_TXT01,
	C_INTR4_TXT02,
	C_INTR4_TXT03,
	C_INTR4_TXT04,
	C_INTR4_TXT05,
	C_INTR4_TXT06,
	C_INTR4_TXT07,
	C_INTR4_TXT08,
	C_INTR4_TXT09,
	C_INTR4_TXT10,
	C_INTR4_TXT11,
	C_INTR4_TXT12,
	C_INTR4_TXT13,
	C_INTR4_TXT14,
	C_INTR4_TXT15,
	C_INTR4_TXT16,
	C_INTR4_TXT17,
	C_INTR4_TXT18,
	C_INTR4_TXT19,
	C_INTR4_TXT20,
	T_NULL
};

// Doom 64 Texts

char *endcluster1[] =   // 8005A2C0
{
    C_END1_TXT01,
	C_END1_TXT02,
	C_END1_TXT03,
	C_END1_TXT04,
	C_END1_TXT05,
	C_END1_TXT06,
	C_END1_TXT07,
	C_END1_TXT08,
	C_END1_TXT09,
	C_END1_TXT10,
	C_END1_TXT11,
	C_END1_TXT12,
	C_END1_TXT13,
	C_END1_TXT14,
	C_END1_TXT15,
	T_NULL
};

char *endcluster2[] =   // 8005A300
{
    C_END2_TXT01,
	C_END2_TXT02,
	C_END2_TXT03,
	C_END2_TXT04,
	C_END2_TXT05,
	C_END2_TXT06,
	C_END2_TXT07,
	C_END2_TXT08,
	C_END2_TXT09,
	C_END2_TXT10,
	C_END2_TXT11,
	T_NULL
};

char *endcluster3[] =   // 8005A330
{
    C_END3_TXT01,
	C_END3_TXT02,
	C_END3_TXT03,
	C_END3_TXT04,
	C_END3_TXT05,
	C_END3_TXT06,
	C_END3_TXT07,
	C_END3_TXT08,
	C_END3_TXT09,
	C_END3_TXT10,
	C_END3_TXT11,
	T_NULL
};

char *endcluster4[] =   // 8005A360
{
    C_END4_TXT01,
	C_END4_TXT02,
	C_END4_TXT03,
	C_END4_TXT04,
	C_END4_TXT05,
	C_END4_TXT06,
	C_END4_TXT07,
	C_END4_TXT08,
	C_END4_TXT09,
	C_END4_TXT10,
	C_END4_TXT11,
	T_NULL
};

char *endcluster5[] =   // 8005A390
{
    C_END5_TXT01,
	C_END5_TXT02,
	C_END5_TXT03,
	C_END5_TXT04,
	C_END5_TXT05,
	C_END5_TXT06,
	C_END5_TXT07,
	T_NULL
};

char *endcluster6[] =   // 8005A3B0
{
    C_END6_TXT01,
	C_END6_TXT02,
	C_END6_TXT03,
	C_END6_TXT04,
	C_END6_TXT05,
	C_END6_TXT06,
	C_END6_TXT07,
	C_END6_TXT08,
	C_END6_TXT09,
	C_END6_TXT10,
	C_END6_TXT11,
	C_END6_TXT12,
	C_END6_TXT13,
	C_END6_TXT14,
	C_END6_TXT15,
	T_NULL
};

// Lost Levels

char *endcluster7[] =
{
    C_END7_TXT01,
    C_END7_TXT02,
    C_END7_TXT03,
    C_END7_TXT04,
    C_END7_TXT05,
    C_END7_TXT06,
    C_END7_TXT07,
    C_END7_TXT08,
    C_END7_TXT09,
    C_END7_TXT10,
    C_END7_TXT11,
    C_END7_TXT12,
    C_END7_TXT13,
    C_END7_TXT14,
    T_NULL
};

// Beta 64

char *endcluster8[] =   // 8005A2C0
{
    C_END8_TXT01,
	C_END8_TXT02,
	C_END8_TXT03,
	C_END8_TXT04,
	C_END8_TXT05,
	C_END8_TXT06,
	C_END8_TXT07,
	C_END8_TXT08,
	C_END8_TXT09,
	C_END8_TXT10,
	C_END8_TXT11,
	C_END8_TXT12,
	C_END8_TXT13,
	C_END8_TXT14,
	C_END8_TXT15,
	T_NULL
};

char *endcluster9[] =   // 8005A300
{
    C_END9_TXT01,
	C_END9_TXT02,
	C_END9_TXT03,
	C_END9_TXT04,
	C_END9_TXT05,
	C_END9_TXT06,
	C_END9_TXT07,
	C_END9_TXT08,
	C_END9_TXT09,
	C_END9_TXT10,
	T_NULL
};

char *endcluster10[] =   // 8005A330
{
    C_END10_TXT01,
	C_END10_TXT02,
	C_END10_TXT03,
	C_END10_TXT04,
	C_END10_TXT05,
	C_END10_TXT06,
	C_END10_TXT07,
	C_END10_TXT08,
	C_END10_TXT09,
	C_END10_TXT10,
	C_END10_TXT11,
	C_END10_TXT12,
	C_END10_TXT13,
	C_END10_TXT14,
	T_NULL
};

char *endcluster11[] =   // 8005A360
{
    C_END11_TXT01,
	C_END11_TXT02,
	C_END11_TXT03,
	C_END11_TXT04,
	C_END11_TXT05,
	C_END11_TXT06,
	C_END11_TXT07,
	C_END11_TXT08,
	C_END11_TXT09,
	C_END11_TXT10,
	C_END11_TXT11,
	C_END11_TXT12,
	C_END11_TXT13,
	C_END11_TXT14,
	C_END11_TXT15,
	T_NULL
};

char *endcluster12[] =   // 8005A390
{
    C_END12_TXT01,
	C_END12_TXT02,
	C_END12_TXT03,
	C_END12_TXT04,
	C_END12_TXT05,
	C_END12_TXT06,
	C_END12_TXT08,
	C_END12_TXT09,
	C_END12_TXT10,
	C_END12_TXT11,
	C_END12_TXT12,
	C_END12_TXT13,
	C_END12_TXT14,
	T_NULL
};

char *endcluster13[] =   // 8005A3B0
{
    C_END13_TXT01,
	C_END13_TXT02,
	C_END13_TXT03,
	C_END13_TXT04,
	C_END13_TXT05,
	C_END13_TXT06,
	C_END13_TXT07,
	C_END13_TXT08,
	C_END13_TXT09,
	C_END13_TXT10,
	C_END13_TXT11,
	C_END13_TXT12,
	C_END13_TXT13,
	C_END13_TXT14,
	T_NULL
};

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
    if (nextmap == 0)
    {
        text = introcluster4;
        textypos = 20;
    } else if (nextmap == 1)
    {
        text = introcluster1;
        textypos = 20;
    } else if (nextmap == LOSTLEVEL)
    {
        text = introcluster2;
        textypos = 20;
    } else if (nextmap == BETALEVEL)
    {
        text = introcluster3;
        textypos = 20;
    }
    else if (nextmap == 9)
    {
        text = endcluster1;
        textypos = 15;
    }
    else if (nextmap == 29)
    {
        text = endcluster2;
        textypos = 43;
    }
    else if (nextmap == 30)
    {
        text = endcluster3;
        textypos = 43;
    }
    else if (nextmap == 31)
    {
        text = endcluster4;
        textypos = 43;
    }
    else if (nextmap == 32)
    {
        text = endcluster5;
        textypos = 71;
    }
	else if (nextmap == 49) // Beta 64
    {
        text = endcluster8;
        textypos = 15;
    }
    else if (nextmap == 72)
    {
        text = endcluster9;
        textypos = 15;
    }
    else if (nextmap == 71)
    {
        text = endcluster10;
        textypos = 15;
    }
    else if (nextmap == 57)
    {
        text = endcluster11;
        textypos = 15;
    }
    else if (nextmap == 70)
    {
        text = endcluster12;
        textypos = 15;
    }

    DrawerStatus = 2;
    textline = 0;
    textalpha = 0;
	
	S_StartMusic(mus_title); // [Immorpher] Play menu music for intermission
}

/*
=================
=
= F_StopIntermission
=
=================
*/

void F_StopIntermission(void) // 80002E14
{
	S_StopMusic(); // [Immorpher] stop intermission music
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
		
		 // [Immorpher] Speed up text intermission by pressing buttons
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
    int i, ypos;
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

void F_Stop(void) // 80003220
{
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
            if (gamemap == 65 ? *endcluster13[textline] : gamemap == 39 ? *endcluster7[textline] : *endcluster6[textline])
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
			
			// [Immorpher] Speed up text intermission by pressing buttons
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
	int i, type, alpha, ypos;

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
				
                ST_DrawString(-1, ypos, (gamemap == 65 ? endcluster13[i] : gamemap == 39 ? endcluster7[i] : endcluster6[i]), PACKRGBA(255, alpha, alpha, alpha));
                ypos += 14;
            }

            ST_DrawString(-1, ypos, (gamemap == 65 ? endcluster13[i] : gamemap == 39 ? endcluster7[i] : endcluster6[i]), PACKRGBA(255, textalpha, textalpha, textalpha));
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

	byte *data;
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


    data = W_CacheLumpNum(lump, PU_CACHE, dec_jag);

    compressed = ((spriteN64_t*)data)->compressed;
    tileh = ((spriteN64_t*)data)->tileheight;
    width = ((spriteN64_t*)data)->width;
    height = ((spriteN64_t*)data)->height;
    tiles = ((spriteN64_t*)data)->tiles;
    xoffs = ((spriteN64_t*)data)->xoffs;
    yoffs = ((spriteN64_t*)data)->yoffs;

    src = data + sizeof(spriteN64_t);

    if (compressed < 0)
    {
        width2 = (width + 7) & ~7;
        tilew = tileh * width2;

        if (((spriteN64_t*)data)->cmpsize & 1)
        {
            paldata = (byte *)(W_CacheLumpNum(((mobjinfo[type].palette + lump) -
                                    (((spriteN64_t*)data)->cmpsize >> 1)), PU_CACHE, dec_jag)) + 8;
        }
        else
        {
            paldata = (src + ((spriteN64_t*)data)->cmpsize);
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
        gDPSetTextureImage(GFX1++, G_IM_FMT_RGBA, G_IM_SIZ_16b , 1, (src + ((spriteN64_t*)data)->cmpsize));

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
