/* texts.h -- level names and intro/ending texts (Data/DOOM64.TXT, linked into the ROM) */

#ifndef __TEXTS_H__
#define __TEXTS_H__

void TX_Init(void);
const char *TX_MapName(int map);
int TX_MapMusic(int map);	/* music sequence number, 0 if none */
char **TX_LoadText(const char *kind, int num);	/* kind is "INTRO" or "END" */
void TX_FreeText(void);

#endif
