#include <cmath>
#include "LightCurve.hh"
#include "AstroUnits.hh"
#include "DetectedEvent.hh"

namespace cfitsio
{
extern "C" {
#include "fitsio.h"
}
}

namespace
{
  void checkFitsStatus(int fits_status);
}

void LightCurve::readFits(const std::string& filename)
{
  cfitsio::fitsfile* fitsfile = nullptr;
  int fits_status = 0;
  std::string colname = "TIME";
  long naxes[2] = {0, 0};
  int nfound = 0;
  short shortnul = 0;
  int anynul = 0;
  
  cfitsio::fits_open_file(&fitsfile, filename.c_str(), READONLY, &fits_status);
  checkFitsStatus(fits_status);

  cfitsio::fits_read_key(fitsfile, TDOUBLE, (char*)"MJD-OBS", &initialTime_, nullptr, &fits_status);
  checkFitsStatus(fits_status);
  initialTime_ *= unit::day;

  cfitsio::ffmahd(fitsfile, 2, IMAGE_HDU, &fits_status);
  checkFitsStatus(fits_status);

  cfitsio::fits_get_colnum(fitsfile, CASEINSEN, const_cast<char*>(colname.c_str()), &colid_, &fits_status);
  checkFitsStatus(fits_status);

  cfitsio::fits_read_keys_lng(fitsfile, (char*)"NAXIS", 1, 2, naxes, &nfound, &fits_status);
  checkFitsStatus(fits_status);

  cfitsio::fits_read_key(fitsfile, TDOUBLE, (char*)"TSTART", &tstart_, nullptr, &fits_status);
  checkFitsStatus(fits_status);

  cfitsio::fits_read_key(fitsfile, TDOUBLE, (char*)"TSTOP", &tstop_, nullptr, &fits_status);
  checkFitsStatus(fits_status);

  cfitsio::fits_read_key(fitsfile, TDOUBLE, (char*)"TIMEZERO", &timezero_, nullptr, &fits_status);
  checkFitsStatus(fits_status);

  const int num_event = static_cast<int>(naxes[1]);
  arrivalTime_.resize(num_event);
  correctedTime_.resize(num_event);

  cfitsio::fits_read_col(fitsfile, TDOUBLE, colid_, (long)1, (long)1, naxes[1], &shortnul,
                         &arrivalTime_[0], &anynul, &fits_status);
  checkFitsStatus(fits_status);
  
  cfitsio::fits_close_file(fitsfile, &fits_status);
  checkFitsStatus(fits_status);
}

void LightCurve::correctTimeWithBinaryParameter()
{
  const int num_event = arrivalTime_.size();
  for (int i=0; i<num_event; i++) {
    arrivalTime_[i] += initialTime_;
  }
  for (int i=0; i<num_event; i++) {
    DetectedEvent ev;
    ev.setArrivalTime(arrivalTime_[i]);
    ev.calculateOrbitalPhase();
    ev.calculateCorrectedTime();
    correctedTime_[i] = ev.CorrectedTime();
  }
  tstartCorrected_ = tstart_ + (correctedTime_[0]-arrivalTime_[0]);
  tstopCorrected_ = tstop_ + (correctedTime_.back()-arrivalTime_.back());
  timezeroCorrected_ = timezero_ + (correctedTime_[0]-arrivalTime_[0]);
  const double offset = correctedTime_[0];
  for (int i=0; i<num_event; i++) {
    correctedTime_[i] -= offset;
  }
}

void LightCurve::correctTimeWithBinaryModulation()
{
  const int num_event = arrivalTime_.size();

  for (int i=0; i<num_event; i++) {
    arrivalTime_[i] += initialTime_;
  }
  
  const double T = BinaryModulation::orbital_period;
  const double t0 = BinaryModulation::t0;
  const double axsini = BinaryModulation::projected_semimajor_axis;
  
  for (int i=0; i<num_event; i++) {
    const double t = arrivalTime_[i];
    double phi = (t-t0)/T;
    phi = (phi-std::floor(phi))*unit::twopi;
    const double tcorr = t-(axsini*std::cos(phi))/unit::c;
    correctedTime_[i] = tcorr;
  }
  tstartCorrected_ = tstart_ + (correctedTime_[0]-arrivalTime_[0]);
  tstopCorrected_ = tstop_ + (correctedTime_.back()-arrivalTime_.back());
  timezeroCorrected_ = timezero_ + (correctedTime_[0]-arrivalTime_[0]);
  const double offset = correctedTime_[0];
  for (int i=0; i<num_event; i++) {
    correctedTime_[i] -= offset;
  }
}

void LightCurve::writeFits(const std::string& filename)
{
  cfitsio::fitsfile* fitsfile = nullptr;
  int fits_status = 0;

  cfitsio::fits_open_file(&fitsfile, filename.c_str(), READWRITE, &fits_status);
  checkFitsStatus(fits_status);

  cfitsio::ffmahd(fitsfile, 2, IMAGE_HDU, &fits_status);
  checkFitsStatus(fits_status);

  const int num_event = correctedTime_.size();
  
  cfitsio::fits_write_col(fitsfile, TDOUBLE, colid_, (long)1, (long)1, num_event,
                          &correctedTime_[0], &fits_status);
  checkFitsStatus(fits_status);

  cfitsio::fits_update_key(fitsfile, TDOUBLE, (char*)"TSTART", &tstartCorrected_, (char*)"time start corrected", &fits_status);
  checkFitsStatus(fits_status);
  
  cfitsio::fits_update_key(fitsfile, TDOUBLE, (char*)"TSTOP", &tstopCorrected_, (char*)"time stop corrected", &fits_status);
  checkFitsStatus(fits_status);

  cfitsio::fits_update_key(fitsfile, TDOUBLE, (char*)"TIMEZERO", &timezeroCorrected_, (char*)"Time Zero corrected", &fits_status);
  checkFitsStatus(fits_status);
  
  cfitsio::fits_close_file(fitsfile, &fits_status);
  checkFitsStatus(fits_status);

}

void LightCurve::process(const std::string& filename, int mode)
{
  readFits(filename);
  if (mode==1) {
    correctTimeWithBinaryParameter();
  }
  if (mode==2) {
    correctTimeWithBinaryModulation();
  }
  writeFits(filename);
}

namespace
{
void checkFitsStatus(int fits_status)
{
  if (fits_status) {
    cfitsio::fits_report_error(stderr, fits_status);
    exit(1);
  }
}
}
