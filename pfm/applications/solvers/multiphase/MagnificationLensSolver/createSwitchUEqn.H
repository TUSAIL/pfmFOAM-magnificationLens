cellSet magLensCellSet(
 IOobject
  (
    "magLensCellSet", 
    "constant/polyMesh/sets",
    mesh,
    IOobject::MUST_READ,
    IOobject::NO_WRITE
  )
); 

const labelList& magLensCells = magLensCellSet.toc();

volScalarField switchMagLens
(
     IOobject
     (
          "switchMagLens",
          runTime.timeName(),
          mesh,
          IOobject::NO_READ,
          IOobject::NO_WRITE
     ),
     mesh,
     dimensionedScalar("switchMagLens", dimless, 0.0)
);

forAll(magLensCells, i) {

     switchMagLens[magLensCells[i]] = 0.0;

}
	
