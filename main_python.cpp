#include <iostream>
#include "Examples/PythonModel.h"
#include "NSRun.h"

using namespace PostEnt2021;
using namespace PostEnt2021::PythonModel;

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: ./main path/to/mymodel.py\n";
        return 1;
    }

    std::string model_path = argv[1];

    PostEnt2021::set_model_path(model_path);

    Tools::RNG rng;
    launch<PythonModelParams, PythonModelData>(rng);

    return 0;
}
