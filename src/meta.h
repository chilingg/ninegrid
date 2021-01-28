#ifndef META_H
#define META_H

namespace Meta {

int lifeGameRule(int tl, int tm, int tr, int ml, int mm, int mr, int bl, int bm, int br)
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

}

#endif // META_H
