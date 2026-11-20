#define	SCREEN_HT	240
#define	SCREEN_WD	320

/* Size of the static audio heap (audio_heap.c). Used by S_Init for bounds checks. */
#define AUDIO_HEAP_SIZE	(0x44800) /* 280576: classic audio library needs more heap than n_audio did (was 0x39000) */
