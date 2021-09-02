#include "Spectrum.hh"
#include <cmath>

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

void Spectrum::readFits(const std::string& filename)
{
  cfitsio::fitsfile* fitsfile = nullptr;
  int fits_status = 0;
  std::vector<std::string> colnames = {"CHANNEL", "COUNTS", "GROUPING"};
  long naxes[2] = {0, 0};
  int nfound = 0;

  cfitsio::fits_open_file(&fitsfile, filename.c_str(), READONLY, &fits_status);
  checkFitsStatus(fits_status);

  cfitsio::fits_movabs_hdu(fitsfile, 2, nullptr, &fits_status);
  checkFitsStatus(fits_status);

  cfitsio::fits_read_key(fitsfile, TDOUBLE, (char*)"EXPOSURE", &exposure_, nullptr, &fits_status);
  checkFitsStatus(fits_status);
  
  cfitsio::fits_read_keys_lng(fitsfile, (char*)"NAXIS", 1, 2, naxes, &nfound, &fits_status);
  checkFitsStatus(fits_status);

  const int num_event = static_cast<int>(naxes[1]);
  
  for (int i=0; i<static_cast<int>(colnames.size()); i++) {
    std::string colname = colnames[i];
    int colid = 0;

    std::vector<double> v(num_event);
    
    cfitsio::fits_get_colnum(fitsfile, CASEINSEN, const_cast<char*>(colname.c_str()), &colid, &fits_status);
    checkFitsStatus(fits_status);
    
    cfitsio::fits_read_col(fitsfile, TDOUBLE, colid, (long)1, (long)1, naxes[1], nullptr, &v[0], nullptr, &fits_status);
    checkFitsStatus(fits_status);

    for (int j=0; j<num_event; j++) {
      double now = v[j];
      if (i==0) channel_.push_back(static_cast<int>(now));
      else if (i==1) counts_.push_back(now);
      else if (i==2) grouping_.push_back(static_cast<int>(now));
      else {
        std::cerr << "ERROR: while reading fitsfile." << std::endl;
        exit(1);
      }
    }
  }

  for (int i=0; i<num_event; i++) {
    const double now = counts_[i]/exposure_;
    countsPerSecond_.push_back(now);
  }

  cfitsio::fits_close_file(fitsfile, &fits_status);
  checkFitsStatus(fits_status);
}

void Spectrum::addSpectrum(Spectrum* spectrum)
{
  auto ch = spectrum->Channel();
  auto cnts = spectrum->Counts();
  auto cps = spectrum->CountsPerSecond();
  if (ch.size()!=cnts.size()) {
    std::cerr << "ERROR: table inappropriate." << std::endl;
    exit(1);
  }
  const int n = ch.size();
  for (int i=0; i<n; i++) {
    const int x = ch[i];
    auto it = std::lower_bound(channel_.begin(), channel_.end(), x);
    if (it!=channel_.end() && *it==x) {
      const int index = it-channel_.begin();
      const double y1 = cnts[i];
      const double y2 = cps[i];
      counts_[index] += y1;
      countsPerSecond_[index] += y2;
    }
  }
}

void Spectrum::calculateRatio(Spectrum* spectrum, std::vector<int>& channels_array, std::vector<double>& ratio, std::vector<double>& ratio_error)
{
  for (int i=0; i<static_cast<int>(channel_.size()); i++) {
    if (grouping_[i]==1) {
      channels_array.push_back(channel_[i]);
    }
  }
  channels_array.push_back(channel_.back()+1);
  const int n = channels_array.size()-1;
  ratio.resize(n);
  ratio_error.resize(n);

  std::vector<int>& ch1 = channel_;
  std::vector<double>& cnts1 = counts_;
  std::vector<double>& cps1 = countsPerSecond_;
  std::vector<int>& ch2 = spectrum->Channel();
  std::vector<double>& cnts2 = spectrum->Counts();
  std::vector<double>& cps2 = spectrum->CountsPerSecond();

  std::vector<double> y1(n), dy1(n), y2(n), dy2(n);

  for (int q=0; q<2; q++) {
    for (int i=0; i<static_cast<int>(ch1.size()); i++) {
      const int x = ch1[i];
      auto it = std::upper_bound(channels_array.begin(), channels_array.end(), x);
      if (it==channels_array.begin() || it==channels_array.end()) continue;
      int index = it-channels_array.begin()-1;
      y1[index] += cps1[i];
      dy1[index] += cnts1[i];
    }
    std::swap(ch1, ch2);
    std::swap(cnts1, cnts2);
    std::swap(cps1, cps2);
    std::swap(y1, y2);
    std::swap(dy1, dy2);
  }

  for (int i=0; i<n; i++) {
    dy1[i] = y1[i]/std::sqrt(dy1[i]);
    dy2[i] = y2[i]/std::sqrt(dy2[i]);
  }

  for (int i=0; i<n; i++) {
    if (y2[i]==0.0) {
      ratio[i] = 0.0;
      ratio_error[i] = 0.0;
      continue;
    }
    ratio[i] = y1[i]/y2[i];
    const double r1 = dy1[i]/y1[i];
    const double r2 = dy2[i]/y2[i];
    const double r = std::sqrt(r1*r1+r2*r2);
    const double e = ratio[i]*r;
    ratio_error[i] = e;
  }
}

void Spectrum::applyEffectiveArea(const std::string& filename)
{
  cfitsio::fitsfile* fitsfile = nullptr;
  int fits_status = 0;
  std::string colname = "SPECRESP";
  long naxes[2] = {0, 0};
  int nfound = 0;

  cfitsio::fits_open_file(&fitsfile, filename.c_str(), READONLY, &fits_status);
  checkFitsStatus(fits_status);

  cfitsio::fits_movabs_hdu(fitsfile, 2, nullptr, &fits_status);
  checkFitsStatus(fits_status);

  cfitsio::fits_read_keys_lng(fitsfile, (char*)"NAXIS", 1, 2, naxes, &nfound, &fits_status);
  checkFitsStatus(fits_status);

  const int num_event = static_cast<int>(naxes[1]);
  int colid = 0;
  effectiveArea_.resize(num_event);
  
  cfitsio::fits_get_colnum(fitsfile, CASEINSEN, const_cast<char*>(colname.c_str()), &colid, &fits_status);
  checkFitsStatus(fits_status);
    
  cfitsio::fits_read_col(fitsfile, TDOUBLE, colid, (long)1, (long)1, naxes[1], nullptr, &effectiveArea_[0], nullptr, &fits_status);
  checkFitsStatus(fits_status);

  if (num_event!=static_cast<int>(countsPerSecond_.size())) {
    std::cerr << "ERROR in reading arf file." << std::endl;
    exit(1);
  }

  for (int i=0; i<num_event; i++) {
    const double now = effectiveArea_[i];
    countsPerSecond_[i] /= now;
  }

  cfitsio::fits_close_file(fitsfile, &fits_status);
  checkFitsStatus(fits_status);
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
