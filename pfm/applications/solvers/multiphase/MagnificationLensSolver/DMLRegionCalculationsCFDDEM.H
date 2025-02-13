cellSet magLensCells(
 IOobject
  (
    "magLensCellSet", 
    "constant/polyMesh/sets",
    mesh,
    IOobject::MUST_READ,
    IOobject::NO_WRITE
  )
); 

const labelList& MLCells = magLensCells.toc();
	
//===================================== Slip Velocity ===========================================================//	
dimensionedScalar alpha2U2Sum("alpha2U2Sum", dimensionSet(0,1,-2,0,0), 0.0);
dimensionedScalar alpha2gNSum("alpha2gNSum", dimensionSet(0,1,-2,0,0), 0.0);
dimensionedScalar alpha1U1Sum("alpha1U1Sum", dimensionSet(0,1,-2,0,0), 0.0);
dimensionedScalar alpha1gNSum("alpha1gNSum", dimensionSet(0,1,-2,0,0), 0.0);

// calculate the quantities on each proc
forAll(MLCells, i)
{      
     alpha2U2Sum += voidfraction[MLCells[i]]*(U2[MLCells[i]]&gN);
     alpha2gNSum += voidfraction[MLCells[i]]*mag(gN);
     alpha1U1Sum += (1.0-voidfraction[MLCells[i]])*(Us[MLCells[i]]&gN);
     alpha1gNSum += (1.0-voidfraction[MLCells[i]])*mag(gN);

}
// sum the quantities from all procs
double alpha2U2Total = returnReduce(alpha2U2Sum.value(), sumOp<scalar>());
double alpha2gNTotal = returnReduce(alpha2gNSum.value(), sumOp<scalar>());
double alpha1U1Total = returnReduce(alpha1U1Sum.value(), sumOp<scalar>());
double alpha1gNTotal = returnReduce(alpha1gNSum.value(), sumOp<scalar>());

double slipVelocity = -( (alpha2U2Total/(alpha2gNTotal + small)) - (alpha1U1Total/(alpha1gNTotal + small) ));

Info<< "slipVelocity_DMLRegionCFDDEM: " << slipVelocity << endl;

//==================================== Turbulent Kinetic Energy Solid ===========================================//	
dimensionedScalar alpha1U1U1Sum("alpha1U1U1Sum", dimensionSet(0,0,0,0,0), 0.0);
double alpha1Sum = 0.0;
dimensionedVector alpha1U1VecSum("alpha1U1VecSum", dimensionSet(0,0,0,0,0), vector(0.0,0.0,0.0));
// calculate the quantities on each proc
forAll(MLCells, i)
{         
     alpha1U1U1Sum += (1.0-voidfraction[MLCells[i]])*(Us[MLCells[i]]&Us[MLCells[i]]);
     alpha1Sum += (1.0-voidfraction[MLCells[i]]);
     alpha1U1VecSum += (1.0-voidfraction[MLCells[i]])*(Us[MLCells[i]]);
}

double alpha1U1U1Total = returnReduce(alpha1U1U1Sum.value(), sumOp<scalar>());
double alpha1Total = returnReduce(alpha1Sum, sumOp<scalar>());
vector alpha1U1VecTotal = returnReduce(alpha1U1VecSum.value(), sumOp<vector>());

double TKESolid = 0.5 * (alpha1U1U1Total/(alpha1Total + small) - ((alpha1U1VecTotal & alpha1U1VecTotal)/( (alpha1Total*alpha1Total) + small) ));

Info<< "TKESolid_DMLRegionCFDDEM: " << TKESolid << endl;
	
//==================================== Variance of alpha1 fluctuations ===========================================//	
alpha1Sum = 0.0;
double alpha1alpha1Sum = 0.0;
double unitySum = 0.0;
// calculate the quantities on each proc
forAll(MLCells, i)
{         
     alpha1alpha1Sum += (1.0-voidfraction[MLCells[i]])*(1.0-voidfraction[MLCells[i]]);
     alpha1Sum += (1.0-voidfraction[MLCells[i]]);
     unitySum += unity[MLCells[i]];
}

double alpha1alpha1Total = returnReduce(alpha1alpha1Sum, sumOp<scalar>());
alpha1Total = 0.0;
alpha1Total = returnReduce(alpha1Sum, sumOp<scalar>());
double unityTotal = returnReduce(unitySum, sumOp<scalar>());	

double PhiP2Solid = (alpha1alpha1Total/unityTotal) - (alpha1Total * alpha1Total)/(unityTotal * unityTotal);
Info<< "PhiP2Solid_DMLRegionCFDDEM: " << PhiP2Solid << endl;	

//===================================== solids mass ===========================================================//	

Info<< "domainAveSolidsVolFrac_DMLRegionCFDDEM: " << alpha1Total/unityTotal << endl;		
    

