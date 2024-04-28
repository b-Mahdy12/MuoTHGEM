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
// $Id: GarfieldDetectorConstruction.cc 999992 2015-12-11 14:47:43Z dpfeiffe $
//
/// \file GarfieldDetectorConstruction.cc
/// \brief Implementation of the GarfieldDetectorConstruction class

#include <math.h>

#include "GarfieldG4FastSimulationModel.hh"
#include "GarfieldDetectorConstruction.hh"
#include "GarfieldMessenger.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GarfieldDetectorConstruction::GarfieldDetectorConstruction() : G4VUserDetectorConstruction(), fGlassPV(0), fGas1PV(0), fGas2PV(0), freadoutPV(
																																	   0),
															   fAbsorberMaterial(0), fAbsorberLV(0), fCheckOverlaps(false), fGarfieldG4FastSimulationModel(
																																0),
															   fGarfieldMessenger(0)
{
	fGarfieldMessenger = new GarfieldMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GarfieldDetectorConstruction::~GarfieldDetectorConstruction()
{
	delete fGarfieldMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume *GarfieldDetectorConstruction::Construct()
{
	// Define materials
	DefineMaterials();

	// Define volumes
	return DefineVolumes();
	// G4double worldSizeXYZ = 1000 * cm;

	// // Get materials
	// G4Material *defaultMaterial = G4Material::GetMaterial("G4_AIR");
	// G4Material *gasMaterial = G4Material::GetMaterial("ArCO2_70_30");
	// G4Material *metalMaterial = G4Material::GetMaterial("G4_Cu");
	// G4Material *glassMaterial = G4Material::GetMaterial("FR4");

	// //
	// // World
	// //
	// G4VSolid *worldS = new G4Box("World",													  // its name
	// 							 0.5 * worldSizeXYZ, 0.5 * worldSizeXYZ, 0.5 * worldSizeXYZ); // its size

	// G4LogicalVolume *worldLV = new G4LogicalVolume(worldS,			// its solid
	// 											   defaultMaterial, // its material
	// 											   "World");		// its name

	// G4VPhysicalVolume *worldPV = new G4PVPlacement(0,				// no rotation
	// 											   G4ThreeVector(), // at (0,0,0)
	// 											   worldLV,			// its logical volume
	// 											   "World",			// its name
	// 											   0,				// its mother  volume
	// 											   false,			// no boolean operation
	// 											   0,				// copy number
	// 											   fCheckOverlaps); // checking overlaps

	// // -----------------------------------------------------------------------------------------------

	// // ! Construct the TH-GEM
	// G4double dpz = 1135 * mm;
	// G4double dpx1 = 484.088 * mm;
	// G4double dpx2 = 285.489 * mm;

	// // dpy1 = dpy2 = plate thickness
	// G4double pitch = 1 * mm;
	// G4double fr4 = 0.4 * mm;
	// G4double metal = 0.018 * mm;
	// G4double holedia = 0.50 * mm;
	// G4double drift = 3.0 * mm;
	// G4double induct = 1.0 * mm;
	// G4double rim = 0.60 * mm;

	// // Rotation
	// G4RotationMatrix *rotationPlacement = new G4RotationMatrix();
	// // rotationPlacement->rotateY(M_PI / 2.0);
	// rotationPlacement->rotateX(0);

	// // Drift Plate
	// G4VSolid *driftS = new G4Trd("metal1", dpx1, dpx2, metal, metal, dpz);
	// G4LogicalVolume *driftLV = new G4LogicalVolume(driftS, metalMaterial, "driftBoardLog", 0, 0, 0, fCheckOverlaps);
	// //G4VPhysicalVolume *driftBoardPhy = new G4PVPlacement(0,G4ThreeVector(0,metal,dpz),"",driftBoardLog,worldLV,false,0,fCheckOverlaps);
	// G4VPhysicalVolume *driftPV = new G4PVPlacement(rotationPlacement, G4ThreeVector(0, 0, 0), driftLV, "driftBoard", worldLV, false, 0, fCheckOverlaps);

	// G4VSolid *gasGap1 = new G4Trd("g1", dpx1, dpx2, drift, drift, dpz);
	// gas1LV = new G4LogicalVolume(gasGap1, gasMaterial, "driftRegion", 0, 0, 0, fCheckOverlaps);
	// fGas1PV = new G4PVPlacement(rotationPlacement, G4ThreeVector(0, -(metal + drift), 0), gas1LV, "drift", worldLV, false, 0, fCheckOverlaps);

	// G4VSolid *THGEM = new G4Trd("fr4", dpx1, dpx2, fr4, fr4, dpz);
	// G4LogicalVolume *THGEMLog = new G4LogicalVolume(THGEM, glassMaterial, "THGEMLog", 0, 0, 0, fCheckOverlaps);
	// fGlassPV = new G4PVPlacement(rotationPlacement, G4ThreeVector(0, -(metal + 2 * drift + fr4), 0), THGEMLog, "THGEM", worldLV, false, 0, fCheckOverlaps);

	// G4VSolid *gasGap2 = new G4Trd("g2", dpx1, dpx2, induct, induct, dpz);
	// gas2LV = new G4LogicalVolume(gasGap2, gasMaterial, "inductRegion", 0, 0, 0, fCheckOverlaps);
	// fGas2PV = new G4PVPlacement(rotationPlacement, G4ThreeVector(0, -(metal + 2 * drift + 2 * fr4 + induct), 0), gas2LV, "induct", worldLV, false, 0, fCheckOverlaps);

	// G4VSolid *inductBoard = new G4Trd("metal2", dpx1, dpx2, metal, metal, dpz);
	// G4LogicalVolume *indutBoardLog = new G4LogicalVolume(inductBoard, metalMaterial, "inductBourd", 0, 0, 0, fCheckOverlaps);
	// //	// G4VPhysicalVolume *inductBoardPhy = new G4PVPlacement(0, G4ThreeVector(0, metal + drift + fr4 + induct + metal, 0), indutBoardLog, "inductBoard", worldLV, false, 0, fCheckOverlaps);
	// // G4VPhysicalVolume *inductBoardPhy = new G4PVPlacement(rotationPlacement, G4ThreeVector(0, -(metal + 2 * drift + 2 * fr4 + 2 * induct + metal), 0), indutBoardLog, "inductBoard", worldLV, false, 0, fCheckOverlaps);
	// // G4PVReplica repX("Linear Array",
	// // 				 indutBoardLog,
	// // 				 worldLV,
	// // 				 kXAxis, 5, 10 * mm);

	// G4PVReplica repR("RSlices",
	// 				 indutBoardLog,
	// 				 worldLV,
	// 				 kRho, 5, 10 * mm, 0);
	// G4int rows = 10;
	// G4int columns = 10;

	// // G4VSolid *readout = new G4Box("microCell", 1 * mm, 1 * mm, 1 * mm);
	// G4VSolid *readout = new G4Trd("drift", dpx1 / columns, dpx2 / columns, metal, metal, dpz / rows);
	// G4LogicalVolume *readOutLog = new G4LogicalVolume(readout, metalMaterial, "ReadOut", 0, 0, 0, fCheckOverlaps);

	// for (G4int i = 0; i < rows; i++)
	// {
	// 	for (G4int j = 0; j < columns; j++)
	// 	{
	// 		G4VPhysicalVolume *readOutPhy = new G4PVPlacement(0, G4ThreeVector(-0.5 * (i + 0.5) * mm / rows, 0, -0.5 * (j + 0.5) * mm / columns), readOutLog, "readoutPhy", indutBoardLog, false, j + i * rows, fCheckOverlaps);
	// 		// G4VPhysicalVolume *readOutPhy = new G4PVPlacement(0, G4ThreeVector(1,j, 0), readOutLog, "readoutPhy", indutBoardLog, false, j + i * rows, fCheckOverlaps);
	// 	}
	// }

	// //
	// // Visualization attributes
	// //
	// worldLV->SetVisAttributes(G4VisAttributes::Invisible);

	// G4VisAttributes *VisAttBlue = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0));
	// G4VisAttributes *VisAttYellow = new G4VisAttributes(G4Colour(1.0, 0, 1.0));
	// G4VisAttributes *VisAttRed = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0));
	// // G4VisAttributes *VisAttWhite = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0));

	// // VisAttBlue->SetForceSolid(true);
	// // VisAttYellow->SetForceSolid(true);

	// readOutLog->SetVisAttributes(VisAttRed);
	// indutBoardLog->SetVisAttributes(VisAttYellow);
	// driftLV->SetVisAttributes(VisAttYellow);
	// gas1LV->SetVisAttributes(VisAttBlue);
	// gas2LV->SetVisAttributes(VisAttBlue);

	// //
	// // Define the Regions
	// //
	// G4Region *regionGarfield = new G4Region("RegionGarfield");
	// regionGarfield->AddRootLogicalVolume(gas1LV);
	// regionGarfield->AddRootLogicalVolume(gas2LV);

	// // G4Region *regionWire = new G4Region("RegionWire");
	// // regionWire->AddRootLogicalVolume(readOutLog);

	// fGarfieldG4FastSimulationModel = new GarfieldG4FastSimulationModel("GarfieldG4FastSimulationModel", regionGarfield);

	// //fGarfieldG4FastSimulationModel->WriteGeometryToGDML(fGasPV);

	// //
	// // Always return the physical World
	// //
	// return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GarfieldDetectorConstruction::DefineMaterials()
{
	G4bool isotopes = false;
	G4String name, symbol;
	G4int ncomponents, natoms;
	G4double density, fractionmass;

	// Lead material defined using NIST Manager
	G4NistManager *nistManager = G4NistManager::Instance();

	// nistManager->FindOrBuildMaterial("G4_Pb");
	nistManager->FindOrBuildMaterial("G4_Cu");
	nistManager->FindOrBuildMaterial("G4_Al");
	// nistManager->FindOrBuildMaterial("G4_Au");
	// nistManager->FindOrBuildMaterial("G4_W");

	nistManager->FindOrBuildMaterial("G4_AIR");

	G4Element *H = nistManager->FindOrBuildElement("H", isotopes);
	G4Element *N = nistManager->FindOrBuildElement("N", isotopes);
	G4Element *C = nistManager->FindOrBuildElement("C", isotopes);
	G4Element *O = nistManager->FindOrBuildElement("O", isotopes);
	G4Element *Ar = nistManager->FindOrBuildElement("Ar", isotopes);
	G4Element *Si = nistManager->FindOrBuildElement("Si", isotopes);

	G4Material *CO2 = new G4Material("CO2",
									 density = 1.977 * CLHEP::mg / CLHEP::cm3, ncomponents = 2);
	CO2->AddElement(C, natoms = 1);
	CO2->AddElement(O, natoms = 2);

	G4Material *ArCO2_70_30 = new G4Material("ArCO2_70_30",
											 density = 1.8223 * CLHEP::mg / CLHEP::cm3, ncomponents = 2,
											 kStateGas);

	ArCO2_70_30->AddElement(Ar, fractionmass = 0.70);
	ArCO2_70_30->AddMaterial(CO2, fractionmass = 0.30);

	G4Material *ArCO2_93_7 = new G4Material("ArCO2_93_7",
											density = 1.8223 * CLHEP::mg / CLHEP::cm3, ncomponents = 2);
	ArCO2_93_7->AddElement(Ar, fractionmass = 0.93);
	ArCO2_93_7->AddMaterial(CO2, fractionmass = 0.07);

	density = 1.413 * CLHEP::g / CLHEP::cm3;
	G4Material *Kapton = new G4Material(name = "Kapton", density, ncomponents = 4);
	Kapton->AddElement(O, 5);
	Kapton->AddElement(C, 22);
	Kapton->AddElement(N, 2);
	Kapton->AddElement(H, 10);

	// Siliconoxide(for FR4)
	G4Material *SiO2 = new G4Material("quartz", density = 2.200 * g / cm3, ncomponents = 2);
	SiO2->AddElement(Si, 1);
	SiO2->AddElement(O, 2);

	//Epoxy (for FR4 )
	density = 1.2 * g / cm3;
	G4Material *Epoxy = new G4Material("Epoxy", density, ncomponents = 2);
	Epoxy->AddElement(H, 2);
	Epoxy->AddElement(C, 2);

	//FR4 (Glass + Epoxy)
	density = 1.86 * g / cm3;
	G4Material *FR4 = new G4Material("FR4", density, ncomponents = 2);
	FR4->AddMaterial(Epoxy, 0.472);
	FR4->AddMaterial(SiO2, 0.528);

	fAbsorberMaterial = FR4;

	// Print materials
	G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume *GarfieldDetectorConstruction::DefineVolumes()
{
	// Geometry parameters
	G4double worldSizeXYZ = 1000 * cm;

	// Get materials
	G4Material *defaultMaterial = G4Material::GetMaterial("G4_AIR");
	G4Material *gasMaterial = G4Material::GetMaterial("ArCO2_70_30");
	G4Material *metalMaterial = G4Material::GetMaterial("G4_Cu");
	G4Material *glassMaterial = G4Material::GetMaterial("FR4");

	//
	// World
	//
	G4VSolid *worldS = new G4Box("World",													  // its name
								 0.5 * worldSizeXYZ, 0.5 * worldSizeXYZ, 0.5 * worldSizeXYZ); // its size

	G4LogicalVolume *worldLV = new G4LogicalVolume(worldS,			// its solid
												   defaultMaterial, // its material
												   "World");		// its name

	G4VPhysicalVolume *worldPV = new G4PVPlacement(0,				// no rotation
												   G4ThreeVector(), // at (0,0,0)
												   worldLV,			// its logical volume
												   "World",			// its name
												   0,				// its mother  volume
												   false,			// no boolean operation
												   0,				// copy number
												   fCheckOverlaps); // checking overlaps

	// -----------------------------------------------------------------------------------------------

	// ! Construct the TH-GEM
	G4double dpz = 1135 * mm;
	G4double dpx1 = 484.088 * mm;
	G4double dpx2 = 285.489 * mm;

	// dpy1 = dpy2 = plate thickness
	G4double pitch = 1 * mm;
	G4double fr4 = 0.4 * mm;
	G4double metal = 0.018 * mm;
	G4double holedia = 0.50 * mm;
	G4double drift = 3.0 * mm;
	G4double induct = 1.0 * mm;
	G4double rim = 0.60 * mm;

	// Drift Plate
	G4VSolid *driftBoard = new G4Trd("drift", dpx1, dpx2, metal, metal, dpz);
	G4LogicalVolume *driftBoardLog = new G4LogicalVolume(driftBoard, metalMaterial, "driftBoard", 0, 0, 0, fCheckOverlaps);
	//G4VPhysicalVolume *driftBoardPhy = new G4PVPlacement(0,G4ThreeVector(0,metal,dpz),"",driftBoardLog,worldLV,false,0,fCheckOverlaps);
	G4VPhysicalVolume *driftBoardPhy = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), driftBoardLog, "driftBoard", worldLV, false, 0, fCheckOverlaps);

	G4VSolid *gasGap1 = new G4Trd("g1", dpx1, dpx2, drift, drift, dpz);
	gas1LV = new G4LogicalVolume(gasGap1, gasMaterial, "driftRegion", 0, 0, 0, fCheckOverlaps);
	fGas1PV = new G4PVPlacement(0, G4ThreeVector(0, metal, 0), gas1LV, "drift", worldLV, false, 0, fCheckOverlaps);

	G4VSolid *THGEM = new G4Trd("fr4", dpx1, dpx2, fr4, fr4, dpz);
	G4LogicalVolume *THGEMLog = new G4LogicalVolume(THGEM, glassMaterial, "THGEMLog", 0, 0, 0, fCheckOverlaps);
	fGlassPV = new G4PVPlacement(0, G4ThreeVector(0, metal + drift, 0), THGEMLog, "THGEM", worldLV, false, 0, fCheckOverlaps);

	G4VSolid *gasGap2 = new G4Trd("g2", dpx1, dpx2, induct, induct, dpz);
	gas2LV = new G4LogicalVolume(gasGap2, gasMaterial, "inductRegion", 0, 0, 0, fCheckOverlaps);
	fGas2PV = new G4PVPlacement(0, G4ThreeVector(0, metal + drift + fr4, 0), gas2LV, "induct", worldLV, false, 0, fCheckOverlaps);

	G4VSolid *inductBoard = new G4Trd("induct", dpx1, dpx2, metal, metal, dpz);
	G4LogicalVolume *indutBoardLog = new G4LogicalVolume(inductBoard, metalMaterial, "inductBourd", 0, 0, 0, fCheckOverlaps);
	// //G4VPhysicalVolume *inductBoardPhy = new G4PVPlacement(0, G4ThreeVector(0, metal + drift + fr4 + induct + metal, 0), indutBoardLog, "inductBoard", worldLV, false, 0, fCheckOverlaps);
	G4VPhysicalVolume *inductBoardPhy = new G4PVPlacement(0, G4ThreeVector(0, metal + drift + fr4 + induct, 0), indutBoardLog, "inductBoard", worldLV, false, 0, fCheckOverlaps);
	
	
	// !Spatial resolution simulation
	G4int rows = 100;
	G4int columns = 100;
	// G4VSolid *readout = new G4Box("microCell", 1 * mm, 1 * mm, 1 * mm);
	G4VSolid *readout = new G4Trd("drift", dpx1 / columns, dpx2 / columns, metal, metal, dpz / rows);
	G4LogicalVolume *readOutLog = new G4LogicalVolume(readout, metalMaterial, "ReadOut", 0, 0, 0, fCheckOverlaps);
	//  for (G4int i = 0; i < rows; i++)
	//  {
	// for (G4int j = 0; j < columns; j++)
	// {
	// G4VPhysicalVolume *readOutPhy = new G4PVPlacement(0, G4ThreeVector(-0.5 * (i + 0.5) * mm / rows, 0, -0.5 * (j + 0.5) * mm / columns), readOutLog, "readoutPhy", indutBoardLog, false, j + i * rows, fCheckOverlaps);
	// // G4VPhysicalVolume *readOutPhy = new G4PVPlacement(0, G4ThreeVector(1, j, 0), readOutLog, "readoutPhy", indutBoardLog, false, j + i * rows, fCheckOverlaps);
	// }
	//  }

	//
	// Visualization attributes
	//
	// worldLV->SetVisAttributes(G4VisAttributes::Invisible);

	G4VisAttributes *VisAttBlue = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0));
	G4VisAttributes *VisAttYellow = new G4VisAttributes(G4Colour(0.0, 1.0, 1.0));
	G4VisAttributes *VisAttRed = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0));
	G4VisAttributes *VisAttWhite = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0));

	// readOutLog->SetVisAttributes(VisAttRed);
	indutBoardLog->SetVisAttributes(VisAttYellow);
	driftBoardLog->SetVisAttributes(VisAttYellow);
	gas1LV->SetVisAttributes(VisAttBlue);
	gas2LV->SetVisAttributes(VisAttBlue);

	//
	// Define the Regions
	//
	G4Region *regionGarfield = new G4Region("RegionGarfield");
	regionGarfield->AddRootLogicalVolume(gas1LV);
	regionGarfield->AddRootLogicalVolume(gas2LV);

	// G4Region *regionWire = new G4Region("RegionWire");
	// regionWire->AddRootLogicalVolume(readOutLog);

	fGarfieldG4FastSimulationModel = new GarfieldG4FastSimulationModel("GarfieldG4FastSimulationModel", regionGarfield);

	//fGarfieldG4FastSimulationModel->WriteGeometryToGDML(fGasPV);

	//
	// Always return the physical World
	//
	return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4Material *GarfieldDetectorConstruction::AbsorberMaterialWithSingleIsotope(
	G4String name, G4String symbol, G4double density, G4int Z, G4int A)
{
	// define a material from an isotope
	//
	G4int ncomponents;
	G4double abundance, massfraction;

	G4Isotope *isotope = new G4Isotope(symbol, Z, A);

	G4Element *element = new G4Element(name, symbol, ncomponents = 1);
	element->AddIsotope(isotope, abundance = 100. * perCent);

	G4Material *material = new G4Material(name, density, ncomponents = 1);
	material->AddElement(element, massfraction = 100. * perCent);

	return material;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GarfieldDetectorConstruction::SetAbsorberMaterial(
	G4String materialChoice)
{
	// search the material by its name
	G4Material *newMaterial = G4NistManager::Instance()->FindOrBuildMaterial(
		materialChoice);

	if (newMaterial)
	{
		if (fAbsorberMaterial != newMaterial)
		{
			fAbsorberMaterial = newMaterial;
			if (fAbsorberLV)
			{
				fAbsorberLV->SetMaterial(fAbsorberMaterial);
			}
			G4RunManager::GetRunManager()->PhysicsHasBeenModified();
		}
	}
	else
	{
		G4cout
			<< "\n--> warning from GarfieldDetectorConstruction::SetMaterial : "
			<< materialChoice << " not found" << G4endl;
	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
