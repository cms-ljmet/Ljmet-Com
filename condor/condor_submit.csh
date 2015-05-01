#!/bin/tcsh

#### Possible options
#### --useMC    Use MC (True) or not (False)
#### --sample   Type of sample (e.g. TTbar)
#### --dataType Type of real data (ElEl, ElMu or MuMu). Not required for MC
#### --listFile File with a list of root files to be processed
#### --outDir   Directory to put output of script
#### --submit   Whether to submit the jobs to condor (True or False)

#MC example
python condor_submit.py --useMC True --sample DYToLL  --fileList /uscms_data/d3/jmanagan/CMSSW_7_3_0/src/LJMet/TTsamples/DYJetsToLL_Phys14PU20.txt  --submit True --outDir /uscms_data/d3/jmanagan/CMSSW_7_3_0/src/LJMet/TTrootfiles

#Data example with JSON file
#python condor_submit.py --useMC False --sample DoubleMu_Run2012A_13Jul2012 --dataType MuMu --json Cert_190456-196531_8TeV_13Jul2012ReReco_Collisions12_JSON_v2.txt --fileList DoubleMuon_Run2012A-13Jul2012.txt --submit True
