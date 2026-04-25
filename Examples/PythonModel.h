#ifndef PostEnt2021_Examples_PythonModel_h
#define PostEnt2021_Examples_PythonModel_h

#include "Tools/RNG.hpp"
#include <pybind11/embed.h>
#include <filesystem>
#include <pybind11/numpy.h>
#include <ostream>

namespace PostEnt2021
{

namespace PythonModel
{


class PythonModelParams
{
    private:
        static int size;
        std::vector<double> us, params;

    public:

        // Generate from the prior
        PythonModelParams(Tools::RNG& rng);

        // Perturb in the usual way
        double perturb(Tools::RNG& rng);

        // Convert us to params (just calls the Python prior_transform)
        void prior_transform();

        // Distance from another particle
        double distance_from(const PythonModelParams& other) const;

        // Print
        void print(std::ostream& out) const;
        static void csv_header(std::ostream& out);

        friend class PythonModelData;

};


class PythonModelData
{
    private:
        pybind11::object data;

    public:

        // Generate given the parameters
        PythonModelData(const PythonModelParams& params, Tools::RNG& rng);

        // Evaluate log likelihood
        double log_likelihood(const PythonModelParams& params) const;

};




} // namespace PythonModel

} // namespace

#endif
