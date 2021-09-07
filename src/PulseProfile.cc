#include <fstream>
#include <algorithm>
#include <cmath>

#include "PulseProfile.hh"
#include "AstroUnits.hh"

namespace
{
void strip(std::string& s);
void split(std::string& s, std::vector<std::string>& vec);
}

void PulseProfile::readQDP(const std::string& filename, int phase_index, int data_index, int error_index, int ignore/*=0*/)
{
  std::ifstream fin(filename);
  std::string line;
  int index = 0;
  while (getline(fin, line)) {
    if (index<ignore) {
      index++;
      continue;
    }
    std::vector<std::string> vec;
    split(line, vec);
    if (data_index>=static_cast<int>(vec.size()) || error_index>=static_cast<int>(vec.size())) {
      std::cerr << "ERROR: data_index or error_index exceeds arrray size." << std::endl;
      exit(1);
    }
    const double phase = std::stod(vec[phase_index]);
    const double data = std::stod(vec[data_index]);
    const double error = std::stod(vec[error_index]);
    if (phase>=1.0) {
      break;
    }
    phase_.push_back(phase);
    data_.push_back(data);
    error_.push_back(error);
    index++;
  }
  fin.close();
}

void PulseProfile::calculatePulseFractionArea()
{
  const int n = data_.size();
  auto it = min_element(data_.begin(), data_.end());
  const double vmin = *it;
  const double imin = it-data_.begin();
  const double emin = error_[imin];
  
  double sum = 0.0;
  double esum = 0.0;
  for (int i=0; i<n; i++) {
    sum += data_[i];
    esum += error_[i]*error_[i];
  }
  esum = std::sqrt(esum);
  const double res = 1.0-n*vmin/sum;
  const double x = emin/vmin;
  const double y = esum/sum;
  const double ratio = std::sqrt(x*x+y*y);
  const double error = (1.0-res)*ratio;

  pulseFractionArea_.first = res;
  pulseFractionArea_.second = error;
}

void PulseProfile::calculatePulseFractionMinMax()
{
  auto it1 = max_element(data_.begin(), data_.end());
  auto it2 = min_element(data_.begin(), data_.end());
  const int i1 = it1-data_.begin();
  const int i2 = it2-data_.begin();
  const double v1 = *it1;
  const double v2 = *it2;
  const double e1 = error_[i1];
  const double e2 = error_[i2];
    
  const double res = (v1-v2)/(v1+v2);
  const double x = std::sqrt(e1*e1+e2*e2)/(v1-v2);
  const double y = std::sqrt(e1*e1+e2*e2)/(v1+v2);  
  const double ratio = std::sqrt(x*x+y*y);
  const double error = res*ratio;

  pulseFractionMinMax_.first = res;
  pulseFractionMinMax_.second = error;
}

void PulseProfile::calculatePulseFractionRms()
{
  const int n = data_.size();
  power_.resize(n);
  powerError_.resize(n);
  phaseShift_.resize(n);
  const double n2 = static_cast<double>(n)*static_cast<double>(n);
  std::vector<double> a(n);
  std::vector<double> b(n);
  std::vector<double> ea(n);
  std::vector<double> eb(n);
  std::vector<double> s2a(n);
  std::vector<double> s2b(n);
  const double twopi = unit::twopi;
  double sum = 0.0;
  double esum = 0.0;
  
  for (int i=0; i<n; i++) {
    for (int j=0; j<n; j++) {
      const double x = phase_[j];
      const double y = data_[j];
      const double ey = error_[j];
      a[i] += (1.0/n)*y*std::cos(twopi*x*i);
      b[i] += (1.0/n)*y*std::sin(twopi*x*i);
      ea[i] += std::pow((1.0/n)*ey*std::cos(twopi*x*i), 2);
      eb[i] += std::pow((1.0/n)*ey*std::sin(twopi*x*i), 2);
      s2a[i] += (1.0/n2)*ey*ey*std::pow(std::cos(twopi*x*i), 2);
      s2b[i] += (1.0/n2)*ey*ey*std::pow(std::sin(twopi*x*i), 2);
    }
    ea[i] = std::sqrt(ea[i]);
    eb[i] = std::sqrt(eb[i]);
    
    const double now = a[i]*a[i]+b[i]*b[i]-s2a[i]-s2b[i];
    if (i==0) power_[i] = now;
    else power_[i] = now*2.0;
    if (i>=1) {
      sum += now;
    }
    double enow = std::sqrt(4.0*a[i]*a[i]*ea[i]*ea[i]+4.0*b[i]*b[i]*eb[i]*eb[i]);
    if (i==0) powerError_[i] = enow;
    else powerError_[i] = enow*2.0;
    if (i>=1) {
      esum += enow*enow;
    }
    phaseShift_[i] = std::atan2(b[i], a[i]);
  }
  esum = std::sqrt(esum);
  const double res = std::sqrt(2.0*sum)/a[0];
  const double ratio = std::sqrt(0.5*(esum*esum/(sum*sum))+ea[0]*ea[0]/(a[0]*a[0]));
  const double error = res*ratio;

  pulseFractionRms_.first = res;
  pulseFractionRms_.second = error;
}

void PulseProfile::calculatePulseFractionAll()
{
  calculatePulseFractionArea();
  calculatePulseFractionMinMax();
  calculatePulseFractionRms();
}

namespace
{

void strip(std::string& s)
{
  while (s.size() && s.back()==' ') {
    s.pop_back();
  }
  while (s.size() && s[0]==' ') {
    s.erase(0, 1);
  }
}

void split(std::string& s, std::vector<std::string>& vec)
{
  strip(s);
  std::string now = "";
  for (char c: s) {
    if (c==' ') {
      vec.push_back(now);
      now = "";
    }
    else {
      now += c;
    }
  }
  if (now.size()) {
    vec.push_back(now);
  }
}

}
