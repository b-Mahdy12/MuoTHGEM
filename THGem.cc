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

#include "Garfield/ComponentAnsys123.hh"
#include "Garfield/MediumMagboltz.hh"
#include "Garfield/ViewMedium.hh"
#include "Garfield/ViewSignal.hh"
#include "Garfield/ViewFEMesh.hh"
#include "Garfield/Sensor.hh"
#include "Garfield/AvalancheMicroscopic.hh"
#include "Garfield/AvalancheMC.hh"
#include "Garfield/Random.hh"
#include "Garfield/TrackHeed.hh"
#include "Garfield/Plotting.hh"

using namespace Garfield;

int main(int argc, char *argv[])
{

  // Temperature (temp) in Kelvin and Pressure (press) in Torr
  double percent = 70.;
  double ppm = 0.;
  double penning = 0.57;
  const char *noble = "ar";
  double temp = 293.15;
  double press = 760;
  // Import

  ComponentAnsys123 *fm = new ComponentAnsys123();
  fm->Initialise("ELIST.lis",
                 "NLIST.lis",
                 "MPLIST.lis",
                 "PRNSOL.lis", "mm");
  fm->SetWeightingField("PRNSOL.lis", "WT");
  fm->EnableMirrorPeriodicityX();
  fm->EnableMirrorPeriodicityY();
  fm->PrintRange();

  double co2 = 100. - percent;
  double ar = percent;

  // Medium
  MediumMagboltz *gas = new MediumMagboltz();
  gas->LoadGasFile("ar_70_co2_30.gas");
  // gas->LoadGasFile("ar_90_co2_10.gas");
  // gas->LoadGasFile("ar_80_co2_20.gas");

  // gas->LoadGasFile("xe-100.gas");

  gas->EnableDrift();
  gas->SetMaxElectronEnergy(18000);
  gas->Initialise();

  if (penning != 0.)
  {
    gas->EnablePenningTransfer(penning, 0., noble);
  }
  else
  {
    gas->DisablePenningTransfer();
  }

  gas->LoadIonMobility("IonMobility_Ar+_Ar.txt");
  int nMaterials = fm->GetNumberOfMaterials();
  for (int i = 0; i < nMaterials; ++i)
  {
    const double eps = fm->GetPermittivity(i);
    if (fabs(eps - 1.) < 1.e-3)
      fm->SetMedium(i, gas);
  }

  fm->PrintMaterials();

  std::string txtfileG = "gain.txt";
  std::string txtfileM = "muon.txt";
  std::string txtfileR = "resolution.txt";
  double zmax =0.3;
  double zmin = -0.1;
  double driftzone = 0.0001;
  int n_events = 5;

  // Time
  const double t0 = 0., tEnd = 1.E3;
  const int nTimeBins = 100;

  // Sensor
  Sensor *sensor = new Sensor();
  sensor->AddComponent(fm);
  sensor->SetArea(-10, -10, zmin, 10, 10, zmax);
  sensor->AddElectrode(fm, "WT");
  sensor->SetTimeWindow(t0, (tEnd - t0) / nTimeBins, nTimeBins);

  // Drift Visualization
  ViewDrift *vDrift = new ViewDrift();
  vDrift->SetArea(-0.5, -0.5, zmin, 0.5, 0.5, zmax);

  // Avalanche Setup
  AvalancheMicroscopic *aval = new AvalancheMicroscopic();
  aval->SetSensor(sensor);
  aval->EnableSignalCalculation();
  aval->SetCollisionSteps(100);
  aval->EnableAvalancheSizeLimit(18000);
  aval->EnablePlotting(vDrift);

  AvalancheMC *drift = new AvalancheMC();
  drift->SetSensor(sensor);
  drift->SetDistanceSteps(2.e-4);
  drift->EnableRKFSteps(true);
  drift->EnablePlotting(vDrift);

  // Muon Track Setup
  TrackHeed *track = new TrackHeed();
  track->SetSensor(sensor);
  track->SetParticle("mu-"); // muon-
  track->SetEnergy(4E9);     // GeV
  track->EnableElectricField();
  track->EnablePlotting(vDrift);

  // File
  FILE *file;
  const char *f_titleG = txtfileG.c_str();
  const char *f_titleM = txtfileM.c_str();
  const char *f_titleR = txtfileR.c_str();

  // Avalanches Calculations
  for (int i = n_events; i--;)
  {
    clock_t begin_time = clock();

    // Random Initial Positions
    double z0 = zmax - driftzone;
    std::cout << "Z0: " << z0 << std::endl;
    // double x0 = (2 * RndmUniform() - 1) / 2;
    // double y0 = (2 * RndmUniform() - 1) / 2;
    double x0 = 0.5 * (RndmUniform() - 1);
    double y0 = 0.5 * (RndmUniform() - 1);
    const double t0 = 0.;
    // Velocity vector (direction only)
    const double dx0 = 0.;
    const double dy0 = 0.;
    const double dz0 = -1;

    track->NewTrack(x0, y0, z0, t0, dx0, dy0, dz0);

    double xc, yc, zc, tc, extra;
    int nc = 0, nsum = 0.;
    double ec = 0., esum = 0.;
    while (track->GetCluster(xc, yc, zc, tc, nc, ec, extra))
    {
      esum += ec;
      nsum += nc;

      for (int j = 0; j < nc; j++)
      {

        // Properties of primary electron (pos, time, energy, velocity vector)
        double xe1, ye1, ze1, te1, e1, dxe, dye, dze;
        track->GetElectron(j, xe1, ye1, ze1, te1, e1, dxe, dye, dze);

        aval->AvalancheElectron(x0, y0, z0, te1, 0., 0., 0., 0.);

        int ne = 0, ni = 0, nf = 0;
        double sumy = 0, sumx = 0;
        // aval->GetAvalancheSize(ne, ni);
        int np = aval->GetNumberOfElectronEndpoints();
        np += ne;

        // Final Positions Analysis
        // Start of track
        double xe2, ye2, ze2, te2, e2;
        // End of track
        double xe3, ye3, ze3, te3, e3;
        int status;

        for (int j = np; j--;)
        {
          aval->GetElectronEndpoint(j, xe2, ye2, ze2, te2, e2,
                                    xe3, ye3, ze3, te3, e3, status);

          if (ze3 <= zmin + 0.0001)
          { // Added a delta to minimize border effects
            nf += 1;

            sumx += xe3;
            sumy += ye3;
          }
        }

        // Integrate current to get total charge on pad
        double sum = 0.;
        for (int k = 0; k < nTimeBins; k++)
        {
          double wt = -1 * sensor->GetSignal("WT", k) / ElementaryCharge;
          sum += wt;
        }
        // std::cout << "Integrated Signal Final" << sum << std::endl;

        // Saving Gains
        float min = float(clock() - begin_time) / (CLOCKS_PER_SEC * 60);
        file = fopen(f_titleG, "a");
        fprintf(file, "%d;%d;%f\n", np, abs(nf), min);
        fclose(file);

        std::cout << "Gain saved" << std::endl;
        
        // Saving Primary electrons
        file = fopen(f_titleM, "a");
        fprintf(file, "%d;%f\n", nsum, esum);
        fclose(file);

        std::cout << "Muon saved" << std::endl;

        // Saving Resolution
        file = fopen(f_titleR, "a");
        fprintf(file, "%f;%f\n", abs(sumx / nf), abs(sumy / nf));
        fclose(file);

        std::cout << "position resolution saved" << std::endl;

        // std::cout << i << "/" << n_events << std::endl;
        // std::cout << "No Of Primary Electrons : " << nsum << std::endl;
        // std::cout << "Electons at Electrode : " << nf << std::endl;
        // std::cout << "gain : " << nf / nsum << std::endl;
      }
    }
  }

  // Geometry and Track Visualization
  TCanvas *c1 = new TCanvas("tr", "Track");
  ViewFEMesh *vFE = new ViewFEMesh();

  vFE->SetComponent(fm);
  vFE->SetCanvas(c1);
  vFE->SetPlane(0, -1, 0, 0, 0, 0);
  vFE->SetFillMesh(true);
  vFE->SetColor(2, kCyan - 3);
  vFE->SetColor(1, kOrange + 7);
  vFE->SetColor(0, kOrange + 7);
  vFE->EnableAxes();
  vFE->SetXaxisTitle("x (cm)");
  vFE->SetYaxisTitle("z (cm)");
  vFE->SetArea(-0.5, -0.5, zmin, 0.5, 0.5, zmax);
  vFE->SetViewDrift(vDrift);
  vFE->Plot();
  c1->SaveAs("avalanch.png");

  // View Signal
  TCanvas *c3 = new TCanvas("c", "Current");
  ViewSignal *signalView = new ViewSignal();
  signalView->SetSensor(sensor);
  signalView->SetCanvas(c3);
  signalView->PlotSignal("WT");
  c3->SaveAs("signal.png");

}
