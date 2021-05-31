#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <TGraph.h>
#include <TF1.h>
#include <TCanvas.h>

#include "AstroUnits.hh"
#include "BinaryParameters.hh"

void read_graph(const std::string& filename, std::vector<double>& x, std::vector<double>& y) {
  std::ifstream fin(filename);
  double v, w;
  while (fin >> v >> w) {
    x.push_back(v);
    y.push_back(w);
  }
  fin.close();
}

void fit(TGraph* gr, const std::string& filename) {
  const double T = BinaryParameters::P0;
  TF1* f = new TF1("f1", "[0]-[1]*sin(2.0*TMath::Pi()*(x-[2])/[3])", 0.0, T);
  f->SetParameters(4.8026, 0.0066, -0.2*T, T);
  f->FixParameter(3, T);
  TCanvas* canv = new TCanvas();
  gr->Fit("f1");
  gr->Draw("AP*");
  canv->SaveAs(filename.c_str());
}

int main() {
  std::string binary_modulation_filename = "/Users/tambatsubasa/work/CenX-3/nustar_archive/cenx-3_analysis/binary_modulation/binary_modulations.txt";
  std::vector<double> x;
  std::vector<double> y;
  read_graph(binary_modulation_filename, x, y);

  const int num_plot = x.size();
  TGraph* gr = new TGraph(num_plot);
  for (int i=0; i<num_plot; i++) {
    gr->SetPoint(i, x[i], y[i]);
  }
  const std::string output_filename = "binary_modulation.png";
  fit(gr, output_filename);
  
  return 0;
}
