#!/usr/bin/python

import argparse

parser = argparse.ArgumentParser()
parser.add_argument("--prefixFile",type=str,help='The name of the file with list of prefixes to use',required=True,nargs=1)
parser.add_argument("--inputFile",type=str,help='The name of file with list of paths to input files to use', required=True,nargs=1)
args = parser.parse_args()

import os
import re
import fileinput

files_per_job = 1

rel_base = os.environ['CMSSW_BASE']
cmssw = 'CMSSW_8_0_14'
logdir = 'Aug19'
outdir = '/eos/uscms/store/user/clint/Spring16/25ns/'+logdir+'/'
#outdir = '/eos/uscms/store/user/clint/Spring15/25ns/'+logdir+'/'

### What is the name of your FWLite Analyzer
FWLiteAnalyzer = 'ljmet'

### Which Systematics to do
DONOMINAL = 'True'
DOJES = 'False'
DOJER = 'False'
DOBTAG = 'False'
DOQCDMC = 'False'
DOTTBARSYS = 'False'

### JSON file to use
MYJSON = "''"

### Systematics flags
BTAGUNCERTUP = 'False'
BTAGUNCERTDOWN = 'False'
JECUNCERTUP = 'False'
JECUNCERTDOWN = 'False'
JERUNCERTUP = 'False'
JERUNCERTDOWN = 'False'


#################################################
### Names to give to your output root files
#################################################
prefixFile= open(args.prefixFile[0],'r')
prefix = []
for line in prefixFile:
    if line.find('#')==-1 and line!='\n':
        line = line.replace('\n','')
        prefix.append(line)
print prefix

###########################################
### Where to save your output root files to
###########################################
dir = []
for i in prefix:
    outdiri = outdir+i
    dir.extend([outdiri])

################################################
### Where is your list of root files to run over
################################################
listfile = open(args.inputFile[0],'r')
list=[]
for line in listfile:
    if line != '\n' and line.find('#')==-1:
        line = line.replace('\n','')
        list.append(line)

for i in range(len(prefix)):
    print i,' ',prefix[i],' ',dir[i],' ',list[i]

### Write the files you wish to run over for each job    
def get_input(num, list):
    result = '' 
    file_list = open(rel_base+"/src/LJMet/Com/python/"+list)
    file_count = 0
    for line in file_list:
        if line.find('root')>0:
            file_count=file_count+1
            if file_count>(num-1) and file_count<(num+files_per_job):
                f_name=line.split('.root')[0]
                f_name=f_name+'.root'
                #result=result+'                 \'' + f_name.group(1)+'\',\n'
                #result=result+'                 \'dcap:///pnfs/cms/WAX/11' + f_name.group(1)+'\',\n'
                result=result+'                 \'root://cmsxrootd.fnal.gov/' + f_name +'\',\n'
    file_list.close()
    #result = result + '                 )\n'
    return result


print str(files_per_job)+' files per job...'

#make tarball and move to eos
#os.system('tar -cvf ljmet.tar ../bin/* ../src/* ../interface/* ../data/* ../python/* ../tools/* ../BuildFile.xml ../setup.sh ../weights/* ')
#os.system('xrdcp -f ljmet.tar root://cmseos.fnal.gov//store/user/clint/ljmet.tar')

for i in range(len(prefix)):

    j = 1
    nfiles = 1

    #make local directory
    locdir = logdir+'/'+prefix[i]
    os.system('mkdir -p  %s' %locdir)

        
    FLAGTAG = 'TriggerResults::PAT'
    
    print 'CONDOR work dir: '+dir[i]
    #os.system('rm -rf '+dir[i])
    #os.system('mkdir -p '+dir[i])

    file_list = open(rel_base+"/src/LJMet/Com/python/"+list[i])
    count = 0
    for line in file_list:
        if line.find('root')>0:
            count = count + 1
    file_list.close()
    #count = count - 1

    print 'File prefix: '+prefix[i]
    print 'Number of input files: '+str(count)

    while ( nfiles <= count ):    

        py_templ_file = open(rel_base+"/src/LJMet/Com/condor/Dilepton_Spring16MC_25ns_python.templ")
        condor_templ_file = open(rel_base+"/src/LJMet/Com/condor/X53condor.templ")
        csh_templ_file    = open(rel_base+"/src/LJMet/Com/condor/X53csh.templ")

        #open local version of file
        localfile=locdir+'/'+prefix[i]+"_"+str(j)+".py"
        runpy = prefix[i]+"_"+str(j)+".py"
        py_file = open(localfile,"w")
        for line in py_templ_file:
            line=line.replace('DIRECTORY',dir[i])
            line=line.replace('PREFIX',prefix[i])
            line=line.replace('JOBID',str(j))
            line=line.replace('INFILES',get_input(nfiles, list[i]))
            line=line.replace('BTAGUNCERTUP',BTAGUNCERTUP)
            line=line.replace('BTAGUNCERTDOWN',BTAGUNCERTDOWN)
            line=line.replace('JECUNCERTUP',JECUNCERTUP)
            line=line.replace('JECUNCERTDOWN',JECUNCERTDOWN)
            line=line.replace('JERUNCERTUP',JERUNCERTUP)
            line=line.replace('JERUNCERTDOWN',JERUNCERTDOWN)
            line=line.replace('EVENTSTOPROCESS',str(-1))
            line=line.replace('FLAGTAG',FLAGTAG)
            line=line.replace('JSONFILE',MYJSON)
            py_file.write(line)
        py_file.close()

        #copy file to eos
        eosfile =   "root://cmseos.fnal.gov/"+dir[i]+"/"+prefix[i]+"_"+str(j)+".py"
        os.system("xrdcp -f %s %s"  % (localfile,eosfile))
        #remove local version
        os.system('rm %s python_cfgs/MC/' % localfile)

        localcondor = locdir+'/'+prefix[i]+"_"+str(j)+".condor"
        eoscondor = "root://cmseos.fnal.gov/"+dir[i]+"/"+prefix[i]+"_"+str(j)+".condor"
        condor_file = open(localcondor,"w")
        for line in condor_templ_file:
            line=line.replace('DIRECTORY',locdir)
            line=line.replace('PREFIX',prefix[i])
            line=line.replace('JOBID',str(j))
            condor_file.write(line)
        condor_file.close()

        #copy local to eos
        #os.system('xrdcp -f %s %s' % (localcondor,eoscondor))
        #remove local copy
        #os.system('rm %s' % localcondor)

        eoscsh="root://cmseos.fnal.gov/"+dir[i]+"/"+prefix[i]+"_"+str(j)+".csh"
        localcsh=locdir+'/'+prefix[i]+"_"+str(j)+".csh"
        eosoutput="root://cmseos.fnal.gov/"+dir[i]+"/"+prefix[i]+'_'+str(j)+'.root'
        locoutput = prefix[i]+'_'+str(j)+'.root'
        csh_file = open(localcsh,"w")
        for line in csh_templ_file:
            line=line.replace('CMSSWBASE',rel_base)
            line=line.replace('DIRECTORY',dir[i])
            line=line.replace('PREFIX',prefix[i])
            line=line.replace('JOBID',str(j))
            line=line.replace('CMSSWVERSION',cmssw)
            line=line.replace('FWLITEANALYZER',FWLiteAnalyzer)
            line=line.replace('EOSPY',eosfile)
            line=line.replace('LOCPY',runpy)
            line=line.replace('EOSOUT',eosoutput)
            line=line.replace('LOCOUT',locoutput)
            csh_file.write(line)
        csh_file.close()

        #os.system('xrdcp -f %s %s' % (localcsh,eoscsh))
        #os.system('rm %s' %localcsh)

        os.system('chmod u+x '+locdir+'/'+prefix[i]+'_'+str(j)+'.csh')
        print 'condor file is: '+locdir+'/'+prefix[i]+'_'+str(j)+'.condor;'
        os.system(' condor_submit %s' % localcondor)
        #os.system('cd '+dir[i]+'; condor_submit '+prefix[i]+'_'+str(j)+'.condor; cd -')
        j = j + 1
        nfiles = nfiles + files_per_job
        py_templ_file.close()
        condor_templ_file.close()
        csh_templ_file.close()

    #print  str(j-1)+' jobs submitted'
    
