/*
  Calculator for the SLiTT analysis
  
  Author: Gena Kukartsev, 2012
*/



#include <iostream>
#include "LJMet/Com/interface/BaseCalc.h"
#include "LJMet/Com/interface/LjmetFactory.h"
#include "LJMet/Com/interface/LjmetEventContent.h"

#include "EgammaAnalysis/ElectronTools/interface/ElectronEffectiveArea.h"
#include "LJMet/Com/interface/TopElectronSelector.h"

#include "DataFormats/JetReco/interface/CATopJetTagInfo.h"
#include "PhysicsTools/SelectorUtils/interface/PFElectronSelector.h"
#include "PhysicsTools/SelectorUtils/interface/PFMuonSelector.h"

using std::cout;
using std::endl;

class LjmetFactory;

class DileptonCalc : public BaseCalc{
  
public:
  
  DileptonCalc();
  virtual ~DileptonCalc(){}
  
  virtual int BeginJob();
  virtual int AnalyzeEvent(edm::EventBase const & event, BaseEventSelector * selector);
  virtual int EndJob(){return 0;}

private:
  
  bool                      isMc;
  std::string               dataType;
  edm::InputTag             rhoSrc_it;
  edm::InputTag             pvCollection_it;
  edm::InputTag             genParticles_it;
  std::vector<unsigned int> keepPDGID;
  std::vector<unsigned int> keepMomPDGID;
  bool keepFullMChistory;
  
  double rhoIso;

  boost::shared_ptr<TopElectronSelector>     electronSelL_, electronSelM_, electronSelT_;
  boost::shared_ptr<PFMuonSelector>          muonSelLJ_, muonSelDil_;
  boost::shared_ptr<PFElectronSelector>      pfElectronSelLJ_, pfElectronSelDil_;

  std::vector<reco::Vertex> goodPVs;
  int findMatch(const reco::GenParticleCollection & genParticles, int idToMatch, double eta, double phi);
  double mdeltaR(double eta1, double phi1, double eta2, double phi2);
  void fillMotherInfo(const reco::Candidate *mother, int i, vector <int> & momid, vector <int> & momstatus, vector<double> & mompt, vector<double> & mometa, vector<double> & momphi, vector<double> & momenergy);
  int dataEE, dataEM, dataMM;
};


static int reg = LjmetFactory::GetInstance()->Register(new DileptonCalc(), "DileptonCalc");


DileptonCalc::DileptonCalc(){
}

int DileptonCalc::BeginJob(){
  
  if (mPset.exists("dataType"))     dataType = mPset.getParameter<std::string>("dataType");
  else                              dataType = "None";
  
  if (mPset.exists("rhoSrc"))       rhoSrc_it = mPset.getParameter<edm::InputTag>("rhoSrc");
  else                              rhoSrc_it = edm::InputTag("kt6PFJetsForIsolation", "rho", "PAT");
  
  if (mPset.exists("pvCollection")) pvCollection_it = mPset.getParameter<edm::InputTag>("pvCollection");
  else                              pvCollection_it = edm::InputTag("goodOfflinePrimaryVertices");
  
  if (mPset.exists("isMc"))         isMc = mPset.getParameter<bool>("isMc");
  else                              isMc = false;
  
  if (mPset.exists("genParticles")) genParticles_it = mPset.getParameter<edm::InputTag>("genParticles");
  else                              genParticles_it = edm::InputTag("prunedGenParticles");
  
  if (mPset.exists("keepPDGID"))    keepPDGID = mPset.getParameter<std::vector<unsigned int> >("keepPDGID");
  else                              keepPDGID.clear();
  
  if (mPset.exists("keepMomPDGID")) keepMomPDGID = mPset.getParameter<std::vector<unsigned int> >("keepMomPDGID");
  else                              keepMomPDGID.clear();

  if (mPset.exists("keepFullMChistory")) keepFullMChistory = mPset.getParameter<bool>("keepFullMChistory");
  else                                   keepFullMChistory = false;
  cout << "keepFullMChistory "     <<    keepFullMChistory << endl;
  
  if ( mPset.exists("cutbasedIDSelectorLoose")){
    electronSelL_ = boost::shared_ptr<TopElectronSelector>( 
	new TopElectronSelector(mPset.getParameter<edm::ParameterSet>("cutbasedIDSelectorLoose")) );
  }
  else {
    std::cout << "DileptonCalc: Loose electron selector not configured, exiting"
	      << std::endl;
    std::exit(-1);
  }
  if ( mPset.exists("cutbasedIDSelectorMedium")){
    electronSelM_ = boost::shared_ptr<TopElectronSelector>( 
	new TopElectronSelector(mPset.getParameter<edm::ParameterSet>("cutbasedIDSelectorMedium")) );
  }
  else {
    std::cout << "DileptonCalc: Medium electron selector not configured, exiting"
	      << std::endl;
    std::exit(-1);
  }
  if ( mPset.exists("cutbasedIDSelectorTight")){
    electronSelT_ = boost::shared_ptr<TopElectronSelector>( 
	new TopElectronSelector(mPset.getParameter<edm::ParameterSet>("cutbasedIDSelectorTight")) );
  }
  else {
    std::cout << "DileptonCalc: Tight electron selector not configured, exiting"
	      << std::endl;
    std::exit(-1);
  }
  if ( mPset.exists("mvaElectronDileptonSelectorLJ")){
    pfElectronSelLJ_ = boost::shared_ptr<PFElectronSelector>( 
    	new PFElectronSelector(mPset.getParameter<edm::ParameterSet>("mvaElectronDileptonSelectorLJ")) );
  }
  else {
    std::cout << "DileptonCalc: L+J MVA electron selector not configured, exiting"
	      << std::endl;
    std::exit(-1);
  }
  if ( mPset.exists("mvaElectronDileptonSelectorDil")){
    pfElectronSelDil_ = boost::shared_ptr<PFElectronSelector>( 
    	new PFElectronSelector(mPset.getParameter<edm::ParameterSet>("mvaElectronDileptonSelectorDil")) );
  }
  else {
    std::cout << "DileptonCalc: Dilepton MVA electron selector not configured, exiting"
	      << std::endl;
    std::exit(-1);
  }

  if ( mPset.exists("muonSelectorLJ")){
    muonSelLJ_ = boost::shared_ptr<PFMuonSelector>( 
    	new PFMuonSelector(mPset.getParameter<edm::ParameterSet>("muonSelectorLJ")) );
  }
  else {
    std::cout << "DileptonCalc: L+J muon selector not configured, exiting"
	      << std::endl;
    std::exit(-1);
  }
  if ( mPset.exists("muonSelectorDil")){
    muonSelDil_ = boost::shared_ptr<PFMuonSelector>( 
    	new PFMuonSelector(mPset.getParameter<edm::ParameterSet>("muonSelectorDil")) );
  }
  else {
    std::cout << "DileptonCalc: Dilepton muon selector not configured, exiting"
	      << std::endl;
    std::exit(-1);
  }

  dataEE = 0;
  dataEM = 0;
  dataMM = 0;
  if      (dataType == "EE" or dataType == "ElEl") dataEE = 1; 
  else if (dataType == "EM" or dataType == "ElMu") dataEM = 1;
  else if (dataType == "MM" or dataType == "MuMu") dataMM = 1;
  else if (dataType == "All" or dataType == "ALL") {
    dataEE = 1; dataEM = 1; dataMM = 1;
  }
  cout << "Data type: "<< dataType<<" "<<dataEE << dataEM << dataMM <<endl;

  return 0;
}

int DileptonCalc::AnalyzeEvent(edm::EventBase const & event,
			       BaseEventSelector * selector){
  //
  // compute event variables here
  //
  
  //
  // _____ Get objects from the selector _____________________
  //
  std::vector<edm::Ptr<pat::Muon> >     const & vSelMuons     = selector->GetSelectedMuons();
  std::vector<edm::Ptr<pat::Electron> > const & vSelElectrons = selector->GetSelectedElectrons();
  std::vector<edm::Ptr<pat::Jet> >      const & vSelJets      = selector->GetSelectedJets();
  edm::Ptr<pat::MET>                    const & pMet          = selector->GetMet();
  std::vector<unsigned int>             const & vSelTriggers  = selector->GetSelectedTriggers();
  
  //
  // _____ Primary dataset (from python cfg) _____________________
  //
  //
  
  SetValue("dataEE", dataEE);
  SetValue("dataEM", dataEM);
  SetValue("dataMM", dataMM);
  
  //
  // ____ Trigger ____________________________
  //
  int passEE = 0;
  int passEM = 0;
  int passMM = 0;
  
  if (vSelTriggers.size() == 3){    
    passEE = (int)vSelTriggers.at(0);
    passEM = (int)vSelTriggers.at(1);
    passMM = (int)vSelTriggers.at(2);
  }
  
  SetValue("trigEE", passEE);
  SetValue("trigEM", passEM);
  SetValue("trigMM", passMM);
  
  //
  //_____ Event kinematics __________________
  //
  
  //Primary vertices
  edm::Handle<std::vector<reco::Vertex> > pvHandle;
  event.getByLabel(pvCollection_it, pvHandle);
  goodPVs = *(pvHandle.product());

  SetValue("nPV", (int)goodPVs.size());
  
  //
  //_____ Electrons _________________________
  //
  
  //Four vector
  std::vector <double> elPt;
  std::vector <double> elEta;
  std::vector <double> elPhi;
  std::vector <double> elEnergy;
  
  //Quality criteria
  std::vector <double> elRelIso;
  std::vector <double> elDxy;
  std::vector <int>    elNotConversion;
  std::vector <int>    elChargeConsistent;
  std::vector <int>    elIsEBEE; 
  std::vector <int>    elQuality;
  std::vector <int>    elCharge;
  
  //ID requirement
  std::vector <double> elDeta;
  std::vector <double> elDphi;
  std::vector <double> elSihih;
  std::vector <double> elHoE;
  std::vector <double> elD0;
  std::vector <double> elDZ;
  std::vector <double> elOoemoop;
  std::vector <int>    elMHits;
  std::vector <int>    elVtxFitConv;
  std::vector <double> elMVA;

  //Extra info about isolation
  std::vector <double> elChIso;
  std::vector <double> elNhIso;
  std::vector <double> elPhIso;
  std::vector <double> elAEff;
  std::vector <double> elRhoIso;

  //mother-information
  //Generator level information -- MC matching
  vector<double> elGen_Reco_dr;
  vector<int> elPdgId;
  vector<int> elStatus;
  vector<int> elMatched;
  vector<int> elNumberOfMothers;
  vector<double> elMother_pt;
  vector<double> elMother_eta;
  vector<double> elMother_phi;
  vector<double> elMother_energy;
  vector<int> elMother_id;
  vector<int> elMother_status;
  //Matched gen electron information:
  vector<double> elMatchedPt;
  vector<double> elMatchedEta;
  vector<double> elMatchedPhi;
  vector<double> elMatchedEnergy;
  
  edm::Handle<double> rhoHandle;
  event.getByLabel(rhoSrc_it, rhoHandle);
  rhoIso = std::max(*(rhoHandle.product()), 0.0);

  pat::strbitset retElectron  = electronSelL_->getBitTemplate();
  pat::strbitset retPFElectron  = pfElectronSelLJ_->getBitTemplate();
  bool retElectronT,retElectronM,retElectronL, retElectronLJ, retElectronDI;


  //
  //_____Electrons______
  //

  for (std::vector<edm::Ptr<pat::Electron> >::const_iterator iel = vSelElectrons.begin(); iel != vSelElectrons.end(); iel++){   
    //Protect against electrons without tracks (should never happen, but just in case)
    if ((*iel)->gsfTrack().isNonnull() and (*iel)->gsfTrack().isAvailable()){
      //Four vector
      elPt     . push_back((*iel)->ecalDrivenMomentum().pt()); //Must check: why ecalDrivenMomentum?
      elEta    . push_back((*iel)->ecalDrivenMomentum().eta());
      elPhi    . push_back((*iel)->ecalDrivenMomentum().phi());
      elEnergy . push_back((*iel)->ecalDrivenMomentum().energy());  
      
      //Isolation
      double AEff  = ElectronEffectiveArea::GetElectronEffectiveArea(ElectronEffectiveArea::kEleGammaAndNeutralHadronIso03, 
								     (*iel)->superCluster()->eta(), ElectronEffectiveArea::kEleEAData2012);
      double chIso = (*iel)->userIsolation(pat::PfChargedHadronIso);
      double nhIso = (*iel)->userIsolation(pat::PfNeutralHadronIso);
      double phIso = (*iel)->userIsolation(pat::PfGammaIso);
      double relIso = ( chIso + std::max(0.0, nhIso + phIso - rhoIso*AEff) ) / (*iel)->pt();
      
      elChIso  . push_back(chIso);
      elNhIso  . push_back(nhIso);
      elPhIso  . push_back(phIso);
      elAEff   . push_back(AEff);
      elRhoIso . push_back(rhoIso);

      elRelIso . push_back(relIso);
      elMVA . push_back((*iel)->electronID("mvaTrigV0"));

      //Conversion rejection
      int nLostHits = (*iel)->gsfTrack()->trackerExpectedHitsInner().numberOfLostHits();
      double dist   = (*iel)->convDist();
      double dcot   = (*iel)->convDcot();
      int notConv   = nLostHits == 0 and (fabs(dist) > 0.02 or fabs(dcot) > 0.02);
      elCharge.push_back((*iel)->charge()); 
      elNotConversion . push_back(notConv);
      
      retElectronL = (*electronSelL_)(**iel, event, retElectron);
      retElectronM = (*electronSelM_)(**iel, event, retElectron);
      retElectronT = (*electronSelT_)(**iel, event, retElectron);
      retElectronLJ = (*pfElectronSelLJ_)(**iel, event, retPFElectron);
      retElectronDI = (*pfElectronSelDil_)(**iel, event, retPFElectron);
      elQuality.push_back( (retElectronLJ<<4) + (retElectronDI<<3) +
	(retElectronT<<2) + (retElectronM<<1) + retElectronL);

      //IP: for some reason this is with respect to the first vertex in the collection
      if(goodPVs.size() > 0){
	elDxy.push_back((*iel)->gsfTrack()->dxy(goodPVs.at(0).position()));
        elDZ.push_back((*iel)->gsfTrack()->dz(goodPVs.at(0).position()));
      } else {
	elDxy.push_back(-999);
        elDZ.push_back(-999);
      }
      elChargeConsistent.push_back((*iel)->isGsfCtfScPixChargeConsistent());
      elIsEBEE.push_back(((*iel)->isEBEEGap()<<2) + ((*iel)->isEE()<<1) + (*iel)->isEB());
      elDeta.push_back((*iel)->deltaEtaSuperClusterTrackAtVtx());
      elDphi.push_back((*iel)->deltaPhiSuperClusterTrackAtVtx());
      elSihih.push_back((*iel)->sigmaIetaIeta());
      elHoE.push_back((*iel)->hadronicOverEm());
      elD0.push_back((*iel)->dB());
      elOoemoop.push_back(1.0/(*iel)->ecalEnergy() + (*iel)->eSuperClusterOverP()/(*iel)->ecalEnergy()); 
      elMHits.push_back((*iel)->gsfTrack()->trackerExpectedHitsInner().numberOfHits());
      elVtxFitConv.push_back((*iel)->passConversionVeto());
      if(isMc && keepFullMChistory){
      cout << "start\n";
        edm::Handle<reco::GenParticleCollection> genParticles;
        event.getByLabel(genParticles_it, genParticles);
	int matchId = findMatch(*genParticles, 11, (*iel)->eta(), (*iel)->phi());
	double closestDR = 10000.;
      cout << "matchId "<<matchId <<endl;
	if (matchId>=0) {
	  const reco::GenParticle & p = (*genParticles).at(matchId);
	  closestDR = mdeltaR( (*iel)->eta(), (*iel)->phi(), p.eta(), p.phi());
      cout << "closestDR "<<closestDR <<endl;
	  if(closestDR < 0.3){
            elGen_Reco_dr.push_back(closestDR);
            elPdgId.push_back(p.pdgId());
            elStatus.push_back(p.status());
            elMatched.push_back(1);
            elMatchedPt.push_back( p.pt());
            elMatchedEta.push_back(p.eta());
            elMatchedPhi.push_back(p.phi());
            elMatchedEnergy.push_back(p.energy());
	    int oldSize = elMother_id.size();
            fillMotherInfo(p.mother(), 0, elMother_id, elMother_status, elMother_pt, elMother_eta, elMother_phi, elMother_energy);
            elNumberOfMothers.push_back(elMother_id.size()-oldSize);
	  }
	} 
	if(closestDR >= 0.3){
	  elNumberOfMothers.push_back(-1);
          elGen_Reco_dr.push_back(-1.0);
          elPdgId.push_back(-1);
          elStatus.push_back(-1);
          elMatched.push_back(0);
          elMatchedPt.push_back(-1000.0);
          elMatchedEta.push_back(-1000.0);
          elMatchedPhi.push_back(-1000.0);
          elMatchedEnergy.push_back(-1000.0);
	  
	}

	
      }//closing the isMC checking criteria       
    }      
  }
  
  //Four vector
  SetValue("elPt"     , elPt);
  SetValue("elEta"    , elEta);
  SetValue("elPhi"    , elPhi);
  SetValue("elEnergy" , elEnergy);
  
  SetValue("elCharge", elCharge);
  //Quality requirements
  SetValue("elRelIso" , elRelIso); //Isolation
  SetValue("elDxy"    , elDxy);    //Dxy
  SetValue("elNotConversion" , elNotConversion);  //Conversion rejection
  SetValue("elChargeConsistent", elChargeConsistent);
  SetValue("elIsEBEE", elIsEBEE);
  SetValue("elQuality", elQuality);

  //ID cuts 
  SetValue("elDeta", elDeta);
  SetValue("elDphi", elDphi);
  SetValue("elSihih", elSihih);
  SetValue("elHoE", elHoE);
  SetValue("elD0", elD0);
  SetValue("elDZ", elDZ);
  SetValue("elOoemoop", elOoemoop);
  SetValue("elMHits", elMHits);
  SetValue("elVtxFitConv", elVtxFitConv);
  SetValue("elMVA", elMVA);

  //Extra info about isolation
  SetValue("elChIso" , elChIso);
  SetValue("elNhIso" , elNhIso);
  SetValue("elPhIso" , elPhIso);
  SetValue("elAEff"  , elAEff);
  SetValue("elRhoIso", elRhoIso);

  //MC matching -- mother information
  SetValue("elNumberOfMothers", elNumberOfMothers);
  SetValue("elGen_Reco_dr", elGen_Reco_dr);
  SetValue("elPdgId", elPdgId);
  SetValue("elStatus", elStatus);
  SetValue("elMatched",elMatched);
  SetValue("elMother_pt", elMother_pt);
  SetValue("elMother_eta", elMother_eta);
  SetValue("elMother_phi", elMother_phi);
  SetValue("elMother_energy", elMother_energy);
  SetValue("elMother_status", elMother_status);
  SetValue("elMother_id", elMother_id);
  //Matched gen muon information:
  SetValue("elMatchedPt", elMatchedPt);
  SetValue("elMatchedEta", elMatchedEta);
  SetValue("elMatchedPhi", elMatchedPhi);
  SetValue("elMatchedEnergy", elMatchedEnergy);
  
  
  //
  //_____ Muons _____________________________
  //
  
  std::vector <int> muCharge;
  std::vector <int> muGlobal;
  std::vector <int> muQuality;

  //Four vector
  std::vector <double> muPt;
  std::vector <double> muEta;
  std::vector <double> muPhi;
  std::vector <double> muEnergy;
  
  //Quality criteria
  std::vector <double> muChi2;
  std::vector <double> muDxy;
  std::vector <double> muDz;
  std::vector <double> muRelIso;
  
  std::vector <int> muNValMuHits;
  std::vector <int> muNMatchedStations;
  std::vector <int> muNValPixelHits;
  std::vector <int> muNTrackerLayers;

  //Extra info about isolation
  std::vector <double> muChIso;
  std::vector <double> muNhIso;
  std::vector <double> muGIso;
  std::vector <double> muPuIso;

  //Generator level information -- MC matching
  vector<double> muGen_Reco_dr;
  vector<int> muPdgId;
  vector<int> muStatus;
  vector<int> muMatched;
  vector<int> muNumberOfMothers;
  vector<double> muMother_pt;
  vector<double> muMother_eta;
  vector<double> muMother_phi;
  vector<double> muMother_energy;
  vector<int> muMother_id;
  vector<int> muMother_status;
  //Matched gen muon information:
  vector<double> muMatchedPt;
  vector<double> muMatchedEta;
  vector<double> muMatchedPhi;
  vector<double> muMatchedEnergy;

  pat::strbitset retMuon  = muonSelLJ_->getBitTemplate();
  bool retMuonLJ, retMuonDI;

  for (std::vector<edm::Ptr<pat::Muon> >::const_iterator imu = vSelMuons.begin(); imu != vSelMuons.end(); imu++){
    //Protect against muons without tracks (should never happen, but just in case)
    if ((*imu)->globalTrack().isNonnull() and (*imu)->globalTrack().isAvailable() and 
	(*imu)->innerTrack().isNonnull()  and (*imu)->innerTrack().isAvailable()){ 
      
      
      //charge
      muCharge.push_back((*imu)->charge());
      
      //Four vector
      muPt     . push_back((*imu)->pt());
      muEta    . push_back((*imu)->eta());
      muPhi    . push_back((*imu)->phi());
      muEnergy . push_back((*imu)->energy());  
      
      int global = (((*imu)->isGlobalMuon()<<2)+(*imu)->isTrackerMuon());
      muGlobal.push_back(global);
      
      retMuonLJ = (*muonSelLJ_)(**imu, retMuon);
      retMuonDI = ((*muonSelDil_)(**imu, retMuon) &&(global>0));
      muQuality.push_back( (retMuonLJ<<1) + retMuonDI);

      //Chi2
      muChi2 . push_back((*imu)->globalTrack()->normalizedChi2());

      //Isolation
      double chIso  = (*imu)->userIsolation(pat::PfChargedHadronIso);
      double nhIso  = (*imu)->userIsolation(pat::PfNeutralHadronIso);
      double gIso   = (*imu)->userIsolation(pat::PfGammaIso);
      double puIso  = (*imu)->userIsolation(pat::PfPUChargedHadronIso);
      double relIso = (chIso + std::max(0.,nhIso + gIso - 0.5*puIso)) / (*imu)->pt();
      muRelIso . push_back(relIso);

      muChIso . push_back(chIso);
      muNhIso . push_back(nhIso);
      muGIso  . push_back(gIso);
      muPuIso . push_back(puIso);

      //IP: for some reason this is with respect to the first vertex in the collection
      if (goodPVs.size() > 0){
	muDxy . push_back((*imu)->muonBestTrack()->dxy(goodPVs.at(0).position()));
	muDz  . push_back((*imu)->muonBestTrack()->dz(goodPVs.at(0).position()));
      } else {
	muDxy . push_back(-999);
	muDz  . push_back(-999);
      }
      //Numbers of hits
      muNValMuHits       . push_back((*imu)->globalTrack()->hitPattern().numberOfValidMuonHits());
      muNMatchedStations . push_back((*imu)->numberOfMatchedStations());
      muNValPixelHits    . push_back((*imu)->innerTrack()->hitPattern().numberOfValidPixelHits());
      muNTrackerLayers   . push_back((*imu)->innerTrack()->hitPattern().trackerLayersWithMeasurement());

      if(isMc && keepFullMChistory){
        edm::Handle<reco::GenParticleCollection> genParticles;
        event.getByLabel(genParticles_it, genParticles);
	int matchId = findMatch(*genParticles, 13, (*imu)->eta(), (*imu)->phi());
	double closestDR = 10000.;
	if (matchId>=0) {
	  const reco::GenParticle & p = (*genParticles).at(matchId);
	  closestDR = mdeltaR( (*imu)->eta(), (*imu)->phi(), p.eta(), p.phi());
	  if(closestDR < 0.3){
            muGen_Reco_dr.push_back(closestDR);
            muPdgId.push_back(p.pdgId());
            muStatus.push_back(p.status());
            muMatched.push_back(1);
            muMatchedPt.push_back( p.pt());
            muMatchedEta.push_back(p.eta());
            muMatchedPhi.push_back(p.phi());
            muMatchedEnergy.push_back(p.energy());
	    int oldSize = muMother_id.size();
            fillMotherInfo(p.mother(), 0, muMother_id, muMother_status, muMother_pt, muMother_eta, muMother_phi, muMother_energy);
            muNumberOfMothers.push_back(muMother_id.size()-oldSize);
	  }
	} 
	if(closestDR >= 0.3){
	  muNumberOfMothers.push_back(-1);
          muGen_Reco_dr.push_back(-1.0);
          muPdgId.push_back(-1);
          muStatus.push_back(-1);
          muMatched.push_back(0);
          muMatchedPt.push_back(-1000.0);
          muMatchedEta.push_back(-1000.0);
          muMatchedPhi.push_back(-1000.0);
          muMatchedEnergy.push_back(-1000.0);
	  
	}

	
      }
    }
  }
  
  
  SetValue("muCharge", muCharge);
  SetValue("muQuality", muQuality);
  SetValue("muGlobal", muGlobal);
  //Four vector
  SetValue("muPt"     , muPt);
  SetValue("muEta"    , muEta);
  SetValue("muPhi"    , muPhi);
  SetValue("muEnergy" , muEnergy);

  //Quality criteria
  SetValue("muChi2"   , muChi2);
  SetValue("muDxy"    , muDxy);
  SetValue("muDz"     , muDz);
  SetValue("muRelIso" , muRelIso);

  SetValue("muNValMuHits"       , muNValMuHits);
  SetValue("muNMatchedStations" , muNMatchedStations);
  SetValue("muNValPixelHits"    , muNValPixelHits);
  SetValue("muNTrackerLayers"   , muNTrackerLayers);
  
  //Extra info about isolation
  SetValue("muChIso", muChIso);
  SetValue("muNhIso", muNhIso);
  SetValue("muGIso" , muGIso);
  SetValue("muPuIso", muPuIso);

   //MC matching -- mother information
  SetValue("muGen_Reco_dr", muGen_Reco_dr);
  SetValue("muPdgId", muPdgId);
  SetValue("muStatus", muStatus);
  SetValue("muMatched",muMatched);
  SetValue("muMother_pt", muMother_pt);
  SetValue("muMother_eta", muMother_eta);
  SetValue("muMother_phi", muMother_phi);
  SetValue("muMother_energy", muMother_energy);
  SetValue("muMother_status", muMother_status);
  SetValue("muMother_id", muMother_id);
  SetValue("muNumberOfMothers", muNumberOfMothers);
  //Matched gen muon information:
  SetValue("muMatchedPt", muMatchedPt);
  SetValue("muMatchedEta", muMatchedEta);
  SetValue("muMatchedPhi", muMatchedPhi);
  SetValue("muMatchedEnergy", muMatchedEnergy);  
  //
  //_____ Jets ______________________________
  //


  //Get AK5 Jets
  //Four vector
  std::vector <double> AK5JetPt;
  std::vector <double> AK5JetEta;
  std::vector <double> AK5JetPhi;
  std::vector <double> AK5JetEnergy;

  std::vector <int>    AK5JetTBag;
  std::vector <double> AK5JetRCN;
  std::vector <int>  AK5JetnChHad;
  std::vector <int>  AK5JetnNeuHad; 

  for (std::vector<edm::Ptr<pat::Jet> >::const_iterator ijet = vSelJets.begin();
	ijet != vSelJets.end(); ijet++){
    
    //Four vector
      TLorentzVector lv = selector->correctJet(**ijet, event);

    AK5JetPt     . push_back(lv.Pt());
    AK5JetEta    . push_back(lv.Eta());
    AK5JetPhi    . push_back(lv.Phi());
    AK5JetEnergy . push_back(lv.Energy());

    AK5JetTBag   . push_back(selector->isJetTagged(**ijet, event));
    AK5JetRCN    . push_back(((*ijet)->chargedEmEnergy()+(*ijet)->chargedHadronEnergy()) / ((*ijet)->neutralEmEnergy()+(*ijet)->neutralHadronEnergy()));    
    AK5JetnChHad    . push_back((*ijet)->chargedHadronMultiplicity());  //saving charged hadron multiplicity.
    AK5JetnNeuHad   . push_back((*ijet)->neutralHadronMultiplicity() );  //saving neutral hadron multiplicity.
}

  //Four vector
  SetValue("AK5JetPt"     , AK5JetPt);
  SetValue("AK5JetEta"    , AK5JetEta);
  SetValue("AK5JetPhi"    , AK5JetPhi);
  SetValue("AK5JetEnergy" , AK5JetEnergy);

  SetValue("AK5JetTBag"   , AK5JetTBag);
  SetValue("AK5JetRCN"    , AK5JetRCN);
  SetValue("AK5JetnChHad", AK5JetnChHad);
  SetValue("AK5JetnNeuHad", AK5JetnNeuHad);

  // MET
  double _met = -9999.0;
  double _met_phi = -9999.0;
  // Corrected MET
  double _corr_met = -9999.0;
  double _corr_met_phi = -9999.0;
  //double _calo_met = -9999.0;
  //double _calo_met_phi = -9999.0;


  if(pMet.isNonnull() && pMet.isAvailable()) {
    _met = pMet->p4().pt();
    _met_phi = pMet->p4().phi();

    TLorentzVector corrMET = selector->correctMet(*pMet, event);
    if(corrMET.Pt()>0) {
	_corr_met = corrMET.Pt();
	_corr_met_phi = corrMET.Phi();
    }

  }

/*
  edm::InputTag patMets = edm::InputTag("patMETs");
  edm::Handle<std::vector<pat::MET> > metHandle;
  event.getByLabel(patMets, metHandle);

  //pat::MET met = *(metHandle->begin());
 //cout << "Size of met vector = " << long((*metHandle->size())) << endl;
 for (std::vector<pat::MET>::const_iterator imet = metHandle->begin(); imet != metHandle->end(); imet++){
  //cout << "Size of met vector = " << metHandle->size() << endl;  
  //cout << "isPFMET = " << imet->isPFMET() << endl;
  if(imet->isCaloMET()){
  _calo_met = imet->pt();
  _calo_met_phi = imet->phi();

   }
 }*/
  SetValue("met", _met);
  SetValue("met_phi", _met_phi);
  SetValue("corr_met", _corr_met);
  SetValue("corr_met_phi", _corr_met_phi);
  //SetValue("calo_met", _calo_met);
  //SetValue("calo_met_phi", _calo_met_phi);

  //
  //_____ Gen Info ______________________________
  //

  //Four vector
  std::vector <double> genPt;
  std::vector <double> genEta;
  std::vector <double> genPhi;
  std::vector <double> genEnergy;

  //Identity
  std::vector <int> genID;
  std::vector <int> genIndex;
  std::vector <int> genStatus;
  std::vector <int> genMotherID;
  std::vector <int> genMotherIndex;

  double higgsWeight = 1.0;

  if (isMc){
    
  //From Mike Luk: April 10th 2013 
      double higgsBBSf     = 1.47415;
      double higgsTauTauSf = 1.32511;
      double higgsMuMuSf   = 1.30178;
      double higgsCCSf     = 1.35842;

      double higgsGGSf         = 0.25024;
      double higgsGammaGammaSf = 5.55457;
      double higgsZGammaSf     = 1.61765;
      double higgsWWSf = 1.22012;
      double higgsZZSf = 1.38307;


  /*  double higgsBBSf     = 0.890;
    double higgsTauTauSf = 0.898;
    double higgsMuMuSf   = 0.902;
    double higgsCCSf     = 0.890;
    double higgsGGSf         = 0.972;
    double higgsGammaGammaSf = 1.020;
    double higgsZGammaSf     = 1.390;
    double higgsWWSf = 1.52;
    double higgsZZSf = 1.66;
  */
    edm::Handle<reco::GenParticleCollection> genParticles;
    event.getByLabel(genParticles_it, genParticles);
    
    for(size_t i = 0; i < genParticles->size(); i++){
      const reco::GenParticle & p = (*genParticles).at(i);

      int id = p.pdgId();
      // find higgs (25)                                                                                                            
      if(abs(id) == 25 && p.status() == 3){
        int absDauIds = 0;
        size_t nDaughters = p.numberOfDaughters();
        // get all daughters                                                                                                        
        for(size_t j = 0; j < nDaughters; ++ j) {
          int dauId = (p.daughter(j))->pdgId();
          absDauIds += abs(dauId);
        }// daughters                                                                                                               

        // for each higgs, find the decay products and weight accordingly                                                           
        if(absDauIds==10) higgsWeight *= higgsBBSf;
        if(absDauIds==30) higgsWeight *= higgsTauTauSf;
        if(absDauIds==26) higgsWeight *= higgsMuMuSf;
        if(absDauIds==8)  higgsWeight *= higgsCCSf;
        if(absDauIds==42) higgsWeight *= higgsGGSf;
        if(absDauIds==44) higgsWeight *= higgsGammaGammaSf;
        if(absDauIds==45) higgsWeight *= higgsZGammaSf;
        if(absDauIds==48) higgsWeight *= higgsWWSf;
        if(absDauIds==46) higgsWeight *= higgsZZSf;
      } // if higgs                                                                                                                 

      //Find status 3 particles
      if (p.status() == 3){
	reco::Candidate* mother = (reco::Candidate*) p.mother();
	if (not mother)            continue;
	
	bool bKeep = false;
	for (unsigned int uk = 0; uk < keepMomPDGID.size(); uk++){
	  if (abs(mother->pdgId()) == (int) keepMomPDGID.at(uk)){
	    bKeep = true;
	    break;
	  }
	}
	
	if (not bKeep){
	  for (unsigned int uk = 0; uk < keepPDGID.size(); uk++){
	    if (abs(p.pdgId()) == (int) keepPDGID.at(uk)){
	      bKeep = true;
	      break;
	    }
	  }
	}
	
	if (not bKeep) continue;
	
	//Find index of mother
	int mInd = 0;
	for(size_t j = 0; j < genParticles->size(); j++){
	  const reco::GenParticle & q = (*genParticles).at(j);
	  if (q.status() != 3) continue;
	  if (mother->pdgId() == q.pdgId() and fabs(mother->eta() - q.eta()) < 0.01 and fabs(mother->pt() - q.pt()) < 0.01){
	    mInd = (int) j;
	    break;
	  }
	}
	
	//Four vector
	genPt     . push_back(p.pt());
	genEta    . push_back(p.eta());
	genPhi    . push_back(p.phi());
	genEnergy . push_back(p.energy());
	
	//Identity
	genID            . push_back(p.pdgId());
	genIndex         . push_back((int) i);
	genStatus        . push_back(p.status());
	genMotherID      . push_back(mother->pdgId());
	genMotherIndex   . push_back(mInd);
      }
    }//End loop over gen particles
  }  //End MC-only if

  //Four vector
  SetValue("genPt"     , genPt);
  SetValue("genEta"    , genEta);
  SetValue("genPhi"    , genPhi);
  SetValue("genEnergy" , genEnergy);

  //Identity
  SetValue("genID"            , genID);
  SetValue("genIndex"         , genIndex);
  SetValue("genStatus"        , genStatus);
  SetValue("genMotherID"      , genMotherID);
  SetValue("genMotherIndex"   , genMotherIndex);
  SetValue("higgsWeight",higgsWeight);

  return 0;
}

int DileptonCalc::findMatch(const reco::GenParticleCollection & genParticles, int idToMatch, double eta, double phi){

  float dRtmp = 1000;
  float closestDR = 10000.;
  int closestGenPart = -1;

  for(size_t j = 0; j < genParticles.size(); ++ j) {
    const reco::GenParticle & p = (genParticles).at(j);
    dRtmp = mdeltaR( eta, phi, p.eta(), p.phi());
    if ( dRtmp < closestDR && abs(p.pdgId()) == idToMatch){// && dRtmp < 0.3) {
      closestDR = dRtmp;
      closestGenPart = j;
    }//end of requirement for matching
  }//end of gen particle loop 
  return closestGenPart;
}


double DileptonCalc::mdeltaR(double eta1, double phi1, double eta2, double phi2) {
  return std::sqrt(deltaR2 (eta1, phi1, eta2, phi2));
}

void DileptonCalc::fillMotherInfo(const reco::Candidate *mother, int i, vector <int> & momid, vector <int> & momstatus, vector<double> & mompt, vector<double> & mometa, vector<double> & momphi, vector<double> & momenergy)
{
  if(mother) {
    momid.push_back(mother->pdgId());
    momstatus.push_back(mother->status());
    mompt.push_back(mother->pt());
    mometa.push_back(mother->eta());
    momphi.push_back(mother->phi());
    momenergy.push_back(mother->energy());
    if(i<10)fillMotherInfo(mother->mother(), i+1, momid, momstatus, mompt, mometa, momphi, momenergy);
  }


} 
