#!/bin/bash
#------------------------------------------------------------------------------
# script for extracting the statistics in the DML region from the log file
#------------------------------------------------------------------------------ 
DATA1=log.twoPhaseEulerTurbFoam

grep --text 'TKESolid_DML:' $DATA1 | \
  cut -d' ' -f2 > TKETFM.tmp 
 
grep --text 'slipVelocity_DML:' $DATA1 | \
  cut -d' ' -f2 > slipVelTFM.tmp
 
grep --text 'PhiP2Solid_DML:' $DATA1 | \
  cut -d' ' -f2 > PhiP2TFM.tmp

grep --text 'domainAveSolidsVolFrac_DML:' $DATA1 | \
  cut -d' ' -f2 > solidsMassTFM.tmp

grep --text 'Time = ' $DATA1 | \
  grep -v Execution | cut -d' ' -f3 > time1.tmp

paste time1.tmp slipVelTFM.tmp > slipVel_DMLRegionTFM.txt
 
paste time1.tmp TKETFM.tmp > TKEsolids_DMLRegionTFM.txt

paste time1.tmp PhiP2TFM.tmp > PhiP2_DMLRegionTFM.txt

paste time1.tmp solidsMassTFM.tmp > domainAveSolidsVolFrac_DMLRegionTFM.txt

rm *.tmp

