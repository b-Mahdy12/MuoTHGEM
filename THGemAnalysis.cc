#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <dirent.h>
#include <math.h>

#include <TROOT.h>
#include <TApplication.h>
#include <TMath.h>
#include <TH1F.h>
#include <TH1I.h>
#include <TCanvas.h>
#include <TFile.h>


// using namespace Garfield;

std::string rootname = "result_hists.root";
TFile *f = new TFile(rootname.c_str(), "RECREATE");

void ReadTXTMuon()
{
  std::string line;
  double nsum, esum;

  std::ifstream File;
  File.open("muon.txt");
  std::vector<double> NSum, ESum;

  getline(File, line);

  if (line[0] != '#' && line.length() != 0)
  { // Ignore info
    size_t stop1 = line.find(";");
    size_t stop2 = line.find(";", stop1 + 1);
    nsum = stod(line.substr(0, stop1));
    esum = stod(line.substr(stop1 + 1, stop2));
    NSum.push_back(nsum);
    ESum.push_back(esum * 1.E-3);

    // Histograms
    TH1F *hNSum = new TH1F("hEne", "No of primary electrons", 100, 0, 200);
    TH1F *hESum = new TH1F("hEle", "Energy loss", 100, 0, 200);

    // std::cout << "\n\nFilling histograms  " << std::endl;
    for (int i = 0; i < 100; i++)
    {
      hNSum->Fill(NSum[i]);
      hESum->Fill(ESum[i]);
    }

    std::cout << "\n\nFilled histograms  " << std::endl;
    hNSum->Write();
    hESum->Write();
    std::cout << "Saved histograms  " << std::endl;
  }
}


void ReadTXTGain(bool draw = 0)
{
  std::string line;
  double rg, eg;
  double t;

  std::ifstream File;
  File.open("gain.txt");
  std::vector<double> RGain, EGain;
  std::vector<double> time;

  getline(File, line);

  if (line[0] != '#' && line.length() != 0)
  { // Ignore info
    size_t stop1 = line.find(";");
    size_t stop2 = line.find(";", stop1 + 1);
    rg = stod(line.substr(0, stop1));
    eg = stod(line.substr(stop1 + 1, stop2));
    t = stod(line.substr(stop2 + 1, line.size()));
    RGain.push_back(rg);
    EGain.push_back(eg);
    time.push_back(t);
  }
  // }

  // Histograms
  int nBins = 100;
  float hmin = 0.;
  float hmaxR = 1.1 * *max_element(RGain.begin(), RGain.end());
  float hmaxE = 1.1 * *max_element(EGain.begin(), EGain.end());
  float hmaxT = 1.1 * *max_element(time.begin(), time.end());
  std::string name_r = "hRGain_";
  std::string name_e = "hEGain_";

  TH1F *hRGain = new TH1F(name_r.c_str(), "Real Gain", nBins, hmin, hmaxR);
  TH1F *hEGain = new TH1F(name_e.c_str(), "Effective Gain", nBins, hmin, hmaxE);
  TH1F *hT = new TH1F("Time", "Time", nBins, hmin, hmaxT);

  for (int i = 0; i < 5; i++)
  {
    if (EGain[i] > 0)
    {
      hRGain->Fill(RGain[i]);
      hEGain->Fill(EGain[i]);
      hT->Fill(time[i]);
    }
  }

  std::cout << "\n\nFilled histograms  " << std::endl;
  hRGain->Write();
  hEGain->Write();
  std::cout << "Saved histograms  " << std::endl;

  if (draw == 1)
  {
    TCanvas *c1 = new TCanvas("", "canvas");
    c1->Divide(1, 2);
    c1->cd(1);
    hRGain->Draw();
    c1->cd(2);
    hEGain->Draw();
    std::string image = name_r + ".pdf";
    c1->SaveAs(image.c_str());
  }

  double m1, m2, s1, s2;
  m1 = hRGain->GetMean();
  m2 = hEGain->GetMean();
  s1 = hRGain->GetStdDev() / sqrt(hRGain->GetEntries());
  s2 = hEGain->GetStdDev() / sqrt(hEGain->GetEntries());
  std::cout << "Real Gain: " << m1 << "(" << s1 << ")" << std::endl;
  std::cout << "Effective Gain: " << m2 << "(" << s2 << ")" << std::endl;
  std::cout << "Number of Entries: " << hRGain->GetEntries() << std::endl;
  std::cout << "Average time: " << hT->GetMean() << std::endl;

  f->Close();
 
}

void ReadTXTPositionResolution()
{
   std::string rootnamepos = "result_pos_hists.root";
   TFile *fpos = new TFile(rootnamepos.c_str(), "RECREATE");

  std::string line;
  int ne;

  std::ifstream File;
  File.open("resolution.txt");
  std::vector<double> PosX;
  std::vector<double> PosY;
  double px, py;

  getline(File, line);

  if (line[0] != '#' && line[0] != 'n' && line.length() != 0)
  { // Ignore info
    size_t stop1 = line.find(";");
    size_t stop2 = line.find(";", stop1 + 1);
    px = stod(line.substr(0, stop1));
    py = stod(line.substr(stop1 + 1, stop2));
    ;
    PosX.push_back(px);
    PosY.push_back(py);
  }

  // Histograms
  int nBins = 100;
  float hmin = *min_element(PosX.begin(), PosX.end()) / 7;
  float hmaxN = *max_element(PosX.begin(), PosX.end()) / 7;
  std::string name_rx = "hPosX_";
  std::string name_ry = "hPosY_";

  TH1I *hpx = new TH1I(name_rx.c_str(), "", nBins, hmin, hmaxN);
  TH1I *hpy = new TH1I(name_ry.c_str(), "", nBins, hmin, hmaxN);

  for (int i = 0; i < 5; i++)
  {
    hpx->Fill(PosX[i]);
    hpy->Fill(PosY[i]);
  }

  std::cout << "\n\nFilled histograms for " << std::endl;
  hpx->Write();
  hpy->Write();
  std::cout << "Saved histograms for " << std::endl;

  double m1x, m2x, s1x, s2x;
  double m1y, m2y, s1y, s2y;

  int bin1x = hpx->FindFirstBinAbove(hpx->GetMaximum() / 2);
  int bin2x = hpx->FindLastBinAbove(hpx->GetMaximum() / 2);
  m2x = hpx->GetBinCenter(bin2x) - hpx->GetBinCenter(bin1x);

  m1x = (hmaxN - hmin) / nBins * 1e4;
  s2x = m1x / sqrt(12);

  int bin1y = hpy->FindFirstBinAbove(hpy->GetMaximum() / 2);
  int bin2y = hpy->FindLastBinAbove(hpy->GetMaximum() / 2);
  m2y = hpy->GetBinCenter(bin2y) - hpy->GetBinCenter(bin1y);

  m1y = (hmaxN - hmin) / nBins * 1e4;
  s2y = m1y / sqrt(12);

  std::cout << "Pad Size: " << m1x << std::endl;
  std::cout << "Position Resolution X: " << m2x * 1e4 << "(" << s2x << ")" << std::endl;
  std::cout << "Position Resolution Y: " << m2y * 1e4 << "(" << s2y << ")" << std::endl;
  std::cout << "Number of Entries: " << hpx->GetEntries() << std::endl;

  std::cout << "\nFinished saving all histograms!\n" << std::endl;
  fpos->Close();
}


int main(int argc, char *argv[])
{

  ReadTXTMuon();
  ReadTXTGain();
  ReadTXTPositionResolution();
}
