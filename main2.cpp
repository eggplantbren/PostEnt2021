#include <iostream>
#include "Examples/PythonModel.h"
#include "NSRun.h"

using namespace PostEnt2021;
using namespace PostEnt2021::PythonModel;

int main()
{
    Tools::RNG rng;
    launch<PythonModelParams, PythonModelData>(rng);

    return 0;
}
