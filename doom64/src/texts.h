/* texts.h -- level names and intro/ending texts (Data/DOOM64.TXT, linked into the ROM) */

#ifndef __TEXTS_H__
#define __TEXTS_H__

void TX_Init(void);
const char *TX_MapName(int map);
int TX_NextMap(int map);	/* next map in DOOM64.TXT, wraps to the first; -1 if none */
int TX_PrevMap(int map);	/* previous map in DOOM64.TXT, wraps to the last; -1 if none */
int TX_MockCount(void);		/* number of [MOCK] texts in DOOM64.TXT */
const char *TX_Mock(int index);	/* mock text 0..TX_MockCount()-1 */
int TX_MapMusic(int map);	/* music sequence number, 0 if none */
char **TX_LoadText(const char *kind, int num);	/* kind is "INTRO" or "END" */
void TX_FreeText(void);

#endif
