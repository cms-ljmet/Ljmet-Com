// -*- C++ -*-
//
// FWLite PAT analyzer-selector for a generic single lepton analysis
//
// Adapted from WprimeEventSelector
// Joshua Swanson, April 2014
//
#ifndef LJMet_Com_interface_singleLepEventSelector_h
#define LJMet_Com_interface_singleLepEventSelector_h

#include <cmath>
#include <iostream>
#include <fstream>

#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TLegend.h"
#include "TLorentzVector.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TTree.h"
#include "TVector3.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/TriggerObject.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "LJMet/Com/interface/TopElectronSelector.h"
#include "PhysicsTools/SelectorUtils/interface/PFJetIDSelectionFunctor.h"
#include "LJMet/Com/interface/PFMuonSelector.h"

#include "PhysicsTools/SelectorUtils/interface/PVSelector.h"
#include "PhysicsTools/SelectorUtils/interface/PVObjectSelector.h"
#include "LJMet/Com/interface/BaseEventSelector.h"
#include "LJMet/Com/interface/LjmetFactory.h"

#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"
#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"

#include "PhysicsTools/CondLiteIO/interface/RecordWriter.h"
//#include "Cintex/Cintex.h"
#include "PhysicsTools/JetMCUtils/interface/combination.h"

#include "DataFormats/METReco/interface/HcalNoiseSummary.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/METReco/interface/BeamHaloSummary.h"

#include "LJMet/Com/interface/MiniIsolation.h"

using trigger::TriggerObject;
using namespace std;

class singleLepEventSelector : public BaseEventSelector {

public:

  
    singleLepEventSelector();  
    ~singleLepEventSelector();
  
  
    // executes before loop over events
    virtual void BeginJob(std::map<std::string, edm::ParameterSet const > par);

    // main method where the cuts are applied
    virtual bool operator()( edm::EventBase const & event, pat::strbitset & ret);

    virtual void AnalyzeEvent( edm::EventBase const & event, LjmetEventContent & ec );
    
    // executes after loop over events
    virtual void EndJob();

    boost::shared_ptr<PVSelector>              const & pvSel()         const { return pvSel_;}

protected:
    std::string legend;
    bool bFirstEntry;

    // containers for config parameter values
    std::map<std::string,bool>           mbPar;
    std::map<std::string,int>            miPar;
    std::map<std::string,double>         mdPar;
    std::map<std::string,std::string>    msPar;
    std::map<std::string, edm::InputTag> mtPar;
    std::map<std::string,std::vector<std::string> > mvsPar;

    boost::shared_ptr<PVSelector>              pvSel_;

    edm::Handle<edm::TriggerResults >           mhEdmTriggerResults;
    edm::Handle<std::vector<pat::Jet> >         mhJets;
    edm::Handle<std::vector<pat::Jet> >         mhJets_AK8;
    edm::Handle<std::vector<pat::Muon> >        mhMuons;
    edm::Handle<std::vector<pat::Electron> >    mhElectrons;
    edm::Handle<std::vector<pat::Tau> >			mhTaus;
    edm::Handle<std::vector<pat::MET> >         mhMet;
    edm::Handle<std::vector<reco::PFMET> >      mhType1CorrMet;
    edm::Handle<double>                         h_rho;
    edm::Handle<std::vector<reco::Vertex> >     h_primVtx;

    std::vector<edm::Ptr<reco::Vertex> >  good_pvs_;



private:
  
    void initialize(std::map<std::string, edm::ParameterSet const> par);

};



static int reg = LjmetFactory::GetInstance()->Register(new singleLepEventSelector(), "singleLepSelector");

singleLepEventSelector::singleLepEventSelector()
{
}

singleLepEventSelector::~singleLepEventSelector()
{
}

void singleLepEventSelector::BeginJob( std::map<std::string, edm::ParameterSet const> par)
{
    BaseEventSelector::BeginJob(par);

    std::string _key;

    _key = "pvSelector";
    if ( par.find(_key)!=par.end() ){
        pvSel_ = boost::shared_ptr<PVSelector>( new PVSelector(par[_key]) );
        std::cout << mLegend << "pv selector configured!"
                  << std::endl;
    }
    else {
        std::cout << mLegend << "PV selector not configured, exiting"
                  << std::endl;
        std::exit(-1);
    }

      _key = "event_selector";
    if ( par.find(_key)!=par.end() ){

        mbPar["debug"]                    = par[_key].getParameter<bool>         ("debug");
        mbPar["isMc"]                     = par[_key].getParameter<bool>         ("isMc");
        mbPar["keepFullMChistory"]        = par[_key].getParameter<bool>         ("keepFullMChistory");
        
        mbPar["trigger_cut"]              = par[_key].getParameter<bool>         ("trigger_cut");
        mbPar["dump_trigger"]             = par[_key].getParameter<bool>         ("dump_trigger");
        mvsPar["trigger_path_el"]         = par[_key].getParameter<std::vector<std::string>>  ("trigger_path_el");
        mvsPar["trigger_path_mu"]         = par[_key].getParameter<std::vector<std::string>>  ("trigger_path_mu");
        mvsPar["mctrigger_path_el"]       = par[_key].getParameter<std::vector<std::string>>  ("mctrigger_path_el");
        mvsPar["mctrigger_path_mu"]       = par[_key].getParameter<std::vector<std::string>>  ("mctrigger_path_mu");

        mtPar["flag_tag"]                 = par[_key].getParameter<edm::InputTag>("flag_tag");
        mbPar["pv_cut"]                   = par[_key].getParameter<bool>         ("pv_cut");
        mbPar["metfilters"]               = par[_key].getParameter<bool>         ("metfilters");

        mbPar["jet_cuts"]                 = par[_key].getParameter<bool>         ("jet_cuts");
        mdPar["jet_minpt"]                = par[_key].getParameter<double>       ("jet_minpt");
        mdPar["jet_maxeta"]               = par[_key].getParameter<double>       ("jet_maxeta");
        mdPar["jet_minpt_AK8"]            = par[_key].getParameter<double>       ("jet_minpt_AK8");
        mdPar["jet_maxeta_AK8"]           = par[_key].getParameter<double>       ("jet_maxeta_AK8");
        miPar["min_jet"]                  = par[_key].getParameter<int>          ("min_jet");
        miPar["max_jet"]                  = par[_key].getParameter<int>          ("max_jet");
        mdPar["leading_jet_pt"]           = par[_key].getParameter<double>       ("leading_jet_pt");

        mbPar["muon_cuts"]                 = par[_key].getParameter<bool>         ("muon_cuts");
        mbPar["muon_selector_medium"]      = par[_key].getParameter<bool>         ("muon_selector_medium");
        mdPar["muon_reliso"]               = par[_key].getParameter<double>       ("muon_reliso");
        mdPar["muon_dxy"]                  = par[_key].getParameter<double>       ("muon_dxy");
        mdPar["muon_dz"]                   = par[_key].getParameter<double>       ("muon_dz");
        mdPar["muon_minpt"]                = par[_key].getParameter<double>       ("muon_minpt");
        mdPar["muon_maxeta"]               = par[_key].getParameter<double>       ("muon_maxeta");
        mbPar["loose_muon_selector_tight"] = par[_key].getParameter<bool>         ("loose_muon_selector_tight");
        mdPar["loose_muon_reliso"]         = par[_key].getParameter<double>       ("loose_muon_reliso");
        mdPar["loose_muon_dxy"]            = par[_key].getParameter<double>       ("loose_muon_dxy");
        mdPar["loose_muon_dz"]             = par[_key].getParameter<double>       ("loose_muon_dz");
        mdPar["loose_muon_minpt"]          = par[_key].getParameter<double>       ("loose_muon_minpt");
        mdPar["loose_muon_maxeta"]         = par[_key].getParameter<double>       ("loose_muon_maxeta");
        miPar["min_muon"]                  = par[_key].getParameter<int>          ("min_muon");
        if (par[_key].exists("muon_useMiniIso")) {
	  mbPar["muon_useMiniIso"]         = par[_key].getParameter<bool>         ("muon_useMiniIso");
	  mdPar["muon_miniIso"]            = par[_key].getParameter<double>       ("muon_miniIso");
	  mdPar["loose_muon_miniIso"]      = par[_key].getParameter<double>       ("loose_muon_miniIso");
	}else{
	  mbPar["muon_useMiniIso"]         = false;
	  mdPar["muon_miniIso"]            = 999;
	  mdPar["loose_muon_miniIso"]      = 999;
	}

        mbPar["electron_cuts"]             = par[_key].getParameter<bool>         ("electron_cuts");
        mdPar["electron_minpt"]            = par[_key].getParameter<double>       ("electron_minpt");
        mdPar["electron_maxeta"]           = par[_key].getParameter<double>       ("electron_maxeta");
        mdPar["electron_miniIso"]          = par[_key].getParameter<double>       ("electron_miniIso");
        mdPar["loose_electron_minpt"]      = par[_key].getParameter<double>       ("loose_electron_minpt");
        mdPar["loose_electron_maxeta"]     = par[_key].getParameter<double>       ("loose_electron_maxeta");
        mdPar["loose_electron_miniIso"]    = par[_key].getParameter<double>       ("loose_electron_miniIso");
        miPar["min_electron"]              = par[_key].getParameter<int>          ("min_electron");
        if (par[_key].exists("electron_useMiniIso")) {
	  mbPar["electron_useMiniIso"]     = par[_key].getParameter<bool>         ("electron_useMiniIso");
	  mdPar["electron_miniIso"]        = par[_key].getParameter<double>       ("electron_miniIso");
	  mdPar["loose_electron_miniIso"]  = par[_key].getParameter<double>       ("loose_electron_miniIso");
	}else{
	  mbPar["electron_useMiniIso"]     = false;
	  mdPar["electron_miniIso"]        = -999;
	  mdPar["loose_electron_miniIso"]  = -999;
	}

        miPar["min_lepton"]               = par[_key].getParameter<int>          ("min_lepton");
        miPar["max_lepton"]               = par[_key].getParameter<int>          ("max_lepton");
        miPar["min_loose_lepton"]         = par[_key].getParameter<int>          ("min_loose_lepton");
        miPar["max_loose_lepton"]         = par[_key].getParameter<int>          ("max_loose_lepton");
        mbPar["second_lepton_veto"]       = par[_key].getParameter<bool>         ("second_lepton_veto");
        mbPar["tau_veto"]                 = par[_key].getParameter<bool>         ("tau_veto");

        mbPar["met_cuts"]                 = par[_key].getParameter<bool>         ("met_cuts");
        mdPar["min_met"]                  = par[_key].getParameter<double>       ("min_met");
	mdPar["max_met"]                  = par[_key].getParameter<double>       ("max_met");

        mbPar["btag_cuts"]                = par[_key].getParameter<bool>         ("btag_cuts");
        mbPar["btag_1"]                   = par[_key].getParameter<bool>         ("btag_1");
        mbPar["btag_2"]                   = par[_key].getParameter<bool>         ("btag_2");
        mbPar["btag_3"]                   = par[_key].getParameter<bool>         ("btag_3");

        mtPar["trigger_collection"]       = par[_key].getParameter<edm::InputTag>("trigger_collection");
        mtPar["pv_collection"]            = par[_key].getParameter<edm::InputTag>("pv_collection");
        mtPar["jet_collection"]           = par[_key].getParameter<edm::InputTag>("jet_collection");
        mtPar["slimmedJetsAK8"]           = par[_key].getParameter<edm::InputTag>("slimmedJetsAK8");	
        mtPar["muon_collection"]          = par[_key].getParameter<edm::InputTag>("muon_collection");
        mtPar["electron_collection"]      = par[_key].getParameter<edm::InputTag>("electron_collection");
        mtPar["tau_collection"]           = par[_key].getParameter<edm::InputTag>("tau_collection");
        mtPar["met_collection"]           = par[_key].getParameter<edm::InputTag>("met_collection");

        mbPar["doNewJEC"]                 = par[_key].getParameter<bool>         ("doNewJEC");
        if (par[_key].exists("doAllJetSyst")) mbPar["doAllJetSyst"] = par[_key].getParameter<bool> ("doAllJetSyst");
        else                                  mbPar["doAllJetSyst"] = false;
        mbPar["doLepJetCleaning"]         = par[_key].getParameter<bool>         ("doLepJetCleaning");
        if (par[_key].exists("CleanLooseLeptons")) mbPar["CleanLooseLeptons"] = par[_key].getParameter<bool> ("CleanLooseLeptons");
	else                                       mbPar["CleanLooseLeptons"] = false;
	if (par[_key].exists("LepJetDR")) mdPar["LepJetDR"] = par[_key].getParameter<double> ("LepJetDR");
	else                              mdPar["LepJetDR"] = 0.6;
	if (par[_key].exists("LepJetDRAK8")) mdPar["LepJetDRAK8"] = par[_key].getParameter<double> ("LepJetDRAK8");
	else                                 mdPar["LepJetDRAK8"] = 0.8;
        if (par[_key].exists("UseElMVA")) mbPar["UseElMVA"] = par[_key].getParameter<bool>         ("UseElMVA");
        else                              mbPar["UseElMVA"] = false;
	if (par[_key].exists("UseElIDV1")) mbPar["UseElIDV1"] = par[_key].getParameter<bool>("UseElIDV1");
	else                               mbPar["UseElIDV1"] = true;

        std::cout << mLegend << "config parameters loaded..."
                  << std::endl;
    }   
    else {
        std::cout << mLegend << "event selector not configured, exiting"
                  << std::endl;
        std::exit(-1);
    }
  
    std::cout << mLegend << "initializing singleLep selection" << std::endl;

    bFirstEntry = true;
  
    push_back("No selection");
    set("No selection");
  
    push_back("Trigger");
    push_back("Primary vertex");
    push_back("MET filters");
    push_back("One jet or more");
    push_back("Two jets or more");
    push_back("Three jets or more");
    push_back("Min jet multiplicity");
    push_back("Max jet multiplicity");
    push_back("Leading jet pt");
    push_back("Min MET");
    push_back("Max MET");
    push_back("Min muon");
    push_back("Min electron");
    push_back("Min lepton");
    push_back("Max lepton");
    push_back("Min loose lepton");
    push_back("Max loose lepton");
    push_back("Second lepton veto");
    push_back("Tau veto");
    push_back("1 btag or more");
    push_back("2 btag or more");
    push_back("3 btag or more");
    push_back("All cuts");          // sanity check

  
    // TOP PAG sync selection v3

    set("Trigger", mbPar["trigger_cut"]); 
    set("Primary vertex", mbPar["pv_cut"]);
    set("MET filters", mbPar["metfilters"]); 
 
    if (mbPar["jet_cuts"]){
        set("One jet or more", true);
        set("Two jets or more", false);
        set("Three jets or more", false);
        set("Min jet multiplicity", miPar["min_jet"]);
        set("Max jet multiplicity", miPar["max_jet"]);
        set("Leading jet pt", mdPar["leading_jet_pt"]);
    }
    else{
        set("One jet or more", false);
        set("Two jets or more", false);
        set("Three jets or more", false);
        set("Min jet multiplicity", false);
        set("Max jet multiplicity", false);
        set("Leading jet pt", false);
    }

    if (mbPar["met_cuts"]){
      set("Min MET", mdPar["min_met"]);
      set("Max MET", mdPar["max_met"]);    
    }

    set("Min muon", miPar["min_muon"]);  
    set("Min electron", miPar["min_electron"]);  
    set("Min lepton", miPar["min_lepton"]);  
    set("Max lepton", miPar["max_lepton"]);  
    set("Min loose lepton", miPar["min_loose_lepton"]);  
    set("Max loose lepton", miPar["max_loose_lepton"]);  
    set("Second lepton veto", mbPar["second_lepton_veto"]);
    set("Tau veto", mbPar["tau_veto"]);
     
    if (mbPar["btag_cuts"]){
        set("1 btag or more", mbPar["btag_1"]);
        set("2 btag or more", mbPar["btag_2"]);
        set("3 btag or more", mbPar["btag_3"]);
    }
    else{
        set("1 btag or more", false);
        set("2 btag or more", false);
        set("3 btag or more", false);
    }

    set("All cuts", true);
    
   
} // end of BeginJob() 

bool singleLepEventSelector::operator()( edm::EventBase const & event, pat::strbitset & ret)
{

  if(!mbPar["isMc"]) BaseEventSelector::JECbyIOV(event);

    while(1){ // standard infinite while loop trick to avoid nested ifs
    
        passCut(ret, "No selection");
    
        //
        //_____ Trigger cuts __________________________________
        //

        bool passTrigElMC = false;
        bool passTrigMuMC = false;
        bool passTrigElData = false;
        bool passTrigMuData = false;
	
        if ( considerCut("Trigger") ) {
	  
	  if (mbPar["debug"]) std::cout<<"trigger cuts..."<<std::endl;
	  
	  event.getByLabel( mtPar["trigger_collection"], mhEdmTriggerResults );
	  const edm::TriggerNames trigNames = event.triggerNames(*mhEdmTriggerResults);
	  
	  bool passTrig = false;
	  unsigned int _tSize = mhEdmTriggerResults->size();
	  
	  
	  // dump trigger names
	  if (bFirstEntry && mbPar["dump_trigger"]){
	    for (unsigned int i=0; i<_tSize; i++){
	      std::string trigName = trigNames.triggerName(i);
	      std::cout << i << "   " << trigName;
	      bool fired = mhEdmTriggerResults->accept(trigNames.triggerIndex(trigName));
	      std::cout <<", FIRED = "<<fired<<std::endl;
	    } 
	  }
	  
	  mvSelTriggersEl.clear();
	  mvSelMCTriggersEl.clear();
	  mvSelTriggersMu.clear();
	  mvSelMCTriggersMu.clear();
	  
	  int passTrigEl = 0;
	  if (mbPar["debug"]) std::cout<<"	In MC El trig list: "<<std::endl;
	  for (unsigned int ipath = 0; ipath < mvsPar["mctrigger_path_el"].size() && mvsPar["mctrigger_path_el"].at(0)!="" ; ipath++){
	    for(unsigned int i=0; i<_tSize; i++){
	      std::string trigName = trigNames.triggerName(i);
	      if (trigName.find(mvsPar["mctrigger_path_el"].at(ipath)) == std::string::npos) continue;
	      if (mhEdmTriggerResults->accept(trigNames.triggerIndex(trigName))) {
		passTrigEl = 1;
		mvSelMCTriggersEl[mvsPar["mctrigger_path_el"].at(ipath)] = 1;
		if (mbPar["debug"]) std::cout << "		" << trigNames.triggerName(i)  << std::endl;
	      }
	      else mvSelMCTriggersEl[mvsPar["mctrigger_path_el"].at(ipath)] = 0;
	    }
	  }
	  if (passTrigEl>0) passTrigElMC = true;
	  
	  int passTrigMu = 0;
	  if (mbPar["debug"]) std::cout<<"	In MC Mu trig list: "<<std::endl;
	  for (unsigned int ipath = 0; ipath < mvsPar["mctrigger_path_mu"].size() && mvsPar["mctrigger_path_mu"].at(0)!="" ; ipath++){
	    for(unsigned int i=0; i<_tSize; i++){
	      std::string trigName = trigNames.triggerName(i);
	      if ( trigName.find(mvsPar["mctrigger_path_mu"].at(ipath)) == std::string::npos) continue;
	      if (mhEdmTriggerResults->accept(trigNames.triggerIndex(trigName))){
		passTrigMu = 1;
		mvSelMCTriggersMu[mvsPar["mctrigger_path_mu"].at(ipath)] = 1;
		if (mbPar["debug"]) std::cout << "		" << trigNames.triggerName(i)  << std::endl;
	      }
	      else mvSelMCTriggersMu[mvsPar["mctrigger_path_mu"].at(ipath)] = 0;
	    }
	  }
	  if (passTrigMu>0) passTrigMuMC = true;
	  
	  //Loop over each data channel separately
	  passTrigEl = 0;
	  if (mbPar["debug"]) std::cout<<"	In Data El trig list: "<<std::endl;
	  for (unsigned int ipath = 0; ipath < mvsPar["trigger_path_el"].size() && mvsPar["trigger_path_el"].at(0)!="" ; ipath++){
	    for(unsigned int i=0; i<_tSize; i++){
	      std::string trigName = trigNames.triggerName(i);
	      if ( trigName.find(mvsPar["trigger_path_el"].at(ipath)) == std::string::npos) continue;
	      if (mhEdmTriggerResults->accept(trigNames.triggerIndex(trigName))){
		passTrigEl = 1;
		mvSelTriggersEl[mvsPar["trigger_path_el"].at(ipath)] = 1;
		if (mbPar["debug"]) std::cout << "		" << trigNames.triggerName(i)  << std::endl;
	      }
	      else mvSelTriggersEl[mvsPar["trigger_path_el"].at(ipath)] = 0;
	    }
	  }
	  if (passTrigEl>0) passTrigElData = true;
	  
	  passTrigMu = 0;
	  if (mbPar["debug"]) std::cout<<"	In Data Mu trig list: "<<std::endl;
	  for (unsigned int ipath = 0; ipath < mvsPar["trigger_path_mu"].size() && mvsPar["trigger_path_mu"].at(0)!="" ; ipath++){
	    for(unsigned int i=0; i<_tSize; i++){
	      std::string trigName = trigNames.triggerName(i);
	      if ( trigName.find(mvsPar["trigger_path_mu"].at(ipath)) == std::string::npos) continue;
	      if (mhEdmTriggerResults->accept(trigNames.triggerIndex(trigName))){
		passTrigMu = 1;
		mvSelTriggersMu[mvsPar["trigger_path_mu"].at(ipath)] = 1;
		if (mbPar["debug"]) std::cout << "		" << trigNames.triggerName(i)  << std::endl;
	      }
	      else mvSelTriggersMu[mvsPar["trigger_path_mu"].at(ipath)] = 0;
	    }
	  }
	  if (passTrigMu>0) passTrigMuData = true;
	  
	  if (mbPar["isMc"] && (passTrigMuMC||passTrigElMC) ) passTrig = true;
	  if (!mbPar["isMc"] && (passTrigMuData||passTrigElData) ) passTrig = true;  
	  
	  if ( ignoreCut("Trigger") || passTrig ) passCut(ret, "Trigger");
	  else break;
	  
        } // end of trigger cuts
    
    

        //
        //_____ Primary vertex cuts __________________________________
        //
        mvSelPVs.clear();
        if ( considerCut("Primary vertex") ) {
            if (mbPar["debug"]) std::cout<<"pv cuts..."<<std::endl;

	    //	    std::cout << "SLES L476: calling pvSel" << std::endl;
            if ( (*pvSel_)(event) ){
                passCut(ret, "Primary vertex"); // PV cuts total
            }
            else break;

	    //	    std::cout << "SLES L482: opening pv_collection" << std::endl;
            event.getByLabel( mtPar["pv_collection"], h_primVtx );
	    //	    std::cout << "SLES L484: got pv_collection" << std::endl;
            int _n_pvs = 0;
            for (std::vector<reco::Vertex>::const_iterator _ipv = h_primVtx->begin();
                 _ipv != h_primVtx->end(); ++_ipv){
	      mvSelPVs.push_back(edm::Ptr<reco::Vertex>(h_primVtx, _n_pvs));
	      ++_n_pvs;
            }
	    //	    std::cout << "SLES L492: got past pv loop" << std::endl;
        } // end of PV cuts


	//
	//______ MET Filters ___________________
	//
	if (considerCut("MET filters")) {

	  edm::Handle<edm::TriggerResults > PatTriggerResults;
	  event.getByLabel( mtPar["flag_tag"], PatTriggerResults );
	  const edm::TriggerNames patTrigNames = event.triggerNames(*PatTriggerResults);
	  
          bool goodvertpass = false;
	  bool globaltighthalopass = false;
	  bool hbhenoisepass = false;
	  bool hbhenoiseisopass = false;
	  bool ecaldeadcellpass = false;
	  bool badpfmuonpass = false;
	  bool badchargedcandpass = false;
	  bool eebadscpass = false;
	  bool eebadcalibpass = false;

	  
	  for (unsigned int i=0; i<PatTriggerResults->size(); i++){
	    if (patTrigNames.triggerName(i) == "Flag_goodVertices") goodvertpass = PatTriggerResults->accept(patTrigNames.triggerIndex(patTrigNames.triggerName(i)));
	    if (patTrigNames.triggerName(i) == "Flag_globalSuperTightHalo2016Filter") globaltighthalopass = PatTriggerResults->accept(patTrigNames.triggerIndex(patTrigNames.triggerName(i)));
	    if (patTrigNames.triggerName(i) == "Flag_HBHENoiseFilter") hbhenoisepass = PatTriggerResults->accept(patTrigNames.triggerIndex(patTrigNames.triggerName(i)));
	    if (patTrigNames.triggerName(i) == "Flag_HBHENoiseIsoFilter") hbhenoiseisopass = PatTriggerResults->accept(patTrigNames.triggerIndex(patTrigNames.triggerName(i)));
	    if (patTrigNames.triggerName(i) == "Flag_EcalDeadCellTriggerPrimitiveFilter") ecaldeadcellpass = PatTriggerResults->accept(patTrigNames.triggerIndex(patTrigNames.triggerName(i)));
	    if (patTrigNames.triggerName(i) == "Flag_BadPFMuonFilter") badpfmuonpass = PatTriggerResults->accept(patTrigNames.triggerIndex(patTrigNames.triggerName(i)));
	    if (patTrigNames.triggerName(i) == "Flag_BadChargedCandidateFilter") badchargedcandpass = PatTriggerResults->accept(patTrigNames.triggerIndex(patTrigNames.triggerName(i)));
	    if (patTrigNames.triggerName(i) == "Flag_eeBadScFilter") eebadscpass = PatTriggerResults->accept(patTrigNames.triggerIndex(patTrigNames.triggerName(i)));
	    if (patTrigNames.triggerName(i) == "Flag_ecalBadCalibFilter") eebadcalibpass = PatTriggerResults->accept(patTrigNames.triggerIndex(patTrigNames.triggerName(i)));
	  }

	  // Rerun ecalBadCalibReducedMINIAODFilter if possible
	  edm::Handle<bool> passecalBadCalibFilterUpdate;
	  if(event.getByLabel(edm::InputTag("ecalBadCalibReducedMINIAODFilter") , passecalBadCalibFilterUpdate)){
	    eebadcalibpass = *passecalBadCalibFilterUpdate;
	  }
	  
	  if(hbhenoisepass && hbhenoiseisopass && globaltighthalopass && ecaldeadcellpass && (mbPar["isMc"] || eebadscpass) && goodvertpass && badpfmuonpass && eebadcalibpass){
	    //badchargedcandpass && // removed based on new recommendation spring 2019
	    passCut(ret, "MET filters");
	  }
	  else break;
	}
	//	std::cout << "SLES L541: done with met filters" << std::endl;

        //======================================================
        //
        //_____ Muon cuts ________________________________
        //      
        // loop over muons

	//packed pf candidates and rho source needed miniIso
	edm::InputTag packedPFCandsLabel_ = edm::InputTag("packedPFCandidates");
	edm::Handle<std::vector<pat::PackedCandidate>> packedPFCands;	
	//	std::cout << "SLES L565: getting candidates " << std::endl;	      
	try{
	  event.getByLabel(packedPFCandsLabel_, packedPFCands);
	}catch(int except){
	  std::cout << "Caught exception error " << except << std::endl;
	  break;
	}

	//	std::cout << "SLES L565: got pf candidates" << std::endl;
	//rho isolation from susy recommendation
	edm::Handle<double> rhoJetsNC;
	event.getByLabel(edm::InputTag("fixedGridRhoFastjetCentralNeutral","") , rhoJetsNC);
	double myRhoJetsNC = *rhoJetsNC;
	//	std::cout << "SLES L573: got rho" << std::endl;

        int _n_muons  = 0;
        int nSelMuons = 0;
        int nLooseMuons = 0;
        if (mbPar["debug"]) std::cout<<"start muon cuts..."<<std::endl;

        if ( mbPar["muon_cuts"] ) {

            //get muons
            event.getByLabel( mtPar["muon_collection"], mhMuons );      
	    //	    std::cout << "SLES L563: got muons" << std::endl;

            mvSelMuons.clear();
	    mvLooseMuons.clear();		      

            for (std::vector<pat::Muon>::const_iterator _imu = mhMuons->begin(); _imu != mhMuons->end(); _imu++){
	      bool pass = false;
    
	      //muon cuts
	      while(1){

		if ( mbPar["muon_selector_medium"] && (*_imu).passed(reco::Muon::CutBasedIdMedium) ){ }
		else if ( !mbPar["muon_selector_medium"] && (*_imu).passed(reco::Muon::CutBasedIdTight) ){ }
		else break; // fail
		  
		//pat::Muon* muptr = new pat::Muon(*_imu);
		//float miniIso = getPFMiniIsolation_EffectiveArea(packedPFCands, dynamic_cast<const reco::Candidate* > (muptr), 0.05, 0.2, 10., false, false,myRhoJetsNC);

		double chIso = (*_imu).pfIsolationR04().sumChargedHadronPt;
		double nhIso = (*_imu).pfIsolationR04().sumNeutralHadronEt;
		double gIso  = (*_imu).pfIsolationR04().sumPhotonEt;
		double puIso = (*_imu).pfIsolationR04().sumPUPt;

		double pt    = (*_imu).pt() ;

		double pfIso = (chIso + std::max(0.,nhIso + gIso - 0.5*puIso))/pt;

		if (!mbPar["muon_useMiniIso"] && pfIso<mdPar["muon_reliso"] ) {}
		else if (mbPar["muon_useMiniIso"] && (*_imu).passed(reco::Muon::MiniIsoTight) ) {}
		else{ break;}

		if (mvSelPVs.size() > 0){
		  if ( fabs((*_imu).muonBestTrack()->dxy((*mvSelPVs[0]).position())) < mdPar["muon_dxy"] ){ }
		  else break;
		  if ( fabs((*_imu).muonBestTrack()->dz((*mvSelPVs[0]).position())) < mdPar["muon_dz"] ){ }
		  else break;
		}
                    
		if ( _imu->pt()>mdPar["muon_minpt"] ){ }
		else break;

		if ( fabs(_imu->eta())<mdPar["muon_maxeta"] ){ }
		else break;
                    
		pass = true; // success
		break;
	      }
	      
	      if ( pass ){
		++nSelMuons; 

		// save every good muon
		mvSelMuons.push_back( edm::Ptr<pat::Muon>( mhMuons, _n_muons) );
		mvLooseMuons.push_back( edm::Ptr<pat::Muon>( mhMuons, _n_muons) );
	      }
	      else {
		bool pass_loose = false;
    
		//muon cuts
		while(1){
    
		  if (mbPar["loose_muon_selector_tight"]) {
		    if ( (*_imu).passed(reco::Muon::CutBasedIdTight) ){ } 
		    else break; // fail
		  }
		  else {
		    if ( (*_imu).passed(reco::Muon::CutBasedIdLoose) ){ } 
		    else break; // fail
		  }
		  
		  //			    pat::Muon* muptr = new pat::Muon(*_imu);
		  //float miniIso = getPFMiniIsolation_EffectiveArea(packedPFCands, dynamic_cast<const reco::Candidate* > (muptr), 0.05, 0.2, 10., false, false,myRhoJetsNC);

		  double chIso = (*_imu).pfIsolationR04().sumChargedHadronPt;
		  double nhIso = (*_imu).pfIsolationR04().sumNeutralHadronEt;
		  double gIso  = (*_imu).pfIsolationR04().sumPhotonEt;
		  double puIso = (*_imu).pfIsolationR04().sumPUPt;
		  double pt    = (*_imu).pt() ;

		  double pfIso = (chIso + std::max(0.,nhIso + gIso - 0.5*puIso))/pt;
		  
		  if (!mbPar["muon_useMiniIso"] && pfIso<mdPar["loose_muon_reliso"] ) {}
		  else if (mbPar["muon_useMiniIso"] && (*_imu).passed(reco::Muon::MiniIsoMedium) ) {}
		  else{ break;}
		  
		  if (mvSelPVs.size() > 0){
		    if ( fabs((*_imu).muonBestTrack()->dxy((*mvSelPVs[0]).position())) < mdPar["loose_muon_dxy"] ){ }
		    else break;
		    if ( fabs((*_imu).muonBestTrack()->dz((*mvSelPVs[0]).position())) < mdPar["loose_muon_dz"] ){ }
		    else break;
		  }
                        
		  if ( _imu->pt()>mdPar["loose_muon_minpt"] ){ }
		  else break;
		  
		  if ( fabs(_imu->eta())<mdPar["loose_muon_maxeta"] ){ }
		  else break;
    
		  pass_loose = true; // success
		  break;
		}
    
		if ( pass_loose ){
		  mvLooseMuons.push_back( edm::Ptr<pat::Muon>( mhMuons, _n_muons) );
		  ++nLooseMuons; 
		}
	      }
              
	      _n_muons++;
            } // end of the muon loop

        } // end of muon cuts
        if (mbPar["debug"]) 	std::cout<<"finish muon cuts..."<<std::endl;

        //
        //_____ Electron cuts __________________________________
        //      
        // loop over electrons

        int _n_electrons  = 0;
        int nSelElectrons = 0;
        int nLooseElectrons = 0;
        if (mbPar["debug"]) std::cout<<"start electron cuts..."<<std::endl;

        if ( mbPar["electron_cuts"] ) {
            //get electrons
            event.getByLabel( mtPar["electron_collection"], mhElectrons );
	    //	    std::cout << "SLES L711: got electrons" << std::endl;

            mvSelElectrons.clear();
	    mvLooseElectrons.clear();
	    //	    std::cout << "SLES L714: getting candidates " << std::endl;	      
	    //packed pf candidates and rho source needed miniIso
	    // edm::Handle<pat::PackedCandidateCollection> packedPFCands;
	    // edm::InputTag packedPFCandsLabel_("packedPFCandidates");
	    // event.getByLabel(packedPFCandsLabel_, packedPFCands);
	    // std::cout << "SLES L714: got candidates " << std::endl;	      
	    // //rho isolation from susy recommendation
	    // edm::Handle<double> rhoJetsNC;
	    // event.getByLabel(edm::InputTag("fixedGridRhoFastjetCentralNeutral","") , rhoJetsNC);
	    // double myRhoJetsNC = *rhoJetsNC;
	    // std::cout << "SLES L724: got rho " << std::endl;	      

	    size_t j = 0;
            for (std::vector<pat::Electron>::const_iterator _iel = mhElectrons->begin(); _iel != mhElectrons->end(); _iel++){
                bool pass = false;

                //electron cuts
                while(1){

                    if (_iel->pt()>mdPar["electron_minpt"]){ }
                    else break;
	  
                    if ( fabs(_iel->superCluster()->eta())<mdPar["electron_maxeta"] ){ }
                    else break;

                    //if (!((*_iel).isEBEEGap())){ }
                    if ( fabs(_iel->superCluster()->eta())<1.4442 || fabs(_iel->superCluster()->eta())>1.566 ){ }
                    else break;

                    if ( mbPar["UseElMVA"] ) {

		      //bool mvapass = true;  // HACK FOR TESTING THE MVA EFFICIENCY
		      bool mvapass = false;
		      if(mbPar["UseElIDV1"]) mvapass = _iel->electronID("mvaEleID-Fall17-noIso-V1-wp90");
		      else mvapass = _iel->electronID("mvaEleID-Fall17-noIso-V2-wp90");

		      if (!mvapass) break;
		      
		      if(mbPar["electron_useMiniIso"]){
			bool passIso = false;
			pat::Electron* elptr = new pat::Electron(*_iel);
			float miniIso = getPFMiniIsolation_EffectiveArea(packedPFCands, dynamic_cast<const reco::Candidate* > (elptr), 0.05, 0.2, 10., false, false,myRhoJetsNC);
			
			if(miniIso < mdPar["electron_miniIso"]) passIso = true;                      
			if(!passIso){delete elptr;  break;}
			delete elptr;
		      }
                    }
                    else {
		      bool cutbasedpass = false;
		      if(mbPar["UseElIDV1"]) cutbasedpass = _iel->electronID("cutBasedElectronID-Fall17-94X-V1-tight");
		      else cutbasedpass = _iel->electronID("cutBasedElectronID-Fall17-94X-V2-tight");
		      
		      if(!cutbasedpass) break;
                    }
		    
                    pass = true; // success
                    break;
                }

                if ( pass ){
                     ++nSelElectrons;

                   
                    // save every good electron
                    mvSelElectrons.push_back( edm::Ptr<pat::Electron>( mhElectrons, _n_electrons) );
		    mvLooseElectrons.push_back( edm::Ptr<pat::Electron>( mhElectrons, _n_electrons) );
                }	
		else {
                    bool pass_loose = false;
    
                    //electron cuts
                    while(1){
    
                        if (_iel->pt()>mdPar["loose_electron_minpt"]){ }
                        else break;
    	  
                        if ( fabs(_iel->superCluster()->eta())<mdPar["loose_electron_maxeta"] ){ }
                        else break;
    
                        if (!((*_iel).isEBEEGap())){ }
                        else break;

                        if ( mbPar["UseElMVA"] ) {
			  //bool mvapass = true; // HACK FOR TESTING THE MVA EFFICIENCY
			  bool mvapass = false;

			  if(mbPar["UseElIDV1"]) mvapass = _iel->electronID("mvaEleID-Fall17-noIso-V1-wpLoose");
			  else mvapass = _iel->electronID("mvaEleID-Fall17-noIso-V2-wpLoose");

			  if (!mvapass) break;
			  
			  if(mbPar["electron_useMiniIso"]){
			    bool passIso = false;
			    pat::Electron* elptr = new pat::Electron(*_iel);
			    float miniIso = getPFMiniIsolation_EffectiveArea(packedPFCands, dynamic_cast<const reco::Candidate* > (elptr), 0.05, 0.2, 10., false, false,myRhoJetsNC);
			    
			    if(miniIso < mdPar["loose_electron_miniIso"]) passIso = true;                      
			    if(!passIso){delete elptr;  break;}
			    delete elptr;
			  }
                        }
                        else {
			  bool cutbasedpass = false;
			  if(mbPar["UseElIDV1"]) cutbasedpass = _iel->electronID("cutBasedElectronID-Fall17-94X-V1-loose");
			  else cutbasedpass = _iel->electronID("cutBasedElectronID-Fall17-94X-V2-loose");
			  
			  if(!cutbasedpass) break;
                        }

                        pass_loose = true; // success
                        break;
                    }
    
                    if ( pass_loose ){
		      ++nLooseElectrons;
		      mvLooseElectrons.push_back( edm::Ptr<pat::Electron>( mhElectrons, _n_electrons) );
		    }
		}
                 	
                _n_electrons++;
            } // end of the electron loop

        } // end of electron cuts
        if (mbPar["debug"]) std::cout<<"finish electron cuts..."<<std::endl;

        //
        //_____ Tau cuts __________________________________
        //      
        // loop over taus

        int _n_taus  = 0;
	mbIsTau = 0;
        if (mbPar["debug"]) std::cout<<"start tau cuts..."<<std::endl;
	

        //if ( mbPar["tau_veto"] ) {
	//get taus
	event.getByLabel( mtPar["tau_collection"], mhTaus );      

	for (std::vector<pat::Tau>::const_iterator _itau = mhTaus->begin(); _itau != mhTaus->end(); _itau++){

	  while(1){

	    //Tau cuts hardcoded here	
	    if(_itau->tauID("decayModeFindingNewDMs")){}
	    else break;

	    if(_itau->tauID("byLooseCombinedIsolationDeltaBetaCorr3Hits")){}
	    else break;

	    if(_itau->pt() > 20 && fabs(_itau->eta()) < 2.4 ){}
	    else break;
		
	    ++_n_taus;
	    mbIsTau = 1;
	    break;
		
	  }
	  //	}	  
	}
        if (mbPar["debug"]) 	std::cout<<"finish tau cuts..."<<std::endl;
        
	//
        // jet loop
        //
        //
        if (mbPar["debug"]) std::cout<<"start jet cuts..."<<std::endl;

        event.getByLabel( mtPar["jet_collection"], mhJets );

        int _n_good_jets = 0;
        int _n_jets = 0;
        int nBtagJets = 0;
        double _leading_jet_pt = 0.0;

        mvSelJets.clear();
        mvAllJets.clear();
        mvCorrJetsWithBTags.clear();
	mvSelCorrJets.clear();
	mvCorrJets_jesup.clear();
	mvCorrJets_jesdn.clear();
	mvCorrJets_jerup.clear();
	mvCorrJets_jerdn.clear();
        mvSelBtagJets.clear();

        // try to get earlier produced data (in a calc)
        //std::cout << "Must be 2.34: " << GetTestValue() << std::endl;

	std::vector<edm::Ptr<pat::Muon>> cleaningMuons;
	cleaningMuons = mvSelMuons;
	if(mbPar["CleanLooseLeptons"]) cleaningMuons = mvLooseMuons;
	
	std::vector<edm::Ptr<pat::Electron>> cleaningElectrons;
	cleaningElectrons = mvSelElectrons;
	if(mbPar["CleanLooseLeptons"]) cleaningElectrons = mvLooseElectrons;	

        for (std::vector<pat::Jet>::const_iterator _ijet = mhJets->begin();
             _ijet != mhJets->end(); ++_ijet){

            bool _pass = false;
            bool _passpf = false;
            bool _isTagged = false;
	    bool _cleaned = false;

	    TLorentzVector jetP4;
	    TLorentzVector jetP4_jesup;
	    TLorentzVector jetP4_jesdn;
	    TLorentzVector jetP4_jerup;
	    TLorentzVector jetP4_jerdn;

	    pat::Jet tmpJet = _ijet->correctedJet(0);
	    pat::Jet corrJet = *_ijet;

	    if ( mbPar["doLepJetCleaning"] ){
	      if (mbPar["debug"]) std::cout << "Checking Overlap" << std::endl;

	      for(unsigned int imu = 0; imu < cleaningMuons.size(); imu++){
		if ( deltaR(cleaningMuons[imu]->p4(),_ijet->p4()) < mdPar["LepJetDR"]) { //0.6 ){
		  std::vector<reco::CandidatePtr> muDaughters;
		  for ( unsigned int isrc = 0; isrc < cleaningMuons[imu]->numberOfSourceCandidatePtrs(); ++isrc ){
		    if (cleaningMuons[imu]->sourceCandidatePtr(isrc).isAvailable()) {
		      muDaughters.push_back( cleaningMuons[imu]->sourceCandidatePtr(isrc) );
		    }
		  }
		  if (mbPar["debug"]) {		    
		    std::cout << "         Muon : pT = " << cleaningMuons[imu]->pt() << " eta = " << cleaningMuons[imu]->eta() << " phi = " << cleaningMuons[imu]->phi() << std::endl;
		    std::cout << "      Raw Jet : pT = " << _ijet->pt() << " eta = " << _ijet->eta() << " phi = " << _ijet->phi() << std::endl;
		  }

		  const std::vector<edm::Ptr<reco::Candidate> > _ijet_consts = _ijet->daughterPtrVector();
		  for ( std::vector<edm::Ptr<reco::Candidate> >::const_iterator _i_const = _ijet_consts.begin(); _i_const != _ijet_consts.end(); ++_i_const){
		    for (unsigned int muI = 0; muI < muDaughters.size(); muI++) {
		      if ( (*_i_const).key() == muDaughters[muI].key() ) {
			tmpJet.setP4( tmpJet.p4() - muDaughters[muI]->p4() );
			if (mbPar["debug"]) std::cout << "  Cleaned Jet : pT = " << tmpJet.pt() << " eta = " << tmpJet.eta() << " phi = " << tmpJet.phi() << std::endl;
			jetP4 = correctJet(tmpJet, event, false, true);
			if (mbPar["doAllJetSyst"]) {
			  jetP4_jesup = correctJet(tmpJet, event,false,true,1);
			  jetP4_jesdn = correctJet(tmpJet, event,false,true,2);
			  jetP4_jerup = correctJet(tmpJet, event,false,true,3);
			  jetP4_jerdn = correctJet(tmpJet, event,false,true,4);
			}
			corrJet = correctJetReturnPatJet(tmpJet, event, false, true);
			if (mbPar["debug"]) std::cout << "Corrected Jet : pT = " << jetP4.Pt() << " eta = " << jetP4.Eta() << " phi = " << jetP4.Phi() << std::endl;
			_cleaned = true;
			muDaughters.erase( muDaughters.begin()+muI );
			break;
		      }
		    }
		  }
		}
	      }
            
	      for(unsigned int iel = 0; iel < cleaningElectrons.size(); iel++){
		if ( deltaR(cleaningElectrons[iel]->p4(),_ijet->p4()) < mdPar["LepJetDR"]){ //0.6 ){
		  std::vector<reco::CandidatePtr> elDaughters;
		  for ( unsigned int isrc = 0; isrc < cleaningElectrons[iel]->numberOfSourceCandidatePtrs(); ++isrc ){
		    if (cleaningElectrons[iel]->sourceCandidatePtr(isrc).isAvailable()) {
		      elDaughters.push_back( cleaningElectrons[iel]->sourceCandidatePtr(isrc) );
		    }
		  }
		  if (mbPar["debug"]) {
		    std::cout << "     Electron : pT = " << cleaningElectrons[iel]->pt() << " eta = " << cleaningElectrons[iel]->eta() << " phi = " << cleaningElectrons[iel]->phi() << std::endl;
		    std::cout << "      Raw Jet : pT = " << _ijet->correctedJet(0).pt() << " eta = " << _ijet->correctedJet(0).eta() << " phi = " << _ijet->correctedJet(0).phi() << std::endl;
		  }
		  const std::vector<edm::Ptr<reco::Candidate> > _ijet_consts = _ijet->daughterPtrVector();
		  for ( std::vector<edm::Ptr<reco::Candidate> >::const_iterator _i_const = _ijet_consts.begin(); _i_const != _ijet_consts.end(); ++_i_const){
		    for (unsigned int elI = 0; elI < elDaughters.size(); elI++) {
		      if ( (*_i_const).key() == elDaughters[elI].key() ) {
			tmpJet.setP4( tmpJet.p4() - elDaughters[elI]->p4() );
			if (mbPar["debug"]) std::cout << "  Cleaned Jet : pT = " << tmpJet.pt() << " eta = " << tmpJet.eta() << " phi = " << tmpJet.phi() << std::endl;
			jetP4 = correctJet(tmpJet, event, false, true);
			if (mbPar["doAllJetSyst"]) {
			  jetP4_jesup = correctJet(tmpJet, event,false,true,1);
			  jetP4_jesdn = correctJet(tmpJet, event,false,true,2);
			  jetP4_jerup = correctJet(tmpJet, event,false,true,3);
			  jetP4_jerdn = correctJet(tmpJet, event,false,true,4);
			}
			corrJet = correctJetReturnPatJet(tmpJet, event, false, true);
			if (mbPar["debug"]) std::cout << "Corrected Jet : pT = " << jetP4.Pt() << " eta = " << jetP4.Eta() << " phi = " << jetP4.Phi() << std::endl;
			_cleaned = true;
			elDaughters.erase( elDaughters.begin()+elI );
			break;
		      }
		    }
		  }
		}
	      }
	    }

	    if (!_cleaned) {
                jetP4 = correctJet(*_ijet, event);
                if (mbPar["doAllJetSyst"]) {
	            jetP4_jesup = correctJet(*_ijet, event,false,true,1);
	            jetP4_jesdn = correctJet(*_ijet, event,false,true,2);
	            jetP4_jerup = correctJet(*_ijet, event,false,true,3);
	            jetP4_jerdn = correctJet(*_ijet, event,false,true,4);
                }
		corrJet = correctJetReturnPatJet(*_ijet, event);
            }

            _isTagged = isJetTagged(*_ijet, event);

            // jet cuts
            while(1){ 

	        // PF Jet ID
	        if (fabs(_ijet->correctedJet(0).eta()) < 2.4 &&
		    _ijet->correctedJet(0).neutralHadronEnergyFraction() < 0.90 &&
		    _ijet->correctedJet(0).neutralEmEnergyFraction() < 0.90 &&
		    _ijet->correctedJet(0).chargedMultiplicity()+_ijet->correctedJet(0).neutralMultiplicity() > 1 &&
		    _ijet->correctedJet(0).chargedHadronEnergyFraction() > 0 &&
		    _ijet->correctedJet(0).chargedMultiplicity() > 0
		    ){ } 
	        else if (fabs(_ijet->correctedJet(0).eta()) >= 2.4 && 
			 fabs(_ijet->correctedJet(0).eta()) < 2.7 &&
			 _ijet->correctedJet(0).neutralHadronEnergyFraction() < 0.90 &&
			 _ijet->correctedJet(0).neutralEmEnergyFraction() < 0.90 &&
			 _ijet->correctedJet(0).chargedMultiplicity()+_ijet->correctedJet(0).neutralMultiplicity() > 1
			 ){ } 
	        else if (fabs(_ijet->correctedJet(0).eta()) >= 2.7 && 
			 fabs(_ijet->correctedJet(0).eta()) < 3.0 && 
			 _ijet->correctedJet(0).neutralEmEnergyFraction() > 0.02 && 
			 _ijet->correctedJet(0).neutralEmEnergyFraction() < 0.99 &&
			 _ijet->correctedJet(0).neutralMultiplicity() > 2
			 ){ }
		else if (fabs(_ijet->correctedJet(0).eta()) >= 3.0 && 
			 _ijet->correctedJet(0).neutralEmEnergyFraction() < 0.9 &&
			 _ijet->correctedJet(0).neutralHadronEnergyFraction() > 0.02 &&
			 _ijet->correctedJet(0).neutralMultiplicity() > 10
			 ){ }
	        else break; // fail 

                _passpf = true;

                if ( jetP4.Pt() > mdPar["jet_minpt"] ){ }
                else break; // fail 
	

                if ( fabs(jetP4.Eta()) < mdPar["jet_maxeta"] ){ }
                else break; // fail
	
                _pass = true;
                break;
            }

            pair<TLorentzVector,bool> jetwithtag;
            jetwithtag.first = jetP4;
            jetwithtag.second = _isTagged;

            if ( _pass ){


                // save all the good jets
                ++_n_good_jets;
                mvSelJets.push_back(edm::Ptr<pat::Jet>( mhJets, _n_jets)); 
		mvSelCorrJets.push_back(corrJet);
                if (mbPar["doAllJetSyst"]) {
		    mvCorrJets_jesup.push_back(jetP4_jesup);
		    mvCorrJets_jesdn.push_back(jetP4_jesdn);
		    mvCorrJets_jerup.push_back(jetP4_jerup);
		    mvCorrJets_jerdn.push_back(jetP4_jerdn);
                }
                else {
		    mvCorrJets_jesup.push_back(jetP4);
		    mvCorrJets_jesdn.push_back(jetP4);
		    mvCorrJets_jerup.push_back(jetP4);
		    mvCorrJets_jerdn.push_back(jetP4);
                }
                mvCorrJetsWithBTags.push_back(jetwithtag);

                if (jetP4.Pt() > _leading_jet_pt) _leading_jet_pt = jetP4.Pt();                         
            
                if (_isTagged) {
                    ++nBtagJets;
                    // save all the good b-tagged jets
                    mvSelBtagJets.push_back(edm::Ptr<pat::Jet>( mhJets, _n_jets)); 
                }
            }
      
            // save all the pf jets regardless of pt or eta
            // needed for MET corrections with JER/JES uncertainty
            if (_passpf)  mvAllJets.push_back(edm::Ptr<pat::Jet>( mhJets, _n_jets)); 

            ++_n_jets; 
      
        } // end of loop over jets
	//	std::cout << "Done with AK4 cleaning" << std::endl;

	////////////////////////////////////////////////////////////////////////////////////////
	// AK8 jets
	////////////////////////////////////////////////////////////////////////////////////////

        event.getByLabel( mtPar["slimmedJetsAK8"], mhJets_AK8 );

        int _n_good_jets_AK8 = 0;
        int _n_jets_AK8 = 0;

        mvSelCorrJets_AK8.clear();

        for (std::vector<pat::Jet>::const_iterator _ijet = mhJets_AK8->begin();
             _ijet != mhJets_AK8->end(); ++_ijet){
      
      	    if(_ijet->pt() < 170) continue;
	    if(_ijet->correctedJet(0).pt() < 170) continue;

            bool _pass = false;
            bool _passpf = false;
            bool _isTagged = false;
	    bool _cleaned = false;

	    TLorentzVector jetP4;

	    pat::Jet tmpJet = _ijet->correctedJet(0);
	    pat::Jet corrJet = *_ijet;

	    if ( mbPar["doLepJetCleaning"] ){
	      if (mbPar["debug"]) std::cout << "Checking Overlap" << std::endl;

	      for(unsigned int imu = 0; imu < cleaningMuons.size(); imu++){
		if ( deltaR(cleaningMuons[imu]->p4(),_ijet->p4()) < mdPar["LepJetDRAK8"]) { //0.8 ){
		  std::vector<reco::CandidatePtr> muDaughters;
		  for ( unsigned int isrc = 0; isrc < cleaningMuons[imu]->numberOfSourceCandidatePtrs(); ++isrc ){
		    if (cleaningMuons[imu]->sourceCandidatePtr(isrc).isAvailable()) {
		      muDaughters.push_back( cleaningMuons[imu]->sourceCandidatePtr(isrc) );
		    }
		  }
		  if (mbPar["debug"]) {		    
		    std::cout << "         Muon : pT = " << cleaningMuons[imu]->pt() << " eta = " << cleaningMuons[imu]->eta() << " phi = " << cleaningMuons[imu]->phi() << " mass = " << cleaningMuons[imu]->mass() << std::endl;
		    std::cout << "      Raw Jet : pT = " << _ijet->pt() << " eta = " << _ijet->eta() << " phi = " << _ijet->phi() << " mass = " << _ijet->mass() << std::endl;
		  }

		  const std::vector<edm::Ptr<reco::Candidate> > _ijet_consts = _ijet->daughterPtrVector();
		  for ( std::vector<edm::Ptr<reco::Candidate> >::const_iterator _i_const = _ijet_consts.begin(); _i_const != _ijet_consts.end(); ++_i_const){
		    for (unsigned int muI = 0; muI < muDaughters.size(); muI++) {
		      if ( (*_i_const).key() == muDaughters[muI].key() ) {
			tmpJet.setP4( tmpJet.p4() - muDaughters[muI]->p4() );
			if (mbPar["debug"]) std::cout << "  Cleaned Jet : pT = " << tmpJet.pt() << " eta = " << tmpJet.eta() << " phi = " << tmpJet.phi() << " mass = " << tmpJet.mass() << std::endl;
			jetP4 = correctJet(tmpJet, event, true, true);
			corrJet = correctJetReturnPatJet(tmpJet, event, true, true);
			if (mbPar["debug"]) std::cout << "Corrected Jet : pT = " << jetP4.Pt() << " eta = " << jetP4.Eta() << " phi = " << jetP4.Phi() << " mass = " << jetP4.M() << std::endl;
			_cleaned = true;
			muDaughters.erase( muDaughters.begin()+muI );
			break;
		      }
		    }
		  }
		}
	      }
            
	      for(unsigned int iel = 0; iel < cleaningElectrons.size(); iel++){
		if ( deltaR(cleaningElectrons[iel]->p4(),_ijet->p4()) < mdPar["LepJetDRAK8"]){ //0.6 ){
		  std::vector<reco::CandidatePtr> elDaughters;
		  for ( unsigned int isrc = 0; isrc < cleaningElectrons[iel]->numberOfSourceCandidatePtrs(); ++isrc ){
		    if (cleaningElectrons[iel]->sourceCandidatePtr(isrc).isAvailable()) {
		      elDaughters.push_back( cleaningElectrons[iel]->sourceCandidatePtr(isrc) );
		    }
		  }
		  if (mbPar["debug"]) {
		    std::cout << "     Electron : pT = " << cleaningElectrons[iel]->pt() << " eta = " << cleaningElectrons[iel]->eta() << " phi = " << cleaningElectrons[iel]->phi() << " mass = " << cleaningElectrons[iel]->mass() << std::endl;
		    std::cout << "      Raw Jet : pT = " << _ijet->correctedJet(0).pt() << " eta = " << _ijet->correctedJet(0).eta() << " phi = " << _ijet->correctedJet(0).phi() << " mass = " << _ijet->correctedJet(0).mass() << std::endl;
		  }
		  const std::vector<edm::Ptr<reco::Candidate> > _ijet_consts = _ijet->daughterPtrVector();
		  for ( std::vector<edm::Ptr<reco::Candidate> >::const_iterator _i_const = _ijet_consts.begin(); _i_const != _ijet_consts.end(); ++_i_const){
		    for (unsigned int elI = 0; elI < elDaughters.size(); elI++) {
		      if ( (*_i_const).key() == elDaughters[elI].key() ) {
			tmpJet.setP4( tmpJet.p4() - elDaughters[elI]->p4() );
			if (mbPar["debug"]) std::cout << "  Cleaned Jet : pT = " << tmpJet.pt() << " eta = " << tmpJet.eta() << " phi = " << tmpJet.phi() << " mass = " << tmpJet.mass() << std::endl;
			jetP4 = correctJet(tmpJet, event, true, true);
			corrJet = correctJetReturnPatJet(tmpJet, event, true, true);
			if (mbPar["debug"]) std::cout << "Corrected Jet : pT = " << jetP4.Pt() << " eta = " << jetP4.Eta() << " phi = " << jetP4.Phi() << " mass = " << jetP4.M() << std::endl;
			_cleaned = true;
			elDaughters.erase( elDaughters.begin()+elI );
			break;
		      }
		    }
		  }
		}
	      }
	    }

	    if (!_cleaned) {
                jetP4 = correctJet(*_ijet, event);
		corrJet = correctJetReturnPatJet(*_ijet, event);
            }

            // jet cuts
            while(1){ 

	      // PF Jet ID                                                                                                                                                                     
	      if (fabs(_ijet->correctedJet(0).eta()) < 2.4 &&
		  _ijet->correctedJet(0).neutralHadronEnergyFraction() < 0.90 &&
		  _ijet->correctedJet(0).neutralEmEnergyFraction() < 0.90 &&
		  //_ijet->userFloat("patPuppiJetSpecificProducer:puppiMultiplicity") > 1 &&
		  _ijet->correctedJet(0).chargedMultiplicity()+_ijet->correctedJet(0).neutralMultiplicity() > 1 &&
		  _ijet->correctedJet(0).chargedHadronEnergyFraction() > 0 &&
		  _ijet->correctedJet(0).chargedMultiplicity() > 0
		  ){ }
	      else if (fabs(_ijet->correctedJet(0).eta()) >= 2.4 &&
		       fabs(_ijet->correctedJet(0).eta()) < 2.7 &&
		       _ijet->correctedJet(0).neutralHadronEnergyFraction() < 0.90 &&
		       _ijet->correctedJet(0).neutralEmEnergyFraction() < 0.90 &&
		       //_ijet->userFloat("patPuppiJetSpecificProducer:puppiMultiplicity") > 1
		       _ijet->correctedJet(0).chargedMultiplicity()+_ijet->correctedJet(0).neutralMultiplicity() > 1
		       ){ }
	      else if (fabs(_ijet->correctedJet(0).eta()) >= 2.7 &&
		       fabs(_ijet->correctedJet(0).eta()) < 3.0 &&
		       _ijet->correctedJet(0).neutralHadronEnergyFraction() < 0.99
		       ){ }
	      else if (fabs(_ijet->correctedJet(0).eta()) >= 3.0 &&                                                                                                                         
		       _ijet->correctedJet(0).neutralEmEnergyFraction() < 0.9 &&                                                                                                               
		       _ijet->correctedJet(0).neutralHadronEnergyFraction() > 0.02 &&                                                                                                          
		       //_ijet->userFloat("patPuppiJetSpecificProducer:neutralPuppiMultiplicity") > 2 &&
		       //_ijet->userFloat("patPuppiJetSpecificProducer:neutralPuppiMultiplicity") < 15                                                                                        
		       _ijet->correctedJet(0).neutralMultiplicity() > 2 &&
		       _ijet->correctedJet(0).neutralMultiplicity() < 15
	             ){ }                                                                                                                                                                    
	      else break; // fail

	      _passpf = true;

	      if ( jetP4.Pt() > mdPar["jet_minpt_AK8"] ){ }
	      else break; // fail 
	

	      if ( fabs(jetP4.Eta()) < mdPar["jet_maxeta_AK8"] ){ }
	      else break; // fail


	      if ( jetP4.M() >= 0 ){ }
	      else{
		std::cout << " singleLepEventSelector -- Warning : Jet mass < 0! Skipping..." << std::endl;
		break; // fail 
	      }
	
	      _pass = true;
	      break;
            }

            if ( _pass ){

                // save all the good jets
                ++_n_good_jets_AK8;
		mvSelCorrJets_AK8.push_back(corrJet);

	    }
      
            ++_n_jets_AK8; 
      
        } // end of loop over AK8 jets
	//	std::cout << "Done with AK8 cleaning" << std::endl;

        //
        if ( mbPar["jet_cuts"] ) {

            if ( ignoreCut("One jet or more") || _n_good_jets >= 1 ) passCut(ret, "One jet or more");
            else break; 
	
            if ( ignoreCut("Two jets or more") || _n_good_jets >= 2 ) passCut(ret, "Two jets or more");
            else break; 
	
            if ( ignoreCut("Three jets or more") || _n_good_jets >= 3 ) passCut(ret, "Three jets or more");
            else break; 
	
            if ( ignoreCut("Min jet multiplicity") || _n_good_jets >= cut("Min jet multiplicity",int()) ) passCut(ret, "Min jet multiplicity");
            else break; 
	
            if ( ignoreCut("Max jet multiplicity") || _n_good_jets <= cut("Max jet multiplicity",int()) ) passCut(ret, "Max jet multiplicity");
            else break; 
            if ( ignoreCut("Leading jet pt") ||  _leading_jet_pt >= cut("Leading jet pt",double()) ) passCut(ret, "Leading jet pt");
            else break;

        } // end of jet cuts
        if (mbPar["debug"]) 	std::cout<<"finish jet cuts..."<<std::endl;

        //
        //_____ MET cuts __________________________________
        //   
        if (mbPar["debug"]) std::cout<<"start met cuts..."<<std::endl;

        event.getByLabel( mtPar["met_collection"], mhMet );
        mpMet = edm::Ptr<pat::MET>( mhMet, 0);

        if ( mbPar["met_cuts"] ) {

            // pfMet
            //if ( mpType1CorrMet.isNonnull() && mpType1CorrMet.isAvailable() ) {
            if ( mpMet.isNonnull() && mpMet.isAvailable() ) {
                pat::MET const & met = mhMet->at(0);
                TLorentzVector corrMET = correctMet(met, event);
                if ( ignoreCut("Min MET") || corrMET.Pt()>cut("Min MET", double()) ) passCut(ret, "Min MET");
                else break;

                if ( ignoreCut("Max MET") || corrMET.Pt()<cut("Max MET", double()) ) passCut(ret, "Max MET");
                else break;

            }
        } // end of MET cuts
        if (mbPar["debug"]) 	std::cout<<"finish met cuts..."<<std::endl;

	//
	//_____ Lepton cuts ________________________________

        if (mbPar["debug"]) std::cout<<"start lepton cuts..."<<std::endl;

        int nLeptons = nSelElectrons + nSelMuons;
	int nTotalLeptons = nLooseElectrons + nLooseMuons + nSelElectrons + nSelMuons;

        if( nSelMuons >= cut("Min muon", int()) || ignoreCut("Min muon") ) passCut(ret, "Min muon");
        else break;
        if( nSelElectrons >= cut("Min electron", int()) || ignoreCut("Min electron") ) passCut(ret, "Min electron");
        else break;
        if( nLeptons >= cut("Min lepton", int()) || ignoreCut("Min lepton") ) passCut(ret, "Min lepton");
        else break;
        if( nLeptons <= cut("Max lepton", int()) || ignoreCut("Max lepton") ) passCut(ret, "Max lepton");
        else break;

        if( nTotalLeptons >= cut("Min loose lepton", int()) || ignoreCut("Min loose lepton") ) passCut(ret, "Min loose lepton");
        else break;
        if( nTotalLeptons <= cut("Max loose lepton", int()) || ignoreCut("Max loose lepton") ) passCut(ret, "Max loose lepton");
        else break;

        
        bool NoSecondLepton = true;
        if ( (nSelMuons > 0 || nSelElectrons > 0) && ((nLooseElectrons + nLooseMuons) > 0) ) NoSecondLepton = false;
       
        if( NoSecondLepton || ignoreCut("Second lepton veto") ) passCut(ret, "Second lepton veto");
        else break;
        
        if( _n_taus == 0 || !mbPar["tau_veto"]) passCut(ret, "Tau veto");
        else break;
        
        if (mbPar["debug"]) 	std::cout<<"finish lepton cuts..."<<std::endl;
    
        //
        //_____ Btagging cuts _____________________
        //
        if (mbPar["debug"]) std::cout<<"start btag cuts..."<<std::endl;

        if ( mbPar["btag_cuts"] ) {
              
            if ( nBtagJets >= 1 || ignoreCut("1 btag or more") )  passCut(ret, "1 btag or more");
            else break;
            if ( nBtagJets >= 2 || ignoreCut("2 btag or more") )  passCut(ret, "2 btag or more");
            else break;
            if ( nBtagJets >= 3 || ignoreCut("3 btag or more") )  passCut(ret, "3 btag or more");
            else break;

        }
        if (mbPar["debug"]) std::cout<<"finish btag cuts..."<<std::endl;
    
        passCut(ret, "All cuts");
        break;

    } // end of while loop


    bFirstEntry = false;
    
    return (bool)ret;
  
    setIgnored(ret);

    return false;
}// end of operator()


void singleLepEventSelector::AnalyzeEvent( edm::EventBase const & event, LjmetEventContent & ec )
{
    //
    // Compute analysis-specific quantities in the event,
    // return via event content
    //

//   ec.SetValue("pi", 3.14);
  

    return;
}

void singleLepEventSelector::EndJob()
{
}

#endif
