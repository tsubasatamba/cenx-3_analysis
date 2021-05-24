#ifndef BinaryParameters_H
#define BinaryParameters_H 1

#include "AstroUnits.hh"

namespace BinaryParameters
{
constexpr double mstar = 20.5*unit::msun;
constexpr double mstar_error = 0.7*unit::msun;
constexpr double rstar = 12.0*unit::rsun;

constexpr double mNS = 1.21*unit::msun;
constexpr double mNS_error = 0.21*unit::msun;

constexpr double distance = 5.7*unit::kpc;
constexpr double distance_error = 1.5*unit::kpc;

constexpr double luminosity = 5.0E37*unit::erg/unit::s;

constexpr double P0 = 2.08704106*unit::day;
constexpr double T0 = 50506.788423*unit::day;
constexpr double PdotToP = -1.800E-6/unit::year;

constexpr double inclination = 70.2*unit::degree;
constexpr double inclination_error = 2.7*unit::degree;
constexpr double projected_semimajor_axis = 39.6612*unit::lts;
constexpr double projected_semimajor_axis_error = 0.0009*unit::lts;

}

#endif
