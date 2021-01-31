#include "model.h"
#include <RTimer.h>

#include <iostream>

using namespace Redopera;

int main()
{
    Model<int> model(4096, 4096, Meta::exchangeRule, Meta::mappingDisplay);

    RTimer timer;

    model.update();
    model.flush();

    std::cout << "Totle Time : " << timer.elapsed() << std::endl;

    return 0;
}
