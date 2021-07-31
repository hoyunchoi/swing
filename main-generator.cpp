#include <chrono>
#include <iostream>

#include "Generator.hpp"

int main(int argc, char* argv[]) {
    using namespace Swing;

    const unsigned size = 10;
    const double friction = 0.5;
    const double coupling = 0.5 / size;
    const int random_engine_seed = -1;
    const unsigned max_iteartion = 100;

    //* Generate and run Swing::Generator
    const auto start = std::chrono::system_clock::now();
    Generator generator(size, friction, coupling, random_engine_seed);
    generator.run(max_iteartion);
    generator.save();
    std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;
    std::cout << sec.count() << std::endl;

    return 0;
}