#ifndef META_H
#define META_H

#include <RColor.h>
#include <climits>

namespace Meta {

inline int lifeGameRule(int tl, int tm, int tr, int ml, int mm, int mr, int bl, int bm, int br)
{
    int around = tl + tm + tr + ml + mr + bl + bm + br;

    switch (around)
    {
    case 0:
    case 1:
        return 0;
    case 2:
        return mm;
    case 3:
        return 1;
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        return 0;
    default:
        throw "Invalid value!";
    }
}

inline Redopera::RColor explicitDisplay(int value)
{
    if(value > 0)
        return Redopera::RColor(0xff0000ffu);
    else if(value < 0)
        return Redopera::RColor(0x0ffffu);
    else
        return Redopera::RColor(0x0u);
}

inline int exchangeRule(int tl, int tm, int tr, int ml, int mm, int mr, int bl, int bm, int br)
{
    mm /= 9;
    mm += tl / 9;
    mm += tm / 9;
    mm += tr / 9;
    mm += ml / 9;
    mm += mr / 9;
    mm += bl / 9;
    mm += bm / 9;
    mm += br / 9;

    return mm;
}

inline Redopera::RColor mappingDisplay(int value)
{
    // 正值映射至RGB 0xffff00红绿值，负值映射0xff蓝色值

    constexpr int range = 8191;

    double ratio = 255. / range;
    Redopera::RColor color;

    if(value > 0)
    {
        if(value < range)
        {
            color.setG(static_cast<unsigned>(ratio * value));
        }
        else if(value < range * 2)
        {
            color.setR(static_cast<unsigned>(ratio * (value - range)));
            color.setG(0xffu);
        }
        else if(value < range * 3)
        {
            color.setR(0xffu);
            color.setG(static_cast<unsigned>(255 - ratio * (value - range * 2)));
        }
        else if(value < range * 4)
        {
            color.setR(0xffu);
            color.setG(static_cast<unsigned>(ratio * (value - range * 3)));
            color.setB(color.g());
        }
        else
            color.setRGB(0xffffff);
    }
    else
    {
        if(value == 0)
            color.setRGB(0x0u);
        else if(value > -range)
            color.setB(static_cast<unsigned>(ratio * -value));
        else
            color.setRGB(0xff);
    }

    return color;
}

} // ns Meta

#endif // META_H
