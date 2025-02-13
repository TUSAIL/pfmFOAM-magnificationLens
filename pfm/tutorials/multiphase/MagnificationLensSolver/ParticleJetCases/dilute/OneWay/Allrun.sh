#!/bin/bash
#------------------------------------------------------------------------------
# allrun script for particle jets
# run maginifcation lens solver
#------------------------------------------------------------------------------

#- define variables
casePath="$(dirname "$(readlink -f ${BASH_SOURCE[0]})")"

#- include functions
source $CFDEM_PROJECT_DIR/etc/functions.sh

bash $casePath/CFD/preparation.sh

#- run parallel CFD-DEM
bash $casePath/parCFDDEMrun.sh
