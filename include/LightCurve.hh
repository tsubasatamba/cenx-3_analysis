#ifndef LightCurve_H
#define LightCurve_H 1

#include <iostream>
#include <vector>

class LightCurve
{
public:
  void readFits(const std::string& filename);
  void correctTimeWithBinaryParameter();
  void correctTimeWithBinaryModulation();
  void writeFits(const std::string& filename);
  void process(const std::string& filename, int mode);
  
private:
  std::vector<double> arrivalTime_;
  std::vector<double> correctedTime_;
  int colid_ = 0;
  double initialTime_ = 0.0;
  double tstart_ = 0.0;
  double tstop_ = 0.0;
  double timezero_ = 0.0;
  double tstartCorrected_ = 0.0;
  double tstopCorrected_ = 0.0;
  double timezeroCorrected_ = 0.0;
};

#endif
