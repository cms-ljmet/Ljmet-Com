import FWCore.ParameterSet.Config as cms

# ---------------------------------------------------------
from FWCore.ParameterSet.VarParsing import VarParsing
options = VarParsing('analysis')
options.inputFiles = ['root://cmsxrootd.fnal.gov//store/mc/RunIIFall17MiniAODv2/TprimeTprime_M-1800_TuneCP5_13TeV-madgraph-pythia8/MINIAODSIM/PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/80000/12A585B9-F46B-E811-A775-FA163EFD0C51.root']
options.maxEvents = 1000
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
process.GlobalTag = GlobalTag(process.GlobalTag, '94X_mc2017_realistic_v17', '')
print 'Using global tag', process.GlobalTag.globaltag

from PhysicsTools.PatAlgos.tools.jetTools import updateJetCollection
from RecoBTag.MXNet.pfDeepBoostedJet_cff import _pfDeepBoostedJetTagsAll as pfDeepBoostedJetTagsAll

updateJetCollection(
    process,
    jetSource = cms.InputTag('slimmedJetsAK8'),
    pvSource = cms.InputTag('offlineSlimmedPrimaryVertices'),
    svSource = cms.InputTag('slimmedSecondaryVertices'),
    rParam = 0.8,
    jetCorrections = ('AK8PFPuppi', cms.vstring(['L2Relative', 'L3Absolute','L2L3Residual']), 'None'),
    btagDiscriminators = pfDeepBoostedJetTagsAll
    #postfix = 'AK8Puppi',
    #printWarning = False
)

process.prefiringweight = cms.EDProducer("L1ECALPrefiringWeightProducer",
                                         ThePhotons = cms.InputTag("slimmedPhotons"),
                                         TheJets = cms.InputTag("slimmedJets"),
                                         L1Maps = cms.string("/uscms_data/d3/jmanagan/CMSSW_9_4_11/src/L1Prefiring/EventWeightProducer/files/L1PrefiringMaps_new.root"),
                                         DataEra = cms.string("2017BtoF"),
                                         UseJetEMPt = cms.bool(False),
                                         PrefiringRateSystematicUncty = cms.double(0.2)
)

process.p = cms.Path(process.updatedPatJetsAK8Puppi+process.prefiringweight)

process.output = cms.OutputModule(
                "PoolOutputModule",
                #fileName = cms.untracked.string('CONDOR_MEDIATOR'),
                fileName = cms.untracked.string('testnew_deepak8.root'),
                #outputCommands = cms.untracked.vstring(['keep *','keep *_deepntuplizer_*_test']),
                outputCommands = cms.untracked.vstring(['keep *']),
                )

process.output_step = cms.EndPath(process.output)
process.scedule = cms.Schedule(
    process.p,
    process.output_step)

