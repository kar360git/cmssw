import FWCore.ParameterSet.Config as cms

from Configuration.Eras.Era_Run3_dd4hep_cff import Run3_dd4hep
process = cms.Process('SIM',Run3_dd4hep)

#from Configuration.Eras.Era_Phase2C11_dd4hep_cff import Phase2C11_dd4hep
#process = cms.Process('SIM',Phase2C11_dd4hep)

process = cms.Process("G4PrintGeometry")

process.load('Configuration.Geometry.GeometryDD4hepExtended2021_cff')
#process.load('Configuration.Geometry.GeometryDD4hepExtended2026D76_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')

#if hasattr(process,'MessageLogger'):
#    process.MessageLogger.HCalGeom=dict()

from SimG4Core.PrintGeomInfo.g4TestGeometry_cfi import *
process = checkOverlap(process)

# enable Geant4 overlap check 
process.g4SimHits.CheckGeometry = True

# Geant4 geometry check 
process.g4SimHits.G4CheckOverlap.OutputBaseName = cms.string("cms2021")
process.g4SimHits.G4CheckOverlap.OverlapFlag = cms.bool(True)
process.g4SimHits.G4CheckOverlap.Tolerance  = cms.double(0.01)
process.g4SimHits.G4CheckOverlap.Resolution = cms.int32(10000)
process.g4SimHits.G4CheckOverlap.Depth      = cms.int32(-1)
# tells if NodeName is G4Region or G4PhysicalVolume
process.g4SimHits.G4CheckOverlap.RegionFlag = cms.bool(False)
# list of names
process.g4SimHits.G4CheckOverlap.NodeNames  = cms.vstring('cms:OCMS_1')
# enable dump gdml file 
process.g4SimHits.G4CheckOverlap.gdmlFlag   = cms.bool(False)
# if defined a G4PhysicsVolume info is printed
process.g4SimHits.G4CheckOverlap.PVname     = ''
# if defined a list of daughter volumes is printed
process.g4SimHits.G4CheckOverlap.LVname     = ''

# extra output files, created if a name is not empty
process.g4SimHits.FileNameField   = ''
process.g4SimHits.FileNameGDML    = ''
process.g4SimHits.FileNameRegions = ''
#
