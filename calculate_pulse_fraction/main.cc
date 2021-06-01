#include <iostream>
#include <string>
#include <fstream>

#include "PulseProfile.hh"

void read_file(const std::string& filename, std::vector<std::string>& vec) {
  std::fstream fin(filename);
  std::string s;
  while (fin >> s) {
    vec.push_back(s);
  }
}

void write_file(const std::string& filename, std::vector<std::vector<double>>& vec, bool header) {
  std::ofstream fout(filename);
  if (header) {
    fout << "READ SERR 2 3 4\n";
  }
  for (int i=0; i<static_cast<int>(vec.size()); i++) {
    for (int j=0; j<static_cast<int>(vec[i].size()); j++) {
      fout << vec[i][j];
      if (j==static_cast<int>(vec[i].size())-1) fout << "\n";
      else fout << " ";
    }
  }
}

int main(int argc, char** argv) {
  if (argc!=4) {
    std::cerr << "USAGE: calculate_pulse_fraction qdps_file times_file output_file";
    exit(1);
  }
  const std::string qdps_file = argv[1];
  const std::string times_file = argv[2];
  const std::string output_file = argv[3];

  std::vector<std::string> input_qdps;
  std::vector<double> input_times;
  std::vector<std::string> tmp;
  read_file(qdps_file, input_qdps);
  read_file(times_file, tmp);
  for (std::string s: tmp) {
    input_times.push_back(std::stod(s));
  }

  if (input_qdps.size()!=input_times.size()) {
    std::cerr << "qdps_file.size != times_file.size()" << std::endl;
    exit(1);
  }
  const int num_files = input_qdps.size();
  std::vector<std::vector<double>> result(num_files, std::vector<double>(7));
  std::vector<std::vector<double>> power(num_files);
  std::vector<std::vector<double>> power_error(num_files);

  for (int i=0; i<num_files; i++) {
    const std::string file = input_qdps[i];
    PulseProfile* a = new PulseProfile();
    a->readQDP(file, 0, 8, 9, 3);
    a->calculatePulseFractionAll();
    result[i][0] = input_times[i];
    result[i][1] = a->PulseFractionArea().first;
    result[i][2] = a->PulseFractionArea().second;
    result[i][3] = a->PulseFractionMinMax().first;
    result[i][4] = a->PulseFractionMinMax().second;
    result[i][5] = a->PulseFractionRms().first;
    result[i][6] = a->PulseFractionRms().second;
    auto p = a->Power();
    auto pe = a->PowerError();
    for (auto e: p) {
      power[i].push_back(e);
    }
    for (auto e: pe) {
      power_error[i].push_back(e);
    }
    delete(a);
  }
  
  write_file(output_file, result, true);
  write_file("power.txt", power, false);
  write_file("power_error.txt", power_error, false);
  
  return 0;
}
