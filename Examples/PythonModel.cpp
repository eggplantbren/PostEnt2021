#include "PythonModel.h"
#include <cmath>
#include "Tools/Misc.hpp"

namespace PostEnt2021
{

namespace PythonModel
{

void ensure_python_initialised()
{
    static auto *guard = new pybind11::scoped_interpreter{};  // never destroyed
    static bool initialised = false;

    if (!initialised) {
        pybind11::module_ sys = pybind11::module_::import("sys");
        std::filesystem::path cwd = std::filesystem::current_path();
        std::filesystem::path examples = cwd / "Examples";
        sys.attr("path").attr("insert")(0, examples.string());
        initialised = true;
    }
}




pybind11::module_& model_module() {
    ensure_python_initialised();
    static pybind11::module_ m = pybind11::module_::import("mymodel");
    return m;
}

pybind11::object& prior_transform_fn() {
    static pybind11::object fn = model_module().attr("prior_transform");
    return fn;
}

pybind11::object& log_likelihood_fn() {
    static pybind11::object fn = model_module().attr("log_likelihood");
    return fn;
}


pybind11::object& distance_fn() {
    static pybind11::object fn = model_module().attr("distance");
    return fn;
}

pybind11::object& generate_data_fn() {
    static pybind11::object fn = model_module().attr("generate_data");
    return fn;
}

int& num_params_ref() {
    static int n = model_module().attr("num_params").cast<int>();
    return n;
}

int PythonModelParams::size = 0;


PythonModelParams::PythonModelParams(Tools::RNG& rng)
{
    if (size == 0)
        size = num_params_ref();
    us.resize(size);
    params.resize(size);

    for(double& u: us)
        u = rng.rand();

    prior_transform();
}

double PythonModelParams::perturb(Tools::RNG& rng)
{
    int reps;
    if(rng.rand() <= 0.5)
        reps = 1;
    else
        reps = (int)pow(us.size(), rng.rand());

    for(int i=0; i<reps; ++i)
    {
        int k = rng.rand_int(us.size());
        us[k] += rng.randh();
        Tools::wrap(us[k], 0.0, 1.0);
    }

    prior_transform();

    return 0.0;
}

void PythonModelParams::print(std::ostream& out) const
{
    for(size_t i=0; i<params.size(); ++i)
    {
        out << params[i];
        if(i < (params.size()-1))
            out << ',';
    }
}

void PythonModelParams::csv_header(std::ostream& out)
{
    for(int i=0; i<size; ++i)
    {
        out << "params[" << i << "]";
        if(i < (size-1))
            out << ',';
    }
}



void PythonModelParams::prior_transform()
{
    pybind11::array_t<double> numpy_array(us.size(),
                                          us.data());
    pybind11::array_t<double> xs =
                prior_transform_fn()(numpy_array).cast<pybind11::array_t<double>>();

    auto buf = xs.request();
    double* ptr = static_cast<double*>(buf.ptr);
    std::copy(ptr, ptr + buf.size, params.begin());
}

double PythonModelParams::distance_from(const PythonModelParams& other) const
{
    pybind11::array_t<double> params1(params.size(), params.data());
    pybind11::array_t<double> params2(other.params.size(), other.params.data());

    double result = distance_fn()(params1, params2).cast<double>();
    return result;
}

PythonModelData::PythonModelData(const PythonModelParams& params, Tools::RNG& rng)
{
    pybind11::array_t<double> numpy_array(params.params.size(),
                                          params.params.data());
    data = generate_data_fn()(numpy_array);
}

double PythonModelData::log_likelihood(const PythonModelParams& params) const
{
    pybind11::array_t<double> numpy_array(params.params.size(),
                                          params.params.data());
    return log_likelihood_fn()(numpy_array, data).cast<double>();
}


} // namespace PythonModel

} // namespace
