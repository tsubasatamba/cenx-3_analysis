#ifndef LightCurve_H
#define LightCurve_H 1

#include <iostream>
#include <string>
#include <vector>

class LightCurve
{
public:
  void process(const std::string& filename);
  void readFits(const std::string& filename);
  void correctData();
  void writeFits(const std::string& filename);
  void correctTime(double& t);
  std::string timeToDate(double t);
  
private:
  double tstart_ = 0.0;
  double tstop_ = 0.0;
  double timezero_ = 0.0;
  std::string dateObs_ = "";
  std::string dateEnd_ = "";
  std::vector<double> eventsTime_;
  std::vector<double> gtiStart_;
  std::vector<double> gtiStop_;
};

#endif
