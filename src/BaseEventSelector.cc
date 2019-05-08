#include <math.h>
#include <boost/algorithm/string.hpp>
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

void BaseEventSelector::JECbyIOV(edm::EventBase const & event) {
/*
 *This function takes an event, looks up the correct JEC file, and produces the correct JetCorrector for JEC corrections. 
 *JEC is run number dependent. 
 *This first gets the run number for the event 
 *It then pulls in the corersponding spring16_V10* file
 *Then uses that. 
 *
 * This is called in singleLepEventSelector
 * */

  int iRun   = event.id().run();

  if(iRun <= 299330){ JetCorrector = JetCorrector_B; JetCorrectorAK8 = JetCorrectorAK8_B;}
  else if(iRun <= 302029){ JetCorrector = JetCorrector_C; JetCorrectorAK8 = JetCorrectorAK8_C;}
  else if(iRun <= 304827){ JetCorrector = JetCorrector_DE; JetCorrectorAK8 = JetCorrectorAK8_DE;}
  else{ JetCorrector = JetCorrector_F; JetCorrectorAK8 = JetCorrectorAK8_F;}
  
}


void BaseEventSelector::BeginJob(std::map<std::string, edm::ParameterSet const > par)
{
    std::string _key = "event_selector";
    bool _missing_config = false;
    if ( par.find(_key)!=par.end() ){
        if (par[_key].exists("isMc")) mbPar["isMc"] = par[_key].getParameter<bool> ("isMc");
        else mbPar["isMc"] = false;
        
        if (par[_key].exists("btagOP")) msPar["btagOP"] = par[_key].getParameter<std::string> ("btagOP");
        else msPar["btagOP"] = "MEDIUM";
        if (par[_key].exists("bdisc_min")) mdPar["bdisc_min"] = par[_key].getParameter<double> ("bdisc_min");
        else mdPar["bdisc_min"] = 0.4941;
        
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
            msPar["MCL1JetPar"] = "";
            _missing_config = true;
        }
        if (par[_key].exists("MCL2JetPar")) msPar["MCL2JetPar"] = par[_key].getParameter<std::string> ("MCL2JetPar");
        else{
            msPar["MCL2JetPar"] = "";
            _missing_config = true;
        }
        if (par[_key].exists("MCL3JetPar")) msPar["MCL3JetPar"] = par[_key].getParameter<std::string> ("MCL3JetPar");
        else{
            msPar["MCL3JetPar"] = "";
            _missing_config = true;
        }
        if (par[_key].exists("MCL1JetParAK8")) msPar["MCL1JetParAK8"] = par[_key].getParameter<std::string> ("MCL1JetParAK8");
        else{
            msPar["MCL1JetParAK8"] = "";
            _missing_config = true;
        }
        if (par[_key].exists("MCL2JetParAK8")) msPar["MCL2JetParAK8"] = par[_key].getParameter<std::string> ("MCL2JetParAK8");
        else{
            msPar["MCL2JetParAK8"] = "";
            _missing_config = true;
        }
        if (par[_key].exists("MCL3JetParAK8")) msPar["MCL3JetParAK8"] = par[_key].getParameter<std::string> ("MCL3JetParAK8");
        else{
            msPar["MCL3JetParAK8"] = "";
            _missing_config = true;
        }
        
        if (par[_key].exists("DataL1JetPar")) msPar["DataL1JetPar"] = par[_key].getParameter<std::string> ("DataL1JetPar");
        else{
            msPar["DataL1JetPar"] = "";
            _missing_config = true;
        }
        if (par[_key].exists("DataL2JetPar")) msPar["DataL2JetPar"] = par[_key].getParameter<std::string> ("DataL2JetPar");
        else{
            msPar["DataL2JetPar"] = "";
            _missing_config = true;
        }
        if (par[_key].exists("DataL3JetPar")) msPar["DataL3JetPar"] = par[_key].getParameter<std::string> ("DataL3JetPar");
        else{
            msPar["DataL3JetPar"] = "";
            _missing_config = true;
        }
        if (par[_key].exists("DataResJetPar")) msPar["DataResJetPar"] = par[_key].getParameter<std::string> ("DataResJetPar");
        else{
            msPar["DataResJetPar"] = "";
            _missing_config = true;
        }
        if (par[_key].exists("DataL1JetParAK8")) msPar["DataL1JetParAK8"] = par[_key].getParameter<std::string> ("DataL1JetParAK8");
        else{
            msPar["DataL1JetParAK8"] = "";
            _missing_config = true;
        }
        if (par[_key].exists("DataL2JetParAK8")) msPar["DataL2JetParAK8"] = par[_key].getParameter<std::string> ("DataL2JetParAK8");
        else{
            msPar["DataL2JetParAK8"] = "";
            _missing_config = true;
        }
        if (par[_key].exists("DataL3JetParAK8")) msPar["DataL3JetParAK8"] = par[_key].getParameter<std::string> ("DataL3JetParAK8");
        else{
            msPar["DataL3JetParAK8"] = "";
            _missing_config = true;
        }
        if (par[_key].exists("DataResJetParAK8")) msPar["DataResJetParAK8"] = par[_key].getParameter<std::string> ("DataResJetParAK8");
        else{
            msPar["DataResJetParAK8"] = "";
            _missing_config = true;
        }
	msPar["DataL1JetParByIOV"] = msPar["DataL1JetPar"];
	msPar["DataL2JetParByIOV"] = msPar["DataL2JetPar"];
	msPar["DataL3JetParByIOV"] = msPar["DataL3JetPar"];
	msPar["DataResJetParByIOV"] = msPar["DataResJetPar"];
	msPar["DataL1JetParByIOVAK8"] = msPar["DataL1JetParAK8"];
	msPar["DataL2JetParByIOVAK8"] = msPar["DataL2JetParAK8"];
	msPar["DataL3JetParByIOVAK8"] = msPar["DataL3JetParAK8"];
	msPar["DataResJetParByIOVAK8"] = msPar["DataResJetParAK8"];

	if (par[_key].exists("DeepCSVfile")) msPar["DeepCSVfile"] = par[_key].getParameter<std::string> ("DeepCSVfile");
	else msPar["DeepCSVfile"] = "data/DeepCSV_102XSF_V1.csv";
	if (par[_key].exists("DeepCSVSubjetfile")) msPar["DeepCSVSubjetfile"] = par[_key].getParameter<std::string> ("DeepCSVSubjetfile");
	else msPar["DeepCSVSubjetfile"] = "data/subjet_DeepCSV_94XSF_v3_B_F.csv";

        if (par[_key].exists("doNewJEC")) mbPar["doNewJEC"] = par[_key].getParameter<bool> ("doNewJEC");
        else mbPar["doNewJEC"] = false;
        
        if (_missing_config) {
            std::cout << mLegend
            << "ONE OF THE FOLLOWING CONFIG OPTIONS MISSING!\n"
            << "MCL1JetPar, MCL2JetPar, MCL3JetPar, DataL1JetPar, DataL2JetPar,\n"
            << "DataL3JetPar, DataResJetPar" <<std::endl;
            std::cout << mLegend
            << "LJMET WILL CRASH" << std::endl;
        }
        if (par[_key].exists("UseElMVA")) mbPar["UseElMVA"] = par[_key].getParameter<bool> ("UseElMVA");
        else mbPar["UseElMVA"] = false;

	mtPar["pv_collection"] = par[_key].getParameter<edm::InputTag>("pv_collection");
    }
        
    bTagCut = mdPar["bdisc_min"];
    std::cout << "b-tag check: DeepCSV "<<msPar["btagOP"]<<" > "<<mdPar["bdisc_min"]<<std::endl;
    std::cout << "b-tag files: " << msPar["DeepCSVfile"] << ", " << msPar["DeepCSVSubjetfile"] << std::endl;
    calib = BTagCalibration("deepcsv",msPar["DeepCSVfile"]);
    calibsj = BTagCalibration("deepcsvsj",msPar["DeepCSVSubjetfile"]);
    if(msPar["btagOP"] == "LOOSE"){
      reader = BTagCalibrationReader(BTagEntry::OP_LOOSE, "central", {"up","down"});
      readerSJ = BTagCalibrationReader(BTagEntry::OP_LOOSE, "central", {"up","down"});
    }else if(msPar["btagOP"] == "TIGHT"){
      reader = BTagCalibrationReader(BTagEntry::OP_TIGHT, "central", {"up","down"});
    }else{
      reader = BTagCalibrationReader(BTagEntry::OP_MEDIUM, "central", {"up","down"});
      readerSJ = BTagCalibrationReader(BTagEntry::OP_MEDIUM, "central", {"up","down"});
    }
    reader.load(calib, BTagEntry::FLAV_B, "comb");
    reader.load(calib, BTagEntry::FLAV_C, "comb");
    reader.load(calib, BTagEntry::FLAV_UDSG, "incl");
    readerSJ.load(calibsj, BTagEntry::FLAV_B, "lt");
    readerSJ.load(calibsj, BTagEntry::FLAV_C, "lt");
    readerSJ.load(calibsj, BTagEntry::FLAV_UDSG, "incl");

    if ( mbPar["isMc"] )
      jecUnc = new JetCorrectionUncertainty(msPar["JEC_txtfile"]);

    resolution = JME::JetResolution(msPar["JER_txtfile"]);
    resolutionAK8 = JME::JetResolution(msPar["JERAK8_txtfile"]);
    resolution_SF = JME::JetResolutionScaleFactor(msPar["JERSF_txtfile"]);    

    std::vector<JetCorrectorParameters> vPar;
    std::vector<JetCorrectorParameters> vParAK8;
    std::vector<JetCorrectorParameters> vPar_B;
    std::vector<JetCorrectorParameters> vParAK8_B;
    std::vector<JetCorrectorParameters> vPar_C;
    std::vector<JetCorrectorParameters> vParAK8_C;
    std::vector<JetCorrectorParameters> vPar_DE;
    std::vector<JetCorrectorParameters> vParAK8_DE;
    std::vector<JetCorrectorParameters> vPar_F;
    std::vector<JetCorrectorParameters> vParAK8_F;

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
      
      if (mbPar["doNewJEC"]) std::cout << mLegend << "Applying new jet energy corrections" << std::endl;
      else std::cout << mLegend << "NOT applying new jet energy corrections - ARE YOU SURE?" << std::endl;     
      
      delete JetCorrector;
      delete JetCorrectorAK8;
      
      JetCorrector = new FactorizedJetCorrector(vPar);
      JetCorrectorAK8 = new FactorizedJetCorrector(vParAK8);
      
    }
    else if ( !mbPar["isMc"] ) {
      // Create the JetCorrectorParameter objects, the order does not matter.
      
      std::string strB = msPar["DataL1JetPar"];
      std::string strC = strB; boost::replace_first(strC,"B","C");
      std::string strDE = strB; boost::replace_first(strDE,"B","DE");
      std::string strF = strB; boost::replace_first(strF,"B","F");  
      msPar["DataL1JetParByIOV_B"] = strB;
      msPar["DataL1JetParByIOV_C"] = strC;
      msPar["DataL1JetParByIOV_DE"] = strDE;
      msPar["DataL1JetParByIOV_F"] = strF;

      strB = msPar["DataL2JetPar"];
      strC = strB; boost::replace_first(strC,"B","C");
      strDE = strB; boost::replace_first(strDE,"B","DE");
      strF = strB; boost::replace_first(strF,"B","F");  
      msPar["DataL2JetParByIOV_B"] = strB;
      msPar["DataL2JetParByIOV_C"] = strC;
      msPar["DataL2JetParByIOV_DE"] = strDE;
      msPar["DataL2JetParByIOV_F"] = strF;

      strB = msPar["DataL3JetPar"];
      strC = strB; boost::replace_first(strC,"B","C");
      strDE = strB; boost::replace_first(strDE,"B","DE");
      strF = strB; boost::replace_first(strF,"B","F");  
      msPar["DataL3JetParByIOV_B"] = strB;
      msPar["DataL3JetParByIOV_C"] = strC;
      msPar["DataL3JetParByIOV_DE"] = strDE;
      msPar["DataL3JetParByIOV_F"] = strF;

      strB = msPar["DataResJetPar"];
      strC = strB; boost::replace_first(strC,"B","C");
      strDE = strB; boost::replace_first(strDE,"B","DE");
      strF = strB; boost::replace_first(strF,"B","F");  
      msPar["DataResJetParByIOV_B"] = strB;
      msPar["DataResJetParByIOV_C"] = strC;
      msPar["DataResJetParByIOV_DE"] = strDE;
      msPar["DataResJetParByIOV_F"] = strF;

      strB = msPar["DataL1JetParAK8"];
      strC = strB; boost::replace_first(strC,"B","C");
      strDE = strB; boost::replace_first(strDE,"B","DE");
      strF = strB; boost::replace_first(strF,"B","F");  
      msPar["DataL1JetParAK8ByIOV_B"] = strB;
      msPar["DataL1JetParAK8ByIOV_C"] = strC;
      msPar["DataL1JetParAK8ByIOV_DE"] = strDE;
      msPar["DataL1JetParAK8ByIOV_F"] = strF;

      strB = msPar["DataL2JetParAK8"];
      strC = strB; boost::replace_first(strC,"B","C");
      strDE = strB; boost::replace_first(strDE,"B","DE");
      strF = strB; boost::replace_first(strF,"B","F");  
      msPar["DataL2JetParAK8ByIOV_B"] = strB;
      msPar["DataL2JetParAK8ByIOV_C"] = strC;
      msPar["DataL2JetParAK8ByIOV_DE"] = strDE;
      msPar["DataL2JetParAK8ByIOV_F"] = strF;

      strB = msPar["DataL3JetParAK8"];
      strC = strB; boost::replace_first(strC,"B","C");
      strDE = strB; boost::replace_first(strDE,"B","DE");
      strF = strB; boost::replace_first(strF,"B","F");  
      msPar["DataL3JetParAK8ByIOV_B"] = strB;
      msPar["DataL3JetParAK8ByIOV_C"] = strC;
      msPar["DataL3JetParAK8ByIOV_DE"] = strDE;
      msPar["DataL3JetParAK8ByIOV_F"] = strF;

      strB = msPar["DataResJetParAK8"];
      strC = strB; boost::replace_first(strC,"B","C");
      strDE = strB; boost::replace_first(strDE,"B","DE");
      strF = strB; boost::replace_first(strF,"B","F");  
      msPar["DataResJetParAK8ByIOV_B"] = strB;
      msPar["DataResJetParAK8ByIOV_C"] = strC;
      msPar["DataResJetParAK8ByIOV_DE"] = strDE;
      msPar["DataResJetParAK8ByIOV_F"] = strF;

      ResJetPar_B = new JetCorrectorParameters(msPar["DataResJetParByIOV_B"]); 
      L3JetPar_B  = new JetCorrectorParameters(msPar["DataL3JetParByIOV_B"]);
      L2JetPar_B  = new JetCorrectorParameters(msPar["DataL2JetParByIOV_B"]);
      L1JetPar_B  = new JetCorrectorParameters(msPar["DataL1JetParByIOV_B"]);
      ResJetParAK8_B = new JetCorrectorParameters(msPar["DataResJetParAK8ByIOV_B"]); 
      L3JetParAK8_B  = new JetCorrectorParameters(msPar["DataL3JetParAK8ByIOV_B"]);
      L2JetParAK8_B  = new JetCorrectorParameters(msPar["DataL2JetParAK8ByIOV_B"]);
      L1JetParAK8_B  = new JetCorrectorParameters(msPar["DataL1JetParAK8ByIOV_B"]);

      ResJetPar_C = new JetCorrectorParameters(msPar["DataResJetParByIOV_C"]); 
      L3JetPar_C  = new JetCorrectorParameters(msPar["DataL3JetParByIOV_C"]);
      L2JetPar_C  = new JetCorrectorParameters(msPar["DataL2JetParByIOV_C"]);
      L1JetPar_C  = new JetCorrectorParameters(msPar["DataL1JetParByIOV_C"]);
      ResJetParAK8_C = new JetCorrectorParameters(msPar["DataResJetParAK8ByIOV_C"]); 
      L3JetParAK8_C  = new JetCorrectorParameters(msPar["DataL3JetParAK8ByIOV_C"]);
      L2JetParAK8_C  = new JetCorrectorParameters(msPar["DataL2JetParAK8ByIOV_C"]);
      L1JetParAK8_C  = new JetCorrectorParameters(msPar["DataL1JetParAK8ByIOV_C"]);

      ResJetPar_DE = new JetCorrectorParameters(msPar["DataResJetParByIOV_DE"]); 
      L3JetPar_DE  = new JetCorrectorParameters(msPar["DataL3JetParByIOV_DE"]);
      L2JetPar_DE  = new JetCorrectorParameters(msPar["DataL2JetParByIOV_DE"]);
      L1JetPar_DE  = new JetCorrectorParameters(msPar["DataL1JetParByIOV_DE"]);
      ResJetParAK8_DE = new JetCorrectorParameters(msPar["DataResJetParAK8ByIOV_DE"]); 
      L3JetParAK8_DE  = new JetCorrectorParameters(msPar["DataL3JetParAK8ByIOV_DE"]);
      L2JetParAK8_DE  = new JetCorrectorParameters(msPar["DataL2JetParAK8ByIOV_DE"]);
      L1JetParAK8_DE  = new JetCorrectorParameters(msPar["DataL1JetParAK8ByIOV_DE"]);

      ResJetPar_F = new JetCorrectorParameters(msPar["DataResJetParByIOV_F"]); 
      L3JetPar_F  = new JetCorrectorParameters(msPar["DataL3JetParByIOV_F"]);
      L2JetPar_F  = new JetCorrectorParameters(msPar["DataL2JetParByIOV_F"]);
      L1JetPar_F  = new JetCorrectorParameters(msPar["DataL1JetParByIOV_F"]);
      ResJetParAK8_F = new JetCorrectorParameters(msPar["DataResJetParAK8ByIOV_F"]); 
      L3JetParAK8_F  = new JetCorrectorParameters(msPar["DataL3JetParAK8ByIOV_F"]);
      L2JetParAK8_F  = new JetCorrectorParameters(msPar["DataL2JetParAK8ByIOV_F"]);
      L1JetParAK8_F  = new JetCorrectorParameters(msPar["DataL1JetParAK8ByIOV_F"]);

      // Load the JetCorrectorParameter objects into a std::vector,
      // IMPORTANT: THE ORDER MATTERS HERE !!!! 
      vPar_B.push_back(*L1JetPar_B);
      vPar_B.push_back(*L2JetPar_B);
      vPar_B.push_back(*L3JetPar_B);
      vPar_B.push_back(*ResJetPar_B);      
      vParAK8_B.push_back(*L1JetParAK8_B);
      vParAK8_B.push_back(*L2JetParAK8_B);
      vParAK8_B.push_back(*L3JetParAK8_B);
      vParAK8_B.push_back(*ResJetParAK8_B);

      vPar_C.push_back(*L1JetPar_C);
      vPar_C.push_back(*L2JetPar_C);
      vPar_C.push_back(*L3JetPar_C);
      vPar_C.push_back(*ResJetPar_C);      
      vParAK8_C.push_back(*L1JetParAK8_C);
      vParAK8_C.push_back(*L2JetParAK8_C);
      vParAK8_C.push_back(*L3JetParAK8_C);
      vParAK8_C.push_back(*ResJetParAK8_C);

      vPar_DE.push_back(*L1JetPar_DE);
      vPar_DE.push_back(*L2JetPar_DE);
      vPar_DE.push_back(*L3JetPar_DE);
      vPar_DE.push_back(*ResJetPar_DE);      
      vParAK8_DE.push_back(*L1JetParAK8_DE);
      vParAK8_DE.push_back(*L2JetParAK8_DE);
      vParAK8_DE.push_back(*L3JetParAK8_DE);
      vParAK8_DE.push_back(*ResJetParAK8_DE);

      vPar_F.push_back(*L1JetPar_F);
      vPar_F.push_back(*L2JetPar_F);
      vPar_F.push_back(*L3JetPar_F);
      vPar_F.push_back(*ResJetPar_F);      
      vParAK8_F.push_back(*L1JetParAK8_F);
      vParAK8_F.push_back(*L2JetParAK8_F);
      vParAK8_F.push_back(*L3JetParAK8_F);
      vParAK8_F.push_back(*ResJetParAK8_F);

      delete JetCorrector_B;
      delete JetCorrector_C;
      delete JetCorrector_DE;
      delete JetCorrector_F;
      delete JetCorrectorAK8_B;
      delete JetCorrectorAK8_C;
      delete JetCorrectorAK8_DE;
      delete JetCorrectorAK8_F;
      
      JetCorrector_B = new FactorizedJetCorrector(vPar_B);
      JetCorrectorAK8_B = new FactorizedJetCorrector(vParAK8_B);
      JetCorrector_C = new FactorizedJetCorrector(vPar_C);
      JetCorrectorAK8_C = new FactorizedJetCorrector(vParAK8_C);
      JetCorrector_DE = new FactorizedJetCorrector(vPar_DE);
      JetCorrectorAK8_DE = new FactorizedJetCorrector(vParAK8_DE);
      JetCorrector_F = new FactorizedJetCorrector(vPar_F);
      JetCorrectorAK8_F = new FactorizedJetCorrector(vParAK8_F);

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

    if (jet.bDiscriminator("pfDeepCSVJetTags:probb")+jet.bDiscriminator("pfDeepCSVJetTags:probbb") > bTagCut) _isTagged = true;

    if (mbPar["isMc"] && applySF){
      
      TLorentzVector lvjet = correctJet(jet, event);
        
      int _jetFlavor = abs(jet.hadronFlavour());
      double _heavySf = 1.0;
      double _heavyEff = 1.0;
      double _lightSf = 1.0;
      double _lightEff = 1.0;

      if(!subjetflag){

	_heavySf = reader.eval_auto_bounds("central",BTagEntry::FLAV_B,fabs(lvjet.Eta()),lvjet.Pt());
	if (shiftflag == 1 ||  mbPar["BTagUncertUp"] ) _heavySf = reader.eval_auto_bounds("up",BTagEntry::FLAV_B,fabs(lvjet.Eta()),lvjet.Pt());
	else if (shiftflag == 2 ||  mbPar["BTagUncertDown"] ) _heavySf = reader.eval_auto_bounds("down",BTagEntry::FLAV_B,fabs(lvjet.Eta()),lvjet.Pt());
	_heavyEff = mBtagCond.GetBtagEfficiency(lvjet.Et(), fabs(lvjet.Eta()), "DeepCSV"+msPar["btagOP"]);       
	
	if(_jetFlavor == 4){
	  _heavySf = reader.eval_auto_bounds("central",BTagEntry::FLAV_C,fabs(lvjet.Eta()),lvjet.Pt());
	  if (shiftflag == 1 ||  mbPar["BTagUncertUp"] ) _heavySf = reader.eval_auto_bounds("up",BTagEntry::FLAV_C,fabs(lvjet.Eta()),lvjet.Pt());
	  else if (shiftflag == 2 ||  mbPar["BTagUncertDown"] ) _heavySf = reader.eval_auto_bounds("down",BTagEntry::FLAV_C,fabs(lvjet.Eta()),lvjet.Pt());
	  _heavyEff = mBtagCond.GetBtagEfficiency(lvjet.Et(), fabs(lvjet.Eta()), "DeepCSV"+msPar["btagOP"]);       
	}
	
	_lightSf = reader.eval_auto_bounds("central",BTagEntry::FLAV_UDSG,fabs(lvjet.Eta()),lvjet.Pt());
	if (shiftflag == 3 || mbPar["MistagUncertUp"] ) _lightSf = reader.eval_auto_bounds("up",BTagEntry::FLAV_UDSG,fabs(lvjet.Eta()),lvjet.Pt());
	else if (shiftflag == 4 ||  mbPar["MistagUncertDown"] ) _lightSf = reader.eval_auto_bounds("down",BTagEntry::FLAV_UDSG,fabs(lvjet.Eta()),lvjet.Pt());
	_lightEff = mBtagCond.GetMistagRate(lvjet.Et(), fabs(lvjet.Eta()), "DeepCSV"+msPar["btagOP"]);
	
      }else{

      	_heavySf = readerSJ.eval_auto_bounds("central",BTagEntry::FLAV_B,fabs(lvjet.Eta()),lvjet.Pt());
      	if (shiftflag == 1 ||  mbPar["BTagUncertUp"] ) _heavySf = readerSJ.eval_auto_bounds("up",BTagEntry::FLAV_B,fabs(lvjet.Eta()),lvjet.Pt());
      	else if (shiftflag == 2 ||  mbPar["BTagUncertDown"] ) _heavySf = readerSJ.eval_auto_bounds("down",BTagEntry::FLAV_B,fabs(lvjet.Eta()),lvjet.Pt());
      	_heavyEff = mBtagCond.GetBtagEfficiency(lvjet.Et(), fabs(lvjet.Eta()), "SJDeepCSV"+msPar["btagOP"]);       
	
      	if(_jetFlavor == 4){
      	  _heavySf = readerSJ.eval_auto_bounds("central",BTagEntry::FLAV_C,fabs(lvjet.Eta()),lvjet.Pt());
      	  if (shiftflag == 1 ||  mbPar["BTagUncertUp"] ) _heavySf = readerSJ.eval_auto_bounds("up",BTagEntry::FLAV_C,fabs(lvjet.Eta()),lvjet.Pt());
      	  else if (shiftflag == 2 ||  mbPar["BTagUncertDown"] ) _heavySf = readerSJ.eval_auto_bounds("down",BTagEntry::FLAV_C,fabs(lvjet.Eta()),lvjet.Pt());
      	  _heavyEff = mBtagCond.GetBtagEfficiency(lvjet.Et(), fabs(lvjet.Eta()), "SJDeepCSV"+msPar["btagOP"]);       
      	}
	
      	_lightSf = readerSJ.eval_auto_bounds("central",BTagEntry::FLAV_UDSG,fabs(lvjet.Eta()),lvjet.Pt());
      	if (shiftflag == 3 || mbPar["MistagUncertUp"] ) _lightSf = readerSJ.eval_auto_bounds("up",BTagEntry::FLAV_UDSG,fabs(lvjet.Eta()),lvjet.Pt());
      	else if (shiftflag == 4 ||  mbPar["MistagUncertDown"] ) _lightSf = readerSJ.eval_auto_bounds("down",BTagEntry::FLAV_UDSG,fabs(lvjet.Eta()),lvjet.Pt());
      	_lightEff = mBtagCond.GetMistagRate(lvjet.Et(), fabs(lvjet.Eta()), "SJDeepCSV"+msPar["btagOP"]);
      }

      mBtagSfUtil.SetSeed(abs(static_cast<int>(sin(jet.phi())*1e5)));
      
      // sanity check
      bool _orig_tag = _isTagged;      
      mBtagSfUtil.modifyBTagsWithSF(_isTagged, _jetFlavor, _heavySf, _heavyEff, _lightSf, _lightEff);     
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
