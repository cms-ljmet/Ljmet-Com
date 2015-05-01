/*
 Calculator for many btagging algorithms
 
 Author: Alex Garabedian, 2014
 Update: Orduna, 2015
 */

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <limits>   // std::numeric_limits

#include "LJMet/Com/interface/BaseCalc.h"
#include "LJMet/Com/interface/LjmetFactory.h"
#include "LJMet/Com/interface/LjmetEventContent.h"
#include "TLorentzVector.h"

#include "EgammaAnalysis/ElectronTools/interface/ElectronEffectiveArea.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/Math/interface/deltaR.h"

class LjmetFactory;

class BTagSFCalc : public BaseCalc {
public:
    BTagSFCalc();
    virtual ~BTagSFCalc();
    virtual int BeginJob();
    virtual int AnalyzeEvent(edm::EventBase const & event, BaseEventSelector * selector);
    virtual int EndJob();
    
private:
    edm::InputTag rhoSrc_;
    edm::InputTag triggerSummary_;
    bool isTB_;
    bool isTT_;
    bool isWJets_;
    
};

static int reg = LjmetFactory::GetInstance()->Register(new BTagSFCalc(), "BTagSFCalc");

BTagSFCalc::BTagSFCalc()
{
}

BTagSFCalc::~BTagSFCalc()
{
}

int BTagSFCalc::BeginJob()
{
    if (mPset.exists("rhoSrc")) rhoSrc_ = mPset.getParameter<edm::InputTag>("rhoSrc");
    else                        rhoSrc_ = edm::InputTag("fixedGridRhoAll");
    
    if (mPset.exists("triggerSummary")) triggerSummary_ = mPset.getParameter<edm::InputTag>("triggerSummary");
    else                                triggerSummary_ = edm::InputTag("selectedPatTrigger");
    
    if (mPset.exists("isTB"))    isTB_ = mPset.getParameter<bool>("isTB");
    else                         isTB_ = false;
    
    if (mPset.exists("isTT"))    isTT_ = mPset.getParameter<bool>("isTT");
    else                         isTT_ = false;
    
    if (mPset.exists("isWJets")) isWJets_ = mPset.getParameter<bool>("isWJets");
    else                         isWJets_ = false;
    
    return 0;
}

int BTagSFCalc::AnalyzeEvent(edm::EventBase const & event, BaseEventSelector * selector)
{    // ----- Get objects from the selector -----
    std::vector<edm::Ptr<pat::Jet> > const & vSelJets = selector->GetSelectedJets();
    
    std::string tagger_ciscv2 = "combinedInclusiveSecondaryVertexV2BJetTags";
    std::string tagger_cmva   = "combinedMVABJetTags";
    std::string tagger_jBp    = "jetBProbabilityBJetTags";
    std::string tagger_jp     = "jetProbabilityBJetTags";
    std::string tagger_pfcsv  = "pfCombinedSecondaryVertexBJetTags";
    std::string tagger_ssv_he = "simpleSecondaryVertexHighEffBJetTags";
    std::string tagger_tchp   = "trackCountingHighPurBJetTags";
    std::string tagger_tche   = "trackCountingHighEffBJetTags";
    std::string tagger_ssv_hp = "simpleSecondaryVertexHighPurBJetTags";
    
    std::vector<std::string> tagger;
    tagger.push_back(tagger_ciscv2);
    tagger.push_back(tagger_cmva);
    tagger.push_back(tagger_jBp);
    tagger.push_back(tagger_jp);
    tagger.push_back(tagger_pfcsv);
    tagger.push_back(tagger_ssv_he);
    tagger.push_back(tagger_tchp);
    tagger.push_back(tagger_tche);
    tagger.push_back(tagger_ssv_hp);

    std::map<std::string, std::vector<double> > Discrims;
    
    int _nJets = 8;
    for (int jetcount = 0; jetcount < _nJets; ++jetcount) {
        for(std::vector<std::string>::iterator st  = tagger.begin(); st != tagger.end(); ++st) {
            Discrims[*st].push_back(-std::numeric_limits<double>::max());
        }
    }
    
    int jetNum = 0;
    for (std::vector<edm::Ptr<pat::Jet> >::const_iterator jet = vSelJets.begin(); jet != vSelJets.end(); ++jet) {
        for (std::vector<std::string>::iterator st  = tagger.begin(); st != tagger.end(); ++st) {
            Discrims[*st][jetNum] = (*jet)->bDiscriminator(*st);
        }
        if (jetNum == (_nJets - 1)) break;
        jetNum++;
    }
    
    for(std::vector<std::string>::iterator st  = tagger.begin(); st != tagger.end(); ++st) {
        SetValue(*st, Discrims[*st]);
    }
    
    return 0;
}

int BTagSFCalc::EndJob()
{
    return 0;
}
