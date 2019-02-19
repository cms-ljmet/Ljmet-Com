#!/usr/bin/python


import os
import re
import fileinput
import commands

rel_base = os.environ['CMSSW_BASE']
cmssw = 'CMSSW_8_0_14'
date = 'Jul13'
locdir = date+'_All'
basedir = '/store/user/drankin/LJMet'
indir = 'root://cmseos.fnal.gov/'+basedir+'/'+date
outdir = 'root://cmseos.fnal.gov/'+basedir+'/'+date+'_All'
eosdir = ''+basedir+'/'+date+'_All'

#################################################
### Names to give to your output root files
#################################################

samples = [
    #'Data',
    #'WJets',
    'WJets_HT100to200',
    'WJets_HT200to400',
    'WJets_HT400to600',
    'WJets_HT600to800',
    'WJets_HT800to1200',
    'WJets_HT1200to2500',
    'WJets_HT2500toInf',
    'TTbar',
    'TTbar_scaleup',
    'TTbar_scaledown',
    'ZJets_M50',
    'T_s',
    'T_t',
    'Tbar_t',
    'T_tW',
    'Tbar_tW',
    'WW',
    'WZ',
    'ZZ',
#    'QCD_Pt_120to170',
#    'QCD_Pt_170to300',
#    'QCD_Pt_300to470',
#    'QCD_Pt_470to600',
#    'QCD_Pt_600to800',
#    'QCD_Pt_800to1000',
#    'QCD_Pt_1000to1400',
]

samples = []

rmasses = [
'1000',
'1100',
'1200',
'1300',
'1400',
'1500',
'1600',
'1700',
'1800',
'1900',
'2000',
'2100',
'2200',
'2300',
'2400',
'2500',
'2600',
'2700',
'2800',
'2900',
'3000',
'3100',
'3200',
'3300',
'3400',
'3500',
'3600',
'3700',
'3800',
'3900',
'4000',
]

for i in rmasses:
    #samples.extend(['Wprime'+i+'Right'])
    samples.extend(['Wprime'+i+'Mix'])
    samples.extend(['Wprime'+i+'Left'])

datalist = [
'SingleElectron_Run2016B_PromptReco_v2',
'SingleMuon_Run2016B_PromptReco_v2',
'SingleElectron_Run2016C_PromptReco_v2',
'SingleMuon_Run2016C_PromptReco_v2',
'SingleElectron_Run2016D_PromptReco_v2',
'SingleMuon_Run2016D_PromptReco_v2',
]

systlist = [
'_JESDOWN',
'_JESUP',
'_JERUP',
'_JERDOWN',
'NOM'
]

#samples = ['Wprime3100Right','Wprime3200Right','Wprime3300Right','Wprime3400Right','Wprime3500Right','Wprime3600Right','Wprime3700Right','Wprime3800Right','Wprime3900Right','Wprime4000Right']

### Write the files you wish to run over for each job    

#make local directory
os.system('mkdir -p  %s' %locdir)
#make eos directory
os.system('eos root://cmseos.fnal.gov mkdir -p  %s' %eosdir)

for i in range(len(samples)):
    for sys in systlist:
        if (sys!='NOM' and (samples[i].startswith('QCD') or samples[i].startswith('TTbar_scale') or samples[i]=='Data')): continue

        condor_templ_file = open(rel_base+"/src/LJMet/Com/condor/PostLJMetcondor.templ")
        csh_templ_file    = open(rel_base+"/src/LJMet/Com/condor/PostLJMetcsh.templ")
    
        localcondor = locdir+'/'+samples[i]+sys+".condor"
        if (sys=='NOM'): localcondor = locdir+'/'+samples[i]+".condor"
        condor_file = open(localcondor,"w")
        for line in condor_templ_file:
            line=line.replace('DIRECTORY',locdir)
            if (sys=='NOM'): line=line.replace('PREFIX',samples[i])
            else: line=line.replace('PREFIX',samples[i]+sys)
            condor_file.write(line)
        condor_file.close()
    
        localcsh=locdir+'/'+samples[i]+sys+".csh"
        if (sys=='NOM'): localcsh=locdir+'/'+samples[i]+".csh"
        csh_file = open(localcsh,"w")
        for line in csh_templ_file:
            line=line.replace('CMSSWVERSION',cmssw)
            if (sys=='NOM'):
                if (samples[i] == 'ZJets_M50'):
                    line=line.replace('HADD', '''hadd -f ZJets_tmp.root `xrdfs root://cmseos.fnal.gov ls -u '''+basedir+'''/'''+date+'''/ZJets_M50/ | grep ".root"`''')
                    line=line.replace('XRDCP', 'xrdcp -f ZJets_tmp.root root://cmseos.fnal.gov/'+eosdir+'/ZJets.root')
                elif (samples[i] == 'Data'):
                    ida = 0
                    tmpstr = ''
                    for dataname in datalist:
                        gooddata = 1
                        firstrun = commands.getstatusoutput('eos root://cmseos.fnal.gov ls '+basedir+'/'+date+'/'+dataname+'/ | grep "_0.root"')
                        if (firstrun[1]==''):
                            numfile = commands.getstatusoutput('ls '+date+'/'+dataname+'/'+dataname+'*.stdout | wc -l')
                            for j in range(1,int(numfile[1])+1):
                                final = commands.getstatusoutput('grep -c \"All cuts          0\" '+date+'/'+dataname+'/'+dataname+'_'+str(j)+'.stdout')
                                if (int(final[1])!=1):
                                    gooddata = j
                                    break
                            tmpstr = tmpstr+'xrdcp -f root://cmseos.fnal.gov/'+basedir+'/'+date+'/'+dataname+'/'+dataname+'_'+str(gooddata)+'.root root://cmseos.fnal.gov/'+basedir+'/'+date+'/'+dataname+'/'+dataname+'_0.root\n'
                            tmpstr = tmpstr+'eosrm '+basedir+'/'+date+'/'+dataname+'/'+dataname+'_'+str(gooddata)+'.root\n'
                        tmpstr = tmpstr+'''hadd -f tmp_'''+str(ida)+'''.root `xrdfs root://cmseos.fnal.gov ls -u '''+basedir+'''/'''+date+'''/'''+dataname+'''/ | grep ".root"`\n'''
                        ida+=1
                    line=line.replace('HADD', tmpstr+'hadd -f tmp.root tmp_*.root')
                    line=line.replace('XRDCP', 'xrdcp -f tmp.root root://cmseos.fnal.gov/'+eosdir+'/SingleLep.root')
                else:
                    line=line.replace('HADD', '''hadd -f '''+samples[i]+'''_tmp.root `xrdfs root://cmseos.fnal.gov ls -u '''+basedir+'''/'''+date+'''/'''+samples[i]+'''/ | grep ".root"`''')
                    line=line.replace('XRDCP', 'xrdcp -f '+samples[i]+'_tmp.root root://cmseos.fnal.gov/'+eosdir+'/'+samples[i]+'.root')
            else:
                if (samples[i] == 'ZJets_M50'):
                    line=line.replace('HADD', '''hadd -f ZJets'''+sys+'''_tmp.root `xrdfs root://cmseos.fnal.gov ls -u '''+basedir+'''/'''+date+'''/ZJets_M50'''+sys+'''/ | grep ".root"`''')
                    line=line.replace('XRDCP', 'xrdcp -f ZJets'+sys+'_tmp.root root://cmseos.fnal.gov/'+eosdir+'/ZJets'+sys+'.root')
                else:
                    line=line.replace('HADD', '''hadd -f '''+samples[i]+sys+'''_tmp.root `xrdfs root://cmseos.fnal.gov ls -u '''+basedir+'''/'''+date+'''/'''+samples[i]+sys+'''/ | grep ".root"`''')
                    line=line.replace('XRDCP', 'xrdcp -f '+samples[i]+sys+'_tmp.root root://cmseos.fnal.gov/'+eosdir+'/'+samples[i]+sys+'.root')
            csh_file.write(line)
        csh_file.close()
    
    
        if (sys=='NOM'): os.system('chmod u+x '+locdir+'/'+samples[i]+'.csh')
        else: os.system('chmod u+x '+locdir+'/'+samples[i]+sys+'.csh')
        print 'condor file is: '+localcondor
        if (os.path.exists('%s.log'  % localcondor)):
            os.system('rm %s.log' % localcondor)
        os.system('condor_submit %s' % localcondor)
    
        condor_templ_file.close()
        csh_templ_file.close()
    
