#pragma once

#include <string>

#include "CSV.hpp"
#include "stringFormat.hpp"

namespace Swing {
const std::string root_directory = "data/";

const std::string directory_name(const std::string& t_type,
                                 const unsigned& t_size,
                                 const unsigned& t_link_size,
                                 const double& t_exponent = 0.0) {
    std::string directory = root_directory + t_type + "N" + std::to_string(t_size) + "L" + std::to_string(t_link_size);

    //* When scale-free network, include degree exponent to directory name
    if (t_type == "SF" || t_type == "CL") {
        directory += "E" + to_stringWithPrecision(t_exponent, 2);
    }

    CSV::generateDirectory(directory);
    return directory + "/";
}

const std::string file_name(const double& t_friction,
                            const double& t_coupling,
                            const unsigned& t_iteration,
                            const int& t_core_num = -1) {
    std::string file = "B" + to_stringWithPrecision(t_friction, 3) + "K" + to_stringWithPrecision(t_coupling, 3) + "I" + std::to_string(t_iteration);
    if (t_core_num >= 0) {
        file += "-" + std::to_string(t_core_num);
    }
    return file + ".dat";
}
} // namespace Swing