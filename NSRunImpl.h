#ifndef PostEnt2021_NSRunImpl_h
#define PostEnt2021_NSRunImpl_h

#include "Constants.h"
#include <fstream>

namespace PostEnt2021
{

template<typename Params, typename Data>
NSRun<Params, Data>::NSRun(int _ns_run_id, Tools::RNG& rng)
:ns_run_id(_ns_run_id)
,truth(rng)
,data(truth, rng)
,particles(Constants::num_particles, truth)
,log_likelihoods(Constants::num_particles, data.log_likelihood(truth))
,distances_from_truth(Constants::num_particles, 0.0)
,threshold(1E300)
,iteration(0)
{

}


template<typename Params, typename Data>
int NSRun<Params, Data>::explore_posterior(int which_particle,
                                           Tools::RNG& rng)
{
    const int& i = which_particle;
    int accepted = 0;
    for(int j=0; j<Constants::mcmc_steps_per_particle; ++j)
    {
        Params proposal = particles[i];
        double logh = proposal.perturb(rng);
        if(rng.rand() <= exp(logh))
        {
            double proposal_logl = data.log_likelihood(proposal);
            double proposal_dist = proposal.distance_from(truth);
            if(rng.rand() <= exp(proposal_logl - log_likelihoods[i])
                && proposal_dist < threshold)
            {
                particles[i] = proposal;
                log_likelihoods[i] = proposal_logl;
                distances_from_truth[i] = proposal_dist;
                ++accepted;
            }
        }
    }
    return accepted;
}

template<typename Params, typename Data>
void NSRun<Params, Data>::explore_posterior(Tools::RNG& rng)
{
    std::cout << "Initial exploration" << std::flush;

    int accepted = 0;
    static constexpr int factor = 10;
    for(size_t i=0; i<particles.size(); ++i)
    {
        for(int j=0; j<factor; ++j)
            accepted += explore_posterior(i, rng);
        std::cout << '.' << std::flush;
    }

    std::cout << "done.\nAcceptance rate = " << accepted << '/';
    std::cout << factor*Constants::num_particles*Constants::mcmc_steps_per_particle << '.';
    std::cout << '\n' << std::endl;
}

template<typename Params, typename Data>
void NSRun<Params, Data>::do_iteration(Tools::RNG& rng)
{
    ++iteration;

    // Find worst particle
    int worst = 0;
    for(size_t i=1; i<particles.size(); ++i)
    {
        if(distances_from_truth[i] > distances_from_truth[worst])
            worst = i;
    }
    threshold = distances_from_truth[worst];

    // Open output file
    auto mode = std::ios::out;
    if(ns_run_id > 1 || iteration > 1)
        mode = mode | std::ios::app;
    std::fstream fout("output.csv", mode);

    // Header
    if(ns_run_id == 1 && iteration == 1)
        fout << "ns_run_id,iteration,depth,distance" << std::endl;

    // Output line
    fout << ns_run_id << ',';
    fout << iteration << ',' << (double)(iteration)/particles.size() << ',';
    fout << distances_from_truth[worst] << std::endl;
    fout.close();

    std::cout << "(ns_run_id, iteration, depth, distance) = ";
    std::cout << '(' << ns_run_id << ", ";
    std::cout << iteration << ", " << (double)(iteration)/particles.size() << ", ";
    std::cout << distances_from_truth[worst] << ")." << std::endl;

    // Replace worst particle
    int copy;
    while(true)
    {
        copy = rng.rand_int(particles.size());
        if(particles.size() == 1 || copy != worst)
            break;
    }

    particles[worst] = particles[copy];
    log_likelihoods[worst] = log_likelihoods[copy];
    distances_from_truth[worst] = distances_from_truth[copy];

    // Refresh with MCMC
    std::cout << "Generating new particle..." << std::flush;
    int accepted = explore_posterior(worst, rng);
    std::cout << "done. Acceptance rate = " << accepted << '/';
    std::cout << Constants::mcmc_steps_per_particle << ".\n";
    std::cout << std::endl;
}


template<typename Params, typename Data>
void NSRun<Params, Data>::execute(Tools::RNG& rng)
{
    explore_posterior(rng);

    int iterations = Constants::target_depth_nats*Constants::num_particles;
    for(int i=0; i<iterations; ++i)
        do_iteration(rng);
}


template<typename Params, typename Data>
void launch(Tools::RNG& rng)
{
    for(int i=0; i<Constants::total_runs; ++i)
    {
        NSRun<Params, Data> ns_run(i+1, rng);
        ns_run.execute(rng);
    }
}

} // namespace

#endif
