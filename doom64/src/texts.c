/* texts.c -- level names, music and intro/ending texts, loaded from Data/DOOM64.TXT in the ROM

   doom64/Data/DOOM64.TXT is linked into the ROM next to the WAD (see doom64.ld):

     # comment
     [MAP 17]
     name = Watch Your Step
     music = 103

     [INTRO 1]
     one screen text line per file line
     (a blank line is a blank text line)

     [END 6]
     ...

   Level names are packed into one small resident block at startup. Intro and
   ending texts are read from the ROM only while their screen is up. */

#include "doomdef.h"
#include "texts.h"

#define TX_MAXNAME	31	/* menus and the automap copy names into small buffers */
#define TX_MAXMAPS	256
#define TX_MAXLINES	64
#define TX_MAXMOCKS	255	/* P_Random only gives 0..255 */
#define TX_MAXMOCK	255	/* longest mock text */

static char tx_empty[] = "";
static char tx_blank[] = " ";

static char *tx_namepool;		/* all level names, NUL separated; offset 0 = "" */
static unsigned short *tx_nameoff;	/* map number -> offset in pool */
static unsigned char tx_music[TX_MAXMAPS];	/* map number -> music sequence (0 = none) */
static char **tx_mock;			/* mock texts shown while the player is dead */
static int tx_nmock;
static void *tx_textblock;		/* result of the last TX_LoadText */

/* Returns the next line and advances *pp, or NULL at the end of the buffer.
   The line is NUL terminated in place (CR/LF removed). */
static char *TX_NextLine(char **pp)
{
	char *s = *pp, *e;

	if (!*s)
		return NULL;

	for (e = s; *e && *e != '\n'; e++)
		;
	*pp = *e ? e + 1 : e;
	if (*e)
		*e = 0;
	if (e > s && e[-1] == '\r')
		e[-1] = 0;
	return s;
}

extern char _doom64_txtSegmentRomStart[], _doom64_txtSegmentRomEnd[];

/* reads Data/DOOM64.TXT from the ROM into a temporary zone block, NUL terminated */
static char *TX_ReadText(void)
{
	int size;
	char *raw;

	size = _doom64_txtSegmentRomEnd - _doom64_txtSegmentRomStart;	/* multiple of 16, ends with NULs */
	if (size < 16 || size > 0x20000 || ((u32)_doom64_txtSegmentRomStart & 1))
		return NULL;	/* bad segment: run without texts instead of crashing */
	size = (size + 15) & ~15;
	raw = Z_Malloc((size + 16) & ~7, PU_STATIC, NULL);
	W_ReadRom((u32)_doom64_txtSegmentRomStart, raw, size);
	raw[size] = 0;
	return raw;
}

static int TX_Len(const char *s)
{
	const char *e = s;

	while (*e)
		e++;
	return e - s;
}

static void TX_Copy(char *d, const char *s)
{
	while ((*d++ = *s++))
		;
}

/* turns every backslash-n typed in the text file into a real new line (in place) */
static void TX_Unescape(char *s)
{
	char *w = s;

	while (*s)
	{
		if (s[0] == '\\' && s[1] == 'n')
		{
			*w++ = '\n';
			s += 2;
		}
		else
			*w++ = *s++;
	}
	*w = 0;
}

static char *TX_Trim(char *s)
{
	char *e;

	while (*s == ' ' || *s == '\t')
		s++;
	e = s + TX_Len(s);
	while (e > s && (e[-1] == ' ' || e[-1] == '\t'))
		e--;
	*e = 0;
	return s;
}

/* Parses "[WORD n]". Returns the section number, or -1 if line is not a header
   or the word does not match. */
static int TX_Header(char *line, const char *word)
{
	int n = 0;

	if (*line++ != '[')
		return -1;
	while (*word)
	{
		if (*line++ != *word++)
			return -1;
	}
	if (*line++ != ' ')
		return -1;
	while (*line >= '0' && *line <= '9')
		n = n * 10 + (*line++ - '0');
	return n;
}

void TX_Init(void)
{
	char *raw, *p, *line, *v;
	char *found[TX_MAXMAPS];
	char *mocks[TX_MAXMOCKS];
	int map, n, total, inmock, nmock;

	tx_namepool = NULL;
	tx_nameoff = NULL;
	tx_mock = NULL;
	tx_nmock = 0;
	for (n = 0; n < TX_MAXMAPS; n++)
		tx_music[n] = 0;

	raw = TX_ReadText();
	if (!raw)
		return;

	for (n = 0; n < TX_MAXMAPS; n++)
		found[n] = NULL;

	/* single pass: remember where each name is (inside raw) */
	map = -1;
	total = 1;
	inmock = 0;
	nmock = 0;
	p = raw;
	while ((line = TX_NextLine(&p)) != NULL)
	{
		if (*line == '[')
		{
			inmock = !D_strncmp(line, "[MOCK]", 6);
			map = TX_Header(line, "MAP");
			if (map >= TX_MAXMAPS)
				map = -1;
		}
		else if (inmock)
		{
			/* every non-empty line of [MOCK] is one mock text */
			v = TX_Trim(line);
			if (*v && *v != '#' && nmock < TX_MAXMOCKS)
			{
				TX_Unescape(v);
				if (TX_Len(v) > TX_MAXMOCK)
					v[TX_MAXMOCK] = 0;
				mocks[nmock++] = v;
			}
		}
		else if (map >= 0 && !D_strncmp(line, "music", 5))
		{
			v = line + 5;
			while (*v == ' ' || *v == '\t')
				v++;
			if (*v != '=')
				continue;
			v = TX_Trim(v + 1);
			n = 0;
			while (*v >= '0' && *v <= '9' && n < 255)
				n = n * 10 + (*v++ - '0');
			tx_music[map] = (n > 255) ? 255 : n;
		}
		else if (map >= 0 && !D_strncmp(line, "name", 4))
		{
			v = line + 4;
			while (*v == ' ' || *v == '\t')
				v++;
			if (*v != '=')
				continue;
			v = TX_Trim(v + 1);
			if (TX_Len(v) > TX_MAXNAME)
				v[TX_MAXNAME] = 0;
			if (!found[map])
				total += TX_Len(v) + 1;
			found[map] = v;
		}
	}

	tx_namepool = Z_Malloc(total, PU_STATIC, NULL);
	tx_nameoff = Z_Malloc(TX_MAXMAPS * sizeof(unsigned short), PU_STATIC, NULL);
	tx_namepool[0] = 0;

	total = 1;
	for (n = 0; n < TX_MAXMAPS; n++)
	{
		tx_nameoff[n] = 0;
		if (found[n])
		{
			TX_Copy(tx_namepool + total, found[n]);
			tx_nameoff[n] = total;
			total += TX_Len(found[n]) + 1;
		}
	}

	if (nmock)
	{
		char *dst;

		total = nmock * sizeof(char *);
		for (n = 0; n < nmock; n++)
			total += TX_Len(mocks[n]) + 1;

		tx_mock = Z_Malloc(total, PU_STATIC, NULL);
		dst = (char *)(tx_mock + nmock);
		for (n = 0; n < nmock; n++)
		{
			tx_mock[n] = dst;
			TX_Copy(dst, mocks[n]);
			dst += TX_Len(mocks[n]) + 1;
		}
		tx_nmock = nmock;
	}

	Z_Free(raw);
}

int TX_MockCount(void)
{
	return tx_nmock;
}

const char *TX_Mock(int index)
{
	if (index < 0 || index >= tx_nmock)
		return tx_empty;
	return tx_mock[index];
}

const char *TX_MapName(int map)
{
	if (!tx_namepool || map < 0 || map >= TX_MAXMAPS)
		return tx_empty;
	return tx_namepool + tx_nameoff[map];
}

/* a map exists if its [MAP n] section gives it a name or a music number */
static int TX_MapExists(int map)
{
	return tx_namepool && (tx_nameoff[map] || tx_music[map]);
}

/* Next map after "map" that is in the text file (map numbers can have gaps).
   After the last map it wraps around to the first one. Returns -1 if there are none. */
int TX_NextMap(int map)
{
	int i;

	for (i = map + 1; i < TX_MAXMAPS; i++)
	{
		if (TX_MapExists(i))
			return i;
	}
	for (i = 0; i <= map && i < TX_MAXMAPS; i++)
	{
		if (TX_MapExists(i))
			return i;
	}
	return -1;
}

/* Previous map before "map" that is in the text file. Before the first map it wraps
   around to the last one. Returns -1 if there are none. */
int TX_PrevMap(int map)
{
	int i;

	for (i = (map < TX_MAXMAPS ? map : TX_MAXMAPS) - 1; i >= 0; i--)
	{
		if (TX_MapExists(i))
			return i;
	}
	for (i = TX_MAXMAPS - 1; i >= map && i >= 0; i--)
	{
		if (TX_MapExists(i))
			return i;
	}
	return -1;
}

int TX_MapMusic(int map)
{
	if (map < 0 || map >= TX_MAXMAPS)
		return 0;
	return tx_music[map];
}

/* Loads "[INTRO n]" or "[END n]" and returns its lines as an array ended by an
   empty string. The array stays valid until TX_FreeText (or the next load). */
char **TX_LoadText(const char *kind, int num)
{
	char *raw, *p, *line, *dst;
	char *lines[TX_MAXLINES];
	char **result;
	int count = 0, i, size, insec = 0;

	TX_FreeText();

	raw = TX_ReadText();
	if (raw)
	{
		p = raw;
		while ((line = TX_NextLine(&p)) != NULL)
		{
			if (*line == '[')
			{
				if (insec)
					break;
				insec = (TX_Header(line, kind) == num);
			}
			else if (insec && *line != '#' && count < TX_MAXLINES)
			{
				lines[count++] = *line ? line : tx_blank;
			}
		}

		/* blank lines at the end of a section only separate it from the next one */
		while (count > 0 && lines[count - 1] == tx_blank)
			count--;
	}

	size = (count + 1) * sizeof(char *);
	for (i = 0; i < count; i++)
		size += TX_Len(lines[i]) + 1;

	tx_textblock = Z_Malloc((size + 7) & ~7, PU_STATIC, NULL);
	result = (char **)tx_textblock;
	dst = (char *)(result + count + 1);

	for (i = 0; i < count; i++)
	{
		result[i] = dst;
		TX_Copy(dst, lines[i]);
		dst += TX_Len(lines[i]) + 1;
	}
	result[count] = tx_empty;

	if (raw)
		Z_Free(raw);

	return result;
}

void TX_FreeText(void)
{
	if (tx_textblock)
	{
		Z_Free(tx_textblock);
		tx_textblock = NULL;
	}
}
