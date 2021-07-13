#include "AstroUnits.hh"

namespace unit
{
double mjd_to_event_time(double mjd)
{
  return mjd-unit::MJDREF;
}

double event_time_to_mjd(double event_time)
{
  return event_time+unit::MJDREF;
}
}
