import FWCore.ParameterSet.Config as cms

JetSubCalc = cms.PSet(
                      debug             = cms.bool(False),
                      AK8slimmedJetColl = cms.InputTag("slimmedJetsAK8")
                      )

