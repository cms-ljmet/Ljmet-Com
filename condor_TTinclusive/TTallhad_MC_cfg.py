import FWCore.ParameterSet.Config as cms
import FWCore.PythonUtilities.LumiList as LumiList
import FWCore.ParameterSet.Types as CfgTypes

# Define the base process
process = cms.Process("LJMetCom")

#Arguments from condor submit script which are used more than once
condorIsMC = bool(CONDOR_ISMC)
relBase    = str('CONDOR_RELBASE')
condorJSON = str('CONDOR_JSON')
############################################################
#
# FWLite application options
process.load('LJMet.Com.ljmet_cfi')
process.ljmet.isMc = cms.bool(condorIsMC)

# Exclude some unnecessary calculators from the process                                                                                                                                         
process.ljmet.excluded_calculators = cms.vstring(
    'TpTpCalc',
    'singleLepCalc',
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
process.singleLepCalc.saveLooseLeps     = cms.bool(False)
process.singleLepCalc.saveGenHT     = cms.bool(True)
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
    
    debug  = cms.bool(False),
    
    isMc  = cms.bool(condorIsMC),
    keepFullMChistory = cms.bool(condorIsMC),
    doLaserCalFilt  = cms.bool(False),
    
    # Trigger cuts
    trigger_cut  = cms.bool(True),
    dump_trigger = cms.bool(False),

    mctrigger_path_el = cms.vstring(
        #'digitisation_step',
        'HLT_Physics',
        'HLT_PFHT900',
        'HLT_PFHT1050',
        ),

    mctrigger_path_mu = cms.vstring(
        #'digitisation_step',
        'HLT_Physics',
        'HLT_PFHT900',
        'HLT_PFHT1050',
        ),

    trigger_path_el = cms.vstring(''),
    trigger_path_mu = cms.vstring(''),   
    
    # PV cuts
    pv_cut         = cms.bool(True),
    flag_tag       = cms.InputTag('TriggerResults::PAT'),
    metfilters     = cms.bool(False),
    
    # Jet cuts
    jet_cuts                 = cms.bool(True),
    jet_minpt                = cms.double(20.0),
    jet_maxeta               = cms.double(5.0),
    jet_minpt_AK8            = cms.double(170.0),
    jet_maxeta_AK8           = cms.double(5.0),
    min_jet                  = cms.int32(1),
    max_jet                  = cms.int32(4000),
    leading_jet_pt           = cms.double(20.0),

    # muon cuts
    muon_cuts                = cms.bool(True),
    min_muon                 = cms.int32(0),
    muon_minpt               = cms.double(30.0),
    muon_maxeta              = cms.double(2.4),
    muon_useMiniIso          = cms.bool(True),
    muon_miniIso             = cms.double(999.9),
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
    electron_minpt           = cms.double(30.0),
    electron_maxeta          = cms.double(2.5),
    electron_useMiniIso      = cms.bool(True),
    electron_miniIso         = cms.double(0.1),
    loose_electron_miniIso   = cms.double(0.4),
    loose_electron_minpt     = cms.double(10.0),
    loose_electron_maxeta    = cms.double(2.5),
    UseElMVA                 = cms.bool(True),
    tight_electron_mva_cuts  = cms.vdouble(0.96165,8.75794,3.13902,0.93193,8.84606,3.59851,0.88993,10.12423,4.35279), # Fall17 noiso WP90 c, tau, A for EB1, EB2, and then EE
    loose_electron_mva_cuts  = cms.vdouble(-0.86,-0.81,-0.72), # Fall17 noiso WP HZZ exact cuts
    #tight_electron_mva_cuts  = cms.vdouble(0.97177,8.91285,1.97124,0.945875,8.83104,2.40850,0.89791,9.81408,4.17158), # Fall17 iso WP90 c, tau, A for EB1, EB2, and then EE
    #loose_electron_mva_cuts  = cms.vdouble(-0.83,-0.77,-0.69), # Fall17 iso WP HZZ exact cuts
 
    ElMVAweightFiles = cms.vstring(
        relBase+'/src/LJMet/Com/weights/EIDmva_EB1_10_2017_puinfo_BDT.weights.xml',
        relBase+'/src/LJMet/Com/weights/EIDmva_EB2_10_2017_puinfo_BDT.weights.xml',
        relBase+'/src/LJMet/Com/weights/EIDmva_EE_10_2017_puinfo_BDT.weights.xml', 
        ),

    ElMVAweightFiles_iso = cms.vstring(
        relBase+'/src/LJMet/Com/weights/EIDmva_EB1_10_2017_puinfo_iso_BDT.weights.xml',
        relBase+'/src/LJMet/Com/weights/EIDmva_EB2_10_2017_puinfo_iso_BDT.weights.xml',
        relBase+'/src/LJMet/Com/weights/EIDmva_EE_10_2017_puinfo_iso_BDT.weights.xml',
        ),

    # more lepton cuts
    min_lepton               = cms.int32(0),    # checks (N tight mu + N tight el) >= cut
    max_lepton               = cms.int32(0),    # checks (N tight mu + N tight el) <= cut
    min_loose_lepton         = cms.int32(0),
    max_loose_lepton         = cms.int32(1000),
    second_lepton_veto       = cms.bool(False),  #checks (N tight lep > 0) AND (N loose lep > 0), vetoes if there are loose leptons.
    tau_veto		     = cms.bool(False),
    
    # MET cuts
    met_cuts                 = cms.bool(False),
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
    slimmedJetsAK8           = cms.InputTag('slimmedJetsAK8'),
    muon_collection          = cms.InputTag('slimmedMuons'),
    electron_collection      = cms.InputTag('slimmedElectrons'),
    tau_collection	     = cms.InputTag('slimmedTaus'),
    met_collection           = cms.InputTag('slimmedMETs'),
    
    # Jet corrections are read from txt files which need updating!
    JEC_txtfile = cms.string(relBase+'/src/LJMet/Com/data/Fall17V6/Fall17_17Nov2017_V6_MC_Uncertainty_AK4PFchs.txt'),
    JERSF_txtfile = cms.string(relBase+'/src/LJMet/Com/data/Spring16V10/Spring16_25nsV10_MC_SF_AK4PFchs.txt'),
    JER_txtfile = cms.string(relBase+'/src/LJMet/Com/data/Spring16V10/Spring16_25nsV10_MC_PtResolution_AK4PFchs.txt'),
    JERAK8_txtfile = cms.string(relBase+'/src/LJMet/Com/data/Spring16V10/Spring16_25nsV10_MC_PtResolution_AK8PFchs.txt'),

    JECup                    = cms.bool(False),
    JECdown                  = cms.bool(False),
    JERup                    = cms.bool(False),
    JERdown                  = cms.bool(False),
    doNewJEC                 = cms.bool(True),
    doLepJetCleaning         = cms.bool(False),
    CleanLooseLeptons        = cms.bool(False),
    LepJetDR                 = cms.double(0.4),
    LepJetDRAK8              = cms.double(0.8),
    
    MCL1JetPar               = cms.string(relBase+'/src/LJMet/Com/data/Fall17V6/Fall17_17Nov2017_V6_MC_L1FastJet_AK4PFchs.txt'),
    MCL2JetPar               = cms.string(relBase+'/src/LJMet/Com/data/Fall17V6/Fall17_17Nov2017_V6_MC_L2Relative_AK4PFchs.txt'),
    MCL3JetPar               = cms.string(relBase+'/src/LJMet/Com/data/Fall17V6/Fall17_17Nov2017_V6_MC_L3Absolute_AK4PFchs.txt'),

    MCL1JetParAK8            = cms.string(relBase+'/src/LJMet/Com/data/Fall17V6/Fall17_17Nov2017_V6_MC_L1FastJet_AK8PFPuppi.txt'),
    MCL2JetParAK8            = cms.string(relBase+'/src/LJMet/Com/data/Fall17V6/Fall17_17Nov2017_V6_MC_L2Relative_AK8PFPuppi.txt'),
    MCL3JetParAK8            = cms.string(relBase+'/src/LJMet/Com/data/Fall17V6/Fall17_17Nov2017_V6_MC_L3Absolute_AK8PFPuppi.txt'),

    DataL1JetPar             = cms.string(relBase+'/src/LJMet/Com/data/Fall17V6/Fall17_17Nov2017B_V6_DATA_L1FastJet_AK4PFchs.txt'),
    DataL2JetPar             = cms.string(relBase+'/src/LJMet/Com/data/Fall17V6/Fall17_17Nov2017B_V6_DATA_L2Relative_AK4PFchs.txt'),
    DataL3JetPar             = cms.string(relBase+'/src/LJMet/Com/data/Fall17V6/Fall17_17Nov2017B_V6_DATA_L3Absolute_AK4PFchs.txt'),
    DataResJetPar            = cms.string(relBase+'/src/LJMet/Com/data/Fall17V6/Fall17_17Nov2017B_V6_DATA_L2L3Residual_AK4PFchs.txt'),

    DataL1JetParAK8          = cms.string(relBase+'/src/LJMet/Com/data/Fall17V6/Fall17_17Nov2017B_V6_DATA_L1FastJet_AK8PFPuppi.txt'),
    DataL2JetParAK8          = cms.string(relBase+'/src/LJMet/Com/data/Fall17V6/Fall17_17Nov2017B_V6_DATA_L2Relative_AK8PFPuppi.txt'),
    DataL3JetParAK8          = cms.string(relBase+'/src/LJMet/Com/data/Fall17V6/Fall17_17Nov2017B_V6_DATA_L3Absolute_AK8PFPuppi.txt'),
    DataResJetParAK8         = cms.string(relBase+'/src/LJMet/Com/data/Fall17V6/Fall17_17Nov2017B_V6_DATA_L2L3Residual_AK8PFPuppi.txt'),

    # Unused parameters
    muon_reliso              = cms.double(0.2),
    muon_selector            = cms.bool(False),
    muon_selector_medium     = cms.bool(False),
    loose_muon_selector      = cms.bool(False),
    loose_muon_selector_tight = cms.bool(False),
    loose_muon_reliso        = cms.double(0.4),
    electron_CutsPlusMVA     = cms.bool(False),
    BTagUncertUp             = cms.bool(False), # no longer needed
    BTagUncertDown           = cms.bool(False), # no longer needed
 
    )



#######################################################
#
# Input files
#

process.inputs = cms.PSet (
    nEvents    = cms.int32(-1),
    skipEvents = cms.int32(0),
    lumisToProcess = CfgTypes.untracked(CfgTypes.VLuminosityBlockRange()),
    fileNames  = cms.vstring('CONDOR_MEDIATOR')
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
    outputName = cms.string('CONDOR_OUTFILE'),
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
process.load('LJMet.Com.pfMuonSelector_cfi') #not used

# Loose muon
process.LoosepfMuonSelector = process.pfMuonSelector.clone() #not used

# Tight electron for 25ns
process.load('LJMet.Com.TopElectronSelector_cfi')
process.TopElectronSelector.version = cms.string('NONE') #not used
                   	       
#Loose electron for 25ns
process.LooseTopElectronSelector = process.TopElectronSelector.clone()
process.LooseTopElectronSelector.version = cms.string('NONE') #not used
