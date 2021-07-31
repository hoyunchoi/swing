#pragma once

namespace Swing {

struct Oscillator {
    //* Member variables
    double phase;
    double natural_frequency;
    double angular_velocity;

    //* Member functions
    Oscillator() {}
    Oscillator(const double&, const double&, const double&);
};

Oscillator::Oscillator(const double& t_natural_frequency,
                       const double& t_initial_phase,
                       const double& t_initial_angular_velocity)
    : phase(t_initial_phase),
      natural_frequency(t_natural_frequency),
      angular_velocity(t_initial_angular_velocity) {
}

} // namespace Swing
