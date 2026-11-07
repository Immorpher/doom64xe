/* bench.c - demo benchmark (build with BENCHMARK=1)
 *
 * Plays the attract-mode demos back to back, times every frame, then shows
 * the results on a CPU-drawn screen and stops. The demos are recorded input,
 * so every run simulates exactly the same frames and the numbers can be
 * compared between builds. The CK value is a checksum of the final game
 * state of each demo: if it changes between two builds, the game plays
 * differently (an optimization bug), whatever the speed.
 *
 * Times are microseconds per frame (average over the demo):
 *   T  game logic (P_Ticker)            - CPU
 *   B  building the frame (P_Drawer)    - CPU, up to handing it to the RCP
 *   G  graphics task, start to finish   - RSP+RDP wall time (includes the
 *                                         time audio takes the RSP from it)
 *   MC slowest frame of T+B, MG slowest G
 *   S  frames (T+B) plus graphics tasks (G) that took over 2 VIs (33366 us)
 */
#ifdef BENCHMARK

#include "doomdef.h"
#include "p_local.h"

extern int D_RunDemo(char *name, skill_t skill, int map);
extern int rndindex;
extern int prndindex;
extern int DefaultConfiguration[6][13];

#define BENCH_DEMOS 5   /* title map + DEMO1..DEMO4 */
#define BENCH_SLOW_US 33366

volatile u32 bench_gfx_start;
volatile u32 bench_submit;

typedef struct
{
    u32 frames;
    u32 tick, build, gfx;      /* sums, microseconds */
    u32 gfxtasks;              /* number of graphics tasks timed */
    u32 maxcpu, maxgfx;
    u32 slow;
    u32 check;
    u32 started;
} bench_t;

static bench_t bench[BENCH_DEMOS];
static int bench_cur;

/* 46.875 MHz CPU counter -> microseconds */
static u32 Bench_US(u32 counts)
{
    return (counts * 64) / 3000;
}

void Bench_Frame(u32 tick_counts, u32 build_counts)
{
    bench_t *b = &bench[bench_cur];
    u32 t = Bench_US(tick_counts);
    u32 bl = Bench_US(build_counts);
    u32 cpu = t + bl;

    /* The first frame of a run has no valid graphics start stamp (it comes
       straight after the level load), so leave it out. */
    if (!b->started)
    {
        b->started = 1;
        return;
    }

    b->frames++;
    b->tick += t;
    b->build += bl;
    if (cpu > b->maxcpu) b->maxcpu = cpu;
    if (cpu > BENCH_SLOW_US) b->slow++;
}

/* Called from the ticker thread when the RCP finishes a graphics task, with
   the time since that task started. Timing it here, on the same thread that
   stamped the start, avoids racing the main thread. */
void Bench_GfxDone(u32 counts)
{
    bench_t *b = &bench[bench_cur < BENCH_DEMOS ? bench_cur : BENCH_DEMOS - 1];
    u32 g;

    if (counts > 46875000) /* over a second: not a real frame */
        return;
    g = Bench_US(counts);
    b->gfx += g;
    b->gfxtasks++;
    if (g > b->maxgfx) b->maxgfx = g;
    if (g > BENCH_SLOW_US) b->slow++;
}

/* called when a demo ends, before the level is torn down */
void Bench_EndDemo(void)
{
    bench_t *b = &bench[bench_cur];
    u32 h = 17;
    mobj_t *mo = players[0].mo;

    if (mo)
    {
        h = h * 31 + (u32)mo->x;
        h = h * 31 + (u32)mo->y;
        h = h * 31 + (u32)mo->z;
        h = h * 31 + (u32)mo->angle;
        h = h * 31 + (u32)mo->health;
    }
    h = h * 31 + (u32)players[0].health;
    h = h * 31 + (u32)prndindex;
    h = h * 31 + (u32)rndindex;
    h = h * 31 + (u32)gametic;
    b->check = h;
}

/* ---- CPU-drawn results screen (5x7 font at 2x) ------------------------- */
static const char bn_chars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ:-.";
static const u8 bn_font[][7] = {
    {0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E}, {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E},
    {0x0E, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1F}, {0x0E, 0x11, 0x01, 0x06, 0x01, 0x11, 0x0E},
    {0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02}, {0x1F, 0x10, 0x1E, 0x01, 0x01, 0x11, 0x0E},
    {0x06, 0x08, 0x10, 0x1E, 0x11, 0x11, 0x0E}, {0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08},
    {0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E}, {0x0E, 0x11, 0x11, 0x0F, 0x01, 0x02, 0x0C},
    {0x0E, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11}, {0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E},
    {0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E}, {0x1C, 0x12, 0x11, 0x11, 0x11, 0x12, 0x1C},
    {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F}, {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10},
    {0x0E, 0x11, 0x10, 0x17, 0x11, 0x11, 0x0F}, {0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11},
    {0x0E, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0E}, {0x07, 0x02, 0x02, 0x02, 0x02, 0x12, 0x0C},
    {0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11}, {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F},
    {0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11}, {0x11, 0x19, 0x15, 0x13, 0x11, 0x11, 0x11},
    {0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E}, {0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10},
    {0x0E, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0D}, {0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11},
    {0x0F, 0x10, 0x10, 0x0E, 0x01, 0x01, 0x1E}, {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04},
    {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E}, {0x11, 0x11, 0x11, 0x11, 0x11, 0x0A, 0x04},
    {0x11, 0x11, 0x11, 0x15, 0x15, 0x1B, 0x11}, {0x11, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x11},
    {0x11, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x04}, {0x1F, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1F},
    {0x00, 0x04, 0x04, 0x00, 0x04, 0x04, 0x00}, {0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C},
};

static void Bench_Text(u32 *fb, const char *s, int x, int y)
{
    int k, row, col, i, j;
    char c;
    u8 bits;

    for (; *s; s++, x += 12)
    {
        c = *s;
        for (k = 0; bn_chars[k]; k++)
            if (bn_chars[k] == c)
                break;
        if (!bn_chars[k] || x + 10 > SCREEN_WD || y + 14 > SCREEN_HT)
            continue;
        for (row = 0; row < 7; row++)
        {
            bits = bn_font[k][row];
            for (col = 0; col < 5; col++)
                if (bits & (0x10 >> col))
                    for (j = 0; j < 2; j++)
                        for (i = 0; i < 2; i++)
                            fb[(y + row*2 + j)*SCREEN_WD + (x + col*2 + i)] = 0xFFFFFFFF;
        }
    }
}

static void Bench_Show(void)
{
    static const char *labels[BENCH_DEMOS] = { "TI", "D1", "D2", "D3", "D4" };
    char line[48];
    int info[8];
    u32 *fb = (u32 *)cfb[0];
    bench_t all;
    int d, i, y = 2;

    for (i = 0; i < SCREEN_WD*SCREEN_HT; i++)
        fb[i] = 0x000020FF;

    D_memset(&all, 0, sizeof(all));
    for (d = 0; d < BENCH_DEMOS; d++)
    {
        bench_t *b = &bench[d];
        u32 n = b->frames ? b->frames : 1;
        u32 ng = b->gfxtasks ? b->gfxtasks : 1;

        info[0] = (int)labels[d]; info[1] = b->frames; info[2] = b->slow; info[3] = b->check;
        D_vsprintf(line, "%s N%d S%d CK%08X", info);
        Bench_Text(fb, line, 8, y); y += 16;

        info[0] = b->tick / n; info[1] = b->build / n; info[2] = b->gfx / ng;
        D_vsprintf(line, " T%d B%d G%d", info);
        Bench_Text(fb, line, 8, y); y += 16;

        all.frames += b->frames;
        all.tick += b->tick; all.build += b->build; all.gfx += b->gfx;
        all.gfxtasks += b->gfxtasks;
        all.slow += b->slow;
        if (b->maxcpu > all.maxcpu) all.maxcpu = b->maxcpu;
        if (b->maxgfx > all.maxgfx) all.maxgfx = b->maxgfx;
    }

    {
        u32 n = all.frames ? all.frames : 1;

        info[0] = all.frames; info[1] = all.slow;
        D_vsprintf(line, "ALL N%d S%d", info);
        Bench_Text(fb, line, 8, y); y += 16;

        info[0] = all.tick / n; info[1] = all.build / n; info[2] = all.gfx / (all.gfxtasks ? all.gfxtasks : 1);
        D_vsprintf(line, " T%d B%d G%d", info);
        Bench_Text(fb, line, 8, y); y += 16;

        info[0] = all.maxcpu; info[1] = all.maxgfx;
        D_vsprintf(line, " MC%d MG%d", info);
        Bench_Text(fb, line, 8, y);
    }

    osWritebackDCacheAll();
    osViBlack(FALSE);
    osViSwapBuffer(fb);
}

/* The title screen's camera fly-through (map 33), the same way D_TitleMap
   runs it but without the Memory Pak prompt. */
static void Bench_Title(void)
{
    demo_p = Z_Alloc(16000, PU_STATIC, NULL);
    bzero(demo_p, 16000);
    D_memcpy(demo_p, DefaultConfiguration[0], 13*sizeof(int));
    G_PlayDemoPtr(sk_medium, 33);
    Z_Free(demo_p);
}

void Bench_Run(void)
{
    static char *names[BENCH_DEMOS] = { NULL, "DEMO1LMP", "DEMO2LMP", "DEMO3LMP", "DEMO4LMP" };
    static const int maps[BENCH_DEMOS] = { 33, 3, 9, 17, 32 };

    for (bench_cur = 0; bench_cur < BENCH_DEMOS; bench_cur++)
    {
        if (names[bench_cur])
            D_RunDemo(names[bench_cur], sk_medium, maps[bench_cur]);
        else
            Bench_Title();
    }

    Bench_Show();
    for (;;)
        osYieldThread();
}

#endif /* BENCHMARK */
