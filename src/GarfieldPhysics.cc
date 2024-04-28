//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: GarfieldPhysics.cc 999996 2015-12-11 14:47:43Z dpfeiffe $
//
/// \file GarfieldPhysics.cc
/// \brief Implementation of the GarfieldPhysics class
#include "GarfieldPhysics.hh"

#include "TGeoManager.h"
#include "TGeoBBox.h"
#include "G4SystemOfUnits.hh"

#include "GarfieldAnalysis.hh"

GarfieldPhysics *GarfieldPhysics::fGarfieldPhysics = 0;
std::string ANSYSData = "../ANSYSrun"; // GEM folder
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

GarfieldPhysics *GarfieldPhysics::GetInstance()
{
	if (!fGarfieldPhysics)
	{
		fGarfieldPhysics = new GarfieldPhysics();
	}
	return fGarfieldPhysics;
}

void GarfieldPhysics::Dispose()
{
	delete fGarfieldPhysics;
	fGarfieldPhysics = 0;
}

GarfieldPhysics::GarfieldPhysics()
{
	fMapParticlesEnergyGeant4 = new MapParticlesEnergy();
	fMapParticlesEnergyGarfield = new MapParticlesEnergy();
	fSecondaryParticles = new std::vector<GarfieldParticle *>();
	fMediumMagboltz = 0;	//Garfield classes
	fSensor = 0;			//Garfield classes
	fAvalanche = 0;			//Garfield classes
	fDrift = 0;				//Garfield classes
	fComponentAnsys123 = 0; //Garfield classes
	fTrackHeed = 0;			//Garfield classes
	// fGeoManager = 0;		//Garfield classes
	// fGeometryRoot = 0;		//Garfield classes
	// fGeometrySimple = 0;	//Garfield classes
	// fTube = 0;				//Garfield classes
	createSecondariesInGeant4 = false;
	fIonizationModel = "PAIPhot";
}

GarfieldPhysics::~GarfieldPhysics()
{
	delete fMapParticlesEnergyGeant4;
	delete fMapParticlesEnergyGarfield;
	DeleteSecondaryParticles();
	delete fSecondaryParticles;
	delete fMediumMagboltz;
	delete fSensor;
	delete fAvalanche;
	delete fDrift;
	delete fComponentAnsys123;
	delete fTrackHeed;
	// delete fGeometryRoot;
	// delete fGeometrySimple;

	// std::cout << "Deconstructor GarfieldPhysics" << std::endl;
}

std::string GarfieldPhysics::GetIonizationModel()
{
	return fIonizationModel;
}

void GarfieldPhysics::SetIonizationModel(std::string model, bool useDefaults)
{
	if (model != "PAIPhot" && model != "PAI" && model != "Heed")
	{

		std::cout << "Unknown ionization model " << model << std::endl;
		std::cout << "Using PAIPhot as default model!" << std::endl;
		model = "PAIPhot";
	}
	fIonizationModel = model;

	if (fIonizationModel == "PAIPhot" || fIonizationModel == "PAI")
	{
		if (useDefaults == true)
		{
			//Particle types and energies for which the G4FastSimulationModel with Garfield++ is valid
			this->AddParticleName("e-", 1e-6, 2e-3, "garfield");
			this->AddParticleName("gamma", 1e-6, 1e+8, "garfield");

			//Particle types and energies for which the PAI or PAIPhot model is valid
			this->AddParticleName("e-", 0, 1e+8, "geant4");
			this->AddParticleName("e+", 0, 1e+8, "geant4");
			this->AddParticleName("mu-", 0, 1e+8, "geant4");
			this->AddParticleName("mu+", 0, 1e+8, "geant4");
			this->AddParticleName("proton", 0, 1e+8, "geant4");
			this->AddParticleName("pi+", 0, 1e+8, "geant4");
			this->AddParticleName("pi-", 0, 1e+8, "geant4");
			this->AddParticleName("alpha", 0, 1e+8, "geant4");
			this->AddParticleName("He3", 0, 1e+8, "geant4");
			this->AddParticleName("GenericIon", 0, 1e+8, "geant4");
		}
	}
	else if (fIonizationModel == "Heed")
	{
		if (useDefaults == true)
		{
			//Particle types and energies for which the G4FastSimulationModel with Garfield++ is valid
			this->AddParticleName("gamma", 1e-6, 1e+8, "garfield");
			this->AddParticleName("e-", 6e-2, 1e+7, "garfield");
			this->AddParticleName("e+", 6e-2, 1e+7, "garfield");
			this->AddParticleName("mu-", 1e+1, 1e+8, "garfield");
			this->AddParticleName("mu+", 1e+1, 1e+8, "garfield");
			this->AddParticleName("pi-", 2e+1, 1e+8, "garfield");
			this->AddParticleName("pi+", 2e+1, 1e+8, "garfield");
			this->AddParticleName("kaon-", 1e+1, 1e+8, "garfield");
			this->AddParticleName("kaon+", 1e+1, 1e+8, "garfield");
			this->AddParticleName("proton", 9.e+1, 1e+8, "garfield");
			this->AddParticleName("anti_proton", 9.e+1, 1e+8, "garfield");
			this->AddParticleName("deuteron", 2.e+2, 1e+8, "garfield");
			this->AddParticleName("alpha", 4.e+2, 1e+8, "garfield");
		}
	}
}

void GarfieldPhysics::AddParticleName(const std::string particleName,
									  double ekin_min_MeV, double ekin_max_MeV, std::string program)
{
	if (ekin_min_MeV >= ekin_max_MeV)
	{
		std::cout << "Ekin_min=" << ekin_min_MeV
				  << " keV is larger than Ekin_max=" << ekin_max_MeV << " keV"
				  << std::endl;
		return;
	}

	if (program == "garfield")
	{
		std::cout << "Garfield model (Heed) is applicable for G4Particle "
				  << particleName << " between " << ekin_min_MeV << " MeV and "
				  << ekin_max_MeV << " MeV" << std::endl;

		fMapParticlesEnergyGarfield->insert(
			std::make_pair(particleName,
						   std::make_pair(ekin_min_MeV, ekin_max_MeV)));
	}
	else
	{
		std::cout << fIonizationModel << " is applicable for G4Particle "
				  << particleName << " between " << ekin_min_MeV << " MeV and "
				  << ekin_max_MeV << " MeV" << std::endl;
		fMapParticlesEnergyGeant4->insert(
			std::make_pair(particleName,
						   std::make_pair(ekin_min_MeV, ekin_max_MeV)));
	}
}

bool GarfieldPhysics::FindParticleName(std::string name, std::string program)
{
	MapParticlesEnergy::iterator it;
	if (program == "garfield")
	{
		it = fMapParticlesEnergyGarfield->find(name);
		if (it != fMapParticlesEnergyGarfield->end())
		{
			return true;
		}
		return false;
	}
	else
	{
		it = fMapParticlesEnergyGeant4->find(name);
		if (it != fMapParticlesEnergyGeant4->end())
		{
			return true;
		}
		return false;
	}
}

bool GarfieldPhysics::FindParticleNameEnergy(std::string name, double ekin_MeV,
											 std::string program)
{
	MapParticlesEnergy::iterator it;
	if (program == "garfield")
	{
		it = fMapParticlesEnergyGarfield->find(name);
		if (it != fMapParticlesEnergyGarfield->end())
		{
			EnergyRange_MeV range = it->second;
			if (range.first <= ekin_MeV && range.second >= ekin_MeV)
			{
				return true;
			}
		}
		return false;
	}
	else
	{
		it = fMapParticlesEnergyGeant4->find(name);
		if (it != fMapParticlesEnergyGeant4->end())
		{
			EnergyRange_MeV range = it->second;
			if (range.first <= ekin_MeV && range.second >= ekin_MeV)
			{
				return true;
			}
		}
		return false;
	}
}

double GarfieldPhysics::GetMinEnergyMeVParticle(std::string name,
												std::string program)
{
	MapParticlesEnergy::iterator it;
	if (program == "garfield")
	{
		it = fMapParticlesEnergyGarfield->find(name);
		if (it != fMapParticlesEnergyGarfield->end())
		{
			EnergyRange_MeV range = it->second;
			return range.first;
		}
		return false;
	}
	else
	{
		it = fMapParticlesEnergyGeant4->find(name);
		if (it != fMapParticlesEnergyGeant4->end())
		{
			EnergyRange_MeV range = it->second;
			return range.first;
		}
	}
	return -1;
}

double GarfieldPhysics::GetMaxEnergyMeVParticle(std::string name,
												std::string program)
{
	MapParticlesEnergy::iterator it;
	if (program == "garfield")
	{
		it = fMapParticlesEnergyGarfield->find(name);
		if (it != fMapParticlesEnergyGarfield->end())
		{
			EnergyRange_MeV range = it->second;
			return range.second;
		}
	}
	else
	{
		it = fMapParticlesEnergyGeant4->find(name);
		if (it != fMapParticlesEnergyGeant4->end())
		{
			EnergyRange_MeV range = it->second;
			return range.second;
		}
	}
	return -1;
}

void GarfieldPhysics::InitializePhysics()
{

	fMediumMagboltz = new Garfield::MediumMagboltz();

	fMediumMagboltz->SetComposition("ar", 70., "co2", 30.);
	fMediumMagboltz->SetTemperature(293.15);
	fMediumMagboltz->SetPressure(760.);
	fMediumMagboltz->EnableDebugging();
	fMediumMagboltz->Initialise(true);
	fMediumMagboltz->DisableDebugging();
	// Set the Penning transfer efficiency.
	const double rPenning = 0.57;
	const double lambdaPenning = 0.;
	fMediumMagboltz->EnablePenningTransfer(rPenning, lambdaPenning, "ar");
	fMediumMagboltz->LoadGasFile("ar_70_co2_30_1000mbar.gas");

	fSensor = new Garfield::Sensor();
	fDrift = new Garfield::AvalancheMC();
	fAvalanche = new Garfield::AvalancheMicroscopic();
	fComponentAnsys123 = new Garfield::ComponentAnsys123();
	fSensor->SetArea(-10, -10, -1 * mm, 10, 10, 3 * mm);

	CreateGeometry();

	fDrift->SetSensor(fSensor);
	fAvalanche->SetSensor(fSensor);

	fTrackHeed = new Garfield::TrackHeed();
	fTrackHeed->EnableDebugging();
	fTrackHeed->SetSensor(fSensor);
	fTrackHeed->EnableElectricField();
	fTrackHeed->DisableMagneticField();
	fTrackHeed->EnablePhotonReabsorption();
	fTrackHeed->EnableDeltaElectronTransport();
}

void GarfieldPhysics::CreateGeometry()
{
	fComponentAnsys123->Initialise(ANSYSData + "/ELIST.lis", ANSYSData + "/NLIST.lis", ANSYSData + "/MPLIST.lis", ANSYSData + "/PRNSOL.lis", "mm");
	fComponentAnsys123->EnableMirrorPeriodicityX();
	fComponentAnsys123->EnableMirrorPeriodicityY();
	fComponentAnsys123->PrintRange();

	const unsigned int nMaterials = fComponentAnsys123->GetNumberOfMaterials();
	for (unsigned int i = 0; i < nMaterials; ++i)
	{
		const double eps = fComponentAnsys123->GetPermittivity(i);
		if (fabs(eps - 1.) < 1.e-3)
			fComponentAnsys123->SetMedium(i, fMediumMagboltz);
	}

	fSensor->AddComponent(fComponentAnsys123);
}

void GarfieldPhysics::DoIt(std::string particleName, double ekin_MeV,
						   double time, double x_cm, double y_cm, double z_cm, double dx,
						   double dy, double dz)
{

	G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
	fEnergyDeposit = 0;
	DeleteSecondaryParticles();

	const double pitch = 0.014;
	const double Zaxis = -1;

	double eKin_eV = ekin_MeV * 1e+6;

	double xc = 0., yc = 0., zc = 0., tc = 0.;
	// Number of electrons produced in a collision
	int nc = 0;
	// Energy loss in a collision
	double ec = 0.;
	// Dummy variable (not used at present)
	double extra = 0.;
	fEnergyDeposit = 0;
	if (fIonizationModel != "Heed" || particleName == "gamma")
	{
		if (particleName == "gamma")
		{
			fTrackHeed->TransportPhoton(x_cm, y_cm, z_cm, time, eKin_eV, dx, dy,
										dz, nc);
		}
		else
		{
			fTrackHeed->TransportDeltaElectron(x_cm, y_cm, z_cm, time, eKin_eV,
											   dx, dy, dz, nc);
			fEnergyDeposit = eKin_eV;
		}

		for (int cl = 0; cl < nc; cl++)
		{
			double xe, ye, ze, te;
			double ee, dxe, dye, dze;
			fTrackHeed->GetElectron(cl, xe, ye, ze, te, ee, dxe, dye, dze);

			if (ze <= Zaxis)
			{
				nsum++;
				if (particleName == "gamma")
				{
					fEnergyDeposit += fTrackHeed->GetW();
				}
				analysisManager->FillH3(1, ze * 10, xe * 10, ye * 10);
				if (createSecondariesInGeant4)
				{
					double newTime = te;
					if (newTime < time)
					{
						newTime += time;
					}
					fSecondaryParticles->push_back(
						new GarfieldParticle("e-", ee, newTime, xe, ye, ze, dxe,
											 dye, dze));
				}

				fDrift->DriftElectron(xe, ye, ze, te);

				double xe1, ye1, ze1, te1;
				double xe2, ye2, ze2, te2;

				int status;
				fDrift->GetElectronEndpoint(0, xe1, ye1, ze1, te1, xe2, ye2,
											ze2, te2, status);

				double e2 = 0.1;
				fAvalanche->AvalancheElectron(xe2, ye2, ze2, te2, e2, 0, 0, 0);

				int ne = 0, ni = 0;
				fAvalanche->GetAvalancheSize(ne, ni);
				fAvalancheSize += ne;
			}
		}
	}
	else
	{
		fTrackHeed->SetParticle(particleName);
		fTrackHeed->SetKineticEnergy(eKin_eV);
		fTrackHeed->NewTrack(x_cm, y_cm, z_cm, time, dx, dy, dz);
		G4cout << "initialPostion:" << x_cm << "," << y_cm << "," << z_cm << G4endl;
	
		while (fTrackHeed->GetCluster(xc, yc, zc, tc, nc, ec, extra))
		{
			if (zc <= Zaxis)
			{
				nsum += nc;
				fEnergyDeposit += ec;
				for (int cl = 0; cl < nc; cl++)
				{
					double xe, ye, ze, te;
					double ee, dxe, dye, dze;
					fTrackHeed->GetElectron(cl, xe, ye, ze, te, ee, dxe, dye,
											dze);
					if (ze <= Zaxis)
					{
						analysisManager->FillH3(1, ze * 10, xe * 10, ye * 10);
						if (createSecondariesInGeant4)
						{
							double newTime = te;
							if (newTime < time)
							{
								newTime += time;
							}
							fSecondaryParticles->push_back(
								new GarfieldParticle("e-", ee, newTime, xe, ye,
													 ze, dxe, dye, dze));
						}

						fDrift->DriftElectron(xe, ye, ze, te);

						double xe1, ye1, ze1, te1;
						double xe2, ye2, ze2, te2;

						int status;
						fDrift->GetElectronEndpoint(0, xe1, ye1, ze1, te1, xe2,
													ye2, ze2, te2, status);

						double e2 = 0.1;
						fAvalanche->AvalancheElectron(xe2, ye2, ze2, te2, e2, 0,
													  0, 0);

						int ne = 0, ni = 0;
						fAvalanche->GetAvalancheSize(ne, ni);
						fAvalancheSize += ne;
					}
				}
			}
		}
	}
	fGain = fAvalancheSize / nsum;
}

std::vector<GarfieldParticle *> *GarfieldPhysics::GetSecondaryParticles()
{
	return fSecondaryParticles;
}

void GarfieldPhysics::DeleteSecondaryParticles()
{
	if (!fSecondaryParticles->empty())
	{
		fSecondaryParticles->erase(fSecondaryParticles->begin(),
								   fSecondaryParticles->end());
	}
}
