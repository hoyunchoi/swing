#include <chrono>
#include <iostream>
#include <filesystem>

#include "FC_name.hpp"
#include "FC_generator.hpp"

int main(int argc, char* argv[]) {
    using namespace Swing;

    const unsigned size = 10;
    const double friction = 0.5;
    const double coupling = 0.5 / size;
    const int random_engine_seed = -1;
    const unsigned max_iteartion = 100;

    //* Generate and run Swing::FC_enerator
    const auto start = std::chrono::system_clock::now();
    FC_generator generator(size, friction, coupling, random_engine_seed);
    generator.run(max_iteartion);

    //* Data directory
    const std::string directory = directory_name("FC", size);
    const std::string file = file_name(friction, coupling, max_iteartion);

    //* Save the result
    generator.save(directory + file);
    std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;
    std::cout << sec.count() << std::endl;

    return 0;
}