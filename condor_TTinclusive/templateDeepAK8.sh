#!/bin/bash
#
# Template of the c-shell script for submitting a CONDOR job
#
# Need to be submitted as arguments from condor .jdl file:
#    - CMSSWBASE       - local release base ($CMSSW_BASE)
#    - OUTPUT_DIR      - condor work and output dir
#    - PREFIX          - some generic name for the set of jobs (like ttbar, wjets)
#    - CONDOR_PROCESS  - condor job number (ranges from 0 to N-1 jobs)
#    - CONDOR_CLUSTER  - condor cluster (for recordkeeping)
# Arguments = INPUTTAR OUTPUT_DIR PREFIX $(process)
TARDIR=$1
INPUTTAR=$2
OUTPUT_DIR=$3
PREFIX=$4
JOBID=$5

uname -n
export SCRAM_ARCH=slc6_amd64_gcc630
source /cvmfs/cms.cern.ch/cmsset_default.sh

echo "running scramv1 project CMSSW CMSSW_9_4_12"
eval `scramv1 project CMSSW CMSSW_9_4_12`

echo "cd to CMSSW_9_4_12"
cd CMSSW_9_4_12
pwd

echo "coping in tarball"
xrdcp ${TARDIR}/${INPUTTAR}.tar . 2>&1
XRDEXIT=$?
if [[ $XRDEXIT -ne 0 ]]; then
    echo "exit code $XRDEXIT, failure in xrdcp of tarball"
    exit $XRDEXIT
fi

echo "unpacking tarball"
tar -xf ${INPUTTAR}.tar
echo "removing tar from condor"
rm -f ${INPUTTAR}.tar

#echo "listing tar content"
#ls -la
echo "running cmsenv"
eval `scramv1 runtime -sh`

echo "testing which ljmet"
which ljmet
echo "testing ldd cmsRun"
ldd cmsRun

cd -

echo "Running deepAK8 producer"
cmsRun producer_${PREFIX}_${JOBID}.py

echo "Sleeping for one minute..."
sleep 60

echo "Setting new PDFSET path"
export LHAPDF_DATA_PATH="/cvmfs/cms.cern.ch/lhapdf/pdfsets/current/":${LHAPDF_DATA_PATH}

echo "Running ljmet"
sed -i "s|CONDOR_RELBASE|$PWD/$INPUTTAR|" ${PREFIX}_${JOBID}.py
ljmet ${PREFIX}_${JOBID}.py

echo "Deleting the mediator MiniAOD file"
rm mediator*.root

# copy output to eos
echo "xrdcp .root output for condor"
for FILE in *.root
do
  echo "xrdcp -f ${FILE} ${OUTDIR}/${FILE}"
  xrdcp -f ${FILE} ${OUTPUT_DIR}/${FILE} 2>&1
  XRDEXIT=$?
  if [[ $XRDEXIT -ne 0 ]]; then
    rm *.root
    echo "exit code $XRDEXIT, failure in xrdcp"
    exit $XRDEXIT
  fi
  rm ${FILE}
done


