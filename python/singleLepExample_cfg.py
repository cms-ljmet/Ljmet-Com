import FWCore.ParameterSet.Config as cms
import FWCore.PythonUtilities.LumiList as LumiList
import FWCore.ParameterSet.Types as CfgTypes

# Define the base process
process = cms.Process("LJMetCom")

# Save the working area to a variable -- EDIT USER DIRECTORY HERE
relBase    = str('/uscms_data/d3/jmanagan/CleanLJMet74X')
############################################################
#
# FWLite application options
process.load('LJMet.Com.ljmet_cfi')
process.ljmet.isMc = cms.bool(True)

# Exclude some unnecessary calculators from the process
process.ljmet.excluded_calculators = cms.vstring(
        'TpTpCalc',
	'DileptonCalc',
	'StopCalc',
	'PdfCalc',
	'ChargedHiggsCalc',
	'TprimeCalc',
	'LjetsTopoCalc',
	'WprimeCalc'
	) 

# common calculator options
process.load('LJMet.Com.commonCalc_cfi')

# Stop calculator options
process.load('LJMet.Com.stopCalc_cfi')

# singleLep calculator options
process.load('LJMet.Com.singleLepCalc_cfi')
process.singleLepCalc.isWJets = cms.bool(False)

# LjetsTopoCalc options
process.load('LJMet.Com.ljetsTopoCalcNew_cfi')
process.LjetsTopoCalcNew.useBestTop = cms.bool(True)

# Stop calculator options
process.load('LJMet.Com.JetSubCalc_cfi')


############################################################
#
# Event selector options
#
process.event_selector = cms.PSet(

    selection = cms.string('singleLepSelector'),

    # Define cuts -- variable names are strings searched by src/singleLepEventSelector.cc

    debug  = cms.bool(False),

    isMc  = cms.bool(True),
    keepFullMChistory = cms.bool(False),
    doLaserCalFilt  = cms.bool(False),

    # Trigger cuts
    trigger_cut  = cms.bool(True),
    dump_trigger = cms.bool(False),
    
    mctrigger_path_el = cms.vstring('HLT_Ele32_eta2p1_WP75_Gsf_v1'),
    mctrigger_path_mu = cms.vstring('HLT_IsoMu24_eta2p1_v1'),
    trigger_path_el = cms.vstring('HLT_Ele32_eta2p1_WPLoose_Gsf_v1','HLT_Ele32_eta2p1_WPLoose_Gsf_v2'),
    trigger_path_mu = cms.vstring('HLT_IsoMu24_eta2p1_v1','HLT_IsoMu24_eta2p1_v2'),

    # PV cuts
    pv_cut         = cms.bool(True),
    hbhe_cut       = cms.bool(True),
    hbheiso_cut    = cms.bool(True),
    hbhe_cut_value = cms.string('Run2Loose'),
    csc_cut        = cms.bool(True),
    eesc_cut       = cms.bool(True),
    flag_tag       = cms.InputTag('TriggerResults::PAT'),

    # Jet cuts
    jet_cuts                 = cms.bool(True),
    jet_minpt                = cms.double(30.0),
    jet_maxeta               = cms.double(4.7),
    min_jet                  = cms.int32(1),
    max_jet                  = cms.int32(4000),
    leading_jet_pt           = cms.double(100.0),

    # muon cuts
    muon_cuts                = cms.bool(True),
    muon_selector            = cms.bool(True),
    muon_selector_medium     = cms.bool(False),
    muon_reliso              = cms.double(0.12),
    muon_minpt               = cms.double(25.0),
    muon_maxeta              = cms.double(2.1),
    min_muon                 = cms.int32(0),
    loose_muon_selector      = cms.bool(True),
    loose_muon_selector_tight = cms.bool(False),
    loose_muon_reliso        = cms.double(0.12),
    loose_muon_minpt         = cms.double(10.0),
    loose_muon_maxeta        = cms.double(2.4),

    # electron cuts
    electron_cuts            = cms.bool(True),
    electron_minpt           = cms.double(30.0),
    electron_maxeta          = cms.double(2.5),
    min_electron             = cms.int32(0),
    loose_electron_minpt     = cms.double(20.0),
    loose_electron_maxeta    = cms.double(2.5),
    tight_electron_mva_cuts   = cms.vdouble(0.913286,0.805013,0.358969),
    loose_electron_mva_cuts   = cms.vdouble(0.913286,0.805013,0.358969),
    
    # more lepton cuts
    min_lepton               = cms.int32(1),
    max_lepton               = cms.int32(1),    
    second_lepton_veto       = cms.bool(True),
    tau_veto		     = cms.bool(False),

    # MET cuts
    met_cuts                 = cms.bool(False),
    min_met                  = cms.double(20.0),
    
    # Btagging cuts
    btag_cuts                = cms.bool(True),
    btag_1                   = cms.bool(False),
    btag_2                   = cms.bool(False),
    btag_3                   = cms.bool(False),

    # Define the branch names of object collections in the input miniAOD file
    trigger_collection       = cms.InputTag('TriggerResults::HLT'),
    pv_collection            = cms.InputTag('offlineSlimmedPrimaryVertices'),
    jet_collection           = cms.InputTag('slimmedJets'),
    muon_collection          = cms.InputTag('slimmedMuons'),
    electron_collection      = cms.InputTag('slimmedElectrons'),
    tau_collection	     = cms.InputTag('slimmedTaus'),
    met_collection           = cms.InputTag('slimmedMETs'),

    # Jet corrections are read from txt files which need updating!
    BTagUncertUp             = cms.bool(False),
    BTagUncertDown           = cms.bool(False),
    JECup                    = cms.bool(False),
    JECdown                  = cms.bool(False),
    JERup                    = cms.bool(False),
    JERdown                  = cms.bool(False),
    JEC_txtfile = cms.string(relBase+'/src/LJMet/Com/data/Summer15_50nsV5_DATA_UncertaintySources_AK4PFchs.txt'),
    doNewJEC                 = cms.bool(True),
    doLepJetCleaning         = cms.bool(True),
    UseElMVA                 = cms.bool(True),

    MCL1JetPar               = cms.string(relBase+'/src/LJMet/Com/data/Summer15_50nsV5_MC_L1FastJet_AK4PFchs.txt'),
    MCL2JetPar               = cms.string(relBase+'/src/LJMet/Com/data/Summer15_50nsV5_MC_L2Relative_AK4PFchs.txt'),
    MCL3JetPar               = cms.string(relBase+'/src/LJMet/Com/data/Summer15_50nsV5_MC_L3Absolute_AK4PFchs.txt'),

    MCL1JetParAK8            = cms.string(relBase+'/src/LJMet/Com/data/Summer15_50nsV5_MC_L1FastJet_AK8PFchs.txt'),
    MCL2JetParAK8            = cms.string(relBase+'/src/LJMet/Com/data/Summer15_50nsV5_MC_L2Relative_AK8PFchs.txt'),
    MCL3JetParAK8            = cms.string(relBase+'/src/LJMet/Com/data/Summer15_50nsV5_MC_L3Absolute_AK8PFchs.txt'),

    DataL1JetPar             = cms.string(relBase+'/src/LJMet/Com/data/Summer15_50nsV5_DATA_L1FastJet_AK4PFchs.txt'),
    DataL2JetPar             = cms.string(relBase+'/src/LJMet/Com/data/Summer15_50nsV5_DATA_L2Relative_AK4PFchs.txt'),
    DataL3JetPar             = cms.string(relBase+'/src/LJMet/Com/data/Summer15_50nsV5_DATA_L3Absolute_AK4PFchs.txt'),
    DataResJetPar            = cms.string(relBase+'/src/LJMet/Com/data/Summer15_50nsV5_DATA_L2L3Residual_AK4PFchs.txt'),

    DataL1JetParAK8          = cms.string(relBase+'/src/LJMet/Com/data/Summer15_50nsV5_DATA_L1FastJet_AK8PFchs.txt'),
    DataL2JetParAK8          = cms.string(relBase+'/src/LJMet/Com/data/Summer15_50nsV5_DATA_L2Relative_AK8PFchs.txt'),
    DataL3JetParAK8          = cms.string(relBase+'/src/LJMet/Com/data/Summer15_50nsV5_DATA_L3Absolute_AK8PFchs.txt'),
    DataResJetParAK8         = cms.string(relBase+'/src/LJMet/Com/data/Summer15_50nsV5_DATA_L2L3Residual_AK8PFchs.txt')
    )


#######################################################
#
# Input files
#

process.inputs = cms.PSet (
    nEvents    = cms.int32(100),
    skipEvents = cms.int32(0),
    lumisToProcess = CfgTypes.untracked(CfgTypes.VLuminosityBlockRange()),
    
    # files can be read locally or from the grid, in which case you need a proxy to run.
    fileNames  = cms.vstring('root://xrootd.unl.edu//store/mc/RunIISpring15DR74/TTJets_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/Asympt50ns_MCRUN2_74_V9A-v1/60000/262BAE9C-E7FE-E411-8B48-1CC1DE1D16E6.root')
)
# JSON
if (not process.ljmet.isMc==cms.bool(True)):
    JsonFile = relBase+'/src/LJMet/Com/data/json/Cert_246908-251883_13TeV_PromptReco_Collisions15_JSON_v2.txt'
    myList   = LumiList.LumiList(filename=JsonFile).getCMSSWString().split(',')
    process.inputs.lumisToProcess.extend(myList)
        
                
#######################################################
#
# Output
#
process.outputs = cms.PSet (
    outputName = cms.string('ljmet_tree'),
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

# jets
process.load('PhysicsTools.SelectorUtils.pfJetIDSelector_cfi') 
process.pfJetIDSelector.version = cms.string('FIRSTDATA')
process.pfJetIDSelector.quality = cms.string('LOOSE')

# Tight muon
process.load('LJMet.Com.pfMuonSelector_cfi') 
process.pfMuonSelector.maxPfRelIso = cms.double(0.2)
process.pfMuonSelector.cutsToIgnore = cms.vstring('TrackerMuon','Chi2')

# Loose muon
process.LoosepfMuonSelector = process.pfMuonSelector.clone()

# Tight electron
process.load('LJMet.Com.TopElectronSelector_cfi')

# Loose electron -- this overrides the default "TIGHT" setting in TopElectronSelector
process.LooseTopElectronSelector = process.TopElectronSelector.clone()
process.LooseTopElectronSelector.version = cms.string('VETO')

