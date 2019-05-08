// -*- C++ -*-
//
// Helper class, provides jet tagging eff, scale factors, etc.
//
// Normally, you would want to get this via official conditions
// mechanisms (event setup). At the moment, there is a memory
// leak somewhere in core software when one gets some conditions
// from fwlite event setup from a root file.
//
// This class is hopefully a temporary solution for the memory leak.
//
// Gena Kukartsev, November 2012
//


#include <cmath>
#include "LJMet/Com/interface/BtagHardcodedConditions.h"
#include "FWCore/Utilities/interface/Exception.h"

using namespace std;
enum shift:char{ central, up, down, uncert };
enum method:char{ mujets, comb };


BtagHardcodedConditions::BtagHardcodedConditions() {
}

BtagHardcodedConditions::~BtagHardcodedConditions() {
}

std::string BtagHardcodedConditions::getAlgoName(const std::string & op)
{
  if     ( op == "DeepCSVLOOSE"   )  return "pfDeepCSVJetTags:probb";
  else if( op == "DeepCSVMEDIUM"  )  return "pfDeepCSVJetTags:probb";
  else if( op == "DeepCSVTIGHT"   )  return "pfDeepCSVJetTags:probb";
  else if( op == "SJDeepCSVLOOSE" )  return "pfDeepCSVJetTags:probb";
  else if( op == "SJDeepCSVMEDIUM")  return "pfDeepCSVJetTags:probb";
  throw cms::Exception("InvalidInput") << "Unknown tagger/operating point: "<< op << std::endl;
}

float BtagHardcodedConditions::getDiscriminant(const std::string & op)
{
  if     ( op == "DeepCSVLOOSE"   ) return 0.1522;
  else if( op == "DeepCSVMEDIUM"  ) return 0.4941;
  else if( op == "DeepCSVTIGHT"   ) return 0.8001;
  else if( op == "SJDeepCSVLOOSE" ) return 0.1522;
  else if( op == "SJDeepCSVMEDIUM") return 0.4941;
  throw cms::Exception("InvalidInput") << "Unknown operating point: "<< op << std::endl;
}

/*.-----------------------------------------------------------------.
  /  .-.                                                         .-.  \
 |  /   \                                                       /   \  |
 | |\_.  |                                                     |    /| |
 |\|  | /|               B-JET SCALE FACTOR SECTION            |\  | |/|
 | `---' |                                                     | `---' |
 |       |                                                     |       | 
 |       |-----------------------------------------------------|       |
 \       |                                                     |       /
  \     /                                                       \     /
   `---'                                                         `---'*/

double BtagHardcodedConditions::GetBtagEfficiency(double pt, double eta, std::string tagger)
{
  // Efficiencies from TTToSemiLeptonic powheg sample for Fall18 (?).
  // ~jmanagan/nobackup/EffsAndNegWeights/NegWeights2018/TTSemiLep.root
  // Uses hadronFlavour() rather than partonFlavour() as recommended in BTV physics plenary CMS Week 10/2015
  // BEff_Nptbins_b/BEff_Dptbins_b
  // [0.403823, 0.618852, 0.679287, 0.706293, 0.717887, 0.713093, 0.670051, 0.59587, 0.531372, 0.483849, 0.417429, 0.30052, 0.20051, 0.124058]
  // BEffLoose_Nptbins_b/BEffLoose_Dptbins_b
  // [0.628235, 0.794337, 0.839644, 0.863855, 0.878786, 0.88415, 0.872817, 0.850809, 0.834119, 0.824796, 0.802984, 0.751513, 0.684949, 0.598841]

  if(tagger == "DeepCSVMEDIUM" or tagger == "SJDeepCSVMEDIUM"){
     if(pt < 30)        return 0.403823;
     else if(pt < 50)   return 0.618852;
     else if(pt < 70)   return 0.679287;
     else if(pt < 100)  return 0.706293;
     else if(pt < 140)  return 0.717887;
     else if(pt < 200)  return 0.713093;
     else if(pt < 300)  return 0.670051;
     else if(pt < 400)  return 0.59587;
     else if(pt < 500)  return 0.531372;
     else if(pt < 600)  return 0.483849;
     else if(pt < 800)  return 0.417429;
     else if(pt < 1000) return 0.30052;
     else if(pt < 1200) return 0.20051;
     else               return 0.124058;
  }
  else if( tagger == "DeepCSVLOOSE" or tagger == "SJDeepCSVLOOSE") {
     if(pt < 30)        return 0.628235;
     else if(pt < 50)   return 0.794337;
     else if(pt < 70)   return 0.839644;
     else if(pt < 100)  return 0.863855;
     else if(pt < 140)  return 0.878786;
     else if(pt < 200)  return 0.88415;
     else if(pt < 300)  return 0.872817;
     else if(pt < 400)  return 0.850809;
     else if(pt < 500)  return 0.834119;
     else if(pt < 600)  return 0.824796;
     else if(pt < 800)  return 0.802984;
     else if(pt < 1000) return 0.751513;
     else if(pt < 1200) return 0.684949;
     else               return 0.598841;
  }			      
  
  
  // unknown tagger, return default
  return -100.0;
  
}

double BtagHardcodedConditions::GetCtagEfficiency(double pt, double eta, std::string tagger)
{
  // BEff_Nptbins_c/BEff_Dptbins_c
  // [0.055637, 0.089934, 0.09309, 0.099994, 0.108785, 0.114926, 0.110015, 0.093696, 0.087263, 0.068838, 0.047241, 0.022655, 0.015532, 0.008043]
  // BEffLoose_Nptbins_c/BEffLoose_Dptbins_c
  // [0.255572, 0.362164, 0.376513, 0.392257, 0.408828, 0.419713, 0.410212, 0.39379, 0.408445, 0.391614, 0.354956, 0.318908, 0.27957, 0.243968]

  if(tagger == "DeepCSVMEDIUM" or tagger == "SJDeepCSVMEDIUM"){
     if(pt < 30)        return 0.055637;
     else if(pt < 50)   return 0.089934;
     else if(pt < 70)   return 0.09309;
     else if(pt < 100)  return 0.099994;
     else if(pt < 140)  return 0.108785;
     else if(pt < 200)  return 0.114926;
     else if(pt < 300)  return 0.110015;
     else if(pt < 400)  return 0.093696;
     else if(pt < 500)  return 0.087263;
     else if(pt < 600)  return 0.068838;
     else if(pt < 800)  return 0.047241;
     else if(pt < 1000) return 0.022655;
     else if(pt < 1200) return 0.015532;
     else               return 0.008043;
  }
  else if( tagger == "DeepCSVLOOSE" or tagger == "SJDeepCSVLOOSE") {
     if(pt < 30)        return 0.255572;
     else if(pt < 50)   return 0.362164;
     else if(pt < 70)   return 0.376513;
     else if(pt < 100)  return 0.392257;
     else if(pt < 140)  return 0.408828;
     else if(pt < 200)  return 0.419713;
     else if(pt < 300)  return 0.410212;
     else if(pt < 400)  return 0.39379;
     else if(pt < 500)  return 0.408445;
     else if(pt < 600)  return 0.391614;
     else if(pt < 800)  return 0.354956;
     else if(pt < 1000) return 0.318908;
     else if(pt < 1200) return 0.27957;
     else               return 0.243968;
  }else{
    std::cerr << "Tagger " << tagger << " not coded into GetCtagEfficiency!" << std::endl;
    return 0;
  }
}

double BtagHardcodedConditions::GetMistagRate(double pt, double eta, std::string tagger)
{
  // BEff_Nptbins_udsg/BEff_Dptbins_udsg
  // [0.00308, 0.007497, 0.006558, 0.006771, 0.00761, 0.008422, 0.009002, 0.00957, 0.010041, 0.00947, 0.007225, 0.00395, 0.002117, 0.001617]
  // BEffLoose_Nptbins_udsg/BEffLoose_Dptbins_udsg
  // [0.086838, 0.090621, 0.078963, 0.081875, 0.091066, 0.102062, 0.114996, 0.127951, 0.142193, 0.149584, 0.153378, 0.152735, 0.141539, 0.124475]

  if(tagger == "DeepCSVMEDIUM" || tagger == "SJDeepCSVMEDIUM"){
     if(pt < 30)        return 0.00308;
     else if(pt < 50)   return 0.007497;
     else if(pt < 70)   return 0.006558;
     else if(pt < 100)  return 0.006771;
     else if(pt < 140)  return 0.00761;
     else if(pt < 200)  return 0.008422;
     else if(pt < 300)  return 0.009002;
     else if(pt < 400)  return 0.00957;
     else if(pt < 500)  return 0.010041;
     else if(pt < 600)  return 0.00947;
     else if(pt < 800)  return 0.007225;
     else if(pt < 1000) return 0.00395;
     else if(pt < 1200) return 0.002117;
     else               return 0.001617;
  }
  else if( tagger == "DeepCSVLOOSE" || tagger == "SJDeepCSVLOOSE") {
     if(pt < 30)        return 0.086838;
     else if(pt < 50)   return 0.090621;
     else if(pt < 70)   return 0.078963;
     else if(pt < 100)  return 0.081875;
     else if(pt < 140)  return 0.091066;
     else if(pt < 200)  return 0.102062;
     else if(pt < 300)  return 0.114996;
     else if(pt < 400)  return 0.127951;
     else if(pt < 500)  return 0.142193;
     else if(pt < 600)  return 0.149584;
     else if(pt < 800)  return 0.153378;
     else if(pt < 1000) return 0.152735;
     else if(pt < 1200) return 0.141539;
     else               return 0.124475;
  }
  else{
    std::cerr << "Tagger " << tagger << " not coded into MistagRate!" << std::endl;
    return 0;
  }
}

/*.-----------------------------------------------------------------.
  /  .-.                                                         .-.  \
 |  /   \                                                       /   \  |
 | |\_.  |                                                     |    /| |
 |\|  | /|            ALLLL BELOW IS DEAD! USING READERS       |\  | |/|
 | `---' |                                                     | `---' |
 |       |                                                     |       | 
 |       |-----------------------------------------------------|       |
 \       |                                                     |       /
  \     /                                                       \     /
   `---'                                                         `---'*/


double BtagHardcodedConditions::GetBtagScaleFactor(double pt, double eta, std::string tagger, int year)
{
  //The main getter for B-tag Scale Factors
  if      (year==2016) return GetBtagSF2016_comb(tagger, central, pt, eta);
  else if (year==2015) return GetBtagScaleFactor2015(pt,eta,tagger);
  else if (year==2012) return GetBtagScaleFactor2012(pt, eta, tagger);
  else if (year==2011) return GetBtagScaleFactor2011(pt, eta, tagger);
  else return 0.;
}//end GetBtagScaleFactor

/************************************************
 *                                              *
 * Main Getter functions for 2016 scale factors *
 *                                              *
 ************************************************/

double BtagHardcodedConditions::GetBtagSF2016(std::string tagger, method Method, shift Shift, double pt, double eta)
{
	//This is the 2016 bjet scale factor getter, which allows for the choice of using either mujets or comb
    if(      tagger == "CSVM" )      return GetBtagSF2016Medium(Method, Shift, pt, eta);
    else if( tagger == "CSVMsubjet") return GetBtagSF2016Medium_subjet( Shift, pt, eta);
    else if( tagger == "CSVL"  )     return GetBtagSF2016Loose( Method, Shift, pt, eta);
    else if( tagger == "CSVLsubjet") return GetBtagSF2016Loose_subjet(  Shift, pt, eta);
    else if( tagger == "CSVT" )      return GetBtagSF2016Tight( Method, Shift, pt, eta);
    else{
	throw cms::Exception("InvalidInput") << "Unknown tagger: "<< tagger << std::endl;
	//cout<<"Error, GetBtagSF2016 received a tagger string it does not know how to interpret."<<endl;
	return 1.0;
    }
}//end main getter.

double BtagHardcodedConditions::GetBtagSF2016_comb(std::string tagger, shift Shift, double pt, double eta)
{
	//This is the 2016 bjet scale factor getter for b-jet SF from the "combine" method
    if(      tagger == "CSVM" )      return GetBtagSF2016Medium_comb(Shift, pt, eta);
    else if( tagger == "CSVMsubjet") return GetBtagSF2016Medium_subjet( Shift, pt, eta);
    else if( tagger == "CSVL"  )     return GetBtagSF2016Loose_comb(  Shift, pt, eta);
    else if( tagger == "CSVLsubjet") return GetBtagSF2016Loose_subjet(  Shift, pt, eta);
    else if( tagger == "CSVT" )      return GetBtagSF2016Tight_comb(Shift, pt, eta);
    else{
	throw cms::Exception("InvalidInput") << "Unknown tagger: "<< tagger << std::endl;
	//cout<<"Error, GetBtagSF2016 received a tagger string it does not know how to interpret."<<endl;
	return 1.0;
    }
}//end main getter.

/*************************************
 *                                   *
 *  Other Getters for 2016 Bjet SF's *
 *                                   *
 ************************************/

double BtagHardcodedConditions::GetBtagSF2016Loose(method Method, shift Shift, double pt, double eta)
{
    if(Method==mujets) 
	return GetBtagSF2016Loose_mujets( Shift, pt, eta);
    else 
	return GetBtagSF2016Loose_comb(     Shift, pt, eta);
}

double BtagHardcodedConditions::GetBtagSF2016Loose_mujets( shift Shift, double pt, double eta)
{
	//not used
    if(pt > 1000.) pt = 1000.;
    if(fabs(eta) > 2.4 or pt<20.) return 1.0; 
    //if(fabs(eta) > 2.4 or pt<20. or pt > 1000.) return 1.0; 
    switch(Shift){
	case uncert:
	    if(pt<30) return 0.025442773476243019;
	    else if(pt<50) return 0.013995612040162086;
	    else if(pt<70) return 0.01321903895586729;
	    else if(pt<100) return 0.013857406564056873;
	    else if(pt<140) return 0.013207088224589825;
	    else if(pt<200) return 0.011531321331858635;
	    else if(pt<300) return 0.01834111288189888;
	    else if(pt<600) return 0.018383314833045006;
	    else  return 0.022504881024360657;
	case up:
	    if(pt<30) return (0.884016*((1.+(0.0331508*pt))/(1.+(0.0285096*pt))))+0.025442773476243019;
	    else if(pt<50) return (0.884016*((1.+(0.0331508*pt))/(1.+(0.0285096*pt))))+0.013995612040162086;
	    else if(pt<70) return (0.884016*((1.+(0.0331508*pt))/(1.+(0.0285096*pt))))+0.01321903895586729;
	    else if(pt<100) return (0.884016*((1.+(0.0331508*pt))/(1.+(0.0285096*pt))))+0.013857406564056873;
	    else if(pt<140) return (0.884016*((1.+(0.0331508*pt))/(1.+(0.0285096*pt))))+0.013207088224589825;
	    else if(pt<200) return (0.884016*((1.+(0.0331508*pt))/(1.+(0.0285096*pt))))+0.011531321331858635;
	    else if(pt<300) return (0.884016*((1.+(0.0331508*pt))/(1.+(0.0285096*pt))))+0.01834111288189888;
	    else if(pt<600) return (0.884016*((1.+(0.0331508*pt))/(1.+(0.0285096*pt))))+0.018383314833045006;
	    else  return (0.884016*((1.+(0.0331508*pt))/(1.+(0.0285096*pt))))+0.022504881024360657;
	case down:
	    if(pt<30) return (0.884016*((1.+(0.0331508*pt))/(1.+(0.0285096*pt))))-0.025442773476243019;
	    else if(pt<50) return (0.884016*((1.+(0.0331508*pt))/(1.+(0.0285096*pt))))-0.013995612040162086;
	    else if(pt<70) return (0.884016*((1.+(0.0331508*pt))/(1.+(0.0285096*pt))))-0.01321903895586729;
	    else if(pt<100) return (0.884016*((1.+(0.0331508*pt))/(1.+(0.0285096*pt))))-0.013857406564056873;
	    else if(pt<140) return (0.884016*((1.+(0.0331508*pt))/(1.+(0.0285096*pt))))-0.013207088224589825;
	    else if(pt<200) return (0.884016*((1.+(0.0331508*pt))/(1.+(0.0285096*pt))))-0.011531321331858635;
	    else if(pt<300) return (0.884016*((1.+(0.0331508*pt))/(1.+(0.0285096*pt))))-0.01834111288189888;
	    else if(pt<600) return (0.884016*((1.+(0.0331508*pt))/(1.+(0.0285096*pt))))-0.018383314833045006;
	    else  return (0.884016*((1.+(0.0331508*pt))/(1.+(0.0285096*pt))))-0.022504881024360657;
	case central: 
	default:
	    return  0.884016*((1.+(0.0331508*pt))/(1.+(0.0285096*pt)));   
    }//end switch on shift
}

double BtagHardcodedConditions::GetBtagSF2016Loose_comb(shift Shift, double pt, double eta)
{
    if(pt > 1000.) pt = 1000.;
    if(fabs(eta) > 2.4 or pt<20.) return 1.0; 
    //if(fabs(eta) > 2.4 or pt<20. or pt > 1000.) return 1.0; 
    switch(Shift){
	case uncert:
	    if(pt<30) return 0.025381835177540779;
	    else if(pt<50) return 0.012564006261527538;
	    else if(pt<70) return 0.011564776301383972;
	    else if(pt<100) return 0.011248723603785038;
	    else if(pt<140) return 0.010811596177518368;
	    else if(pt<200) return 0.010882497765123844;
	    else if(pt<300) return 0.013456921093165874;
	    else if(pt<600) return 0.017094610258936882;
	    else  return 0.02186630479991436;
	case up:
	    if(pt<30) return (0.887973*((1.+(0.0523821*pt))/(1.+(0.0460876*pt))))+0.025381835177540779;
	    else if(pt<50) return (0.887973*((1.+(0.0523821*pt))/(1.+(0.0460876*pt))))+0.012564006261527538;
	    else if(pt<70) return (0.887973*((1.+(0.0523821*pt))/(1.+(0.0460876*pt))))+0.011564776301383972;
	    else if(pt<100) return (0.887973*((1.+(0.0523821*pt))/(1.+(0.0460876*pt))))+0.011248723603785038;
	    else if(pt<140) return (0.887973*((1.+(0.0523821*pt))/(1.+(0.0460876*pt))))+0.010811596177518368;
	    else if(pt<200) return (0.887973*((1.+(0.0523821*pt))/(1.+(0.0460876*pt))))+0.010882497765123844;
	    else if(pt<300) return (0.887973*((1.+(0.0523821*pt))/(1.+(0.0460876*pt))))+0.013456921093165874;
	    else if(pt<600) return (0.887973*((1.+(0.0523821*pt))/(1.+(0.0460876*pt))))+0.017094610258936882;
	    else  return (0.887973*((1.+(0.0523821*pt))/(1.+(0.0460876*pt))))+0.02186630479991436;
	case down:
	    if(pt<30) return (0.887973*((1.+(0.0523821*pt))/(1.+(0.0460876*pt))))-0.025381835177540779;
	    else if(pt<50) return (0.887973*((1.+(0.0523821*pt))/(1.+(0.0460876*pt))))-0.012564006261527538;
	    else if(pt<70) return (0.887973*((1.+(0.0523821*pt))/(1.+(0.0460876*pt))))-0.011564776301383972;
	    else if(pt<100) return (0.887973*((1.+(0.0523821*pt))/(1.+(0.0460876*pt))))-0.011248723603785038;
	    else if(pt<140) return (0.887973*((1.+(0.0523821*pt))/(1.+(0.0460876*pt))))-0.010811596177518368;
	    else if(pt<200) return (0.887973*((1.+(0.0523821*pt))/(1.+(0.0460876*pt))))-0.010882497765123844;
	    else if(pt<300) return (0.887973*((1.+(0.0523821*pt))/(1.+(0.0460876*pt))))-0.013456921093165874;
	    else if(pt<600) return (0.887973*((1.+(0.0523821*pt))/(1.+(0.0460876*pt))))-0.017094610258936882;
	    else  return (0.887973*((1.+(0.0523821*pt))/(1.+(0.0460876*pt))))-0.02186630479991436;
	case central:
	default:
	    return   0.887973*((1.+(0.0523821*pt))/(1.+(0.0460876*pt)));  
    }//end switch on shift
}

double BtagHardcodedConditions::GetBtagSF2016Medium(method Method, shift Shift, double pt, double eta)
{
    if(Method==mujets) 
	return GetBtagSF2016Medium_mujets( Shift, pt, eta);
    else 
	return GetBtagSF2016Medium_comb(     Shift, pt, eta);
}

double BtagHardcodedConditions::GetBtagSF2016Medium_mujets(shift Shift, double pt, double eta)
{
	//not used
    if(pt > 1000.) pt = 1000.;
    if(fabs(eta) > 2.4 or pt<20.) return 1.0; 
    //if(fabs(eta) > 2.4 or pt<20. or pt > 1000.) return 1.0; 
    switch(Shift){
	case uncert:
	    if(pt<30) return 0.040554910898208618;
	    else if(pt<50) return 0.01836167648434639;
	    else if(pt<70) return 0.016199169680476189;
	    else if(pt<100) return 0.014634267427027225;
	    else if(pt<140) return 0.014198922552168369;
	    else if(pt<200) return 0.016547618433833122;
	    else if(pt<300) return 0.02140621654689312;
	    else if(pt<600) return 0.023563217371702194;
	    else  return 0.034716218709945679;
	case up:
	    if(pt<30) return (0.718014*((1.+(0.0685826*pt))/(1.+(0.0475779*pt))))+0.040554910898208618;
	    else if(pt<50) return (0.718014*((1.+(0.0685826*pt))/(1.+(0.0475779*pt))))+0.01836167648434639;
	    else if(pt<70) return (0.718014*((1.+(0.0685826*pt))/(1.+(0.0475779*pt))))+0.016199169680476189;
	    else if(pt<100) return (0.718014*((1.+(0.0685826*pt))/(1.+(0.0475779*pt))))+0.014634267427027225;
	    else if(pt<140) return (0.718014*((1.+(0.0685826*pt))/(1.+(0.0475779*pt))))+0.014198922552168369;
	    else if(pt<200) return (0.718014*((1.+(0.0685826*pt))/(1.+(0.0475779*pt))))+0.016547618433833122;
	    else if(pt<300) return (0.718014*((1.+(0.0685826*pt))/(1.+(0.0475779*pt))))+0.02140621654689312;
	    else if(pt<600) return (0.718014*((1.+(0.0685826*pt))/(1.+(0.0475779*pt))))+0.023563217371702194;
	    else  return (0.718014*((1.+(0.0685826*pt))/(1.+(0.0475779*pt))))+0.034716218709945679;
	case down:
	    if(pt<30) return (0.718014*((1.+(0.0685826*pt))/(1.+(0.0475779*pt))))-0.040554910898208618;
	    else if(pt<50) return (0.718014*((1.+(0.0685826*pt))/(1.+(0.0475779*pt))))-0.01836167648434639;
	    else if(pt<70) return (0.718014*((1.+(0.0685826*pt))/(1.+(0.0475779*pt))))-0.016199169680476189;
	    else if(pt<100) return (0.718014*((1.+(0.0685826*pt))/(1.+(0.0475779*pt))))-0.014634267427027225;
	    else if(pt<140) return (0.718014*((1.+(0.0685826*pt))/(1.+(0.0475779*pt))))-0.014198922552168369;
	    else if(pt<200) return (0.718014*((1.+(0.0685826*pt))/(1.+(0.0475779*pt))))-0.016547618433833122;
	    else if(pt<300) return (0.718014*((1.+(0.0685826*pt))/(1.+(0.0475779*pt))))-0.02140621654689312;
	    else if(pt<600) return (0.718014*((1.+(0.0685826*pt))/(1.+(0.0475779*pt))))-0.023563217371702194;
	    else  return (0.718014*((1.+(0.0685826*pt))/(1.+(0.0475779*pt))))-0.034716218709945679;
	case central:
	default:
	    return    0.718014*((1.+(0.0685826*pt))/(1.+(0.0475779*pt)));
    }//end switch on shift
}

double BtagHardcodedConditions::GetBtagSF2016Medium_comb(shift Shift, double pt, double eta)
{
    if(pt > 1000.) pt = 1000.;
    if(fabs(eta) > 2.4 or pt<20.) return 1.0; 
    //if(fabs(eta) > 2.4 or pt<20. or pt > 1000.) return 1.0; 
    switch(Shift){
	case uncert:
	    if(pt<30) return 0.040213499218225479;
	    else if(pt<50) return 0.014046305790543556;
	    else if(pt<70) return 0.012372690252959728;
	    else if(pt<100) return 0.012274007312953472;
	    else if(pt<140) return 0.011465910822153091;
	    else if(pt<200) return 0.012079551815986633;
	    else if(pt<300) return 0.014995276927947998;
	    else if(pt<600) return 0.021414462476968765;
	    else  return 0.032377112656831741;
	case up:
	    if(pt<30) return (0.561694*((1.+(0.31439*pt))/(1.+(0.17756*pt))))+0.040213499218225479;
	    else if(pt<50) return (0.561694*((1.+(0.31439*pt))/(1.+(0.17756*pt))))+0.014046305790543556;
	    else if(pt<70) return (0.561694*((1.+(0.31439*pt))/(1.+(0.17756*pt))))+0.012372690252959728;
	    else if(pt<100) return (0.561694*((1.+(0.31439*pt))/(1.+(0.17756*pt))))+0.012274007312953472;
	    else if(pt<140) return (0.561694*((1.+(0.31439*pt))/(1.+(0.17756*pt))))+0.011465910822153091;
	    else if(pt<200) return (0.561694*((1.+(0.31439*pt))/(1.+(0.17756*pt))))+0.012079551815986633;
	    else if(pt<300) return (0.561694*((1.+(0.31439*pt))/(1.+(0.17756*pt))))+0.014995276927947998;
	    else if(pt<600) return (0.561694*((1.+(0.31439*pt))/(1.+(0.17756*pt))))+0.021414462476968765;
	    else  return (0.561694*((1.+(0.31439*pt))/(1.+(0.17756*pt))))+0.032377112656831741;
	case down:
	    if(pt<30) return (0.561694*((1.+(0.31439*pt))/(1.+(0.17756*pt))))-0.040213499218225479;
	    else if(pt<50) return (0.561694*((1.+(0.31439*pt))/(1.+(0.17756*pt))))-0.014046305790543556;
	    else if(pt<70) return (0.561694*((1.+(0.31439*pt))/(1.+(0.17756*pt))))-0.012372690252959728;
	    else if(pt<100) return (0.561694*((1.+(0.31439*pt))/(1.+(0.17756*pt))))-0.012274007312953472;
	    else if(pt<140) return (0.561694*((1.+(0.31439*pt))/(1.+(0.17756*pt))))-0.011465910822153091;
	    else if(pt<200) return (0.561694*((1.+(0.31439*pt))/(1.+(0.17756*pt))))-0.012079551815986633;
	    else if(pt<300) return (0.561694*((1.+(0.31439*pt))/(1.+(0.17756*pt))))-0.014995276927947998;
	    else if(pt<600) return (0.561694*((1.+(0.31439*pt))/(1.+(0.17756*pt))))-0.021414462476968765;
	    else  return (0.561694*((1.+(0.31439*pt))/(1.+(0.17756*pt))))-0.032377112656831741;
	case central:
	default:
	    return    0.561694*((1.+(0.31439*pt))/(1.+(0.17756*pt)));
    }//end switch on shift
}

double BtagHardcodedConditions::GetBtagSF2016Tight(method Method, shift Shift, double pt, double eta)
{
    if(Method==mujets) 
	return GetBtagSF2016Tight_mujets( Shift, pt, eta);
    else 
	return GetBtagSF2016Tight_comb(     Shift, pt, eta);
}

double BtagHardcodedConditions::GetBtagSF2016Tight_mujets( shift Shift, double pt, double eta)
{
	//not used
    if(pt > 1000.) pt = 1000.;
    if(fabs(eta) > 2.4 or pt<20.) return 1.0; 
//    if(fabs(eta) > 2.4 or pt<20. or pt > 1000.) return 1.0; 
    switch(Shift){
	case uncert:
	    if(pt<30) return 0.034476730972528458;
	    else if(pt<50) return 0.022783603519201279;
	    else if(pt<70) return 0.021012712270021439;
	    else if(pt<100) return 0.017111778259277344;
	    else if(pt<140) return 0.016918083652853966;
	    else if(pt<200) return 0.016693713143467903;
	    else if(pt<300) return 0.02831784263253212;
	    else if(pt<600) return 0.032944366335868835;
	    else  return 0.054636202752590179;
	case up:
	    if(pt<30) return (0.849497*((1.+(0.01854*pt))/(1.+(0.0153613*pt))))+0.034476730972528458;
	    else if(pt<50) return (0.849497*((1.+(0.01854*pt))/(1.+(0.0153613*pt))))+0.022783603519201279;
	    else if(pt<70) return (0.849497*((1.+(0.01854*pt))/(1.+(0.0153613*pt))))+0.021012712270021439;
	    else if(pt<100) return (0.849497*((1.+(0.01854*pt))/(1.+(0.0153613*pt))))+0.017111778259277344;
	    else if(pt<140) return (0.849497*((1.+(0.01854*pt))/(1.+(0.0153613*pt))))+0.016918083652853966;
	    else if(pt<200) return (0.849497*((1.+(0.01854*pt))/(1.+(0.0153613*pt))))+0.016693713143467903;
	    else if(pt<300) return (0.849497*((1.+(0.01854*pt))/(1.+(0.0153613*pt))))+0.02831784263253212;
	    else if(pt<600) return (0.849497*((1.+(0.01854*pt))/(1.+(0.0153613*pt))))+0.032944366335868835;
	    else  return (0.849497*((1.+(0.01854*pt))/(1.+(0.0153613*pt))))+0.054636202752590179;
	case down:
	    if(pt<30) return (0.849497*((1.+(0.01854*pt))/(1.+(0.0153613*pt))))-0.034476730972528458;
	    else if(pt<50) return (0.849497*((1.+(0.01854*pt))/(1.+(0.0153613*pt))))-0.022783603519201279;
	    else if(pt<70) return (0.849497*((1.+(0.01854*pt))/(1.+(0.0153613*pt))))-0.021012712270021439;
	    else if(pt<100) return (0.849497*((1.+(0.01854*pt))/(1.+(0.0153613*pt))))-0.017111778259277344;
	    else if(pt<140) return (0.849497*((1.+(0.01854*pt))/(1.+(0.0153613*pt))))-0.016918083652853966;
	    else if(pt<200) return (0.849497*((1.+(0.01854*pt))/(1.+(0.0153613*pt))))-0.016693713143467903;
	    else if(pt<300) return (0.849497*((1.+(0.01854*pt))/(1.+(0.0153613*pt))))-0.02831784263253212;
	    else if(pt<600) return (0.849497*((1.+(0.01854*pt))/(1.+(0.0153613*pt))))-0.032944366335868835;
	    else  return (0.849497*((1.+(0.01854*pt))/(1.+(0.0153613*pt))))-0.054636202752590179;
	case central:
	default:
	    return  0.849497*((1.+(0.01854*pt))/(1.+(0.0153613*pt)));
    }
}

double BtagHardcodedConditions::GetBtagSF2016Tight_comb( shift Shift, double pt, double eta)
{
    if(pt > 1000.) pt = 1000.;
    if(fabs(eta) > 2.4 or pt<20.) return 1.0; 
    //if(fabs(eta) > 2.4 or pt<20. or pt > 1000.) return 1.0; 
    switch(Shift){
	case uncert:
	    if(pt<30) return 0.033732704818248749;
	    else if(pt<50) return 0.01562843844294548;
	    else if(pt<70) return 0.013530348427593708;
	    else if(pt<100) return 0.013609844259917736;
	    else if(pt<140) return 0.013236711733043194;
	    else if(pt<200) return 0.013806583359837532;
	    else if(pt<300) return 0.019633084535598755;
	    else if(pt<600) return 0.030928170308470726;
	    else  return 0.052857179194688797;
	case up:
	    if(pt<30) return (0.817647*((1.+(0.038703*pt))/(1.+(0.0312388*pt))))+0.033732704818248749;
	    else if(pt<50) return (0.817647*((1.+(0.038703*pt))/(1.+(0.0312388*pt))))+0.01562843844294548;
	    else if(pt<70) return (0.817647*((1.+(0.038703*pt))/(1.+(0.0312388*pt))))+0.013530348427593708;
	    else if(pt<100) return (0.817647*((1.+(0.038703*pt))/(1.+(0.0312388*pt))))+0.013609844259917736;
	    else if(pt<140) return (0.817647*((1.+(0.038703*pt))/(1.+(0.0312388*pt))))+0.013236711733043194;
	    else if(pt<200) return (0.817647*((1.+(0.038703*pt))/(1.+(0.0312388*pt))))+0.013806583359837532;
	    else if(pt<300) return (0.817647*((1.+(0.038703*pt))/(1.+(0.0312388*pt))))+0.019633084535598755;
	    else if(pt<600) return (0.817647*((1.+(0.038703*pt))/(1.+(0.0312388*pt))))+0.030928170308470726;
	    else  return (0.817647*((1.+(0.038703*pt))/(1.+(0.0312388*pt))))+0.052857179194688797;
	case down:
	    if(pt<30) return (0.817647*((1.+(0.038703*pt))/(1.+(0.0312388*pt))))-0.033732704818248749;
	    else if(pt<50) return (0.817647*((1.+(0.038703*pt))/(1.+(0.0312388*pt))))-0.01562843844294548;
	    else if(pt<70) return (0.817647*((1.+(0.038703*pt))/(1.+(0.0312388*pt))))-0.013530348427593708;
	    else if(pt<100) return (0.817647*((1.+(0.038703*pt))/(1.+(0.0312388*pt))))-0.013609844259917736;
	    else if(pt<140) return (0.817647*((1.+(0.038703*pt))/(1.+(0.0312388*pt))))-0.013236711733043194;
	    else if(pt<200) return (0.817647*((1.+(0.038703*pt))/(1.+(0.0312388*pt))))-0.013806583359837532;
	    else if(pt<300) return (0.817647*((1.+(0.038703*pt))/(1.+(0.0312388*pt))))-0.019633084535598755;
	    else if(pt<600) return (0.817647*((1.+(0.038703*pt))/(1.+(0.0312388*pt))))-0.030928170308470726;
	    else  return (0.817647*((1.+(0.038703*pt))/(1.+(0.0312388*pt))))-0.052857179194688797;
	case central:
	default:
	    return   0.817647*((1.+(0.038703*pt))/(1.+(0.0312388*pt)));
    }//end switch on shift
}

double BtagHardcodedConditions::GetBtagSF2016Medium_subjet(shift Shift, double pt, double eta)
{
    if(fabs(eta) > 2.4 or pt<30.) return 1.0; 
    switch(Shift){
	case uncert:
	    if(pt<120) return 0.03395;
	    else if(pt<180) return 0.01788;
	    else if(pt<240) return 0.02364;
	    else  return 0.03332;
	case up:
	    if(pt<120) return 1.01236;
	    else if(pt<180) return 1.02287;
	    else if(pt<240) return 1.0347;
	    else  return 1.05521;
	case down:
	    if(pt<120) return 0.94446;
	    else if(pt<180) return 0.98711;
	    else if(pt<240) return 0.98742;
	    else  return 0.98857;
	case central: 
	default:
	    if(pt<120) return 0.97841;
	    else if(pt<180) return 1.00499;
	    else if(pt<240) return 1.01106;
	    else  return 1.02189;
    }//end switch on shift
}

double BtagHardcodedConditions::GetBtagSF2016Loose_subjet(shift Shift, double pt, double eta)
{
    if(fabs(eta) > 2.4 or pt<30.) return 1.0; 
    switch(Shift){
	case uncert: 
	    if(pt<120) return 0.01071;
	    else if(pt<180) return 0.01083;
	    else if(pt<240) return 0.01191;
	    else  return 0.01939;
	case up:
	    if(pt<120) return 1.0091;
	    else if(pt<180) return 1.01303;
	    else if(pt<240) return 1.01659;
	    else  return 1.03703;
	case down:
	    if(pt<120) return 0.98769;
	    else if(pt<180) return 0.99137;
	    else if(pt<240) return 0.99277;
	    else  return 0.99825;
	case central: 
	default:
	    if(pt<120) return 0.99839;
	    else if(pt<180) return 1.0022;
	    else if(pt<240) return 1.00468;
	    else  return 1.01764;
    }//end switch on shift
}

/*.-----------------------------------------------------------------.
  /  .-.                                                         .-.  \
 |  /   \                                                       /   \  |
 | |\_.  |                                                     |    /| |
 |\|  | /|      B-JET SCALE FACTOR UNCERTAINTY SECTION         |\  | |/|
 | `---' |                                                     | `---' |
 |       |                                                     |       | 
 |       |-----------------------------------------------------|       |
 \       |                                                     |       /
  \     /                                                       \     /
   `---'                                                         `---'*/


double BtagHardcodedConditions::GetBtagSFUncertUp(double pt, double eta, std::string tagger, int year)
{
  if(year == 2016){
    if(tagger == "CSVMsubjet" or tagger == "CSVLsubjet") return (pt>450 ? 2.0 : 1.0) * GetBtagSF2016_comb(tagger, uncert, pt, eta); 
    else return (pt>1000 ? 2.0 : 1.0) * GetBtagSF2016_comb(tagger, uncert, pt, eta); 
  }
  else if(year == 2015) return GetBtagSFUncertainty2015(pt, eta, tagger);
  else if(year == 2012) return GetBtagSFUncertainty2012(pt, eta, tagger);
  else if(year == 2011) return GetBtagSFUncertainty2011(pt, eta, tagger);
  else return 0.;
}//end GetBtagSFUncertUp

double BtagHardcodedConditions::GetBtagSFUncertDown(double pt, double eta, std::string tagger, int year)
{
  if(year == 2016){
    if(tagger == "CSVMsubjet" or tagger == "CSVLsubjet") return (pt>450 ? 2.0 : 1.0) * GetBtagSF2016_comb(tagger, uncert, pt, eta); 
    else return (pt>1000 ? 2.0 : 1.0) * GetBtagSF2016_comb(tagger, uncert, pt, eta); 
  }
  else if(year == 2015) return GetBtagSFUncertainty2015(pt, eta, tagger);
  else if(year == 2012) return GetBtagSFUncertainty2012(pt, eta, tagger);
  else if(year == 2011) return GetBtagSFUncertainty2011(pt, eta, tagger);
  else return 0.;
}//end GetBtagSFUncertDown

/*.-----------------------------------------------------------------.
  /  .-.                                                         .-.  \
 |  /   \                                                       /   \  |
 | |\_.  |                                                     |    /| |
 |\|  | /|               C-JET SCALE FACTOR SECTION            |\  | |/|
 | `---' |                                                     | `---' |
 |       |                                                     |       | 
 |       |-----------------------------------------------------|       |
 \       |                                                     |       /
  \     /                                                       \     /
   `---'                                                         `---'*/


double BtagHardcodedConditions::GetCtagScaleFactor(double pt, double eta, std::string tagger, int year)
{
  //The main getter for C-tag Scale Factors
  if(year==2016) return GetCtagSF2016_comb(tagger, central, pt, eta);
  else {
    std::cerr << "Year " << year << " not coded for C-tag SFs. Are you really earlier than 2016 data?" << std::endl;
    return 0.;
  }
}//end GetCtagScaleFactor

double BtagHardcodedConditions::GetCtagSFUncertUp(double pt, double eta, std::string tagger, int year)
{
  if(year == 2016){
    if(tagger == "CSVMsubjet" or tagger == "CSVLsubjet") return (pt>450 ? 2.0 : 1.0) * GetCtagSF2016_comb(tagger, uncert, pt, eta); 
    else return (pt>1000 ? 2.0 : 1.0) * GetCtagSF2016_comb(tagger, uncert, pt, eta); 
  }
  else {
    std::cerr << "Year " << year << " not coded for C-tag SFs. Are you really earlier than 2016 data?" << std::endl;
    return 0.;
  }
}//end GetCtagSFUncertUp

double BtagHardcodedConditions::GetCtagSFUncertDown(double pt, double eta, std::string tagger, int year)
{
  if(year == 2016){
    if(tagger == "CSVMsubjet" or tagger == "CSVLsubjet") return (pt>450 ? 2.0 : 1.0) * GetCtagSF2016_comb(tagger, uncert, pt, eta); 
    else return (pt>1000 ? 2.0 : 1.0) * GetCtagSF2016_comb(tagger, uncert, pt, eta); 
  }
  else {
    std::cerr << "Year " << year << " not coded for C-tag SFs. Are you really earlier than 2016 data?" << std::endl;
    return 0.;
  }
}//end GetBtagSFUncertDown

double BtagHardcodedConditions::GetCtagSF2016_comb(std::string tagger, shift Shift, double pt, double eta)
{
  //This is the 2016 bjet scale factor getter for b-jet SF from the "combine" method
  if(      tagger == "CSVM" )      return GetCtagSF2016Medium_comb(Shift, pt, eta);
  else if( tagger == "CSVMsubjet") return GetCtagSF2016Medium_subjet( Shift, pt, eta);
  else if( tagger == "CSVL"  )     return GetCtagSF2016Loose_comb(  Shift, pt, eta);
  else if( tagger == "CSVLsubjet") return GetCtagSF2016Loose_subjet(  Shift, pt, eta);
  else{
    throw cms::Exception("InvalidInput") << "Unknown tagger for C-tag SFs: "<< tagger << std::endl;
    //cout<<"Error, GetBtagSF2016 received a tagger string it does not know how to interpret."<<endl;
    return 1.0;
  }
}//end main getter.

double BtagHardcodedConditions::GetCtagSF2016Loose_comb(shift Shift, double pt, double eta)
{
  // SFs are identical to B tag, with 2.5x uncertainties
  if(pt > 1000.) pt = 1000.;
  if(fabs(eta) > 2.4 or pt<20.) return 1.0; 
  switch(Shift){
  case uncert: return 2.5 * GetBtagSF2016Loose_comb(uncert, pt, eta);
  case up: return (GetBtagSF2016Loose_comb(central, pt, eta) + 2.5*GetBtagSF2016Loose_comb(uncert, pt, eta));
  case down: return (GetBtagSF2016Loose_comb(central, pt, eta) - 2.5*GetBtagSF2016Loose_comb(uncert, pt, eta));
  case central:
  default: return GetBtagSF2016Loose_comb(central, pt, eta);
  }//end switch on shift
}

double BtagHardcodedConditions::GetCtagSF2016Medium_comb(shift Shift, double pt, double eta)
{

  // SFs are identical with 3x uncertainty as B tag
  if(pt > 1000.) pt = 1000.;
  if(fabs(eta) > 2.4 or pt<20.) return 1.0; 
  switch(Shift){
  case uncert: return 3.0 * GetBtagSF2016Medium_comb(uncert, pt, eta);
  case up: return (GetBtagSF2016Medium_comb(central, pt, eta) + 3.0*GetBtagSF2016Medium_comb(uncert, pt, eta));
  case down: return (GetBtagSF2016Medium_comb(central, pt, eta) - 3.0*GetBtagSF2016Medium_comb(uncert, pt, eta));
  case central:
  default: return GetBtagSF2016Medium_comb(central, pt, eta);
  }//end switch on shift
}

double BtagHardcodedConditions::GetCtagSF2016Medium_subjet(shift Shift, double pt, double eta)
{
  // same with 2x uncertainty
  if(fabs(eta) > 2.4 or pt<30.) return 1.0; 
  switch(Shift){
  case uncert: return 2.0 * GetBtagSF2016Medium_subjet(uncert, pt, eta);
  case up: return (GetBtagSF2016Medium_subjet(central, pt, eta) + 2.0*GetBtagSF2016Medium_subjet(uncert, pt, eta));
  case down: return (GetBtagSF2016Medium_subjet(central, pt, eta) - 2.0*GetBtagSF2016Medium_subjet(uncert, pt, eta));
  case central:
  default: return GetBtagSF2016Medium_subjet(central, pt, eta);
  }//end switch on shift
}

double BtagHardcodedConditions::GetCtagSF2016Loose_subjet(shift Shift, double pt, double eta)
{
  // same with 2x uncertainty
  if(fabs(eta) > 2.4 or pt<30.) return 1.0; 
  switch(Shift){
  case uncert: return 2.0 * GetBtagSF2016Loose_subjet(uncert, pt, eta);
  case up: return (GetBtagSF2016Loose_subjet(central, pt, eta) + 2.0*GetBtagSF2016Loose_subjet(uncert, pt, eta));
  case down: return (GetBtagSF2016Loose_subjet(central, pt, eta) - 2.0*GetBtagSF2016Loose_subjet(uncert, pt, eta));
  case central:
  default: return GetBtagSF2016Loose_subjet(central, pt, eta);
  }//end switch on shift
}

/*.-----------------------------------------------------------------.
  /  .-.                                                         .-.  \
 |  /   \                                                       /   \  |
 | |\_.  |                                                     |    /| |
 |\|  | /|                                                     |\  | |/|
 | `---' |           Mistag Rate                               | `---' |
 |       |                                                     |       | 
 |       |-----------------------------------------------------|       |
 \       |                                                     |       /
  \     /                                                       \     /
   `---'                                                         `---'*/

/*double BtagHardcodedConditions::GetMistagRate(double pt, double eta, std::string tagger)
{
  // Mistag rates from TT powheg sample for Moriond17.
  // See distribution in /uscms_data/d3/jmanagan/EffsAndNewWeights/TagEffsM17/BEff.png
  // Uses hadronFlavour() rather than partonFlavour() as recommended in BTV physics plenary CMS Week 10/2015
  if(tagger == "CSVM"){
    if(pt < 30)        return 0.003385;
    else if(pt < 50)   return 0.009673;
    else if(pt < 70)   return 0.008316;
    else if(pt < 100)  return 0.008524;
    else if(pt < 140)  return 0.009092;
    else if(pt < 200)  return 0.011431;
    else if(pt < 300)  return 0.013666;
    else if(pt < 400)  return 0.020405;
    else if(pt < 500)  return 0.023609;
    else if(pt < 600)  return 0.025348;
    else if(pt < 800)  return 0.028858;
    else if(pt < 1000) return 0.030427;
    else if(pt < 1200) return 0.034091;
    else return 0.047619;
  }
  else if( tagger == "CSVL") {
    if(pt < 30)        return 0.068717;
    else if(pt < 50)   return 0.095095;
    else if(pt < 70)   return 0.083338;
    else if(pt < 100)  return 0.085001;
    else if(pt < 140)  return 0.086867;
    else if(pt < 200)  return 0.101223;
    else if(pt < 300)  return 0.114555;
    else if(pt < 400)  return 0.139321;
    else if(pt < 500)  return 0.155025;
    else if(pt < 600)  return 0.167581;
    else if(pt < 800)  return 0.189058;
    else if(pt < 1000) return 0.203596;
    else if(pt < 1200) return 0.206650;
    else return 0.243775;
  }
  
    // mistag, x-pT
    // from https://twiki.cern.ch/twiki/pub/CMS/BtagPOG/MistagFuncs.C
    
    if (pt>670) pt=670;
    else if (pt<20) pt=20;
    double _absEta = abs(eta);
    if( _absEta< 0.0 or _absEta > 2.4) return -100.;

    else if( tagger == "CSVT") return 0.00315116*(((1+(-0.00769281*pt))+(2.58066e-05*(pt*pt)))+(-2.02149e-08*(pt*(pt*pt))));
    else if( tagger == "JBPL"){
	if(_absEta<0.5) return (0.0277261+(0.000808207*pt))+(-6.44146e-07*(pt*pt));
	else if(_absEta<1.0) return (0.0278926+(0.000827697*pt))+(-7.01497e-07*(pt*pt));
	else if(_absEta<1.5) return (0.0221411+(0.000900444*pt))+(-6.52873e-07*(pt*pt));
	else return (0.0227045+(0.000808122*pt))+(-5.67134e-07*(pt*pt));
    }
    else if( tagger == "JBPM"){
	if( _absEta<0.8) return (((0.00206106+(0.000105851*pt))+(2.691e-08*(pt*pt)))+(-4.34651e-11*(pt*(pt*pt))))+(-6.73107e-14*(pt*(pt*(pt*pt))));
	else if( _absEta<1.6) return (((0.00318438+(4.40327e-05*pt))+(3.46922e-07*(pt*pt)))+(-3.93396e-10*(pt*(pt*pt))))+(3.94283e-14*(pt*(pt*(pt*pt))));
	else return (((0.00209833+(4.27753e-05*pt))+(1.96076e-07*(pt*pt)))+(6.19275e-11*(pt*(pt*pt))))+(-2.63318e-13*(pt*(pt*(pt*pt))));
    }
    else if( tagger == "JBPT") return (-3.36681e-05+(1.37292e-05*pt))+(1.78479e-08*(pt*pt));
    else if( tagger == "JPL" ){
	if( _absEta<0.5) return (0.060001+(0.000332202*pt))+(-2.36709e-07*(pt*pt));
	else if( _absEta<1.0) return (0.0597675+(0.000370979*pt))+(-2.94673e-07*(pt*pt));
	else if( _absEta<1.5) return (0.0483728+(0.000528418*pt))+(-3.17825e-07*(pt*pt));
	else return (0.0463159+(0.000546644*pt))+(-3.40486e-07*(pt*pt));
    }
    else if( tagger == "JPM"){
	if( _absEta<0.8) return (0.00727084+(4.48901e-05*pt))+(-4.42894e-09*(pt*pt));
	else if( _absEta<1.6) return (0.00389156+(6.35508e-05*pt))+(1.54183e-08*(pt*pt));
	else return (0.0032816+(4.18867e-05*pt))+(7.44912e-08*(pt*pt));
	}
    else if( tagger == "JPT" ) return (0.000379966+(8.30969e-06*pt))+(1.10364e-08*(pt*pt));
    else if( tagger == "SSVHEM"){
	if( _absEta<0.8) return (((0.000547883+(0.00023023*pt))+(-7.31792e-07*(pt*pt)))+(1.15659e-09*(pt*(pt*pt))))+(-7.00641e-13*(pt*(pt*(pt*pt))));
	else if( _absEta<1.6) return (((0.000615562+(0.000240254*pt))+(-7.00237e-07*(pt*pt)))+(1.2566e-09*(pt*(pt*pt))))+(-8.59011e-13*(pt*(pt*(pt*pt))));
	else return (((0.000372388+(0.000309735*pt))+(-4.35952e-07*(pt*pt)))+(3.63763e-10*(pt*(pt*pt))))+(-2.11993e-13*(pt*(pt*(pt*pt))));
    }
    else if( tagger == "SSVHPT" ) return (-2.9605e-05+(2.35624e-05*pt))+(-1.77552e-08*(pt*pt));
    else if( tagger == "TCHEL" ){
	if( _absEta<0.5) return (((-0.0235318+(0.00268868*pt))+(-6.47688e-06*(pt*pt)))+(7.92087e-09*(pt*(pt*pt))))+(-4.06519e-12*(pt*(pt*(pt*pt))));
	else if( _absEta<1.0) return (((-0.0257274+(0.00289337*pt))+(-7.48879e-06*(pt*pt)))+(9.84928e-09*(pt*(pt*pt))))+(-5.40844e-12*(pt*(pt*(pt*pt))));
	else if( _absEta<1.5) return (((-0.0310046+(0.00307803*pt))+(-7.94145e-06*(pt*pt)))+(1.06889e-08*(pt*(pt*pt))))+(-6.08971e-12*(pt*(pt*(pt*pt))));
	else return (((-0.0274561+(0.00301096*pt))+(-8.89588e-06*(pt*pt)))+(1.40142e-08*(pt*(pt*pt))))+(-8.95723e-12*(pt*(pt*(pt*pt))));
    }
    else if( tagger == "TCHEM"){
	if(_absEta<0.8) return (0.000919586+(0.00026266*pt))+(-1.75723e-07*(pt*pt));
	else if( _absEta<1.6) return (-0.00364137+(0.000350371*pt))+(-1.89967e-07*(pt*pt));
	else return (-0.00483904+(0.000367751*pt))+(-1.36152e-07*(pt*pt));
    }
    else if( tagger == "TCHPM"){
	if( _absEta<0.8) return (((-0.00464673+(0.000247485*pt))+(9.13236e-07*(pt*pt)))+(-2.49994e-09*(pt*(pt*pt))))+(1.65678e-12*(pt*(pt*(pt*pt))));
	else if( _absEta<1.6) return (((-0.0060878+(0.000297422*pt))+(1.13369e-06*(pt*pt)))+(-2.84945e-09*(pt*(pt*pt))))+(1.64721e-12*(pt*(pt*(pt*pt))));
	else if( _absEta<2.4) return (((-0.00836219+(0.000391889*pt))+(2.78156e-07*(pt*pt)))+(-6.14017e-10*(pt*(pt*pt))))+(-1.30592e-13*(pt*(pt*(pt*pt))));
    }
    else if( tagger == "TCHPT" ) return (-0.00101+(4.70405e-05*pt))+(8.3338e-09*(pt*pt));
    
    // unknown tagger, return default
    return -100.0;
}*/

/*.-----------------------------------------------------------------.
  /  .-.                                                         .-.  \
 |  /   \                                                       /   \  |
 | |\_.  |                                                     |    /| |
 |\|  | /|      UDSG SCALE FACTOR & UP/DOWNS SECTION           |\  | |/|
 | `---' |       aka Mistag Rate Scale Factors                 | `---' |
 |       |                                                     |       | 
 |       |-----------------------------------------------------|       |
 \       |                                                     |       /
  \     /                                                       \     /
   `---'                                                         `---'*/

double BtagHardcodedConditions::GetMistagScaleFactor(double pt, double eta, std::string tagger, int year)
{
    if(year == 2016) return GetLFSF2016( tagger, central, pt, eta);
    else if(year == 2015){
	// 2015 scale factors from csv file in https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation76X
	if(tagger == "CSVT") return 0.992339;
	else return GetMistagSF2015(pt,eta,tagger,"mean");
    }
    else if(year == 2012) return GetMistagSF2012(pt, eta, tagger, "mean");
    else if(year == 2011) return GetMistagSF2011(pt, eta, tagger, "mean");
    else return 0.;
}//end GetMistagScaleFactor

double BtagHardcodedConditions::GetMistagSFUncertDown(double pt, double eta, std::string tagger, int year)
{

    if(year == 2016) return (pt>1000 ? 2.0 : 1.0) * GetLFSF2016( tagger, uncert, pt, eta);
    else if(year==2015){
	if(tagger == "CSVT") return (pt>1000 ? 2.0 : 1.0) * (0.992339-0.810103);
	else return (pt>1000 ? 2.0 : 1.0) * (GetMistagSF2015(pt,eta,tagger,"mean") - GetMistagSF2015(pt,eta,tagger,"min"));
    }
    else if(year==2012) return (pt>800?2.0:1.0) * (GetMistagSF2012(pt, eta, tagger, "mean")- GetMistagSF2012(pt, eta, tagger, "min"));
    else if(year==2011) return (pt>670?2.0:1.0) * (GetMistagSF2011(pt, eta, tagger, "mean")- GetMistagSF2011(pt, eta, tagger, "min"));
    else return 0.;
}//end GetMistagSFUncertDown

double BtagHardcodedConditions::GetMistagSFUncertUp(double pt, double eta, std::string tagger, int year)
{
    if(year == 2016) return (pt>1000 ? 2.0 : 1.0) * GetLFSF2016( tagger, uncert, pt, eta);
    else if(year==2015){
	if(tagger == "CSVT") return (pt>1000 ? 2.0 : 1.0) * (1.17457-0.992339);
	else return (pt>1000 ? 2.0 : 1.0) * (GetMistagSF2015(pt,eta,tagger,"max") - GetMistagSF2015(pt,eta,tagger,"mean"));
    }
    else if(year==2012) return (pt>800?2.0:1.0) * (GetMistagSF2012(pt, eta, tagger, "max")- GetMistagSF2012(pt, eta, tagger, "mean"));
    else if(year==2011) return (pt>670?2.0:1.0) * (GetMistagSF2011(pt, eta, tagger, "max")- GetMistagSF2011(pt, eta, tagger, "mean"));
    else return 0.;
}//end GetMistagSFUncertUp

double BtagHardcodedConditions::GetLFSF2016(std::string tagger, shift Shift, double pt, double eta)
{
    if(      tagger == "CSVM" )      return GetLFSF2016Medium(Shift, pt, eta);
    else if( tagger == "CSVMsubjet") return GetLFSF2016Medium_subjet( Shift, pt, eta);
    else if( tagger == "CSVL"  )     return GetLFSF2016Loose( Shift, pt, eta);
    else if( tagger == "CSVLsubjet") return GetLFSF2016Loose_subjet(  Shift, pt, eta);
    else if( tagger == "CSVT" )      return GetLFSF2016Tight( Shift, pt, eta);
    else{
	throw cms::Exception("InvalidInput") << "Unknown tagger: "<< tagger << std::endl;
	//cout<<"Error, GetLFSF2016 received a tagger string it does not know how to interpret."<<endl;
	return 1.0;
    }
}//end main getter.

double BtagHardcodedConditions::GetLFSF2016Medium( shift Shift, double pt, double eta)
{
    if(pt > 1000.) pt = 1000.;
    if(fabs(eta) > 2.4 or pt<20.) return 1.;
    switch(Shift){
	case uncert:
	    return  (1.0589+0.000382569*pt+-2.4252e-07*pt*pt+2.20966e-10*pt*pt*pt)*((0.100485+3.95509e-05*pt+-4.90326e-08*pt*pt));
	case up:
	    return  (1.0589+0.000382569*pt+-2.4252e-07*pt*pt+2.20966e-10*pt*pt*pt)*(1+(0.100485+3.95509e-05*pt+-4.90326e-08*pt*pt));
	case down:
	    return  (1.0589+0.000382569*pt+-2.4252e-07*pt*pt+2.20966e-10*pt*pt*pt)*(1-(0.100485+3.95509e-05*pt+-4.90326e-08*pt*pt));
	case central:
	default:
	    return  1.0589+0.000382569*pt+-2.4252e-07*pt*pt+2.20966e-10*pt*pt*pt;
    }//end switch Shift
}//end GetLFSF2016

double BtagHardcodedConditions::GetLFSF2016Medium_subjet( shift Shift, double pt, double eta)
{
    if(pt > 1000.) pt = 1000.;
    if(fabs(eta) > 2.4 or pt<20.) return 1.;
    switch(Shift){
	case uncert:
		return GetLFSF2016Medium_subjet( up, pt, eta) - GetLFSF2016Medium_subjet( central, pt, eta);
	case up:
	    return  0.676736+0.00286128*pt+-4.34618e-06*pt*pt+2.44485e-09*pt*pt*pt;
	case down:
	    return  0.582177+0.00204348*pt+-2.94226e-06*pt*pt+1.6536e-09*pt*pt*pt;
	case central: 
	default:
	    return  0.629961+0.00245187*pt+-3.64539e-06*pt*pt+2.04999e-09*pt*pt*pt;
    }//end switch Shift
}//end GetLFSF2016

double BtagHardcodedConditions::GetLFSF2016Loose( shift Shift, double pt, double eta)
{
    if(pt > 1000.) pt = 1000.;
    if(fabs(eta) > 2.4 or pt<20.) return 1.;
    switch(Shift){
	case uncert:
	    return  (1.13904+-0.000594946*pt+1.97303e-06*pt*pt+-1.38194e-09*pt*pt*pt)*((0.0996438+-8.33354e-05*pt+4.74359e-08*pt*pt));
	case up:
	    return  (1.13904+-0.000594946*pt+1.97303e-06*pt*pt+-1.38194e-09*pt*pt*pt)*(1+(0.0996438+-8.33354e-05*pt+4.74359e-08*pt*pt));
	case down:
	    return  (1.13904+-0.000594946*pt+1.97303e-06*pt*pt+-1.38194e-09*pt*pt*pt)*(1-(0.0996438+-8.33354e-05*pt+4.74359e-08*pt*pt));
	case central:
	default:
	    return  1.13904+-0.000594946*pt+1.97303e-06*pt*pt+-1.38194e-09*pt*pt*pt;
    }//end switch Shift
}//end GetLFSF2016

double BtagHardcodedConditions::GetLFSF2016Loose_subjet( shift Shift, double pt, double eta)
{
    if(pt > 1000.) pt = 1000.;
    if(fabs(eta) > 2.4 or pt<20.) return 1.;
    switch(Shift){
	case uncert:
	    return GetLFSF2016Loose_subjet(up,pt,eta) - GetLFSF2016Loose_subjet(central,pt,eta);
	case up:
	    return  1.0358+0.000107516*pt+9.58049e-07*pt*pt+-8.59906e-10*pt*pt*pt;
	case down:
	    return  0.873638+0.0005247*pt+-3.15316e-07*pt*pt+4.83633e-11*pt*pt*pt;
	case central:
	default:
	    return  0.954689+0.000316059*pt+3.22024e-07*pt*pt+-4.06201e-10*pt*pt*pt;
    }//end switch Shift
}//end GetLFSF2016

double BtagHardcodedConditions::GetLFSF2016Tight( shift Shift, double pt, double eta)
{
    if(pt > 1000.) pt = 1000.;
    if(fabs(eta) > 2.4 or pt<20. ) return 1.;
    switch(Shift){
	case uncert:
	    return  (0.971945+163.215/(pt*pt)+0.000517836*pt)*((0.291298+-0.000222983*pt+1.69699e-07*pt*pt));
	case up:
	    return  (0.971945+163.215/(pt*pt)+0.000517836*pt)*(1+(0.291298+-0.000222983*pt+1.69699e-07*pt*pt));
	case down:
	    return  (0.971945+163.215/(pt*pt)+0.000517836*pt)*(1-(0.291298+-0.000222983*pt+1.69699e-07*pt*pt));
	case central:
	default:
	    return  0.971945+163.215/(pt*pt)+0.000517836*pt;
    }//end switch Shift
}//end GetLFSF2016


/*.-----------------------------------------------------------------.
  /  .-.                                                         .-.  \
 |  /   \                                                       /   \  |
 | |\_.  |                                                     |    /| |
 |\|  | /|            OUTDATED SCALE FACTOR SECTION            |\  | |/|
 | `---' |                                                     | `---' |
 |       |                                                     |       | 
 |       |-----------------------------------------------------|       |
 \       |                                                     |       /
  \     /                                                       \     /
   `---'                                                         `---'*/

// 2015 scale factors from csv file in https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation76X

double BtagHardcodedConditions::GetBtagScaleFactor2015(double pt, double eta,
						       std::string tagger){
    double SFb=0;
    if( tagger=="CSVL")  SFb = 0.908299+(2.70877e-06*(log(pt+370.144)*(log(pt+370.144)*(3-(-(104.614*log(pt+370.144)))))));
    else if( tagger=="CSVM")  SFb = -(0.0443172)+(0.00496634*(log(pt+1267.85)*(log(pt+1267.85)*(3-(-(0.110428*log(pt+1267.85)))))));
    else if( tagger=="CSVT")  SFb = -(5.1345)+(0.000820101*(log(pt+11518.1)*(log(pt+11518.1)*(3-(-(8.66128*log(pt+11518.1)))))));
    return SFb;
}

double BtagHardcodedConditions::GetBtagScaleFactor2012(double pt, double eta, std::string tagger)
{
    if (pt>800) pt=800;
    else if (pt<20) pt=20;
    
    double SFb=0;
    if( tagger=="JPL")   SFb = 0.977721*((1.+(-1.02685e-06*pt))/(1.+(-2.56586e-07*pt)));
    else if( tagger=="JPM")   SFb = 0.87887*((1.+(0.0393348*pt))/(1.+(0.0354499*pt)));
    else if( tagger=="JPT")    SFb = 0.802097*((1.+(0.013219*pt))/(1.+(0.0107842*pt)));
    else if( tagger=="TCHPT")  SFb = 0.305208*((1.+(0.595166*pt))/(1.+(0.186968*pt)));
    else if( tagger=="CSVL")  SFb = 0.981149*((1.+(-0.000713295*pt))/(1.+(-0.000703264*pt)));
    else if( tagger=="CSVM")  SFb = 0.726981*((1.+(0.253238*pt))/(1.+(0.188389*pt)));
    else if( tagger=="CSVT")  SFb = 0.869965*((1.+(0.0335062*pt))/(1.+(0.0304598*pt)));
    return SFb;
}


double BtagHardcodedConditions::GetBtagScaleFactor2011(double pt, double eta, std::string tagger)
{
    // This is 2011 muon-in-jet
    if (pt>670) pt=670;
    else if (pt<30) pt=30;
    
    double SFb=0;
    if( tagger=="JPL")   SFb = 0.969851*((1.+(-6.06362e-05*pt))/(1.+(-0.000156638*pt)));
    else if( tagger=="JPM")   SFb = 0.90806*((1.+(0.000236997*pt))/(1.+(5.49455e-05*pt)));
    else if( tagger=="JPT")    SFb = 0.835882*((1.+(0.00167826*pt))/(1.+(0.00120221*pt)));
    else if( tagger=="TCHPT")  SFb = 0.895596*((1.+(9.43219e-05*pt))/(1.+(-4.63927e-05*pt)));
    else if( tagger=="CSVL")  SFb = 1.02658*((1.+(0.0195388*pt))/(1.+(0.0209145*pt)));
    else if( tagger=="CSVM")  SFb = 0.6981*((1.+(0.414063*pt))/(1.+(0.300155*pt)));
    else if( tagger=="CSVT")  SFb = 0.901615*((1.+(0.552628*pt))/(1.+(0.547195*pt)));
    return SFb;
}

double BtagHardcodedConditions::GetMistagSF2015(double pt, double eta,
                                                std::string tagger, std::string meanminmax)
{
  
  double _absEta = abs(eta);
  double sf = -1;
  if( tagger=="CSVL" ){
    if(_absEta < 0.3) {
      if( meanminmax == "mean" ) sf = ((1.13199+(0.00121087*pt))+(-2.1714e-06*(pt*pt)))+(1.06583e-09*(pt*(pt*pt)));
      else if( meanminmax == "min" ) sf = ((1.09518+(0.000823444*pt))+(-1.1289e-06*(pt*pt)))+(3.82933e-10*(pt*(pt*pt))); 
      else if( meanminmax == "max" ) sf = ((1.1688+(0.00159691*pt))+(-3.21137e-06*(pt*pt)))+(1.74795e-09*(pt*(pt*pt)));
    }
    else if(_absEta<0.6) {
      if( meanminmax == "mean" ) sf = ((1.09165+(0.000702517*pt))+(-1.02871e-06*(pt*pt)))+(4.30424e-10*(pt*(pt*pt))) ;
      else if( meanminmax == "min" ) sf = ((1.05683+(0.000364968*pt))+(-1.36739e-07*(pt*pt)))+(-1.49029e-10*(pt*(pt*pt))) ;
      else if( meanminmax == "max" ) sf = ((1.12646+(0.00103951*pt))+(-1.91997e-06*(pt*pt)))+(1.00985e-09*(pt*(pt*pt))) ;
    }
    else if(_absEta<0.9) {
      if( meanminmax == "mean" ) sf =  ((1.0784+(0.000436514*pt))+(-4.80533e-07*(pt*pt)))+(1.33671e-10*(pt*(pt*pt))) ;
      else if( meanminmax == "min" ) sf = ((1.04698+(9.25694e-05*pt))+(4.4349e-07*(pt*pt)))+(-4.79468e-10*(pt*(pt*pt))) ;
      else if( meanminmax == "max" ) sf = ((1.10982+(0.000780264*pt))+(-1.40461e-06*(pt*pt)))+(7.47048e-10*(pt*(pt*pt))) ;
    }
    else if(_absEta<1.2) {
      if( meanminmax == "mean" ) sf = ((1.07503+(0.000526042*pt))+(-6.49158e-07*(pt*pt)))+(1.62348e-10*(pt*(pt*pt))) ;
      else if( meanminmax == "min" ) sf = ((1.04579+(0.000173093*pt))+(3.18263e-07*(pt*pt)))+(-4.91223e-10*(pt*(pt*pt))) ;
      else if( meanminmax == "max" ) sf = ((1.10426+(0.000878747*pt))+(-1.61676e-06*(pt*pt)))+(8.16318e-10*(pt*(pt*pt))) ;
    }
    else if(_absEta<1.5) {
      if( meanminmax == "mean" ) sf = ((1.04288+(0.000657559*pt))+(-1.06594e-06*(pt*pt)))+(3.86717e-10*(pt*(pt*pt))) ;
      else if( meanminmax == "min" ) sf = ((1.01419+(0.000577889*pt))+(-8.55682e-07*(pt*pt)))+(2.468e-10*(pt*(pt*pt))) ;
      else if( meanminmax == "max" ) sf = ((1.07156+(0.00073673*pt))+(-1.27588e-06*(pt*pt)))+(5.26892e-10*(pt*(pt*pt))) ;
    }
    else if(_absEta<1.8) {
      if( meanminmax == "mean" ) sf = ((1.03033+(0.000622107*pt))+(-1.09864e-06*(pt*pt)))+(4.7558e-10*(pt*(pt*pt))) ;
      else if( meanminmax == "min" ) sf = ((1.00339+(0.000508366*pt))+(-7.81139e-07*(pt*pt)))+(2.53078e-10*(pt*(pt*pt))) ;
      else if( meanminmax == "max" ) sf = ((1.05727+(0.00073532*pt))+(-1.4156e-06*(pt*pt)))+(6.98246e-10*(pt*(pt*pt))) ;
    }
    else if(_absEta<2.4) {
      if( meanminmax == "mean" ) sf = ((1.00087+(0.000789414*pt))+(-1.51105e-06*(pt*pt)))+(8.63264e-10*(pt*(pt*pt))) ;
      else if( meanminmax == "min" ) sf = ((0.976332+(0.000678258*pt))+(-1.19056e-06*(pt*pt)))+(6.19232e-10*(pt*(pt*pt))) ;
      else if( meanminmax == "max" ) sf = ((1.02541+(0.000899915*pt))+(-1.83065e-06*(pt*pt)))+(1.10768e-09*(pt*(pt*pt))) ;
    }
  }
  else if( tagger=="CSVM" ){
    if(_absEta<0.8) {
      if( meanminmax == "mean" ) sf = ((0.994351+(0.000250077*pt))+(9.24801e-07*(pt*pt)))+(-8.73293e-10*(pt*(pt*pt))) ;
      else if( meanminmax == "min" ) sf = ((0.949401+(-0.000356232*pt))+(2.24887e-06*(pt*pt)))+(-1.66011e-09*(pt*(pt*pt))) ;
      else if( meanminmax == "max" ) sf = ((1.03928+(0.000857422*pt))+(-4.02756e-07*(pt*pt)))+(-8.45836e-11*(pt*(pt*pt))) ;
    }
    else if(_absEta<1.6) {
      if( meanminmax == "mean" ) sf = ((1.00939+(0.000461283*pt))+(-6.30306e-07*(pt*pt)))+(3.53075e-10*(pt*(pt*pt))) ;
      else if( meanminmax == "min" ) sf = ((0.964857+(-2.19898e-05*pt))+(4.74117e-07*(pt*pt)))+(-3.36548e-10*(pt*(pt*pt))) ;
      else if( meanminmax == "max" ) sf = ((1.05392+(0.000944135*pt))+(-1.73386e-06*(pt*pt)))+(1.04242e-09*(pt*(pt*pt))) ;
    }
    else if(_absEta<2.4) {
      if( meanminmax == "mean" ) sf = ((0.955798+(0.00146058*pt))+(-3.76689e-06*(pt*pt)))+(2.39196e-09*(pt*(pt*pt))) ;
      else if( meanminmax == "min" ) sf = ((0.910086+(0.00116371*pt))+(-3.02747e-06*(pt*pt)))+(1.86906e-09*(pt*(pt*pt))) ;
      else if( meanminmax == "max" ) sf = ((1.00151+(0.00175547*pt))+(-4.50251e-06*(pt*pt)))+(2.91473e-09*(pt*(pt*pt))) ;
    }
  }

  return sf;

}



double BtagHardcodedConditions::GetMistagSF2011(double pt, double eta,
                                                std::string tagger, std::string meanminmax)
{
    double _absEta = abs(eta);
    double sf = -1;
    if ((pt<670)||(tagger[tagger.length()-1]=='T')) {
        if (pt<20) pt=20;
        else if (pt>670) pt=670;
        if( tagger=="CSVL"&& _absEta>=0.0 && _absEta<0.5) {
            if( meanminmax == "mean" ) sf = ((1.07536+(0.000175506*pt))+(-8.63317e-07*(pt*pt)))+(3.27516e-10*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.994425+(-8.66392e-05*pt))+(-3.03813e-08*(pt*pt)))+(-3.52151e-10*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.15628+(0.000437668*pt))+(-1.69625e-06*(pt*pt)))+(1.00718e-09*(pt*(pt*pt)));
        }
        else if( tagger=="CSVL" && _absEta>=0.5 && _absEta<1.0) {
            if( meanminmax == "mean" ) sf = ((1.07846+(0.00032458*pt))+(-1.30258e-06*(pt*pt)))+(8.50608e-10*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.998088+(6.94916e-05*pt))+(-4.82731e-07*(pt*pt)))+(1.63506e-10*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.15882+(0.000579711*pt))+(-2.12243e-06*(pt*pt)))+(1.53771e-09*(pt*(pt*pt)));
        }
        else if( tagger=="CSVL" && _absEta>=1.0 && _absEta<1.5) {
            if( meanminmax == "mean" ) sf = ((1.08294+(0.000474818*pt))+(-1.43857e-06*(pt*pt)))+(1.13308e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((1.00294+(0.000289844*pt))+(-7.9845e-07*(pt*pt)))+(5.38525e-10*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.16292+(0.000659848*pt))+(-2.07868e-06*(pt*pt)))+(1.72763e-09*(pt*(pt*pt)));
        }
        else if( tagger=="CSVL" && _absEta>=1.5 && _absEta<2.4) {
            if( meanminmax == "mean" ) sf = ((1.0617+(0.000173654*pt))+(-5.29009e-07*(pt*pt)))+(5.55931e-10*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.979816+(0.000138797*pt))+(-3.14503e-07*(pt*pt)))+(2.38124e-10*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.14357+(0.00020854*pt))+(-7.43519e-07*(pt*pt)))+(8.73742e-10*(pt*(pt*pt)));
        }
        else if( tagger=="CSVM" && _absEta>=0.0 && _absEta<0.8) {
            if( meanminmax == "mean" ) sf = ((1.06182+(0.000617034*pt))+(-1.5732e-06*(pt*pt)))+(3.02909e-10*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.972455+(7.51396e-06*pt))+(4.91857e-07*(pt*pt)))+(-1.47661e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.15116+(0.00122657*pt))+(-3.63826e-06*(pt*pt)))+(2.08242e-09*(pt*(pt*pt)));
        }
        else if( tagger=="CSVM" && _absEta>=0.8 && _absEta<1.6) {
            if( meanminmax == "mean" ) sf = ((1.111+(-9.64191e-06*pt))+(1.80811e-07*(pt*pt)))+(-5.44868e-10*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((1.02055+(-0.000378856*pt))+(1.49029e-06*(pt*pt)))+(-1.74966e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.20146+(0.000359543*pt))+(-1.12866e-06*(pt*pt)))+(6.59918e-10*(pt*(pt*pt)));
        }
        else if( tagger=="CSVM" && _absEta>=1.6 && _absEta<2.4) {
            if( meanminmax == "mean" ) sf = ((1.08498+(-0.000701422*pt))+(3.43612e-06*(pt*pt)))+(-4.11794e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.983476+(-0.000607242*pt))+(3.17997e-06*(pt*pt)))+(-4.01242e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.18654+(-0.000795808*pt))+(3.69226e-06*(pt*pt)))+(-4.22347e-09*(pt*(pt*pt)));
        }
        else if( tagger=="CSVT" && _absEta>=0.0 && _absEta<2.4) {
            if( meanminmax == "mean" ) sf = ((0.948463+(0.00288102*pt))+(-7.98091e-06*(pt*pt)))+(5.50157e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.899715+(0.00102278*pt))+(-2.46335e-06*(pt*pt)))+(9.71143e-10*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((0.997077+(0.00473953*pt))+(-1.34985e-05*(pt*pt)))+(1.0032e-08*(pt*(pt*pt)));
        }
        else if( tagger=="JBPL" && _absEta>=0.0 && _absEta<0.5) {
            if( meanminmax == "mean" ) sf = ((0.996303+(-0.00049586*pt))+(1.48662e-06*(pt*pt)))+(-1.60955e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.909313+(-0.000483037*pt))+(1.48507e-06*(pt*pt)))+(-1.60327e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.08332+(-0.000508763*pt))+(1.48816e-06*(pt*pt)))+(-1.61583e-09*(pt*(pt*pt)));
        }
        else if( tagger=="JBPL" && _absEta>=0.5 && _absEta<1.0) {
            if( meanminmax == "mean" ) sf = ((1.01607+(-0.000958122*pt))+(3.12318e-06*(pt*pt)))+(-3.13777e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.925793+(-0.000877501*pt))+(2.88538e-06*(pt*pt)))+(-2.9089e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.10639+(-0.0010389*pt))+(3.36098e-06*(pt*pt)))+(-3.36665e-09*(pt*(pt*pt)));
        }
        else if( tagger=="JBPL" && _absEta>=1.0 && _absEta<1.5) {
            if( meanminmax == "mean" ) sf = ((1.04234+(-0.00109152*pt))+(3.71686e-06*(pt*pt)))+(-3.57219e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.947786+(-0.000985917*pt))+(3.39659e-06*(pt*pt)))+(-3.28635e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.13696+(-0.00119731*pt))+(4.03713e-06*(pt*pt)))+(-3.85803e-09*(pt*(pt*pt)));
        }
        else if( tagger=="JBPL" && _absEta>=1.5 && _absEta<2.4) {
            if( meanminmax == "mean" ) sf = ((0.960685+(-0.000514241*pt))+(2.69297e-06*(pt*pt)))+(-3.12123e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.875356+(-0.000455763*pt))+(2.42337e-06*(pt*pt)))+(-2.83637e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.04606+(-0.000572874*pt))+(2.96257e-06*(pt*pt)))+(-3.40609e-09*(pt*(pt*pt)));
        }
        else if( tagger=="JBPM" && _absEta>=0.0 && _absEta<0.8) {
            if( meanminmax == "mean" ) sf = ((0.932447+(0.000285676*pt))+(-1.03771e-06*(pt*pt)))+(4.52275e-10*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.822274+(0.000138316*pt))+(-4.14616e-07*(pt*pt)))+(-9.7638e-11*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.0426+(0.000433059*pt))+(-1.6608e-06*(pt*pt)))+(1.00219e-09*(pt*(pt*pt)));
        }
        else if( tagger=="JBPM" && _absEta>=0.8 && _absEta<1.6) {
            if( meanminmax == "mean" ) sf = ((0.924959+(0.000170347*pt))+(-1.56056e-07*(pt*pt)))+(-2.06751e-10*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.822394+(-2.61379e-05*pt))+(6.08356e-07*(pt*pt)))+(-9.28476e-10*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.02752+(0.000366822*pt))+(-9.20467e-07*(pt*pt)))+(5.14974e-10*(pt*(pt*pt)));
        }
        else if( tagger=="JBPM" && _absEta>=1.6 && _absEta<2.4) {
            if( meanminmax == "mean" ) sf = ((0.846053+(0.000224848*pt))+(2.87503e-07*(pt*pt)))+(-5.93182e-10*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.714511+(0.000568422*pt))+(-7.56289e-07*(pt*pt)))+(2.61634e-10*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((0.977599+(-0.000118755*pt))+(1.3313e-06*(pt*pt)))+(-1.448e-09*(pt*(pt*pt)));
        }
        else if( tagger=="JBPT" && _absEta>=0.0 && _absEta<2.4) {
            if( meanminmax == "mean" ) sf = ((0.771257+(0.00238891*pt))+(-6.2112e-06*(pt*pt)))+(4.33595e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.666101+(0.00163462*pt))+(-3.92728e-06*(pt*pt)))+(2.48081e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((0.87631+(0.00314343*pt))+(-8.49513e-06*(pt*pt)))+(6.19109e-09*(pt*(pt*pt)));
        }
        else if( tagger=="JPL" && _absEta>=0.0 && _absEta<0.5) {
            if( meanminmax == "mean" ) sf = ((1.02571+(-0.000391686*pt))+(1.01948e-06*(pt*pt)))+(-1.16475e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.931859+(-0.00045457*pt))+(1.25431e-06*(pt*pt)))+(-1.36433e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.11958+(-0.00032886*pt))+(7.84649e-07*(pt*pt)))+(-9.65161e-10*(pt*(pt*pt)));
        }
        else if( tagger=="JPL" && _absEta>=0.5 && _absEta<1.0) {
            if( meanminmax == "mean" ) sf = ((1.03375+(-0.00068776*pt))+(2.13443e-06*(pt*pt)))+(-2.24163e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.936905+(-0.000681017*pt))+(2.13885e-06*(pt*pt)))+(-2.22607e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.13063+(-0.000694616*pt))+(2.13001e-06*(pt*pt)))+(-2.25719e-09*(pt*(pt*pt)));
        }
        else if( tagger=="JPL" && _absEta>=1.0 && _absEta<1.5) {
            if( meanminmax == "mean" ) sf = ((1.03597+(-0.000778058*pt))+(3.02129e-06*(pt*pt)))+(-3.0478e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.938438+(-0.00074623*pt))+(2.89732e-06*(pt*pt)))+(-2.92483e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.13355+(-0.000810039*pt))+(3.14525e-06*(pt*pt)))+(-3.17077e-09*(pt*(pt*pt)));
        }
        else if( tagger=="JPL" && _absEta>=1.5 && _absEta<2.4) {
            if( meanminmax == "mean" ) sf = ((0.95897+(-0.000111286*pt))+(1.6091e-06*(pt*pt)))+(-2.18387e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.867768+(-9.92078e-05*pt))+(1.46903e-06*(pt*pt)))+(-2.02118e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.0502+(-0.000123474*pt))+(1.74917e-06*(pt*pt)))+(-2.34655e-09*(pt*(pt*pt)));
        }
        else if( tagger=="JPM" && _absEta>=0.0 && _absEta<0.8) {
            if( meanminmax == "mean" ) sf = ((0.970028+(0.00118179*pt))+(-4.23119e-06*(pt*pt)))+(3.61065e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.840326+(0.000626372*pt))+(-2.08293e-06*(pt*pt)))+(1.57604e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.09966+(0.00173739*pt))+(-6.37946e-06*(pt*pt)))+(5.64527e-09*(pt*(pt*pt)));
        }
        else if( tagger=="JPM" && _absEta>=0.8 && _absEta<1.6) {
            if( meanminmax == "mean" ) sf = ((0.918387+(0.000898595*pt))+(-2.00643e-06*(pt*pt)))+(1.26486e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.790843+(0.000548016*pt))+(-6.70941e-07*(pt*pt)))+(1.90355e-11*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.0459+(0.00124924*pt))+(-3.34192e-06*(pt*pt)))+(2.51068e-09*(pt*(pt*pt)));
        }
        else if( tagger=="JPM" && _absEta>=1.6 && _absEta<2.4) {
            if( meanminmax == "mean" ) sf = ((0.790103+(0.00117865*pt))+(-2.07334e-06*(pt*pt)))+(1.42608e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.667144+(0.00105593*pt))+(-1.43608e-06*(pt*pt)))+(5.24039e-10*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((0.913027+(0.00130143*pt))+(-2.71061e-06*(pt*pt)))+(2.32812e-09*(pt*(pt*pt)));
        }
        else if( tagger=="JPT" && _absEta>=0.0 && _absEta<2.4) {
            if( meanminmax == "mean" ) sf = ((0.831392+(0.00269525*pt))+(-7.33391e-06*(pt*pt)))+(5.73942e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.671888+(0.0020106*pt))+(-5.03177e-06*(pt*pt)))+(3.74225e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((0.990774+(0.00338018*pt))+(-9.63606e-06*(pt*pt)))+(7.73659e-09*(pt*(pt*pt)));
        }
        else if( tagger=="SSVHEM" && _absEta>=0.0 && _absEta<0.8) {
            if( meanminmax == "mean" ) sf = ((0.86318+(0.000801639*pt))+(-1.64119e-06*(pt*pt)))+(2.59121e-10*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.790364+(0.000463086*pt))+(-4.35934e-07*(pt*pt)))+(-9.08296e-10*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((0.935969+(0.0011402*pt))+(-2.84645e-06*(pt*pt)))+(1.42654e-09*(pt*(pt*pt)));
        }
        else if( tagger=="SSVHEM" && _absEta>=0.8 && _absEta<1.6) {
            if( meanminmax == "mean" ) sf = ((0.958973+(-0.000269555*pt))+(1.381e-06*(pt*pt)))+(-1.87744e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.865771+(-0.000279908*pt))+(1.34144e-06*(pt*pt)))+(-1.75588e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.0522+(-0.000259296*pt))+(1.42056e-06*(pt*pt)))+(-1.999e-09*(pt*(pt*pt)));
        }
        else if( tagger=="SSVHEM" && _absEta>=1.6 && _absEta<2.4) {
            if( meanminmax == "mean" ) sf = ((0.923033+(-0.000898227*pt))+(4.74565e-06*(pt*pt)))+(-6.11053e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.828021+(-0.000731926*pt))+(4.19613e-06*(pt*pt)))+(-5.81379e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.01812+(-0.00106483*pt))+(5.29518e-06*(pt*pt)))+(-6.40728e-09*(pt*(pt*pt)));
        }
        else if( tagger=="SSVHPT" && _absEta>=0.0 && _absEta<2.4) {
            if( meanminmax == "mean" ) sf = ((0.97409+(0.000646241*pt))+(-2.86294e-06*(pt*pt)))+(2.79484e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.807222+(0.00103676*pt))+(-3.6243e-06*(pt*pt)))+(3.17368e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.14091+(0.00025586*pt))+(-2.10157e-06*(pt*pt)))+(2.41599e-09*(pt*(pt*pt)));
        }
        else if( tagger=="TCHEL" && _absEta>=0.0 && _absEta<0.5) {
            if( meanminmax == "mean" ) sf = (1.13615*((1+(-0.00119852*pt))+(1.17888e-05*(pt*pt))))+(-9.8581e-08*(pt*(pt*(pt/(1+(0.00689317*pt))))));
            else if( meanminmax == "min" ) sf = (1.0369*((1+(-0.000945578*pt))+(7.73273e-06*(pt*pt))))+(-4.47791e-08*(pt*(pt*(pt/(1+(0.00499343*pt))))));
            else if( meanminmax == "max" ) sf = (1.22179*((1+(-0.000946228*pt))+(7.37821e-06*(pt*pt))))+(-4.8451e-08*(pt*(pt*(pt/(1+(0.0047976*pt))))));
        }
        else if( tagger=="TCHEL" && _absEta>=0.5 && _absEta<1.0) {
            if( meanminmax == "mean" ) sf = (1.13277*((1+(-0.00084146*pt))+(3.80313e-06*(pt*pt))))+(-8.75061e-09*(pt*(pt*(pt/(1+(0.00118695*pt))))));
            else if( meanminmax == "min" ) sf = (0.983748*((1+(7.13613e-05*pt))+(-1.08648e-05*(pt*pt))))+(2.96162e-06*(pt*(pt*(pt/(1+(0.282104*pt))))));
            else if( meanminmax == "max" ) sf = (1.22714*((1+(-0.00085562*pt))+(3.74425e-06*(pt*pt))))+(-8.91028e-09*(pt*(pt*(pt/(1+(0.00109346*pt))))));
        }
        else if( tagger=="TCHEL" && _absEta>=1.0 && _absEta<1.5) {
            if( meanminmax == "mean" ) sf = (1.17163*((1+(-0.000828475*pt))+(3.0769e-06*(pt*pt))))+(-4.692e-09*(pt*(pt*(pt/(1+(0.000337759*pt))))));
            else if( meanminmax == "min" ) sf = (1.0698*((1+(-0.000731877*pt))+(2.56922e-06*(pt*pt))))+(-3.0318e-09*(pt*(pt*(pt/(1+(5.04118e-05*pt))))));
            else if( meanminmax == "max" ) sf = (1.27351*((1+(-0.000911891*pt))+(3.5465e-06*(pt*pt))))+(-6.69625e-09*(pt*(pt*(pt/(1+(0.000590847*pt))))));
        }
        else if( tagger=="TCHEL" && _absEta>=1.5 && _absEta<2.4) {
            if( meanminmax == "mean" ) sf = (1.14554*((1+(-0.000128043*pt))+(4.10899e-07*(pt*pt))))+(-2.07565e-10*(pt*(pt*(pt/(1+(-0.00118618*pt))))));
            else if( meanminmax == "min" ) sf = (1.04766*((1+(-6.87499e-05*pt))+(2.2454e-07*(pt*pt))))+(-1.18395e-10*(pt*(pt*(pt/(1+(-0.00128734*pt))))));
            else if( meanminmax == "max" ) sf = (1.24367*((1+(-0.000182494*pt))+(5.92637e-07*(pt*pt))))+(-3.3745e-10*(pt*(pt*(pt/(1+(-0.00107694*pt))))));
        }
        else if( tagger=="TCHEM" && _absEta>=0.0 && _absEta<0.8) {
            if( meanminmax == "mean" ) sf = (1.2875*((1+(-0.000356371*pt))+(1.08081e-07*(pt*pt))))+(-6.89998e-11*(pt*(pt*(pt/(1+(-0.0012139*pt))))));
            else if( meanminmax == "min" ) sf = (1.11418*((1+(-0.000442274*pt))+(1.53463e-06*(pt*pt))))+(-4.93683e-09*(pt*(pt*(pt/(1+(0.00152436*pt))))));
            else if( meanminmax == "max" ) sf = (1.47515*((1+(-0.000484868*pt))+(2.36817e-07*(pt*pt))))+(-2.05073e-11*(pt*(pt*(pt/(1+(-0.00142819*pt))))));
        }
        else if( tagger=="TCHEM" && _absEta>=0.8 && _absEta<1.6) {
            if( meanminmax == "mean" ) sf = (1.24986*((1+(-0.00039734*pt))+(5.37486e-07*(pt*pt))))+(-1.74023e-10*(pt*(pt*(pt/(1+(-0.00112954*pt))))));
            else if( meanminmax == "min" ) sf = (1.08828*((1+(-0.000208737*pt))+(1.50487e-07*(pt*pt))))+(-2.54249e-11*(pt*(pt*(pt/(1+(-0.00141477*pt))))));
            else if( meanminmax == "max" ) sf = (1.41211*((1+(-0.000559603*pt))+(9.50754e-07*(pt*pt))))+(-5.81148e-10*(pt*(pt*(pt/(1+(-0.000787359*pt))))));
        }
        else if( tagger=="TCHEM" && _absEta>=1.6 && _absEta<2.4) {
            if( meanminmax == "mean" ) sf = (1.10763*((1+(-0.000105805*pt))+(7.11718e-07*(pt*pt))))+(-5.3001e-10*(pt*(pt*(pt/(1+(-0.000821215*pt))))));
            else if( meanminmax == "min" ) sf = (0.958079*((1+(0.000327804*pt))+(-4.09511e-07*(pt*pt))))+(-1.95933e-11*(pt*(pt*(pt/(1+(-0.00143323*pt))))));
            else if( meanminmax == "max" ) sf = (1.26236*((1+(-0.000524055*pt))+(2.08863e-06*(pt*pt))))+(-2.29473e-09*(pt*(pt*(pt/(1+(-0.000276268*pt))))));
        }
        else if( tagger=="TCHPM" && _absEta>=0.0 && _absEta<0.8) {
            if( meanminmax == "mean" ) sf = ((1.27011+(-0.000869141*pt))+(2.49796e-06*(pt*pt)))+(-2.62962e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((1.12949+(-0.000678492*pt))+(2.02219e-06*(pt*pt)))+(-2.21675e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.41077+(-0.00105992*pt))+(2.97373e-06*(pt*pt)))+(-3.0425e-09*(pt*(pt*pt)));
        }
        else if( tagger=="TCHPM" && _absEta>=0.8 && _absEta<1.6) {
            if( meanminmax == "mean" ) sf = ((1.36167+(-0.00153237*pt))+(4.54567e-06*(pt*pt)))+(-4.38874e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((1.21289+(-0.00126411*pt))+(3.81676e-06*(pt*pt)))+(-3.75847e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.51053+(-0.00180085*pt))+(5.27457e-06*(pt*pt)))+(-5.01901e-09*(pt*(pt*pt)));
        }
        else if( tagger=="TCHPM" && _absEta>=1.6 && _absEta<2.4) {
            if( meanminmax == "mean" ) sf = ((1.22696+(0.000249231*pt))+(9.55279e-08*(pt*pt)))+(-1.04034e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((1.07572+(0.00055366*pt))+(-9.55796e-07*(pt*pt)))+(-3.73943e-11*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.3782+(-5.52498e-05*pt))+(1.14685e-06*(pt*pt)))+(-2.04329e-09*(pt*(pt*pt)));
        }
        else if( tagger=="TCHPT" && _absEta>=0.0 && _absEta<2.4) {
            if( meanminmax == "mean" ) sf = ((1.20711+(0.000681067*pt))+(-1.57062e-06*(pt*pt)))+(2.83138e-10*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((1.03418+(0.000428273*pt))+(-5.43024e-07*(pt*pt)))+(-6.18061e-10*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.38002+(0.000933875*pt))+(-2.59821e-06*(pt*pt)))+(1.18434e-09*(pt*(pt*pt)));
        }
    } else {
        pt=670;
        if( tagger=="CSVM" && _absEta>=0.0 && _absEta<2.4)
        {
            if( meanminmax == "mean" ) sf = ((1.04318+(0.000848162*pt))+(-2.5795e-06*(pt*pt)))+(1.64156e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.962627+(0.000448344*pt))+(-1.25579e-06*(pt*pt)))+(4.82283e-10*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.12368+(0.00124806*pt))+(-3.9032e-06*(pt*pt)))+(2.80083e-09*(pt*(pt*pt)));
        }
        else if( tagger=="CSVL" && _absEta>=0.0 && _absEta<2.4)
        {
            if( meanminmax == "mean" ) sf = ((1.0344+(0.000962994*pt))+(-3.65392e-06*(pt*pt)))+(3.23525e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.956023+(0.000825106*pt))+(-3.18828e-06*(pt*pt)))+(2.81787e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.11272+(0.00110104*pt))+(-4.11956e-06*(pt*pt)))+(3.65263e-09*(pt*(pt*pt)));
        }
        else if( tagger=="JBPL" && _absEta>=0.0 && _absEta<2.4)
        {
            if( meanminmax == "mean" ) sf = ((0.974968+(-0.000192541*pt))+(7.08162e-07*(pt*pt)))+(-9.7623e-10*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.891217+(-0.000201377*pt))+(7.41513e-07*(pt*pt)))+(-9.86349e-10*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.05873+(-0.000183755*pt))+(6.74811e-07*(pt*pt)))+(-9.6611e-10*(pt*(pt*pt)));
        }
        else if( tagger=="JBPM" && _absEta>=0.0 && _absEta<2.4)
        {
            if( meanminmax == "mean" ) sf = ((0.859232+(0.00117533*pt))+(-3.51857e-06*(pt*pt)))+(2.63162e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.769143+(0.000865281*pt))+(-2.47018e-06*(pt*pt)))+(1.72476e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((0.949262+(0.00148551*pt))+(-4.56695e-06*(pt*pt)))+(3.53847e-09*(pt*(pt*pt)));
        }
        else if( tagger=="JPL" && _absEta>=0.0 && _absEta<2.4)
        {
            if( meanminmax == "mean" ) sf = ((0.978061+(0.000211142*pt))+(-6.67003e-07*(pt*pt)))+(2.94232e-10*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.889182+(0.000124259*pt))+(-3.83838e-07*(pt*pt)))+(5.99164e-11*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.06693+(0.00029804*pt))+(-9.50169e-07*(pt*pt)))+(5.28547e-10*(pt*(pt*pt)));
        }
        else if( tagger=="JPM" && _absEta>=0.0 && _absEta<2.4)
        {
            if( meanminmax == "mean" ) sf = ((0.871294+(0.00215201*pt))+(-6.77675e-06*(pt*pt)))+(5.79197e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.7654+(0.00149792*pt))+(-4.47192e-06*(pt*pt)))+(3.67664e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((0.977076+(0.00280638*pt))+(-9.08158e-06*(pt*pt)))+(7.9073e-09*(pt*(pt*pt)));
        }
        else if( tagger=="SSVHEM" && _absEta>=0.0 && _absEta<2.4)
        {
            if( meanminmax == "mean" ) sf = ((0.890254+(0.000553319*pt))+(-1.29993e-06*(pt*pt)))+(4.19294e-10*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.817099+(0.000421567*pt))+(-9.46432e-07*(pt*pt)))+(1.62339e-10*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((0.963387+(0.000685092*pt))+(-1.65343e-06*(pt*pt)))+(6.76249e-10*(pt*(pt*pt)));
        }
        else if( tagger=="TCHEL" && _absEta>=0.0 && _absEta<2.4)
        {
            if( meanminmax == "mean" ) sf = (1.10649*((1+(-9.00297e-05*pt))+(2.32185e-07*(pt*pt))))+(-4.04925e-10*(pt*(pt*(pt/(1+(-0.00051036*pt))))));
            else if( meanminmax == "min" ) sf = (1.01541*((1+(-6.04627e-05*pt))+(1.38195e-07*(pt*pt))))+(-2.83043e-10*(pt*(pt*(pt/(1+(-0.000633609*pt))))));
            else if( meanminmax == "max" ) sf = (1.19751*((1+(-0.000114197*pt))+(3.08558e-07*(pt*pt))))+(-5.27598e-10*(pt*(pt*(pt/(1+(-0.000422372*pt))))));
        }
        else if( tagger=="TCHEM" && _absEta>=0.0 && _absEta<2.4)
        {
            if( meanminmax == "mean" ) sf = (1.06268*((1+(0.00390509*pt))+(-5.85405e-05*(pt*pt))))+(7.87135e-07*(pt*(pt*(pt/(1+(0.01259*pt))))));
            else if( meanminmax == "min" ) sf = (0.967092*((1+(0.00201431*pt))+(-1.49359e-05*(pt*pt))))+(6.94324e-08*(pt*(pt*(pt/(1+(0.00459787*pt))))));
            else if( meanminmax == "max" ) sf = (1.22691*((1+(0.00211682*pt))+(-2.07959e-05*(pt*pt))))+(1.72938e-07*(pt*(pt*(pt/(1+(0.00658853*pt))))));
        }
        else if( tagger=="TCHPM" && _absEta>=0.0 && _absEta<2.4)
        {
            if( meanminmax == "mean" ) sf = ((1.27417+(-0.000449095*pt))+(1.0719e-06*(pt*pt)))+(-1.35208e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((1.14205+(-0.000350151*pt))+(8.43333e-07*(pt*pt)))+(-1.14104e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.4063+(-0.000548107*pt))+(1.30047e-06*(pt*pt)))+(-1.56311e-09*(pt*(pt*pt)));
        }
    }
    
    return sf;
}

double BtagHardcodedConditions::GetMistagSF2012(double pt, double eta, std::string tagger, std::string meanminmax)
{
    
    double _absEta = abs(eta);
    double sf = -1;
    
    if ( (tagger[tagger.length()-1]=='T') ||
        ((tagger[tagger.length()-1]=='M') && (pt<800)) ||
        ((tagger[tagger.length()-1]=='L') && (pt<700) ) ) {
        
        if (pt<20) pt=20;
        else if (pt>800) pt=800;
        
        if( tagger=="CSVL"&& _absEta>=0.0 && _absEta<0.5) {
            if( meanminmax == "mean" ) sf = ((1.04901+(0.00152181*pt))+(-3.43568e-06*(pt*pt)))+(2.17219e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.973773+(0.00103049*pt))+(-2.2277e-06*(pt*pt)))+(1.37208e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.12424+(0.00201136*pt))+(-4.64021e-06*(pt*pt)))+(2.97219e-09*(pt*(pt*pt)));
        }
        else if( tagger=="CSVL" && _absEta>=0.5 && _absEta<1.0) {
            if( meanminmax == "mean" ) sf = ((0.991915+(0.00172552*pt))+(-3.92652e-06*(pt*pt)))+(2.56816e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.921518+(0.00129098*pt))+(-2.86488e-06*(pt*pt)))+(1.86022e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.06231+(0.00215815*pt))+(-4.9844e-06*(pt*pt)))+(3.27623e-09*(pt*(pt*pt)));
        }
        else if( tagger=="CSVL" && _absEta>=1.0 && _absEta<1.5) {
            if( meanminmax == "mean" ) sf = ((0.962127+(0.00192796*pt))+(-4.53385e-06*(pt*pt)))+(3.0605e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.895419+(0.00153387*pt))+(-3.48409e-06*(pt*pt)))+(2.30899e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.02883+(0.00231985*pt))+(-5.57924e-06*(pt*pt)))+(3.81235e-09*(pt*(pt*pt)));
        }
        else if( tagger=="CSVL" && _absEta>=1.5 && _absEta<2.4) {
            if( meanminmax == "mean" ) sf = ((1.06121+(0.000332747*pt))+(-8.81201e-07*(pt*pt)))+(7.43896e-10*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.983607+(0.000196747*pt))+(-3.98327e-07*(pt*pt)))+(2.95764e-10*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.1388+(0.000468418*pt))+(-1.36341e-06*(pt*pt)))+(1.19256e-09*(pt*(pt*pt)));
        }
        else if( tagger=="CSVM" && _absEta>=0.0 && _absEta<0.8) {
            if( meanminmax == "mean" ) sf = ((1.06238+(0.00198635*pt))+(-4.89082e-06*(pt*pt)))+(3.29312e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.972746+(0.00104424*pt))+(-2.36081e-06*(pt*pt)))+(1.53438e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.15201+(0.00292575*pt))+(-7.41497e-06*(pt*pt)))+(5.0512e-09*(pt*(pt*pt)));
        }
        else if( tagger=="CSVM" && _absEta>=0.8 && _absEta<1.6) {
            if( meanminmax == "mean" ) sf = ((1.08048+(0.00110831*pt))+(-2.96189e-06*(pt*pt)))+(2.16266e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.9836+(0.000649761*pt))+(-1.59773e-06*(pt*pt)))+(1.14324e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.17735+(0.00156533*pt))+(-4.32257e-06*(pt*pt)))+(3.18197e-09*(pt*(pt*pt)));
        }
        else if( tagger=="CSVM" && _absEta>=1.6 && _absEta<2.4) {
            if( meanminmax == "mean" ) sf = ((1.09145+(0.000687171*pt))+(-2.45054e-06*(pt*pt)))+(1.7844e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((1.00616+(0.000358884*pt))+(-1.23768e-06*(pt*pt)))+(6.86678e-10*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.17671+(0.0010147*pt))+(-3.66269e-06*(pt*pt)))+(2.88425e-09*(pt*(pt*pt)));
        }
        else if( tagger=="CSVT" && _absEta>=0.0 && _absEta<2.4) {
            if( meanminmax == "mean" ) sf = ((1.01739+(0.00283619*pt))+(-7.93013e-06*(pt*pt)))+(5.97491e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.953587+(0.00124872*pt))+(-3.97277e-06*(pt*pt)))+(3.23466e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.08119+(0.00441909*pt))+(-1.18764e-05*(pt*pt)))+(8.71372e-09*(pt*(pt*pt)));
        }
        
        else if( tagger == "JPL" && _absEta >= 0.0 && _absEta < 0.5){
            if( meanminmax == "mean" ) sf = ((1.05617+(0.000986016*pt))+(-2.05398e-06*(pt*pt)))+(1.25408e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.918762+(0.000749113*pt))+(-1.48511e-06*(pt*pt)))+(8.78559e-10*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.19358+(0.00122182*pt))+(-2.62078e-06*(pt*pt)))+(1.62951e-09*(pt*(pt*pt)));
        }
        else if( tagger == "JPL" && _absEta >= 0.0 && _absEta < 2.4){
            if( meanminmax == "mean" ) sf = ((1.04356+(0.000798695*pt))+(-1.83026e-06*(pt*pt)))+(1.19459e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.909334+(0.000638944*pt))+(-1.43578e-06*(pt*pt)))+(9.25276e-10*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.17779+(0.000957469*pt))+(-2.22278e-06*(pt*pt)))+(1.46383e-09*(pt*(pt*pt)));
        }
        else if( tagger == "JPL" && _absEta >= 0.5 && _absEta < 1.0){
            if( meanminmax == "mean" ) sf = ((1.02884+(0.000471854*pt))+(-1.15441e-06*(pt*pt)))+(7.83716e-10*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.893017+(0.000369124*pt))+(-8.68577e-07*(pt*pt)))+(5.79006e-10*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.16466+(0.000573985*pt))+(-1.43899e-06*(pt*pt)))+(9.88387e-10*(pt*(pt*pt)));
        }
        else if( tagger == "JPL" && _absEta >= 1.0 && _absEta < 1.5){
            if( meanminmax == "mean" ) sf = ((1.02463+(0.000907924*pt))+(-2.07133e-06*(pt*pt)))+(1.37083e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.89415+(0.000712877*pt))+(-1.57703e-06*(pt*pt)))+(1.02034e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.15511+(0.00110197*pt))+(-2.56374e-06*(pt*pt)))+(1.72152e-09*(pt*(pt*pt)));
        }
        else if( tagger == "JPL" && _absEta >= 1.5 && _absEta < 2.4){
            if( meanminmax == "mean" ) sf = ((1.05387+(0.000951237*pt))+(-2.35437e-06*(pt*pt)))+(1.66123e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.918611+(0.000781707*pt))+(-1.8923e-06*(pt*pt)))+(1.312e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.1891+(0.00112006*pt))+(-2.81586e-06*(pt*pt)))+(2.01249e-09*(pt*(pt*pt)));
        }
        else if( tagger == "JPM" && _absEta >= 0.0 && _absEta < 0.8){
            if( meanminmax == "mean" ) sf = ((0.980407+(0.00190765*pt))+(-4.49633e-06*(pt*pt)))+(3.02664e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.813164+(0.00127951*pt))+(-2.74274e-06*(pt*pt)))+(1.78799e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.14766+(0.00253327*pt))+(-6.24447e-06*(pt*pt)))+(4.26468e-09*(pt*(pt*pt)));
        }
        else if( tagger == "JPM" && _absEta >= 0.0 && _absEta < 2.4){
            if( meanminmax == "mean" ) sf = ((0.980066+(0.00222324*pt))+(-5.51689e-06*(pt*pt)))+(3.84294e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.827418+(0.00152453*pt))+(-3.56396e-06*(pt*pt)))+(2.44144e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.13272+(0.00291881*pt))+(-7.46281e-06*(pt*pt)))+(5.24363e-09*(pt*(pt*pt)));
        }
        else if( tagger == "JPM" && _absEta >= 0.8 && _absEta < 1.6){
            if( meanminmax == "mean" ) sf = ((1.01783+(0.00183763*pt))+(-4.64972e-06*(pt*pt)))+(3.34342e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.860873+(0.00110031*pt))+(-2.48023e-06*(pt*pt)))+(1.73776e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.17479+(0.00257252*pt))+(-6.81377e-06*(pt*pt)))+(4.94891e-09*(pt*(pt*pt)));
        }
        else if( tagger == "JPM" && _absEta >= 1.6 && _absEta < 2.4){
            if( meanminmax == "mean" ) sf = ((0.866685+(0.00396887*pt))+(-1.11342e-05*(pt*pt)))+(8.84085e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.740983+(0.00302736*pt))+(-8.12284e-06*(pt*pt)))+(6.281e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((0.992297+(0.00490671*pt))+(-1.41403e-05*(pt*pt)))+(1.14097e-08*(pt*(pt*pt)));
        }
        else if( tagger == "JPT" && _absEta >= 0.0 && _absEta < 2.4){
            if( meanminmax == "mean" ) sf = ((0.89627+(0.00328988*pt))+(-8.76392e-06*(pt*pt)))+(6.4662e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.666092+(0.00262465*pt))+(-6.5345e-06*(pt*pt)))+(4.73926e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.12648+(0.00394995*pt))+(-1.0981e-05*(pt*pt)))+(8.19134e-09*(pt*(pt*pt)));
        }
        else if( tagger == "TCHPT" && _absEta >= 0.0 && _absEta < 2.4){
            if( meanminmax == "mean" ) sf = ((1.1676+(0.00136673*pt))+(-3.51053e-06*(pt*pt)))+(2.4966e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.988346+(0.000914722*pt))+(-2.37077e-06*(pt*pt)))+(1.72082e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.34691+(0.00181637*pt))+(-4.64484e-06*(pt*pt)))+(3.27122e-09*(pt*(pt*pt)));
        }
        
    } else {
        if (pt>800) pt=800;
        if( tagger=="CSVM" && _absEta>=0.0 && _absEta<2.4)
        {
            if( meanminmax == "mean" )     sf = ((1.07585+(0.00119553*pt))+(-3.00163e-06*(pt*pt)))+(2.10724e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.987005+(0.000726254*pt))+(-1.73476e-06*(pt*pt)))+(1.20406e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.1647+(0.00166318*pt))+(-4.26493e-06*(pt*pt)))+(3.01017e-09*(pt*(pt*pt)));
        }
        else if( tagger=="CSVL" && _absEta>=0.0 && _absEta<2.4)
        {
            if( meanminmax == "mean" )     sf = ((1.02804+(0.000869782*pt))+(-1.69179e-06*(pt*pt)))+(1.03241e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.952169+(0.000693017*pt))+(-1.2994e-06*(pt*pt)))+(7.72617e-10*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.10391+(0.00104574*pt))+(-2.0828e-06*(pt*pt)))+(1.2924e-09*(pt*(pt*pt)));
        }
        else if( tagger=="JPL" && _absEta>=0.0 && _absEta<2.4)
        {
            if( meanminmax == "mean" )     sf = ((1.04356+(0.000798695*pt))+(-1.83026e-06*(pt*pt)))+(1.19459e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.909334+(0.000638944*pt))+(-1.43578e-06*(pt*pt)))+(9.25276e-10*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.17779+(0.000957469*pt))+(-2.22278e-06*(pt*pt)))+(1.46383e-09*(pt*(pt*pt)));
        }
        else if( tagger=="JPM" && _absEta>=0.0 && _absEta<2.4)
        {
            if( meanminmax == "mean" )     sf = ((0.980066+(0.00222324*pt))+(-5.51689e-06*(pt*pt)))+(3.84294e-09*(pt*(pt*pt)));
            else if( meanminmax == "min" ) sf = ((0.827418+(0.00152453*pt))+(-3.56396e-06*(pt*pt)))+(2.44144e-09*(pt*(pt*pt)));
            else if( meanminmax == "max" ) sf = ((1.13272+(0.00291881*pt))+(-7.46281e-06*(pt*pt)))+(5.24363e-09*(pt*(pt*pt)));
        }
    }
    
    return sf;
}//end GetMistagSF2012

double BtagHardcodedConditions::GetBtagSFUncertainty2011(double pt, double eta,
                                                         std::string tagger)
{
    if (pt<30) return 0.12;
    int bin = findBin(pt, ptRange11);
    float err = -1;
    
    if( tagger=="JPL")   	   err =  SFb_JPL_error11[bin];
    else if( tagger=="JPM")  err =  SFb_JPM_error11[bin];
    else if( tagger=="JPT")  err =  SFb_JPT_error11[bin];
    else if( tagger=="TCHPT")err =  SFb_TCHPT_error11[bin];
    else if( tagger=="CSVL") err =  SFb_CSVL_error11[bin];
    else if( tagger=="CSVM") err =  SFb_CSVM_error11[bin];
    else if( tagger=="CSVT") err =  SFb_CSVT_error11[bin];
    
    if (pt>670) err*=2.0;
    return err;
}

double BtagHardcodedConditions::GetBtagSFUncertainty2012(double pt, double eta, std::string tagger)
{
    int bin = findBin(pt, ptRange12);
    float err = -1;
    
    if( tagger=="JPL")   	   err =  SFb_JPL_error12[bin];
    else if( tagger=="JPM")  err =  SFb_JPM_error12[bin];
    else if( tagger=="JPT")  err =  SFb_JPT_error12[bin];
    else if( tagger=="TCHPT")err =  SFb_TCHPT_error12[bin];
    else if( tagger=="CSVL") err =  SFb_CSVL_error12[bin];
    else if( tagger=="CSVM") err =  SFb_CSVM_error12[bin];
    else if( tagger=="CSVT") err =  SFb_CSVT_error12[bin];
    
    if ((pt>670) || (pt<20)) err*=2.0;
    return err;
}

// 2015 scale factors from csv file in https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation74X50ns
double BtagHardcodedConditions::GetBtagSFUncertainty2015(double pt, double eta,
                                                         std::string tagger)
{
    int bin = findBin(pt, ptRange15);
    float err = -1;
    
    if( tagger=="CSVL") err =  SFb_CSVL_error15[bin];
    else if( tagger=="CSVM") err =  SFb_CSVM_error15[bin];
    else if( tagger=="CSVT") err =  SFb_CSVT_error15[bin];
    
    if ((pt>670) || (pt<30)) err*=2.0;
    return err;
}
