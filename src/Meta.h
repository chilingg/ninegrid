#ifndef META_H
#define META_H

#include <RColor.h>

namespace Meta {

// Rule function

int lifeGameRule(int tl, int tm, int tr, int ml, int mm, int mr, int bl, int bm, int br);

int exchangeRule(int tl, int tm, int tr, int ml, int mm, int mr, int bl, int bm, int br);

struct MUnit{
    MUnit():atrri(0u), value(0) {}
    MUnit(int32_t n) { atrri = (n & 0xf) % 9; value = n >> 16; }

    unsigned direct() const { return atrri & 0xf; }

    uint16_t atrri;
    int16_t value;
};

MUnit motionRule(MUnit tl, MUnit tm, MUnit tr, MUnit ml, MUnit mm, MUnit mr, MUnit bl, MUnit bm, MUnit br);


// Display function

Redopera::RColor explicitDisplay(int value);

Redopera::RColor mappingDisplay(int value);

Redopera::RColor munitDisplay(MUnit value);
Redopera::RColor munitExplicitDisplay(MUnit value);

} // ns Meta

#endif // META_H
