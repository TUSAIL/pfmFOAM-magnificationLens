#!/bin/bash
#------------------------------------------------------------------------------
# allrun script for periodic box simulation
# run maginifcation lens solver
#------------------------------------------------------------------------------

#- define variables
casePath="$(dirname "$(readlink -f ${BASH_SOURCE[0]})")"

#- include functions
source $CFDEM_PROJECT_DIR/etc/functions.sh

bash $casePath/CFD/preparation.sh

if [ -f "$casePath/DEM/post/restart/InitialPack.restart" ];  then
    echo "LIGGGHTS init was run before - using existing restart file"
else
    #- run serial DEM
    $casePath/parDEMrun.sh
fi

#- run parallel CFD-DEM
bash $casePath/parCFDDEMrun.sh
