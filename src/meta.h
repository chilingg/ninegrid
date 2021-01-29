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

inline Redopera::RColor lifeGameDisplay(int value)
{
    return value ? Redopera::RColor(0xffffffffu) : Redopera::RColor(0x0u);
}

inline int exchangeRule(int tl, int tm, int tr, int ml, int mm, int mr, int bl, int bm, int br)
{
    mm = 0;
    mm += tl / 8;
    mm += tm / 8;
    mm += tr / 8;
    mm += ml / 8;
    mm += mr / 8;
    mm += bl / 8;
    mm += bm / 8;
    mm += br / 8;

    return mm;
}

inline Redopera::RColor exchangeDisplay(int value)
{
    value = INT_MAX / 8947848.0 * value; // 8947848 = 0x888888
    Redopera::RGBA rgb = 0x888888 + value;
    return Redopera::RColor(rgb << 8);
}

} // ns Meta

#endif // META_H
