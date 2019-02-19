import os, sys
execfile("/uscms_data/d3/jmanagan/EOSSafeUtils.py")

dirlist=[
'SingleMuon_Mar2018',
]

for sample in dirlist:
    os.system('eos root://cmseos.fnal.gov/ mkdir /store/user/lpcljm/2018/LJMet94X_1lepTT_091518/nominal/'+sample)
    rootfiles = EOSlist_root_files('/store/user/saj32265/TT2018/LJMet94X_1lepTT_081518/nominal/'+sample)
    print rootfiles[0]
    for file in rootfiles:
        os.system('xrdcp root://cmseos.fnal.gov//store/user/saj32265/TT2018/LJMet94X_1lepTT_081518/nominal/'+sample+'/'+file+' root://cmseos.fnal.gov//store/user/lpcljm/2018/LJMet94X_1lepTT_091518/nominal/'+sample+'/'+file)
