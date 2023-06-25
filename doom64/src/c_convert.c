/* c_convert.c  */

/*-----------------------------------*/
/* Color Converter RGB2HSV & HSV2RGB */
/*-----------------------------------*/

#include "doomdef.h"

/*
===================
=
= LightGetHSV
= Set HSV values based on given RGB
=
===================
*/

int LightGetHSV(int r,int g,int b) // 800020BC
{
    unsigned char min, max;
	unsigned char h_, s_, v_;

    min = r < g ? (r < b ? r : b) : (g < b ? g : b);
    max = r > g ? (r > b ? r : b) : (g > b ? g : b);

	v_ = max;
    if (v_ == 0)
    {
        return 0;
    }
	
	s_ = (255 * (long)(max - min)) / v_;
	if (s_ == 0)
	{
        return v_ & 0x000000FF;
    }

    if (max == r)
    {
        h_ = 0 + 43 * (g - b) / (max - min);
    }
	else if (max == g)
    {
        h_ = 85 + 43 * (b - r) / (max - min);
    }
	else
    {
        h_ = 171 + 43 * (r - g) / (max - min);
    }
	
    return (((h_ << 16) | (s_ << 8) | v_) & 0x00FFFFFF);
}

/*
===================
=
= LightGetRGB
= Set RGB values based on given HSV
=
===================
*/

int LightGetRGB(int h,int s,int v) // 8000248C
{
    unsigned char r,g,b;
    unsigned char region, remainder, p, q, t;

    if (s == 0)
    {
        return (((v&0xff) << 16) | ((v&0xff) << 8) | (v&0xff)) & 0x00FFFFFF;
    }

    region = h / 43;
    remainder = (h - (region * 43)) * 6; 

    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region)
    {
        case 0:
        {
            r = v; g = t; b = p;
            break;
        }
        case 1:
        {
            r = q; g = v; b = p;
            break;
        }
        case 2:
        {
            r = p; g = v; b = t;
            break;
        }
        case 3:
        {
            r = p; g = q; b = v;
            break;
        }
        case 4:
        {
            r = t; g = p; b = v;
            break;
        }
        default:
        {
            r = v; g = p; b = q;
            break;
        }
    }

    return (((r&0xff) << 16) | ((g&0xff) << 8) | (b&0xff)) & 0x00FFFFFF;
}
