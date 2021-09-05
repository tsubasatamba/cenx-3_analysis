#include <iostream>
#include <string>
#include <fstream>

#include "PulseProfile.hh"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

int main(int argc, char** argv) {
  if (argc!=3) {
    std::cerr << "USAGE: analyze_pulse_profile qdp_file output_json_file";
    exit(1);
  }
  const std::string qdp_file = argv[1];
  const std::string output_json_file = argv[2];

  PulseProfile* a = new PulseProfile();
  a->readQDP(qdp_file, 0, 8, 9, 3);
  a->calculatePulseFractionAll();
    
  std::pair<double, double>& pulse_fraction_area = a->PulseFractionArea();
  std::pair<double, double>& pulse_fraction_minmax = a->PulseFractionMinMax();
  std::pair<double, double>& pulse_fraction_rms = a->PulseFractionRms();
  std::vector<double>& power = a->Power();
  std::vector<double>& power_error = a->PowerError();
  const int power_size = power.size();  

  json js;
  json js_area, js_minmax, js_rms, js_power;
  js_area.push_back(json::object_t::value_type("value", pulse_fraction_area.first));
  js_area.push_back(json::object_t::value_type("error", pulse_fraction_area.second));
  js_minmax.push_back(json::object_t::value_type("value", pulse_fraction_minmax.first));
  js_minmax.push_back(json::object_t::value_type("error", pulse_fraction_minmax.second));
  js_rms.push_back(json::object_t::value_type("value", pulse_fraction_rms.first));
  js_rms.push_back(json::object_t::value_type("error", pulse_fraction_rms.second));
  for (int i=0; i<power_size; i++) {
    const double value = power[i];
    const double error = power_error[i];
    json js_now;
    js_now.push_back(json::object_t::value_type("index", i));
    js_now.push_back(json::object_t::value_type("value", value));
    js_now.push_back(json::object_t::value_type("error", error));
    js_power.push_back(js_now);
  }

  js.push_back(json::object_t::value_type("pulse_fraction_area", js_area));
  js.push_back(json::object_t::value_type("pulse_fraction_minmax", js_minmax));
  js.push_back(json::object_t::value_type("pulse_fraction_rms", js_rms));
  js.push_back(json::object_t::value_type("power", js_power));

  std::ofstream fout(output_json_file);
  fout << std::setw(2) << js;

  delete(a);
  return 0;
}
