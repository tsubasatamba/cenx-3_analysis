#include "LightCurve.hh"
#include "DetectedEvent.hh"
#include "AstroUnits.hh"
#include <cmath>
#include <sstream>
#include <iomanip>

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

void LightCurve::process(const std::string& filename)
{
  readFits(filename);
  correctData();
  writeFits(filename);
}

void LightCurve::readFits(const std::string& filename)
{
  cfitsio::fitsfile* fitsfile = nullptr;
  int fits_status = 0;
  std::vector<std::string> keywords = {"TSTART", "TSTOP", "DATE-OBS", "DATE-END"};
  
  cfitsio::fits_open_file(&fitsfile, filename.c_str(), READONLY, &fits_status);
  checkFitsStatus(fits_status);

  cfitsio::fits_read_key(fitsfile, TDOUBLE, keywords[0].c_str(), &tstart_, nullptr, &fits_status);
  checkFitsStatus(fits_status);
  cfitsio::fits_read_key(fitsfile, TDOUBLE, keywords[1].c_str(), &tstop_, nullptr, &fits_status);
  checkFitsStatus(fits_status);
  char* tmp1;
  char* tmp2;
  tmp1 = (char*)malloc(sizeof(char)*100);
  tmp2 = (char*)malloc(sizeof(char)*100);
  cfitsio::fits_read_keyword(fitsfile, keywords[2].c_str(), tmp1, nullptr, &fits_status);
  checkFitsStatus(fits_status);
  dateObs_ = tmp1;
  cfitsio::fits_read_keyword(fitsfile, keywords[3].c_str(), tmp2, nullptr, &fits_status);
  checkFitsStatus(fits_status);
  dateEnd_ = tmp2;

  /// EVENTS extension
  cfitsio::fits_movabs_hdu(fitsfile, 2, nullptr, &fits_status);
  checkFitsStatus(fits_status);

  {
    cfitsio::fits_read_key(fitsfile, TDOUBLE, "TIMEZERO", &timezero_, nullptr, &fits_status);
    checkFitsStatus(fits_status);
  }

  {
    long naxes[2] = {0, 0};
    std::string colname = "TIME";
    int colid = 0;
    int nfound = 0;
    cfitsio::fits_read_keys_lng(fitsfile, (char*)"NAXIS", 1, 2, naxes, &nfound, &fits_status);
    checkFitsStatus(fits_status);

    const int num_event = static_cast<int>(naxes[1]);
    eventsTime_.resize(num_event);

    cfitsio::fits_get_colnum(fitsfile, CASEINSEN, const_cast<char*>(colname.c_str()), &colid, &fits_status);
    checkFitsStatus(fits_status);

    cfitsio::fits_read_col(fitsfile, TDOUBLE, colid, (long)1, (long)1, naxes[1], nullptr, &eventsTime_[0], nullptr, &fits_status);
    checkFitsStatus(fits_status);

    for (int i=0; i<num_event; i++) {
      eventsTime_[i] += timezero_;
    }
  }
  
  /// GTI extension
  cfitsio::fits_movabs_hdu(fitsfile, 3, nullptr, &fits_status);
  checkFitsStatus(fits_status);

  for (std::string colname: {"START", "STOP"}) {
    long naxes[2] = {0, 0};
    int colid = 0;
    int nfound = 0;
    cfitsio::fits_read_keys_lng(fitsfile, (char*)"NAXIS", 1, 2, naxes, &nfound, &fits_status);
    checkFitsStatus(fits_status);

    const int num_event = static_cast<int>(naxes[1]);
    gtiStart_.resize(num_event);

    cfitsio::fits_get_colnum(fitsfile, CASEINSEN, const_cast<char*>(colname.c_str()), &colid, &fits_status);
    checkFitsStatus(fits_status);

    cfitsio::fits_read_col(fitsfile, TDOUBLE, colid, (long)1, (long)1, naxes[1], nullptr, &gtiStart_[0], nullptr, &fits_status);
    checkFitsStatus(fits_status);
    std::swap(gtiStart_, gtiStop_);
  }

  cfitsio::fits_close_file(fitsfile, &fits_status);
  checkFitsStatus(fits_status);
}

void LightCurve::correctData()
{
  correctTime(tstart_);
  correctTime(tstop_);
  correctTime(timezero_);
  dateObs_ = timeToDate(tstart_);
  dateEnd_ = timeToDate(tstop_);

  for (double& t: eventsTime_) {
    correctTime(t);
    t -= timezero_;
  }
  for (double& t: gtiStart_) {
    correctTime(t);
  }
  for (double& t: gtiStop_) {
    correctTime(t);
  }
}

void LightCurve::writeFits(const std::string& filename)
{
  cfitsio::fitsfile* fitsfile = nullptr;
  int fits_status = 0;
  std::vector<std::string> keywords = {"TSTART", "TSTOP", "DATE-OBS", "DATE-END"};  
  
  cfitsio::fits_open_file(&fitsfile, filename.c_str(), READWRITE, &fits_status);
  checkFitsStatus(fits_status);

  for (int i=1; i<=4; i++) {
    cfitsio::fits_movabs_hdu(fitsfile, i, nullptr, &fits_status);
    checkFitsStatus(fits_status);

    cfitsio::fits_update_key(fitsfile, TDOUBLE, keywords[0].c_str(), &tstart_, nullptr, &fits_status);
    checkFitsStatus(fits_status);
    cfitsio::fits_update_key(fitsfile, TDOUBLE, keywords[1].c_str(), &tstop_, nullptr, &fits_status);
    checkFitsStatus(fits_status);
    cfitsio::fits_update_key(fitsfile, TSTRING, keywords[2].c_str(), const_cast<char*>(dateObs_.c_str()), nullptr, &fits_status);
    checkFitsStatus(fits_status);
    cfitsio::fits_update_key(fitsfile, TSTRING, keywords[3].c_str(), const_cast<char*>(dateEnd_.c_str()), nullptr, &fits_status);
    checkFitsStatus(fits_status);
  }

  /// EVENTS extension
  cfitsio::fits_movabs_hdu(fitsfile, 2, nullptr, &fits_status);
  checkFitsStatus(fits_status);

  {
    cfitsio::fits_update_key(fitsfile, TDOUBLE, "TIMEZERO", &timezero_, nullptr, &fits_status);
    checkFitsStatus(fits_status);
  }
  
  {
    long naxes[2] = {0, 0};
    std::string colname = "TIME";
    int colid = 0;
    int nfound = 0;
    cfitsio::fits_read_keys_lng(fitsfile, (char*)"NAXIS", 1, 2, naxes, &nfound, &fits_status);
    checkFitsStatus(fits_status);
    
    const int num_event = eventsTime_.size();
    if (num_event!=static_cast<int>(naxes[1])) {
      std::cerr << "num event different." << std::endl;
      exit(1);
    }
    
    cfitsio::fits_get_colnum(fitsfile, CASEINSEN, const_cast<char*>(colname.c_str()), &colid, &fits_status);
    checkFitsStatus(fits_status);
    
    cfitsio::fits_write_col(fitsfile, TDOUBLE, colid, (long)1, (long)1, naxes[1], &eventsTime_[0], &fits_status);
    checkFitsStatus(fits_status);
  }

  /// GTI extension
  cfitsio::fits_movabs_hdu(fitsfile, 3, nullptr, &fits_status);
  checkFitsStatus(fits_status);

  for (std::string colname: {"START", "STOP"}) {
    long naxes[2] = {0, 0};
    int colid = 0;
    int nfound = 0;
    cfitsio::fits_read_keys_lng(fitsfile, (char*)"NAXIS", 1, 2, naxes, &nfound, &fits_status);
    checkFitsStatus(fits_status);
    
    const int num_event = gtiStart_.size();
    if (num_event!=naxes[1]) {
      std::cerr << "num event different." << std::endl;
      exit(1);
    }
    
    cfitsio::fits_get_colnum(fitsfile, CASEINSEN, const_cast<char*>(colname.c_str()), &colid, &fits_status);
    checkFitsStatus(fits_status);
    
    cfitsio::fits_write_col(fitsfile, TDOUBLE, colid, (long)1, (long)1, naxes[1], &gtiStart_[0], &fits_status);
    checkFitsStatus(fits_status);
    std::swap(gtiStart_, gtiStop_);
  }
  
  cfitsio::fits_close_file(fitsfile, &fits_status);
  checkFitsStatus(fits_status);
}

void LightCurve::correctTime(double& t)
{
  DetectedEvent ev;
  ev.setArrivalTime(t);
  ev.calculateBinaryTime();
  t = ev.BinaryTime();
}

std::string LightCurve::timeToDate(double t)
{
  std::string s = "";
  const double mjd = unit::event_time_to_mjd(t)/unit::day;
  int year = 2015;
  int month = 0;
  int day = 0;
  if (std::floor(mjd)==57356.0) {
    month = 11;
    day = 30;
  }
  else if (std::floor(mjd)==57357.0) {
    month = 12;
    day = 1;
  }
  else {
    std::cerr << "cannot calculate date." << std::endl;
    exit(1);
  }
  int rem = static_cast<int>((mjd-std::floor(mjd))*unit::day);
  int hour = rem/3600;
  rem %= 3600;
  int minute = rem/60;
  rem %= 60;
  int second = rem;

  std::ostringstream sout;
  sout << year << "-";
  sout << std::setfill('0') << std::setw(2) << month << "-";
  sout << std::setfill('0') << std::setw(2) << day << "T";
  sout << std::setfill('0') << std::setw(2) << hour << ":";
  sout << std::setfill('0') << std::setw(2) << minute << ":";
  sout << std::setfill('0') << std::setw(2) << second;
  s = sout.str();
  
  return s;
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
