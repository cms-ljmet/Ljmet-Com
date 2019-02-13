import os, sys, getopt
execfile("/uscms_data/d3/jmanagan/EOSSafeUtils.py")

## CheckErrors.py /uscms_data/d3/path/to/logs/nominal/ --verbose 1 (prints) --resubmit 1 (if you want to resubmit) --resub_num -1 (resubmits all the fails it looks for)

dir = sys.argv[1]

print; print 'Checking', dir

try:
    opts, args = getopt.getopt(sys.argv[2:], "", ["verbose=", "resubmit=", "resub_num=", "cleanlogs="])
except getopt.GetoptError as err:
    print str(err)
    sys.exit(1)
    
verbose_level = 0
resubmit = '0'
resub_num = -2
cleanlogs = False

for o, a in opts:
	print o, a
	if o == '--verbose': verbose_level = int(a)
	if o == '--resubmit': resubmit = a
	if o == '--resub_num': resub_num = int(a)
	if o == '--cleanlogs': cleanlogs = bool(a)

rootdir = '/eos/uscms/store/user/lpcljm/2018/'+dir.split('/')[-3]+'/'+dir.split('/')[-2]+'/'
rootdir = rootdir.replace('_logs','')
print 'checking ROOT files in:',rootdir
folders = [x for x in os.walk(dir).next()[1]]

total_total = 0
total_succeeded = 0
total_error = 0
total_running = 0
total_roots = 0

no_log = 0
empty_log = 0
copy_fail = 0
mem_fail = 0
kill_fail = 0
fatal_fail = 0

for folder in folders:
        if 'Tprime' in folder: continue
	if verbose_level > 0:  print; print folder

        rootfiles = EOSlist_root_files(rootdir+folder)
        total_roots += len(rootfiles)

	files = [x for x in os.listdir(dir+'/'+folder) if '.jdl' in x]
	
	os.listdir(dir+'/'+folder)
	
	resub_index = []
	count_total = 0
	for file in files:
		total_total+=1
		index = file[file.find('_')+1:file.find('.')]
		if '_' in index: index = index.split('_')[-1]
		count_total += 1
	
                ## CHECK THE CONDOR LOG FIRST
                ## Memory fails, walltime fails, removals/kills, check for termination
                term = False                
		try:
                    current = open(dir + '/'+folder+'/'+file.replace('.jdl','.condor'),'r')
                    overmem = False
                    killed = False
                    term = False
                    iline = 0
                    overmem_index = 0
                    kill_index = 0
                    term_index = 0
                    for line in current:
                        if 'SYSTEM_PERIODIC_REMOVE' in line: 
                            overmem = True
                            term = True
                            overmem_index = iline
                        elif 'condor_rm' in line: 
                            killed = True
                            term = True
                            kill_index = iline
                        elif 'termination' in line: 
                            term = True
                            term_index = iline
                        iline += 1

                    if overmem and overmem_index > term_index: 
                        if verbose_level > 0: 
                            print '\tMEM/TIME FAIL:',file,' and JobIndex:',index
                        mem_fail+=1
                        if resub_num == -1 or resub_num == 3:resub_index.append(index)
                        continue

                    if killed and kill_index > term_index: 
                        if verbose_level > 0: 
                            print '\tKILL_FAIL:',file,' and JobIndex:',index
                        kill_fail+=1
                        if resub_num == -1 or resub_num == 4:resub_index.append(index)
                        continue

                    if not term: 
                        total_running += 1
                            
		except: pass
		
                ### JOBS STILL RUNNING, not terminated (or there was no condor log...)
                if not term: 
                    if os.path.exists(dir + '/'+folder+'/'+file.replace('.jdl','.condor')):
                        if verbose_level > 1:
                            print '\tRUNNING:',file,' and JobIndex:',index
                        continue
                    elif os.path.exists(dir + '/'+folder+'/'+file.replace('.jdl','.log')):
                        if verbose_level > 2: 
                            print '\tCLEANED:',file,' and JobIndex:',index
                        continue
                    else:
                        if verbose_level > 0:
                            print '\tSUBMIT FAIL:',file,' and JobIndex:',index,' FIX MANUALLY!!'
                        continue

                ### NOW LOOK FOR FILE READ ERRORS
                try:
                    current = open(dir + '/'+folder+'/'+file.replace('.jdl','.err'),'r')
                    good = True
                    for line in current:
                        if 'Begin Fatal Exception' in line: good = False
                    if not good: 
                        if verbose_level > 0: 
                            print '\tFATAL EX:',file,' and JobIndex:',index
                        fatal_fail+=1
                        if resub_num == -1 or resub_num == 5:resub_index.append(index)
                        continue
		except: pass

                ### NOW LOOK FOR COPY FAILURES
		try:
                    current = open(dir + '/'+folder+'/'+file.replace('.jdl','.out'),'r')
                    good = True
                    for line in current:
                        if 'failure' in line: good = False
                    if not good: 
                        if verbose_level > 0: 
                            print '\tXRDCP FAIL:',file,' and JobIndex:',index
                        copy_fail+=1
                        if resub_num == -1 or resub_num == 2:resub_index.append(index)
                        continue
		except: pass

                ### UNNAMED LJMET CRASHES, LOG EXISTS BUT IS EMPTY
		try:
                    current = open(dir + '/'+folder+'/'+file.replace('.jdl','.log'),'r')
                    good = False
                    for line in current:
                        if 'All cuts' in line: good = True
                    if not good: 
                        if verbose_level > 0: 
                            print '\tEMPTY LOG:',file,' and JobIndex:',index
                        empty_log+=1
                        if resub_num == -1 or resub_num == 0:resub_index.append(index)
                        continue
		except: pass

                ### ALL OTHER CRASHES, NO LJMET LOG FILE
		try:
                    if not os.path.isfile(dir+'/'+folder+'/'+file.replace('.jdl','.log')): 
                        if verbose_level > 0: 
                            print '\tNO LOG:',file,' and JobIndex:',index
                        no_log += 1
                        if resub_num == -1 or resub_num == 1: resub_index.append(index)
                        continue
		except: pass


                ### OTHERWISE IT LOOKS GOOD, double check that it's done
                if term and cleanlogs:
                    if verbose_level == 3:
                        print '\tCLEANING UP:',file,' and JobIndex:',index
                    ## these files are the largest for most samples
                    os.system('rm ' + dir + '/' + folder + '/' + folder.replace('/logfiles/','') + '_' + index + '.out')
                    os.system('rm ' + dir + '/' + folder + '/' + folder.replace('/logfiles/','') + '_' + index + '.condor')
                    ## and for ttbar these are full of messages
                    if 'TTTo' in folder or 'Mtt' in folder:
                        os.system('rm ' + dir + '/' + folder + '/' + folder.replace('/logfiles/','') + '_' + index + '.err')

	if resub_index != []: print 'RESUBS:', resub_index
	if resubmit != '1': continue

	indexind = 0
	for index in resub_index:
		os.chdir(dir + '/' + folder)

                f = open(dir + '/' + folder + '/' + folder.replace('/logfiles/','') + '_' + index + '.py', 'rU')
                ConfigLines = f.readlines()
                f.close()
                with open(dir + '/' + folder + '/' + folder.replace('/logfiles/','') + '_' + index + '.py','w') as fout:
                    for line in ConfigLines:
			if line.startswith('relBase    = str('): fout.write('relBase    = str(\'CONDOR_RELBASE\')\n')
			else: fout.write(line)
		os.system('rm ' + dir + '/' + folder + '/' + folder.replace('/logfiles/','') + '_' + index + '.log')
		os.system('rm ' + dir + '/' + folder + '/' + folder.replace('/logfiles/','') + '_' + index + '.err')
		os.system('rm ' + dir + '/' + folder + '/' + folder.replace('/logfiles/','') + '_' + index + '.out')
		os.system('rm ' + dir + '/' + folder + '/' + folder.replace('/logfiles/','') + '_' + index + '.condor')
		os.system('condor_submit ' + dir + '/' + folder + '/' + folder.replace('/logfiles/','') + '_' + index + '.jdl')
		indexind+=1
	
	

print
print 'TOTAL JOBS: ', total_total
print 'ROOT files:', total_roots
print 'TOTAL Running:', total_running
print
print 'NO LOG:', no_log
print 'FATAL EX:', fatal_fail
print 'EMPTY LOG:', empty_log
print 'XRDCP FAIL:', copy_fail
print 'MEM/TIME FAIL:', mem_fail
print 'KILLED:', kill_fail

print 'DONE:', total_total - total_running - no_log - fatal_fail - empty_log - copy_fail - mem_fail - kill_fail
