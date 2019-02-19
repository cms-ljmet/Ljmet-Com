import FWCore.ParameterSet.Config as cms
import FWCore.PythonUtilities.LumiList as LumiList
import FWCore.ParameterSet.Types as CfgTypes
import os

# Define the base process
process = cms.Process("LJMetCom")

#Arguments from condor submit script which are used more than once
condorIsMC = bool(True)
relBase    = os.environ['CMSSW_BASE']
condorJSON = str('Cert_294927-306462_13TeV_PromptReco_Collisions17_JSON.txt')
############################################################
#
# FWLite application options
process.load('LJMet.Com.ljmet_cfi')
process.ljmet.isMc = cms.bool(condorIsMC)

# Exclude some unnecessary calculators from the process
process.ljmet.excluded_calculators = cms.vstring(
    'PileUpCalc',
    'BTagSFCalc',
    'TprimeCalc',
    'CATopoCalc',
    'DileptonCalc',
    'DileptonEventSelector',
    'StopCalc',
    'PdfCalc',
    'ChargedHiggsCalc',
    'LjetsTopoCalc',
    'WprimeCalc'
    ) 

# common calculator options
process.load('LJMet.Com.commonCalc_cfi')

# BestCalc options
process.load('LJMet.Com.BestCalc_cfi')
process.BestCalc.dnnFile = cms.string(relBase+'/src/LJMet/Com/data/BEST_mlp.json')

# singleLep calculator options                                                                                                                                                                  
process.load('LJMet.Com.singleLepCalc_cfi')
process.singleLepCalc.isMc              = cms.bool(condorIsMC)
process.singleLepCalc.keepFullMChistory = cms.bool(condorIsMC)
process.singleLepCalc.UseElMVA          = cms.bool(True)
process.singleLepCalc.UseElIDV1         = cms.bool(True)
process.singleLepCalc.saveLooseLeps     = cms.bool(False)
process.singleLepCalc.saveGenHT     = cms.bool(True)
process.singleLepCalc.OverrideLHEWeights = cms.bool(True)
process.singleLepCalc.triggerCollection = cms.InputTag("TriggerResults::HLT")

# Jet substructure calculator options                                                                                                                                                           
process.load('LJMet.Com.JetSubCalc_cfi')
process.JetSubCalc.killHF = cms.bool(False)
process.JetSubCalc.isMc = cms.bool(condorIsMC)

############################################################
#
# Event selector options
#
process.event_selector = cms.PSet(
    
    selection = cms.string('singleLepSelector'),
    
    # Define cuts -- variable names are strings searched by src/singleLepEventSelector.cc
    
    debug  = cms.bool(True),
    
    isMc  = cms.bool(condorIsMC),
    keepFullMChistory = cms.bool(condorIsMC),
    doLaserCalFilt  = cms.bool(False),
    
    # Trigger cuts
    trigger_cut  = cms.bool(True),
    dump_trigger = cms.bool(False),

    mctrigger_path_el = cms.vstring(
        #'digitisation_step',
        'HLT_Ele35_WPTight_Gsf',
        'HLT_Ele38_WPTight_Gsf',
        'HLT_Ele40_WPTight_Gsf',
        'HLT_Ele28_eta2p1_WPTight_Gsf_HT150',
        'HLT_Ele15_IsoVVVL_PFHT450_PFMET50',
        'HLT_Ele15_IsoVVVL_PFHT450',
        'HLT_Ele50_IsoVVVL_PFHT450',
        'HLT_Ele15_IsoVVVL_PFHT600',
        'HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165',
        'HLT_Ele115_CaloIdVT_GsfTrkIdT'

        'HLT_Ele32_WPTight_Gsf',
        'HLT_Ele32_WPTight_Gsf_L1DoubleEG',
        'HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned',
        ),

    mctrigger_path_mu = cms.vstring(
        #'digitisation_step',
        'HLT_IsoMu24',
        'HLT_IsoMu24_eta2p1',
        'HLT_IsoMu27',
        'HLT_IsoMu30',
        'HLT_Mu50',
        'HLT_TkMu50',
        'HLT_Mu55',
        'HLT_Mu15_IsoVVVL_PFHT450_PFMET50',
        'HLT_Mu15_IsoVVVL_PFHT450',
        'HLT_Mu50_IsoVVVL_PFHT450',
        'HLT_Mu15_IsoVVVL_PFHT600',

        'HLT_IsoTkMu24',
        'HLT_IsoMu24_2p1',
        ),

    trigger_path_el = cms.vstring(''),
    trigger_path_mu = cms.vstring(''),   
    
    # PV cuts
    pv_cut         = cms.bool(True),
    flag_tag       = cms.InputTag('TriggerResults::PAT'),
    metfilters     = cms.bool(True),
    
    # Jet cuts
    jet_cuts                 = cms.bool(True),
    jet_minpt                = cms.double(20.0),
    jet_maxeta               = cms.double(3.0),
    jet_minpt_AK8            = cms.double(170.0),
    jet_maxeta_AK8           = cms.double(2.4),
    min_jet                  = cms.int32(1),
    max_jet                  = cms.int32(4000),
    leading_jet_pt           = cms.double(20.0),

    # muon cuts
    muon_cuts                = cms.bool(True),
    min_muon                 = cms.int32(0),
    muon_minpt               = cms.double(25.0),
    muon_maxeta              = cms.double(2.4),
    muon_useMiniIso          = cms.bool(True),
    muon_miniIso             = cms.double(999.9), #now using built-in flag
    loose_muon_miniIso       = cms.double(999.9),
    loose_muon_minpt         = cms.double(10.0),
    loose_muon_maxeta        = cms.double(2.4),
    # specify IP cuts
    muon_dxy                 = cms.double(0.2),
    muon_dz                  = cms.double(0.5),
    loose_muon_dxy           = cms.double(999999.),
    loose_muon_dz            = cms.double(999999.),

    # electron cuts
    electron_cuts            = cms.bool(True),
    min_electron             = cms.int32(0),
    electron_minpt           = cms.double(25.0),
    electron_maxeta          = cms.double(2.5),
    electron_useMiniIso      = cms.bool(True),
    electron_miniIso         = cms.double(0.1),
    loose_electron_miniIso   = cms.double(0.4),
    loose_electron_minpt     = cms.double(10.0),
    loose_electron_maxeta    = cms.double(2.5),
    UseElMVA                 = cms.bool(True),
    UseElIDV1                = cms.bool(True),
 
    # more lepton cuts
    min_lepton               = cms.int32(1),    # checks (N tight mu + N tight el) >= cut
    max_lepton               = cms.int32(1),    # checks (N tight mu + N tight el) <= cut
    min_loose_lepton         = cms.int32(0),
    max_loose_lepton         = cms.int32(1000),
    second_lepton_veto       = cms.bool(True),  #checks (N tight lep > 0) AND (N loose lep > 0), vetoes if there are loose leptons.
    tau_veto		     = cms.bool(False),
    
    # MET cuts
    met_cuts                 = cms.bool(True),
    min_met                  = cms.double(30.0),
    max_met                  = cms.double(99999999999.0),
    
    # Btagging cuts
    btagOP                   = cms.string('MEDIUM'),
    bdisc_min                = cms.double(0.4941),
    DeepCSVfile              = cms.string(relBase+'/src/LJMet/Com/data/DeepCSV_94XSF_V3_B_F.csv'),
    DeepCSVSubjetfile        = cms.string(relBase+'/src/LJMet/Com/data/subjet_DeepCSV_94XSF_V3_B_F.csv'),
    btag_cuts                = cms.bool(False),
    btag_1                   = cms.bool(False),
    btag_2                   = cms.bool(False),
    btag_3                   = cms.bool(False),
    
    # Define the branch names of object collections in the input miniAOD file
    trigger_collection       = cms.InputTag('TriggerResults::HLT'),
    pv_collection            = cms.InputTag('offlineSlimmedPrimaryVertices'),
    jet_collection           = cms.InputTag('slimmedJets'),
    slimmedJetsAK8           = cms.InputTag('slimmedJetsAK8'),#'packedJetsAK8Puppi'), #
    muon_collection          = cms.InputTag('slimmedMuons'),
    electron_collection      = cms.InputTag('slimmedElectrons'), #'slimmedElectrons::PATtest'), #
    tau_collection	     = cms.InputTag('slimmedTaus'),
    met_collection           = cms.InputTag('slimmedMETs'),
    
    # Jet corrections are read from txt files which need updating!
    JEC_txtfile = cms.string(relBase+'/src/LJMet/Com/data/Fall17V32/Fall17_17Nov2017_V32_MC_Uncertainty_AK4PFchs.txt'),
    JERSF_txtfile = cms.string(relBase+'/src/LJMet/Com/data/Fall17V3/Fall17_V3_MC_SF_AK4PFchs.txt'),
    JER_txtfile = cms.string(relBase+'/src/LJMet/Com/data/Fall17V3/Fall17_V3_MC_PtResolution_AK4PFchs.txt'),
    JERAK8_txtfile = cms.string(relBase+'/src/LJMet/Com/data/Fall17V3/Fall17_V3_MC_PtResolution_AK8PFPuppi.txt'),

    JECup                    = cms.bool(False),
    JECdown                  = cms.bool(False),
    JERup                    = cms.bool(False),
    JERdown                  = cms.bool(False),
    doNewJEC                 = cms.bool(True),
    doLepJetCleaning         = cms.bool(True),
    CleanLooseLeptons        = cms.bool(False),
    LepJetDR                 = cms.double(0.4),
    LepJetDRAK8              = cms.double(0.8),
    
    MCL1JetPar               = cms.string(relBase+'/src/LJMet/Com/data/Fall17V32/Fall17_17Nov2017_V32_MC_L1FastJet_AK4PFchs.txt'),
    MCL2JetPar               = cms.string(relBase+'/src/LJMet/Com/data/Fall17V32/Fall17_17Nov2017_V32_MC_L2Relative_AK4PFchs.txt'),
    MCL3JetPar               = cms.string(relBase+'/src/LJMet/Com/data/Fall17V32/Fall17_17Nov2017_V32_MC_L3Absolute_AK4PFchs.txt'),

    MCL1JetParAK8            = cms.string(relBase+'/src/LJMet/Com/data/Fall17V32/Fall17_17Nov2017_V32_MC_L1FastJet_AK8PFPuppi.txt'),
    MCL2JetParAK8            = cms.string(relBase+'/src/LJMet/Com/data/Fall17V32/Fall17_17Nov2017_V32_MC_L2Relative_AK8PFPuppi.txt'),
    MCL3JetParAK8            = cms.string(relBase+'/src/LJMet/Com/data/Fall17V32/Fall17_17Nov2017_V32_MC_L3Absolute_AK8PFPuppi.txt'),

    DataL1JetPar             = cms.string(relBase+'/src/LJMet/Com/data/Fall17V32/Fall17_17Nov2017B_V32_DATA_L1FastJet_AK4PFchs.txt'),
    DataL2JetPar             = cms.string(relBase+'/src/LJMet/Com/data/Fall17V32/Fall17_17Nov2017B_V32_DATA_L2Relative_AK4PFchs.txt'),
    DataL3JetPar             = cms.string(relBase+'/src/LJMet/Com/data/Fall17V32/Fall17_17Nov2017B_V32_DATA_L3Absolute_AK4PFchs.txt'),
    DataResJetPar            = cms.string(relBase+'/src/LJMet/Com/data/Fall17V32/Fall17_17Nov2017B_V32_DATA_L2L3Residual_AK4PFchs.txt'),

    DataL1JetParAK8          = cms.string(relBase+'/src/LJMet/Com/data/Fall17V32/Fall17_17Nov2017B_V32_DATA_L1FastJet_AK8PFPuppi.txt'),
    DataL2JetParAK8          = cms.string(relBase+'/src/LJMet/Com/data/Fall17V32/Fall17_17Nov2017B_V32_DATA_L2Relative_AK8PFPuppi.txt'),
    DataL3JetParAK8          = cms.string(relBase+'/src/LJMet/Com/data/Fall17V32/Fall17_17Nov2017B_V32_DATA_L3Absolute_AK8PFPuppi.txt'),
    DataResJetParAK8         = cms.string(relBase+'/src/LJMet/Com/data/Fall17V32/Fall17_17Nov2017B_V32_DATA_L2L3Residual_AK8PFPuppi.txt'),

    # Unused parameters
    muon_reliso              = cms.double(0.2),
    muon_selector_medium     = cms.bool(False),
    loose_muon_selector_tight = cms.bool(False),
    loose_muon_reliso        = cms.double(0.4),
    BTagUncertUp             = cms.bool(False), # no longer needed
    BTagUncertDown           = cms.bool(False), # no longer needed
 
    )


#######################################################
#
# Input files
#

process.inputs = cms.PSet (
    nEvents    = cms.int32(100),
    skipEvents = cms.int32(22300),
    lumisToProcess = CfgTypes.untracked(CfgTypes.VLuminosityBlockRange()),
    fileNames  = cms.vstring(
        #'test_deep_boosted_jet_MINIAODSIM.root',
        'root://cmsxrootd.fnal.gov//store/mc/RunIIFall17MiniAODv2/BprimeBprime_M-1000_TuneCP5_13TeV-madgraph-pythia8/MINIAODSIM/PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/00000/42313EDA-F08E-E811-B7B2-FA163E042BD9.root',
        #'root://cmsxrootd.fnal.gov//store/mc/RunIIFall17MiniAODv2/TprimeTprime_M-1800_TuneCP5_13TeV-madgraph-pythia8/MINIAODSIM/PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/80000/12A585B9-F46B-E811-A775-FA163EFD0C51.root',
        )
    )


# JSON
if (not process.ljmet.isMc==cms.bool(True)):
    JsonFile = relBase+'/src/LJMet/Com/data/json/'+condorJSON
    myList   = LumiList.LumiList(filename=JsonFile).getCMSSWString().split(',')
    process.inputs.lumisToProcess.extend(myList)
    
    
#######################################################
#
# Output
#
process.outputs = cms.PSet (
    outputName = cms.string('testmc'),
    treeName   = cms.string('ljmet'),
    )

#######################################################
#
# Object selector options
#
# Primary vertex
process.load('PhysicsTools.SelectorUtils.pvSelector_cfi')
process.pvSelector.pvSrc   = cms.InputTag('offlineSlimmedPrimaryVertices')
process.pvSelector.minNdof = cms.double(4.0)
process.pvSelector.maxZ    = cms.double(24.0)
process.pvSelector.maxRho  = cms.double(2.0)




