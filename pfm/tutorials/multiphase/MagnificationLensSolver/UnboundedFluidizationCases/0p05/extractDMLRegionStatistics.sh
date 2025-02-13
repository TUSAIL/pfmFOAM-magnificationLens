#!/bin/bash
#------------------------------------------------------------------------------
# script for extracting the statistics in the DML region from the log file
#------------------------------------------------------------------------------ 
DATA1=log_run_parallel_MagLensSolver

# Extract TKE of particle phase
grep --text 'TKESolid_DMLRegionCFDDEM:' $DATA1 | \
  cut -d' ' -f2 > TKECFDDEM.tmp 

# Extract domain averaged slip velocity
grep --text 'slipVelocity_DMLRegionCFDDEM:' $DATA1 | \
  cut -d' ' -f2 > slipVelCFDDEM.tmp

# Extract domain averaged variance of solids volume fraction
grep --text 'PhiP2Solid_DMLRegionCFDDEM:' $DATA1 | \
  cut -d' ' -f2 > PhiP2CFDDEM.tmp

# Extract domain averaged solids volume fraction
grep --text 'domainAveSolidsVolFrac_DMLRegionCFDDEM:' $DATA1 | \
  cut -d' ' -f2 > solidsMassCFDDEM.tmp

# Extract time
grep --text 'Time = ' $DATA1 | \
  grep -v Execution | cut -d' ' -f3 > time1.tmp


grep --text 'TKESolid_DMLRegionTFM:' $DATA1 | \
  cut -d' ' -f2 > TKETFM.tmp 
 
grep --text 'slipVelocity_DMLRegionTFM:' $DATA1 | \
  cut -d' ' -f2 > slipVelTFM.tmp
 
grep --text 'PhiP2_DMLRegionTFM:' $DATA1 | \
  cut -d' ' -f2 > PhiP2TFM.tmp

grep --text 'domainAveSolidsVolFrac_DMLRegionTFM:' $DATA1 | \
  cut -d' ' -f2 > solidsMassTFM.tmp

grep --text 'Time = ' $DATA1 | \
  grep -v Execution | cut -d' ' -f3 > time1.tmp


paste time1.tmp slipVelCFDDEM.tmp > slipVel_DMLRegionCFDDEM.txt
 
paste time1.tmp TKECFDDEM.tmp > TKEsolids_DMLRegionCFDDEM.txt

paste time1.tmp PhiP2CFDDEM.tmp > PhiP2_DMLRegionCFDDEM.txt

paste time1.tmp solidsMassCFDDEM.tmp > domainAveSolidsVolFrac_DMLRegionCFDDEM.txt

paste time1.tmp slipVelTFM.tmp > slipVel_DMLRegionTFM.txt
 
paste time1.tmp TKETFM.tmp > TKEsolids_DMLRegionTFM.txt

paste time1.tmp PhiP2TFM.tmp > PhiP2_DMLRegionTFM.txt

paste time1.tmp solidsMassTFM.tmp > domainAveSolidsVolFrac_DMLRegionTFM.txt

rm *.tmp

