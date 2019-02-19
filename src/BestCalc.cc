/*

Created:        28 November 2017
Last Updated:    4 December 2017
Justin Pilot
UC Davis
Dan Marley
daniel.edison.marley@cernSPAMNOT.ch
Texas A&M University
-----
BoostedEventShapeTagger
Development of standalone BEST package for analyzers.
Based on the framework created by Justin:
  https://github.com/justinrpilot/BESTAnalysis
Requires MiniAOD inputs to access proper set of information
*/

#include <iostream>
#include <algorithm>
#include "LJMet/Com/interface/BaseCalc.h"
#include "LJMet/Com/interface/LjmetFactory.h"
#include "LJMet/Com/interface/LjmetEventContent.h"
#include "TLorentzVector.h" 
#include "DataFormats/Math/interface/deltaR.h"
#include "LJMet/Com/interface/BestCalc.h"

using namespace std;

class LjmetFactory;


static int reg = LjmetFactory::GetInstance()->Register(new BestCalc(), "BestCalc");



BestCalc::BestCalc(){

}

int BestCalc::BeginJob(){
  if (mPset.exists("numSubjetsMin"))     m_numSubjetsMin = mPset.getParameter<int>("numSubjetsMin");
  else                                   m_numSubjetsMin = 0; 

  if (mPset.exists("numDaughtersMin"))   m_numDaughtersMin = mPset.getParameter<int>("numDaughtersMin");
  else                                   m_numDaughtersMin = 0;

  if (mPset.exists("jetSoftDropMassMin")) m_jetSoftDropMassMin = mPset.getParameter<double>("jetSoftDropMassMin");
  else                                    m_jetSoftDropMassMin = 0;

  if (mPset.exists("jetPtMin"))   m_jetPtMin = mPset.getParameter<double>("jetPtMin");
  else                            m_jetPtMin = 0;

  if (mPset.exists("radiusSmall"))     m_radiusSmall = mPset.getParameter<double>("radiusSmall");
  else                                 m_radiusSmall = 0; 

  if (mPset.exists("radiusLarge")) m_radiusLarge = mPset.getParameter<double>("radiusLarge");
  else                             m_radiusLarge = 0;

  if (mPset.exists("reclusterJetPtMin")) m_reclusterJetPtMin = mPset.getParameter<double>("reclusterJetPtMin");
  else                                   m_reclusterJetPtMin = 0;

  if (mPset.exists("jetChargeKappa")) m_jetChargeKappa = mPset.getParameter<double>("jetChargeKappa");
  else                                m_jetChargeKappa = 0;

  if (mPset.exists("maxJetSize"))     m_maxJetSize = mPset.getParameter<int>("maxJetSize");
  else                                m_maxJetSize = 0; 

  if (mPset.exists("dnnFile"))     m_dnnFile = mPset.getParameter<std::string>("dnnFile");
  else                             m_dnnFile = "None";

  std::ifstream input_cfg( m_dnnFile );                     // original: "data/BEST_mlp.json"
  lwt::JSONConfig cfg = lwt::parse_json( input_cfg );
  m_lwtnn = new lwt::LightweightNeuralNetwork(cfg.inputs, cfg.layers, cfg.outputs);

  return 0;

}


int BestCalc::AnalyzeEvent(edm::EventBase const & event, BaseEventSelector * selector){

  //Get all AK8 jets (not just for W and Top)                                                                                                                                                             
  //edm::InputTag AK8JetColl = edm::InputTag("slimmedJetsAK8");
  //edm::Handle<std::vector<pat::Jet> > AK8Jets;
  //event.getByLabel(AK8JetColl, AK8Jets);

  std::vector<pat::Jet> const & vSelCorrJets_AK8 = selector->GetSelectedCorrJets_AK8();

  //Four std::vector
                                  
  std::vector <double> AK8JetPt;
  std::vector <double> AK8JetEta;
  std::vector <double> AK8JetPhi;
  std::vector <double> AK8JetEnergy;

  std::vector <double> dnn_QCD;
  std::vector <double> dnn_Top;
  std::vector <double> dnn_Higgs;
  std::vector <double> dnn_Z;
  std::vector <double> dnn_W;
  std::vector <double> dnn_B;

  std::vector <double> bDisc;
  std::vector <double> bDisc1;
  std::vector <double> bDisc2;

  std::vector <double> et;
  std::vector <double> eta;
  std::vector <double> mass;
  std::vector <double> SDmass;
  std::vector <double> tau32;
  std::vector <double> tau21;
  std::vector <double> q;

  std::vector <double> m1234_jet;
  std::vector <double> m12_jet;
  std::vector <double> m23_jet;
  std::vector <double> m13_jet;
  
  std::vector <double> m1234top;
  std::vector <double> m12top;
  std::vector <double> m23top;
  std::vector <double> m13top;

  std::vector <double> m1234W;
  std::vector <double> m12W;
  std::vector <double> m23W;
  std::vector <double> m13W;

  std::vector <double> m1234Z;
  std::vector <double> m12Z;
  std::vector <double> m23Z;
  std::vector <double> m13Z;

  std::vector <double> m1234H;
  std::vector <double> m12H;
  std::vector <double> m23H;
  std::vector <double> m13H;

  std::vector <double> pzOverp_top;
  std::vector <double> pzOverp_W;
  std::vector <double> pzOverp_Z;
  std::vector <double> pzOverp_H;
  std::vector <double> pzOverp_jet;

  std::vector <double> Njets_top;
  std::vector <double> Njets_W;
  std::vector <double> Njets_Z;
  std::vector <double> Njets_H;
  std::vector <double> Njets_jet;
  std::vector <double> Njets_orig;

  std::vector <double> FWmoment1top;
  std::vector <double> FWmoment2top;
  std::vector <double> FWmoment3top;
  std::vector <double> FWmoment4top;
  std::vector <double> isotropytop;
  std::vector <double> sphericitytop;
  std::vector <double> aplanaritytop;
  std::vector <double> thrusttop;

  std::vector <double> FWmoment1W;
  std::vector <double> FWmoment2W;
  std::vector <double> FWmoment3W;
  std::vector <double> FWmoment4W;
  std::vector <double> isotropyW;
  std::vector <double> sphericityW;
  std::vector <double> aplanarityW;
  std::vector <double> thrustW;

  std::vector <double> FWmoment1Z;
  std::vector <double> FWmoment2Z;
  std::vector <double> FWmoment3Z;
  std::vector <double> FWmoment4Z;
  std::vector <double> isotropyZ;
  std::vector <double> sphericityZ;
  std::vector <double> aplanarityZ;
  std::vector <double> thrustZ;

  std::vector <double> FWmoment1H;
  std::vector <double> FWmoment2H;
  std::vector <double> FWmoment3H;
  std::vector <double> FWmoment4H;
  std::vector <double> isotropyH;
  std::vector <double> sphericityH;
  std::vector <double> aplanarityH;
  std::vector <double> thrustH;

  std::vector <int>    dnn_largest;

  std::vector <double> AK8JetCSV;

  //   std::vector <double> AK8JetRCN;                                                                                                                                                                    
  //for (std::vector<pat::Jet>::const_iterator ijet = AK8Jets->begin(); ijet != AK8Jets->end(); ijet++){
  for (std::vector<pat::Jet>::const_iterator ii = vSelCorrJets_AK8.begin(); ii != vSelCorrJets_AK8.end(); ii++){

    if(ii->pt() < 170) continue; // not all info there for lower pt                                                                                                                                     
    //pat::Jet corrak8 = 	selector->correctJetReturnPatJet(*ijet, event, true);
    //Four std::vector                                                                                                                                                                                  
    AK8JetPt     . push_back(ii->pt());
    AK8JetEta    . push_back(ii->eta());
    AK8JetPhi    . push_back(ii->phi());
    AK8JetEnergy . push_back(ii->energy());

    AK8JetCSV    . push_back(ii->bDiscriminator( "pfCombinedInclusiveSecondaryVertexV2BJetTags" ));
    //     AK8JetRCN    . push_back((corrak8.chargedEmEnergy()+corrak8.chargedHadronEnergy()) / (corrak8.neutralEmEnergy()+corrak8.neutralHadronEnergy()));

    std::map<std::string,double> myMap;
    std::map<std::string,double> varMap;
    myMap = {
      {"dnn_qcd",  -999},
      {"dnn_top",  -999},
      {"dnn_higgs",-999},
      {"dnn_z",    -999},
      {"dnn_w",    -999},
      {"dnn_b",    -999}
    };
    varMap = {
      {"bDisc",      -999},
      {"bDisc1",     -999},
      {"bDisc2",     -999},
      {"et",         -999},
      {"eta",        -999},
      {"mass",       -999},
      {"SDmass",     -999},
      {"tau32",      -999},
      {"tau21",      -999},
      {"q",          -999},
      {"m1234_jet",  -999},
      {"m12_jet",    -999},
      {"m23_jet",    -999},
      {"m13_jet",    -999},
      {"m1234top",   -999},
      {"m12top",     -999},
      {"m23top",     -999},
      {"m13top",     -999},
      {"m1234W",     -999},
      {"m12W",       -999},
      {"m23W",       -999},
      {"m13W",       -999},
      {"m1234Z",     -999},
      {"m12Z",       -999},
      {"m23Z",       -999},
      {"m13Z",       -999},
      {"m1234H",     -999},
      {"m12H",       -999},
      {"m23H",       -999},
      {"m13H",       -999},
      {"pzOverp_top",-999},
      {"pzOverp_W",  -999},
      {"pzOverp_Z",  -999},
      {"pzOverp_H",  -999},
      {"pzOverp_jet",-999},
      {"Njets_top",      -999},
      {"Njets_W",        -999},
      {"Njets_Z",        -999},
      {"Njets_H",        -999},
      {"Njets_jet",      -999},
      {"Njets_orig",     -999},
      {"FWmoment1top",   -999},
      {"FWmoment2top",   -999},
      {"FWmoment3top",   -999},
      {"FWmoment4top",   -999},
      {"isotropytop",    -999},
      {"sphericitytop",  -999},
      {"aplanaritytop",  -999},
      {"thrusttop",      -999},
      {"FWmoment1W",     -999},
      {"FWmoment2W",     -999},
      {"FWmoment3W",     -999},
      {"FWmoment4W",     -999},
      {"isotropyW",      -999},
      {"sphericityW",    -999},
      {"aplanarityW",    -999},
      {"thrustW",        -999},
      {"FWmoment1Z",     -999},
      {"FWmoment2Z",     -999},
      {"FWmoment3Z",     -999},
      {"FWmoment4Z",     -999},
      {"isotropyZ",      -999},
      {"sphericityZ",    -999},
      {"aplanarityZ",    -999},
      {"thrustZ",        -999},
      {"FWmoment1H",     -999},
      {"FWmoment2H",     -999},
      {"FWmoment3H",     -999},
      {"FWmoment4H",     -999},
      {"isotropyH",      -999},
      {"sphericityH",    -999},
      {"aplanarityH",    -999},
      {"thrustH",        -999}
    };

    std::vector<std::string> labels = ii->subjetCollectionNames();
    if(labels.size() == 0) std::cout << "there are no subjet collection labels" << std::endl;
    // for (unsigned int j = 0; j < labels.size(); j++){
    //   std::cout << labels.at(j) << std::endl;
    // }
    auto const& thisSubjets   = ii->subjets("SoftDropPuppi");

    unsigned int numDaughters = ii->numberOfDaughters();
    float softdropmass = ii->userFloat("ak8PFJetsPuppiSoftDropMass");
    int largest = 10;

    if (thisSubjets.size() >= m_numSubjetsMin && numDaughters >= m_numDaughtersMin && softdropmass >= m_jetSoftDropMassMin){
      varMap = BestCalc::execute(*ii);
      myMap = m_lwtnn->compute(varMap);

      if (myMap["dnn_qcd"] > myMap["dnn_top"] && myMap["dnn_qcd"] > myMap["dnn_higgs"] && myMap["dnn_qcd"] > myMap["dnn_z"] && myMap["dnn_qcd"] > myMap["dnn_w"] && myMap["dnn_qcd"] > myMap["dnn_b"]){
	largest = 0;
      } else if (myMap["dnn_top"] > myMap["dnn_qcd"] && myMap["dnn_top"] > myMap["dnn_higgs"] && myMap["dnn_top"] > myMap["dnn_z"] && myMap["dnn_top"] > myMap["dnn_w"] && myMap["dnn_top"] > myMap["dnn_b"]){
	largest = 1;
      } else if (myMap["dnn_higgs"] > myMap["dnn_top"] && myMap["dnn_higgs"] > myMap["dnn_qcd"] && myMap["dnn_higgs"] > myMap["dnn_z"] && myMap["dnn_higgs"] > myMap["dnn_w"] && myMap["dnn_higgs"] > myMap["dnn_b"]){
	largest = 2;
      } else if (myMap["dnn_z"] > myMap["dnn_top"] && myMap["dnn_z"] > myMap["dnn_higgs"] && myMap["dnn_z"] > myMap["dnn_qcd"] && myMap["dnn_z"] > myMap["dnn_w"] && myMap["dnn_z"] > myMap["dnn_b"]){
	largest = 3;
      } else if (myMap["dnn_w"] > myMap["dnn_top"] && myMap["dnn_w"] > myMap["dnn_higgs"] && myMap["dnn_w"] > myMap["dnn_qcd"] && myMap["dnn_w"] > myMap["dnn_z"] && myMap["dnn_w"] > myMap["dnn_b"]){
	largest = 4;
      } else if (myMap["dnn_b"] > myMap["dnn_top"] && myMap["dnn_b"] > myMap["dnn_higgs"] && myMap["dnn_b"] > myMap["dnn_qcd"] && myMap["dnn_b"] > myMap["dnn_z"] && myMap["dnn_b"] > myMap["dnn_w"]){
        largest = 5;
      } else
	largest = 10;
    }

    bDisc.push_back(varMap["bDisc"]);
    bDisc1.push_back(varMap["bDisc1"]);
    bDisc2.push_back(varMap["bDisc2"]);

    et.push_back(varMap["et"]);
    eta.push_back(varMap["eta"]);
    mass.push_back(varMap["mass"]);
    SDmass.push_back(varMap["SDmass"]);
    tau32.push_back(varMap["tau32"]);
    tau21.push_back(varMap["tau21"]);
    q.push_back(varMap["q"]);

    m1234_jet.push_back(varMap["m1234_jet"]);
    m12_jet.push_back(varMap["m12_jet"]);
    m23_jet.push_back(varMap["m23_jet"]);
    m13_jet.push_back(varMap["m13_jet"]);

    m1234top.push_back(varMap["m1234top"]);
    m12top.push_back(varMap["m12top"]);
    m23top.push_back(varMap["m23top"]);
    m13top.push_back(varMap["m13top"]);

    m1234W.push_back(varMap["m1234W"]);
    m12W.push_back(varMap["m12W"]);
    m23W.push_back(varMap["m23W"]);
    m13W.push_back(varMap["m13W"]);

    m1234Z.push_back(varMap["m1234Z"]);
    m12Z.push_back(varMap["m12Z"]);
    m23Z.push_back(varMap["m23Z"]);
    m13Z.push_back(varMap["m13Z"]);

    m1234H.push_back(varMap["m1234H"]);
    m12H.push_back(varMap["m12H"]);
    m23H.push_back(varMap["m23H"]);
    m13H.push_back(varMap["m13H"]);

    pzOverp_top.push_back(varMap["pzOverp_top"]);
    pzOverp_W.push_back(varMap["pzOverp_W"]);
    pzOverp_Z.push_back(varMap["pzOverp_Z"]);
    pzOverp_H.push_back(varMap["pzOverp_H"]);
    pzOverp_jet.push_back(varMap["pzOverp_jet"]);

    Njets_top.push_back(varMap["Njets_top"]);
    Njets_W.push_back(varMap["Njets_W"]);
    Njets_Z.push_back(varMap["Njets_Z"]);
    Njets_H.push_back(varMap["Njets_H"]);
    Njets_jet.push_back(varMap["Njets_jet"]);
    Njets_orig.push_back(varMap["Njets_orig"]);

    FWmoment1top.push_back(varMap["FWmoment1top"]);
    FWmoment2top.push_back(varMap["FWmoment2top"]);
    FWmoment3top.push_back(varMap["FWmoment3top"]);
    FWmoment4top.push_back(varMap["FWmoment4top"]);
    isotropytop.push_back(varMap["isotropytop"]);
    sphericitytop.push_back(varMap["sphericitytop"]);
    aplanaritytop.push_back(varMap["aplanaritytop"]);
    thrusttop.push_back(varMap["thrusttop"]);

    FWmoment1W.push_back(varMap["FWmoment1W"]);
    FWmoment2W.push_back(varMap["FWmoment2W"]);
    FWmoment3W.push_back(varMap["FWmoment3W"]);
    FWmoment4W.push_back(varMap["FWmoment4W"]);
    isotropyW.push_back(varMap["isotropyW"]);
    sphericityW.push_back(varMap["sphericityW"]);
    aplanarityW.push_back(varMap["aplanarityW"]);
    thrustW.push_back(varMap["thrustW"]);

    FWmoment1Z.push_back(varMap["FWmoment1Z"]);
    FWmoment2Z.push_back(varMap["FWmoment2Z"]);
    FWmoment3Z.push_back(varMap["FWmoment3Z"]);
    FWmoment4Z.push_back(varMap["FWmoment4Z"]);
    isotropyZ.push_back(varMap["isotropyZ"]);
    sphericityZ.push_back(varMap["sphericityZ"]);
    aplanarityZ.push_back(varMap["aplanarityZ"]);
    thrustZ.push_back(varMap["thrustZ"]);

    FWmoment1H.push_back(varMap["FWmoment1H"]);
    FWmoment2H.push_back(varMap["FWmoment2H"]);
    FWmoment3H.push_back(varMap["FWmoment3H"]);
    FWmoment4H.push_back(varMap["FWmoment4H"]);
    isotropyH.push_back(varMap["isotropyH"]);
    sphericityH.push_back(varMap["sphericityH"]);
    aplanarityH.push_back(varMap["aplanarityH"]);
    thrustH.push_back(varMap["thrustH"]);

    dnn_QCD.push_back(myMap["dnn_qcd"]);
    dnn_Top.push_back(myMap["dnn_top"]);
    dnn_Higgs.push_back(myMap["dnn_higgs"]);
    dnn_Z.push_back(myMap["dnn_z"]);
    dnn_W.push_back(myMap["dnn_w"]);
    dnn_B.push_back(myMap["dnn_b"]);

    dnn_largest.push_back(largest);
    
  }

  //Four std::vector                                                                                                                                                                                      
  SetValue("AK8JetPt"     , AK8JetPt);
  SetValue("AK8JetEta"    , AK8JetEta);
  SetValue("AK8JetPhi"    , AK8JetPhi);
  SetValue("AK8JetEnergy" , AK8JetEnergy);
  SetValue("AK8JetCSV"    , AK8JetCSV);
  
  SetValue("dnn_QCD", dnn_QCD);
  SetValue("dnn_Top",dnn_Top);
  SetValue("dnn_Higgs",dnn_Higgs);
  SetValue("dnn_Z",dnn_Z);
  SetValue("dnn_W",dnn_W);
  SetValue("dnn_B",dnn_B);

  SetValue("dnn_largest",dnn_largest);

  SetValue("bDisc",bDisc);
  SetValue("bDisc1",bDisc1);
  SetValue("bDisc2",bDisc2);

  SetValue("et",et);
  SetValue("eta",eta);
  SetValue("mass",mass);
  SetValue("SDmass",SDmass);
  SetValue("tau32",tau32);
  SetValue("tau21",tau21);
  SetValue("q",q);

  SetValue("m1234_jet",m1234_jet);
  SetValue("m12_jet",m12_jet);
  SetValue("m23_jet",m23_jet);
  SetValue("m13_jet",m13_jet);

  SetValue("m1234top",m1234top);
  SetValue("m12top",m12top);
  SetValue("m23top",m23top);
  SetValue("m13top",m13top);

  SetValue("m1234W",m1234W);
  SetValue("m12W",m12W);
  SetValue("m23W",m23W);
  SetValue("m13W",m13W);

  SetValue("m1234Z",m1234Z);
  SetValue("m12Z",m12Z);
  SetValue("m23Z",m23Z);
  SetValue("m13Z",m13Z);

  SetValue("m1234H",m1234H);
  SetValue("m12H",m12H);
  SetValue("m23H",m23H);
  SetValue("m13H",m13H);

  SetValue("pzOverp_top",pzOverp_top);
  SetValue("pzOverp_W",pzOverp_W);
  SetValue("pzOverp_Z",pzOverp_Z);
  SetValue("pzOverp_H",pzOverp_H);
  SetValue("pzOverp_jet",pzOverp_jet);

  SetValue("Njets_top",Njets_top);
  SetValue("Njets_W",Njets_W);
  SetValue("Njets_Z",Njets_Z);
  SetValue("Njets_H",Njets_H);
  SetValue("Njets_jet",Njets_jet);
  SetValue("Njets_orig",Njets_orig);

  SetValue("FWmoment1top",FWmoment1top);
  SetValue("FWmoment2top",FWmoment2top);
  SetValue("FWmoment3top",FWmoment3top);
  SetValue("FWmoment4top",FWmoment4top);
  SetValue("isotropytop",isotropytop);
  SetValue("sphericitytop",sphericitytop);
  SetValue("aplanaritytop",aplanaritytop);
  SetValue("thrusttop",thrusttop);

  SetValue("FWmoment1W",FWmoment1W);
  SetValue("FWmoment2W",FWmoment2W);
  SetValue("FWmoment3W",FWmoment3W);
  SetValue("FWmoment4W",FWmoment4W);
  SetValue("isotropyW",isotropyW);
  SetValue("sphericityW",sphericityW);
  SetValue("aplanarityW",aplanarityW);
  SetValue("thrustW",thrustW);

  SetValue("FWmoment1Z",FWmoment1Z);
  SetValue("FWmoment2Z",FWmoment2Z);
  SetValue("FWmoment3Z",FWmoment3Z);
  SetValue("FWmoment4Z",FWmoment4Z);
  SetValue("isotropyZ",isotropyZ);
  SetValue("sphericityZ",sphericityZ);
  SetValue("aplanarityZ",aplanarityZ);
  SetValue("thrustZ",thrustZ);

  SetValue("FWmoment1H",FWmoment1H);
  SetValue("FWmoment2H",FWmoment2H);
  SetValue("FWmoment3H",FWmoment3H);
  SetValue("FWmoment4H",FWmoment4H);
  SetValue("isotropyH",isotropyH);
  SetValue("sphericityH",sphericityH);
  SetValue("aplanarityH",aplanarityH);
  SetValue("thrustH",thrustH);

  return 0;

}


std::map<std::string,double> BestCalc::execute( const pat::Jet& jet ){
  /* Dan Guest's lightweight DNN framework */
  getJetValues(jet);                            // update m_BESTvars

  // set values (testing)
  /*  m_NNresults = {
    {"dnn_qcd",  0.7},
    {"dnn_top",  0.6},
    {"dnn_higgs",0.5},
    {"dnn_z",    0.4},
    {"dnn_w",    0.3}
    };*/

  //m_NNresults = m_lwtnn->compute(m_BESTvars);

  return m_BESTvars;
}


void BestCalc::getJetValues( const pat::Jet& jet ){
  /* Grab attributes from the jet and store them in map
       Jet requirements:
         pT > 500 GeV
         soft-drop mass > 40 GeV
         >=2 subjets
         >=2 daughters
           daughter->pt() >= 0.05
         m_reclusterJetPtMin = 30 GeV
         maxJets = 4
           sumP, sumPz
           pair-wise invariant mass
  */
  using namespace fastjet;
  typedef reco::Candidate::PolarLorentzVector fourv;

  // clear the map from the previous jet's values
  m_BESTvars.clear();

  // Access the subjets
  auto const& thisSubjets   = jet.subjets("SoftDropPuppi");
  unsigned int numDaughters = jet.numberOfDaughters();

  // Do some checks on the jets and print warnings to the user
  if (thisSubjets.size() < m_numSubjetsMin){
    std::cout << " WARNING :: BEST : Number of subjets, " << thisSubjets.size() << ", is less than " << m_numSubjetsMin << std::endl;
    std::cout << " WARNING :: BEST : -- BEST will NOT run.  Please check your jets! " << std::endl;
  }
  if (numDaughters < m_numDaughtersMin){
    std::cout << " WARNING :: BEST : Number of daughters, " << numDaughters << ", is less than " << m_numDaughtersMin << std::endl;
    std::cout << " WARNING :: BEST : -- BEST will NOT run.  Please check your jets! " << std::endl;
  }
  if ( jet.pt() < m_jetPtMin){
    std::cout << " WARNING :: BEST : The jet pT " << jet.pt() << ", is less than " << m_jetPtMin << std::endl;
    std::cout << " WARNING :: BEST : -- BEST will run, but the results can't be trusted! Please check your jets! " << std::endl;
  }
  if (jet.userFloat("ak8PFJetsPuppiSoftDropMass") < m_jetSoftDropMassMin){
    std::cout << " WARNING :: BEST : The soft-drop mass " << jet.userFloat("ak8PFJetsPuppiSoftDropMass") << ", is less than " << m_jetSoftDropMassMin << std::endl;
    std::cout << " WARNING :: BEST : -- BEST will run, but the results can't be trusted! Please check your jets! " << std::endl;
  }

  // b-tagging
  float btagValue1 = thisSubjets.at(0)->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");
  float btagValue2 = thisSubjets.at(1)->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");

  // n-subjettiness
  float tau1 = jet.userFloat("NjettinessAK8Puppi:tau1");
  float tau2 = jet.userFloat("NjettinessAK8Puppi:tau2");
  float tau3 = jet.userFloat("NjettinessAK8Puppi:tau3");

  // BEST vars
  fourv thisJet = jet.polarP4();

  // Boost to rest frame
  TLorentzVector thisJetLV;
  TLorentzVector thisJetLV_W;
  TLorentzVector thisJetLV_Z;
  TLorentzVector thisJetLV_H;
  TLorentzVector thisJetLV_top;
  thisJetLV.SetPtEtaPhiM(thisJet.Pt(), thisJet.Eta(), thisJet.Phi(), thisJet.M() );
  thisJetLV_W.SetPtEtaPhiM(thisJet.Pt(),   thisJet.Eta(), thisJet.Phi(), m_Wmass ); // W mass [GeV]
  thisJetLV_Z.SetPtEtaPhiM(thisJet.Pt(),   thisJet.Eta(), thisJet.Phi(), m_Zmass ); // Z mass
  thisJetLV_H.SetPtEtaPhiM(thisJet.Pt(),   thisJet.Eta(), thisJet.Phi(), m_Hmass ); // Higgs mass
  thisJetLV_top.SetPtEtaPhiM(thisJet.Pt(), thisJet.Eta(), thisJet.Phi(), m_Tmass ); // Top mass

  std::vector<TLorentzVector> particles_jet;
  std::vector<TLorentzVector> particles_top;
  std::vector<TLorentzVector> particles_W;
  std::vector<TLorentzVector> particles_Z;
  std::vector<TLorentzVector> particles_H;

  std::vector<math::XYZVector> particles2_jet;
  std::vector<math::XYZVector> particles2_top;
  std::vector<math::XYZVector> particles2_W;
  std::vector<math::XYZVector> particles2_Z;
  std::vector<math::XYZVector> particles2_H;

  std::vector<reco::LeafCandidate> particles3_jet;
  std::vector<reco::LeafCandidate> particles3_top;
  std::vector<reco::LeafCandidate> particles3_W;
  std::vector<reco::LeafCandidate> particles3_Z;
  std::vector<reco::LeafCandidate> particles3_H;

  std::vector<fastjet::PseudoJet> topFJparticles;
  std::vector<fastjet::PseudoJet> ZFJparticles;
  std::vector<fastjet::PseudoJet> WFJparticles;
  std::vector<fastjet::PseudoJet> HFJparticles;
  std::vector<fastjet::PseudoJet> topFJparticles_noBoost;
  std::vector<fastjet::PseudoJet> jetFJparticles;
  std::vector<fastjet::PseudoJet> jetFJparticles_noBoost;
  std::vector<fastjet::PseudoJet> jetFJparticles_transformed;

  TVector3 transformedV;

  // Jet charge calculation (from daughters)
  float qxptsum(0.0);                             // jet charge
  float ptsum = pow(jet.pt(), m_jetChargeKappa);  // weighted jet pT

  // loop over daughters
  auto daus(jet.daughterPtrVector());             // load the daughters
  auto daughter0 = jet.daughter(0); //daus.at(0); // First soft drop constituent
  auto daughter1 = jet.daughter(1); //daus.at(1); // Second soft drop constituent
  std::vector<reco::Candidate*> daughtersOfJet;   // store all daughters in one vector

  // access daughters of the first soft drop constituent
  for (unsigned int i=0,size=daughter0->numberOfDaughters(); i<size; i++){
    daughtersOfJet.push_back( (reco::Candidate *) daughter0->daughter(i) );
  }
  // access daughters of the second soft drop constituent
  for (unsigned int i=0,size=daughter1->numberOfDaughters(); i<size; i++){
    daughtersOfJet.push_back( (reco::Candidate *) daughter1->daughter(i));
  }

  // access remaining daughters not retained by in soft drop
  for (unsigned int i=2,size=jet.numberOfDaughters()/*daus.size()*/; i<size; i++){
    daughtersOfJet.push_back( (reco::Candidate*)jet.daughter(i) ); //(reco::Candidate)daus.at(i) );
  }

  for(unsigned int i=0,size=daughtersOfJet.size(); i<size; i++){

    auto daughter = daughtersOfJet.at(i);

    if (daughter->pt() < 0.5) continue;

    float dau_px = daughter->px();
    float dau_py = daughter->py();
    float dau_pz = daughter->pz();
    float dau_e  = daughter->energy();

    TLorentzVector thisParticleLV_jet( dau_px,dau_py,dau_pz,dau_e );
    TLorentzVector thisParticleLV_top( dau_px,dau_py,dau_pz,dau_e );
    TLorentzVector thisParticleLV_W(   dau_px,dau_py,dau_pz,dau_e );
    TLorentzVector thisParticleLV_Z(   dau_px,dau_py,dau_pz,dau_e );
    TLorentzVector thisParticleLV_H(   dau_px,dau_py,dau_pz,dau_e );

    if (daughter->pt() > 1.0)
      qxptsum += daughter->charge() * pow( daughter->pt(), m_jetChargeKappa);


    topFJparticles_noBoost.push_back( PseudoJet( thisParticleLV_top.X(), thisParticleLV_top.Y(), thisParticleLV_top.Z(), thisParticleLV_top.T() ) );
    jetFJparticles_noBoost.push_back( PseudoJet( thisParticleLV_jet.X(), thisParticleLV_jet.Y(), thisParticleLV_jet.Z(), thisParticleLV_jet.T() ) );

    TLorentzVector thisParticleLV_transformed( transformedV.X(), transformedV.Y(), transformedV.Z(), thisParticleLV_jet.E() );

    jetFJparticles_transformed.push_back( PseudoJet( thisParticleLV_transformed.X(), thisParticleLV_transformed.Y(), thisParticleLV_transformed.Z(), thisParticleLV_transformed.T() ) );

    thisParticleLV_jet.Boost( -thisJetLV.BoostVector() );
    thisParticleLV_Z.Boost(   -thisJetLV_Z.BoostVector() );
    thisParticleLV_W.Boost(   -thisJetLV_W.BoostVector() );
    thisParticleLV_H.Boost(   -thisJetLV_H.BoostVector() );
    thisParticleLV_top.Boost( -thisJetLV_top.BoostVector() );

    pboost( thisJetLV_W.Vect(),   thisParticleLV_W.Vect(), thisParticleLV_W);
    pboost( thisJetLV_Z.Vect(),   thisParticleLV_Z.Vect(), thisParticleLV_Z);
    pboost( thisJetLV_top.Vect(), thisParticleLV_top.Vect(), thisParticleLV_top);
    pboost( thisJetLV_H.Vect(),   thisParticleLV_H.Vect(), thisParticleLV_H);

    particles_jet.push_back( thisParticleLV_jet );
    particles_top.push_back( thisParticleLV_top );
    particles_W.push_back(   thisParticleLV_W );
    particles_Z.push_back(   thisParticleLV_Z );
    particles_H.push_back(   thisParticleLV_H );

    topFJparticles.push_back( PseudoJet( thisParticleLV_top.X(), thisParticleLV_top.Y(), thisParticleLV_top.Z(), thisParticleLV_top.T() ) );
    WFJparticles.push_back(   PseudoJet( thisParticleLV_W.X(), thisParticleLV_W.Y(), thisParticleLV_W.Z(), thisParticleLV_W.T() ) );
    ZFJparticles.push_back(   PseudoJet( thisParticleLV_Z.X(), thisParticleLV_Z.Y(), thisParticleLV_Z.Z(), thisParticleLV_Z.T() ) );
    HFJparticles.push_back(   PseudoJet( thisParticleLV_H.X(), thisParticleLV_H.Y(), thisParticleLV_H.Z(), thisParticleLV_H.T() ) );
    jetFJparticles.push_back( PseudoJet( thisParticleLV_jet.X(), thisParticleLV_jet.Y(), thisParticleLV_jet.Z(), thisParticleLV_jet.T() ) );

    particles2_top.push_back( math::XYZVector( thisParticleLV_top.X(), thisParticleLV_top.Y(), thisParticleLV_top.Z() ));
    particles3_top.push_back( reco::LeafCandidate(+1, reco::Candidate::LorentzVector( thisParticleLV_top.X(), thisParticleLV_top.Y(), thisParticleLV_top.Z(), thisParticleLV_top.T()     ) ));
    particles2_W.push_back(   math::XYZVector( thisParticleLV_W.X(), thisParticleLV_W.Y(), thisParticleLV_W.Z() ));
    particles3_W.push_back(   reco::LeafCandidate(+1, reco::Candidate::LorentzVector( thisParticleLV_W.X(), thisParticleLV_W.Y(), thisParticleLV_W.Z(), thisParticleLV_W.T()     ) ));
    particles2_Z.push_back(   math::XYZVector( thisParticleLV_Z.X(), thisParticleLV_Z.Y(), thisParticleLV_Z.Z() ));
    particles3_Z.push_back(   reco::LeafCandidate(+1, reco::Candidate::LorentzVector( thisParticleLV_Z.X(), thisParticleLV_Z.Y(), thisParticleLV_Z.Z(), thisParticleLV_Z.T()     ) ));
    particles2_H.push_back(   math::XYZVector( thisParticleLV_H.X(), thisParticleLV_H.Y(), thisParticleLV_H.Z() ));
    particles3_H.push_back(   reco::LeafCandidate(+1, reco::Candidate::LorentzVector( thisParticleLV_H.X(), thisParticleLV_H.Y(), thisParticleLV_H.Z(), thisParticleLV_H.T()     ) ));
  } // end loop over daughters

  float jetq = qxptsum / ptsum;  // Jet Charge

  // Fox-Wolfram Moments
  double fwm[5]     = {0.0, 0.0 ,0.0 ,0.0, 0.0};
  double fwm_W[5]   = {0.0, 0.0 ,0.0 ,0.0, 0.0};
  double fwm_top[5] = {0.0, 0.0 ,0.0 ,0.0, 0.0};
  double fwm_Z[5]   = {0.0, 0.0 ,0.0 ,0.0, 0.0};
  double fwm_H[5]   = {0.0, 0.0 ,0.0 ,0.0, 0.0};

  FWMoments( particles_W,   fwm_W);
  FWMoments( particles_jet, fwm);
  FWMoments( particles_top, fwm_top);
  FWMoments( particles_Z,   fwm_Z);
  FWMoments( particles_H,   fwm_H);

  // Event Shapes
  EventShapeVariables eventShapes_top( particles2_top );
  EventShapeVariables eventShapes_W( particles2_W );
  EventShapeVariables eventShapes_Z( particles2_Z );
  EventShapeVariables eventShapes_H( particles2_H );

  // Thrust
  Thrust thrustCalculator_top( particles3_top.begin(), particles3_top.end() );
  Thrust thrustCalculator_W( particles3_W.begin(), particles3_W.end() );
  Thrust thrustCalculator_Z( particles3_Z.begin(), particles3_Z.end() );
  Thrust thrustCalculator_H( particles3_H.begin(), particles3_H.end() );

  // Recluster constituents
  JetDefinition jet_def(antikt_algorithm,  m_radiusSmall);
  JetDefinition jet_def2(antikt_algorithm, m_radiusLarge);

  ClusterSequence cs(    topFJparticles, jet_def);
  ClusterSequence cs_W(  WFJparticles,   jet_def);
  ClusterSequence cs_Z(  ZFJparticles,   jet_def);
  ClusterSequence cs_H(  HFJparticles,   jet_def);
  ClusterSequence cs_jet(jetFJparticles, jet_def);
  ClusterSequence cs_noBoost(topFJparticles_noBoost, jet_def2);

  ClusterSequence cs_transformed(jetFJparticles_transformed, jet_def);

  std::vector<PseudoJet> jetsFJ     = sorted_by_pt( cs.inclusive_jets(m_reclusterJetPtMin) );
  std::vector<PseudoJet> jetsFJ_W   = sorted_by_pt( cs_W.inclusive_jets(m_reclusterJetPtMin) );
  std::vector<PseudoJet> jetsFJ_Z   = sorted_by_pt( cs_Z.inclusive_jets(m_reclusterJetPtMin) );
  std::vector<PseudoJet> jetsFJ_H   = sorted_by_pt( cs_H.inclusive_jets(m_reclusterJetPtMin) );
  std::vector<PseudoJet> jetsFJ_jet = sorted_by_pt( cs_jet.inclusive_jets(m_reclusterJetPtMin) );
  std::vector<PseudoJet> jetsFJ_noBoost     = sorted_by_pt( cs_noBoost.inclusive_jets(m_reclusterJetPtMin) );
  std::vector<PseudoJet> jetsFJ_transformed = sorted_by_pt( cs_transformed.inclusive_jets(m_reclusterJetPtMin) );


  // pair-wise invariant masses
  TLorentzVector m1234LV_jet(0.,0.,0.,0.);
  TLorentzVector m1234LV_W(0.,0.,0.,0.);
  TLorentzVector m1234LV_Z(0.,0.,0.,0.);
  TLorentzVector m1234LV_H(0.,0.,0.,0.);
  TLorentzVector m1234LV_top(0.,0.,0.,0.);

  TLorentzVector m12LV_jet(0.,0.,0.,0.);
  TLorentzVector m12LV_W(0.,0.,0.,0.);
  TLorentzVector m12LV_Z(0.,0.,0.,0.);
  TLorentzVector m12LV_H(0.,0.,0.,0.);
  TLorentzVector m12LV_top(0.,0.,0.,0.);

  TLorentzVector m13LV_jet(0.,0.,0.,0.);
  TLorentzVector m13LV_W(0.,0.,0.,0.);
  TLorentzVector m13LV_Z(0.,0.,0.,0.);
  TLorentzVector m13LV_H(0.,0.,0.,0.);
  TLorentzVector m13LV_top(0.,0.,0.,0.);

  TLorentzVector m23LV_jet(0.,0.,0.,0.);
  TLorentzVector m23LV_W(0.,0.,0.,0.);
  TLorentzVector m23LV_Z(0.,0.,0.,0.);
  TLorentzVector m23LV_H(0.,0.,0.,0.);
  TLorentzVector m23LV_top(0.,0.,0.,0.);

  // sum of jet pz and p  Indices = top, W, Z, H, j
  float sumP[5]  = {0.0,0.0,0.0,0.0,0.0};
  float sumPz[5] = {0.0,0.0,0.0,0.0,0.0};

  // -- top
  for (size_t ii=0,size=std::min(m_maxJetSize,jetsFJ.size()); ii<size; ii++){
    sumPz[0] += jetsFJ[ii].pz();
    sumP[0]  += sqrt( jetsFJ[ii].modp2() );
    thisJetLV    = TLorentzVector(jetsFJ[ii].px(), jetsFJ[ii].py(), jetsFJ[ii].pz(), jetsFJ[ii].e());
    m1234LV_top += thisJetLV;
    switch (ii){
    case 0:
      m12LV_top += thisJetLV;
      m13LV_top += thisJetLV;
      break;
    case 1:
      m12LV_top += thisJetLV;
      m23LV_top += thisJetLV;
      break;
    case 2:
      m13LV_top += thisJetLV;
      m23LV_top += thisJetLV;
      break;
    case 3:
      break;
    }
  }

  // -- W jets
  for (size_t ii=0,size=std::min(m_maxJetSize,jetsFJ_W.size()); ii<size; ii++){
    sumPz[1] += jetsFJ_W[ii].pz();
    sumP[1]  += sqrt( jetsFJ_W[ii].modp2() );
    thisJetLV  = TLorentzVector(jetsFJ_W[ii].px(), jetsFJ_W[ii].py(), jetsFJ_W[ii].pz(), jetsFJ_W[ii].e());
    m1234LV_W += thisJetLV;
    switch (ii){
    case 0:
      m12LV_W += thisJetLV;
      m13LV_W += thisJetLV;
      break;
    case 1:
      m12LV_W += thisJetLV;
      m23LV_W += thisJetLV;
      break;
    case 2:
      m13LV_W += thisJetLV;
      m23LV_W += thisJetLV;
      break;
    case 3:
      break;
    }
  }

  // -- Z jets
  for (size_t ii=0,size=std::min(m_maxJetSize,jetsFJ_Z.size()); ii<size; ii++){
    sumPz[2] += jetsFJ_Z[ii].pz();
    sumP[2]  += sqrt( jetsFJ_Z[ii].modp2() );
    thisJetLV  = TLorentzVector(jetsFJ_Z[ii].px(), jetsFJ_Z[ii].py(), jetsFJ_Z[ii].pz(), jetsFJ_Z[ii].e());
    m1234LV_Z += thisJetLV;
    switch (ii){
    case 0:
      m12LV_Z += thisJetLV;
      m13LV_Z += thisJetLV;
      break;
    case 1:
      m12LV_Z += thisJetLV;
      m23LV_Z += thisJetLV;
      break;
    case 2:
      m13LV_Z += thisJetLV;
      m23LV_Z += thisJetLV;
      break;
    case 3:
      break;
    }
  }

  // -- Higgs jets
  for (size_t ii=0,size=std::min(m_maxJetSize,jetsFJ_H.size()); ii<size; ii++){
    sumPz[3] += jetsFJ_H[ii].pz();
    sumP[3]  += sqrt( jetsFJ_H[ii].modp2() );
    thisJetLV  = TLorentzVector(jetsFJ_H[ii].px(), jetsFJ_H[ii].py(), jetsFJ_H[ii].pz(), jetsFJ_H[ii].e());
    m1234LV_H += thisJetLV;
    switch (ii){
    case 0:
      m12LV_H += thisJetLV;
      m13LV_H += thisJetLV;
      break;
    case 1:
      m12LV_H += thisJetLV;
      m23LV_H += thisJetLV;
      break;
    case 2:
      m13LV_H += thisJetLV;
      m23LV_H += thisJetLV;
      break;
    case 3:
      break;
    }
  }

  // -- jets
  for (size_t ii=0,size=std::min(m_maxJetSize,jetsFJ_jet.size()); ii<size; ii++){
    sumPz[4] += jetsFJ_jet[ii].pz();
    sumP[4]  += sqrt( jetsFJ_jet[ii].modp2() );
    thisJetLV    = TLorentzVector(jetsFJ_jet[ii].px(), jetsFJ_jet[ii].py(), jetsFJ_jet[ii].pz(), jetsFJ_jet[ii].e());
    m1234LV_jet += thisJetLV;
    switch (ii){
    case 0:
      m12LV_jet += thisJetLV;
      m13LV_jet += thisJetLV;
      break;
    case 1:
      m12LV_jet += thisJetLV;
      m23LV_jet += thisJetLV;
      break;
    case 2:
      m13LV_jet += thisJetLV;
      m23LV_jet += thisJetLV;
      break;
    case 3:
      break;
    }
  }


  // Update the map with new values
  m_BESTvars["bDisc"]   = (btagValue1 > btagValue2) ? btagValue1 : btagValue2;
  m_BESTvars["bDisc1"]  = btagValue1;
  m_BESTvars["bDisc2"]  = btagValue2;
  m_BESTvars["et"]      = thisJet.Pt();
  m_BESTvars["eta"]     = thisJet.Rapidity();
  m_BESTvars["mass"]    = thisJet.M();
  m_BESTvars["SDmass"]  = jet.userFloat("ak8PFJetsPuppiSoftDropMass");
  m_BESTvars["tau32"]   = (tau2 > 1e-8) ? tau3/tau2 : 999.;
  m_BESTvars["tau21"]   = (tau1 > 1e-8) ? tau2/tau1 : 999.;
  m_BESTvars["q"]       = jetq;

  m_BESTvars["m1234_jet"] = m1234LV_jet.M();
  m_BESTvars["m12_jet"]   = m12LV_jet.M();
  m_BESTvars["m23_jet"]   = m23LV_jet.M();
  m_BESTvars["m13_jet"]   = m13LV_jet.M();

  m_BESTvars["m1234top"] = m1234LV_top.M();
  m_BESTvars["m12top"]   = m12LV_top.M();
  m_BESTvars["m23top"]   = m23LV_top.M();
  m_BESTvars["m13top"]   = m13LV_top.M();

  m_BESTvars["m1234W"] = m1234LV_W.M();
  m_BESTvars["m12W"]   = m12LV_W.M();
  m_BESTvars["m23W"]   = m23LV_W.M();
  m_BESTvars["m13W"]   = m13LV_W.M();

  m_BESTvars["m1234Z"] = m1234LV_Z.M();
  m_BESTvars["m12Z"]   = m12LV_Z.M();
  m_BESTvars["m23Z"]   = m23LV_Z.M();
  m_BESTvars["m13Z"]   = m13LV_Z.M();

  m_BESTvars["m1234H"] = m1234LV_H.M();
  m_BESTvars["m12H"]   = m12LV_H.M();
  m_BESTvars["m23H"]   = m23LV_H.M();
  m_BESTvars["m13H"]   = m13LV_H.M();

  std::vector<std::string> jetNames = {"top","W","Z","H","jet"};

  for (unsigned int pp=0; pp<5; pp++){
    std::string jetName = jetNames[pp];

    m_BESTvars["sumPz_"+jetName]   = sumPz[pp];
    m_BESTvars["sumP_"+jetName]    = sumP[pp];
    m_BESTvars["pzOverp_"+jetName] =  ( sumPz[pp] / (sumP[pp] + 0.0001) ); // not used for 'jet'
  }

  m_BESTvars["Njets_top"]  = jetsFJ.size();
  m_BESTvars["Njets_W"]    = jetsFJ_W.size();
  m_BESTvars["Njets_Z"]    = jetsFJ_Z.size();
  m_BESTvars["Njets_H"]    = jetsFJ_H.size();
  m_BESTvars["Njets_jet"]  = jetsFJ_jet.size();
  m_BESTvars["Njets_orig"] = jetsFJ_noBoost.size();

  // -- top values
  m_BESTvars["FWmoment1top"] = fwm_top[1];
  m_BESTvars["FWmoment2top"] = fwm_top[2];
  m_BESTvars["FWmoment3top"] = fwm_top[3];
  m_BESTvars["FWmoment4top"] = fwm_top[4];
  m_BESTvars["isotropytop"]   = eventShapes_top.isotropy();
  m_BESTvars["sphericitytop"] = eventShapes_top.sphericity();
  m_BESTvars["aplanaritytop"] = eventShapes_top.aplanarity();
  m_BESTvars["thrusttop"]     = thrustCalculator_top.thrust();

  // -- W values
  m_BESTvars["FWmoment1W"] = fwm_W[1];
  m_BESTvars["FWmoment2W"] = fwm_W[2];
  m_BESTvars["FWmoment3W"] = fwm_W[3];
  m_BESTvars["FWmoment4W"] = fwm_W[4];
  m_BESTvars["isotropyW"]   = eventShapes_W.isotropy();
  m_BESTvars["sphericityW"] = eventShapes_W.sphericity();
  m_BESTvars["aplanarityW"] = eventShapes_W.aplanarity();
  m_BESTvars["thrustW"]     = thrustCalculator_W.thrust();

  // -- Z values
  m_BESTvars["FWmoment1Z"] = fwm_Z[1];
  m_BESTvars["FWmoment2Z"] = fwm_Z[2];
  m_BESTvars["FWmoment3Z"] = fwm_Z[3];
  m_BESTvars["FWmoment4Z"] = fwm_Z[4];
  m_BESTvars["isotropyZ"]   = eventShapes_Z.isotropy();
  m_BESTvars["sphericityZ"] = eventShapes_Z.sphericity();
  m_BESTvars["aplanarityZ"] = eventShapes_Z.aplanarity();
  m_BESTvars["thrustZ"]     = thrustCalculator_Z.thrust();

  // -- H values
  m_BESTvars["FWmoment1H"] = fwm_H[1];
  m_BESTvars["FWmoment2H"] = fwm_H[2];
  m_BESTvars["FWmoment3H"] = fwm_H[3];
  m_BESTvars["FWmoment4H"] = fwm_H[4];
  m_BESTvars["isotropyH"]   = eventShapes_H.isotropy();
  m_BESTvars["sphericityH"] = eventShapes_H.sphericity();
  m_BESTvars["aplanarityH"] = eventShapes_H.aplanarity();
  m_BESTvars["thrustH"]     = thrustCalculator_H.thrust();

  return;
}


void BestCalc::pboost( TVector3 pbeam, TVector3 plab, TLorentzVector &pboo ){
  /* Given jet constituent momentum plab, find momentum relative to
       beam direction pbeam
  */
  double pl = plab.Dot(pbeam);
  pl *= 1 / pbeam.Mag();
  // double pt = sqrt(plab.Mag()*plab.Mag()-pl*pl);

  // set x axis direction along pbeam x (0,0,1)
  TVector3 pbx;
  pbx.SetX(pbeam.Y());
  pbx.SetY(pbeam.X());
  pbx.SetZ(0.0);
  pbx *= (1/pbx.Mag());

  // set y axis direction along -pbx x pbeam
  TVector3 pby;
  pby  = -pbx.Cross(pbeam);
  pby *= (1/pby.Mag());

  pboo.SetX(plab.Dot(pbx));
  pboo.SetY(plab.Dot(pby));
  pboo.SetZ(pl);

  return;
}


void BestCalc::FWMoments( std::vector<TLorentzVector> particles, double (&outputs)[5] ){
  /* Fox-Wolfram moments */
  int numParticles = particles.size();

  float s(0.0);
  for(int i = 0; i < numParticles; i++){
    s += particles[i].E();
  }

  float H0(0.0);
  float H4(0.0);
  float H3(0.0);
  float H2(0.0);
  float H1(0.0);

  for (int i=0; i<numParticles; i++){
    for (int j=i; j<numParticles; j++){
      float costh = ( particles[i].Px() * particles[j].Px() + particles[i].Py() * particles[j].Py() + particles[i].Pz() * particles[j].Pz() ) / ( particles[i].P() * particles[j].P() );
      float w1 = particles[i].P();
      float w2 = particles[j].P();

      float fw0 = LegP(costh, 0);
      float fw1 = LegP(costh, 1);
      float fw2 = LegP(costh, 2);
      float fw3 = LegP(costh, 3);
      float fw4 = LegP(costh, 4);

      H0 += w1 * w2 * fw0;
      H1 += w1 * w2 * fw1;
      H2 += w1 * w2 * fw2;
      H3 += w1 * w2 * fw3;
      H4 += w1 * w2 * fw4;
    }
  }

  H0 += 1e-3;              // prevent dividing by 0
  outputs[0] = (H0);
  outputs[1] = (H1 / H0);
  outputs[2] = (H2 / H0);
  outputs[3] = (H3 / H0);
  outputs[4] = (H4 / H0);

  return;
}


float BestCalc::LegP(float x, int order){
  /* Calculation in FWMoments */
  float value(0.0);

  if (order == 0) value = 1;
  else if (order == 1) value = x;
  else if (order == 2) value = 0.5*(3*x*x - 1);
  else if (order == 3) value = 0.5*(5*x*x*x - 3*x);
  else if (order == 4) value = 0.125*(35*x*x*x*x - 30*x*x + 3);
  else value = 0;

  return value;
}


unsigned int BestCalc::getParticleID(){
  /* Use simple algorithm to get the predicted particle ID
       - Particle ID = Particle Type with largest score
           (particleType == 0) QCD
           (particleType == 1) Top
           (particleType == 2) H
           (particleType == 3) Z
           (particleType == 4) W
        Here you can also add more sophisticated algorithms for determining the tagging,
        e.g., define working points to "tag" a jet.
  */
  std::vector<double> values{ m_NNresults["dnn_qcd"],   m_NNresults["dnn_top"],
      m_NNresults["dnn_higgs"], m_NNresults["dnn_z"], m_NNresults["dnn_w"] };

  unsigned int particleID(0);
  double max_value(-1.0);
  for (unsigned int pid=0,size=values.size();pid<size;pid++){
    if (values.at(pid) > max_value){
      max_value  = values.at(pid);
      particleID = pid;
    }
  }

  return particleID;
}


int BestCalc::EndJob()
{
  delete m_lwtnn;
  return 0;
}
