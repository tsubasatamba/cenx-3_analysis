#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cmath>
#include "Spectrum.hh"

double pi_to_energy(int pi) {
  const double e0 = 1.6;
  const double de = 0.04;
  const double energy = e0 + de*pi;
  return energy;
}

int main(int argc, char** argv) {
  if (argc!=5) {
    std::cerr << "USAGE: extract_lightcurve_from_spectrum directory energy_min energy_max output_filename" << std::endl;
    exit(1);
  }
  const std::string directory = argv[1];
  const double energy_min = std::stod(argv[2]);
  const double energy_max = std::stod(argv[3]);
  const std::string output_filename = argv[4];

  const std::string input_filename1 = directory + "/FPMA_sr_grp.pha";
  const std::string input_filename2 = directory + "/FPMB_sr_grp.pha";
  
  Spectrum* spectrum1 = new Spectrum();
  spectrum1->readFits(input_filename1);
  Spectrum* spectrum2 = new Spectrum();
  spectrum2->readFits(input_filename2);

  spectrum1->addSpectrum(spectrum2);
  
  std::vector<int>& ch = spectrum1->Channel();
  std::vector<double>& cnts = spectrum1->Counts();
  std::vector<double>& cps = spectrum1->CountsPerSecond();
  double value = 0.0;
  double error = 0.0;
  const int n = ch.size();

  for (int i=0; i<n; i++) {
    int ch1 = ch[i];
    int ch2 = ch1+1;
    if (i!=n-1) {
      ch2 = ch[i+1];
    }
    const double e1 = pi_to_energy(ch1);
    const double e2 = pi_to_energy(ch2);
    const double e = 0.5*(e1+e2);
    if (e>=energy_min && e<=energy_max) {
      value += cps[i];
      error += cnts[i];
    }
  }
  error = value/std::sqrt(error);
  
  std::ofstream fout(output_filename);
  fout << value << " " << error << "\n";
  fout.close();
  
  delete(spectrum1);
  delete(spectrum2);

  return 0;
}
