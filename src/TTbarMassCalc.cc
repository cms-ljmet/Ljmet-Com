/*
  Calculator for the Run 2 inclusive TprimeTprime analysis
  Finds Tprime or Bprime particles and tags the decays chain so branching ratios can be scanned

  Author: Julie Hogan, 2015
*/


#include <iostream>
#include <algorithm>
#include "LJMet/Com/interface/BaseCalc.h"
#include "LJMet/Com/interface/LjmetFactory.h"
#include "LJMet/Com/interface/LjmetEventContent.h"
#include "TLorentzVector.h" 
#include "DataFormats/Math/interface/deltaR.h"
#include "SimDataFormats/JetMatching/interface/JetFlavourInfo.h"
#include "DataFormats/JetReco/interface/GenJet.h"

using namespace std;

class LjmetFactory;


class TTbarMassCalc : public BaseCalc{
  
public:
  
    TTbarMassCalc();
    virtual ~TTbarMassCalc(){}

    virtual int BeginJob(){
 
      if (mPset.exists("genParticles")) genParticles_it = mPset.getParameter<edm::InputTag>("genParticles");
      else                              genParticles_it = edm::InputTag("prunedGenParticles");

      if (mPset.exists("genJets"))      genJets_it = mPset.getParameter<edm::InputTag>("genJets");
      else                              genJets_it = edm::InputTag("slimmedGenJets");
 
      return 0;
    }

    virtual int AnalyzeEvent(edm::EventBase const & event, BaseEventSelector * selector);
    virtual int EndJob(){return 0;}

  
private:
  
  edm::InputTag genParticles_it;
  edm::InputTag genJets_it;
 
};



static int reg = LjmetFactory::GetInstance()->Register(new TTbarMassCalc(), "TTbarMassCalc");



TTbarMassCalc::TTbarMassCalc(){
}

int TTbarMassCalc::AnalyzeEvent(edm::EventBase const & event,
                             BaseEventSelector * selector){
    //
    // compute event variables here
    //
      
    std::vector<int>    topID;
    std::vector<int>    topMotherID;
    std::vector<double> topPt;
    std::vector<double> topEta;
    std::vector<double> topPhi;
    std::vector<double> topEnergy;
    std::vector<double> topMass;

    std::vector<int>    topbID;
    std::vector<double> topbPt;
    std::vector<double> topbEta;
    std::vector<double> topbPhi;
    std::vector<double> topbEnergy;

    std::vector<int>    topWID;
    std::vector<double> topWPt;
    std::vector<double> topWEta;
    std::vector<double> topWPhi;
    std::vector<double> topWEnergy;

    std::vector<int>    allTopsID;
    std::vector<int>    allTopsStatus;
    std::vector<double> allTopsPt;
    std::vector<double> allTopsEta;
    std::vector<double> allTopsPhi;
    std::vector<double> allTopsEnergy;

    double ttbarMass = -999.9;
    TLorentzVector top1;
    TLorentzVector top2;

    // Get the generated particle collection
    edm::Handle<reco::GenParticleCollection> genParticles;
    if(event.getByLabel(genParticles_it, genParticles)){

      // loop over all gen particles in event
      for(size_t i = 0; i < genParticles->size(); i++){
	const reco::GenParticle &p = (*genParticles).at(i);
	int id = p.pdgId();
	
	// find tops 
	if(abs(id) != 6) continue;

	size_t nDs = p.numberOfDaughters();
	if(nDs == 2){
	  int d1Id = p.daughter(0)->pdgId();
	  int d2Id = p.daughter(1)->pdgId();
	  
	  if(abs(d1Id)==6 || abs(d2Id)==6) continue;

	  if((abs(d1Id)==5 && abs(d2Id)==24) || (abs(d1Id)==24 && abs(d2Id)==5)){
	    const reco::Candidate *d = p.daughter(0);
	    if(abs(d1Id)==24) d = p.daughter(1);

	    topbID.push_back(d->pdgId());
	    topbPt.push_back(d->pt());
	    topbEta.push_back(d->eta());
	    topbPhi.push_back(d->phi());
	    topbEnergy.push_back(d->energy());

	    const reco::Candidate *W = p.daughter(1);
	    if(abs(d2Id)==5) W = p.daughter(0);

	    while(W->numberOfDaughters() == 1) W = W->daughter(0);
	    
	    size_t nWDs = W->numberOfDaughters();
	    if(nWDs > 2) cout << "W daughters: " << nWDs << endl;
	    int Wd1Id = abs(W->daughter(0)->pdgId());
	    int Wd2Id = abs(W->daughter(1)->pdgId());
	    if((Wd1Id > 10 && Wd1Id < 17 && Wd2Id > 10 && Wd2Id < 17) || (Wd1Id < 6 && Wd2Id < 6)){
	      topWID.push_back(W->daughter(0)->pdgId());
	      topWID.push_back(W->daughter(1)->pdgId());
	      topWPt.push_back(W->daughter(0)->pt());
	      topWPt.push_back(W->daughter(1)->pt());
	      topWEta.push_back(W->daughter(0)->eta());
	      topWEta.push_back(W->daughter(1)->eta());
	      topWPhi.push_back(W->daughter(0)->phi());
	      topWPhi.push_back(W->daughter(1)->phi());
	      topWEnergy.push_back(W->daughter(0)->energy());
	      topWEnergy.push_back(W->daughter(1)->energy());
	    }else{
	      cout << "Weird W decay: " << Wd1Id << ", " << Wd2Id << endl;
	    }
	  }else{
	    cout << "2 top daughters are: " << d1Id << ", " << d2Id << endl;
	  }
	}

	// Find original tops, before t -> t -> t chains
	if(abs(p.mother()->pdgId()) == 6) continue;      
	
	topID.push_back(id);
	topPt.push_back(p.pt());
	topEta.push_back(p.eta());
	topPhi.push_back(p.phi());
	topMass.push_back(p.mass());
	topEnergy.push_back(p.energy());
	topMotherID.push_back(p.mother()->pdgId());

      }
            
      // Now time for some checking -- do we have the right particles? 
      if(topID.size() == 0) ttbarMass = -999;
      else if(topID.size() != 2){
	//	std::cout << "TTbarMassCalc: More/less than 2 quarks stored: " << topID.size() << std::endl;
	//	for(size_t i = 0; i < topID.size(); i++){	
	//	  std::cout << "quark " << i << " = " << topID[i] << ", mother ID = " << topMotherID[i] << std::endl;
	//	}
      }
      else{
	if(topID[0]*topID[1] > 0){
	  cout << "tops have same ID sign!" << endl;
	}
	else{
	  /*
	  cout << "----------------------------------" << endl;
	  cout << "particle " << topID[0] << " pt = " << topPt[0] << endl;
	  cout << "particle " << topID[1] << " pt = " << topPt[1] << endl;
	  */
	  top1.SetPtEtaPhiE(topPt[0],topEta[0],topPhi[0],topEnergy[0]);
	  top2.SetPtEtaPhiE(topPt[1],topEta[1],topPhi[1],topEnergy[1]);
	  
	  ttbarMass = (top1+top2).M();
	}
      }

      // loop over all gen particles in event
      for(size_t i = 0; i < genParticles->size(); i++){
	const reco::GenParticle &p = (*genParticles).at(i);
	int id = p.pdgId();
	
	// find tops 
	if(abs(id) != 6) continue;
	
	allTopsID.push_back(id);
	allTopsPt.push_back(p.pt());
	allTopsEta.push_back(p.eta());
	allTopsPhi.push_back(p.phi());
	allTopsEnergy.push_back(p.energy());
	allTopsStatus.push_back(p.status());
	
      }
    }
    SetValue("topID",topID);
    SetValue("topPt",topPt);
    SetValue("topEta",topEta);
    SetValue("topPhi",topPhi);
    SetValue("topMass",topMass);
    SetValue("topEnergy",topEnergy);

    SetValue("topbID",topbID);
    SetValue("topbPt",topbPt);
    SetValue("topbEta",topbEta);
    SetValue("topbPhi",topbPhi);
    SetValue("topbEnergy",topbEnergy);

    SetValue("topWID",topWID);
    SetValue("topWPt",topWPt);
    SetValue("topWEta",topWEta);
    SetValue("topWPhi",topWPhi);
    SetValue("topWEnergy",topWEnergy);
    
    SetValue("ttbarMass",ttbarMass);

    SetValue("allTopsID",allTopsID);
    SetValue("allTopsPt",allTopsPt);
    SetValue("allTopsEta",allTopsEta);
    SetValue("allTopsPhi",allTopsPhi);
    SetValue("allTopsStatus",allTopsStatus);
    SetValue("allTopsEnergy",allTopsEnergy);

    int isTTbb = 0;
    int isTTbj = 0;
    int isTTcc = 0;
    int isTTll = 0;
    int isTTlf = 0;
    int isTT = 0;

    int foundb1 = 0;
    int foundb2 = 0;
    int foundW[4] = {0};
    int extrab = 0;
    int cjets = 0;
    int bjets = 0;
    int ljets = 0;
    
    int WdecayJets = 4;
    int WdecayCMatches = 0;
    int WdecayLMatches = 0;
    if(topbID.size() >= 2 && topWID.size() >= 4){

      TLorentzVector b1, b2, w1, w2, w3, w4, jet;
      b1.SetPtEtaPhiE(topbPt[0],topbEta[0],topbPhi[0],topbEnergy[0]);
      b2.SetPtEtaPhiE(topbPt[1],topbEta[1],topbPhi[1],topbEnergy[1]);
      w1.SetPtEtaPhiE(topWPt[0],topWEta[0],topWPhi[0],topWEnergy[0]);
      w2.SetPtEtaPhiE(topWPt[1],topWEta[1],topWPhi[1],topWEnergy[1]);
      w3.SetPtEtaPhiE(topWPt[2],topWEta[2],topWPhi[2],topWEnergy[2]);
      w4.SetPtEtaPhiE(topWPt[3],topWEta[3],topWPhi[3],topWEnergy[3]);
      
      edm::InputTag JetColl = edm::InputTag("slimmedJets");
      edm::Handle<std::vector<pat::Jet> > Jets;
      event.getByLabel(JetColl, Jets);

      for(int i = 0; i < 4; i++){
	if(abs(topWID[i]) > 10 && abs(topWID[i]) < 17){
	  WdecayJets -= 1;
	  foundW[i] = 1;
	}
      }
      
      //cout << "W -> leptons = " << (4.0-WdecayJets)/2.0 << endl;
      
      for (std::vector<pat::Jet>::const_iterator ijet = Jets->begin(); ijet != Jets->end(); ijet++){
	if(ijet->pt() < 30 || fabs(ijet->eta()) > 2.4) continue;
	
	jet.SetPtEtaPhiE(ijet->pt(),ijet->eta(),ijet->phi(),ijet->energy());
	
	//cout << "PartonFlavour = " << ijet->partonFlavour() << ", HadronFlavour = " << ijet->hadronFlavour() << endl;      
	const reco::GenParticle *JetGenParticle = ijet->genParton();
	//if(JetGenParticle){
	//cout << "GenParticle = " << JetGenParticle->pdgId() << endl;
	//}
	
	bool leptonoverlap = false;
	double dRb1 = b1.DeltaR(jet);
	double dRb2 = b2.DeltaR(jet);
	double dRW[4] = {w1.DeltaR(jet),w2.DeltaR(jet),w3.DeltaR(jet),w4.DeltaR(jet)};
	
	for(unsigned int i = 0; i < 4; i++){
	  if(abs(topWID[i]) > 10 && abs(topWID[i]) < 17 && dRW[i] < 0.4) leptonoverlap = true;
	}
	if(leptonoverlap) continue;
	
	if((JetGenParticle && JetGenParticle->pdgId() == 5) || ijet->hadronFlavour()==5){
	  //cout << "BJET: DR b1 = " << dRb1 << ", DR b2 = " << dRb2 << endl;
	  if(!foundb1 && dRb1 < 0.3) foundb1 = 1;
	  if(!foundb2 && dRb2 < 0.3) foundb2 = 1;
	  if(dRb1 > 0.3 && dRb2 > 0.3) extrab++;
	  bjets++;
	}
	
	if((JetGenParticle && JetGenParticle->pdgId() == 4) || ijet->hadronFlavour()==4){
	  for(int i=0; i < 4; i++){
	    //cout << "CJET: DR W" << i << " = " << dRW[i] << endl;
	    if(foundW[i]==0 && dRW[i] < 0.3){
	      WdecayCMatches += 1;
	      foundW[i] = 1;
	    }
	  }
	  cjets++;
	}
	if((JetGenParticle && (JetGenParticle->pdgId() < 4 || JetGenParticle->pdgId() == 21)) || (ijet->hadronFlavour()<4 || ijet->hadronFlavour()==21)){
	  for(int i=0; i < 4; i++){
	    //cout << "LJET: dR W" << i << " = " << dRW[i] << endl;
	    if(foundW[i]==0 && dRW[i] < 0.3){
	      WdecayLMatches += 1;
	      foundW[i] = 1;
	    }
	  }
	  ljets++;
	}
      }
      //cout << "Found b1 and b2 = " << foundb1 << ", " << foundb2 << endl;
      //cout << "Extra b's = " << extrab << endl;
      //cout << "W jets = " << WdecayJets << ", matched " << WdecayCMatches+WdecayLMatches << endl;
      //cout << "B jets = 2, matched " << foundb1+foundb2 << endl;
      //cout << "Extra b's = " << extrab << ", c's = " << cjets-WdecayCMatches << ", l's = " << ljets-WdecayLMatches << endl;
      
      if(extrab >= 2) isTTbb = 1;
      else if(extrab == 1) isTTbj = 1;
      else if((cjets-WdecayCMatches) >= 2) isTTcc = 1;
      else if((cjets-WdecayCMatches) == 0 && (ljets-WdecayLMatches) >= 2) isTTll = 1;
      else if((cjets-WdecayCMatches)+(ljets-WdecayLMatches) >= 1) isTTlf = 1;
      else isTT = 1;
      
      //cout << "isTTbb = " << isTTbb << ", isTTbj = " << isTTbj << endl;
      //cout << "isTTcc = " << isTTcc << ", isTTll = " << isTTll << endl;
      //cout << "isTTlf = " << isTTlf << ", isTT = " << isTT << endl;
      //cout << "---------------------------------------" << endl;
    }      
    SetValue("NBsFromTTbar",foundb1+foundb2);
    SetValue("NWdecaysFromTTbar",(4-WdecayJets)+WdecayCMatches+WdecayLMatches);
    SetValue("NExtraBs",extrab);
    SetValue("NExtraBs",cjets-WdecayCMatches);
    SetValue("NExtraBs",ljets-WdecayLMatches);
    SetValue("NTotalBs",bjets);
    SetValue("NCharm",cjets);
    SetValue("NLight",ljets);        
    SetValue("isTTbb",isTTbb);
    SetValue("isTTbj",isTTbj);
    SetValue("isTTcc",isTTcc);
    SetValue("isTTll",isTTll);
    SetValue("isTTlf",isTTlf);
    SetValue("isTT",isTT);
     
    return 0;
}

