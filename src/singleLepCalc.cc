/*
 Calculator for a generic single lepton analysis
 
 Author: Joshua Swanson, 2014
 */

#include <iostream>
#include <limits>   // std::numeric_limits

#include "LJMet/Com/interface/BaseCalc.h"
#include "LJMet/Com/interface/LjmetFactory.h"
#include "LJMet/Com/interface/LjmetEventContent.h"
#include "TLorentzVector.h"

#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/PatCandidates/interface/TriggerObject.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "EgammaAnalysis/ElectronTools/interface/ElectronEffectiveArea.h"
#include "LJMet/Com/interface/MVAElectronSelector.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "DataFormats/BTauReco/interface/CATopJetTagInfo.h"
#include "LJMet/Com/interface/MiniIsolation.h"

#include "LHAPDF/LHAPDF.h"
#include "LHAPDF/GridPDF.h"
#include "LHAPDF/Info.h"
#include "LHAPDF/Config.h"
#include "LHAPDF/PDFInfo.h"
#include "LHAPDF/PDFSet.h"
#include "LHAPDF/Factories.h"

using std::cout;
using std::endl;

class LjmetFactory;

class singleLepCalc : public BaseCalc {
public:
    singleLepCalc();
    virtual ~singleLepCalc();
    virtual int BeginJob();
    virtual int AnalyzeEvent(edm::EventBase const & event, BaseEventSelector * selector);
    virtual int EndJob(){return 0;};
    
private:
    edm::InputTag rhoSrc_;
    edm::InputTag muonSrc_;
    bool                      isMc;
    std::string               dataType;
    std::vector<std::string>  elTrigMatchFilters;
    std::vector<std::string>  muTrigMatchFilters;
//    edm::InputTag             rhoSrc_it;
    edm::InputTag             triggerSummary_;
    edm::InputTag             triggerCollection_;
    edm::InputTag             pvCollection_it;
    edm::InputTag             genParticles_it;
    edm::InputTag             packedPFCandsLabel_;
    edm::InputTag             genJets_it;
    edm::InputTag             elec_it;
    edm::InputTag             muon_it;
    std::vector<unsigned int> keepPDGID;
    std::vector<unsigned int> keepMomPDGID;
    std::vector<unsigned int> keepPDGIDForce;
    std::vector<unsigned int> keepStatusForce;
    bool keepFullMChistory;
    bool cleanGenJets;
    bool UseElMVA;
    bool doElSCMETCorr;
    bool orlhew;
    bool saveLooseLeps;
    bool doAllJetSyst;
    std::string basePDFname;
    std::string newPDFname;

    double rhoIso;

    std::vector<reco::Vertex> goodPVs;
    int findMatch(const reco::GenParticleCollection & genParticles, int idToMatch, double eta, double phi);
    double mdeltaR(double eta1, double phi1, double eta2, double phi2);
    void fillMotherInfo(const reco::Candidate *mother, int i, std::vector <int> & momid, std::vector <int> & momstatus, std::vector<double> & mompt, std::vector<double> & mometa, std::vector<double> & momphi, std::vector<double> & momenergy);

    static bool SortLVByPt(const TLorentzVector a, const TLorentzVector b) {return a.Pt() > b.Pt();}

};

static int reg = LjmetFactory::GetInstance()->Register(new singleLepCalc(), "singleLepCalc");

/*namespace LHAPDF {
      void initPDFSet(int nset, const std::string& filename, int member=0);
      int numberPDF(int nset);
      void usePDFMember(int nset, int member);
      double xfx(int nset, double x, double Q, int fl);
      double getXmin(int nset, int member);
      double getXmax(int nset, int member);
      double getQ2min(int nset, int member);
      double getQ2max(int nset, int member);
      void extrapolate(bool extrapolate=true);
}*/
using namespace LHAPDF;


singleLepCalc::singleLepCalc()
{
}

singleLepCalc::~singleLepCalc()
{
}

int singleLepCalc::BeginJob()
{
    if (mPset.exists("dataType"))     dataType = mPset.getParameter<std::string>("dataType");
    else                              dataType = "None"; 

    if (mPset.exists("elTrigMatchFilters"))     elTrigMatchFilters = mPset.getParameter<std::vector<std::string>>("elTrigMatchFilters");

    if (mPset.exists("muTrigMatchFilters"))     muTrigMatchFilters = mPset.getParameter<std::vector<std::string>>("muTrigMatchFilters");

    if (mPset.exists("rhoSrc")) rhoSrc_ = mPset.getParameter<edm::InputTag>("rhoSrc");
    else                        rhoSrc_ = edm::InputTag("fixedGridRhoAll");

    if (mPset.exists("pvCollection")) pvCollection_it = mPset.getParameter<edm::InputTag>("pvCollection");
    else                              pvCollection_it = edm::InputTag("offlineSlimmedPrimaryVertices");

    if (mPset.exists("triggerSummary")) triggerSummary_ = mPset.getParameter<edm::InputTag>("triggerSummary");
    else                                triggerSummary_ = edm::InputTag("selectedPatTrigger");
    
    if (mPset.exists("triggerCollection")) triggerCollection_ = mPset.getParameter<edm::InputTag>("triggerCollection");
    else                                   triggerCollection_ = edm::InputTag("TriggerResults::HLT");
    
    if (mPset.exists("isMc"))         isMc = mPset.getParameter<bool>("isMc");
    else                              isMc = false;

    if (mPset.exists("genParticles")) genParticles_it = mPset.getParameter<edm::InputTag>("genParticles");
    else                              genParticles_it = edm::InputTag("prunedGenParticles");

    if (mPset.exists("genJets"))      genJets_it = mPset.getParameter<edm::InputTag>("genJets");
    else                              genJets_it = edm::InputTag("slimmedGenJets");

    if (mPset.exists("packedPFCands"))	packedPFCandsLabel_ = mPset.getParameter<edm::InputTag>("packedPFCands");
    else                              	packedPFCandsLabel_ = edm::InputTag("packedPFCandidates");

    if (mPset.exists("keepPDGID"))    keepPDGID = mPset.getParameter<std::vector<unsigned int> >("keepPDGID");
    else                              keepPDGID.clear();

    if (mPset.exists("keepMomPDGID")) keepMomPDGID = mPset.getParameter<std::vector<unsigned int> >("keepMomPDGID");
    else                              keepMomPDGID.clear();
    
    if (mPset.exists("keepPDGIDForce")) keepPDGIDForce = mPset.getParameter<std::vector<unsigned int> >("keepPDGIDForce");
    else                              keepPDGIDForce.clear();
    
    if (mPset.exists("keepStatusForce")) keepStatusForce = mPset.getParameter<std::vector<unsigned int> >("keepStatusForce");
    else                              keepStatusForce.clear();

    if (keepStatusForce.size() != keepPDGIDForce.size()) {
        cout<<"Sizes of forced status and ID std::vectors do not match, ignoring input!!!"<<endl;
        keepStatusForce.clear();
        keepPDGIDForce.clear();
    }
    
    if (mPset.exists("keepFullMChistory")) keepFullMChistory = mPset.getParameter<bool>("keepFullMChistory");
    else                                   keepFullMChistory = true;
    cout << "keepFullMChistory "     <<    keepFullMChistory << endl;
 
    if (mPset.exists("cleanGenJets")) cleanGenJets = mPset.getParameter<bool>("cleanGenJets");
    else                              cleanGenJets = false;

    if (mPset.exists("UseElMVA")) UseElMVA = mPset.getParameter<bool>("UseElMVA");
    else                          UseElMVA = false;

    if (mPset.exists("doElSCMETCorr")) doElSCMETCorr = mPset.getParameter<bool>("doElSCMETCorr");
    else                               doElSCMETCorr = false;
    if (mPset.exists("electronCollection"))      elec_it = mPset.getParameter<edm::InputTag>("electronCollection");
    else                                         elec_it = edm::InputTag("slimmedElectrons");

    if (mPset.exists("OverrideLHEWeights")) orlhew = mPset.getParameter<bool>("OverrideLHEWeights");
    else                                   orlhew = false;
    if (mPset.exists("basePDFname")) basePDFname = mPset.getParameter<std::string>("basePDFname");
    else                             basePDFname = "cteq6";
    if (mPset.exists("newPDFname"))  newPDFname = mPset.getParameter<std::string>("newPDFname");
    else                             newPDFname = "PDF4LHC15_nlo_mc_pdfas";
    if (orlhew) {
        cout << "Overriding LHE weights, using "<<newPDFname<<" as new and "<<basePDFname<<" as base PDF set." << endl;
        LHAPDF::Info& cfg = LHAPDF::getConfig();
        cfg.set_entry("Verbosity", 0);

    }
    else cout << "Writing LHE weights (no override)." << endl;

    if (mPset.exists("saveLooseLeps")) saveLooseLeps = mPset.getParameter<bool>("saveLooseLeps");
    else                               saveLooseLeps = false;

    if (mPset.exists("doAllJetSyst"))  doAllJetSyst = mPset.getParameter<bool>("doAllJetSyst");
    else                               doAllJetSyst = false;

    return 0;
}

int singleLepCalc::AnalyzeEvent(edm::EventBase const & event, BaseEventSelector * selector)
{     // ----- Get objects from the selector -----
    std::vector<edm::Ptr<pat::Jet> >            const & vSelJets = selector->GetSelectedJets();
    std::vector<pat::Jet>                       const & vSelCorrJets = selector->GetSelectedCorrJets();
    std::vector<TLorentzVector>                 const & vSelCorrJets_jesup = selector->GetSelectedCorrJets_jesup();
    std::vector<TLorentzVector>                 const & vSelCorrJets_jesdn = selector->GetSelectedCorrJets_jesdn();
    std::vector<TLorentzVector>                 const & vSelCorrJets_jerup = selector->GetSelectedCorrJets_jerup();
    std::vector<TLorentzVector>                 const & vSelCorrJets_jerdn = selector->GetSelectedCorrJets_jerdn();
    std::vector<edm::Ptr<pat::Jet> >            const & vSelBtagJets = selector->GetSelectedBtagJets();
    std::vector<edm::Ptr<pat::Jet> >            const & vAllJets = selector->GetAllJets();
    std::vector<std::pair<TLorentzVector,bool>> const & vCorrBtagJets = selector->GetCorrJetsWithBTags();
    std::vector<edm::Ptr<pat::Muon> >           const & vSelectedMuons = selector->GetSelectedMuons();
    std::vector<edm::Ptr<pat::Electron> >       const & vSelectedElectrons = selector->GetSelectedElectrons();
    std::vector<edm::Ptr<pat::Muon> >           const & vLooseMuons = selector->GetLooseMuons();
    std::vector<edm::Ptr<pat::Electron> >       const & vLooseElectrons = selector->GetLooseElectrons();
    edm::Ptr<pat::MET>                          const & pMet = selector->GetMet();
    std::map<std::string, unsigned int>         const & mSelMCTriggersEl = selector->GetSelectedMCTriggersEl();
    std::map<std::string, unsigned int>         const & mSelTriggersEl = selector->GetSelectedTriggersEl();
    std::map<std::string, unsigned int>         const & mSelMCTriggersMu = selector->GetSelectedMCTriggersMu();
    std::map<std::string, unsigned int>         const & mSelTriggersMu = selector->GetSelectedTriggersMu();

    std::vector<edm::Ptr<pat::Muon> > vSelMuons;
    std::vector<edm::Ptr<pat::Electron> > vSelElectrons;
    if(saveLooseLeps){
      vSelMuons = vLooseMuons;
      vSelElectrons = vLooseElectrons;
    }else{
      vSelMuons = vSelectedMuons;
      vSelElectrons = vSelectedElectrons;
    }

    // ----- Event kinematics -----
    int _nAllJets        = (int)vAllJets.size();
    int _nSelJets        = (int)vSelJets.size();
    int _nSelBtagJets    = (int)vSelBtagJets.size();
    int _nCorrBtagJets   = (int)vCorrBtagJets.size();
    int _nSelMuons       = (int)vSelMuons.size();
    int _nSelElectrons   = (int)vSelElectrons.size();

    edm::Handle<std::vector<reco::Vertex> > pvHandle;
    event.getByLabel(pvCollection_it, pvHandle);
    goodPVs = *(pvHandle.product());

    SetValue("nPV", (int)goodPVs.size());

    int NumTrueInts = -1;
    int NumPUInts = -1;
    if(isMc){
      edm::Handle<std::vector<PileupSummaryInfo>> PupInfo;
      if(!event.getByLabel(edm::InputTag("addPileupInfo"), PupInfo)){
	event.getByLabel(edm::InputTag("slimmedAddPileupInfo"), PupInfo);
      }
      
      for(std::vector<PileupSummaryInfo>::const_iterator PVI = PupInfo->begin(); PVI != PupInfo->end(); ++PVI){
	int BX = PVI->getBunchCrossing();
	if(BX == 0){
	  NumTrueInts = PVI->getTrueNumInteractions();
	  NumPUInts = PVI->getPU_NumInteractions();
	}
      }
    }
    SetValue("nTrueInteractions",NumTrueInts);
    SetValue("nPileupInteractions",NumPUInts);

     // _____ Primary dataset (from python cfg) _____________________
     //
     //   
    int dataE  = 0;
    int dataM  = 0;
 
    if      (dataType == "E" or dataType == "Electron") dataE = 1;
    else if (dataType == "M" or dataType == "Muon") dataM = 1;
    else if (dataType == "All" or dataType == "ALL") {
        dataE = 1; dataM = 1; 
    }

    SetValue("dataE", dataE);
    SetValue("dataM", dataM);


 
    // Trigger
    std::vector<std::string> vsSelMCTriggersEl, vsSelTriggersEl, vsSelMCTriggersMu, vsSelTriggersMu;
    std::vector<int> viSelMCTriggersEl, viSelTriggersEl, viSelMCTriggersMu, viSelTriggersMu;
    for(std::map<std::string, unsigned int>::const_iterator j = mSelMCTriggersEl.begin(); j != mSelMCTriggersEl.end();j++) {
        vsSelMCTriggersEl.push_back(j->first);
        viSelMCTriggersEl.push_back((int)(j->second));
    }
    for(std::map<std::string, unsigned int>::const_iterator j = mSelTriggersEl.begin(); j != mSelTriggersEl.end();j++) {
        vsSelTriggersEl.push_back(j->first);
        viSelTriggersEl.push_back((int)(j->second));
    }
    for(std::map<std::string, unsigned int>::const_iterator j = mSelMCTriggersMu.begin(); j != mSelMCTriggersMu.end();j++) {
        vsSelMCTriggersMu.push_back(j->first);
        viSelMCTriggersMu.push_back((int)(j->second));
    }
    for(std::map<std::string, unsigned int>::const_iterator j = mSelTriggersMu.begin(); j != mSelTriggersMu.end();j++) {
        vsSelTriggersMu.push_back(j->first);
        viSelTriggersMu.push_back((int)(j->second));
    }
    SetValue("vsSelMCTriggersEl", vsSelMCTriggersEl);
    SetValue("vsSelTriggersEl", vsSelTriggersEl);
    SetValue("vsSelMCTriggersMu", vsSelMCTriggersMu);
    SetValue("vsSelTriggersMu", vsSelTriggersMu);
    SetValue("viSelMCTriggersEl", viSelMCTriggersEl);
    SetValue("viSelTriggersEl", viSelTriggersEl);
    SetValue("viSelMCTriggersMu", viSelMCTriggersMu);
    SetValue("viSelTriggersMu", viSelTriggersMu);

 
    std::vector< TLorentzVector > vGenLep;
    TLorentzVector tmpLV;

    //rho source for miniIso
    edm::Handle<double> rhoJetsNC;
    event.getByLabel(edm::InputTag("fixedGridRhoFastjetCentralNeutral","") , rhoJetsNC);
    double myRhoJetsNC = *rhoJetsNC;
    double _rhoNC = myRhoJetsNC;

    // Muon
   
    std::vector <int> muCharge;
    std::vector <int> muGlobal;
    //Four std::vector
    std::vector <double> muPt;
    std::vector <double> muEta;
    std::vector <double> muPhi;
    std::vector <double> muEnergy;
    //Three InnerTrack std::vector
    std::vector <double> muInnerPt;
    std::vector <double> muInnerEta;
    std::vector <double> muInnerPhi;
    //Quality criteria
    std::vector <double> muChi2;
    std::vector <double> muDxy;
    std::vector <double> muDz;
    std::vector <double> muRelIso;
    std::vector <double> muMiniIso;
    std::vector <double> muMiniIsoDB;

    std::vector <int> muNValMuHits;
    std::vector <int> muNMatchedStations;
    std::vector <int> muNValPixelHits;
    std::vector <int> muNTrackerLayers;
    //Extra info about isolation
    std::vector <double> muChIso;
    std::vector <double> muNhIso;
    std::vector <double> muGIso;
    std::vector <double> muPuIso;
    //ID info
    std::vector <int> muIsTight;
    std::vector <int> muIsMedium;
    std::vector<int> muIsLoose;

    //Generator level information -- MC matching
    std::vector<double> muGen_Reco_dr;
    std::vector<int> muPdgId;
    std::vector<int> muStatus;
    std::vector<int> muMatched;
    std::vector<int> muNumberOfMothers;
    std::vector<double> muMother_pt;
    std::vector<double> muMother_eta;
    std::vector<double> muMother_phi;
    std::vector<double> muMother_energy;
    std::vector<int> muMother_id;
    std::vector<int> muMother_status;
    //Matched gen muon information:
    std::vector<double> muMatchedPt;
    std::vector<double> muMatchedEta;
    std::vector<double> muMatchedPhi;
    std::vector<double> muMatchedEnergy;

    for (std::vector<edm::Ptr<pat::Muon> >::const_iterator imu = vSelMuons.begin(); imu != vSelMuons.end(); imu++) {
        //Protect against muons without tracks (should never happen, but just in case)
        //if ((*imu)->globalTrack().isNonnull() and (*imu)->globalTrack().isAvailable() and
        //    (*imu)->innerTrack().isNonnull()  and (*imu)->innerTrack().isAvailable()){
        //Protect against muons without inner tracks (should never happen, but just in case)
        if ((*imu)->innerTrack().isNonnull()  and (*imu)->innerTrack().isAvailable()){
            bool validGlob = (*imu)->globalTrack().isNonnull() && (*imu)->globalTrack().isAvailable();

	    if ((*imu)->genParticle()!=0) {
	        tmpLV.SetPtEtaPhiE((*imu)->genParticle()->pt(),(*imu)->genParticle()->eta(),(*imu)->genParticle()->phi(),(*imu)->genParticle()->energy());
                vGenLep.push_back(tmpLV);
	    }

            bool goodGlob = false;
            if (validGlob) {
                if ((*imu)->isGlobalMuon() &&
                (*imu)->globalTrack()->normalizedChi2() < 3 && 
                (*imu)->combinedQuality().chi2LocalPosition < 12 && 
                (*imu)->combinedQuality().trkKink < 20) goodGlob = true;
            }
            bool ismediummuon = (*imu)->isLooseMuon() &&
                (*imu)->innerTrack()->validFraction() > 0.49 && 
                (*imu)->segmentCompatibility() > (goodGlob ? 0.303 : 0.451); 

            //charge
            muCharge.push_back((*imu)->charge());
            // 4-std::vector 
            muPt     . push_back((*imu)->pt());
            muEta    . push_back((*imu)->eta());
            muPhi    . push_back((*imu)->phi());
            muEnergy . push_back((*imu)->energy());

            muInnerPt     . push_back((*imu)->innerTrack()->pt());
            muInnerEta    . push_back((*imu)->innerTrack()->eta());
            muInnerPhi    . push_back((*imu)->innerTrack()->phi());

            muIsTight.push_back((*imu)->isTightMuon(goodPVs.at(0)));
            muIsMedium.push_back(ismediummuon);
            muIsLoose.push_back((*imu)->isLooseMuon());

            muGlobal.push_back(((*imu)->isGlobalMuon()<<2)+(*imu)->isTrackerMuon());
            //chi2
            if (validGlob) muChi2 . push_back((*imu)->globalTrack()->normalizedChi2());
            else muChi2 . push_back(999999.);
            //Isolation
            double chIso  = (*imu)->userIsolation(pat::PfChargedHadronIso);
            double nhIso  = (*imu)->userIsolation(pat::PfNeutralHadronIso);
            double gIso   = (*imu)->userIsolation(pat::PfGammaIso);
            double puIso  = (*imu)->userIsolation(pat::PfPUChargedHadronIso);
            double relIso = (chIso + std::max(0.,nhIso + gIso - 0.5*puIso)) / (*imu)->pt();

            edm::Handle<pat::PackedCandidateCollection> packedPFCands;
            event.getByLabel(packedPFCandsLabel_, packedPFCands);
            double miniIso = getPFMiniIsolation_EffectiveArea(packedPFCands, dynamic_cast<const reco::Candidate *>(imu->get()), 0.05, 0.2, 10., false, false,myRhoJetsNC);
            double miniIsoDB = getPFMiniIsolation_DeltaBeta(packedPFCands, dynamic_cast<const reco::Candidate *>(imu->get()), 0.05, 0.2, 10., false);

            muRelIso . push_back(relIso);
            muMiniIso . push_back(miniIso);
            muMiniIsoDB . push_back(miniIsoDB);
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
            if (validGlob) muNValMuHits       . push_back((*imu)->globalTrack()->hitPattern().numberOfValidMuonHits());
            else muNValMuHits       . push_back(-999999);
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
    SetValue("muGlobal", muGlobal);
    SetValue("muPt"     , muPt);
    SetValue("muEta"    , muEta);
    SetValue("muPhi"    , muPhi);
    SetValue("muInnerPt"     , muInnerPt);
    SetValue("muInnerEta"    , muInnerEta);
    SetValue("muInnerPhi"    , muInnerPhi);
    SetValue("muEnergy" , muEnergy);
    SetValue("muIsTight", muIsTight);
    SetValue("muIsMedium", muIsMedium);
    SetValue("muIsLoose",muIsLoose); 
    //Quality criteria
    SetValue("muChi2"   , muChi2);
    SetValue("muDxy"    , muDxy);
    SetValue("muDz"     , muDz);
    SetValue("muRelIso" , muRelIso);
    SetValue("muMiniIso", muMiniIso);
    SetValue("muMiniIsoDB", muMiniIsoDB);

    SetValue("muNValMuHits"       , muNValMuHits);
    SetValue("muNMatchedStations" , muNMatchedStations);
    SetValue("muNValPixelHits"    , muNValPixelHits);
    SetValue("muNTrackerLayers"   , muNTrackerLayers);
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




    // Electron
    //Four std::vector
    std::vector <double> elPt;
    std::vector <double> elEta;
    std::vector <double> elPFEta;
    std::vector <double> elPhi;
    std::vector <double> elSCE;
    std::vector <double> elPFPhi;
    std::vector <double> elEnergy;

    std::vector <double> elEtaVtx;
    std::vector <double> elPhiVtx;
    std::vector <double> elDEtaSCTkAtVtx;
    std::vector <double> elDPhiSCTkAtVtx;

    //Quality criteria
    std::vector <double> elRelIso;
    std::vector <double> elMiniIso;
    std::vector <double> elDxy;
    std::vector <int>    elNotConversion;
    std::vector <int>    elChargeConsistent;
    std::vector <int>    elIsEBEE;
    std::vector <int>    elCharge;
    std::vector <int>    elGsfCharge;
    std::vector <int>    elCtfCharge;
    std::vector <int>    elScPixCharge;

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

    std::vector <double> elMVAValue;
    std::vector <double> elMVAValue_alt;
 
    //Extra info about isolation
    std::vector <double> elChIso;
    std::vector <double> elNhIso;
    std::vector <double> elPhIso;
    std::vector <double> elAEff;
    std::vector <double> elRhoIso;
    std::vector <double> elEcalPFClusterIso;
    std::vector <double> elHcalPFClusterIso;
    std::vector <double> elDR03TkSumPt;

    //mother-information
    //Generator level information -- MC matching
    std::vector<double> elGen_Reco_dr;
    std::vector<int> elPdgId;
    std::vector<int> elStatus;
    std::vector<int> elMatched;
    std::vector<int> elNumberOfMothers;
    std::vector<double> elMother_pt;
    std::vector<double> elMother_eta;
    std::vector<double> elMother_phi;
    std::vector<double> elMother_energy;
    std::vector<int> elMother_id;
    std::vector<int> elMother_status;
    //Matched gen electron information:
    std::vector<double> elMatchedPt;
    std::vector<double> elMatchedEta;
    std::vector<double> elMatchedPhi;
    std::vector<double> elMatchedEnergy;

    edm::Handle<double> rhoHandle;
    event.getByLabel(rhoSrc_, rhoHandle);
    double rhoIso = std::max(*(rhoHandle.product()), 0.0);

    //
    //_____Electrons______
    //

    for (std::vector<edm::Ptr<pat::Electron> >::const_iterator iel = vSelElectrons.begin(); iel != vSelElectrons.end(); iel++){
        //Protect against electrons without tracks (should never happen, but just in case)
        if ((*iel)->gsfTrack().isNonnull() and (*iel)->gsfTrack().isAvailable()){

	    if ((*iel)->genParticle()!=0) {
                tmpLV.SetPtEtaPhiE((*iel)->genParticle()->pt(),(*iel)->genParticle()->eta(),(*iel)->genParticle()->phi(),(*iel)->genParticle()->energy());
                vGenLep.push_back(tmpLV);
            }

            //Four std::vector
            elPt     . push_back((*iel)->pt()); //Must check: why ecalDrivenMomentum?
            elEta    . push_back((*iel)->superCluster()->eta());
            elPFEta  . push_back((*iel)->eta());
            elPhi    . push_back((*iel)->superCluster()->phi());
            elSCE    . push_back((*iel)->superCluster()->energy());
            elPFPhi  . push_back((*iel)->phi());
            elEnergy . push_back((*iel)->energy());

	    elEtaVtx.push_back((*iel)->trackMomentumAtVtxWithConstraint().Eta());
	    elPhiVtx.push_back((*iel)->trackMomentumAtVtxWithConstraint().Phi());
	    elDEtaSCTkAtVtx.push_back((*iel)->deltaEtaSuperClusterTrackAtVtx());
	    elDPhiSCTkAtVtx.push_back((*iel)->deltaPhiSuperClusterTrackAtVtx());

            //Isolation
            double scEta = (*iel)->superCluster()->eta();
            double AEff;
            if(fabs(scEta) >2.4) AEff = 0.2687;
            else if(fabs(scEta) >2.3) AEff = 0.2243;
            else if(fabs(scEta) >2.2) AEff = 0.1903;
            else if(fabs(scEta) >2.0) AEff = 0.1534;
            else if(fabs(scEta) >1.479) AEff = 0.1411;
            else if(fabs(scEta) >0.1) AEff = 0.1862;
            else AEff = 0.1752;
  
            double chIso = ((*iel)->pfIsolationVariables()).sumChargedHadronPt;
            double nhIso = ((*iel)->pfIsolationVariables()).sumNeutralHadronEt;
            double phIso = ((*iel)->pfIsolationVariables()).sumPhotonEt;
            double relIso = ( chIso + max(0.0, nhIso + phIso - rhoIso*AEff) ) / (*iel)->pt();

            elChIso  . push_back(chIso);
            elNhIso  . push_back(nhIso);
            elPhIso  . push_back(phIso);
            elAEff   . push_back(AEff);
            elRhoIso . push_back(rhoIso);

	    elEcalPFClusterIso.push_back((*iel)->ecalPFClusterIso());
	    elHcalPFClusterIso.push_back((*iel)->hcalPFClusterIso());
	    elDR03TkSumPt.push_back((*iel)->dr03TkSumPt());

            edm::Handle<pat::PackedCandidateCollection> packedPFCands;
            event.getByLabel(packedPFCandsLabel_, packedPFCands);
            double miniIso = getPFMiniIsolation_EffectiveArea(packedPFCands, dynamic_cast<const reco::Candidate *>(iel->get()), 0.05, 0.2, 10., false, false,myRhoJetsNC);

            elRelIso . push_back(relIso);
            elMiniIso . push_back(miniIso);

	    //get three different charges
	    elGsfCharge.push_back( (*iel)->gsfTrack()->charge());
	    if( (*iel)->closestCtfTrackRef().isNonnull()) elCtfCharge.push_back((*iel)->closestCtfTrackRef()->charge());
	    else elCtfCharge.push_back(-999);
	    elScPixCharge.push_back((*iel)->scPixCharge());
	    elCharge.push_back((*iel)->charge());

            //IP: for some reason this is with respect to the first vertex in the collection
            if(goodPVs.size() > 0){
                elDxy.push_back((-1.0)*(*iel)->gsfTrack()->dxy(goodPVs.at(0).position()));
                elD0.push_back((-1.0)*(*iel)->gsfTrack()->dxy(goodPVs.at(0).position()));
                elDZ.push_back((*iel)->gsfTrack()->dz(goodPVs.at(0).position()));
            } else {
                elDxy.push_back(-999);
                elD0.push_back(-999);
                elDZ.push_back(-999);
            }
            elChargeConsistent.push_back((*iel)->isGsfCtfScPixChargeConsistent());
            elIsEBEE.push_back(((*iel)->isEBEEGap()<<2) + ((*iel)->isEE()<<1) + (*iel)->isEB());
            elDeta.push_back((*iel)->deltaEtaSuperClusterTrackAtVtx());
            elDphi.push_back((*iel)->deltaPhiSuperClusterTrackAtVtx());
            elSihih.push_back((*iel)->full5x5_sigmaIetaIeta());
            elHoE.push_back((*iel)->hcalOverEcal());
            elOoemoop.push_back(fabs(1.0/(*iel)->ecalEnergy() - (*iel)->eSuperClusterOverP()/(*iel)->ecalEnergy()));
            elMHits.push_back((*iel)->gsfTrack()->hitPattern().numberOfHits(reco::HitPattern::MISSING_INNER_HITS));
            elVtxFitConv.push_back((*iel)->passConversionVeto());
            elNotConversion.push_back((*iel)->passConversionVeto());

            if (UseElMVA) {
                elMVAValue.push_back( selector->mvaValue_alt(iel->operator*(),event) );
                elMVAValue_alt.push_back( selector->mvaValue(iel->operator*(),event) );
            }

            if(isMc && keepFullMChistory){
                //cout << "start\n";
                edm::Handle<reco::GenParticleCollection> genParticles;
                event.getByLabel(genParticles_it, genParticles);
                int matchId = findMatch(*genParticles, 11, (*iel)->eta(), (*iel)->phi());
                double closestDR = 10000.;
                //cout << "matchId "<<matchId <<endl;
                if (matchId>=0) {
                    const reco::GenParticle & p = (*genParticles).at(matchId);
                    closestDR = mdeltaR( (*iel)->eta(), (*iel)->phi(), p.eta(), p.phi());
                    //cout << "closestDR "<<closestDR <<endl;
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

    //Four std::vector
    SetValue("elPt"     , elPt);
    SetValue("elEta"    , elEta);
    SetValue("elPFEta"  , elPFEta);
    SetValue("elPhi"    , elPhi);
    SetValue("elSCE"    , elSCE);
    SetValue("elPFPhi"  , elPFPhi);
    SetValue("elEnergy" , elEnergy);

    SetValue("elEtaVtx" , elEtaVtx);
    SetValue("elPhiVtx" , elPhiVtx);
    SetValue("elDEtaSCTkAtVtx" , elDEtaSCTkAtVtx);
    SetValue("elDPhiSCTkAtVtx" , elDPhiSCTkAtVtx);

    SetValue("elCharge", elCharge);
    SetValue("elGsfCharge", elGsfCharge);
    SetValue("elCtfCharge", elCtfCharge);
    SetValue("elScPixCharge", elScPixCharge);
    //Quality requirements
    SetValue("elRelIso" , elRelIso); //Isolation
    SetValue("elMiniIso" , elMiniIso); //Mini Isolation
    SetValue("elDxy"    , elDxy);    //Dxy
    SetValue("elNotConversion" , elNotConversion);  //Conversion rejection
    SetValue("elChargeConsistent", elChargeConsistent);
    SetValue("elIsEBEE", elIsEBEE);

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

    SetValue("elMVAValue", elMVAValue);
    SetValue("elMVAValue_alt", elMVAValue_alt);

    //Extra info about isolation
    SetValue("elChIso" , elChIso);
    SetValue("elNhIso" , elNhIso);
    SetValue("elPhIso" , elPhIso);
    SetValue("elAEff"  , elAEff);
    SetValue("elRhoIso", elRhoIso);
    SetValue("elEcalPFClusterIso", elEcalPFClusterIso);
    SetValue("elHcalPFClusterIso", elHcalPFClusterIso);
    SetValue("elDR03TkSumPt", elDR03TkSumPt);

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
    //______Trigger Matching __________________
    //

    edm::Handle<edm::TriggerResults > mhEdmTriggerResults;
    event.getByLabel( triggerCollection_ , mhEdmTriggerResults );
    edm::Handle<pat::TriggerObjectStandAloneCollection> mhEdmTriggerObjectColl;  
    event.getByLabel(triggerSummary_,mhEdmTriggerObjectColl);

    const edm::TriggerNames &names = event.triggerNames(*mhEdmTriggerResults);

    std::vector<int> _electron_1_hltmatched;
    std::vector<int> _muon_1_hltmatched;

    bool trigmatched;
    if ( elEta.size()>0 ){
        for (unsigned int iel = 0; iel < elTrigMatchFilters.size(); iel++) {
            trigmatched = false;
            for(pat::TriggerObjectStandAlone obj : *mhEdmTriggerObjectColl){       
                obj.unpackPathNames(names);
                for(unsigned h = 0; h < obj.filterLabels().size(); ++h){
                    if ( obj.filterLabels()[h]!=elTrigMatchFilters[iel] ) {
                        continue;
                    }
                    if ( deltaR(obj.eta(),obj.phi(),elEta[0],elPhi[0]) < 0.5 ) {
                        _electron_1_hltmatched.push_back(1);
                        trigmatched = true;
                        break;
                    }
	        }
                if (trigmatched) break;
            }
            if (!trigmatched) _electron_1_hltmatched.push_back(0);
        }
    }
    if ( muEta.size()>0 ){
        for (unsigned int imu = 0; imu < muTrigMatchFilters.size(); imu++) {
            trigmatched = false;
            for(pat::TriggerObjectStandAlone obj : *mhEdmTriggerObjectColl){       
                obj.unpackPathNames(names);
                for(unsigned h = 0; h < obj.filterLabels().size(); ++h){
                    if ( obj.filterLabels()[h]!=muTrigMatchFilters[imu] ) {
                        continue;
                    }
	            if ( deltaR(obj.eta(),obj.phi(),muEta[0],muPhi[0]) < 0.5 ) {
                        _muon_1_hltmatched.push_back(1);
                        trigmatched = true;
                        break;
                    }
	        }
                if (trigmatched) break;
	    }
            if (!trigmatched) _muon_1_hltmatched.push_back(0);
        }
    }

    SetValue("electron_hltfilters",elTrigMatchFilters);
    SetValue("muon_hltfilters",muTrigMatchFilters);
    SetValue("electron_1_hltmatched",_electron_1_hltmatched);
    SetValue("muon_1_hltmatched",_muon_1_hltmatched);


    //
    //_____ Jets ______________________________
    //

       
    //Get all AK8 jets (not just for W and Top)
    edm::InputTag AK8JetColl = edm::InputTag("slimmedJetsAK8");
    edm::Handle<std::vector<pat::Jet> > AK8Jets;
    event.getByLabel(AK8JetColl, AK8Jets);

    //Four std::vector
    std::vector <double> AK8JetPt;
    std::vector <double> AK8JetPt_jesup;
    std::vector <double> AK8JetPt_jesdn;
    std::vector <double> AK8JetPt_jerup;
    std::vector <double> AK8JetPt_jerdn;
    std::vector <double> AK8JetEta;
    std::vector <double> AK8JetPhi;
    std::vector <double> AK8JetEnergy;
    std::vector <double> AK8JetEnergy_jesup;
    std::vector <double> AK8JetEnergy_jesdn;
    std::vector <double> AK8JetEnergy_jerup;
    std::vector <double> AK8JetEnergy_jerdn;

    std::vector <double> AK8JetCSV;
    //   std::vector <double> AK8JetRCN;       
    for (std::vector<pat::Jet>::const_iterator ijet = AK8Jets->begin(); ijet != AK8Jets->end(); ijet++){

	// PF Loose
	bool looseJetID = false;
	pat::Jet rawJet = ijet->correctedJet(0);
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

        if (doAllJetSyst) {
            TLorentzVector lvak8_jesup = selector->correctJet(*ijet, event,true,false,1);
            TLorentzVector lvak8_jesdn = selector->correctJet(*ijet, event,true,false,2);
            TLorentzVector lvak8_jerup = selector->correctJet(*ijet, event,true,false,3);
            TLorentzVector lvak8_jerdn = selector->correctJet(*ijet, event,true,false,4);
            AK8JetPt_jesup     . push_back(lvak8_jesup.Pt());
            AK8JetPt_jesdn     . push_back(lvak8_jesdn.Pt());
            AK8JetPt_jerup     . push_back(lvak8_jerup.Pt());
            AK8JetPt_jerdn     . push_back(lvak8_jerdn.Pt());
            AK8JetEnergy_jesup     . push_back(lvak8_jesup.Energy());
            AK8JetEnergy_jesdn     . push_back(lvak8_jesdn.Energy());
            AK8JetEnergy_jerup     . push_back(lvak8_jerup.Energy());
            AK8JetEnergy_jerdn     . push_back(lvak8_jerdn.Energy());
        }

        TLorentzVector lvak8 = selector->correctJet(*ijet, event,true);
        //Four std::vector
        AK8JetPt     . push_back(lvak8.Pt());
        AK8JetEta    . push_back(lvak8.Eta());
        AK8JetPhi    . push_back(lvak8.Phi());
        AK8JetEnergy . push_back(lvak8.Energy());

        AK8JetCSV    . push_back(ijet->bDiscriminator( "pfCombinedInclusiveSecondaryVertexV2BJetTags" ));
        //     AK8JetRCN    . push_back((ijet->chargedEmEnergy()+ijet->chargedHadronEnergy()) / (ijet->neutralEmEnergy()+ijet->neutralHadronEnergy()));
    }
 
    //Four std::vector
    SetValue("AK8JetPt"     , AK8JetPt);
    SetValue("AK8JetPt_jesup"     , AK8JetPt_jesup);
    SetValue("AK8JetPt_jesdn"     , AK8JetPt_jesdn);
    SetValue("AK8JetPt_jerup"     , AK8JetPt_jerup);
    SetValue("AK8JetPt_jerdn"     , AK8JetPt_jerdn);
    SetValue("AK8JetEta"    , AK8JetEta);
    SetValue("AK8JetPhi"    , AK8JetPhi);
    SetValue("AK8JetEnergy" , AK8JetEnergy);
    SetValue("AK8JetEnergy_jesup"     , AK8JetEnergy_jesup);
    SetValue("AK8JetEnergy_jesdn"     , AK8JetEnergy_jesdn);
    SetValue("AK8JetEnergy_jerup"     , AK8JetEnergy_jerup);
    SetValue("AK8JetEnergy_jerdn"     , AK8JetEnergy_jerdn);

    SetValue("AK8JetCSV"    , AK8JetCSV);
    //   SetValue("AK8JetRCN"    , AK8JetRCN);
    //Get AK4 Jets
    //Four std::vector
    std::vector <double> AK4JetPt;
    std::vector <double> AK4JetPt_jesup;
    std::vector <double> AK4JetPt_jesdn;
    std::vector <double> AK4JetPt_jerup;
    std::vector <double> AK4JetPt_jerdn;
    std::vector <double> AK4JetEta;
    std::vector <double> AK4JetPhi;
    std::vector <double> AK4JetEnergy;
    std::vector <double> AK4JetEnergy_jesup;
    std::vector <double> AK4JetEnergy_jesdn;
    std::vector <double> AK4JetEnergy_jerup;
    std::vector <double> AK4JetEnergy_jerdn;

    std::vector <int>    AK4JetBTag;
    std::vector <int>    AK4JetBTag_bSFup;
    std::vector <int>    AK4JetBTag_bSFdn;
    std::vector <int>    AK4JetBTag_lSFup;
    std::vector <int>    AK4JetBTag_lSFdn;
    std::vector <double> AK4JetBDisc;
    std::vector <int>    AK4JetFlav;

    //std::vector <double> AK4JetRCN;   
    double AK4HT =.0;
    for (std::vector<pat::Jet>::const_iterator ii = vSelCorrJets.begin(); ii != vSelCorrJets.end(); ii++){
      int index = (int)(ii-vSelCorrJets.begin());

      AK4JetPt     . push_back(ii->pt());
      AK4JetEta    . push_back(ii->eta());
      AK4JetPhi    . push_back(ii->phi());
      AK4JetEnergy . push_back(ii->energy());

      AK4JetBTag   . push_back(vCorrBtagJets[index].second);
      AK4JetBTag_bSFup.push_back(selector->isJetTagged(*ii, event, true, 1));
      AK4JetBTag_bSFdn.push_back(selector->isJetTagged(*ii, event, true, 2));
      AK4JetBTag_lSFup.push_back(selector->isJetTagged(*ii, event, true, 3));
      AK4JetBTag_lSFdn.push_back(selector->isJetTagged(*ii, event, true, 4));

      //AK4JetRCN    . push_back(((*ijet)->chargedEmEnergy()+(*ijet)->chargedHadronEnergy()) / ((*ijet)->neutralEmEnergy()+(*ijet)->neutralHadronEnergy()));
      AK4JetBDisc  . push_back(ii->bDiscriminator( "pfCombinedInclusiveSecondaryVertexV2BJetTags" ));
      AK4JetFlav   . push_back(abs(ii->hadronFlavour()));

      //HT
      AK4HT += ii->pt(); 
    }
    
    double AK4HT_jesup =.0;
    double AK4HT_jesdn =.0;
    double AK4HT_jerup =.0;
    double AK4HT_jerdn =.0;
    if (doAllJetSyst) {
        for (std::vector<TLorentzVector>::const_iterator ii_jesup = vSelCorrJets_jesup.begin(); ii_jesup != vSelCorrJets_jesup.end(); ii_jesup++){
          AK4JetPt_jesup     . push_back(ii_jesup->Pt());
          AK4JetEnergy_jesup  . push_back(ii_jesup->Energy());
          //HT
          AK4HT_jesup += ii_jesup->Pt(); 
        }
    
        for (std::vector<TLorentzVector>::const_iterator ii_jesdn = vSelCorrJets_jesdn.begin(); ii_jesdn != vSelCorrJets_jesdn.end(); ii_jesdn++){
          AK4JetPt_jesdn     . push_back(ii_jesdn->Pt());
          AK4JetEnergy_jesdn . push_back(ii_jesdn->Energy());
          //HT
          AK4HT_jesdn += ii_jesdn->Pt(); 
        }
    
        for (std::vector<TLorentzVector>::const_iterator ii_jerup = vSelCorrJets_jerup.begin(); ii_jerup != vSelCorrJets_jerup.end(); ii_jerup++){
          AK4JetPt_jerup     . push_back(ii_jerup->Pt());
          AK4JetEnergy_jerup . push_back(ii_jerup->Energy());
          //HT
          AK4HT_jerup += ii_jerup->Pt(); 
        }
    
        for (std::vector<TLorentzVector>::const_iterator ii_jerdn = vSelCorrJets_jerdn.begin(); ii_jerdn != vSelCorrJets_jerdn.end(); ii_jerdn++){
          AK4JetPt_jerdn     . push_back(ii_jerdn->Pt());
          AK4JetEnergy_jerdn . push_back(ii_jerdn->Energy());
          //HT
          AK4HT_jerdn += ii_jerdn->Pt(); 
        }
    }

    //Four std::vector
    SetValue("AK4JetPt"     , AK4JetPt);
    SetValue("AK4JetPt_jesup"     , AK4JetPt_jesup);
    SetValue("AK4JetPt_jesdn"     , AK4JetPt_jesdn);
    SetValue("AK4JetPt_jerup"     , AK4JetPt_jerup);
    SetValue("AK4JetPt_jerdn"     , AK4JetPt_jerdn);
    SetValue("AK4JetEta"    , AK4JetEta);
    SetValue("AK4JetPhi"    , AK4JetPhi);
    SetValue("AK4JetEnergy" , AK4JetEnergy);
    SetValue("AK4JetEnergy_jesup"     , AK4JetEnergy_jesup);
    SetValue("AK4JetEnergy_jesdn"     , AK4JetEnergy_jesdn);
    SetValue("AK4JetEnergy_jerup"     , AK4JetEnergy_jerup);
    SetValue("AK4JetEnergy_jerdn"     , AK4JetEnergy_jerdn);
    SetValue("AK4HT"        , AK4HT);
    SetValue("AK4HT_jesup"     , AK4HT_jesup);
    SetValue("AK4HT_jesdn"     , AK4HT_jesdn);
    SetValue("AK4HT_jerup"     , AK4HT_jerup);
    SetValue("AK4HT_jerdn"     , AK4HT_jerdn);
    SetValue("AK4JetBTag"   , AK4JetBTag);
    SetValue("AK4JetBTag_bSFup"   , AK4JetBTag_bSFup);
    SetValue("AK4JetBTag_bSFdn"   , AK4JetBTag_bSFdn);
    SetValue("AK4JetBTag_lSFup"   , AK4JetBTag_lSFup);
    SetValue("AK4JetBTag_lSFdn"   , AK4JetBTag_lSFdn);
    //SetValue("AK4JetRCN"    , AK4JetRCN);
    SetValue("AK4JetBDisc"  , AK4JetBDisc);
    SetValue("AK4JetFlav"   , AK4JetFlav);

    // MET
    double _met = -9999.0;
    double _met_phi = -9999.0;
    // Corrected MET
    std::vector<double> _corr_met;
    std::vector<double> _corr_met_phi;

    if(pMet.isNonnull() && pMet.isAvailable()) {
        _met = pMet->p4().pt();
        _met_phi = pMet->p4().phi();
            
        for (unsigned int corri = 0; corri<5; corri++) {

            if (!doAllJetSyst && corri>0) {
                _corr_met.push_back(-9999.0);
                _corr_met_phi.push_back(-9999.0);
                continue;
            }

            TLorentzVector corrMET = selector->correctMet(*pMet, event, corri);
    
            if (doElSCMETCorr) {
                TLorentzVector tmpPF, tmpSC;
                edm::Handle< std::vector<pat::Electron> > mhElectrons;
                event.getByLabel( elec_it, mhElectrons );
                //std::cout<<"----------------------------"<<std::endl;
                //std::cout<<"Orig MET: met="<<corrMET.Pt()<<", phi="<<corrMET.Phi()<<std::endl;
                for (std::vector<pat::Electron>::const_iterator _iel = mhElectrons->begin(); _iel != mhElectrons->end(); _iel++){
                    tmpSC.SetPtEtaPhiE(_iel->superCluster()->energy()/TMath::CosH(_iel->superCluster()->eta()),_iel->superCluster()->eta(),_iel->superCluster()->phi(),_iel->superCluster()->energy());
                    //std::cout<<"SC vec: pt="<<tmpSC.Pt()<<", eta="<<tmpSC.Eta()<<", phi="<<tmpSC.Phi()<<", e="<<tmpSC.Energy()<<std::endl;
                    tmpPF.SetPtEtaPhiE(_iel->pt(),_iel->eta(),_iel->phi(),_iel->energy());
                    //std::cout<<"PF vec: pt="<<tmpPF.Pt()<<", eta="<<tmpPF.Eta()<<", phi="<<tmpPF.Phi()<<", e="<<tmpPF.Energy()<<std::endl;
                    if (tmpSC.Pt()==tmpPF.Pt()) continue;
                    corrMET += tmpPF - tmpSC;
                    //std::cout<<"Corr MET: met="<<corrMET.Pt()<<", phi="<<corrMET.Phi()<<std::endl;
                }
            }
    
            if(corrMET.Pt()>0) {
                _corr_met.push_back(corrMET.Pt());
                _corr_met_phi.push_back(corrMET.Phi());
            }
            else{
                _corr_met.push_back(-9999.0);
                _corr_met_phi.push_back(-9999.0);
            }
        }
    }
    SetValue("met", _met);
    SetValue("met_phi", _met_phi);
    SetValue("corr_met", _corr_met[0]);
    SetValue("corr_met_phi", _corr_met_phi[0]);
    SetValue("corr_met_jesup", _corr_met[1]);
    SetValue("corr_met_jesup_phi", _corr_met_phi[1]);
    SetValue("corr_met_jesdn", _corr_met[2]);
    SetValue("corr_met_jesdn_phi", _corr_met_phi[2]);
    SetValue("corr_met_jerup", _corr_met[3]);
    SetValue("corr_met_jerup_phi", _corr_met_phi[3]);
    SetValue("corr_met_jerdn", _corr_met[4]);
    SetValue("corr_met_jerdn_phi", _corr_met_phi[4]);

    double _metnohf = -9999.0;
    double _metnohf_phi = -9999.0;
    // Corrected METNOHF
    double _corr_metnohf = -9999.0;
    double _corr_metnohf_phi = -9999.0;

    edm::InputTag METnoHFColl = edm::InputTag("slimmedMETsNoHF");
    edm::Handle<std::vector<pat::MET> > METnoHF;
    if(event.getByLabel(METnoHFColl, METnoHF)){
      edm::Ptr<pat::MET> metnohf = edm::Ptr<pat::MET>( METnoHF, 0);
      
      if(metnohf.isNonnull() && metnohf.isAvailable()) {
        _metnohf = metnohf->p4().pt();
        _metnohf_phi = metnohf->p4().phi();
        
        TLorentzVector corrMETNOHF = selector->correctMet(*metnohf, event, false);
        //std::cout<<(selector->GetCleanedCorrMet()).Pt()<<std::endl;
        if(corrMETNOHF.Pt()>0) {
	  _corr_metnohf = corrMETNOHF.Pt();
	  _corr_metnohf_phi = corrMETNOHF.Phi();
        }
      }
    }
    
    SetValue("metnohf", _metnohf);
    SetValue("metnohf_phi", _metnohf_phi);
    SetValue("corr_metnohf", _corr_metnohf);
    SetValue("corr_metnohf_phi", _corr_metnohf_phi);

    //_____ Gen Info ______________________________
    //

    //Four std::vector
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
    double HTfromHEPEUP = 0;
    int NPartonsfromHEPEUP = 0;

   //event weights
   std::vector<double> evtWeightsMC;
   float MCWeight=1;
    std::vector<double> LHEweights;
    std::vector<int> LHEweightids;

    std::vector <double> genJetPt;
    std::vector <double> genJetEta;
    std::vector <double> genJetPhi;
    std::vector <double> genJetEnergy;

    //Tau Decay Lepton
    double genTDLPt = -9999.;
    double genTDLEta = -9999.;
    double genTDLPhi = -9999.;
    double genTDLEnergy = -9999.;
    int genTDLID = 0;

    //B Soft Leptons
    std::vector <double> genBSLPt;
    std::vector <double> genBSLEta;
    std::vector <double> genBSLPhi;
    std::vector <double> genBSLEnergy;
    std::vector <int> genBSLID;

    if (isMc){
        //load info for event weight
        edm::Handle<GenEventInfoProduct> genEvtInfo;
        edm::InputTag gen_it("generator");
        event.getByLabel(gen_it, genEvtInfo );

        std::vector<double> evtWeights = genEvtInfo->weights();
        double theWeight = genEvtInfo->weight();
      
        evtWeightsMC=evtWeights;
        MCWeight = theWeight;
        
        if (orlhew) {
          float x1 = genEvtInfo->pdf()->x.first;
          float x2 = genEvtInfo->pdf()->x.second;
          double Q = genEvtInfo->pdf()->scalePDF;
          int id1 = genEvtInfo->pdf()->id.first;
          int id2 = genEvtInfo->pdf()->id.second;
          //std::cout<<x1<<" "<<x2<<" "<<Q<<" "<<id1<<" "<<id2<<std::endl;

          //Initialize PDF sets
          LHAPDF::PDF* basepdf1 = LHAPDF::mkPDF(basePDFname,0);
          const LHAPDF::GridPDF& pdf1 = * dynamic_cast<const LHAPDF::GridPDF*>(basepdf1);
  
          // calculate central PDFs for generator set,
          double pdf1_gen = pdf1.xfxQ(id1, x1, Q);
          double pdf2_gen = pdf1.xfxQ(id2, x2, Q);
          delete basepdf1;
  
          const LHAPDF::PDFSet newset(newPDFname);
          const size_t nmem = newset.size();
          const std::vector<LHAPDF::PDF*> newpdfs = newset.mkPDFs();
          delete (newpdfs[0]);
          for (size_t i = 1; i<nmem; i++) {
            const LHAPDF::GridPDF& pdf2 = * dynamic_cast<const LHAPDF::GridPDF*>(newpdfs[i]);

            double pdf1_new = pdf2.xfxQ(id1, x1, Q);
            double pdf2_new = pdf2.xfxQ(id2, x2, Q);
  
            //std::cout<<"weight"<<i<<" = "<<(pdf1_new*pdf2_new)/(pdf1_gen*pdf2_gen)<<std::endl;
            LHEweights.push_back((pdf1_new*pdf2_new)/(pdf1_gen*pdf2_gen));
            LHEweightids.push_back(2000+i);

            delete (newpdfs[i]);
          }
        }
        else {
  	  edm::Handle<LHEEventProduct> EvtHandle;
  	  edm::InputTag theSrc("externalLHEProducer");
  	  if(event.getByLabel(theSrc,EvtHandle)){
  	  
	    // Save LHE-level HT calculation from quarks:
	    /*
	    for ( unsigned int icount = 0 ; icount < (unsigned int)EvtHandle->hepeup().NUP; icount++ ) {
	      int pdgid = EvtHandle->hepeup().IDUP[icount];
	      int status = EvtHandle->hepeup().ISTUP[icount];
	      int mom1id = abs(EvtHandle->hepeup().IDUP[EvtHandle->hepeup().MOTHUP[icount].first-1]);
	      int mom2id = abs(EvtHandle->hepeup().IDUP[EvtHandle->hepeup().MOTHUP[icount].second-1]);
	      float px = (EvtHandle->hepeup().PUP[icount])[0];
	      float py = (EvtHandle->hepeup().PUP[icount])[1];
	      float pt = sqrt(px*px+py*py);
	      	      
	      if(status==1){
		if(mom1id!=6 && mom2id!=6 && mom1id!=24 && mom2id!=24 && mom1id!=23 && mom2id!=23 && mom1id!=25 && mom2id!=25) {		  
		  HTfromHEPEUP += pt;
		  NPartonsfromHEPEUP++;
		}
	      }
	    }
	    */
  	    // Storing LHE weights https://twiki.cern.ch/twiki/bin/viewauth/CMS/LHEReaderCMSSW
  	    // for MC@NLO renormalization and factorization scale. 
  	    // ID numbers 1001 - 1009. (muR,muF) = 
  	    // 0 = 1001: (1,1)    3 = 1004: (2,1)    6 = 1007: (0.5,1)  
  	    // 1 = 1002: (1,2)    4 = 1005: (2,2)  	 7 = 1008: (0.5,2)  
  	    // 2 = 1003: (1,0.5)  5 = 1006: (2,0.5)	 8 = 1009: (0.5,0.5)
  	    // for PDF variations: ID numbers > 2000
  
  	    std::string weightidstr;
  	    int weightid;
  	    if(EvtHandle->weights().size() > 0){	  
  	      for(unsigned int i = 0; i < EvtHandle->weights().size(); i++){
  	        weightidstr = EvtHandle->weights()[i].id;
  	        weightid = std::stoi(weightidstr);
  	        LHEweights.push_back(EvtHandle->weights()[i].wgt/EvtHandle->originalXWGTUP());
  	        LHEweightids.push_back(weightid);
  	      }
  	    }
  	  }
  	}

        //load genparticles collection
        edm::Handle<reco::GenParticleCollection> genParticles;
        event.getByLabel(genParticles_it, genParticles);

        edm::Handle<std::vector< reco::GenJet> > genJets;
        event.getByLabel(genJets_it, genJets);

	//std::cout << "---------------------------------" << std::endl;
	//std::cout << "\tStatus\tmoment\tmass\tpt\teta\tphi\tID\tMomID\tMomStat\tGMomID\tGMomSt\tGGMomID\tGGMomSt" << std::endl;
	//std::cout << std::endl;
	
        for(size_t i = 0; i < genParticles->size(); i++){
            const reco::GenParticle & p = (*genParticles).at(i);

            bool forceSave = false;
            for (unsigned int ii = 0; ii < keepPDGIDForce.size(); ii++){
                if (abs(p.pdgId()) == (int) keepPDGIDForce.at(ii) && p.status() == (int) keepStatusForce.at(ii)){
                    forceSave = true;
                    break;
                }
            }
            //if (abs(p.pdgId())==11 || abs(p.pdgId())==13 || abs(p.pdgId())==15) {
	    //    std::cout << i << "\t" << p.status() << "\t" << p.p() << "\t" << p.mass() << "\t" << p.pt() << "\t" << p.eta() << "\t" << p.phi() << "\t" << p.pdgId() << "\t";
	    //    if (!(!(p.mother()))) {
            //        std::cout << p.mother()->pdgId() << "\t" << p.mother()->status() << "\t";
	    //        if (!(!(p.mother()->mother()))) std::cout << p.mother()->mother()->pdgId() << "\t" << p.mother()->mother()->status() << "\t";
            //    }
            //}

	    if (p.status() == 1 && (abs(p.pdgId()) == 11 || abs(p.pdgId()) == 13)){
		if (!(!(p.mother()))) {
		    if (!(p.mother()->status()==23 && (abs(p.mother()->pdgId()) == 11 || abs(p.mother()->pdgId()) == 13))) {
                        if (!(!(p.mother()->mother()))) {
			    if (abs(p.mother()->mother()->pdgId()) == 15 && abs(p.mother()->pdgId()) == 15) {
                		genTDLPt     = p.pt();
                		genTDLEta    = p.eta();
                		genTDLPhi    = p.phi();
                		genTDLEnergy = p.energy();
                		genTDLID     = p.pdgId();
                        	//std::cout << "<-- TDL";
			    }
			    else {
                		genBSLPt     . push_back(p.pt());
                		genBSLEta    . push_back(p.eta());
                		genBSLPhi    . push_back(p.phi());
                		genBSLEnergy . push_back(p.energy());
                		genBSLID     . push_back(p.pdgId());
                        	//std::cout << "<-- BSL";
			    }
			}
		    }
		}
	    }
            //if (abs(p.pdgId())==11 || abs(p.pdgId())==13 || abs(p.pdgId())==15) {
	    //    std::cout << std::endl;
            //}

            //Find status 23 particles
            if (p.status() == 23){

                reco::Candidate* mother = (reco::Candidate*) p.mother();
                if (not mother)            continue;

                bool bKeep = false;
                for (unsigned int uk = 0; uk < keepMomPDGID.size(); uk++){
                    if (abs(mother->pdgId()) == (int) keepMomPDGID.at(uk)){
                        bKeep = true;
                        break;
                    }
                }

                if (not bKeep) {
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

                //Four std::vector
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
            else if (forceSave) {
                //Four std::vector
                genPt     . push_back(p.pt());
                genEta    . push_back(p.eta());
                genPhi    . push_back(p.phi());
                genEnergy . push_back(p.energy());

                //Identity
                genID            . push_back(p.pdgId());
                genIndex         . push_back((int) i);
                genStatus        . push_back(p.status());
                genMotherID      . push_back(0);
                genMotherIndex   . push_back(0);
            }
        }//End loop over gen particles
	TLorentzVector tmpJet;
	TLorentzVector tmpLep;
	TLorentzVector tmpCand;
        std::vector<TLorentzVector> tmpVec;
        for(const reco::GenJet &j : *genJets){
            tmpJet.SetPtEtaPhiE(j.pt(),j.eta(),j.phi(),j.energy());
            if (cleanGenJets) {
	        for(size_t k = 0; k < vGenLep.size(); k++){
                    tmpLep = vGenLep[k];
	            for (unsigned int id = 0, nd = j.numberOfDaughters(); id < nd; ++id) {
		        if ( !(j.daughterPtr(id).isNonnull()) ) continue;
		        if ( !(j.daughterPtr(id).isAvailable()) ) continue;
		        const reco::Candidate &_ijet_const = dynamic_cast<const reco::Candidate &>(*j.daughter(id));
                        tmpCand.SetPtEtaPhiE(_ijet_const.pt(),_ijet_const.eta(),_ijet_const.phi(),_ijet_const.energy());
	                if ( tmpLep.DeltaR(tmpCand) < 0.001 && (abs(_ijet_const.pdgId())==13 || abs(_ijet_const.pdgId())==11) ) {//genjet cleaning to mimic reco jet cleaning
			    //std::cout<<"Found overlap:"<<std::endl;
			    //std::cout<<"Gen Lep      : pt = "<<tmpLep.Pt()<<", eta = "<<tmpLep.Eta()<<", phi = "<<tmpLep.Phi()<<std::endl;
			    //std::cout<<"Gen Jet      : pt = "<<tmpJet.Pt()<<", eta = "<<tmpJet.Eta()<<", phi = "<<tmpJet.Phi()<<std::endl;
			    //std::cout<<"Gen Jet Const: pt = "<<tmpCand.Pt()<<", eta = "<<tmpCand.Eta()<<", phi = "<<tmpCand.Phi()<<std::endl;
 		            tmpJet = tmpJet - tmpLep;
 	                }
                        if (abs(_ijet_const.pdgId())==12 || abs(_ijet_const.pdgId())==14 || abs(_ijet_const.pdgId())==16) tmpJet = tmpJet - tmpCand;//temporary fix for neutrinos being included in genjets, can be removed in later releases (i.e. >74X)
		    }
                }
            }
            tmpVec.push_back(tmpJet);
        }
        std::sort(tmpVec.begin(), tmpVec.end(), SortLVByPt);
        for (unsigned int i = 0; i < tmpVec.size(); i++) {
	    //std::cout<<"Gen Jet      : pt = "<<tmpVec.at(i).Pt()<<", eta = "<<tmpVec.at(i).Eta()<<", phi = "<<tmpVec.at(i).Phi()<<std::endl;
            genJetPt     . push_back(tmpVec.at(i).Pt());
            genJetEta    . push_back(tmpVec.at(i).Eta());
            genJetPhi    . push_back(tmpVec.at(i).Phi());
            genJetEnergy . push_back(tmpVec.at(i).Energy());
        }  //End loop over gen jets
    }  //End MC-only if
    // Four std::vector
    SetValue("genPt"    , genPt);
    SetValue("genEta"   , genEta);
    SetValue("genPhi"   , genPhi);
    SetValue("genEnergy", genEnergy);

    // Identity
    SetValue("genID"         , genID);
    SetValue("genIndex"      , genIndex);
    SetValue("genStatus"     , genStatus);
    SetValue("genMotherID"   , genMotherID);
    SetValue("genMotherIndex", genMotherIndex);

    // Four std::vector
    SetValue("genJetPt"    , genJetPt);
    SetValue("genJetEta"   , genJetEta);
    SetValue("genJetPhi"   , genJetPhi);
    SetValue("genJetEnergy", genJetEnergy);

    SetValue("genBSLPt"    , genBSLPt);
    SetValue("genBSLEta"   , genBSLEta);
    SetValue("genBSLPhi"   , genBSLPhi);
    SetValue("genBSLEnergy", genBSLEnergy);
    SetValue("genBSLID"    , genBSLID);

    SetValue("genTDLPt"    , genTDLPt);
    SetValue("genTDLEta"   , genTDLEta);
    SetValue("genTDLPhi"   , genTDLPhi);
    SetValue("genTDLEnergy", genTDLEnergy);
    SetValue("genTDLID"    , genTDLID);

    SetValue("evtWeightsMC", evtWeightsMC);
    SetValue("MCWeight", MCWeight);
    SetValue("LHEweights", LHEweights);
    SetValue("LHEweightids", LHEweightids);
    //    SetValue("HTfromHEPUEP", HTfromHEPEUP);
    //    SetValue("NPartonsfromHEPUEP", NPartonsfromHEPEUP);

    return 0;
}

int singleLepCalc::findMatch(const reco::GenParticleCollection & genParticles, int idToMatch, double eta, double phi)
{
    float dRtmp = 1000;
    float closestDR = 10000.;
    int closestGenPart = -1;

    for(size_t j = 0; j < genParticles.size(); ++ j) {
        const reco::GenParticle & p = (genParticles).at(j);
        if (!(p.statusFlags().isPrompt())) continue;
        dRtmp = mdeltaR( eta, phi, p.eta(), p.phi());
        if ( dRtmp < closestDR && abs(p.pdgId()) == idToMatch){// && dRtmp < 0.3) {
            closestDR = dRtmp;
            closestGenPart = j;
        }//end of requirement for matching
    }//end of gen particle loop 
    return closestGenPart;
}


double singleLepCalc::mdeltaR(double eta1, double phi1, double eta2, double phi2) {
    return std::sqrt(deltaR2 (eta1, phi1, eta2, phi2));
}

void singleLepCalc::fillMotherInfo(const reco::Candidate *mother, int i, std::vector <int> & momid, std::vector <int> & momstatus, std::vector<double> & mompt, std::vector<double> & mometa, std::vector<double> & momphi, std::vector<double> & momenergy)
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
