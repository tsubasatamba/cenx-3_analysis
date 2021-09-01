#ifndef DetectedEvent_H
#define DetectedEvent_H 1

#include <iostream>
#include "BinaryParameters.hh"

class DetectedEvent
{
public:
  double calculateOrbitalPhase(double t);
  void calculateBinaryTime();
  void calculateArrivalTime();
  
  void setArrivalTime(double v) { arrivalTime_ = v; }
  double ArrivalTime() {return arrivalTime_; }
  void setOrbitalPhase(double v) { orbitalPhase_ = v; }
  double OrbitalPhase() { return orbitalPhase_; }
  void setBinaryTime(double v) { binaryTime_ = v; }
  double BinaryTime() { return binaryTime_; }
  
private:
  double arrivalTime_ = 0.0;
  double orbitalPhase_ = 0.0;
  double binaryTime_ = 0.0;
};

#endif
