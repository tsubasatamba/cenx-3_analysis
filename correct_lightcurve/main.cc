#include <iostream>
#include <stdlib.h>
#include "AstroUnits.hh"
#include "DetectedEvent.hh"
#include "LightCurve.hh"

int main(int argc, char** argv) {
  if (argc!=3) {
    std::cerr << "USAGE: correct_lightcurve base_file new_file" << std::endl;
    exit(1);
  }
  
  std::string base_file = argv[1];
  std::string new_file = argv[2];
  std::string command = "cp " + base_file + " " + new_file;
  system(command.c_str());
  LightCurve* lightcurve = new LightCurve();
  lightcurve->process(new_file);

  delete(lightcurve);
  
  return 0;
}
