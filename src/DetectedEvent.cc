#include <cmath>
#include "DetectedEvent.hh"

void DetectedEvent::calculateOrbitalPhase()
{
  const double p0 = BinaryParameters::P0;
  const double t0 = BinaryParameters::T0;
  const double a = BinaryParameters::PdotToP;
  const double t = ArrivalTime();
  const double dt = t-t0;
  
  const double dphi = (exp(a*dt)-1.0)/(a*p0);
  const double phi = (dphi-floor(dphi))*unit::twopi;
  setOrbitalPhase(phi);
}

void DetectedEvent::calculateCorrectedTime()
{
  const double axsini = BinaryParameters::projected_semimajor_axis;
  const double t = ArrivalTime();
  const double phi = OrbitalPhase();
  const double tcorr = t-(axsini*std::cos(phi))/unit::c;
  setCorrectedTime(tcorr);
}
