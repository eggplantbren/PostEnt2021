#ifndef PostEnt2021_Constants_h
#define PostEnt2021_Constants_h

#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

// From copilot

namespace PostEnt2021 {
namespace Constants {

// --- tiny trim helper -------------------------------------------------------
inline std::string trim(const std::string& s) {
    auto start = s.find_first_not_of(" \t\r\n");
    auto end   = s.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    return s.substr(start, end - start + 1);
}

// --- class holding all constants --------------------------------------------
class Values {
public:
    // defaults
    static inline int num_particles = 10;
    static inline int mcmc_steps_per_particle = 5000;
    static inline double target_depth_nats = 20.0;
    static inline int total_runs = 1000;

private:
    // static initializer
    struct Loader {
        Loader() {
            load_from_file("OPTIONS");
        }

        void load_from_file(const std::string& path) {
            std::ifstream f(path);
            if (!f) return; // file missing → keep defaults

            std::string line;
            while (std::getline(f, line)) {
                // strip comments
                auto hash = line.find('#');
                if (hash != std::string::npos)
                    line = line.substr(0, hash);

                line = trim(line);
                if (line.empty()) continue;

                auto eq = line.find('=');
                if (eq == std::string::npos) continue;

                std::string key = trim(line.substr(0, eq));
                std::string val = trim(line.substr(eq + 1));

                if (key == "num_particles")
                    Values::num_particles = std::stoi(val);
                else if (key == "mcmc_steps_per_particle")
                    Values::mcmc_steps_per_particle = std::stoi(val);
                else if (key == "target_depth_nats")
                    Values::target_depth_nats = std::stod(val);
                else if (key == "total_runs")
                    Values::total_runs = std::stoi(val);
            }
        }
    };

    // static instance triggers loading at startup
    static inline Loader loader{};
};

// public aliases so old code still works
static inline int& num_particles = Values::num_particles;
static inline int& mcmc_steps_per_particle = Values::mcmc_steps_per_particle;
static inline double& target_depth_nats = Values::target_depth_nats;
static inline int& total_runs = Values::total_runs;

} // namespace Constants
} // namespace PostEnt2021

#endif

