#####################################################################################################################################################################################
######
###### This python script looks at the output directory created by condor. All files are searched for thier exit codes and if any errors have been found those files are resubmitted
######
###### To run: python CondorResubmitScript.py
######
###### Created by: Michael Segala 
######
#####################################################################################################################################################################################


import os
import re
import sys


dirpre = 'Jul13/'
debug = False

files = 0
rootfiles = 0
goodfiles = 0
badfiles = 0

badfilelist = []
badfileerror = []
badrootfiles = []

for s in os.listdir(dirpre):
    print s
    #if (not s.endswith('UP') and not s.endswith('DOWN')): continue
    #if (s.startswith('Single')): continue
    if (not (s.startswith('Wprime')) or ('Right' in s)): continue
    #if (not (s=='Wprime1500Right' or s=='Wprime2000Right' or s=='Wprime2500Right')): continue
    Dir=dirpre+s
    for f in os.listdir(Dir):
        if f.find('.root')>0:
            rootfiles = rootfiles + 1
            #print f 
        '''if f.find('condor.log')>0:
            files = files + 1
            file = open(Dir+"/"+f)
            for line in file:
                if line.find('return value')>0:
                    f_name=re.search('(return value 0)',line)
                    if f_name:
                        goodfiles = goodfiles + 1
                                
                    f_name_bad=re.search('return value [^0]',line)
                    if f_name_bad:
                        badfiles = badfiles + 1
                        badfilelist.append(f)
                        badfileerror.append(f_name_bad.group(0))
            file.close()'''

        if f.find('stdout')>0:
            files = files + 1
            file = open(Dir+"/"+f)
            good = True
            for line in file:
                if line.find('Abort')>=0:
                    if badfilelist.count(f[:-6])==0:
                        if (debug): print line
                        badfiles = badfiles + 1
                        badfilelist.append(f[:-6])
                        good = False
                                
            if good:
                goodfiles = goodfiles + 1
                
            file.close() 

        if f.find('stderr')>0:
            files = files + 1
            file = open(Dir+"/"+f)
            good = True
            for line in file:
                if line.find('segmentation')>=0:
                    if badfilelist.count(f[:-6])==0:
                        if (debug): print line
                        badfiles = badfiles + 1
                        badfilelist.append(f[:-6])
                        good = False
                                
                elif line.find('An exception of category')>=0:
                    if badfilelist.count(f[:-6])==0:
                        if (debug): print line
                        badfiles = badfiles + 1
                        badfilelist.append(f[:-6])
                        good = False

                elif line.find('Server responded with an error')>=0:
                    if badfilelist.count(f[:-6])==0:
                        if (debug): print line
                        badfiles = badfiles + 1
                        badfilelist.append(f[:-6])
                        good = False

                elif line.find('Error in <TBranchElement::GetBasket>')>=0:
                    if badfilelist.count(f[:-6])==0:
                        if (debug): print line
                        badfiles = badfiles + 1
                        badfilelist.append(f[:-6])
                        good = False

            if good:
                goodfiles = goodfiles + 1
                
            file.close()
                
        if f.find('condor.log')>0:
            files = files + 1
            file = open(Dir+"/"+f)
            good = True
            for line in file:
                if line.find('The system macro SYSTEM_PERIODIC_REMOVE expression')>=0:
                    if badfilelist.count(f[:-10])==0:
                        if (debug): print line
                        badfiles = badfiles + 1
                        badfilelist.append(f[:-10])
                        good = False
                                
                elif line.find('Job was aborted by the user')>=0:
                    if badfilelist.count(f[:-10])==0:
                        if (debug): print line
                        badfiles = badfiles + 1
                        badfilelist.append(f[:-10])
                        good = False
                                
            if good:
                goodfiles = goodfiles + 1

    for i in range(len(badfilelist)):
        os.system('rm '+Dir+'/'+badfilelist[i]+'condor.log;')
        os.system('condor_submit '+Dir+'/'+badfilelist[i]+'condor;')
        print 'File: ' + badfilelist[i] + 'condor resubmitted succesfully!'
    badfilelist=[]

if ((goodfiles+badfiles)>files): sys.exit('ERROR: multiple return statements in condor log files, would have submitted duplicate jobs. Exiting...')

unseccues = files - rootfiles

### This is needed if you have combined all your root files already
if unseccues == -1:
    unseccues = 0

print 'Total number of files ran over = ', files
print 'Total number of root files found = ', rootfiles
print 'Number of files not returning a root file = ', unseccues
print 'Number of files exited successfully = ', goodfiles
print 'Number of files exited with an error = ', badfiles
print '-------------------------------------'

