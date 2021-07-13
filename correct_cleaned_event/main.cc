#include <iostream>
#include <stdlib.h>
#include "AstroUnits.hh"
#include "DetectedEvent.hh"
#include "LightCurve.hh"
#include "CleanedEvents.hh"

int main() {
  for (char c='A'; c<='B'; c++) {
    std::string id = "";
    id += c;
    const std::string base_filename = "/Users/tamba/work/cenx-3/nustar_archive/barycorr_event/nu30101055002" + id + "01_cl_barycorr.evt";
    const std::string new_filename = "/Users/tamba/work/cenx-3/nustar_archive/binary_corrected_event/nu30101055002" + id + "01_cl_barycorr_binarycorrected.evt";
    std::string command = "cp " + base_filename + " " + new_filename;
    system(command.c_str());
    CleanedEvents* cleaned_ev = new CleanedEvents();
    cleaned_ev->process(new_filename);
    delete(cleaned_ev);
  }

  return 0;
}
