import FWCore.ParameterSet.Config as cms

# ---------------------------------------------------------
from FWCore.ParameterSet.VarParsing import VarParsing
options = VarParsing('analysis')
options.register('isMC', '', VarParsing.multiplicity.singleton, VarParsing.varType.bool, 'Is MC') 
options.inputFiles = [CONDOR_FILELIST]
options.isMC = CONDOR_ISMC
options.maxEvents = -1
options.parseArguments()

# ---------------------------------------------------------
process = cms.Process("test")

process.load('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 5000

process.options = cms.untracked.PSet(
   allowUnscheduled = cms.untracked.bool(True),  
   wantSummary=cms.untracked.bool(False)
)

process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(options.maxEvents))

process.source = cms.Source('PoolSource',
    fileNames=cms.untracked.vstring(options.inputFiles)
)

# ---------------------------------------------------------
process.load("FWCore.MessageService.MessageLogger_cfi")
process.load("Configuration.EventContent.EventContent_cff")
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '94X_mc2017_realistic_v13', '')
if options.isMC == False: process.GlobalTag = GlobalTag(process.GlobalTag, '94X_dataRun2_v6')
print 'Using global tag', process.GlobalTag.globaltag

from PhysicsTools.PatAlgos.tools.jetTools import updateJetCollection
from RecoBTag.MXNet.pfDeepBoostedJet_cff import _pfDeepBoostedJetTagsAll as pfDeepBoostedJetTagsAll

updateJetCollection(
    process,
    jetSource = cms.InputTag('slimmedJetsAK8'),
    pvSource = cms.InputTag('offlineSlimmedPrimaryVertices'),
    svSource = cms.InputTag('slimmedSecondaryVertices'),
    rParam = 0.8,
    jetCorrections = ('AK8PFPuppi',cms.vstring(['L2Relative','L3Absolute','L2L3Residual']),'None'),
    btagDiscriminators = pfDeepBoostedJetTagsAll,
    postfix = 'AK8Puppi',
    printWarning = False
)

#process.p = cms.Path(process.deepntuplizer)

process.output = cms.OutputModule(
                "PoolOutputModule",
                fileName = cms.untracked.string('CONDOR_MEDIATOR'),
                #fileName = cms.untracked.string('testnewdeepak8.root'),
                outputCommands = cms.untracked.vstring(['keep *']),
                #outputCommands = cms.untracked.vstring(['keep *','keep *_deepntuplizer_*_test']),
                )

process.output_step = cms.EndPath(process.output)
process.scedule = cms.Schedule(
    #process.p,
    process.output_step)

