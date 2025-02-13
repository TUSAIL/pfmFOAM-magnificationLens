DynamicList<word> cyclicPatchNames;
forAll(mesh.boundary(), patch) //loop through all boundary patches
{
  const word& patchName = mesh.boundary()[patch].name();
  const label& patchID = mesh.boundaryMesh().findPatchID(patchName);  
  const polyPatch& patchType = mesh.boundaryMesh()[patchID];
  if (isA<cyclicPolyPatch>(patchType)) //check if it is a cyclic patch
  {  
    cyclicPatchNames.append(patchName);
  }
  

}

PtrList<List<scalar>> patchFaceGlobalIDs_;
for (int i=0; i<cyclicPatchNames.size(); i++)
{
            patchFaceGlobalIDs_.append
            (
                new IOList<scalar>
                (
                    IOobject
                    (
			    "patchFaceGlobalIDs_" + cyclicPatchNames[i],
			    "0",
		  	     mesh, 
		  	     IOobject::READ_IF_PRESENT,
		   	     IOobject::AUTO_WRITE
                    )
                )
            );

}

cyclicPatchNames.clear();


