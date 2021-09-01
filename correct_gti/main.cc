#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "DetectedEvent.hh"

void read(const std::string& filename, std::vector<std::pair<double, double>>& v) {
  std::ifstream fin(filename);
  double t1 = 0.0;
  double t2 = 0.0;
  while (fin >> t1 >> t2) {
    v.emplace_back(t1, t2);
  }
  fin.close();
}

void write(const std::string& filename, std::vector<std::pair<double, double>>& v) {
  std::ofstream fout(filename);
  const int n = v.size();
  for (int i=0; i<n; i++) {
    fout << std::fixed << std::setprecision(15) << v[i].first << " " << v[i].second << "\n";
  }
  fout.close();
}

void binary_time_to_arrival_time(std::vector<std::pair<double, double>>& v) {
  const int n = v.size();
  for (int i=0; i<n; i++) {
    for (int j=0; j<2; j++) {
      const double t = v[i].first;
      DetectedEvent ev;
      ev.setBinaryTime(t);
      ev.calculateArrivalTime();
      v[i].first = ev.ArrivalTime();
      std::swap(v[i].first, v[i].second);
    }
  }
}

int main(int argc, char** argv) {
  if (argc!=3) {
    std::cerr << "USAGE: correct_gti binary_gti_file arrival_gti_file" << std::endl;
    exit(1);
  }
  const std::string binary_gti_file = argv[1];
  const std::string arrival_gti_file = argv[2];

  std::vector<std::pair<double, double>> gti;
  read(binary_gti_file, gti);
  binary_time_to_arrival_time(gti);
  write(arrival_gti_file, gti);
  
  return 0;
}
