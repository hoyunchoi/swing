#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>

#include "Networks.hpp"

#include "Net_generator.hpp"
#include "Net_name.hpp"

int main(int argc, char* argv[]) {
    using namespace Swing;
    //* --------------------------------- Input -----------------------------------------

    //* Network parameters
    const std::string type = "ER";
    const unsigned size = 10;
    const double mean_degree = 4.0;
    const unsigned long long link_size = size * mean_degree / 2;
    const double degree_exponent = 3.0;

    //* Swing equation parameters
    const double friction = 0.5;
    const double coupling = 0.5;

    //* Iteration parameters
    const int random_engine_seed = -1;
    const unsigned max_iteration = 100;

    //* --------------------------------- Run -----------------------------------------
    const auto start = std::chrono::system_clock::now();

    //* Random engine for network
    pcg32 network_engine;
    random_engine_seed == -1 ? network_engine.seed((std::random_device())()) : network_engine.seed(random_engine_seed);

    //* Generate network
    Network<unsigned> network;
    if (type == "ER") {
        network = ER::generate(size, link_size, network_engine);
    } else if (type == "SF") {
        network = SF::generate(size, link_size, degree_exponent, network_engine);
    } else if (type == "CL") {
        network = CL::generate(size, link_size, degree_exponent, network_engine);
    } else {
        std::cout << "Input type is invalid\n";
        return 1;
    }

    //* Run RK4 swing equation
    Net_generator generator(network, friction, coupling, random_engine_seed);
    generator.run(max_iteration);

    //* --------------------------------- Save -----------------------------------------
    //* directory and file path
    const std::string directory = directory_name(type, size, link_size, degree_exponent);
    const std::string file = file_name(friction, coupling, max_iteration);

    //* Save the result
    generator.save(directory + file);

    //* --------------------------------- End -----------------------------------------
    std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;
    std::cout << sec.count() << std::endl;
}