#ifndef Spectrum_H
#define Spectrum_H 1

#include <iostream>
#include <string>
#include <vector>

class Spectrum
{
public:
  Spectrum() {}
  void readFits(const std::string& filename);
  void addSpectrum(Spectrum* spectrum);
  void calculateRatio(Spectrum* spectrum, std::vector<int>& channels_array, std::vector<double>& ratio, std::vector<double>& ratio_error);
//  void writeData(const std::string& filename);

  std::vector<int>& Channel() { return channel_; }
  std::vector<double>& Counts() { return counts_; }
  std::vector<int>& Grouping() { return grouping_; }
  std::vector<double>& CountsPerSecond() { return countsPerSecond_; }
  double Exposure() { return exposure_; }
  
private:
  std::vector<int> channel_;
  std::vector<double> counts_;
  std::vector<int> grouping_;
  std::vector<double> countsPerSecond_;
  double exposure_ = 0.0;
};

#endif
