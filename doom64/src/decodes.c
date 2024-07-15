/* decodes.c */

#include "doomdef.h"


/*=======*/
/* TYPES */
/*=======*/

typedef struct {
    int dec_bit_count;
    int dec_bit_buffer;
    byte* ostart;
    byte* output;
    byte* istart;
    byte* input;
} buffers_t;

/*=========*/
/* GLOBALS */
/*=========*/

static const int offsetTable[12] = { // 800B2250
    0, 16, 80, 336, 1360, 5456,
    15, 79, 335, 1359, 5455, 21839
};

#define WINDOW_SIZE 21902

typedef struct {
    short DecodeTable[2516]; // 800B22A8
    short array01[1258];     // 800B3660
    byte window[WINDOW_SIZE];      // 800B4054
} decoder_t;

static buffers_t buffers;       // 800B4034
static decoder_t *decoder = NULL;

/*
============================================================================

DECODE BASED ROUTINES

============================================================================
*/

void AllocDecodeBuffers(void)
{
    if (!decoder)
        decoder = Z_Malloc(sizeof(decoder_t), PU_STATIC, 0);
}

/*
========================
=
= ReadByte -> Old GetDecodeByte
=
========================
*/

static int ReadByte(void) // 8002D1D0
{
    return *buffers.input++;
}

/*
========================
=
= WriteByte -> Old WriteOutput
=
========================
*/

static void WriteByte(byte outByte) // 8002D214
{
    *buffers.output++ = outByte;
}

/*
========================
=
= ReadBinary -> old DecodeScan
=
========================
*/

static int ReadBinary(void) // 8002D2F4
{
    int resultbyte;

    resultbyte = buffers.dec_bit_count;

    buffers.dec_bit_count = (resultbyte - 1);
    if ((resultbyte < 1))
    {
        resultbyte = ReadByte();

        buffers.dec_bit_buffer = resultbyte;
        buffers.dec_bit_count = 7;
    }

    resultbyte = (0 < (buffers.dec_bit_buffer & 0x80));
    buffers.dec_bit_buffer = (buffers.dec_bit_buffer << 1);

    return resultbyte;
}

/*
========================
=
= ReadCodeBinary -> old RescanByte
=
========================
*/

static int ReadCodeBinary(int byte) // 8002D3B8
{
    int shift;
    int i;
    int resultbyte;

    resultbyte = 0;
    i = 0;
    shift = 1;

    if (byte <= 0)
        return resultbyte;

    do
    {
        if (ReadBinary() != 0)
            resultbyte |= shift;

        i++;
        shift = (shift << 1);
    } while (i != byte);

    return resultbyte;
}

/*
========================
=
= InitTables -> old InitDecodeTable
=
========================
*/

static void InitTables(void) // 8002D468
{
    int evenVal, oddVal, incrVal;

    short* curArray;
    short* incrTbl;
    short* evenTbl;
    short* oddTbl;

    buffers.dec_bit_count = 0;
    buffers.dec_bit_buffer = 0;

    curArray = &decoder->array01[(0 + 2)];
    incrTbl = &decoder->DecodeTable[(1258 + 2)];

    incrVal = 2;

    do {
        *incrTbl++ = (short)(incrVal / 2);
        *curArray++ = 1;
    } while (++incrVal < 1258);

    oddTbl = &decoder->DecodeTable[(629 + 1)];
    evenTbl = &decoder->DecodeTable[(0 + 1)];

    evenVal = 1;
    oddVal = 3;

    do
    {
        *oddTbl++ = (short)oddVal;
        oddVal += 2;

        *evenTbl++ = (short)(evenVal * 2);
        evenVal++;
    } while (evenVal < 629);
}

/*
========================
=
= CheckTable
=
========================
*/

static void CheckTable(int a0, int a1) // 8002D624
{
    int i;
    int idByte1;
    int idByte2;
    short* curArray;
    short* evenTbl;
    short* oddTbl;
    short* incrTbl;
    short *array01;

    i = 0;
    evenTbl = &decoder->DecodeTable[0];
    oddTbl  = &decoder->DecodeTable[629];
    incrTbl = &decoder->DecodeTable[1258];
    array01 = decoder->array01;

    idByte1 = a0;

    do {
        idByte2 = incrTbl[idByte1];

        array01[idByte2] = (array01[a1] + array01[a0]);

        a0 = idByte2;

        if (idByte2 != 1) {
            idByte1 = incrTbl[idByte2];
            idByte2 = evenTbl[idByte1];

            a1 = idByte2;

            if (a0 == idByte2) {
                a1 = oddTbl[idByte1];
            }
        }

        idByte1 = a0;
    } while (a0 != 1);

    if (array01[1] != 0x7D0) {
        return;
    }

    array01[1] >>= 1;

    curArray = &array01[2];
    do
    {
        curArray[3] >>= 1;
        curArray[2] >>= 1;
        curArray[1] >>= 1;
        curArray[0] >>= 1;
        curArray += 4;
        i += 4;
    } while (i != 1256);
}

/*
========================
=
= UpdateTables -> old DecodeByte
=
========================
*/

static void UpdateTables(int tblpos) // 8002D72C
{
    int incrIdx;
    int evenVal;
    int idByte1;
    int idByte2;
    int idByte3;
    int idByte4;

    short* evenTbl;
    short* oddTbl;
    short* incrTbl;
    short* tmpIncrTbl;
    short *array01;

    evenTbl = &decoder->DecodeTable[0];
    oddTbl  = &decoder->DecodeTable[629];
    incrTbl = &decoder->DecodeTable[1258];
    array01 = decoder->array01;

    idByte1 = (tblpos + 0x275);
    array01[idByte1] += 1;

    if (incrTbl[idByte1] != 1)
    {
        tmpIncrTbl = &incrTbl[idByte1];
        idByte2 = *tmpIncrTbl;

        if (idByte1 == evenTbl[idByte2]) {
            CheckTable(idByte1, oddTbl[idByte2]);
        }
        else {
            CheckTable(idByte1, evenTbl[idByte2]);
        }

        do
        {
            incrIdx = incrTbl[idByte2];
            evenVal = evenTbl[incrIdx];

            if (idByte2 == evenVal) {
                idByte3 = oddTbl[incrIdx];
            }
            else {
                idByte3 = evenVal;
            }

            if (array01[idByte3] < array01[idByte1])
            {
                if (idByte2 == evenVal) {
                    oddTbl[incrIdx] = (short)idByte1;
                }
                else {
                    evenTbl[incrIdx] = (short)idByte1;
                }

                evenVal = evenTbl[idByte2];

                if (idByte1 == evenVal) {
                    idByte4 = oddTbl[idByte2];
                    evenTbl[idByte2] = (short)idByte3;
                }
                else {
                    idByte4 = evenVal;
                    oddTbl[idByte2] = (short)idByte3;
                }

                incrTbl[idByte3] = (short)idByte2;

                *tmpIncrTbl = (short)incrIdx;
                CheckTable(idByte3, idByte4);

                tmpIncrTbl = &incrTbl[idByte3];
            }

            idByte1 = *tmpIncrTbl;
            tmpIncrTbl = &incrTbl[idByte1];

            idByte2 = *tmpIncrTbl;
        } while (idByte2 != 1);
    }
}

/*
========================
=
= StartDecodeByte
=
========================
*/

static int StartDecodeByte(void) // 8002D904
{
    int lookup;
    short* evenTbl;
    short* oddTbl;

    lookup = 1;

    evenTbl = &decoder->DecodeTable[0];
    oddTbl  = &decoder->DecodeTable[629];

    while (lookup < 0x275) {
        if (ReadBinary() == 0) {
            lookup = evenTbl[lookup];
        }
        else {
            lookup = oddTbl[lookup];
        }
    }

    lookup = (lookup + -0x275);
    UpdateTables(lookup);

    return lookup;
}

/*
========================
=
= DecodeD64
=
= Exclusive Doom 64
=
========================
*/

void DecodeD64(unsigned char* input, unsigned char* output) // 8002DFA0
{
    int copyPos, storePos;
    int dec_byte, resc_byte;
    int incrBit, copyCnt, j;
    unsigned shiftPos;
    byte *window;

    //PRINTF_D2(WHITE, 0, 15, "DecodeD64");

    InitTables();

    incrBit = 0;
    window = decoder->window;

    buffers.input = buffers.istart = input;
    buffers.output = buffers.ostart = output;

    // decodewindow = (byte *)Z_Alloc(WINDOW_SIZE, PU_STATIC, NULL);

    dec_byte = StartDecodeByte();

    while (dec_byte != 256)
    {
        if (dec_byte < 256)
        {
            /*  Decode the data directly using binary data code */

            WriteByte((byte)(dec_byte & 0xff));
            window[incrBit] = (byte)dec_byte;

            /*  Resets the count once the memory limit is exceeded in allocPtr,
                so to speak resets it at startup for reuse */
            incrBit += 1;
            if (incrBit == WINDOW_SIZE) {
                incrBit = 0;
            }
        }
        else
        {
            /*  Decode the data using binary data code,
                a count is obtained for the repeated data,
                positioning itself in the root that is being stored in allocPtr previously. */

            /*  A number is obtained from a range from 0 to 5,
                necessary to obtain a shift value in the ShiftTable*/
            shiftPos = (dec_byte + -257) / 62;

            /*  Get a count number for data to copy */
            copyCnt = (dec_byte - (shiftPos * 62)) + -254;

            /*  To start copying data, you receive a position number
                that you must sum with the position of table tableVar01 */
            resc_byte = ReadCodeBinary((shiftPos<<1)+4);

            /*  with this formula the exact position is obtained
                to start copying previously stored data */
            copyPos = incrBit - ((offsetTable[shiftPos] + resc_byte) + copyCnt);

            if (copyPos < 0) {
                copyPos += WINDOW_SIZE;
            }

            storePos = incrBit;

            for (j = 0; j < copyCnt; j++)
            {
                /* write the copied data */
                WriteByte(window[copyPos]);

                /* save copied data at current position in memory allocPtr */
                window[storePos] = window[copyPos];

                storePos++; /* advance to next allocPtr memory block to store */
                copyPos++;  /* advance to next allocPtr memory block to copy */

                /* reset the position of storePos once the memory limit is exceeded */
                if (storePos == WINDOW_SIZE) {
                    storePos = 0;
                }

                /* reset the position of copyPos once the memory limit is exceeded */
                if (copyPos == WINDOW_SIZE) {
                    copyPos = 0;
                }
            }

            /*  Resets the count once the memory limit is exceeded in allocPtr,
                so to speak resets it at startup for reuse */
            incrBit += copyCnt;
            if (incrBit >= WINDOW_SIZE) {
                incrBit -= WINDOW_SIZE;
            }
        }

        dec_byte = StartDecodeByte();
    }

    //PRINTF_D2(WHITE, 0, 21, "DecodeD64:End");
}

/*
== == == == == == == == == ==
=
= DecodeJaguar (decode original name)
=
= Exclusive Psx Doom / Doom 64 from Jaguar Doom
=
== == == == == == == == == ==
*/

#define LOOKAHEAD_SIZE  16

#define LENSHIFT 4      /* this must be log2(LOOKAHEAD_SIZE) */

void DecodeJaguar(unsigned char *input, unsigned char *output) // 8002E1f4
{
    int getidbyte = 0;
    int len;
    int pos;
    int i;
    unsigned char *source;
    int idbyte = 0;

    while (1)
    {
        /* get a new idbyte if necessary */
        if (!getidbyte) idbyte = *input++;
        getidbyte = (getidbyte + 1) & 7;

        if (idbyte & 1)
        {
            /* decompress */
            pos = *input++ << LENSHIFT;
            pos = pos | (*input >> LENSHIFT);
            source = output - pos - 1;
            len = (*input++ & 0xf) + 1;
            if (len == 1) break;

            //for (i = 0; i<len; i++)
                //*output++ = *source++;

            i = 0;
            if (len > 0)
            {
                if ((len & 3))
                {
                    while(i != (len & 3))
                    {
                        *output++ = *source++;
                        i++;
                    }
                }
                while(i != len)
                {
                    output[0] = source[0];
                    output[1] = source[1];
                    output[2] = source[2];
                    output[3] = source[3];
                    output += 4;
                    source += 4;
                    i += 4;
                }
            }
        }
        else
        {
            *output++ = *input++;
        }

        idbyte = idbyte >> 1;
    }
}