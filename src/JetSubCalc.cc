/*
 Calculator for substructure variables
 
 Author: Joshua Swanson, 2014. Updated Julie Hogan, 2015. 
 */

#include <iostream>
#include <limits>   // std::numeric_limits
#include <vector>
#include <string>
#include "TLorentzVector.h"

#include "LJMet/Com/interface/BaseCalc.h"
#include "LJMet/Com/interface/BaseEventSelector.h"
#include "LJMet/Com/interface/LjmetFactory.h"
#include "LJMet/Com/interface/LjmetEventContent.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/JetReco/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/PatCandidates/interface/PATObject.h"
#include "DataFormats/BTauReco/interface/CATopJetTagInfo.h"
#include "LJMet/Com/interface/HTTTopJetTagInfo.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"
#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"

using namespace std;

class LjmetFactory;

class JetSubCalc : public BaseCalc {
    
public:
    JetSubCalc();
    virtual ~JetSubCalc();
    virtual int BeginJob();
    virtual int AnalyzeEvent(edm::EventBase const & event, BaseEventSelector * selector);
    virtual int EndJob();
    
private:
  edm::InputTag slimmedJetColl_it;
  edm::InputTag slimmedJetsAK8Coll_it;
  edm::InputTag slimmedJetsAK8SDColl_it;
  edm::InputTag slimmedJetsAK8CTTColl_it;
  edm::InputTag selectedPatJetsCA15Coll_it;
  edm::InputTag genParticles_it;
  edm::InputTag httTagInfo_it;
  std::string bDiscriminant;
  std::string tagInfo;
  double kappa;
  bool useHTT;
  bool killHF;
  bool doNewJEC;
  bool isMc;
  bool JECup;
  bool JECdn;
  bool JERup;
  bool JERdn;
  bool useL2L3Mass;
  std::string MCL3;
  std::string MCL2;
  std::string MCSF;
  std::string MCPT;
  std::string DataL3;
  std::string DataL2;
  std::string DataL2L3;
  JME::JetResolution resolutionAK8;
  JME::JetResolutionScaleFactor resolution_SF;
  FactorizedJetCorrector *jecak8;
  JetCorrectorParameters *L3JetParAK8;
  JetCorrectorParameters *L2JetParAK8;
  JetCorrectorParameters *ResJetParAK8; 
  JetCorrectionUncertainty *jecUnc;
  TRandom3 JERrand;
};

static int reg = LjmetFactory::GetInstance()->Register(new JetSubCalc(), "JetSubCalc");

struct sortclass {
  //  bool operator() (double i,double j) { return (i>j);}
  bool operator() (pat::Jet i, pat::Jet j){ return (i.pt() > j.pt());}
} ptsorter;


JetSubCalc::JetSubCalc()
{
}

JetSubCalc::~JetSubCalc()
{
}

int JetSubCalc::BeginJob()
{
    if (mPset.exists("useHTT")) useHTT = mPset.getParameter<bool>("useHTT");
    else useHTT = false;

    if (mPset.exists("slimmedJetColl")){ slimmedJetColl_it = mPset.getParameter<edm::InputTag>("slimmedJetColl");}
    else slimmedJetColl_it = edm::InputTag("slimmedJets");
    
    if (mPset.exists("slimmedJetsAK8Coll")) slimmedJetsAK8Coll_it = mPset.getParameter<edm::InputTag>("slimmedJetsAK8Coll");
    else slimmedJetsAK8Coll_it = edm::InputTag("slimmedJetsAK8");

    if (mPset.exists("genParticles")) genParticles_it = mPset.getParameter<edm::InputTag>("genParticles");
    else                              genParticles_it = edm::InputTag("prunedGenParticles");

    if (mPset.exists("bDiscriminant")) bDiscriminant = mPset.getParameter<std::string>("bDiscriminant");
    else bDiscriminant = "pfCombinedInclusiveSecondaryVertexV2BJetTags";
    
    std::cout << " JetSubCalc Bdisc = " << bDiscriminant << std::endl;
    
    if (mPset.exists("tagInfo")) tagInfo = mPset.getParameter<std::string>("tagInfo");
    else tagInfo = "CATop";

    if(mPset.exists("kappa")) kappa = mPset.getParameter<double>("kappa");
    else kappa = 0.5;

    if(mPset.exists("killHF")) killHF = mPset.getParameter<bool>("killHF");
    else killHF = false;

    if(mPset.exists("doNewJEC")) doNewJEC = mPset.getParameter<bool>("doNewJEC");
    else doNewJEC = false;

    cout << "JetSubCalc: doNewJEC = " << doNewJEC << ", killHF = " << killHF << endl;

    if(useHTT){
      cout << " JetSubCalc: using HTT -- have you installed HEPTopTagger v2?" << endl;

      if (mPset.exists("httTagInfo")) httTagInfo_it = mPset.getParameter<edm::InputTag>("httTagInfo");
      else httTagInfo_it = edm::InputTag("HTT");
  
      if (mPset.exists("selectedJetsCA15Coll")) selectedPatJetsCA15Coll_it = mPset.getParameter<edm::InputTag>("selectedJetsCA15Coll");
      else selectedPatJetsCA15Coll_it = edm::InputTag("selectedPatJetsCA15PFCHSNoHF");
    }

    if(mPset.exists("useL2L3Mass")) useL2L3Mass = mPset.getParameter<bool>("useL2L3Mass");
    else useL2L3Mass = false;

    if(mPset.exists("isMc")) isMc = mPset.getParameter<bool>("isMc");
    else isMc = false;

    if(mPset.exists("JECup")) JECup = mPset.getParameter<bool>("JECup");
    else JECup = false;

    if(mPset.exists("JECdown")) JECdn = mPset.getParameter<bool>("JECdown");
    else JECdn = false;

    if(mPset.exists("JERup")) JERup = mPset.getParameter<bool>("JERup");
    else JERup = false;

    if(mPset.exists("JERdown")) JERdn = mPset.getParameter<bool>("JERdown");
    else JERdn = false;

    if(useL2L3Mass){
      cout << "JetSubCalc: using L2+L3 corrected groomed masses" << endl;
      if(mPset.exists("MCL2JetParAK8")) MCL2 = mPset.getParameter<std::string>("MCL2JetParAK8");
      else MCL2 = "/uscms_data/d3/jmanagan/CMSSW_7_4_14/src/LJMet/Com/data/Summer15_25nsV5_MC_L1FastJet";
      if(mPset.exists("MCL3JetParAK8")) MCL3 = mPset.getParameter<std::string>("MCL3JetParAK8");
      else MCL3 = "/uscms_data/d3/jmanagan/CMSSW_7_4_14/src/LJMet/Com/data/Summer15_25nsV5_MC_L1FastJet";
      if(mPset.exists("MCSF")) MCSF = mPset.getParameter<std::string>("MCSF");
      else MCSF = "/uscms_data/d3/jmanagan/CMSSW_7_4_14/src/LJMet/Com/data/Summer15_25nsV5_MC_L1FastJet";
      if(mPset.exists("MCPTResAK8")) MCPT = mPset.getParameter<std::string>("MCPTResAK8");
      else MCPT = "/uscms_data/d3/jmanagan/CMSSW_7_4_14/src/LJMet/Com/data/Summer15_25nsV5_MC_L1FastJet";

      if(mPset.exists("DataL2JetParAK8")) DataL2 = mPset.getParameter<std::string>("DataL2JetParAK8");
      else DataL2 = "/uscms_data/d3/jmanagan/CMSSW_7_4_14/src/LJMet/Com/data/Summer15_25nsV5_Data_L1FastJet";
      if(mPset.exists("DataL3JetParAK8")) DataL3 = mPset.getParameter<std::string>("DataL3JetParAK8");
      else DataL3 = "/uscms_data/d3/jmanagan/CMSSW_7_4_14/src/LJMet/Com/data/Summer15_25nsV5_Data_L1FastJet";
      if(mPset.exists("DataL2L3JetParAK8")) DataL2L3 = mPset.getParameter<std::string>("DataL2L3JetParAK8");
      else DataL2L3 = "/uscms_data/d3/jmanagan/CMSSW_7_4_14/src/LJMet/Com/data/Summer15_25nsV5_Data_L1FastJet";
 
      std::vector<JetCorrectorParameters> vParAK8;
      
      if(isMc){
	L3JetParAK8  = new JetCorrectorParameters(MCL3);
	L2JetParAK8  = new JetCorrectorParameters(MCL2);
	vParAK8.push_back(*L2JetParAK8);
	vParAK8.push_back(*L3JetParAK8);
      }else{
	ResJetParAK8 = new JetCorrectorParameters(DataL2L3); 
	L3JetParAK8  = new JetCorrectorParameters(DataL3);
	L2JetParAK8  = new JetCorrectorParameters(DataL2);
	vParAK8.push_back(*L2JetParAK8);
	vParAK8.push_back(*L3JetParAK8);
	vParAK8.push_back(*ResJetParAK8);
      }
      
      jecak8 = new FactorizedJetCorrector(vParAK8);
      jecUnc = new JetCorrectionUncertainty(mPset.getParameter<std::string>("UncertaintyAK8"));

      resolutionAK8 = JME::JetResolution(MCPT);
      resolution_SF = JME::JetResolutionScaleFactor(MCSF);    

    }

    return 0;
}

int JetSubCalc::AnalyzeEvent(edm::EventBase const & event, BaseEventSelector * selector)
{
    // ----- Get AK4 jet objects from the selector -----
    // This is updated -- original version used all AK4 jets without selection 
    std::vector<pat::Jet>                       const & theJets = selector->GetSelectedCorrJets();
    std::vector<std::pair<TLorentzVector,bool>> const & theCorrBtagJets = selector->GetCorrJetsWithBTags();

    // Old version
    //    edm::Handle<std::vector<pat::Jet> > theJets;
    //    event.getByLabel(slimmedJetColl_it, theJets);

    float subjetCSV;
    int CSVL, CSVM, CSVT;
    double theJetHT;
    
    // Available variables
    std::vector<double> theJetPt;
    std::vector<double> theJetEta;
    std::vector<double> theJetPhi;
    std::vector<double> theJetEnergy;
    std::vector<double> theJetCSV;

    std::vector<double> theJetCEmEnergy;
    std::vector<double> theJetNEmEnergy;
    std::vector<double> theJetCEmEFrac;
    std::vector<double> theJetNEmEFrac;
    std::vector<double> theJetCHadEnergy;
    std::vector<double> theJetNHadEnergy;
    std::vector<double> theJetCHadEFrac;
    std::vector<double> theJetNHadEFrac;
    std::vector<double> theJetMuonEFrac;

    // Additional variables related to the associated secondary vertex if there is one
    // Mass of the vertex
    std::vector<double> theJetVtxMass;
    // Number of tracks
    std::vector<double> theJetVtxNtracks;
    // Decay length value and significance
    std::vector<double> theJetVtx3DVal;
    std::vector<double> theJetVtx3DSig;
    
    // Discriminator for the MVA PileUp id.
    // NOTE: Training used is for ak5PFJetsCHS in CMSSW 5.3.X and Run 1 pileup
    std::vector<double> theJetPileupJetId;
    
    //Identity
    std::vector<int> theJetIndex;
    std::vector<int> theJetnDaughters;
    
    //Daughter four std::vector and index
    std::vector<double> theJetDaughterPt;
    std::vector<double> theJetDaughterEta;
    std::vector<double> theJetDaughterPhi;
    std::vector<double> theJetDaughterEnergy;
    
    std::vector<int> theJetDaughterMotherIndex;
    std::vector<int> theJetCSVLSubJets;
    std::vector<int> theJetCSVMSubJets;
    std::vector<int> theJetCSVTSubJets;

    std::vector<int> theJetPFlav;
    std::vector<int> theJetHFlav;
    std::vector<int> theJetBTag;
    std::vector<int> theJetBTag_bSFup;
    std::vector<int> theJetBTag_bSFdn;
    std::vector<int> theJetBTag_lSFup;
    std::vector<int> theJetBTag_lSFdn;
    
    double theVtxMass, theVtxNtracks, theVtx3DVal, theVtx3DSig, thePileupJetId;

    for (std::vector<pat::Jet>::const_iterator ijet = theJets.begin(); ijet != theJets.end(); ijet++) {
      int index = (int)(ijet-theJets.begin());

      if(killHF && fabs(ijet->eta()) > 2.4) continue;

      theVtxNtracks  = -std::numeric_limits<double>::max();
      theVtxNtracks  = (double)ijet->userFloat("vtxNtracks");
				
      if (theVtxNtracks > 0) {
	theVtxMass     = -std::numeric_limits<double>::max();
	theVtx3DVal    = -std::numeric_limits<double>::max();
	theVtx3DSig    = -std::numeric_limits<double>::max();
	
	theVtxMass     = (double)ijet->userFloat("vtxMass");
	theVtx3DVal    = (double)ijet->userFloat("vtx3DVal");
	theVtx3DSig    = (double)ijet->userFloat("vtx3DSig");
        
	theJetVtxNtracks.push_back(theVtxNtracks);
	theJetVtxMass.push_back(theVtxMass);
	theJetVtx3DVal.push_back(theVtx3DVal);
	theJetVtx3DSig.push_back(theVtx3DSig);
      }
      
      thePileupJetId = -std::numeric_limits<double>::max();
      thePileupJetId = (double)ijet->userFloat("pileupJetId:fullDiscriminant");
      theJetPileupJetId.push_back(thePileupJetId);

      theJetPt     . push_back(ijet->pt());
      theJetEta    . push_back(ijet->eta());
      theJetPhi    . push_back(ijet->phi());
      theJetEnergy . push_back(ijet->energy());
      
      theJetCSV.push_back(ijet->bDiscriminator( bDiscriminant ));
      theJetBTag.push_back(selector->isJetTagged(*ijet, event, true, 0));
      theJetBTag_bSFup.push_back(selector->isJetTagged(*ijet, event, true, 1));
      theJetBTag_bSFdn.push_back(selector->isJetTagged(*ijet, event, true, 2));
      theJetBTag_lSFup.push_back(selector->isJetTagged(*ijet, event, true, 3));
      theJetBTag_lSFdn.push_back(selector->isJetTagged(*ijet, event, true, 4));
      theJetPFlav.push_back(abs(ijet->partonFlavour()));
      theJetHFlav.push_back(abs(ijet->hadronFlavour()));

      theJetCEmEnergy.push_back(ijet->chargedEmEnergy());
      theJetNEmEnergy.push_back(ijet->neutralEmEnergy());
      theJetCEmEFrac.push_back(ijet->chargedEmEnergyFraction());
      theJetNEmEFrac.push_back(ijet->neutralEmEnergyFraction());	    

      theJetCHadEnergy.push_back(ijet->chargedHadronEnergy());
      theJetNHadEnergy.push_back(ijet->neutralHadronEnergy());
      theJetCHadEFrac.push_back(ijet->chargedHadronEnergyFraction());
      theJetNHadEFrac.push_back(ijet->neutralHadronEnergyFraction());      
      theJetMuonEFrac.push_back(ijet->muonEnergyFraction());

      theJetIndex.push_back(index);
      theJetnDaughters.push_back((int)ijet->numberOfDaughters());
      
      //HT
      theJetHT += ijet->pt(); 

    }
    
    double leading_pt = -999.;
    for (std::vector<pat::Jet>::const_iterator ijet = theJets.begin(); ijet != theJets.end(); ijet++) { 

      if(killHF && fabs(ijet->eta()) > 2.4) continue;
      if (ijet->pt() > leading_pt){leading_pt = ijet->pt();}
    }

    double second_leading_pt =-999.;
    for (std::vector<pat::Jet>::const_iterator ijet = theJets.begin(); ijet != theJets.end(); ijet++) { 

      if(killHF && fabs(ijet->eta()) > 2.4) continue;
      if(ijet->pt() > second_leading_pt && ijet->pt() < leading_pt){
	second_leading_pt = ijet->pt();
      }
    }

    SetValue("theJetPt",     theJetPt);
    SetValue("theJetEta",    theJetEta);
    SetValue("theJetPhi",    theJetPhi);
    SetValue("theJetEnergy", theJetEnergy);
    SetValue("theJetCSV",    theJetCSV);
    SetValue("theJetPFlav",  theJetPFlav);
    SetValue("theJetHFlav",  theJetHFlav);
    SetValue("theJetBTag",   theJetBTag);
    SetValue("theJetBTag_bSFup",   theJetBTag_bSFup);
    SetValue("theJetBTag_bSFdn",   theJetBTag_bSFdn);
    SetValue("theJetBTag_lSFup",   theJetBTag_lSFup);
    SetValue("theJetBTag_lSFdn",   theJetBTag_lSFdn);

    SetValue("theJetCEmEnergy", theJetCEmEnergy); 
    SetValue("theJetNEmEnergy", theJetNEmEnergy); 
    SetValue("theJetCEmEFrac",  theJetCEmEFrac);  
    SetValue("theJetNEmEFrac",  theJetNEmEFrac);  
    SetValue("theJetCHadEnergy",theJetCHadEnergy);
    SetValue("theJetNHadEnergy",theJetNHadEnergy);
    SetValue("theJetCHadEFrac", theJetCHadEFrac); 
    SetValue("theJetNHadEFrac", theJetNHadEFrac); 
    SetValue("theJetMuonEFrac", theJetMuonEFrac);

    SetValue("theJetHT", theJetHT);    
    SetValue("theJetLeadPt", leading_pt);
    SetValue("theJetSubLeadPt", second_leading_pt);

    SetValue("theJetVtxMass",     theJetVtxMass);
    SetValue("theJetVtxNtracks",  theJetVtxNtracks);
    SetValue("theJetVtx3DVal",    theJetVtx3DVal);
    SetValue("theJetVtx3DSig",    theJetVtx3DSig);
    SetValue("theJetPileupJetId", theJetPileupJetId);
    SetValue("theJetnDaughters", theJetnDaughters);

    // Load in AK8 jets (no selection performed on these)
    edm::Handle<std::vector<pat::Jet> > theAK8Jets;
    event.getByLabel(slimmedJetsAK8Coll_it, theAK8Jets);

    // Four std::vector
    std::vector<double> theJetAK8Pt;
    std::vector<double> theJetAK8Eta;
    std::vector<double> theJetAK8Phi;
    std::vector<double> theJetAK8Energy;
    std::vector<double> theJetAK8CSV;
    std::vector<double> theJetAK8JetCharge;
    std::vector<double> theJetAK8GenPt;
    std::vector<double> theJetAK8GenDR;

    std::vector<double> theJetAK8CEmEnergy;
    std::vector<double> theJetAK8NEmEnergy;
    std::vector<double> theJetAK8CEmEFrac;
    std::vector<double> theJetAK8NEmEFrac;
    std::vector<double> theJetAK8CHadEnergy;
    std::vector<double> theJetAK8NHadEnergy;
    std::vector<double> theJetAK8CHadEFrac;
    std::vector<double> theJetAK8NHadEFrac;
    
    // Pruned, trimmed and filtered masses available
    std::vector<double> theJetAK8PrunedMass;
    std::vector<double> theJetAK8PrunedMassWtagUncerts;
    std::vector<double> theJetAK8PrunedMassWtagUncerts_JMSup;
    std::vector<double> theJetAK8PrunedMassWtagUncerts_JMSdn;
    std::vector<double> theJetAK8PrunedMassWtagUncerts_JMRup;
    std::vector<double> theJetAK8PrunedMassWtagUncerts_JMRdn;
    std::vector<double> theJetAK8SoftDropMass;
    std::vector<double> theJetAK8SoftDropMassWtagUncerts;
    std::vector<double> theJetAK8SoftDropMassWtagUncerts_JMSup;
    std::vector<double> theJetAK8SoftDropMassWtagUncerts_JMSdn;
    std::vector<double> theJetAK8SoftDropMassWtagUncerts_JMRup;
    std::vector<double> theJetAK8SoftDropMassWtagUncerts_JMRdn;
    
    // n-subjettiness variables tau1, tau2, and tau3 available
    std::vector<double> theJetAK8NjettinessTau1;
    std::vector<double> theJetAK8NjettinessTau2;
    std::vector<double> theJetAK8NjettinessTau3;
    std::vector<bool>   theJetAK8PrunedTau21Tag;

    std::vector<double> theJetAK8Mass;
    std::vector<int>    theJetAK8Index;
    std::vector<int>    theJetAK8nDaughters;
    
    std::vector<double> theJetAK8SDSubjetPt;
    std::vector<double> theJetAK8SDSubjetEta;
    std::vector<double> theJetAK8SDSubjetPhi;
    std::vector<double> theJetAK8SDSubjetMass;
    std::vector<double> theJetAK8SDSubjetCSV;
    std::vector<double> theJetAK8SDSubjetBTag;
    std::vector<double> theJetAK8SDSubjetDR;
    std::vector<int> theJetAK8SDSubjetIndex;
    std::vector<int> theJetAK8SDSubjetSize;
    std::vector<int> theJetAK8SDSubjetNCSVL;
    std::vector<int> theJetAK8SDSubjetNCSVMSF;
    std::vector<int> theJetAK8SDSubjetNCSVM_lSFup;
    std::vector<int> theJetAK8SDSubjetNCSVM_lSFdn;
    std::vector<int> theJetAK8SDSubjetNCSVM_bSFup;
    std::vector<int> theJetAK8SDSubjetNCSVM_bSFdn;
    
    double topMass, minMass, jetCharge;
    int nSubJets;
    double thePrunedMass,theSoftDropMass;
    //double  theTrimmedMass, theFilteredMass;// - not currently in 80x miniAOD set to dummy
    double theNjettinessTau1, theNjettinessTau2, theNjettinessTau3;

    double SDsubjetPt;
    double SDsubjetEta; 
    double SDsubjetPhi;      
    double SDsubjetMass;      
    double SDsubjetBdisc;     
    double SDsubjetBTag;     
    double SDdeltaRsubjetJet; 

    int nSDSubJets;
    int nSDSubsCSVL;
    int nSDSubsCSVM;
    int nSDSubsCSVMSF;
    int nSDSubsCSVM_bSFup;
    int nSDSubsCSVM_bSFdn;
    int nSDSubsCSVM_lSFup;
    int nSDSubsCSVM_lSFdn;
    int SDSubJetIndex;

    for (std::vector<pat::Jet>::const_iterator ijet = theAK8Jets->begin(); ijet != theAK8Jets->end(); ijet++) {
      int index = (int)(ijet-theAK8Jets->begin());

      pat::Jet rawJet = ijet->correctedJet(0);
      bool looseJetID = false;
      if(abs(rawJet.eta()) <= 2.7){
	looseJetID = (rawJet.neutralHadronEnergyFraction() < 0.99 && 
		      rawJet.neutralEmEnergyFraction() < 0.99 && 
		      (rawJet.chargedMultiplicity()+rawJet.neutralMultiplicity()) > 1) && 
	  ((abs(rawJet.eta()) <= 2.4 && 
	    rawJet.chargedHadronEnergyFraction() > 0 && 
	    rawJet.chargedEmEnergyFraction() < 0.99 && 
	    rawJet.chargedMultiplicity() > 0) || 
	   abs(rawJet.eta()) > 2.4);
      }else if(abs(rawJet.eta()) <= 3.0){
	looseJetID = rawJet.neutralEmEnergyFraction() > 0.01 && rawJet.neutralHadronEnergyFraction() < 0.98 && rawJet.neutralMultiplicity() > 2;
      }else{
	looseJetID = rawJet.neutralEmEnergyFraction() < 0.9 && rawJet.neutralMultiplicity() > 10;
      }
      if(!looseJetID) continue;	

      pat::Jet corrak8;
      if(doNewJEC){
	corrak8 = selector->correctJetReturnPatJet(*ijet, event, true);
      }else{
	corrak8 = *ijet;
      }

      if(killHF && fabs(corrak8.eta()) > 2.4) continue;

      theJetAK8Pt    .push_back(corrak8.pt());
      theJetAK8Eta   .push_back(corrak8.eta());
      theJetAK8Phi   .push_back(corrak8.phi());
      theJetAK8Energy.push_back(corrak8.energy());
      theJetAK8Mass  .push_back(corrak8.mass());

      double genDR = -99;
      double genpt = -99;
      TLorentzVector ak8jet;
      ak8jet.SetPtEtaPhiE(corrak8.pt(),corrak8.eta(),corrak8.phi(),corrak8.energy());
      const reco::GenJet * genJet = corrak8.genJet();
      if(genJet){
	TLorentzVector genP4;
	genP4.SetPtEtaPhiE(genJet->pt(),genJet->eta(),genJet->phi(),genJet->energy());
	genDR = ak8jet.DeltaR(genP4);	
	genpt = genJet->pt();
      }
      theJetAK8GenPt.push_back(genpt);
      theJetAK8GenDR.push_back(genDR);

      thePrunedMass   = -std::numeric_limits<double>::max();
      theSoftDropMass = -std::numeric_limits<double>::max();

      double unc = 1.0;
      double unc_pruned = 1.0;
      double unc_pruned_up = 1.0;
      double unc_pruned_dn = 1.0;
      double ptscale_pruned = 1.0;
      double ptscale_pruned_up = 1.0;
      double ptscale_pruned_dn = 1.0;
      
      if(useL2L3Mass){
	edm::Handle<double> rhoHandle;
	edm::InputTag rhoSrc_("fixedGridRhoFastjetAll", "");
	event.getByLabel(rhoSrc_, rhoHandle);
	double rho = std::max(*(rhoHandle.product()), 0.0);

	pat::Jet l2l3jet = *ijet;
	jecak8->setJetEta(l2l3jet.correctedJet(0).eta());
	jecak8->setJetPt(l2l3jet.correctedJet(0).pt());
	jecak8->setJetE(l2l3jet.correctedJet(0).energy());
	jecak8->setJetA(l2l3jet.jetArea());
	jecak8->setRho(rho);
	double corr = jecak8->getCorrection();

	l2l3jet.scaleEnergy(corr);

	// Apply JER uncertainty + W tagging uncertainty addition for pruned mass only
	if(isMc){
	  JME::JetParameters parameters;
	  parameters.setJetPt(l2l3jet.pt());
	  parameters.setJetEta(l2l3jet.eta());
	  parameters.setRho(rho);
	  double res = resolutionAK8.getResolution(parameters);
	  Variation JERcentral = Variation::NOMINAL;
	  Variation JERshifted = Variation::UP; // symmetric
	  // JetWTagging TWiki: 80X JMR for pruned mass + nsubjettiness, resolution scale factor = 1.07
	  // uncertainty is 10.3% within eta 2.5, JER #oplus 10.3% outside
	  double factor_pruned = 0.07;
	  double uncert = fabs(resolution_SF.getScaleFactor(parameters,JERcentral) - resolution_SF.getScaleFactor(parameters,JERshifted));
	  double uncert_pruned = 0.103;
	  if(fabs(l2l3jet.eta()) > 2.5) uncert_pruned = sqrt(uncert*uncert + 0.103*0.103);
	  double factor_pruned_up = factor_pruned + uncert_pruned;
	  double factor_pruned_dn = factor_pruned - uncert_pruned;

	  const reco::GenJet * genJet = l2l3jet.genJet();
	  bool smeared = false;
	  if(genJet){
	    double deltaPt = fabs(genJet->pt() - l2l3jet.pt());
	    double deltaR = reco::deltaR(genJet->p4(),l2l3jet.p4());	
	    if (deltaR < 0.4 && deltaPt <= 3*l2l3jet.pt()*res){
	      double deltapt_pruned = (l2l3jet.pt() - genJet->pt()) * factor_pruned;
	      double deltapt_pruned_up = (l2l3jet.pt() - genJet->pt()) * factor_pruned_up;
	      double deltapt_pruned_dn = (l2l3jet.pt() - genJet->pt()) * factor_pruned_dn;
	      ptscale_pruned = max(0.0, (l2l3jet.pt() + deltapt_pruned) / l2l3jet.pt());
	      ptscale_pruned_up = max(0.0, (l2l3jet.pt() + deltapt_pruned_up) / l2l3jet.pt());
	      ptscale_pruned_dn = max(0.0, (l2l3jet.pt() + deltapt_pruned_dn) / l2l3jet.pt());
	      smeared = true;
	    }
	  }
	  if (!smeared && factor_pruned>0) {
	    JERrand.SetSeed(abs(static_cast<int>(l2l3jet.phi()*1e4)));
	    ptscale_pruned = max(0.0, JERrand.Gaus(l2l3jet.pt(),sqrt(factor_pruned*(factor_pruned+2))*res*l2l3jet.pt())/l2l3jet.pt());
	  }
	  if (!smeared && factor_pruned_up>0) {
	    JERrand.SetSeed(abs(static_cast<int>(l2l3jet.phi()*1e4)));
	    ptscale_pruned_up = max(0.0, JERrand.Gaus(l2l3jet.pt(),sqrt(factor_pruned_up*(factor_pruned_up+2))*res*l2l3jet.pt())/l2l3jet.pt());
	  }
	  if (!smeared && factor_pruned_dn>0) {
	    JERrand.SetSeed(abs(static_cast<int>(l2l3jet.phi()*1e4)));
	    ptscale_pruned_dn = max(0.0, JERrand.Gaus(l2l3jet.pt(),sqrt(factor_pruned_dn*(factor_pruned_dn+2))*res*l2l3jet.pt())/l2l3jet.pt());
	  }

	  // JetWtagging TWiki: JMS scale factor = 1, uncertainty = sqrt(JESunc^2 + 0.02^2)
	  jecUnc->setJetEta(l2l3jet.eta());
	  jecUnc->setJetPt(l2l3jet.pt());
	  unc = jecUnc->getUncertainty(true);
	  unc_pruned_up = 1.0 + sqrt(unc*unc + 0.02*0.02);

	  jecUnc->setJetEta(l2l3jet.eta());
	  jecUnc->setJetPt(l2l3jet.pt());
	  unc = jecUnc->getUncertainty(false);
	  unc_pruned_dn = 1.0 - sqrt(unc*unc + 0.02*0.02);
	}

	if(corr == 1.0) cout << "L2+L3 correction is 1.0" << endl;
	
	thePrunedMass   = corr*(double)l2l3jet.userFloat("ak8PFJetsCHSPrunedMass");
	theSoftDropMass = corr*(double)l2l3jet.userFloat("ak8PFJetsCHSSoftDropMass");	
      }else{
	thePrunedMass   = (double)corrak8.userFloat("ak8PFJetsCHSPrunedMass");
	theSoftDropMass = (double)corrak8.userFloat("ak8PFJetsCHSSoftDropMass");
      }

      theNjettinessTau1 = -std::numeric_limits<double>::max();
      theNjettinessTau2 = -std::numeric_limits<double>::max();
      theNjettinessTau3 = -std::numeric_limits<double>::max();
      theNjettinessTau1 = (double)corrak8.userFloat("NjettinessAK8:tau1");
      theNjettinessTau2 = (double)corrak8.userFloat("NjettinessAK8:tau2");
      theNjettinessTau3 = (double)corrak8.userFloat("NjettinessAK8:tau3");

      double theNjettinessTau21 = 99;
      double theNjettinessTau32 = 99;
      if(theNjettinessTau1!=2) theNjettinessTau21 = theNjettinessTau2/theNjettinessTau1;
      if(theNjettinessTau2!=2) theNjettinessTau32 = theNjettinessTau3/theNjettinessTau2;

      bool PruningTau21Tagged = false;
      if(corrak8.pt() > 200 && theNjettinessTau21 < 0.55 && thePrunedMass > 65 && thePrunedMass < 105){
	PruningTau21Tagged = true;
      }
      
      theJetAK8CSV.push_back(corrak8.bDiscriminator( bDiscriminant ));

      theJetAK8CEmEnergy.push_back(corrak8.chargedEmEnergy());
      theJetAK8NEmEnergy.push_back(corrak8.neutralEmEnergy());
      theJetAK8CEmEFrac.push_back(corrak8.chargedEmEnergyFraction());
      theJetAK8NEmEFrac.push_back(corrak8.neutralEmEnergyFraction());	    

      theJetAK8CHadEnergy.push_back(corrak8.chargedHadronEnergy());
      theJetAK8NHadEnergy.push_back(corrak8.neutralHadronEnergy());
      theJetAK8CHadEFrac.push_back(corrak8.chargedHadronEnergyFraction());
      theJetAK8NHadEFrac.push_back(corrak8.neutralHadronEnergyFraction());
      
      theJetAK8PrunedMass  .push_back(thePrunedMass); // JEC only
      theJetAK8SoftDropMass.push_back(theSoftDropMass); // JEC only
      theJetAK8PrunedMassWtagUncerts.push_back(thePrunedMass*ptscale_pruned); // JEC+JMR
      theJetAK8PrunedMassWtagUncerts_JMSup.push_back(thePrunedMass*ptscale_pruned*unc_pruned_up);
      theJetAK8PrunedMassWtagUncerts_JMSdn.push_back(thePrunedMass*ptscale_pruned*unc_pruned_dn);
      theJetAK8PrunedMassWtagUncerts_JMRup.push_back(thePrunedMass*ptscale_pruned_up*unc_pruned);
      theJetAK8PrunedMassWtagUncerts_JMRdn.push_back(thePrunedMass*ptscale_pruned_dn*unc_pruned);
      theJetAK8SoftDropMassWtagUncerts.push_back(theSoftDropMass*ptscale_pruned); // JEC+JMR
      theJetAK8SoftDropMassWtagUncerts_JMSup.push_back(theSoftDropMass*ptscale_pruned*unc_pruned_up);
      theJetAK8SoftDropMassWtagUncerts_JMSdn.push_back(theSoftDropMass*ptscale_pruned*unc_pruned_dn);
      theJetAK8SoftDropMassWtagUncerts_JMRup.push_back(theSoftDropMass*ptscale_pruned_up*unc_pruned);
      theJetAK8SoftDropMassWtagUncerts_JMRdn.push_back(theSoftDropMass*ptscale_pruned_dn*unc_pruned);
      
      theJetAK8NjettinessTau1.push_back(theNjettinessTau1);
      theJetAK8NjettinessTau2.push_back(theNjettinessTau2);
      theJetAK8NjettinessTau3.push_back(theNjettinessTau3);
      theJetAK8PrunedTau21Tag.push_back(PruningTau21Tagged);

      theJetAK8nDaughters.push_back((int)corrak8.numberOfDaughters());
      theJetAK8Index.push_back(index);

      //JetCharge calculation

      reco::Jet::Constituents constituents = corrak8.getJetConstituents();

      double sumWeightedCharge = 0.0;
      int con_charge = 0;
      double con_pt = 0.0;
      for(auto constituentItr=constituents.begin(); constituentItr!=constituents.end(); ++constituentItr){
	edm::Ptr<reco::Candidate> constituent=*constituentItr;

	con_charge = (int)constituent->charge();
	con_pt     = (double)constituent->pt();
	
	sumWeightedCharge = sumWeightedCharge + ( con_charge * pow(con_pt,kappa) );
	
      }

      jetCharge  = 1.0/( pow( (corrak8.pt()), kappa) ) * sumWeightedCharge;

      theJetAK8JetCharge.push_back(jetCharge);

      // Get Soft drop subjets for subjet b-tagging
      SDSubJetIndex = (int)theJetAK8SDSubjetPt.size();
      nSDSubJets  = std::numeric_limits<int>::min();
      nSDSubsCSVL = 0;
      nSDSubsCSVMSF = 0;
      nSDSubsCSVM_bSFup = 0;
      nSDSubsCSVM_bSFdn = 0;
      nSDSubsCSVM_lSFup = 0;
      nSDSubsCSVM_lSFdn = 0;
      double maxSubCSV = 0;

      auto const & sdSubjets = corrak8.subjets("SoftDrop");
      nSDSubJets = (int)sdSubjets.size();
      for ( auto const & it : sdSubjets ) {

	pat::Jet corrsubjet;
	if(doNewJEC){
	  corrsubjet = selector->correctJetReturnPatJet(*it, event, false);
	}else{
	  corrsubjet = *it;
	}

	SDsubjetPt        = -std::numeric_limits<double>::max();
	SDsubjetEta       = -std::numeric_limits<double>::max();
	SDsubjetPhi       = -std::numeric_limits<double>::max();
	SDsubjetMass      = -std::numeric_limits<double>::max();
	SDsubjetBdisc     = -std::numeric_limits<double>::max();
	SDdeltaRsubjetJet = std::numeric_limits<double>::max();
      
	SDsubjetPt        = corrsubjet.pt();
	SDsubjetEta       = corrsubjet.eta();
	SDsubjetPhi       = corrsubjet.phi();
	SDsubjetMass      = corrsubjet.mass();
	SDsubjetBdisc     = corrsubjet.bDiscriminator(bDiscriminant); 
	SDsubjetBTag      = selector->isJetTagged(corrsubjet, event, true, 0, true);
	SDdeltaRsubjetJet = deltaR(corrak8.eta(), corrak8.phi(), SDsubjetEta, SDsubjetPhi);

	if(SDsubjetBdisc > 0.460) nSDSubsCSVL++;
	if(SDsubjetBTag > 0) nSDSubsCSVMSF++;
	if(selector->isJetTagged(corrsubjet, event, true, 1, true) > 0) nSDSubsCSVM_bSFup++;
	if(selector->isJetTagged(corrsubjet, event, true, 2, true) > 0) nSDSubsCSVM_bSFdn++;
	if(selector->isJetTagged(corrsubjet, event, true, 3, true) > 0) nSDSubsCSVM_lSFup++;
	if(selector->isJetTagged(corrsubjet, event, true, 4, true) > 0) nSDSubsCSVM_lSFdn++;

	theJetAK8SDSubjetPt.push_back(SDsubjetPt);
	theJetAK8SDSubjetEta.push_back(SDsubjetEta);
	theJetAK8SDSubjetPhi.push_back(SDsubjetPhi);
	theJetAK8SDSubjetMass.push_back(SDsubjetMass);
	theJetAK8SDSubjetCSV.push_back(SDsubjetBdisc);
	theJetAK8SDSubjetBTag.push_back(SDsubjetBTag);
	theJetAK8SDSubjetDR.push_back(SDdeltaRsubjetJet);
      }

      theJetAK8SDSubjetIndex.push_back(SDSubJetIndex);
      theJetAK8SDSubjetSize.push_back(nSDSubJets);
      theJetAK8SDSubjetNCSVL.push_back(nSDSubsCSVL);
      theJetAK8SDSubjetNCSVMSF.push_back(nSDSubsCSVMSF);
      theJetAK8SDSubjetNCSVM_bSFup.push_back(nSDSubsCSVM_bSFup);
      theJetAK8SDSubjetNCSVM_bSFdn.push_back(nSDSubsCSVM_bSFdn);
      theJetAK8SDSubjetNCSVM_lSFup.push_back(nSDSubsCSVM_lSFup);
      theJetAK8SDSubjetNCSVM_lSFdn.push_back(nSDSubsCSVM_lSFdn);

    }

    SetValue("theJetAK8Pt",     theJetAK8Pt);
    SetValue("theJetAK8Eta",    theJetAK8Eta);
    SetValue("theJetAK8Phi",    theJetAK8Phi);
    SetValue("theJetAK8Energy", theJetAK8Energy);
    SetValue("theJetAK8CSV",    theJetAK8CSV);
    SetValue("theJetAK8JetCharge", theJetAK8JetCharge);
    SetValue("theJetAK8GenPt",  theJetAK8GenPt);
    SetValue("theJetAK8GenDR",  theJetAK8GenDR);

    SetValue("theJetAK8CEmEnergy", theJetAK8CEmEnergy); 
    SetValue("theJetAK8NEmEnergy", theJetAK8NEmEnergy); 
    SetValue("theJetAK8CEmEFrac",  theJetAK8CEmEFrac);  
    SetValue("theJetAK8NEmEFrac",  theJetAK8NEmEFrac);  
    SetValue("theJetAK8CHadEnergy",theJetAK8CHadEnergy);
    SetValue("theJetAK8NHadEnergy",theJetAK8NHadEnergy);
    SetValue("theJetAK8CHadEFrac", theJetAK8CHadEFrac); 
    SetValue("theJetAK8NHadEFrac", theJetAK8NHadEFrac); 

    SetValue("theJetAK8PrunedMass",   theJetAK8PrunedMass);
    SetValue("theJetAK8PrunedMassWtagUncerts",   theJetAK8PrunedMassWtagUncerts);
    SetValue("theJetAK8PrunedMassWtagUncerts_JMSup",   theJetAK8PrunedMassWtagUncerts_JMSup);
    SetValue("theJetAK8PrunedMassWtagUncerts_JMSdn",   theJetAK8PrunedMassWtagUncerts_JMSdn);
    SetValue("theJetAK8PrunedMassWtagUncerts_JMRup",   theJetAK8PrunedMassWtagUncerts_JMRup);
    SetValue("theJetAK8PrunedMassWtagUncerts_JMRdn",   theJetAK8PrunedMassWtagUncerts_JMRdn);
    SetValue("theJetAK8SoftDropMass", theJetAK8SoftDropMass);
    SetValue("theJetAK8SoftDropMassWtagUncerts",   theJetAK8SoftDropMassWtagUncerts);
    SetValue("theJetAK8SoftDropMassWtagUncerts_JMSup",   theJetAK8SoftDropMassWtagUncerts_JMSup);
    SetValue("theJetAK8SoftDropMassWtagUncerts_JMSdn",   theJetAK8SoftDropMassWtagUncerts_JMSdn);
    SetValue("theJetAK8SoftDropMassWtagUncerts_JMRup",   theJetAK8SoftDropMassWtagUncerts_JMRup);
    SetValue("theJetAK8SoftDropMassWtagUncerts_JMRdn",   theJetAK8SoftDropMassWtagUncerts_JMRdn);
    
    SetValue("theJetAK8NjettinessTau1", theJetAK8NjettinessTau1);
    SetValue("theJetAK8NjettinessTau2", theJetAK8NjettinessTau2);
    SetValue("theJetAK8NjettinessTau3", theJetAK8NjettinessTau3);
    SetValue("theJetAK8PrunedTau21Tag", theJetAK8PrunedTau21Tag);

    SetValue("theJetAK8Mass",   theJetAK8Mass);
    SetValue("theJetAK8nDaughters", theJetAK8nDaughters);

    SetValue("theJetAK8SDSubjetPt",   theJetAK8SDSubjetPt);   
    SetValue("theJetAK8SDSubjetEta",  theJetAK8SDSubjetEta);  
    SetValue("theJetAK8SDSubjetPhi",  theJetAK8SDSubjetPhi);  
    SetValue("theJetAK8SDSubjetMass", theJetAK8SDSubjetMass); 
    SetValue("theJetAK8SDSubjetCSV",  theJetAK8SDSubjetCSV);  
    SetValue("theJetAK8SDSubjetBTag",  theJetAK8SDSubjetBTag);  
    SetValue("theJetAK8SDSubjetDR",   theJetAK8SDSubjetDR);   
    SetValue("theJetAK8SDSubjetIndex",theJetAK8SDSubjetIndex);
    SetValue("theJetAK8SDSubjetSize", theJetAK8SDSubjetSize); 
    SetValue("theJetAK8SDSubjetNCSVL",theJetAK8SDSubjetNCSVL);
    SetValue("theJetAK8SDSubjetNCSVMSF",theJetAK8SDSubjetNCSVMSF);
    SetValue("theJetAK8SDSubjetNCSVM_bSFup",theJetAK8SDSubjetNCSVM_bSFup);
    SetValue("theJetAK8SDSubjetNCSVM_bSFdn",theJetAK8SDSubjetNCSVM_bSFdn);
    SetValue("theJetAK8SDSubjetNCSVM_lSFup",theJetAK8SDSubjetNCSVM_lSFup);
    SetValue("theJetAK8SDSubjetNCSVM_lSFdn",theJetAK8SDSubjetNCSVM_lSFdn);


    //////////////// TRUE HADRONIC W/Z/H/Top decays //////////////////
    std::vector<int>    HadronicVHtID;
    std::vector<int>    HadronicVHtStatus;
    std::vector<double> HadronicVHtPt;
    std::vector<double> HadronicVHtEta;
    std::vector<double> HadronicVHtPhi;
    std::vector<double> HadronicVHtEnergy;
    std::vector<double> HadronicVHtD0Pt;
    std::vector<double> HadronicVHtD0Eta;
    std::vector<double> HadronicVHtD0Phi;
    std::vector<double> HadronicVHtD0E;
    std::vector<double> HadronicVHtD1Pt;
    std::vector<double> HadronicVHtD1Eta;
    std::vector<double> HadronicVHtD1Phi;
    std::vector<double> HadronicVHtD1E;
    std::vector<double> HadronicVHtD2Pt;
    std::vector<double> HadronicVHtD2Eta;
    std::vector<double> HadronicVHtD2Phi;
    std::vector<double> HadronicVHtD2E;
  
    // Get the generated particle collection
    edm::Handle<reco::GenParticleCollection> genParticles;
    if(isMc && event.getByLabel(genParticles_it, genParticles)){
      
      for(size_t i = 0; i < genParticles->size(); i++){
	const reco::GenParticle &p = (*genParticles).at(i);
	int id = p.pdgId();

	bool hasRadiation = false;
	bool hasLepton = false;
	  
	if(abs(id) == 23 || abs(id) == 24 || abs(id) == 25 || abs(id) == 6){
	    
	  size_t nDs = p.numberOfDaughters();
	  for(size_t j = 0; j < nDs; j++){
	    int dauId = (p.daughter(j))->pdgId();
	    const reco::Candidate *d = p.daughter(j);
	    if(d->pdgId() != dauId) std::cout << "making daughter GenParticle didn't work" << std::endl;
	    
	    if(abs(dauId) == abs(id)) hasRadiation = true;
	    else if(abs(dauId) == 24){  // check t->Wb->leptons and H->WW->leptons
	      while(d->numberOfDaughters() == 1) d = d->daughter(0);
	      if(abs(d->daughter(0)->pdgId()) > 10 && abs(d->daughter(0)->pdgId()) < 17) hasLepton = true;
	      if(abs(d->daughter(1)->pdgId()) > 10 && abs(d->daughter(1)->pdgId()) < 17) hasLepton = true;
	    }
	    else if(abs(dauId) == 23){  // check H->ZZ->leptons
	      while(d->numberOfDaughters() == 1) d = d->daughter(0);
	      if(abs(d->daughter(0)->pdgId()) > 10 && abs(d->daughter(0)->pdgId()) < 17) hasLepton = true;
	      if(abs(d->daughter(1)->pdgId()) > 10 && abs(d->daughter(1)->pdgId()) < 17) hasLepton = true;
	    }
	    else if(abs(dauId) > 10 && abs(dauId) < 17) hasLepton = true;	    
	    
	  }
	  
	  if(hasRadiation) continue;	  
	  if(hasLepton) continue;	  
	  if(p.pt() < 175) continue;
	  
	  if(abs(id) == 24){
	    double dRWb = 1000;
	    double dRWW = 1000;
	    
	    const reco::Candidate *mother = p.mother();
	    while(abs(mother->pdgId()) == 24) mother = mother->mother();
	    
	    if(abs(mother->pdgId()) == 6){
	      double dr = reco::deltaR(p.p4(),mother->daughter(1)->p4());
	      if(abs(mother->daughter(1)->pdgId()) == 24) dr = reco::deltaR(p.p4(),mother->daughter(0)->p4());
	      if(dr < dRWb) dRWb = dr;
	    }else if(abs(mother->pdgId()) == 25){
	      double dr = 1000;
	      if(p.pdgId()*mother->daughter(0)->pdgId() > 0){
		dr = reco::deltaR(p.p4(),mother->daughter(1)->p4());
	      }else{
		dr = reco::deltaR(p.p4(),mother->daughter(0)->p4());
	      }
	      if(dr < dRWW) dRWW = dr;
	    }		
	    
	    if(dRWW < 0.8) continue; // W from merged H
	    if(dRWb < 0.8) continue; // W from merged t
	  }
	  
	  if(abs(id) == 23){
	    double dRZZ = 1000;
	    
	    const reco::Candidate *mother = p.mother();
	    while(abs(mother->pdgId()) == 23) mother = mother->mother();
	    
	    if(abs(mother->pdgId()) == 25){
	      double dr = 1000;
	      if(p.pdgId()*mother->daughter(0)->pdgId() > 0){
		dr = reco::deltaR(p.p4(),mother->daughter(1)->p4());
	      }else{
		dr = reco::deltaR(p.p4(),mother->daughter(0)->p4());
	      }
	      if(dr < dRZZ) dRZZ = dr;
	    }
	    
	    if(dRZZ < 0.8) continue; // Z from merged H
	  }
	  
	  if(p.numberOfDaughters() < 2){
	    std::cout << p.numberOfDaughters() << " daughters from " << p.pdgId() << std::endl;
	    continue;
	  }

	  HadronicVHtStatus.push_back( p.status() );
	  HadronicVHtID.push_back( p.pdgId() );
	  HadronicVHtPt.push_back( p.pt() );
	  HadronicVHtEta.push_back( p.eta() );
	  HadronicVHtPhi.push_back( p.phi() );
	  HadronicVHtEnergy.push_back( p.energy() );
	  
	  if(abs(id) != 6){
	    HadronicVHtD0Pt.push_back( p.daughter(0)->pt());
	    HadronicVHtD0Eta.push_back( p.daughter(0)->eta());
	    HadronicVHtD0Phi.push_back( p.daughter(0)->phi());
	    HadronicVHtD0E.push_back( p.daughter(0)->energy());
	    HadronicVHtD1Pt.push_back( p.daughter(1)->pt());	    
	    HadronicVHtD1Eta.push_back( p.daughter(1)->eta());
	    HadronicVHtD1Phi.push_back( p.daughter(1)->phi());
	    HadronicVHtD1E.push_back( p.daughter(1)->energy());
	    HadronicVHtD2Pt.push_back(-99.9);
	    HadronicVHtD2Eta.push_back(-99.9);
	    HadronicVHtD2Phi.push_back(-99.9);
	    HadronicVHtD2E.push_back(-99.9);
	  }else{
	    const reco::Candidate *W = p.daughter(0);
	    const reco::Candidate *b = p.daughter(1);
	    if(fabs(W->pdgId()) != 24){
	      W = p.daughter(1);
	      b = p.daughter(0);
	    }
	    while(W->numberOfDaughters() == 1) W = W->daughter(0);
	    HadronicVHtD0Pt.push_back( b->pt());
	    HadronicVHtD0Eta.push_back( b->eta());
	    HadronicVHtD0Phi.push_back( b->phi());
	    HadronicVHtD0E.push_back( b->energy());
	    HadronicVHtD1Pt.push_back( W->daughter(0)->pt());
	    HadronicVHtD2Pt.push_back( W->daughter(1)->pt());
	    HadronicVHtD1Eta.push_back( W->daughter(0)->eta());
	    HadronicVHtD2Eta.push_back( W->daughter(1)->eta());
	    HadronicVHtD1Phi.push_back( W->daughter(0)->phi());
	    HadronicVHtD2Phi.push_back( W->daughter(1)->phi());
	    HadronicVHtD1E.push_back( W->daughter(0)->energy());
	    HadronicVHtD2E.push_back( W->daughter(1)->energy());
	  }
	}
      }
    }

    SetValue("HadronicVHtStatus",HadronicVHtStatus);
    SetValue("HadronicVHtID",HadronicVHtID);
    SetValue("HadronicVHtPt",HadronicVHtPt);
    SetValue("HadronicVHtEta",HadronicVHtEta);
    SetValue("HadronicVHtPhi",HadronicVHtPhi);
    SetValue("HadronicVHtEnergy",HadronicVHtEnergy);
    SetValue("HadronicVHtD0Pt",HadronicVHtD0Pt);
    SetValue("HadronicVHtD0Eta",HadronicVHtD0Eta);
    SetValue("HadronicVHtD0Phi",HadronicVHtD0Phi);
    SetValue("HadronicVHtD0E",HadronicVHtD0E);
    SetValue("HadronicVHtD1Pt",HadronicVHtD1Pt);
    SetValue("HadronicVHtD1Eta",HadronicVHtD1Eta);
    SetValue("HadronicVHtD1Phi",HadronicVHtD1Phi);
    SetValue("HadronicVHtD1E",HadronicVHtD1E);
    SetValue("HadronicVHtD2Pt",HadronicVHtD2Pt);
    SetValue("HadronicVHtD2Eta",HadronicVHtD2Eta);
    SetValue("HadronicVHtD2Phi",HadronicVHtD2Phi);
    SetValue("HadronicVHtD2E",HadronicVHtD2E);

    if(useHTT){

      ///////////////////////////////////////////////////////////////////////
      ///  
      ///   NOTE: You must install HTT to load the HTTTopJetTagInfo object!
      ///   In your CMSSW_X_X_X/src/ directory:
      ///   git cms-merge-topic gkasieczka:htt-v2-74X
      ///   scramv1 b -r -j 8
      ///   
      ///////////////////////////////////////////////////////////////////////
    
      std::vector<double> theJetCA15Pt;
      std::vector<double> theJetCA15Eta;
      std::vector<double> theJetCA15Phi;
      std::vector<double> theJetCA15Energy;
      std::vector<double> theJetCA15Mass;
      std::vector<double> theJetCA15SoftDropMass;
      std::vector<double> theJetCA15NjettinessTau1;
      std::vector<double> theJetCA15NjettinessTau2;
      std::vector<double> theJetCA15NjettinessTau3;
      
      std::vector<double> theJetHTTTopMass;
      std::vector<double> theJetHTTfRec;
      std::vector<double> theJetHTTRopt;
      std::vector<double> theJetHTTRoptCalc;
      std::vector<double> theJetHTTRoptCalcPt;
      std::vector<double> theJetHTTNjettinessTau1;
      std::vector<double> theJetHTTNjettinessTau2;
      std::vector<double> theJetHTTNjettinessTau3;
      std::vector<double> theJetHTTufNjettinessTau1;
      std::vector<double> theJetHTTufNjettinessTau2;
      std::vector<double> theJetHTTufNjettinessTau3;

      double theHTTTopMass;
      double theHTTfRec;
      double theHTTRopt;
      double theHTTRoptCalc;
      double theHTTRoptCalcPt;
      double NjettinessTau1;
      double NjettinessTau2;
      double NjettinessTau3;
      
      edm::Handle<std::vector<pat::Jet> > theCA15Jets;
      event.getByLabel(selectedPatJetsCA15Coll_it, theCA15Jets);

      edm::Handle<std::vector<reco::HTTTopJetTagInfo> > httTagInfo;
      event.getByLabel(httTagInfo_it, httTagInfo);
      for (std::vector<pat::Jet>::const_iterator ijet = theCA15Jets->begin(); ijet != theCA15Jets->end(); ijet++){

	theJetCA15Pt.push_back(ijet->pt());
	theJetCA15Eta.push_back(ijet->eta());
	theJetCA15Phi.push_back(ijet->phi());
	theJetCA15Energy.push_back(ijet->energy());
	theJetCA15Mass.push_back(ijet->mass());

	theNjettinessTau1 = -std::numeric_limits<double>::max();
	theNjettinessTau2 = -std::numeric_limits<double>::max();
	theNjettinessTau3 = -std::numeric_limits<double>::max();
	theNjettinessTau1 = (double)ijet->userFloat("NjettinessCA15CHSNoHF:tau1");
	theNjettinessTau2 = (double)ijet->userFloat("NjettinessCA15CHSNoHF:tau2");
	theNjettinessTau3 = (double)ijet->userFloat("NjettinessCA15CHSNoHF:tau3");
      
	theJetCA15NjettinessTau1.push_back(theNjettinessTau1);
	theJetCA15NjettinessTau2.push_back(theNjettinessTau2);
	theJetCA15NjettinessTau3.push_back(theNjettinessTau3);
	
	theSoftDropMass = -std::numeric_limits<double>::max();
	theSoftDropMass = (double)ijet->userFloat("ca15PFJetsCHSNoHFSoftDropMass");
	theJetCA15SoftDropMass.push_back(theSoftDropMass);
	
	TLorentzVector ca15;
	ca15.SetPtEtaPhiE(ijet->pt(),ijet->eta(),ijet->phi(),ijet->energy());
	
	double minDR = 1000;
	reco::HTTTopJetProperties CA15HTTProperties;
	for (std::vector<reco::HTTTopJetTagInfo>::const_iterator ijet = httTagInfo->begin(); ijet != httTagInfo->end(); ijet++) {
	  
	  reco::HTTTopJetTagInfo const jetInfo = (*ijet);
	  reco::HTTTopJetProperties HTTProperties = jetInfo.properties();
	  
	  TLorentzVector fj;
	  fj.SetPtEtaPhiM(HTTProperties.fjPt, HTTProperties.fjEta, HTTProperties.fjPhi, HTTProperties.fjMass);
	  
	  double DR = ca15.DeltaR(fj);
	  if(DR < minDR){
	    minDR = DR;
	    CA15HTTProperties = HTTProperties;
	  }
	}
	
 	theHTTTopMass = -99.99;
	theHTTfRec = -99.99;
	theHTTRopt = -99.99;
	theHTTRoptCalc = -99.99;
	theHTTRoptCalcPt = -99.99;
	theNjettinessTau1 = -99.99;
	theNjettinessTau2 = -99.99;
	theNjettinessTau3 = -99.99;
	NjettinessTau1 = -99.99;
	NjettinessTau2 = -99.99;
	NjettinessTau3 = -99.99;

	if(minDR < 0.5){ // should be asymptotically 0, 1.5 probably too generous
	  theHTTTopMass = CA15HTTProperties.topMass;    // mass of the top quark candidate
	  theHTTfRec = CA15HTTProperties.fRec;          // min distance of m_ij/m_123 from m_W/m_top
	  theHTTRopt = CA15HTTProperties.Ropt;          // R_opt found in Optimal procedure
	  theHTTRoptCalc = CA15HTTProperties.RoptCalc;     // R_opt calc for a top based on filtered fat jet pT
	  theHTTRoptCalcPt = CA15HTTProperties.ptForRoptCalc; // Filtered fat jet pT for R_opt calculation
	  theNjettinessTau1 = CA15HTTProperties.tau1Filtered; 
	  theNjettinessTau2 = CA15HTTProperties.tau2Filtered;
	  theNjettinessTau3 = CA15HTTProperties.tau3Filtered;
	  NjettinessTau1 = CA15HTTProperties.tau1Unfiltered;
	  NjettinessTau2 = CA15HTTProperties.tau2Unfiltered;
	  NjettinessTau3 = CA15HTTProperties.tau3Unfiltered;
	}

	theJetHTTRoptCalc.push_back(theHTTRoptCalc);
	theJetHTTRoptCalcPt.push_back(theHTTRoptCalcPt);
	theJetHTTNjettinessTau1.push_back(theNjettinessTau1);
	theJetHTTNjettinessTau2.push_back(theNjettinessTau2);
	theJetHTTNjettinessTau3.push_back(theNjettinessTau3);
	theJetHTTufNjettinessTau1.push_back(NjettinessTau1);
	theJetHTTufNjettinessTau2.push_back(NjettinessTau2);
	theJetHTTufNjettinessTau3.push_back(NjettinessTau3);
	
      }
      
      SetValue("theJetCA15Pt", theJetCA15Pt);
      SetValue("theJetCA15Eta", theJetCA15Eta);
      SetValue("theJetCA15Phi", theJetCA15Phi);
      SetValue("theJetCA15Energy", theJetCA15Energy);
      SetValue("theJetCA15Mass", theJetCA15Mass);
      SetValue("theJetCA15SoftDropMass", theJetCA15SoftDropMass);
      SetValue("theJetCA15NjettinessTau1", theJetCA15NjettinessTau1);
      SetValue("theJetCA15NjettinessTau2", theJetCA15NjettinessTau2);
      SetValue("theJetCA15NjettinessTau3", theJetCA15NjettinessTau3);
      
      SetValue("theJetHTTTopMass", theJetHTTTopMass);
      SetValue("theJetHTTfRec", theJetHTTfRec);
      SetValue("theJetHTTRopt", theJetHTTRopt);
      SetValue("theJetHTTRoptCalc", theJetHTTRoptCalc);
      SetValue("theJetHTTRoptCalcPt", theJetHTTRoptCalcPt);
      
      SetValue("theJetHTTNjettinessTau1",theJetHTTNjettinessTau1);    
      SetValue("theJetHTTNjettinessTau2",theJetHTTNjettinessTau2);
      SetValue("theJetHTTNjettinessTau3",theJetHTTNjettinessTau3);
      SetValue("theJetHTTufNjettinessTau1",theJetHTTufNjettinessTau1);    
      SetValue("theJetHTTufNjettinessTau2",theJetHTTufNjettinessTau2);
      SetValue("theJetHTTufNjettinessTau3",theJetHTTufNjettinessTau3);
    }

    return 0;
}

int JetSubCalc::EndJob()
{
  delete jecUnc;
  delete jecak8;
  delete L2JetParAK8;
  delete L3JetParAK8;
  delete ResJetParAK8;
  return 0;
}
