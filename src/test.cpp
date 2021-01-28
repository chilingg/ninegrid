#include "model.h"

#include <RTimer.h>
#include <RDebug.h>

using namespace Redopera;

int main()
{
    Model<int, 4096> model;
    RTimer timer;

    timer.start();
    model.update();
    model.flush();
    rDebug << timer.elapsed();

	return 0;
}
