/* generated by multigen */

#define FF_FULLBRIGHT    0x8000    // flag in thing->frame
#define FF_FRAMEMASK     0x7fff

typedef enum {
    SPR_SPOT,
    SPR_PLAY,
    SPR_SARG,
    SPR_FATT,
    SPR_POSS,
    SPR_TROO,
    SPR_HEAD,
    SPR_BOSS,
    SPR_SKUL,
    SPR_BSPI,
    SPR_CYBR,
    SPR_PAIN,
    SPR_RECT,
    SPR_MISL,
    SPR_PLSS,
    SPR_BFS1,
    SPR_LASS,
    SPR_BAL1,
    SPR_BAL3,
    SPR_BAL2,
    SPR_BAL7,
    SPR_BAL8,
    SPR_APLS,
    SPR_MANF,
    SPR_TRCR,
    SPR_DART,
    SPR_FIRE,
    SPR_RBAL,
    SPR_PUF2,
    SPR_PUF3,
    SPR_PUFF,
    SPR_BLUD,
    SPR_A027,
    SPR_TFOG,
    SPR_BFE2,
    SPR_ARM1,
    SPR_ARM2,
    SPR_BON1,
    SPR_BON2,
    SPR_BKEY,
    SPR_RKEY,
    SPR_YKEY,
    SPR_YSKU,
    SPR_RSKU,
    SPR_BSKU,
    SPR_ART1,
    SPR_ART2,
    SPR_ART3,
    SPR_STIM,
    SPR_MEDI,
    SPR_SOUL,
    SPR_PINV,
    SPR_PSTR,
    SPR_PINS,
    SPR_SUIT,
    SPR_PMAP,
    SPR_PVIS,
    SPR_MEGA,
    SPR_CLIP,
    SPR_AMMO,
    SPR_RCKT,
    SPR_BROK,
    SPR_CELL,
    SPR_CELP,
    SPR_SHEL,
    SPR_SBOX,
    SPR_BPAK,
    SPR_BFUG,
    SPR_CSAW,
    SPR_MGUN,
    SPR_LAUN,
    SPR_PLSM,
    SPR_SHOT,
    SPR_SGN2,
    SPR_LSRG,
    SPR_CAND,
    SPR_BAR1,
    SPR_LMP1,
    SPR_LMP2,
    SPR_A031,
    SPR_A030,
    SPR_A032,
    SPR_A033,
    SPR_A034,
    SPR_BFLM,
    SPR_RFLM,
    SPR_YFLM,
    SPR_A006,
    SPR_A021,
    SPR_A003,
    SPR_A020,
    SPR_A014,
    SPR_A016,
    SPR_A008,
    SPR_A007,
    SPR_A015,
    SPR_A001,
    SPR_A012,
    SPR_A010,
    SPR_A018,
    SPR_A017,
    SPR_A026,
    SPR_A022,
    SPR_A028,
    SPR_A029,
    SPR_A035,
    SPR_A036,
    SPR_TRE3,
    SPR_TRE2,
    SPR_TRE1,
    SPR_A013,
    SPR_A019,
    SPR_A004,
    SPR_A005,
    SPR_A023,
    SPR_SAWG,
    SPR_PUNG,
    SPR_PISG,
    SPR_SHT1,
    SPR_SHT2,
    SPR_CHGG,
    SPR_ROCK,
    SPR_PLAS,
    SPR_BFGG,
    SPR_LASR,
    #if ENABLE_REMASTER_SPRITES == 1
    SPR_GBLD,
    SPR_WTIM,
    SPR_WEDI,
    SPR_WSTR,
    #endif
    NUMSPRITES
} spritenum_t;

typedef enum
{
	S_000,
    S_001,
    S_002,
    S_003,
    S_004,
    S_005,
    S_006,
    S_007,
    S_008,
    S_009,
    S_010,
    S_011,
    S_012,
    S_013,
    S_014,
    S_015,
    S_016,
    S_017,
    S_018,
    S_019,
    S_020,
    S_021,
    S_022,
    S_023,
    S_024,
    S_025,
    S_026,
    S_027,
    S_028,
    S_029,
    S_030,
    S_031,
    S_032,
    S_033,
    S_034,
    S_035,
    S_036,
    S_037,
    S_038,
    S_039,
    S_040,
    S_041,
    S_042,
    S_043,
    S_044,
    S_045,
    S_046,
    S_047,
    S_048,
    S_049,
    S_050,
    S_051,
    S_052,
    S_053,
    S_054,
    S_055,
    S_056,
    S_057,
    S_058,
    S_059,
    S_060,
    S_061,
    S_062,
    S_063,
    S_064,
    S_065,
    S_066,
    S_067,
    S_068,
    S_069,
    S_070,
    S_071,
    S_072,
    S_073,
    S_074,
    S_075,
    S_076,
    S_077,
    S_078,
    S_079,
    S_080,
    S_081,
    S_082,
    S_083,
    S_084,
    S_085,
    S_086,
    S_087,
    S_088,
    S_089,
    S_090,
    S_091,
    S_092,
    S_093,
    S_094,
    S_095,
    S_096,
    S_097,
    S_098,
    S_099,
    S_100,
    S_101,
    S_102,
    S_103,
    S_104,
    S_105,
    S_106,
    S_107,
    S_108,
    S_109,
    S_110,
    S_111,
    S_112,
    S_113,
    S_114,
    S_115,
    S_116,
    S_117,
    S_118,
    S_119,
    S_120,
    S_121,
    S_122,
    S_123,
    S_124,
    S_125,
    S_126,
    S_127,
    S_128,
    S_129,
    S_130,
    S_131,
    S_132,
    S_133,
    S_134,
    S_135,
    S_136,
    S_137,
    S_138,
    S_139,
    S_140,
    S_141,
    S_142,
    S_143,
    S_144,
    S_145,
    S_146,
    S_147,
    S_148,
    S_149,
    S_150,
    S_151,
    S_152,
    S_153,
    S_154,
    S_155,
    S_156,
    S_157,
    S_158,
    S_159,
    S_160,
    S_161,
    S_162,
    S_163,
    S_164,
    S_165,
    S_166,
    S_167,
    S_168,
    S_169,
    S_170,
    S_171,
    S_172,
    S_173,
    S_174,
    S_175,
    S_176,
    S_177,
    S_178,
    S_179,
    S_180,
    S_181,
    S_182,
    S_183,
    S_184,
    S_185,
    S_186,
    S_187,
    S_188,
    S_189,
    S_190,
    S_191,
    S_192,
    S_193,
    S_194,
    S_195,
    S_196,
    S_197,
    S_198,
    S_199,
    S_200,
    S_201,
    S_202,
    S_203,
    S_204,
    S_205,
    S_206,
    S_207,
    S_208,
    S_209,
    S_210,
    S_211,
    S_212,
    S_213,
    S_214,
    S_215,
    S_216,
    S_217,
    S_218,
    S_219,
    S_220,
    S_221,
    S_222,
    S_223,
    S_224,
    S_225,
    S_226,
    S_227,
    S_228,
    S_229,
    S_230,
    S_231,
    S_232,
    S_233,
    S_234,
    S_235,
    S_236,
    S_237,
    S_238,
    S_239,
    S_240,
    S_241,
    S_242,
    S_243,
    S_244,
    S_245,
    S_246,
    S_247,
    S_248,
    S_249,
    S_250,
    S_251,
    S_252,
    S_253,
    S_254,
    S_255,
    S_256,
    S_257,
    S_258,
    S_259,
    S_260,
    S_261,
    S_262,
    S_263,
    S_264,
    S_265,
    S_266,
    S_267,
    S_268,
    S_269,
    S_270,
    S_271,
    S_272,
    S_273,
    S_274,
    S_275,
    S_276,
    S_277,
    S_278,
    S_279,
    S_280,
    S_281,
    S_282,
    S_283,
    S_284,
    S_285,
    S_286,
    S_287,
    S_288,
    S_289,
    S_290,
    S_291,
    S_292,
    S_293,
    S_294,
    S_295,
    S_296,
    S_297,
    S_298,
    S_299,
    S_300,
    S_301,
    S_302,
    S_303,
    S_304,
    S_305,
    S_306,
    S_307,
    S_308,
    S_309,
    S_310,
    S_311,
    S_312,
    S_313,
    S_314,
    S_315,
    S_316,
    S_317,
    S_318,
    S_319,
    S_320,
    S_321,
    S_322,
    S_323,
    S_324,
    S_325,
    S_326,
    S_327,
    S_328,
    S_329,
    S_330,
    S_331,
    S_332,
    S_333,
    S_334,
    S_335,
    S_336,
    S_337,
    S_338,
    S_339,
    S_340,
    S_341,
    S_342,
    S_343,
    S_344,
    S_345,
    S_346,
    S_347,
    S_348,
    S_349,
    S_350,
    S_351,
    S_352,
    S_353,
    S_354,
    S_355,
    S_356,
    S_357,
    S_358,
    S_359,
    S_360,
    S_361,
    S_362,
    S_363,
    S_364,
    S_365,
    S_366,
    S_367,
    S_368,
    S_369,
    S_370,
    S_371,
    S_372,
    S_373,
    S_374,
    S_375,
    S_376,
    S_377,
    S_378,
    S_379,
    S_380,
    S_381,
    S_382,
    S_383,
    S_384,
    S_385,
    S_386,
    S_387,
    S_388,
    S_389,
    S_390,
    S_391,
    S_392,
    S_393,
    S_394,
    S_395,
    S_396,
    S_397,
    S_398,
    S_399,
    S_400,
    S_401,
    S_402,
    S_403,
    S_404,
    S_405,
    S_406,
    S_407,
    S_408,
    S_409,
    S_410,
    S_411,
    S_412,
    S_413,
    S_414,
    S_415,
    S_416,
    S_417,
    S_418,
    S_419,
    S_420,
    S_421,
    S_422,
    S_423,
    S_424,
    S_425,
    S_426,
    S_427,
    S_428,
    S_429,
    S_430,
    S_431,
    S_432,
    S_433,
    S_434,
    S_435,
    S_436,
    S_437,
    S_438,
    S_439,
    S_440,
    S_441,
    S_442,
    S_443,
    S_444,
    S_445,
    S_446,
    S_447,
    S_448,
    S_449,
    S_450,
    S_451,
    S_452,
    S_453,
    S_454,
    S_455,
    S_456,
    S_457,
    S_458,
    S_459,
    S_460,
    S_461,
    S_462,
    S_463,
    S_464,
    S_465,
    S_466,
    S_467,
    S_468,
    S_469,
    S_470,
    S_471,
    S_472,
    S_473,
    S_474,
    S_475,
    S_476,
    S_477,
    S_478,
    S_479,
    S_480,
    S_481,
    S_482,
    S_483,
    S_484,
    S_485,
    S_486,
    S_487,
    S_488,
    S_489,
    S_490,
    S_491,
    S_492,
    S_493,
    S_494,
    S_495,
    S_496,
    S_497,
    S_498,
    S_499,
    S_500,
    S_501,
    S_502,
    S_503,
    S_504,
    S_505,
    S_506,
    S_507,
    S_508,
    S_509,
    S_510,
    S_511,
    S_512,
    S_513,
    S_514,
    S_515,
    S_516,
    S_517,
    S_518,
    S_519,
    S_520,
    S_521,
    S_522,
    S_523,
    S_524,
    S_525,
    S_526,
    S_527,
    S_528,
    S_529,
    S_530,
    S_531,
    S_532,
    S_533,
    S_534,
    S_535,
    S_536,
    S_537,
    S_538,
    S_539,
    S_540,
    S_541,
    S_542,
    S_543,
    S_544,
    S_545,
    S_546,
    S_547,
    S_548,
    S_549,
    S_550,
    S_551,
    S_552,
    S_553,
    S_554,
    S_555,
    S_556,
    S_557,
    S_558,
    S_559,
    S_560,
    S_561,
    S_562,
    S_563,
    S_564,
    S_565,
    S_566,
    S_567,
    S_568,
    S_569,
    S_570,
    S_571,
    S_572,
    S_573,
    S_574,
    S_575,
    S_576,
    S_577,
    S_578,
    S_579,
    S_580,
    S_581,
    S_582,
    S_583,
    S_584,
    S_585,
    S_586,
    S_587,
    S_588,
    S_589,
    S_590,
    S_591,
    S_592,
    S_593,
    S_594,
    S_595,
    S_596,
    S_597,
    S_598,
    S_599,
    S_600,
    S_601,
    S_602,
    S_603,
    S_604,
    S_605,
    S_606,
    S_607,
    S_608,
    S_609,
    S_610,
    S_611,
    S_612,
    S_613,
    S_614,
    S_615,
    S_616,
    S_617,
    S_618,
    S_619,
    S_620,
    S_621,
    S_622,
    S_623,
    S_624,
    S_625,
    S_626,
    S_627,
    S_628,
    S_629,
    S_630,
    S_631,
    S_632,
    S_633,
    S_634,
    S_635,
    S_636,
    S_637,
    S_638,
    S_639,
    S_640,
    S_641,
    S_642,
    S_643,
    S_644,
    S_645,
    S_646,
    S_647,
    S_648,
    S_649,
    S_650,
    S_651,
    S_652,
    S_653,
    S_654,
    S_655,
    S_656,
    S_657,
    S_658,
    S_659,
    S_660,
    S_661,
    S_662,
    S_663,
    S_664,
    S_665,
    S_666,
    S_667,
    S_668,
    S_669,
    S_670,
    S_671,
    S_672,
    S_673,
    S_674,
    S_675,
    S_676,
    S_677,
    S_678,
    S_679,
    S_680,
    S_681,
    S_682,
    S_683,
    S_684,
    S_685,
    S_686,
    S_687,
    S_688,
    S_689,
    S_690,
    S_691,
    S_692,
    S_693,
    S_694,
    S_695,
    S_696,
    S_697,
    S_698,
    S_699,
    S_700,
    S_701,
    S_702,
    S_703,
    S_704,
    S_705,
    S_706,
    S_707,
    S_708,
    S_709,
    S_710,
    S_711,
    S_712,
    S_713,
    S_714,
    S_715,
    S_716,
    S_717,
    S_718,
    S_719,
    S_720,
    S_721,
    S_722,
    S_723,
    S_724,
    S_725,
    S_726,
    S_727,
    S_728,
    S_729,
    S_730,
    S_731,
    S_732,
    S_733,
    S_734,
    S_735,
    S_736,
    S_737,
    S_738,
    S_739,
    S_740,
    S_741,
    S_742,
    S_743,
    S_744,
    S_745,
    S_746,
    S_747,
    S_748,
    S_749,
    S_750,
    S_751,
    S_752,
    S_753,
    S_754,
    S_755,
    S_756,
    S_757,
    S_758,
    S_759,
    S_760,
    S_761,
    S_762,
    S_763,
    S_764,
    S_765,
    S_766,
    S_767,
    S_768,
    S_769,
    S_770,
    S_771,
    S_772,
    S_773,
    S_774,
    S_775,
    S_776,
    S_777,
    S_778,
    S_779,
    S_780,
    S_781,
    S_782,
    S_783,
    S_784,
    S_785,
    S_786,
    S_787,
    S_788,
    S_789,
    S_790,
    S_791,
    S_792,
    S_793,
    S_794,
    S_795,
    S_796,
    #if ENABLE_REMASTER_SPRITES == 1
    S_797,
    S_798,
    S_799,
    S_800,
    S_801,
    S_802,
    S_803,
    #endif
	NUMSTATES
} statenum_t;

typedef struct
{
	 spritenum_t	sprite;
	 long			frame;
	 long			tics;
	 void			(*action) ();
	 statenum_t		nextstate;
} state_t;

extern state_t	states[NUMSTATES];
extern char *sprnames[NUMSPRITES];

typedef enum
{
	MT_PLAYER,              // 0
    MT_PLAYERBOT1,          // 1
    MT_PLAYERBOT2,          // 2
    MT_PLAYERBOT3,          // 3
    MT_DEMON1,              // 4
    MT_DEMON2,              // 5
    MT_MANCUBUS,            // 6
    MT_POSSESSED1,          // 7
    MT_POSSESSED2,          // 8
    MT_IMP1,                // 9
    MT_IMP2,                // 10
    MT_CACODEMON,           // 11
    MT_BRUISER1,            // 12
    MT_BRUISER2,            // 13
    MT_SKULL,               // 14
    MT_BABY,                // 15
    MT_CYBORG,              // 16
    MT_CYBORG_TITLE,        // 17
    MT_PAIN,                // 18
    MT_RESURRECTOR,         // 19
    MT_CAMERA,              // 20
    MT_DEST_TELEPORT,       // 21
    MT_DEST_PROJECTILE,     // 22
    MT_FAKEITEM,            // 23
    MT_LASERMARKER,         // 24
    MT_PROJ_ROCKET,         // 25
    MT_PROJ_PLASMA,         // 26
    MT_PROJ_BFG,            // 27
    MT_PROJ_LASER,          // 28
    MT_PROJ_IMP1,           // 29
    MT_PROJ_IMP2,           // 30
    MT_PROJ_HEAD,           // 31
    MT_PROJ_BRUISER1,       // 32
    MT_PROJ_BRUISER2,       // 33
    MT_PROJ_BABY,           // 34
    MT_PROJ_FATSO,          // 35
    MT_PROJ_TRACER,         // 36
    MT_PROJ_DART,           // 37
    MT_PROJ_RECTFIRE,       // 38
    MT_PROJ_RECT,           // 39
    MT_SMOKE_GRAY,          // 40
    MT_SMOKE_RED,           // 41
    MT_SMOKE_SMALL,         // 42
    MT_BLOOD,               // 43
    MT_GIB_CRUSHED,         // 44
    MT_TELEPORTFOG,         // 45
    MT_BFGSPREAD,           // 46
    MT_ITEM_ARMOR1,         // 47
    MT_ITEM_ARMOR2,         // 48
    MT_ITEM_BONUSHEALTH,    // 49
    MT_ITEM_BONUSARMOR,     // 50
    MT_ITEM_BLUECARDKEY,    // 51
    MT_ITEM_REDCARDKEY,     // 52
    MT_ITEM_YELLOWCARDKEY,  // 53
    MT_ITEM_YELLOWSKULLKEY, // 54
    MT_ITEM_REDSKULLKEY,    // 55
    MT_ITEM_BLUESKULLKEY,   // 56
    MT_ITEM_ARTIFACT1,      // 57
    MT_ITEM_ARTIFACT2,      // 58
    MT_ITEM_ARTIFACT3,      // 59
    MT_ITEM_STIMPACK,       // 60
    MT_ITEM_MEDKIT,         // 61
    MT_ITEM_SOULSPHERE,     // 62
    MT_ITEM_INVULSPHERE,    // 63
    MT_ITEM_BERSERK,        // 64
    MT_ITEM_INVISSPHERE,    // 65
    MT_ITEM_RADSPHERE,      // 66
    MT_ITEM_AUTOMAP,        // 67
    MT_ITEM_PVIS,           // 68
    MT_ITEM_MEGASPHERE,     // 69
    MT_AMMO_CLIP,           // 70
    MT_AMMO_CLIPBOX,        // 71
    MT_AMMO_ROCKET,         // 72
    MT_AMMO_ROCKETBOX,      // 73
    MT_AMMO_CELL,           // 74
    MT_AMMO_CELLPACK,       // 75
    MT_AMMO_SHELL,          // 76
    MT_AMMO_SHELLBOX,       // 77
    MT_AMMO_BACKPACK,       // 78
    MT_WEAP_BFG,            // 79
    MT_WEAP_CHAINSAW,       // 80
    MT_WEAP_CHAINGUN,       // 81
    MT_WEAP_LAUNCHER,       // 82
    MT_WEAP_PLASMA,         // 83
    MT_WEAP_SHOTGUN,        // 84
    MT_WEAP_SSHOTGUN,       // 85
    MT_WEAP_LCARBINE,       // 86
    MT_PROP_FIRE,           // 87
    MT_PROP_CANDLE,         // 88
    MT_PROP_BARREL,         // 89
    MT_EXPLOSION1,          // 90
    MT_EXPLOSION2,          // 91
    MT_PROP_TECHLAMP1,      // 92
    MT_PROP_TECHLAMP2,      // 93
    MT_PROP_TORCHBLUE,      // 94
    MT_PROP_TORCHYELLOW,    // 95
    MT_PROP_TORCHRED,       // 96
    MT_PROP_POLEBASELONG,   // 97
    MT_PROP_POLEBASESHORT,  // 98
    MT_PROP_FIREBLUE,       // 99
    MT_PROP_FIRERED,        // 100
    MT_PROP_FIREYELLOW,     // 101
    MT_GIB_MEATSTICK,       // 102
    MT_GIB_MEATHANG,        // 103
    MT_GIB_TORSOHANG,       // 104
    MT_GIB_RIBFLOOR,        // 105
    MT_GIB_TWITCHFLOOR,     // 106
    MT_GIB_BLOODPOOL,       // 107
    MT_GIB_BONEFLOOR,       // 108
    MT_GIB_MEATRIBFLOOR,    // 109
    MT_GIB_MEATRIBCAGE,     // 110
    MT_GIB_CHAINHOOK,       // 111
    MT_GIB_HANGCAGE,        // 112
    MT_GIB_CHAINPINSER,     // 113
    MT_GIB_CHAINARM,        // 114
    MT_GIB_HANGMACE1,       // 115
    MT_GIB_HEADSTICK1,      // 116
    MT_GIB_HEADSTICK2,      // 117
    MT_GIB_DOUBLEMEATSTICK, // 118
    MT_PROP_STATUE1,        // 119
    MT_PROP_STATUE2,        // 120
    MT_PROP_TECHPOLELONG,   // 121
    MT_PROP_TECHPOLESHORT,  // 122
    MT_PROP_TREESTUMPSMALL, // 123
    MT_PROP_TREESTUMPLARGE, // 124
    MT_PROP_TREE,           // 125
    MT_PROP_BLOODYPOLE,     // 126
    MT_GIB_HANGMACE2,       // 127
    MT_GIB_HANGWHITEMEAT,   // 128
    MT_GIB_HANGHEAD,        // 129
    MT_GIB_HANGRIB,         // 130
	NUMMOBJTYPES
} mobjtype_t;

typedef struct {
	int	doomednum;
	int	spawnstate;
	int	spawnhealth;
	int	seestate;
	int	seesound;
	int	reactiontime;
	int	attacksound;
	int	painstate;
	int	painchance;
	int	painsound;
	int	meleestate;
	int	missilestate;
	int	deathstate;
	int	xdeathstate;
	int	deathsound;
	int	speed;
	int	radius;
	int	height;
	int	mass;
	int	damage;
	int	activesound;
	int	flags;
	int palette;
    int alpha;
} mobjinfo_t;

extern mobjinfo_t mobjinfo[NUMMOBJTYPES];

