/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2021 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Application
    Creating a local cellset ids and sort them in each processor to be in the
    same order as the VTK file in the LIGGGHTS
    
    Behrad Esgandari, PFM, JKU 2023-

Description

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "IOList.H"
#include "scalarIOField.H"
#include "cellSet.H"//for cellset
#include <stdlib.h>
#include <vector>


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    argList::addNote
    (
        "Creating a local id list of cellset cell ids and sorts them\n"
        "\n"
        "Input arguments:\n"
        "cellSet Name\n"
    );
  
    argList::validArgs.append("cellSet Name");
  
    #include "setRootCase.H"
    #include "createTime.H"
    #include "createMesh.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //


    Info<< "Time = " << runTime.timeName() << endl;
    const word cellSetName = args[1];
    
    std::vector<int> localCellSetID;


    if (Pstream::parRun())
    {
     
      //read local cellset cell ids from processors
      cellSet magLensCellSet
       (
        IOobject
         (
            cellSetName,
            "constant/polyMesh/sets",
            mesh,
            IOobject::MUST_READ,
            IOobject::NO_WRITE
          )
        );
     
        const labelList& cells = magLensCellSet.toc();
        localCellSetID.resize(cells.size(), 0);
        forAll(cells, i)
        {         
             localCellSetID[i] = cells[i];

        }
         
         // Since the cellset list in the polymesh is not sorted from smallest to largest id, we need to sort them!
         std::sort(localCellSetID.begin(), localCellSetID.end());
      
	IOList<scalar> localCellSetIDs 
	(
	   IOobject
		(
	    	  "localCellSetIDs",
	    	  "0", 
	  	  mesh,
	  	  IOobject::NO_READ,
	   	  IOobject::AUTO_WRITE
		),
	   localCellSetID.size()
	);    
       
        //looping over the object elements and fill the object with sorted local ids
	for(int i=0; i < localCellSetID.size(); i++) 
	{

	   localCellSetIDs[i] = localCellSetID[i];

	}
	
	localCellSetIDs.write();


}

    Info<< "End\n" << endl;

    

/*  //in case you want to write out the global ids of the patch to a file (use for parallel programming stuff)
    OFstream procFile ("finalArrGlobalFaces");
    procFile << finalArrGlobalFaces << endl;
    procFile.flush(); // to be sure that data flushed to disk   
    
*/
    return 0;
}


// ************************************************************************* //
