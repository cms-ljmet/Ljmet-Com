#!/bin/csh
#
# Template of the c-shell script for submitting a CONDOR job
#
# Need to be submitted as arguments from condor .jdl file:
#    - CMSSWBASE       - local release base ($CMSSW_BASE)
#    - OUTPUT_DIR      - condor work and output dir
#    - PREFIX          - some generic name for the set of jobs (like ttbar, wjets)
#    - CONDOR_PROCESS  - condor job number (ranges from 0 to N-1 jobs)
#    - CONDOR_CLUSTER  - condor cluster (for recordkeeping)
#
set CMSSWBASE=$1
set OUTPUT_DIR=$2
set PREFIX=$3
set CONDOR_PROCESS=$4
set CONDOR_CLUSTER=$5

@ JOBID = $CONDOR_PROCESS + 1

#
#_____ setup the environment ____________________________________________
#
setenv PATH /bin:/usr/bin:/usr/local/bin:/usr/krb5/bin:/usr/afsws/bin:/usr/krb5/bin/aklog

cd ${CMSSWBASE}/src/

setenv SCRAM_ARCH slc6_amd64_gcc491
SETUP
eval `scramv1 runtime -csh`
rehash

cp ${OUTPUT_DIR}/${PREFIX}_${JOBID}.py ${_CONDOR_SCRATCH_DIR}/${PREFIX}_${CONDOR_CLUSTER}_${JOBID}.py
cd ${_CONDOR_SCRATCH_DIR}
ljmet ${PREFIX}_${CONDOR_CLUSTER}_${JOBID}.py
