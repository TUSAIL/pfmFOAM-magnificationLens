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
    Creating a global id list of patch faces
    
    Behrad Esgandari, PFM, JKU 2023-

Description

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "IOList.H"
#include "scalarIOField.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    argList::addNote
    (
        "Creating a global id list of patch faces\n"
        "\n"
        "Input arguments:\n"
        "patchName\n"
        "TimeFolderName\n"
    );
  
    argList::validArgs.append("patchName");
    argList::validArgs.append("TimeFolderName");
  
    #include "setRootCase.H"
    #include "createTime.H"
    #include "createMesh.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    Info<< "Time = " << runTime.timeName() << endl;
    const word patchName = args[1];
    const word TimeFolderName = args[2];

    List<List<label>> processorFaceToArray; //Array for storing the faceProcAddressing from processor folders
    List<List<label>> processPatchFace;    //Array for storing the patch face ids of every processor
    label numProcPatchFaces; // Number of patch faces 


    if (Pstream::parRun())
    {

    	processorFaceToArray.resize(Pstream::nProcs()); //resizing the array to the number of processors
     
    	//read local face addressing from processors folders (the elements of this array contains the global ids related to the faces of each processor and the index of the array is the local (processor) id! boundary file in the processor folder shows the patch face ids based on the index of this array!
	labelIOList localFaceProcAddr
	(
	IOobject
	(
	    "faceProcAddressing",
	    mesh.facesInstance(),
	    mesh.meshSubDir,
	    mesh,
	    IOobject::MUST_READ,
	    IOobject::NO_WRITE
	 )
	);

        //reading the faceProcAddressing and gathering them on master processor
	processorFaceToArray[Pstream::myProcNo()] = localFaceProcAddr; //Assigning faceProcAddressing of each processor to the array related to that processor
    	Pstream::gatherList(processorFaceToArray); //Gather all the arrays on master processor    
    	//Info << "processorFaceToArray is: " << processorFaceToArray << endl;
    
        
	processPatchFace.resize(Pstream::nProcs()); //resizing the array to the size of the processors (a pactch can be divided between different processors)
    
	label patchI = mesh.boundaryMesh().findPatchID(patchName); //finding the ID of the input patchName
	//Info << "patchID is: " << patchI << endl;
	
	const polyPatch& cPatch = mesh.boundaryMesh()[patchI]; //selecting the patchName
	
	//Storing the local (processor) patch face ids of the patchName
	DynamicList<label> procPatchIDs(cPatch.size()); //could be different for each processor so DynamicList has used!
	forAll(cPatch, faceI)       
	{           
	    procPatchIDs.append(cPatch.start()+faceI);     //Ids of the local (processor) patch face 
	}

	processPatchFace[Pstream::myProcNo()] = procPatchIDs; //storing the local (processor) patch faces ids to each processors array
	Pstream::gatherList(processPatchFace); //Gather all the arrays on master to do the stuff there and find the global ids
    
	// trying to get the size of the final global ids array size and use dynamic list to append the elements  
	numProcPatchFaces = cPatch.size(); //size of the patch faces on each processor
	const scalar totNumProcPatchFaces = returnReduce(numProcPatchFaces, sumOp<label>()); //totNumProcPatchFaces is the total number of faces on the desired patch! using the returnReduce to be able to assign the summation of numProcPatchFaces to a variable (totNumProcPatchFaces)
	//Info << "TotnumProcPatchFaces is: " << TotnumProcPatchFaces << endl; //should be equal to the nFace of the patch in the undecomposed mesh
	
	DynamicList<label> finalArrGlobalFaces(totNumProcPatchFaces); //Array for storing the global patch face ids!
	if(Pstream::master()) //do stuff on master processor
	{ 		 
             forAll(processPatchFace, iProc)
             {
             
		labelList& thisProcessorPatchFace = processPatchFace[iProc]; //local patch face ids
		labelList& thisProcessorFaceIDs = processorFaceToArray[iProc]; //local face ids
	     	      
		forAll(thisProcessorPatchFace, FaceI)
		{
		     
		     label localPatchFaces = thisProcessorPatchFace[FaceI]; //getting the index related to the local face ids array   	             
		     finalArrGlobalFaces.append(thisProcessorFaceIDs[localPatchFaces]- 1); //Reading the global patch face ids  
		/*why subtracting by 1? 
		For decomposed meshes there are additional files (labelIOLists) that refer back to the undecomposed mesh: 
		faceProcAddressing  for every face the original face in the undecomposed mesh. Also codes  whether the face has been reversed. If procFaceI is the local face index  and globalFaceI the index in the undecomposed mesh: 
		- faceProcAddressing[procFaceI] == 0 : not allowed 
		- faceProcAddressing[procFaceI] >0 : globalFaceI =  mag(faceProcAddressing[procFaceI])-1 and orientation same as  undecomposed mesh 
		- faceProcAddressing[procFaceI] <0 : globalFaceI =  mag(faceProcAddressing[procFaceI])-1 and orientation opposite to  undecomposed mesh.
		faceProcAddressing ids with negative values are the processor boundary patch faces!
		*/		  	      	      
	        }
		 	
	     }
		//sending the global patch face ids array to slaves (in case if you want to write the ids field to every "0" in processor folders
		for (label jSlave=Pstream::firstSlave(); jSlave<=Pstream::lastSlave(); jSlave++) 
		{
			OPstream toSlave (Pstream::commsTypes::scheduled, jSlave); //sending array from master processor to slaves
			toSlave << finalArrGlobalFaces;
		}
	}
	else
	{
		IPstream fromMaster(Pstream::commsTypes::scheduled, Pstream::masterNo()); //receiveing array on slave processors
		fromMaster >> finalArrGlobalFaces;
	}
    

	//trying to write out the globalpatchfaceIDs to the "0"/time folders            
	IOList<scalar> patchFaceGlobalIDs //create an object to store the global ids
	(
	   IOobject
		(
	    	  "patchFaceGlobalIDs_" + patchName,
	    	  TimeFolderName,
	  	  mesh,
	  	  IOobject::NO_READ,
	   	  IOobject::AUTO_WRITE
		),
	   totNumProcPatchFaces//initializing to the number of global patch faces
	);    
       
        //looping over the object elements and fill the object with global ids!
	for(int i=0; i < totNumProcPatchFaces; i++) 
	{

	   patchFaceGlobalIDs[i] = finalArrGlobalFaces[i];

	}
	

	//makin the array to scatter the field to be able to write it out to all "0" folders of processors!
	List<List<scalar>> scatterpatchFaceGlobalIDs(Pstream::nProcs(), patchFaceGlobalIDs);
	Pstream::scatterList(scatterpatchFaceGlobalIDs); //scatter the array to slaves

	//write out the array to the "0"/time folder
	patchFaceGlobalIDs.write();

	//clearing dynamic arrays from memory
        procPatchIDs.clear();
        finalArrGlobalFaces.clear();

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
