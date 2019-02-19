import os,sys,datetime

shift = sys.argv[1]

cernList=[

	# SHOULD BE AT CERN, USE eoscms.cern.ch in condor_submit.py
	# MOVE THINGS HERE TO REMIND YOURSELF WHAT IS AT CERN

	#### TTbar

	]

externalList = [
	# USE cmsxrootd.fnal.gov in condor_submit.py UNLESS YOU CHECK THE LIST LOG FOR CERN PRESENCE
	#'SingleElectron_Mar2018.txt'
	#'SingleMuon_Mar2018.txt'
]
	
print '====== LJMET SUBMISSION ======'
	
relBase = os.environ['CMSSW_BASE']
print 'Relbase:',relBase

thisDir = relBase+'/src/LJMet/Com/condor_TTinclusive/' 
tarfile = relBase+'.tar'
print 'Making tar:'
if os.path.exists(tarfile):
	print 'tar already exists! Will not re-tar!'
else: 
	os.chdir(relBase)
	os.chdir('../')
	# YOU NEED TO EXCLUDE ANYTHING ELSE THAT MIGHT LIVE IN THE SAME CMSSW RELEASE
	print 'tar --exclude="src/LJMet/Com/.git" --exclude="src/NNKit/.git" --exclude="src/.git" --exclude="src/LJMet-Slimmer" --exclude="tmp" -zcf'+tarfile+' '+relBase.split('/')[-1]+'/'
	os.system('tar --exclude="src/LJMet/Com/.git" --exclude="src/NNKit/.git" --exclude="src/.git" --exclude="src/LJMet-Slimmer" --exclude="tmp" -zcf '+tarfile+' '+relBase.split('/')[-1])
	os.chdir(thisDir)

for sample in cernList:
	GenHT = False
	accessor = 'eoscms.cern.ch'
	if 'HT' in sample and 'madgraphMLM' in sample: GenHT = True
	os.system('python condor_submitDeepAK8.py --useMC True --sample '+sample.split('.')[0]+' --fileList '+thisDir+'fileListsSummer18/'+sample+' --submit True --inputTar '+tarfile+' --outDir /eos/uscms/store/user/saj32265/TT2018/LJMet94X_1lepTT_081518 --shift '+shift+' --saveGenHT '+str(GenHT)+' --accessor '+accessor)

for sample in externalList:
	GenHT = False
	accessor = 'cmsxrootd.fnal.gov'
	if 'HT' in sample and 'madgraphMLM' in sample: GenHT = True
	os.system('python condor_submitDeepAK8.py --useMC False --sample '+sample.split('.')[0]+' --json Cert_294927-306462_13TeV_PromptReco_Collisions17_JSON.txt --fileList '+thisDir+'fileListsSummer18/'+sample+' --submit True --inputTar '+tarfile+' --outDir /eos/uscms/store/user/saj32265/TT2018/LJMet94X_1lepTT_081518 --shift '+shift+' --accessor '+accessor)

## shift should be (one at a time): nominal, JECup, JECdown, JERup, JERdown
## If you want to use different directory names, edit lines 144 - 147 in condor_submit.py so the config is edited correctly
