#!/usr/bin/python
execfile("/uscms_data/d3/jmanagan/EOSSafeUtils.py")

import os
import re
import sys
import fileinput
import getopt
import subprocess
import socket
import datetime
files_per_job = 2

#Configuration options parsed from arguments
#Switching to getopt for compatibility with older python
try:
    opts, args = getopt.getopt(sys.argv[1:], "", ["useMC=", "sample=","fileList=", "outDir=","shift=","submit=","json=","inputTar=","saveGenHT=","newpdf=","isTTbar=","accessor="])
except getopt.GetoptError as err:
    print str(err)
    sys.exit(1)

useMC    = bool(False)
prefix   = str('None')
fileList = str('None')
outDir   = str('None')
shift    = str('None')
json     = str('None')
submit   = bool(False)
changeJEC= bool(False)
tarfile  = str('None')
saveGHT  = str('False')
newpdf   = str('False')
isTTbar  = str('False')
accessor = str('None')

for o, a in opts:
    print o, a
    if o == "--useMC":
        checkUseMC = a in ['True', 'False']
        if not checkUseMC:
            print '--useMC must be True or False!'
            print 'Setting useMC to False'
        else:
            useMC = a
    elif o == "--sample":   prefix   = str(a)
    elif o == "--fileList": fileList = str(a)
    elif o == "--outDir":   outDir   = str(a)
    elif o == "--shift":    shift    = str(a)
    elif o == "--inputTar": tarfile = str(a).split('.')[0]
    elif o == "--json":     json   = str(a)
    elif o == "--saveGenHT": saveGHT = str(a)
    elif o == "--newpdf": newpdf = str(a)
    elif o == "--isTTbar": isTTbar = str(a)
    elif o == "--accessor": accessor = str(a)
    elif o == "--submit":
        if a == 'True':     submit = True


#Absolute path that precedes '/store...'
# The script checks whether it is an absolute path or not

if (socket.gethostname().find('fnal')>=0):
    brux=bool(False)
#    sePath='\'root://cmseos.fnal.gov/'         # stored on LPC, most of our samples
#    sePath='\'root://eoscms.cern.ch/'      # stored at CERN
    sePath='\'root://'+accessor+'/'      # stored elsewhere
    storePath=''
    setupString='source \/cvmfs\/cms.cern.ch\/cmsset_default.csh'
    outPath='root://cmseos.fnal.gov/'          # output on LPC
    print 'Submitting jobs at FNAL, reading files from: '+sePath+', writing files to: '+outPath
else:
    print "Script not done for ", socket.gethostname()
    exit()

relBase = os.environ['CMSSW_BASE']

files = fileList

if (not os.path.isfile(files)):
    print 'File with input root files '+ fileList +' not found. Please give absolute path'
    sys.exit(1)

outputdir = os.path.abspath('.')
if str(outDir) != 'None': outputdir = outDir
else:                     print 'Output dir not specified. Setting it to current directory.'

if submit: print 'Will submit jobs to Condor'
else:      print 'Will not submit jobs to Condor'

dir = outputdir+'/'+shift+'/'+prefix
xrddir = outPath+dir[10:]
tardir = outPath+outputdir[10:]

if EOSpathExists(dir):
    print 'Output directory already exists!'
    sys.exit(1)
else:
    if dir[:4] == '/eos':
        print 'Making outputDir via eos command'
        os.system('eos root://cmseos.fnal.gov/ mkdir -p '+dir[10:])
    else:
        os.system('mkdir -p '+dir)

#mediatorDir = '/store/user/lpcljm/GenTTbarIdMiniAOD/'+prefix
#if shift == 'nominal':
#    if EOSpathExists(mediatorDir):
#        print 'Output GenTTbarID file directory already exists:',mediatorDir
#        sys.exit(1)
#    else:
#        if dir[:6] == '/store':
#            print 'Making GenTTbarId dir via eos command'
#            os.system('eos root://cmseos.fnal.gov/ mkdir -p '+mediatorDir)
#        else:
#            os.system('mkdir -p '+dir)

if not EOSisfile(outputdir[10:]+'/'+tarfile.split('/')[-1]+'.tar'):
    print 'COPYING TAR TO EOS: '+tardir
    os.system('xrdcp -f '+tarfile+'.tar '+tardir+'/')
else: print 'TAR ON EOS, NOT COPYING'

def get_input(num, list):
    result = ''
    file_list = open(files)
    file_count = 0
    for line in file_list:
        if line.find('root')>0:
            file_count=file_count+1
            if file_count>(num-1) and file_count<(num+files_per_job):
                result=result+ sePath
		if (line.strip()[:6]=='/store'):
		    result=result+ storePath
                result=result+line.strip()+'\',\n'
    file_list.close()
    return result

j = 1
nfiles = 1

tempdir = '/uscms_data/d3/yiting11/'+outputdir.split('/')[-1]+'_logs/'+shift+'/'+prefix
if not os.path.exists(tempdir): os.makedirs(tempdir)

print 'CONDOR work dir: '+tempdir

count = 0
file_list = open(files)
for line in file_list:
    if line.find('.root')>0:
        count = count + 1

file_list.close()

# only produce the miniAOD for nominal
if shift == 'nominal': os.system('sed -e \'s/SETUP/'+setupString+'/g\' templateDeepAK8.sh > '+tempdir+'/'+prefix+'.sh')
else: os.system('sed -e \'s/SETUP/'+setupString+'/g\' templateDeepAK8.sh > '+tempdir+'/'+prefix+'.sh')

while ( nfiles <= count ):

    # ADD YOUR CONFIG FILE HERE!!
    #Avoid calling the get_input function once per line in template
    singleFileList = get_input(nfiles, files)
    mediatorFile = 'mediator_'+prefix+'_'+str(j)+'.root'

    genId_templ_file = open(relBase+'/src/LJMet/Com/condor_TTinclusive/produce_L1_MET_EgV2_dpAK8_ttGen_template_cfg.py')
    genId_file = open(tempdir+'/producer_'+prefix+'_'+str(j)+'.py','w')
    for line in genId_templ_file:
        line=line.replace('CONDOR_FILELIST', singleFileList)
        line=line.replace('CONDOR_ISMC', useMC)
        line=line.replace('CONDOR_ISTTBAR', isTTbar)
        line=line.replace('CONDOR_MEDIATOR', mediatorFile)
        genId_file.write(line)
    genId_file.close()
    genId_templ_file.close()

    py_templ_file = open(relBase+'/src/LJMet/Com/condor_TTinclusive/ljmet_cfg.py')
    py_file = open(tempdir+'/'+prefix+'_'+str(j)+'.py','w')
    for line in py_templ_file:
        line=line.replace('CONDOR_ISMC',     useMC)
        line=line.replace('CONDOR_JSON',     json)
        line=line.replace('CONDOR_MEDIATOR', mediatorFile)
        line=line.replace('CONDOR_OUTFILE',  prefix+'_'+str(j))
        line=line.replace('SAVEGENHT',  saveGHT)
        line=line.replace('NEWPDF',  newpdf)
        if 'JECup' in shift  and 'JECup' in line: line=line.replace('False',  'True')
        if 'JECdown' in shift and 'JECdown' in line: line=line.replace('False',  'True')
        if 'JERup' in shift and 'JERup' in line: line=line.replace('False',  'True')
        if 'JERdown' in shift and 'JERdown' in line: line=line.replace('False',  'True')
        py_file.write(line)
    py_file.close()
    py_templ_file.close()

    jdl_templ_file = open(relBase+'/src/LJMet/Com/condor_TTinclusive/templateDeepAK8.jdl')
    jdl_file       = open(tempdir+'/'+prefix+'_'+str(j)+'.jdl','w')
    for line in jdl_templ_file:
        line=line.replace('PREFIX',     prefix)
        line=line.replace('JOBID',      str(j))
        line=line.replace('OUTPUT_DIR', xrddir)
        line=line.replace('INPUTTAR',   tarfile)
        line=line.replace('TAR_FILE',   tarfile.split('/')[-1])
        line=line.replace('TAR_DIR',    tardir)
        jdl_file.write(line)
    jdl_file.close()
    jdl_templ_file.close()

    j = j + 1
    nfiles = nfiles + files_per_job

njobs = j - 1

if (submit):
    savedPath = os.getcwd()
    os.chdir(tempdir)
    print 'Submitting',njobs,'jobs!'
    #proc = subprocess.Popen(['condor_submit', prefix+'_1.jdl'], stdout=subprocess.PIPE)
    #(out, err) = proc.communicate()
    #if len(out) > 0: print "condor output:", out

    for k in range(1,njobs+1):
        os.system('condor_submit '+prefix+'_'+str(k)+'.jdl')
        os.system('sleep 0.5')

    os.chdir(savedPath)
