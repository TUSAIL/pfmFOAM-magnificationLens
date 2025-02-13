#!/bin/bash
#==================================#
#- function to run a parallel CFD-DEM case

parCFDDEMrun()
{
    #--------------------------------------------------------------------------------#
    #- define variables
    logpath="$1"
    logfileName="$2"
    casePath="$3"
    headerText="$4"
    solverName="$5"
    nrProcs="$6"
    machineFileName="$7"
    debugMode="$8"
    reconstuctCase="$9"
    cleanCase="$10"
    #--------------------------------------------------------------------------------#

    if [ $debugMode == "on" ]; then
        debugMode="valgrind"
    elif [ $debugMode == "strict" ]; then
        #debugMode="valgrind --leak-check=full -v --trace-children=yes --track-origins=yes" 
        debugMode="valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes"  
    else
        debugMode=""
    fi

    #- clean up old log file
    rm $logpath/$logfileName

    #- change path
    cd $casePath/CFD

    #- remove old data
    #rm -rf processor*

    #- decompose case
    #decomposePar

    #- make proc dirs visible
    count=0
    for i in `seq $nrProcs`
    do
        let count=$i-1
        (cd $casePath/CFD/processor$count && touch file.foam)
    done

    #- header
    echo 2>&1 | tee -a /$logpath/$logfileName
    echo "//   $headerText   //" 2>&1 | tee -a $logpath/$logfileName
    echo 2>&1 | tee -a $logpath/$logfileName

    #- write path
    pwd 2>&1 | tee -a $logpath/$logfileName
    echo 2>&1 | tee -a $logpath/$logfileName

    #- clean up case
    rm couplingFiles/*

    #- run applictaion
    if [[ $machineFileName == "none" ]]; then
        mpirun -np $nrProcs $debugMode $solverName -parallel > $logpath/$logfileName

        #- reconstruct case
        if [[ $reconstuctCase == "true" ]]; then   
            #pseudoParallelRun "reconstructPar" $nrProcs
            reconstructPar
        fi
    else
        mpirun -machinefile $machineFileName -np $nrProcs $debugMode $solverName -parallel > $logpath/$logfileName

        #- reconstruct case
        if [[ $reconstuctCase == "true" ]]; then   
            #pseudoParallelRun "reconstructPar" $nrProcs
            reconstructPar
        fi
    fi

    #- keep terminal open (if started in new terminal)
    #read
}
#==================================#

