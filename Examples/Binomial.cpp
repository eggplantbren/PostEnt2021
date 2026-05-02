#include "Binomial.h"
#include <cmath>
#include "Tools/Misc.hpp"


namespace PostEnt2021
{

namespace Examples
{

BinomialParams::BinomialParams(Tools::RNG& rng)
:theta(rng.rand())
{

}

double BinomialParams::perturb(Tools::RNG& rng)
{
    theta += rng.randh();
    Tools::wrap(theta);
    return 0.0;
}

double BinomialParams::distance_from(const BinomialParams& other) const
{
    return std::abs(other.theta - theta);
}

void BinomialParams::print(std::ostream& out) const
{
    out << theta;
}

void BinomialParams::csv_header(std::ostream& out)
{
    out << "theta";
}

BinomialData::BinomialData(const BinomialParams& params, Tools::RNG& rng)
{
    x = 0;
    for(int i=0; i<N; ++i)
    {
        if(rng.rand() <= params.theta)
            ++x;
    }
}

double BinomialData::log_likelihood(const BinomialParams& params) const
{
    double logl = 0.0;
    logl += std::lgamma(N + 1) - std::lgamma(x + 1) - std::lgamma(N - x + 1);
    logl += x*log(params.theta) + (N - x)*log(1.0 - params.theta);
    return logl;
}


} // namespace Examples

} // namespace
