#ifndef ANLGEANT4_AstroUnits_H
#define ANLGEANT4_AstroUnits_H 1

#include <iostream>
#include <math.h>

namespace unit
{
// length
constexpr double cm = 1.0;
constexpr double m = 100.0*cm;
constexpr double km = 1000.0*m;
constexpr double astronomical_unit = 149597870.700 * km;
constexpr double AU = astronomical_unit;
constexpr double solar_radius = 6.955E+08 * m;
constexpr double rsun = solar_radius;
constexpr double pc = 3.085677581E16*m;
constexpr double kpc = 1000.0*pc;

// angle
constexpr double radian = 1.0;
constexpr double degree = M_PI/180.0;
constexpr double arcminute = degree/60.0;
constexpr double arcsecond = degree/3600.0;

// mass
constexpr double g = 1.0;
constexpr double mg = 1.0E-3*g;
constexpr double kg = 1000.0*g;
constexpr double msun = 1.98892E+30*kg;

// time
constexpr double s = 1.0;
constexpr double ks = 1000.0 * s;
constexpr double hour = 3600.0 * s;
constexpr double day = 24.0 * hour;
constexpr double year = 365.25 * day;

// energy
constexpr double erg = 1.0;
constexpr double J = erg*1.0E+7;
constexpr double eV = 1.602176634E-19;
constexpr double keV = 1000.0*eV;
constexpr double MeV = 1000.0*keV;

// math
constexpr double pi = M_PI;
constexpr double twopi = 2.0*pi;

// physcal
constexpr double G = 6.67430E-8;
constexpr double c = 2.99792450*m/s;
constexpr double lts = c*s;
constexpr double lty = c*year;

//event time
constexpr double MJDREFI = 55197.0*day;
constexpr double MJDREFF = 7.660185200000000E-04*s;
constexpr double MJDREF = MJDREFI+MJDREFF;
double mjd_to_event_time(double mjd);
double event_time_to_mjd(double event_time);
}

#endif
