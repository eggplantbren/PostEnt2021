#ifndef PostEnt2021_Examples_Binomial_h
#define PostEnt2021_Examples_Binomial_h

#include "Tools/RNG.hpp"
#include <ostream>

namespace PostEnt2021
{

namespace Examples
{

class BinomialParams
{
    private:
        double theta;

    public:

        // Generate from the prior
        BinomialParams(Tools::RNG& rng);

        // Perturb in the usual way
        double perturb(Tools::RNG& rng);

        // Distance from another particle
        double distance_from(const BinomialParams& other) const;

        // Print
        void print(std::ostream& out) const;
        static void csv_header(std::ostream& out);

        friend class BinomialData;

};


class BinomialData
{
    private:
        static constexpr int N = 100;
        int x;

    public:

        // Generate given the parameters
        BinomialData(const BinomialParams& params, Tools::RNG& rng);

        // Evaluate log likelihood
        double log_likelihood(const BinomialParams& params) const;

};



} // namespace Examples

} // namespace

#endif
