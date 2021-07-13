#include <cmath>
#include "DetectedEvent.hh"

void DetectedEvent::calculateOrbitalPhase()
{
  const double p0 = BinaryModulation::orbital_period;
  const double t0 = BinaryModulation::t0;
  const double t = ArrivalTime();
  const double dt = t-t0;

  const double phi = (dt/p0-std::floor(dt/p0))*unit::twopi;
  setOrbitalPhase(phi);
}

void DetectedEvent::calculateCorrectedTime()
{
  const double axsini = BinaryModulation::projected_semimajor_axis;
  const double t = ArrivalTime();
  const double phi = OrbitalPhase();
  const double tcorr = t-(axsini*std::cos(phi))/unit::c;
  setCorrectedTime(tcorr);
}

void DetectedEvent::process()
{
  calculateOrbitalPhase();
  calculateCorrectedTime();
}
