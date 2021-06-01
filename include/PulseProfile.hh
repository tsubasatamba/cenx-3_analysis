#ifndef PulseProfile_H
#define PulseProfile_H 1

#include <iostream>
#include <vector>

class PulseProfile
{
public:
  PulseProfile() {}

  void readQDP(const std::string& filename, int phase_index, int data_index, int error_index, int ignore=0);
  void calculatePulseFractionArea();
  void calculatePulseFractionMinMax();
  void calculatePulseFractionRms();
  void calculatePulseFractionAll();

  void setData(std::vector<double>& v) { data_ = v; }
  void setError(std::vector<double>& v) { error_ = v; }
  void setPhase(std::vector<double>& v) { phase_ = v; }
  std::vector<double>& Data() { return data_; }
  std::vector<double>& Error() { return error_; }
  std::vector<double>& Phase() { return phase_; }
  std::pair<double, double>& PulseFractionArea() { return pulseFractionArea_; }
  std::pair<double, double>& PulseFractionMinMax() { return pulseFractionMinMax_; }
  std::pair<double, double>& PulseFractionRms() { return pulseFractionRms_; }
  std::vector<double>& Power() { return power_; }
  std::vector<double>& PowerError() { return powerError_; }
  
private:
  std::vector<double> phase_;
  std::vector<double> data_;
  std::vector<double> error_;
  std::pair<double, double> pulseFractionArea_ = std::pair<double, double>(0.0, 0.0);
  std::pair<double, double> pulseFractionMinMax_ = std::pair<double, double>(0.0, 0.0);
  std::pair<double, double> pulseFractionRms_ = std::pair<double, double>(0.0, 0.0);
  std::vector<double> power_;
  std::vector<double> powerError_;
};

#endif
