#!/bin/csh  

set end = `grep "All cuts" *log | awk '{done+=$6} END {print done}'`
set initial = `grep "No selection" *log | awk '{done+=$6} END {print done}'`
echo Initial number of events : $initial
echo Final number of events   : $end
set nbrFailed = `grep -L "No selection" *log | wc -l`
if ($nbrFailed > 0) then
  echo Failed jobs:
  grep -L "No selection" *log
endif


