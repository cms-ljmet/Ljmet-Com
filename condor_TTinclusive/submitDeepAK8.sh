#!/bin/bash

#echo "SUBMITTING LJMET -- nominal"
#cp TTsinglelep_MC_cfg.py ljmet_cfg.py
#python -u condor_submitMC2017.py nominal

#cp TTsinglelep_Data_cfg.py ljmet_cfg.py
#python -u condor_submitData2017.py nominal

# echo "SUBMITTING LJMET -- JECup"
# cp TTsinglelep_MC_cfg.py ljmet_cfg.py
# python -u condor_submitMC2017.py JECup

# echo "SUBMITTING LJMET -- JECdown"
# cp TTsinglelep_MC_cfg.py ljmet_cfg.py
# python -u condor_submitMC2017.py JECdown

echo "SUBMITTING LJMET -- JERup"
cp TTsinglelep_MC_cfg.py ljmet_cfg.py
python -u condor_submitMC2017.py JERup

# echo "SUBMITTING LJMET -- JERdown"
# cp TTsinglelep_MC_cfg.py ljmet_cfg.py
# python -u condor_submitMC2017.py JERdown

echo "DONE"
