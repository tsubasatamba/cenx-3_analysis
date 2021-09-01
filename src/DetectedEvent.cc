#include <cmath>
#include "DetectedEvent.hh"

double DetectedEvent::calculateOrbitalPhase(double t)
{
  const double p0 = BinaryModulation::orbital_period;
  const double t0 = BinaryModulation::t0;
  const double dt = t-t0;

  const double phi = (dt/p0-std::floor(dt/p0))*unit::twopi;
  return phi;
}

void DetectedEvent::calculateBinaryTime()
{
  const double axsini = BinaryModulation::projected_semimajor_axis;
  const double t = ArrivalTime();
  const double phi = calculateOrbitalPhase(t);
  const double tcorr = t-(axsini*std::cos(phi))/unit::c;
  setBinaryTime(tcorr);
}

void DetectedEvent::calculateArrivalTime()
{
  const double axsini = BinaryModulation::projected_semimajor_axis;
  const double t = BinaryTime();
  const double phi = calculateOrbitalPhase(t);
  const double tcorr = t+(axsini*std::cos(phi))/unit::c;
  setArrivalTime(tcorr);
}
