#!/bin/sh
cd ${0%/*} || exit 1    # Run from this directory

# Source tutorial run functions
. $WM_PROJECT_DIR/bin/tools/RunFunctions

cp -r orig.0/ 0/

# create mesh
runApplication blockMesh

# create baffles
runApplication createBaffles -overwrite

# create sets
runApplication topoSet 

# Decompose (force to time 0 because solver reads data from folder 0)
runApplication decomposePar 

# create the fields of cell set ids in each processor
runParallel writeCellSetLocalIDs magLensCellSet

# create the fields of patch face ids based on global ids 
# takes the patch name and also the destination folder name for the created fields
runParallel writePatchFaceGlobalIDsList magLens_bottom_master 0
rm -r log.writePatchFaceGlobalIDsList
runParallel writePatchFaceGlobalIDsList magLens_bottom_slave 0
rm -r log.writePatchFaceGlobalIDsList
runParallel writePatchFaceGlobalIDsList magLens_top_master 0
rm -r log.writePatchFaceGlobalIDsList
runParallel writePatchFaceGlobalIDsList magLens_top_slave 0

#------------------------------------------------------------------------------
