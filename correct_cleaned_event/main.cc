#include <iostream>
#include <stdlib.h>
#include "AstroUnits.hh"
#include "DetectedEvent.hh"
#include "LightCurve.hh"
#include "CleanedEvents.hh"

int main(int argc, char** argv) {
  if (argc!=3) {
    std::cerr << "USAGE correct_cleaned_event base_filename new_filename" << std::endl;
    exit(1);
  }

  const std::string base_filename = argv[1];
  const std::string new_filename = argv[2];
  std::string command = "cp " + base_filename + " " + new_filename;
  system(command.c_str());
  CleanedEvents* cleaned_ev = new CleanedEvents();
  cleaned_ev->process(new_filename);
  delete(cleaned_ev);

  return 0;
}
