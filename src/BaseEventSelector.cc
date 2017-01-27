#include <math.h>

#include "LJMet/Com/interface/BaseEventSelector.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "DataFormats/Candidate/interface/Candidate.h"

using namespace std;

BaseEventSelector::BaseEventSelector():
mName(""),
mLegend("")
{
}

void BaseEventSelector::BeginJob(std::map<std::string, edm::ParameterSet const > par)
{
    std::string _key = "event_selector";
    bool _missing_config = false;
    if ( par.find(_key)!=par.end() ){
        if (par[_key].exists("isMc")) mbPar["isMc"] = par[_key].getParameter<bool> ("isMc");
        else mbPar["isMc"] = false;
        
        if (par[_key].exists("btagOP")) msPar["btagOP"] = par[_key].getParameter<std::string> ("btagOP");
        else msPar["btagOP"] = "CSVM";
        
        if (par[_key].exists("JECup")) mbPar["JECup"] = par[_key].getParameter<bool> ("JECup");
        else mbPar["JECup"] = false;
        if (par[_key].exists("JECdown")) mbPar["JECdown"] = par[_key].getParameter<bool> ("JECdown");
        else mbPar["JECdown"] = false;
        if (par[_key].exists("JERup")) mbPar["JERup"] = par[_key].getParameter<bool> ("JERup");
        else mbPar["JERup"] = false;
        if (par[_key].exists("JERdown")) mbPar["JERdown"] = par[_key].getParameter<bool> ("JERdown");
        else mbPar["JERdown"] = false;

	// Assumption (accurate as of Fall15_25nsV2) that JEC Unc and JER SF don't split AK4 and AK8
        if (par[_key].exists("JEC_txtfile")) msPar["JEC_txtfile"] = par[_key].getParameter<std::string> ("JEC_txtfile");
        else{
            msPar["JEC_txtfile"] = "";
            _missing_config = true;
        }
        if (par[_key].exists("JER_txtfile")) msPar["JER_txtfile"] = par[_key].getParameter<std::string> ("JER_txtfile");
        else msPar["JER_txtfile"] = "";
        if (par[_key].exists("JERAK8_txtfile")) msPar["JERAK8_txtfile"] = par[_key].getParameter<std::string> ("JERAK8_txtfile");
        else msPar["JERAK8_txtfile"] = "";
        if (par[_key].exists("JERSF_txtfile")) msPar["JERSF_txtfile"] = par[_key].getParameter<std::string> ("JERSF_txtfile");
        else msPar["JERSF_txtfile"] = "";

        if (par[_key].exists("BTagUncertUp")) mbPar["BTagUncertUp"] = par[_key].getParameter<bool> ("BTagUncertUp");
        else mbPar["BTagUncertUp"] = false;
        if (par[_key].exists("BTagUncertDown")) mbPar["BTagUncertDown"] = par[_key].getParameter<bool> ("BTagUncertDown");
        else mbPar["BTagUncertDown"] = false;

        if (par[_key].exists("MistagUncertUp")) mbPar["MistagUncertUp"] = par[_key].getParameter<bool> ("MistagUncertUp");
        else{
	  // default to the correlated version, uncertainty will be too large (better than too small)
	  if (par[_key].exists("BTagUncertUp")) mbPar["MistagUncertUp"] = par[_key].getParameter<bool> ("BTagUncertUp");
	  else mbPar["MistagUncertUp"] = false;
	}
        if (par[_key].exists("MistagUncertDown")) mbPar["MistagUncertDown"] = par[_key].getParameter<bool> ("MistagUncertDown");
        else{
	  // default to the correlated version, uncertainty will be too large (better than too small)
	  if (par[_key].exists("BTagUncertDown")) mbPar["MistagUncertDown"] = par[_key].getParameter<bool> ("BTagUncertDown");
	  else mbPar["MistagUncertDown"] = false;
	}
        
        if (par[_key].exists("MCL1JetPar")) msPar["MCL1JetPar"] = par[_key].getParameter<std::string> ("MCL1JetPar");
        else{
            msPar["MCL1JetPar"] = "../data/PHYS14_25_V2_L1FastJet_AK4PFchs.txt";
            _missing_config = true;
        }
        if (par[_key].exists("MCL2JetPar")) msPar["MCL2JetPar"] = par[_key].getParameter<std::string> ("MCL2JetPar");
        else{
            msPar["MCL2JetPar"] = "../data/PHYS14_25_V2_L2Relative_AK4PFchs.txt";
            _missing_config = true;
        }
        if (par[_key].exists("MCL3JetPar")) msPar["MCL3JetPar"] = par[_key].getParameter<std::string> ("MCL3JetPar");
        else{
            msPar["MCL3JetPar"] = "../data/PHYS14_25_V2_L3Absolute_AK4PFchs.txt";
            _missing_config = true;
        }
        if (par[_key].exists("MCL1JetParAK8")) msPar["MCL1JetParAK8"] = par[_key].getParameter<std::string> ("MCL1JetParAK8");
        else{
            msPar["MCL1JetParAK8"] = "../data/PHYS14_25_V2_L1FastJet_AK8PFchs.txt";
            _missing_config = true;
        }
        if (par[_key].exists("MCL2JetParAK8")) msPar["MCL2JetParAK8"] = par[_key].getParameter<std::string> ("MCL2JetParAK8");
        else{
            msPar["MCL2JetParAK8"] = "../data/PHYS14_25_V2_L2Relative_AK8PFchs.txt";
            _missing_config = true;
        }
        if (par[_key].exists("MCL3JetParAK8")) msPar["MCL3JetParAK8"] = par[_key].getParameter<std::string> ("MCL3JetParAK8");
        else{
            msPar["MCL3JetParAK8"] = "../data/PHYS14_25_V2_L3Absolute_AK8PFchs.txt";
            _missing_config = true;
        }
        
        if (par[_key].exists("DataL1JetPar")) msPar["DataL1JetPar"] = par[_key].getParameter<std::string> ("DataL1JetPar");
        else{
            msPar["DataL1JetPar"] = "../data/FT_53_V10_AN3_L1FastJet_AK5PFchs.txt";
            _missing_config = true;
        }
        if (par[_key].exists("DataL2JetPar")) msPar["DataL2JetPar"] = par[_key].getParameter<std::string> ("DataL2JetPar");
        else{
            msPar["DataL2JetPar"] = "../data/FT_53_V10_AN3_L2Relative_AK5PFchs.txt";
            _missing_config = true;
        }
        if (par[_key].exists("DataL3JetPar")) msPar["DataL3JetPar"] = par[_key].getParameter<std::string> ("DataL3JetPar");
        else{
            msPar["DataL3JetPar"] = "../data/FT_53_V10_AN3_L3Absolute_AK5PFchs.txt";
            _missing_config = true;
        }
        if (par[_key].exists("DataResJetPar")) msPar["DataResJetPar"] = par[_key].getParameter<std::string> ("DataResJetPar");
        else{
            msPar["DataResJetPar"] = "../data/FT_53_V10_AN3_L2L3Residual_AK5PFchs.txt";
            _missing_config = true;
        }
        if (par[_key].exists("DataL1JetParAK8")) msPar["DataL1JetParAK8"] = par[_key].getParameter<std::string> ("DataL1JetParAK8");
        else{
            msPar["DataL1JetParAK8"] = "../data/FT_53_V10_AN3_L1FastJet_AK5PFchs.txt";
            _missing_config = true;
        }
        if (par[_key].exists("DataL2JetParAK8")) msPar["DataL2JetParAK8"] = par[_key].getParameter<std::string> ("DataL2JetParAK8");
        else{
            msPar["DataL2JetParAK8"] = "../data/FT_53_V10_AN3_L2Relative_AK5PFchs.txt";
            _missing_config = true;
        }
        if (par[_key].exists("DataL3JetParAK8")) msPar["DataL3JetParAK8"] = par[_key].getParameter<std::string> ("DataL3JetParAK8");
        else{
            msPar["DataL3JetParAK8"] = "../data/FT_53_V10_AN3_L3Absolute_AK5PFchs.txt";
            _missing_config = true;
        }
        if (par[_key].exists("DataResJetParAK8")) msPar["DataResJetParAK8"] = par[_key].getParameter<std::string> ("DataResJetParAK8");
        else{
            msPar["DataResJetParAK8"] = "../data/FT_53_V10_AN3_L2L3Residual_AK5PFchs.txt";
            _missing_config = true;
        }
        if (par[_key].exists("doNewJEC")) mbPar["doNewJEC"] = par[_key].getParameter<bool> ("doNewJEC");
        else mbPar["doNewJEC"] = false;
        
        if (_missing_config) {
            std::cout << mLegend
            << "ONE OF THE FOLLOWING CONFIG OPTIONS MISSING!\n"
            << "MCL1JetPar, MCL2JetPar, MCL3JetPar, DataL1JetPar, DataL2JetPar,\n"
            << "DataL3JetPar, DataResJetPar" <<std::endl;
            std::cout << mLegend
            << "USING DEFAULT VALUES" << std::endl;
        }
        if (par[_key].exists("UseElMVA")) {
            mbPar["UseElMVA"] = par[_key].getParameter<bool> ("UseElMVA");
            if ( par[_key].exists("ElMVAweightFiles") )
                mvsPar["ElMVAweightFiles"] = par[_key].getParameter<std::vector<std::string> >("ElMVAweightFiles");
            if (mvsPar["ElMVAweightFiles"].size()!=3) {
                mvsPar["ElMVAweightFiles"].clear();
                mvsPar["ElMVAweightFiles"].push_back("../weights/EIDmva_EB1_10_oldNonTrigSpring15_ConvVarCwoBoolean_TMVA412_FullStatLowPt_PairNegWeightsGlobal_BDT.weights.xml");
                mvsPar["ElMVAweightFiles"].push_back("../weights/EIDmva_EB2_10_oldNonTrigSpring15_ConvVarCwoBoolean_TMVA412_FullStatLowPt_PairNegWeightsGlobal_BDT.weights.xml");
                mvsPar["ElMVAweightFiles"].push_back("../weights/EIDmva_EE_10_oldNonTrigSpring15_ConvVarCwoBoolean_TMVA412_FullStatLowPt_PairNegWeightsGlobal_BDT.weights.xml");
            }
            if ( par[_key].exists("ElMVAweightFiles_alt") )
                mvsPar["ElMVAweightFiles_alt"] = par[_key].getParameter<std::vector<std::string> >("ElMVAweightFiles_alt");
            if (mvsPar["ElMVAweightFiles_alt"].size()!=3) {
                mvsPar["ElMVAweightFiles_alt"].clear();
                mvsPar["ElMVAweightFiles_alt"].push_back("../weights/electronID_mva_Spring16_GeneralPurpose_V1_EB1_10.weights.xml");
                mvsPar["ElMVAweightFiles_alt"].push_back("../weights/electronID_mva_Spring16_GeneralPurpose_V1_EB2_10.weights.xml");
                mvsPar["ElMVAweightFiles_alt"].push_back("../weights/electronID_mva_Spring16_GeneralPurpose_V1_EE_10.weights.xml");
            }
            // these are for 25ns, and are up-to-date as of Sep 24 2015
            // this needs to be checked periodically, as well as the list of variables for the MVA
            // look here: https://github.com/cms-sw/cmssw/blob/CMSSW_7_4_X/RecoEgamma/ElectronIdentification/plugins/ElectronMVAEstimatorRun2Spring15NonTrig.cc
            // and here: https://github.com/cms-sw/cmssw/blob/CMSSW_7_4_X/RecoEgamma/ElectronIdentification/python/Identification/mvaElectronID_Spring15_25ns_nonTrig_V1_cff.py
            mtPar["pv_collection"] = par[_key].getParameter<edm::InputTag>("pv_collection");
        }
        else mbPar["UseElMVA"] = false;
    }
    
    msPar["btagger"] = mBtagCond.getAlgoName(msPar["btagOP"]);
    mdPar["btag_min_discr"] = mBtagCond.getDiscriminant(msPar["btagOP"]);
    
    bTagCut = mdPar["btag_min_discr"];
    std::cout << "b-tag check "<<msPar["btagOP"]<<" "<< msPar["btagger"]<<" "<<mdPar["btag_min_discr"]<<std::endl;
 
    if ( mbPar["isMc"] )
      jecUnc = new JetCorrectionUncertainty(msPar["JEC_txtfile"]);

    resolution = JME::JetResolution(msPar["JER_txtfile"]);
    resolutionAK8 = JME::JetResolution(msPar["JERAK8_txtfile"]);
    resolution_SF = JME::JetResolutionScaleFactor(msPar["JERSF_txtfile"]);    

    std::vector<JetCorrectorParameters> vPar;
    std::vector<JetCorrectorParameters> vParAK8;

    if ( mbPar["isMc"] ) {
        // Create the JetCorrectorParameter objects, the order does not matter.

        L3JetPar  = new JetCorrectorParameters(msPar["MCL3JetPar"]);
        L2JetPar  = new JetCorrectorParameters(msPar["MCL2JetPar"]);
    	L1JetPar  = new JetCorrectorParameters(msPar["MCL1JetPar"]);
        	   
	L3JetParAK8  = new JetCorrectorParameters(msPar["MCL3JetParAK8"]);
        L2JetParAK8  = new JetCorrectorParameters(msPar["MCL2JetParAK8"]);
    	L1JetParAK8  = new JetCorrectorParameters(msPar["MCL1JetParAK8"]);
    	// Load the JetCorrectorParameter objects into a std::vector,
    	// IMPORTANT: THE ORDER MATTERS HERE !!!! 
    	vPar.push_back(*L1JetPar);
    	vPar.push_back(*L2JetPar);
    	vPar.push_back(*L3JetPar);

    	vParAK8.push_back(*L1JetParAK8);
    	vParAK8.push_back(*L2JetParAK8);
    	vParAK8.push_back(*L3JetParAK8);

    }
    else if ( !mbPar["isMc"] ) {
        // Create the JetCorrectorParameter objects, the order does not matter.

        ResJetPar = new JetCorrectorParameters(msPar["DataResJetPar"]); 
    	L3JetPar  = new JetCorrectorParameters(msPar["DataL3JetPar"]);
    	L2JetPar  = new JetCorrectorParameters(msPar["DataL2JetPar"]);
    	L1JetPar  = new JetCorrectorParameters(msPar["DataL1JetPar"]);

        ResJetParAK8 = new JetCorrectorParameters(msPar["DataResJetParAK8"]); 
    	L3JetParAK8  = new JetCorrectorParameters(msPar["DataL3JetParAK8"]);
    	L2JetParAK8  = new JetCorrectorParameters(msPar["DataL2JetParAK8"]);
    	L1JetParAK8  = new JetCorrectorParameters(msPar["DataL1JetParAK8"]);
    	// Load the JetCorrectorParameter objects into a std::vector,
    	// IMPORTANT: THE ORDER MATTERS HERE !!!! 
    	vPar.push_back(*L1JetPar);
   	vPar.push_back(*L2JetPar);
    	vPar.push_back(*L3JetPar);
    	vPar.push_back(*ResJetPar);

    	vParAK8.push_back(*L1JetParAK8);
   	vParAK8.push_back(*L2JetParAK8);
    	vParAK8.push_back(*L3JetParAK8);
    	vParAK8.push_back(*ResJetParAK8);

    }
    if (mbPar["doNewJEC"]) std::cout << mLegend << "Applying new jet energy corrections" << std::endl;
    else std::cout << mLegend << "NOT applying new jet energy corrections - ARE YOU SURE?" << std::endl;

    JetCorrector = new FactorizedJetCorrector(vPar);
    JetCorrectorAK8 = new FactorizedJetCorrector(vParAK8);
 
    if (mbPar["UseElMVA"]) {

        tmpTMVAReader_EB.SetOptions("!Color:Silent:!Error");
        tmpTMVAReader_EE.SetOptions("!Color:Silent:!Error");

        // Pure ECAL -> shower shapes
        tmpTMVAReader_EB.AddVariable("ele_oldsigmaietaieta", &allMVAVars.see);
        tmpTMVAReader_EB.AddVariable("ele_oldsigmaiphiiphi", &allMVAVars.spp);
        tmpTMVAReader_EB.AddVariable("ele_oldcircularity",   &allMVAVars.OneMinusE1x5E5x5);
        tmpTMVAReader_EB.AddVariable("ele_oldr9",            &allMVAVars.R9);
        tmpTMVAReader_EB.AddVariable("ele_scletawidth",      &allMVAVars.etawidth);
        tmpTMVAReader_EB.AddVariable("ele_sclphiwidth",      &allMVAVars.phiwidth);
        tmpTMVAReader_EB.AddVariable("ele_he",               &allMVAVars.HoE);
        
        //Pure tracking variables
        tmpTMVAReader_EB.AddVariable("ele_kfhits",           &allMVAVars.kfhits);
        tmpTMVAReader_EB.AddVariable("ele_kfchi2",           &allMVAVars.kfchi2);
        tmpTMVAReader_EB.AddVariable("ele_gsfchi2",        &allMVAVars.gsfchi2);
      
        // Energy matching
        tmpTMVAReader_EB.AddVariable("ele_fbrem",           &allMVAVars.fbrem);
      
        tmpTMVAReader_EB.AddVariable("ele_gsfhits",         &allMVAVars.gsfhits);
        tmpTMVAReader_EB.AddVariable("ele_expected_inner_hits",             &allMVAVars.expectedMissingInnerHits);
        tmpTMVAReader_EB.AddVariable("ele_conversionVertexFitProbability",  &allMVAVars.convVtxFitProbability);
      
        tmpTMVAReader_EB.AddVariable("ele_ep",              &allMVAVars.EoP);
        tmpTMVAReader_EB.AddVariable("ele_eelepout",        &allMVAVars.eleEoPout);
        tmpTMVAReader_EB.AddVariable("ele_IoEmIop",         &allMVAVars.IoEmIoP);
        
        // Geometrical matchings
        tmpTMVAReader_EB.AddVariable("ele_deltaetain",      &allMVAVars.deta);
        tmpTMVAReader_EB.AddVariable("ele_deltaphiin",      &allMVAVars.dphi);
        tmpTMVAReader_EB.AddVariable("ele_deltaetaseed",    &allMVAVars.detacalo);

        // Spectator variables  
        tmpTMVAReader_EB.AddSpectator("ele_pT",             &allMVAVars.pt);
        tmpTMVAReader_EB.AddSpectator("ele_isbarrel",       &allMVAVars.isBarrel);
        tmpTMVAReader_EB.AddSpectator("ele_isendcap",       &allMVAVars.isEndcap);
        tmpTMVAReader_EB.AddSpectator("scl_eta",            &allMVAVars.SCeta);
      
        tmpTMVAReader_EB.AddSpectator("ele_eClass",                 &allMVAVars.eClass);
        tmpTMVAReader_EB.AddSpectator("ele_pfRelIso",               &allMVAVars.pfRelIso);
        tmpTMVAReader_EB.AddSpectator("ele_expected_inner_hits",    &allMVAVars.expectedInnerHits);
        tmpTMVAReader_EB.AddSpectator("ele_vtxconv",                &allMVAVars.vtxconv);
        tmpTMVAReader_EB.AddSpectator("mc_event_weight",            &allMVAVars.mcEventWeight);
        tmpTMVAReader_EB.AddSpectator("mc_ele_CBmatching_category", &allMVAVars.mcCBmatchingCategory);

        // Pure ECAL -> shower shapes
        tmpTMVAReader_EE.AddVariable("ele_oldsigmaietaieta", &allMVAVars.see);
        tmpTMVAReader_EE.AddVariable("ele_oldsigmaiphiiphi", &allMVAVars.spp);
        tmpTMVAReader_EE.AddVariable("ele_oldcircularity",   &allMVAVars.OneMinusE1x5E5x5);
        tmpTMVAReader_EE.AddVariable("ele_oldr9",            &allMVAVars.R9);
        tmpTMVAReader_EE.AddVariable("ele_scletawidth",      &allMVAVars.etawidth);
        tmpTMVAReader_EE.AddVariable("ele_sclphiwidth",      &allMVAVars.phiwidth);
        tmpTMVAReader_EE.AddVariable("ele_he",               &allMVAVars.HoE);
        // Endcap only variables
        tmpTMVAReader_EE.AddVariable("ele_psEoverEraw",    &allMVAVars.PreShowerOverRaw);
        
        //Pure tracking variables
        tmpTMVAReader_EE.AddVariable("ele_kfhits",           &allMVAVars.kfhits);
        tmpTMVAReader_EE.AddVariable("ele_kfchi2",           &allMVAVars.kfchi2);
        tmpTMVAReader_EE.AddVariable("ele_gsfchi2",        &allMVAVars.gsfchi2);
      
        // Energy matching
        tmpTMVAReader_EE.AddVariable("ele_fbrem",           &allMVAVars.fbrem);
      
        tmpTMVAReader_EE.AddVariable("ele_gsfhits",         &allMVAVars.gsfhits);
        tmpTMVAReader_EE.AddVariable("ele_expected_inner_hits",             &allMVAVars.expectedMissingInnerHits);
        tmpTMVAReader_EE.AddVariable("ele_conversionVertexFitProbability",  &allMVAVars.convVtxFitProbability);
      
        tmpTMVAReader_EE.AddVariable("ele_ep",              &allMVAVars.EoP);
        tmpTMVAReader_EE.AddVariable("ele_eelepout",        &allMVAVars.eleEoPout);
        tmpTMVAReader_EE.AddVariable("ele_IoEmIop",         &allMVAVars.IoEmIoP);
        
        // Geometrical matchings
        tmpTMVAReader_EE.AddVariable("ele_deltaetain",      &allMVAVars.deta);
        tmpTMVAReader_EE.AddVariable("ele_deltaphiin",      &allMVAVars.dphi);
        tmpTMVAReader_EE.AddVariable("ele_deltaetaseed",    &allMVAVars.detacalo);

        // Spectator variables  
        tmpTMVAReader_EE.AddSpectator("ele_pT",             &allMVAVars.pt);
        tmpTMVAReader_EE.AddSpectator("ele_isbarrel",       &allMVAVars.isBarrel);
        tmpTMVAReader_EE.AddSpectator("ele_isendcap",       &allMVAVars.isEndcap);
        tmpTMVAReader_EE.AddSpectator("scl_eta",            &allMVAVars.SCeta);
      
        tmpTMVAReader_EE.AddSpectator("ele_eClass",                 &allMVAVars.eClass);
        tmpTMVAReader_EE.AddSpectator("ele_pfRelIso",               &allMVAVars.pfRelIso);
        tmpTMVAReader_EE.AddSpectator("ele_expected_inner_hits",    &allMVAVars.expectedInnerHits);
        tmpTMVAReader_EE.AddSpectator("ele_vtxconv",                &allMVAVars.vtxconv);
        tmpTMVAReader_EE.AddSpectator("mc_event_weight",            &allMVAVars.mcEventWeight);
        tmpTMVAReader_EE.AddSpectator("mc_ele_CBmatching_category", &allMVAVars.mcCBmatchingCategory);

        tmpTMVAReader_EB.BookMVA( "Spring15_V1_EB1",  mvsPar["ElMVAweightFiles"].at(0) );
        tmpTMVAReader_EB.BookMVA( "Spring15_V1_EB2",  mvsPar["ElMVAweightFiles"].at(1) );
        tmpTMVAReader_EE.BookMVA(  "Spring15_V1_EE",   mvsPar["ElMVAweightFiles"].at(2) );

        //----alt----

        tmpTMVAReader_EB_alt.SetOptions("!Color:Silent:!Error");
        tmpTMVAReader_EE_alt.SetOptions("!Color:Silent:!Error");

        // Pure ECAL -> shower shapes
        tmpTMVAReader_EB_alt.AddVariable("ele_oldsigmaietaieta", &allMVAVars_alt.see);
        tmpTMVAReader_EB_alt.AddVariable("ele_oldsigmaiphiiphi", &allMVAVars_alt.spp);
        tmpTMVAReader_EB_alt.AddVariable("ele_oldcircularity",   &allMVAVars_alt.OneMinusE1x5E5x5);
        tmpTMVAReader_EB_alt.AddVariable("ele_oldr9",            &allMVAVars_alt.R9);
        tmpTMVAReader_EB_alt.AddVariable("ele_scletawidth",      &allMVAVars_alt.etawidth);
        tmpTMVAReader_EB_alt.AddVariable("ele_sclphiwidth",      &allMVAVars_alt.phiwidth);
        tmpTMVAReader_EB_alt.AddVariable("ele_oldhe",               &allMVAVars_alt.HoE);
        
        //Pure tracking variables
        tmpTMVAReader_EB_alt.AddVariable("ele_kfhits",           &allMVAVars_alt.kfhits);
        tmpTMVAReader_EB_alt.AddVariable("ele_kfchi2",           &allMVAVars_alt.kfchi2);
        tmpTMVAReader_EB_alt.AddVariable("ele_gsfchi2",        &allMVAVars_alt.gsfchi2);
      
        // Energy matching
        tmpTMVAReader_EB_alt.AddVariable("ele_fbrem",           &allMVAVars_alt.fbrem);
      
        tmpTMVAReader_EB_alt.AddVariable("ele_gsfhits",         &allMVAVars_alt.gsfhits);
        tmpTMVAReader_EB_alt.AddVariable("ele_expected_inner_hits",             &allMVAVars_alt.expectedMissingInnerHits);
        tmpTMVAReader_EB_alt.AddVariable("ele_conversionVertexFitProbability",  &allMVAVars_alt.convVtxFitProbability);
      
        tmpTMVAReader_EB_alt.AddVariable("ele_ep",              &allMVAVars_alt.EoP);
        tmpTMVAReader_EB_alt.AddVariable("ele_eelepout",        &allMVAVars_alt.eleEoPout);
        tmpTMVAReader_EB_alt.AddVariable("ele_IoEmIop",         &allMVAVars_alt.IoEmIoP);
        
        // Geometrical matchings
        tmpTMVAReader_EB_alt.AddVariable("ele_deltaetain",      &allMVAVars_alt.deta);
        tmpTMVAReader_EB_alt.AddVariable("ele_deltaphiin",      &allMVAVars_alt.dphi);
        tmpTMVAReader_EB_alt.AddVariable("ele_deltaetaseed",    &allMVAVars_alt.detacalo);

        // Spectator variables  
        tmpTMVAReader_EB_alt.AddVariable("ele_pt",              &allMVAVars_alt.pt);
        tmpTMVAReader_EB_alt.AddVariable("scl_eta",             &allMVAVars_alt.SCeta);
      
        // Pure ECAL -> shower shapes
        tmpTMVAReader_EE_alt.AddVariable("ele_oldsigmaietaieta", &allMVAVars_alt.see);
        tmpTMVAReader_EE_alt.AddVariable("ele_oldsigmaiphiiphi", &allMVAVars_alt.spp);
        tmpTMVAReader_EE_alt.AddVariable("ele_oldcircularity",   &allMVAVars_alt.OneMinusE1x5E5x5);
        tmpTMVAReader_EE_alt.AddVariable("ele_oldr9",            &allMVAVars_alt.R9);
        tmpTMVAReader_EE_alt.AddVariable("ele_scletawidth",      &allMVAVars_alt.etawidth);
        tmpTMVAReader_EE_alt.AddVariable("ele_sclphiwidth",      &allMVAVars_alt.phiwidth);
        tmpTMVAReader_EE_alt.AddVariable("ele_oldhe",            &allMVAVars_alt.HoE);
        
        //Pure tracking variables
        tmpTMVAReader_EE_alt.AddVariable("ele_kfhits",           &allMVAVars_alt.kfhits);
        tmpTMVAReader_EE_alt.AddVariable("ele_kfchi2",           &allMVAVars_alt.kfchi2);
        tmpTMVAReader_EE_alt.AddVariable("ele_gsfchi2",        &allMVAVars_alt.gsfchi2);
      
        // Energy matching
        tmpTMVAReader_EE_alt.AddVariable("ele_fbrem",           &allMVAVars_alt.fbrem);
      
        tmpTMVAReader_EE_alt.AddVariable("ele_gsfhits",         &allMVAVars_alt.gsfhits);
        tmpTMVAReader_EE_alt.AddVariable("ele_expected_inner_hits",             &allMVAVars_alt.expectedMissingInnerHits);
        tmpTMVAReader_EE_alt.AddVariable("ele_conversionVertexFitProbability",  &allMVAVars_alt.convVtxFitProbability);
      
        tmpTMVAReader_EE_alt.AddVariable("ele_ep",              &allMVAVars_alt.EoP);
        tmpTMVAReader_EE_alt.AddVariable("ele_eelepout",        &allMVAVars_alt.eleEoPout);
        tmpTMVAReader_EE_alt.AddVariable("ele_IoEmIop",         &allMVAVars_alt.IoEmIoP);
        
        // Geometrical matchings
        tmpTMVAReader_EE_alt.AddVariable("ele_deltaetain",      &allMVAVars_alt.deta);
        tmpTMVAReader_EE_alt.AddVariable("ele_deltaphiin",      &allMVAVars_alt.dphi);
        tmpTMVAReader_EE_alt.AddVariable("ele_deltaetaseed",    &allMVAVars_alt.detacalo);

        // Spectator variables  
        tmpTMVAReader_EE_alt.AddVariable("ele_pt",              &allMVAVars_alt.pt);
        tmpTMVAReader_EE_alt.AddVariable("scl_eta",             &allMVAVars_alt.SCeta);
      
        // Endcap only variables
        tmpTMVAReader_EE_alt.AddVariable("ele_psEoverEraw",    &allMVAVars_alt.PreShowerOverRaw);

        tmpTMVAReader_EB_alt.BookMVA( "Spring16_V1_EB1",  mvsPar["ElMVAweightFiles_alt"].at(0) );
        tmpTMVAReader_EB_alt.BookMVA( "Spring16_V1_EB2",  mvsPar["ElMVAweightFiles_alt"].at(1) );
        tmpTMVAReader_EE_alt.BookMVA(  "Spring16_V1_EE",   mvsPar["ElMVAweightFiles_alt"].at(2) );
    }
 
}

double BaseEventSelector::GetPerp(TVector3 & v1, TVector3 & v2)
{
    double perp;
    double _mag = v1.Cross(v2.Unit()).Mag();
    double _phi1 = v1.Phi();
    double _phi2 = v2.Phi();
    double _dphi = _phi1 - _phi2;
    if ( (_dphi > M_PI) || (_dphi > -M_PI && _dphi < 0.0) ) perp = _mag;
    else perp = -_mag;
    
    return perp;
}

void BaseEventSelector::Init( void )
{
    // init sanity check histograms
    mpEc->SetHistogram(mName, "jes_correction", 100, 0.8, 1.2);
    mpEc->SetHistogram(mName, "met_correction", 100, 0.0, 2.0);
    mpEc->SetHistogram(mName, "nBtagSfCorrections", 100, 0.0, 10.0);
}

TLorentzVector BaseEventSelector::correctJetForMet(const pat::Jet & jet, edm::EventBase const & event, unsigned int syst)
{

    TLorentzVector jetP4, offJetP4;
    jetP4.SetPtEtaPhiM(0.000001,1.,1.,0.000001);

    if ( jet.chargedEmEnergyFraction() + jet.neutralEmEnergyFraction() > 0.90 ) {
        return jetP4-jetP4;
    }

    pat::Jet correctedJet = jet.correctedJet(0);                 //copy original jet

    jetP4.SetPtEtaPhiM(correctedJet.pt(),correctedJet.eta(),correctedJet.phi(),correctedJet.mass());

    const std::vector<reco::CandidatePtr> & cands = jet.daughterPtrVector();
    for ( std::vector<reco::CandidatePtr>::const_iterator cand = cands.begin();
        cand != cands.end(); ++cand ) {
        const reco::PFCandidate *pfcand = dynamic_cast<const reco::PFCandidate *>(cand->get());
        const reco::Candidate *mu = (pfcand != 0 ? ( pfcand->muonRef().isNonnull() ? pfcand->muonRef().get() : 0) : cand->get());
        if ( mu != 0 && (mu->isGlobalMuon() || mu->isStandAloneMuon()) ) {
	    TLorentzVector muonP4;
            muonP4.SetPtEtaPhiM((*cand)->pt(),(*cand)->eta(),(*cand)->phi(),(*cand)->mass());
	    jetP4 -= muonP4;
        }
    }
    offJetP4 = jetP4;

    double ptscale = 1.0;
    double unc = 1.0;
    double pt = correctedJet.pt();
    std::vector<float> corrVec;

    edm::Handle<double> rhoHandle;
    edm::InputTag rhoSrc_("fixedGridRhoFastjetAll", "");
    event.getByLabel(rhoSrc_, rhoHandle);
    double rho = std::max(*(rhoHandle.product()), 0.0);

    if ( mbPar["isMc"] ){ 

        JetCorrector->setJetEta(correctedJet.eta());
  	JetCorrector->setJetPt(pt);
        JetCorrector->setJetA(jet.jetArea());
  	JetCorrector->setRho(rho); 
    
        try{
    	    corrVec = JetCorrector->getSubCorrections();
        }
  	catch(...){
    	    std::cout << mLegend << "WARNING! Exception thrown by JetCorrectionUncertainty!" << std::endl;
    	    std::cout << mLegend << "WARNING! Possibly, trying to correct a jet/MET outside correction range." << std::endl;
    	    std::cout << mLegend << "WARNING! Jet/MET will remain uncorrected." << std::endl;
        }
      
        jetP4 *= corrVec[corrVec.size()-1];
        offJetP4 *= corrVec[0];
        pt = jetP4.Pt();

        Variation JERsystematic = Variation::NOMINAL;
        if(mbPar["JERup"] || syst==3) JERsystematic = Variation::UP;
        if(mbPar["JERdown"] || syst==4) JERsystematic = Variation::DOWN;

	JME::JetParameters parameters;
	parameters.setJetPt(pt);
	parameters.setJetEta(jetP4.Eta());
	parameters.setRho(rho);
	double res = 0.0;
	res = resolution.getResolution(parameters);
	double factor = resolution_SF.getScaleFactor(parameters,JERsystematic) - 1;

        const reco::GenJet * genJet = jet.genJet();
        bool smeared = false;
	if(genJet){
	  TLorentzVector genP4;
	  genP4.SetPtEtaPhiE(genJet->pt(),genJet->eta(),genJet->phi(),genJet->energy());
	  double deltaPt = fabs(genJet->pt() - pt);
	  double deltaR = jetP4.DeltaR(genP4);	
	  if (deltaR < 0.2 && deltaPt <= 3*pt*res){
      	    double gen_pt = genJet->pt();
      	    double reco_pt = pt;
            double deltapt = (reco_pt - gen_pt) * factor;
            ptscale = max(0.0, (reco_pt + deltapt) / reco_pt);
            smeared = true;
	  }
	}
        if (!smeared && factor>0) {
          JERrand.SetSeed(abs(static_cast<int>(jet.phi()*1e4)));
          ptscale = max(0.0, JERrand.Gaus(pt,sqrt(factor*(factor+2))*res*pt)/pt);
        }

        if ( mbPar["JECup"] || mbPar["JECdown"] || syst==1 || syst==2) {
            jecUnc->setJetEta(jetP4.Eta());
            jecUnc->setJetPt(jetP4.Pt()*ptscale);

            if (mbPar["JECup"] || syst==1) { 
	        try{
                    unc = jecUnc->getUncertainty(true);
	        }
	        catch(...){ // catch all exceptions. Jet Uncertainty tool throws when binning out of range
	            std::cout << mLegend << "WARNING! Exception thrown by JetCorrectionUncertainty!" << std::endl;
                    std::cout << mLegend << "WARNING! Possibly, trying to correct a jet/MET outside correction range." << std::endl;
	            std::cout << mLegend << "WARNING! Jet/MET will remain uncorrected." << std::endl;
	            unc = 0.0;
	        }
                unc = 1 + unc; 
            }
            else { 
	        try{
                    unc = jecUnc->getUncertainty(false);
	        }
	        catch(...){
	            std::cout << mLegend << "WARNING! Exception thrown by JetCorrectionUncertainty!" << std::endl;
	            std::cout << mLegend << "WARNING! Possibly, trying to correct a jet/MET outside correction range." << std::endl;
	            std::cout << mLegend << "WARNING! Jet/MET will remain uncorrected." << std::endl;
	            unc = 0.0;
	        }
                unc = 1 - unc; 
            }

            if (jetP4.Pt()*ptscale < 10.0 && (mbPar["JECup"] || syst==1)) unc = 2.0;
            if (jetP4.Pt()*ptscale < 10.0 && (mbPar["JECdown"] || syst==2)) unc = 0.01;

        }

    }
    else if (!mbPar["isMc"]) {
      
        JetCorrector->setJetEta(correctedJet.eta());
  	JetCorrector->setJetPt(pt);
        JetCorrector->setJetA(jet.jetArea());
  	JetCorrector->setRho(rho); 
    
        try{
    	    corrVec = JetCorrector->getSubCorrections();
        }
  	catch(...){
    	    std::cout << mLegend << "WARNING! Exception thrown by JetCorrectionUncertainty!" << std::endl;
    	    std::cout << mLegend << "WARNING! Possibly, trying to correct a jet/MET outside correction range." << std::endl;
    	    std::cout << mLegend << "WARNING! Jet/MET will remain uncorrected." << std::endl;
        }
      
      
        jetP4 *= corrVec[corrVec.size()-1];
        offJetP4 *= corrVec[0];
        pt = jetP4.Pt();
	
    }

    jetP4 *= unc*ptscale;
    offJetP4 *= unc*ptscale;
    if (jetP4.Pt()<=15.) {
        offJetP4 = jetP4;
    }

    return offJetP4-jetP4;
}

TLorentzVector BaseEventSelector::correctJet(const pat::Jet & jet, edm::EventBase const & event, bool doAK8Corr, bool forceCorr, unsigned int syst)
{

  // JES and JES systematics
    pat::Jet correctedJet;
    if (mbPar["doNewJEC"] || forceCorr)
        correctedJet = jet.correctedJet(0);                 //copy original jet
    else
        correctedJet = jet;                                 //copy default corrected jet

    double ptscale = 1.0;
    double unc = 1.0;
    double pt = correctedJet.pt();
    double correction = 1.0;

    edm::Handle<double> rhoHandle;
    edm::InputTag rhoSrc_("fixedGridRhoFastjetAll", "");
    event.getByLabel(rhoSrc_, rhoHandle);
    double rho = std::max(*(rhoHandle.product()), 0.0);

    if ( mbPar["isMc"] ){ 

    	if (mbPar["doNewJEC"] || forceCorr) {
      	    // We need to undo the default corrections and then apply the new ones

      	    double pt_raw = jet.correctedJet(0).pt();
	    if (doAK8Corr){
                JetCorrectorAK8->setJetEta(jet.eta());
          	JetCorrectorAK8->setJetPt(pt_raw);
                JetCorrectorAK8->setJetA(jet.jetArea());
          	JetCorrectorAK8->setRho(rho); 
    
                try{
    		    correction = JetCorrectorAK8->getCorrection();
                }
          	catch(...){
    		    std::cout << mLegend << "WARNING! Exception thrown by JetCorrectionUncertainty!" << std::endl;
    		    std::cout << mLegend << "WARNING! Possibly, trying to correct a jet/MET outside correction range." << std::endl;
    		    std::cout << mLegend << "WARNING! Jet/MET will remain uncorrected." << std::endl;
                }
	    }

	    else{
                JetCorrector->setJetEta(jet.eta());
          	JetCorrector->setJetPt(pt_raw);
                JetCorrector->setJetA(jet.jetArea());
          	JetCorrector->setRho(rho); 
    
                try{
    		    correction = JetCorrector->getCorrection();
                }
          	catch(...){
    		    std::cout << mLegend << "WARNING! Exception thrown by JetCorrectionUncertainty!" << std::endl;
    		    std::cout << mLegend << "WARNING! Possibly, trying to correct a jet/MET outside correction range." << std::endl;
    		    std::cout << mLegend << "WARNING! Jet/MET will remain uncorrected." << std::endl;
                }
	    }
      
            correctedJet.scaleEnergy(correction);
            pt = correctedJet.pt();

        }

        Variation JERsystematic = Variation::NOMINAL;
        if(mbPar["JERup"] || syst==3) JERsystematic = Variation::UP;
        if(mbPar["JERdown"] || syst==4) JERsystematic = Variation::DOWN;

	JME::JetParameters parameters;
	parameters.setJetPt(pt);
	parameters.setJetEta(correctedJet.eta());
	parameters.setRho(rho);
	double res = 0.0;
	if(doAK8Corr) res = resolutionAK8.getResolution(parameters);
	else res = resolution.getResolution(parameters);
	double factor = resolution_SF.getScaleFactor(parameters,JERsystematic) - 1;

        const reco::GenJet * genJet = jet.genJet();
        bool smeared = false;
	if(genJet){
	  double deltaPt = fabs(genJet->pt() - pt);
	  double deltaR = reco::deltaR(genJet->p4(),correctedJet.p4());	
	  if (deltaR < ((doAK8Corr) ? 0.4 : 0.2) && deltaPt <= 3*pt*res){
      	    double gen_pt = genJet->pt();
      	    double reco_pt = pt;
            double deltapt = (reco_pt - gen_pt) * factor;
            ptscale = max(0.0, (reco_pt + deltapt) / reco_pt);
            smeared = true;
	  }
	}
        if (!smeared && factor>0) {
          JERrand.SetSeed(abs(static_cast<int>(jet.phi()*1e4)));
          ptscale = max(0.0, JERrand.Gaus(pt,sqrt(factor*(factor+2))*res*pt)/pt);
        }

        if ( mbPar["JECup"] || mbPar["JECdown"] || syst==1 || syst==2) {
            jecUnc->setJetEta(jet.eta());
            jecUnc->setJetPt(pt*ptscale);

            if (mbPar["JECup"] || syst==1) { 
	        try{
                    unc = jecUnc->getUncertainty(true);
	        }
	        catch(...){ // catch all exceptions. Jet Uncertainty tool throws when binning out of range
	            std::cout << mLegend << "WARNING! Exception thrown by JetCorrectionUncertainty!" << std::endl;
                    std::cout << mLegend << "WARNING! Possibly, trying to correct a jet/MET outside correction range." << std::endl;
	            std::cout << mLegend << "WARNING! Jet/MET will remain uncorrected." << std::endl;
	            unc = 0.0;
	        }
                unc = 1 + unc; 
            }
            else { 
	        try{
                    unc = jecUnc->getUncertainty(false);
	        }
	        catch(...){
	            std::cout << mLegend << "WARNING! Exception thrown by JetCorrectionUncertainty!" << std::endl;
	            std::cout << mLegend << "WARNING! Possibly, trying to correct a jet/MET outside correction range." << std::endl;
	            std::cout << mLegend << "WARNING! Jet/MET will remain uncorrected." << std::endl;
	            unc = 0.0;
	        }
                unc = 1 - unc; 
            }

            if (pt*ptscale < 10.0 && (mbPar["JECup"] || syst==1)) unc = 2.0;
            if (pt*ptscale < 10.0 && (mbPar["JECdown"] || syst==2)) unc = 0.01;

        }

	correctedJet.scaleEnergy(unc*ptscale);

    }
    else if (!mbPar["isMc"]) {
      
      if (mbPar["doNewJEC"] || forceCorr) {

	double pt_raw = jet.correctedJet(0).pt();	
	// We need to undo the default corrections and then apply the new ones
	
	if (doAK8Corr){
	  JetCorrectorAK8->setJetEta(jet.eta());
	  JetCorrectorAK8->setJetPt(pt_raw);
	  JetCorrectorAK8->setJetA(jet.jetArea());
	  JetCorrectorAK8->setRho(rho); 
	  
	  try{
	    correction = JetCorrectorAK8->getCorrection();
	  }
	  catch(...){
	    std::cout << mLegend << "WARNING! Exception thrown by JetCorrectionUncertainty!" << std::endl;
	    std::cout << mLegend << "WARNING! Possibly, trying to correct a jet/MET outside correction range." << std::endl;
	    std::cout << mLegend << "WARNING! Jet/MET will remain uncorrected." << std::endl;
	  }
	}
	
	else{
	  JetCorrector->setJetEta(jet.eta());
	  JetCorrector->setJetPt(pt_raw);
	  JetCorrector->setJetA(jet.jetArea());
	  JetCorrector->setRho(rho); 
	  
	  try{
	    correction = JetCorrector->getCorrection();
	  }
	  catch(...){
	    std::cout << mLegend << "WARNING! Exception thrown by JetCorrectionUncertainty!" << std::endl;
	    std::cout << mLegend << "WARNING! Possibly, trying to correct a jet/MET outside correction range." << std::endl;
	    std::cout << mLegend << "WARNING! Jet/MET will remain uncorrected." << std::endl;
	  }
	  
	}
	correctedJet.scaleEnergy(correction);
	pt = correctedJet.pt();

	
      }
    }

    TLorentzVector jetP4;
    jetP4.SetPtEtaPhiM(correctedJet.pt(), correctedJet.eta(),correctedJet.phi(), correctedJet.mass() );
    //jetP4.SetPtEtaPhiM(correctedJet.pt(), correctedJet.eta(),correctedJet.phi(), correctedJet.mass() );
    //std::cout<<"jet pt: "<<jetP4.Pt()<<" eta: "<<jetP4.Eta()<<" phi: "<<jetP4.Phi()<<" energy: "<<jetP4.E()<<std::endl;


    // sanity check - save correction of the first jet
    if (mNCorrJets==0){
        double _orig_pt = jet.pt();
        if (fabs(_orig_pt)<0.000000001){
            _orig_pt = 0.000000001;
        }
        SetHistValue("jes_correction", jetP4.Pt()/_orig_pt);
        ++mNCorrJets;
    }
    //if (jetP4.Pt()>30.) std::cout<<"JEC Ratio (new/old) = "<<jetP4.Pt()/jet.pt()<<"     -->    corrected pT / eta = "<<jetP4.Pt()<<" / "<<jetP4.Eta()<<std::endl;

    return jetP4;
}

pat::Jet BaseEventSelector::correctJetReturnPatJet(const pat::Jet & jet, edm::EventBase const & event, bool doAK8Corr, bool forceCorr, unsigned int syst)
{

  // JES and JES systematics
    pat::Jet correctedJet;
    if (mbPar["doNewJEC"] || forceCorr)
        correctedJet = jet.correctedJet(0);                 //copy original jet
    else
        correctedJet = jet;                                 //copy default corrected jet

    double ptscale = 1.0;
    double unc = 1.0;
    double pt = correctedJet.pt();
    double correction = 1.0;

    edm::Handle<double> rhoHandle;
    edm::InputTag rhoSrc_("fixedGridRhoFastjetAll", "");
    event.getByLabel(rhoSrc_, rhoHandle);
    double rho = std::max(*(rhoHandle.product()), 0.0);

    if ( mbPar["isMc"] ){ 

    	if (mbPar["doNewJEC"] || forceCorr) {
      	    // We need to undo the default corrections and then apply the new ones
 
      	    double pt_raw = jet.correctedJet(0).pt();

	    if (doAK8Corr){
                JetCorrectorAK8->setJetEta(jet.eta());
          	JetCorrectorAK8->setJetPt(pt_raw);
                JetCorrectorAK8->setJetA(jet.jetArea());
          	JetCorrectorAK8->setRho(rho); 
    
                try{
    		    correction = JetCorrectorAK8->getCorrection();
                }
          	catch(...){
    		    std::cout << mLegend << "WARNING! Exception thrown by JetCorrectionUncertainty!" << std::endl;
    		    std::cout << mLegend << "WARNING! Possibly, trying to correct a jet/MET outside correction range." << std::endl;
    		    std::cout << mLegend << "WARNING! Jet/MET will remain uncorrected." << std::endl;
                }
	    }

	    else{
                JetCorrector->setJetEta(jet.eta());
          	JetCorrector->setJetPt(pt_raw);
                JetCorrector->setJetA(jet.jetArea());
          	JetCorrector->setRho(rho); 
    
                try{
    		    correction = JetCorrector->getCorrection();
                }
          	catch(...){
    		    std::cout << mLegend << "WARNING! Exception thrown by JetCorrectionUncertainty!" << std::endl;
    		    std::cout << mLegend << "WARNING! Possibly, trying to correct a jet/MET outside correction range." << std::endl;
    		    std::cout << mLegend << "WARNING! Jet/MET will remain uncorrected." << std::endl;
                }
	    }
      
            correctedJet.scaleEnergy(correction);
            pt = correctedJet.pt();

        }

        Variation JERsystematic = Variation::NOMINAL;
        if(mbPar["JERup"] || syst==3) JERsystematic = Variation::UP;
        if(mbPar["JERdown"] || syst==4) JERsystematic = Variation::DOWN;

	JME::JetParameters parameters;
	parameters.setJetPt(pt);
	parameters.setJetEta(correctedJet.eta());
	parameters.setRho(rho);
	double res = 0.0;
	if(doAK8Corr) res = resolutionAK8.getResolution(parameters);
	else res = resolution.getResolution(parameters);
	double factor = resolution_SF.getScaleFactor(parameters,JERsystematic) - 1;

        const reco::GenJet * genJet = jet.genJet();
        bool smeared = false;
	if(genJet){
	  double deltaPt = fabs(genJet->pt() - pt);
	  double deltaR = reco::deltaR(genJet->p4(),correctedJet.p4());	
	  if (deltaR < ((doAK8Corr) ? 0.4 : 0.2) && deltaPt <= 3*pt*res){
      	    double gen_pt = genJet->pt();
      	    double reco_pt = pt;
            double deltapt = (reco_pt - gen_pt) * factor;
            ptscale = max(0.0, (reco_pt + deltapt) / reco_pt);
            smeared = true;
	  }
	}
        if (!smeared && factor>0) {
          JERrand.SetSeed(abs(static_cast<int>(jet.phi()*1e4)));
          ptscale = max(0.0, JERrand.Gaus(pt,sqrt(factor*(factor+2))*res*pt)/pt);
        }

        if ( mbPar["JECup"] || mbPar["JECdown"] || syst==1 || syst==2) {
            jecUnc->setJetEta(jet.eta());
            jecUnc->setJetPt(pt*ptscale);

            if (mbPar["JECup"] || syst==1) { 
    	        try{
                    unc = jecUnc->getUncertainty(true);
                }
    	        catch(...){ // catch all exceptions. Jet Uncertainty tool throws when binning out of range
    	            std::cout << mLegend << "WARNING! Exception thrown by JetCorrectionUncertainty!" << std::endl;
                    std::cout << mLegend << "WARNING! Possibly, trying to correct a jet/MET outside correction range." << std::endl;
                    std::cout << mLegend << "WARNING! Jet/MET will remain uncorrected." << std::endl;
    	            unc = 0.0;
    	        }
                unc = 1 + unc; 
            }
            else { 
    	        try{
                    unc = jecUnc->getUncertainty(false);
    	        }
    	        catch(...){
    	            std::cout << mLegend << "WARNING! Exception thrown by JetCorrectionUncertainty!" << std::endl;
    	            std::cout << mLegend << "WARNING! Possibly, trying to correct a jet/MET outside correction range." << std::endl;
    	            std::cout << mLegend << "WARNING! Jet/MET will remain uncorrected." << std::endl;
    	            unc = 0.0;
    	        }
                unc = 1 - unc; 
            }
    
            if (pt*ptscale < 10.0 && (mbPar["JECup"] || syst==1)) unc = 2.0;
            if (pt*ptscale < 10.0 && (mbPar["JECdown"] || syst==2)) unc = 0.01;

        }

	correctedJet.scaleEnergy(unc*ptscale);

    }
    else if (!mbPar["isMc"]) {
      
        if (mbPar["doNewJEC"] || forceCorr) {

	  double pt_raw = jet.correctedJet(0).pt();	
            // We need to undo the default corrections and then apply the new ones

	  if (doAK8Corr){
	    JetCorrectorAK8->setJetEta(jet.eta());
	    JetCorrectorAK8->setJetPt(pt_raw);
	    JetCorrectorAK8->setJetA(jet.jetArea());
	    JetCorrectorAK8->setRho(rho); 
	    
	    try{
	      correction = JetCorrectorAK8->getCorrection();
	    }
	    catch(...){
	      std::cout << mLegend << "WARNING! Exception thrown by JetCorrectionUncertainty!" << std::endl;
	      std::cout << mLegend << "WARNING! Possibly, trying to correct a jet/MET outside correction range." << std::endl;
	      std::cout << mLegend << "WARNING! Jet/MET will remain uncorrected." << std::endl;
	    }
	  }

	  else{
            JetCorrector->setJetEta(jet.eta());
            JetCorrector->setJetPt(pt_raw);
            JetCorrector->setJetA(jet.jetArea());
            JetCorrector->setRho(rho); 
	    
	    try{
	      correction = JetCorrector->getCorrection();
	    }
	    catch(...){
	      std::cout << mLegend << "WARNING! Exception thrown by JetCorrectionUncertainty!" << std::endl;
	      std::cout << mLegend << "WARNING! Possibly, trying to correct a jet/MET outside correction range." << std::endl;
	      std::cout << mLegend << "WARNING! Jet/MET will remain uncorrected." << std::endl;
	    }
	  }
	  correctedJet.scaleEnergy(correction);
	  pt = correctedJet.pt();

        }
    }

    return correctedJet;
}

bool BaseEventSelector::isJetTagged(const pat::Jet & jet, edm::EventBase const & event, bool applySF, int shiftflag, bool subjetflag)
{
    bool _isTagged = false;
    
    if ( jet.bDiscriminator( msPar["btagger"] ) > bTagCut ) _isTagged = true;
    
    string tagger = msPar["btagOP"];
    if (subjetflag) tagger += "subjet";

    if (mbPar["isMc"] && applySF) {
        TLorentzVector lvjet = correctJet(jet, event);
        
        double _lightSf = mBtagCond.GetMistagScaleFactor(lvjet.Et(), lvjet.Eta(), tagger);
        if (shiftflag == 3 || mbPar["MistagUncertUp"] ) _lightSf += mBtagCond.GetMistagSFUncertUp(lvjet.Et(), lvjet.Eta(), tagger);
        else if (shiftflag == 4 ||  mbPar["MistagUncertDown"] ) _lightSf -= mBtagCond.GetMistagSFUncertDown(lvjet.Et(), lvjet.Eta(), tagger);
        double _lightEff = mBtagCond.GetMistagRate(lvjet.Et(), lvjet.Eta(), tagger);
        
        int _jetFlavor = abs(jet.hadronFlavour());
        double _btagSf = mBtagCond.GetBtagScaleFactor(lvjet.Et(), lvjet.Eta(), tagger);
        if (shiftflag == 1 ||  mbPar["BTagUncertUp"] ) _btagSf += (mBtagCond.GetBtagSFUncertUp(lvjet.Et(), lvjet.Eta(), tagger)*(_jetFlavor==4?2:1));
        else if (shiftflag == 2 ||  mbPar["BTagUncertDown"] ) _btagSf -= (mBtagCond.GetBtagSFUncertDown(lvjet.Et(), lvjet.Eta(), tagger)*(_jetFlavor==4?2:1));
        double _btagEff = mBtagCond.GetBtagEfficiency(lvjet.Et(), lvjet.Eta(), tagger);
        
        mBtagSfUtil.SetSeed(abs(static_cast<int>(sin(jet.phi())*1e5)));
        
        // sanity check
        bool _orig_tag = _isTagged;
        
        mBtagSfUtil.modifyBTagsWithSF(_isTagged, _jetFlavor, _btagSf, _btagEff, _lightSf, _lightEff);
        
        // sanity check
        if (_isTagged != _orig_tag) ++mNBtagSfCorrJets;
        
    } // end of btag scale factor corrections
    return _isTagged;
}

TLorentzVector BaseEventSelector::correctMet(const pat::MET & met, edm::EventBase const & event, unsigned int syst, bool useHF)
{
    double correctedMET_px = met.uncorPx();
    double correctedMET_py = met.uncorPy();
    if ( mbPar["doNewJEC"] ) {
        for (std::vector<edm::Ptr<pat::Jet> >::const_iterator ijet = mvAllJets.begin();
             ijet != mvAllJets.end(); ++ijet) {
            if (!useHF && fabs((**ijet).eta())>2.6) continue;
            TLorentzVector lv = correctJetForMet(**ijet, event, syst);
            correctedMET_px += lv.Px();
            correctedMET_py += lv.Py();
        }
    }
    else {
        correctedMET_px = met.px();
        correctedMET_py = met.py();
    }
    
    correctedMET_p4.SetPxPyPzE(correctedMET_px, correctedMET_py, 0, sqrt(correctedMET_px*correctedMET_px+correctedMET_py*correctedMET_py));
    
    // sanity check histogram
    double _orig_met = met.pt();
    if (fabs(_orig_met) < 1.e-9) {
        _orig_met = 1.e-9;
    }
    SetHistValue("met_correction", correctedMET_p4.Pt()/_orig_met);
    
    return correctedMET_p4;
}

TLorentzVector BaseEventSelector::correctMet(const pat::MET & met, edm::EventBase const & event, std::vector<pat::Jet> jets, unsigned int syst, bool useHF)
{
    
    double correctedMET_px = met.uncorPx();
    double correctedMET_py = met.uncorPy();
    if ( mbPar["doNewJEC"] ) {
        for (std::vector<pat::Jet>::const_iterator ijet = jets.begin();
             ijet != jets.end(); ++ijet) {
            if (!useHF && fabs((*ijet).eta())>2.6) continue;
            TLorentzVector lv = correctJetForMet(*ijet, event, syst);
            correctedMET_px += lv.Px();
            correctedMET_py += lv.Py();
        }
    }
    else {
        correctedMET_px = met.px();
        correctedMET_py = met.py();
    }
    
    correctedMET_p4.SetPxPyPzE(correctedMET_px, correctedMET_py, 0, sqrt(correctedMET_px*correctedMET_px+correctedMET_py*correctedMET_py));
    
    // sanity check histogram
    double _orig_met = met.pt();
    if (fabs(_orig_met) < 1.e-9) {
        _orig_met = 1.e-9;
    }
    SetHistValue("met_correction", correctedMET_p4.Pt()/_orig_met);
    return correctedMET_p4;
}
TLorentzVector BaseEventSelector::correctMet(const pat::MET& met, edm::EventBase const & event, std::vector<edm::Ptr<pat::Jet> > jets, unsigned int syst, bool useHF){

  std::vector<pat::Jet> patJets;
  for(std::vector<edm::Ptr<pat::Jet> >::const_iterator ijet = jets.begin(); ijet!= jets.end(); ++ijet){
    patJets.push_back(**ijet);
  }

  TLorentzVector correctedMET = BaseEventSelector::correctMet(met, event, patJets, syst, useHF); //note that doing this also forces correctedMET_p4 member to be correctly set so it preserves the BaseEventSelector::GetCorrectedMET function, though as usual that function has to be called in order the corrected met to be produced
  return correctedMET;

}
double BaseEventSelector::mvaValue(const pat::Electron & electron, edm::EventBase const & event)
{
       
    edm::Handle<std::vector<reco::Vertex> > pvtxHandle;
    event.getByLabel( mtPar["pv_collection"], pvtxHandle);
    Int_t PVsize = pvtxHandle->size();
    if ( PVsize > 0 ) {
    } else {
        throw cms::Exception("InvalidInput") << " There needs to be at least one primary vertex in the event." << std::endl;
    }
    
    edm::Handle<reco::ConversionCollection> conversions;
    edm::InputTag convLabel_ ("reducedEgamma:reducedConversions");
    event.getByLabel(convLabel_, conversions);
    edm::Handle<reco::BeamSpot> bsHandle;
    edm::InputTag bsLabel_ ("offlineBeamSpot");
    event.getByLabel(bsLabel_, bsHandle);
    const reco::BeamSpot &beamspot = *bsHandle.product();


    // Pure ECAL -> shower shapes
    allMVAVars.see            = electron.full5x5_sigmaIetaIeta();
    allMVAVars.spp            = electron.full5x5_sigmaIphiIphi();
    allMVAVars.OneMinusE1x5E5x5 = 1. - electron.full5x5_e1x5() / electron.full5x5_e5x5();
    allMVAVars.R9             = electron.full5x5_r9();
    allMVAVars.etawidth       = electron.superCluster()->etaWidth();
    allMVAVars.phiwidth       = electron.superCluster()->phiWidth();
    allMVAVars.HoE            = electron.full5x5_hcalOverEcal();
    // Endcap only variables
    allMVAVars.PreShowerOverRaw  = electron.superCluster()->preshowerEnergy() / electron.superCluster()->rawEnergy();
  
    // To get to CTF track information in pat::Electron, we have to have the pointer
    // to pat::Electron, it is not accessible from the pointer to reco::GsfElectron.
    // This behavior is reported and is expected to change in the future (post-7.4.5 some time).
    reco::TrackRef myTrackRef = electron.closestCtfTrackRef();
    const pat::Electron * elePatPtr = dynamic_cast<const pat::Electron *>(&electron);
    // Check if this is really a pat::Electron, and if yes, get the track ref from this new
    // pointer instead
    if( elePatPtr != NULL ) myTrackRef = elePatPtr->closestCtfTrackRef();
    bool validKF = (myTrackRef.isAvailable() && (myTrackRef.isNonnull()) );  
  
    //Pure tracking variables
    allMVAVars.kfhits         = (validKF) ? myTrackRef->hitPattern().trackerLayersWithMeasurement() : -1. ;
    allMVAVars.kfchi2          = (validKF) ? myTrackRef->normalizedChi2() : 0;
    allMVAVars.gsfchi2         = electron.gsfTrack()->normalizedChi2();
  
    // Energy matching
    allMVAVars.fbrem           = electron.fbrem();
  
    allMVAVars.gsfhits         = electron.gsfTrack()->hitPattern().trackerLayersWithMeasurement();
    allMVAVars.expectedMissingInnerHits = electron.gsfTrack()->hitPattern().numberOfHits(reco::HitPattern::MISSING_INNER_HITS);
  
    reco::ConversionRef conv_ref = ConversionTools::matchedConversion(electron, conversions, beamspot.position());
    double vertexFitProbability = -1.; 
    if(!conv_ref.isNull()) {
      const reco::Vertex &vtx = conv_ref.get()->conversionVertex(); if (vtx.isValid()) {
        vertexFitProbability = TMath::Prob( vtx.chi2(), vtx.ndof());
      } 
    }
    allMVAVars.convVtxFitProbability    = vertexFitProbability;
  
    allMVAVars.EoP             = electron.eSuperClusterOverP();
    allMVAVars.eleEoPout       = electron.eEleClusterOverPout();
    allMVAVars.IoEmIoP         = (1.0/electron.ecalEnergy()) - (1.0 / electron.trackMomentumAtVtx().R() );
  
    // Geometrical matchings
    allMVAVars.deta            = electron.deltaEtaSuperClusterTrackAtVtx();
    allMVAVars.dphi            = electron.deltaPhiSuperClusterTrackAtVtx();
    allMVAVars.detacalo        = electron.deltaEtaSeedClusterTrackAtCalo();

    // Spectator variables  
    allMVAVars.pt              = electron.pt();
    allMVAVars.SCeta           = electron.superCluster()->eta();
    constexpr float ebeeSplit = 1.479;
    allMVAVars.isBarrel        = ( fabs(allMVAVars.SCeta) < ebeeSplit );
    allMVAVars.isEndcap        = ( fabs(allMVAVars.SCeta) >= ebeeSplit );
    // The spectator variables below were examined for training, but
    // are not necessary for evaluating the discriminator, so they are
    // given dummy values (the specator variables above are also unimportant).
    // They are introduced only to match the definition of the discriminator 
    // in the weights file.
    constexpr unsigned nines = 999;
    allMVAVars.eClass               = nines;
    allMVAVars.pfRelIso             = nines;
    allMVAVars.expectedInnerHits    = nines;
    allMVAVars.vtxconv              = nines;
    allMVAVars.mcEventWeight        = nines;
    allMVAVars.mcCBmatchingCategory = nines;

    // Constrain values

    if(allMVAVars.fbrem < -1.) allMVAVars.fbrem = -1.;
    allMVAVars.deta = fabs(allMVAVars.deta);
    if(allMVAVars.deta > 0.06) allMVAVars.deta = 0.06;
    allMVAVars.dphi = fabs(allMVAVars.dphi);
    if(allMVAVars.dphi > 0.6) allMVAVars.dphi = 0.6;
    if(allMVAVars.EoP > 20.) allMVAVars.EoP = 20.;
    if(allMVAVars.eleEoPout > 20.) allMVAVars.eleEoPout = 20.;
    allMVAVars.detacalo = fabs(allMVAVars.detacalo);
    if(allMVAVars.detacalo > 0.2) allMVAVars.detacalo = 0.2;
    if(allMVAVars.OneMinusE1x5E5x5 < -1.) allMVAVars.OneMinusE1x5E5x5 = -1;
    if(allMVAVars.OneMinusE1x5E5x5 > 2.) allMVAVars.OneMinusE1x5E5x5 = 2.; 
    if(allMVAVars.R9 > 5) allMVAVars.R9 = 5;
    if(allMVAVars.gsfchi2 > 200.) allMVAVars.gsfchi2 = 200;
    if(allMVAVars.kfchi2 > 10.) allMVAVars.kfchi2 = 10.;

    double cutValue;
    if (fabs(allMVAVars.SCeta)<=0.8) cutValue = tmpTMVAReader_EB.EvaluateMVA( "Spring15_V1_EB1" );
    else if (fabs(allMVAVars.SCeta)<=1.479) cutValue = tmpTMVAReader_EB.EvaluateMVA( "Spring15_V1_EB2" );
    else cutValue = tmpTMVAReader_EE.EvaluateMVA( "Spring15_V1_EE" );
    //std::cout<<"cutValue = "<<cutValue<<std::endl;

    return cutValue;	
}
double BaseEventSelector::mvaValue_alt(const pat::Electron & electron, edm::EventBase const & event)
{
       
    edm::Handle<std::vector<reco::Vertex> > pvtxHandle;
    event.getByLabel( mtPar["pv_collection"], pvtxHandle);
    Int_t PVsize = pvtxHandle->size();
    if ( PVsize > 0 ) {
    } else {
        throw cms::Exception("InvalidInput") << " There needs to be at least one primary vertex in the event." << std::endl;
    }
    
    edm::Handle<reco::ConversionCollection> conversions;
    edm::InputTag convLabel_ ("reducedEgamma:reducedConversions");
    event.getByLabel(convLabel_, conversions);
    edm::Handle<reco::BeamSpot> bsHandle;
    edm::InputTag bsLabel_ ("offlineBeamSpot");
    event.getByLabel(bsLabel_, bsHandle);
    const reco::BeamSpot &beamspot = *bsHandle.product();


    // Pure ECAL -> shower shapes
    allMVAVars_alt.see            = electron.full5x5_sigmaIetaIeta();
    allMVAVars_alt.spp            = electron.full5x5_sigmaIphiIphi();
    allMVAVars_alt.OneMinusE1x5E5x5 = 1. - electron.full5x5_e1x5() / electron.full5x5_e5x5();
    allMVAVars_alt.R9             = electron.full5x5_r9();
    allMVAVars_alt.etawidth       = electron.superCluster()->etaWidth();
    allMVAVars_alt.phiwidth       = electron.superCluster()->phiWidth();
    allMVAVars_alt.HoE            = electron.full5x5_hcalOverEcal();
    // Endcap only variables
    allMVAVars_alt.PreShowerOverRaw  = electron.superCluster()->preshowerEnergy() / electron.superCluster()->rawEnergy();
  
    // To get to CTF track information in pat::Electron, we have to have the pointer
    // to pat::Electron, it is not accessible from the pointer to reco::GsfElectron.
    // This behavior is reported and is expected to change in the future (post-7.4.5 some time).
    reco::TrackRef myTrackRef = electron.closestCtfTrackRef();
    const pat::Electron * elePatPtr = dynamic_cast<const pat::Electron *>(&electron);
    // Check if this is really a pat::Electron, and if yes, get the track ref from this new
    // pointer instead
    if( elePatPtr != NULL ) myTrackRef = elePatPtr->closestCtfTrackRef();
    bool validKF = (myTrackRef.isAvailable() && (myTrackRef.isNonnull()) );  
  
    //Pure tracking variables
    allMVAVars_alt.kfhits         = (validKF) ? myTrackRef->hitPattern().trackerLayersWithMeasurement() : -1. ;
    allMVAVars_alt.kfchi2          = (validKF) ? myTrackRef->normalizedChi2() : 0;
    allMVAVars_alt.gsfchi2         = electron.gsfTrack()->normalizedChi2();
  
    // Energy matching
    allMVAVars_alt.fbrem           = electron.fbrem();
  
    allMVAVars_alt.gsfhits         = electron.gsfTrack()->hitPattern().trackerLayersWithMeasurement();
    allMVAVars_alt.expectedMissingInnerHits = electron.gsfTrack()->hitPattern().numberOfHits(reco::HitPattern::MISSING_INNER_HITS);
  
    reco::ConversionRef conv_ref = ConversionTools::matchedConversion(electron, conversions, beamspot.position());
    double vertexFitProbability = -1.; 
    if(!conv_ref.isNull()) {
      const reco::Vertex &vtx = conv_ref.get()->conversionVertex(); if (vtx.isValid()) {
        vertexFitProbability = TMath::Prob( vtx.chi2(), vtx.ndof());
      } 
    }
    allMVAVars_alt.convVtxFitProbability    = vertexFitProbability;
  
    allMVAVars_alt.EoP             = electron.eSuperClusterOverP();
    allMVAVars_alt.eleEoPout       = electron.eEleClusterOverPout();
    allMVAVars_alt.IoEmIoP         = (1.0/electron.ecalEnergy()) - (1.0 / electron.trackMomentumAtVtx().R() );
  
    // Geometrical matchings
    allMVAVars_alt.deta            = electron.deltaEtaSuperClusterTrackAtVtx();
    allMVAVars_alt.dphi            = electron.deltaPhiSuperClusterTrackAtVtx();
    allMVAVars_alt.detacalo        = electron.deltaEtaSeedClusterTrackAtCalo();

    // Spectator variables  
    allMVAVars_alt.pt              = electron.pt();
    allMVAVars_alt.SCeta           = electron.superCluster()->eta();
    constexpr float ebeeSplit = 1.479;
    allMVAVars_alt.isBarrel        = ( fabs(allMVAVars_alt.SCeta) < ebeeSplit );
    allMVAVars_alt.isEndcap        = ( fabs(allMVAVars_alt.SCeta) >= ebeeSplit );


    // Constrain values

    if(allMVAVars_alt.fbrem < -1.) allMVAVars_alt.fbrem = -1.;
    allMVAVars_alt.deta = fabs(allMVAVars_alt.deta);
    if(allMVAVars_alt.deta > 0.06) allMVAVars_alt.deta = 0.06;
    allMVAVars_alt.dphi = fabs(allMVAVars_alt.dphi);
    if(allMVAVars_alt.dphi > 0.6) allMVAVars_alt.dphi = 0.6;
    if(allMVAVars_alt.EoP > 20.) allMVAVars_alt.EoP = 20.;
    if(allMVAVars_alt.eleEoPout > 20.) allMVAVars_alt.eleEoPout = 20.;
    allMVAVars_alt.detacalo = fabs(allMVAVars_alt.detacalo);
    if(allMVAVars_alt.detacalo > 0.2) allMVAVars_alt.detacalo = 0.2;
    if(allMVAVars_alt.OneMinusE1x5E5x5 < -1.) allMVAVars_alt.OneMinusE1x5E5x5 = -1;
    if(allMVAVars_alt.OneMinusE1x5E5x5 > 2.) allMVAVars_alt.OneMinusE1x5E5x5 = 2.; 
    if(allMVAVars_alt.R9 > 5) allMVAVars_alt.R9 = 5;
    if(allMVAVars_alt.gsfchi2 > 200.) allMVAVars_alt.gsfchi2 = 200;
    if(allMVAVars_alt.kfchi2 > 10.) allMVAVars_alt.kfchi2 = 10.;

    double cutValue;
    if (fabs(allMVAVars_alt.SCeta)<=0.8) cutValue = tmpTMVAReader_EB_alt.EvaluateMVA( "Spring16_V1_EB1" );
    else if (fabs(allMVAVars_alt.SCeta)<=1.479) cutValue = tmpTMVAReader_EB_alt.EvaluateMVA( "Spring16_V1_EB2" );
    else cutValue = tmpTMVAReader_EE_alt.EvaluateMVA( "Spring16_V1_EE" );
    //std::cout<<"cutValue = "<<cutValue<<std::endl;

    return cutValue;	
}
