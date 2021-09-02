#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "Spectrum.hh"

double pi_to_energy(int pi) {
  const double e0 = 1.6;
  const double de = 0.04;
  const double energy = e0 + de*pi;
  return energy;
}

void write_qdp(std::vector<double>& energy_array, std::vector<double>& ratio, std::vector<double>& ratio_error, const std::string& filename) {
  std::ofstream fout(filename);
  const int n = ratio.size();
  if (n!=static_cast<int>(ratio_error.size()) || n+1!=static_cast<int>(energy_array.size())) {
    std::cerr << "ERROR: arrays not appropriate." << std::endl;
    exit(1);
  }
  fout << "READ SERR 1 2\n";
  for (int i=0; i<n; i++) {
    const double x = 0.5*(energy_array[i]+energy_array[i+1]);
    const double dx = 0.5*(energy_array[i+1]-energy_array[i]);
    const double y = ratio[i];
    const double dy = ratio_error[i];
    if (y==0.0) continue;
    if (dy/y>=0.5) continue; 
    fout << x << " " << dx << " " << y << " " << dy << "\n";
  }
  fout.close();
}

int main(int argc, char** argv) {
  if (argc!=4) {
    std::cerr << "USAGE: calculate_spectrum_ratio directory1 directory2 output_filename" << std::endl;
    exit(1);
  }
  const std::string directory1 = argv[1];
  const std::string directory2 = argv[2];
  const std::string output_filename = argv[3];
  
  const std::string input_filename1 = directory1 + "/FPMA_sr_grp.pha";
  const std::string input_filename2 = directory1 + "/FPMB_sr_grp.pha";
  const std::string input_filename3 = directory2 + "/FPMA_sr_grp.pha";
  const std::string input_filename4 = directory2 + "/FPMB_sr_grp.pha";

  const std::string arf_filename1 = directory1 + "/FPMA_sr.arf";
  const std::string arf_filename2 = directory1 + "/FPMB_sr.arf";
  const std::string arf_filename3 = directory2 + "/FPMA_sr.arf";
  const std::string arf_filename4 = directory2 + "/FPMB_sr.arf";
  
  Spectrum* spectrum1 = new Spectrum();
  spectrum1->readFits(input_filename1);
  spectrum1->applyEffectiveArea(arf_filename1);

  Spectrum* spectrum2 = new Spectrum();
  spectrum2->readFits(input_filename2);
  spectrum2->applyEffectiveArea(arf_filename2);
  
  Spectrum* spectrum3 = new Spectrum();
  spectrum3->readFits(input_filename3);
  spectrum3->applyEffectiveArea(arf_filename3);

  Spectrum* spectrum4 = new Spectrum();
  spectrum4->readFits(input_filename4);
  spectrum4->applyEffectiveArea(arf_filename4);
  
  spectrum1->addSpectrum(spectrum2);
  spectrum3->addSpectrum(spectrum4);

  std::vector<int> channels_array;
  std::vector<double> ratio;
  std::vector<double> ratio_error;
  spectrum1->calculateRatio(spectrum3, channels_array, ratio, ratio_error);

  std::vector<double> energy_array;
  for (int i=0; i<static_cast<int>(channels_array.size()); i++) {
    const double pi = channels_array[i];
    const double e = pi_to_energy(pi);
    energy_array.push_back(e);
  }

  write_qdp(energy_array, ratio, ratio_error, output_filename);
  
  delete(spectrum1);
  delete(spectrum2);
  delete(spectrum3);
  delete(spectrum4);

  return 0;
}
