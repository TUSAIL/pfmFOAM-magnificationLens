#!/bin/bash
 
#DATA0=log.0p05_Sch
DATA1=log_run_parallel_MagLensSolver
#DATA1=log_run_PB_radl
 
#STARTFIT=0
#STARTFITF=0
#ENDOFFIT=05


#grep --text 'Courant Number mean:' $DATA0 | \
#  cut -d' ' -f6 > courant.tmp
 
#grep --text 'INFO: Particle insertion insMagLens2: inserted' $DATA1 | \
#  cut -d' ' -f6 > Number.tmp 
  
#grep --text 'TotalBoundaryMassTop:' $DATA1 | \
#  cut -d' ' -f2 > Mass.tmp
 
#grep --text 'TotalMassFluxBottom:' $DATA1 | \
#  cut -d' ' -f2 > Massout.tmp
  
grep --text 'Courant Number mean:' $DATA1 | \
  cut -d' ' -f6 > courantMean.tmp 
 
 

grep --text 'Time = ' $DATA1 | \
  grep -v Execution | cut -d' ' -f3 > time1.tmp

 
#grep --text 'Time = ' $DATA2 | \
#  grep -v Execution | cut -d' ' -f3 > time2.tmp


#paste time0.tmp courant.tmp > courant_c0.dat
 
#paste time1.tmp Massout.tmp > massOutCFDDEM.txt

#paste time1.tmp Number.tmp > number.txt

#paste time1.tmp Mass.tmp > Mass.txt

paste time1.tmp courantMean.tmp > courantMean.txt

rm *.tmp


