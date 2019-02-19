import FWCore.ParameterSet.Config as cms
from PhysicsTools.PatAlgos.tools.helpers import getPatAlgosToolsTask
import os

CONDOR_ISMC = False
CONDOR_ISTTBAR = False

relBase = os.environ['CMSSW_BASE']

from FWCore.ParameterSet.VarParsing import VarParsing
options = VarParsing('analysis')
options.register('isMC', '', VarParsing.multiplicity.singleton, VarParsing.varType.bool, 'Is MC') 
options.register('isTTbar', '', VarParsing.multiplicity.singleton, VarParsing.varType.bool, 'Is TTbar') 
options.isMC = CONDOR_ISMC
options.isTTbar = CONDOR_ISTTBAR
options.inputFiles = [
    'root://cmsxrootd.fnal.gov//store/data/Run2017B/SingleMuon/MINIAOD/31Mar2018-v1/80000/4462B46E-653C-E811-BA97-0025905A6064.root'
    #'root://cmsxrootd.fnal.gov//store/mc/RunIIFall17MiniAODv2/TprimeTprime_M-1800_TuneCP5_13TeV-madgraph-pythia8/MINIAODSIM/PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/80000/12A585B9-F46B-E811-A775-FA163EFD0C51.root'
    #'root://cmsxrootd.fnal.gov//store/mc/RunIIFall17MiniAODv2/TTToSemiLeptonic_TuneCP5_PSweights_13TeV-powheg-pythia8/MINIAODSIM/PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/50000/5E7E4AA9-0743-E811-999A-0CC47A7C35A8.root'
    ]
options.maxEvents = 200
options.parseArguments()

process = cms.Process("PATtest")

## MessageLogger
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100

## Options and Output Report
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

## Source
process.source = cms.Source("PoolSource", fileNames=cms.untracked.vstring(options.inputFiles)
)
## Maximal Number of Events
process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(options.maxEvents))

## Geometry and Detector Conditions (needed for a few patTuple production steps)
process.load("Configuration.Geometry.GeometryRecoDB_cff")
process.load('Configuration.StandardSequences.Services_cff')
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '94X_mc2017_realistic_v17', '')
if options.isMC == False: process.GlobalTag = GlobalTag(process.GlobalTag, '94X_dataRun2_v11')
print 'Using global tag', process.GlobalTag.globaltag


## Output Module Configuration (expects a path 'p')
#from PhysicsTools.PatAlgos.patEventContent_cff import patEventContentNoCleaning
process.out = cms.OutputModule("PoolOutputModule",
                               fileName = cms.untracked.string('patTuple.root'),
                               outputCommands = cms.untracked.vstring('keep *')#, *patEventContentNoCleaning )
                               )

patAlgosToolsTask = getPatAlgosToolsTask(process)
process.outpath = cms.EndPath(process.out, patAlgosToolsTask)

## and add them to the event content
from PhysicsTools.PatAlgos.tools.jetTools import updateJetCollection
from RecoBTag.MXNet.pfDeepBoostedJet_cff import *

updateJetCollection(
   process,
   jetSource = cms.InputTag('slimmedJetsAK8'),
   pvSource = cms.InputTag('offlineSlimmedPrimaryVertices'),
   svSource = cms.InputTag('slimmedSecondaryVertices'),
   rParam = 0.8,
   jetCorrections = ('AK8PFPuppi', cms.vstring(['L2Relative', 'L3Absolute']), 'None'),
   btagDiscriminators = ['pfCombinedInclusiveSecondaryVertexV2BJetTags',
                         'pfDeepBoostedJetTags:probTbcq', 'pfDeepBoostedJetTags:probTbqq', 
                         'pfDeepBoostedJetTags:probWcq', 'pfDeepBoostedJetTags:probWqq', 
                         'pfDeepBoostedJetTags:probZbb', 'pfDeepBoostedJetTags:probZcc', 'pfDeepBoostedJetTags:probZqq', 
                         'pfDeepBoostedJetTags:probHbb', 'pfDeepBoostedJetTags:probHcc', 'pfDeepBoostedJetTags:probHqqqq', 
                         'pfDeepBoostedJetTags:probQCDbb', 'pfDeepBoostedJetTags:probQCDcc', 
                         'pfDeepBoostedJetTags:probQCDb', 'pfDeepBoostedJetTags:probQCDc', 
                         'pfDeepBoostedJetTags:probQCDothers', 
                         'pfDeepBoostedDiscriminatorsJetTags:TvsQCD', 'pfDeepBoostedDiscriminatorsJetTags:WvsQCD', 
                         'pfDeepBoostedDiscriminatorsJetTags:ZvsQCD', 'pfDeepBoostedDiscriminatorsJetTags:ZbbvsQCD', 
                         'pfDeepBoostedDiscriminatorsJetTags:HbbvsQCD', 'pfDeepBoostedDiscriminatorsJetTags:H4qvsQCD', 
                         'pfMassDecorrelatedDeepBoostedJetTags:probTbcq', 'pfMassDecorrelatedDeepBoostedJetTags:probTbqq', 
                         'pfMassDecorrelatedDeepBoostedJetTags:probWcq', 'pfMassDecorrelatedDeepBoostedJetTags:probWqq', 
                         'pfMassDecorrelatedDeepBoostedJetTags:probZbb', 'pfMassDecorrelatedDeepBoostedJetTags:probZcc', 'pfMassDecorrelatedDeepBoostedJetTags:probZqq', 
                         'pfMassDecorrelatedDeepBoostedJetTags:probHbb', 'pfMassDecorrelatedDeepBoostedJetTags:probHcc', 'pfMassDecorrelatedDeepBoostedJetTags:probHqqqq', 
                         'pfMassDecorrelatedDeepBoostedJetTags:probQCDbb', 'pfMassDecorrelatedDeepBoostedJetTags:probQCDcc', 
                         'pfMassDecorrelatedDeepBoostedJetTags:probQCDb', 'pfMassDecorrelatedDeepBoostedJetTags:probQCDc', 
                         'pfMassDecorrelatedDeepBoostedJetTags:probQCDothers', 
                         'pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:TvsQCD', 'pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:WvsQCD', 
                         'pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:ZHbbvsQCD', 'pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:ZHccvsQCD', 
                         'pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:bbvsLight', 'pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:ccvsLight'],
   postfix = 'AK8Puppi',
   printWarning = False
   )

#### Establish references between PATified fat jets and subjets using the BoostedJetMerger
process.updatedJetsAK8PuppiSoftDropPacked = cms.EDProducer("BoostedJetMerger",
                                                           jetSrc=cms.InputTag('selectedUpdatedPatJetsAK8Puppi'),
                                                           subjetSrc=cms.InputTag('slimmedJetsAK8PFPuppiSoftDropPacked','SubJets')
                                                           )

#### Pack fat jets with subjets
process.packedJetsAK8Puppi = cms.EDProducer("JetSubstructurePacker",
                                            jetSrc=cms.InputTag('selectedUpdatedPatJetsAK8Puppi'),
                                            distMax = cms.double(0.8),
                                            fixDaughters = cms.bool(False),
                                            algoTags = cms.VInputTag(cms.InputTag("updatedJetsAK8PuppiSoftDropPacked")),
                                            algoLabels =cms.vstring('SoftDropPuppi')
                                            )


process.prefiringweight = cms.EDProducer("L1ECALPrefiringWeightProducer",
                                         ThePhotons = cms.InputTag("slimmedPhotons"),
                                         TheJets = cms.InputTag("slimmedJets"),
                                         L1Maps = cms.string(relBase+"/src/L1Prefiring/EventWeightProducer/files/L1PrefiringMaps_new.root"),
                                         DataEra = cms.string("2017BtoF"),
                                         UseJetEMPt = cms.bool(False),
                                         PrefiringRateSystematicUncty = cms.double(0.2)
)

from RecoEgamma.EgammaTools.EgammaPostRecoTools import setupEgammaPostRecoSeq
setupEgammaPostRecoSeq(process,
                       runVID=True,
                       era='2017-Nov17ReReco')

from PhysicsTools.PatUtils.tools.runMETCorrectionsAndUncertainties import runMetCorAndUncFromMiniAOD

isData = True
if options.isMC == True: isData = False
runMetCorAndUncFromMiniAOD(
    process,
    isData = isData,
    fixEE2017 = True,
    fixEE2017Params = {'userawPt': True, 'ptThreshold':50.0, 'minEtaThreshold':2.65, 'maxEtaThreshold': 3.139},
    postfix = "ModifiedMET"
    )

process.load('RecoMET.METFilters.ecalBadCalibFilter_cfi')

baddetEcallist = cms.vuint32(
    [872439604,872422825,872420274,872423218,
     872423215,872416066,872435036,872439336,
     872420273,872436907,872420147,872439731,
     872436657,872420397,872439732,872439339,
     872439603,872422436,872439861,872437051,
     872437052,872420649,872421950,872437185,
     872422564,872421566,872421695,872421955,
     872421567,872437184,872421951,872421694,
     872437056,872437057,872437313,872438182,
     872438951,872439990,872439864,872439609,
     872437181,872437182,872437053,872436794,
     872436667,872436536,872421541,872421413,
     872421414,872421031,872423083,872421439] 
)

process.ecalBadCalibReducedMINIAODFilter = cms.EDFilter(
    "EcalBadCalibFilter",
    EcalRecHitSource = cms.InputTag("reducedEgamma:reducedEERecHits"),
    ecalMinEt        = cms.double(50.),
    baddetEcal    = baddetEcallist, 
    taggingMode = cms.bool(True),
    debug = cms.bool(False)
    )

# ----------------------- GenHFHadronMatcher -----------------                                                
if options.isTTbar == True:
    process.load("PhysicsTools.JetMCAlgos.GenHFHadronMatcher_cff")

    from PhysicsTools.JetMCAlgos.HadronAndPartonSelector_cfi import selectedHadronsAndPartons
    process.selectedHadronsAndPartons = selectedHadronsAndPartons.clone(
        particles = cms.InputTag("prunedGenParticles")
        )
    from PhysicsTools.JetMCAlgos.AK4PFJetsMCFlavourInfos_cfi import ak4JetFlavourInfos
    process.genJetFlavourInfos = ak4JetFlavourInfos.clone(
        jets = cms.InputTag("slimmedGenJets")
        )
    from PhysicsTools.JetMCAlgos.GenHFHadronMatcher_cff import matchGenBHadron
    process.matchGenBHadron = matchGenBHadron.clone(
        genParticles = cms.InputTag("prunedGenParticles"),
        jetFlavourInfos = "genJetFlavourInfos"
        )
    from PhysicsTools.JetMCAlgos.GenHFHadronMatcher_cff import matchGenCHadron
    process.matchGenCHadron = matchGenCHadron.clone(
        genParticles = cms.InputTag("prunedGenParticles"),
        jetFlavourInfos = "genJetFlavourInfos"
        )
    process.load("TopQuarkAnalysis.TopTools.GenTtbarCategorizer_cfi")
    process.categorizeGenTtbar.genJets = cms.InputTag("slimmedGenJets")

    process.ttbarcat = cms.Sequence(
        process.selectedHadronsAndPartons * process.genJetFlavourInfos * process.matchGenBHadron 
        * process.matchGenCHadron* ## gen HF flavour matching            
        process.categorizeGenTtbar  ## return already a categorization id for tt                  
        )

    process.p = cms.Path(process.fullPatMetSequenceModifiedMET * 
                         process.prefiringweight * 
                         process.egammaPostRecoSeq * 
                         process.updatedJetsAK8PuppiSoftDropPacked * 
                         process.packedJetsAK8Puppi *
                         process.ecalBadCalibReducedMINIAODFilter *
                         process.ttbarcat
                         )
else:
    process.p = cms.Path(process.fullPatMetSequenceModifiedMET * 
                         process.prefiringweight * 
                         process.egammaPostRecoSeq * 
                         process.updatedJetsAK8PuppiSoftDropPacked * 
                         process.packedJetsAK8Puppi *
                         process.ecalBadCalibReducedMINIAODFilter
                         )


#from Configuration.EventContent.EventContent_cff import MINIAODSIMEventContent
process.out.outputCommands.append('drop *_*_*_PATtest')
process.out.outputCommands.append('keep *_prefiringweight*_*_PATtest')
process.out.outputCommands.append('keep *_slimmedElectrons_*_PATtest')
process.out.outputCommands.append('keep *_packedJetsAK8Puppi*_*_PATtest')
process.out.outputCommands.append('keep *_*_SubJets_*')
process.out.outputCommands.append('keep *_slimmedMETsModifiedMET_*_PATtest')
process.out.outputCommands.append('keep *_ecalBadCalibReducedMINIAODFilter_*_PATtest')
process.out.outputCommands.append('keep int_categorize*_*_PATtest'),

process.out.fileName = 'test_deep_boosted_jet_MINIAODSIM.root'


process.scedule = cms.Schedule(
    process.p,
    process.outpath)
