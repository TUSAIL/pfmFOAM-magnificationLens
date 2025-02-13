# Magnification lens solver
OpenFOAM based developments made by the [Department of Particulate Flow Modelling at Johannes Kepler University in Linz, Austria.](https://www.jku.at/pfm)

[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0.html)

## Disclaimer

> This offering is not approved or endorsed by OpenCFD Limited, producer and distributor of the OpenFOAM software via www.openfoam.com, and owner of the OPENFOAM®  and OpenCFD®  trade marks.

## Features
This solver is capable of coupling Two-Fluid Model with CFD-DEM in a specified region. It is possible to choose between one-way coupling and two-way coupling between Two-Fluid Model and CFD-DEM.
In case of using two-way coupling methodology user can choose between mass and momentum coupling or solely momentum coupling between Two-Fluid Model and CFD-DEM. The Two-Fluid Model solver is 
based on "twoPhaseEulerTurbFoam" and the CFD-DEM simulations are based on CFDEMcoupling and LIGGGHTS developed by Department of Particulate Flow Modelling at Johannes Kepler University in Linz, Austria.

## How to cite
Behrad Esgandari, Daniel Queteschiner, Stefan Pirker, and Simon Schneiderbauer. "Discrete magnification lens model: A new hybrid multi-scale modelling method for fluid-particle systems." Powder Technology 445 (2024): 120094.

## Installation
The OpenFOAM 6, CFDEMcoupling compatible with this solver [(CFDEMcoupling-magnificationLens)](https://github.com/behradesg/CFDEMcoupling-magnificationLens) and LIGGGHTS_ML should already be compiled in your system. Then, download the solver or clone it using git clone. Change the cloned folder name to pfmFOAM. Load CFDEMcoupling_ML bashrc in a terminal and in the same terminal type
```bash
cd pfmFOAM/pfm/src/TurbulenceModels
./Allwmake
```
Then, direct to the pre-processing folder and wmake writeCellGlobalID, writeCellSetLocalIDs, and writePatchFaceGlobalIDsList,
```bash
cd pfmFOAM/pfm/applications/utilities/preProcessing/writeCellGlobalID
wmake
cd ..
cd pfmFOAM/pfm/applications/utilities/preProcessing/writeCellSetLocalIDs
wmake
cd ..
cd pfmFOAM/pfm/applications/utilities/preProcessing/writePatchFaceGlobalIDsList
wmake
```
After compiling the utilities necessary to run magnification solver cases, direct to magnification lens solver folder and compile the solver,
```bash
cd pfmFOAM/pfm/applications/solvers/multiphase/MagnificationLensSolver
./Allwmake 
```
In addition, it is also possible to compile twoPhaseEulerTurbFoam solver which can be used to simulation Two-Fluid Model cases with,
```bash
cd pfmFOAM/pfm/applications/solvers/multiphase/twoPhaseEulerTurbFoam
./Allwmake 
```

## Tutorial
The tutorials related to the discrete magnification lens cases in Esgandari et al. (2024) can be found in pfmFOAM/pfm/tutorials/multiphase/MagnificationLensSolver and the Two-Fluid Model cases can be found in tutorials/multiphase/twoPhaseEulerTurbFoam/KT/DiscreteMagnificationLensPaperCases.


## License

[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0.html)

- This software is distributed under the [GNU General Public License](https://www.gnu.org/licenses/gpl-3.0.html).
- Copyright © 2019- JKU Linz
- Some parts of this code are based on OpenFOAM® and Copyright on these parts is held by the OpenFOAM® Foundation (www.openfoam.org) and potentially other parties.
