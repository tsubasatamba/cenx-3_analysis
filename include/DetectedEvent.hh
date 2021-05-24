#ifndef Event_H
#define Event_H

#include <iostream>
#include "BinaryParameters.hh"

class DetectedEvent
{
public:
  void calculateOrbitalPhase();
  
  void setArrivalTime(double v) { arrivalTime_ = v; }
  double ArrivalTime() {return arrivalTime_; }
  void setOrbitalPhase(double v) { orbitalPhase_ = v; }
  double OrbitalPhase() { return orbitalPhase_; }
  void setCorrectedTime(double v) { correctedTime_ = v; }
  double CorrectedTime() { return correctedTime_; }
  
private:
  double arrivalTime_ = 0.0;
  double orbitalPhase_ = 0.0;
  double correctedTime_ = 0.0;
};

#endif