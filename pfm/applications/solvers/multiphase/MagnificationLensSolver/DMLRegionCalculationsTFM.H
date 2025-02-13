//===================================== Slip Velocity ===========================================================//	
dimensionedScalar alpha2U2SumTFM("alpha2U2SumTFM", dimensionSet(0,1,-2,0,0), 0.0);
dimensionedScalar alpha2gNSumTFM("alpha2gNSumTFM", dimensionSet(0,1,-2,0,0), 0.0);
dimensionedScalar alpha1U1SumTFM("alpha1U1SumTFM", dimensionSet(0,1,-2,0,0), 0.0);
dimensionedScalar alpha1gNSumTFM("alpha1gNSumTFM", dimensionSet(0,1,-2,0,0), 0.0);

// calculate the quantities on each proc
forAll(MLCells, i)
{         
     alpha2U2SumTFM += alpha2[MLCells[i]]*(U2[MLCells[i]]&gN);
     alpha2gNSumTFM += alpha2[MLCells[i]]*mag(gN);
     alpha1U1SumTFM += alpha1[MLCells[i]]*(U1[MLCells[i]]&gN);
     alpha1gNSumTFM += alpha1[MLCells[i]]*mag(gN);
}
// sum the quantities from all procs
double alpha2U2TotalTFM = returnReduce(alpha2U2SumTFM.value(), sumOp<scalar>());
double alpha2gNTotalTFM = returnReduce(alpha2gNSumTFM.value(), sumOp<scalar>());
double alpha1U1TotalTFM = returnReduce(alpha1U1SumTFM.value(), sumOp<scalar>());
double alpha1gNTotalTFM = returnReduce(alpha1gNSumTFM.value(), sumOp<scalar>());

double slipVelocityTFM = -( (alpha2U2TotalTFM/alpha2gNTotalTFM) - (alpha1U1TotalTFM/alpha1gNTotalTFM) );
Info<< "slipVelocity_DMLRegionTFM: " << slipVelocityTFM << endl;

	
//==================================== Turbulent Kinetic Energy Solid ===========================================//	
dimensionedScalar alpha1U1U1SumTFM("alpha1U1U1SumTFM", dimensionSet(0,0,0,0,0), 0.0);
double alpha1SumTFM = 0.0;
dimensionedVector alpha1U1VecSumTFM("alpha1U1VecSumTFM", dimensionSet(0,0,0,0,0), vector(0.0,0.0,0.0));
// calculate the quantities on each proc
forAll(MLCells, i)
{         
     alpha1U1U1SumTFM += alpha1[MLCells[i]]*(U1[MLCells[i]]&U1[MLCells[i]]);
     alpha1SumTFM += alpha1[MLCells[i]];
     alpha1U1VecSumTFM += alpha1[MLCells[i]]*(U1[MLCells[i]]);

}

double alpha1U1U1TotalTFM = returnReduce(alpha1U1U1SumTFM.value(), sumOp<scalar>());
double alpha1TotalTFM = returnReduce(alpha1SumTFM, sumOp<scalar>());
vector alpha1U1VecTotalTFM = returnReduce(alpha1U1VecSumTFM.value(), sumOp<vector>());


double TKESolidTFM = 0.5 * (alpha1U1U1TotalTFM/alpha1TotalTFM - ((alpha1U1VecTotalTFM & alpha1U1VecTotalTFM)/(alpha1TotalTFM*alpha1TotalTFM)));
Info<< "TKESolid_DMLRegionTFM: " << TKESolidTFM << endl;            
	
//==================================== Variance of alpha1 fluctuations ===========================================//	
alpha1SumTFM = 0.0;
double alpha1alpha1SumTFM = 0.0;
double unitySumTFM = 0.0;
// calculate the quantities on each proc
forAll(MLCells, i)
{         
     alpha1alpha1SumTFM += alpha1[MLCells[i]]*alpha1[MLCells[i]];
     alpha1SumTFM += alpha1[MLCells[i]];
     unitySumTFM += unity[MLCells[i]];
}

double alpha1alpha1TotalTFM = returnReduce(alpha1alpha1SumTFM, sumOp<scalar>());
alpha1TotalTFM = 0.0;
alpha1TotalTFM = returnReduce(alpha1SumTFM, sumOp<scalar>());
double unityTotalTFM = returnReduce(unitySumTFM, sumOp<scalar>());	

double PhiP2SolidTFM = (alpha1alpha1TotalTFM/unityTotalTFM) - (alpha1TotalTFM * alpha1TotalTFM)/(unityTotalTFM * unityTotalTFM);
Info<< "PhiP2_DMLRegionTFM: " << PhiP2SolidTFM << endl;
//===================================== solids mass ===========================================================//	
	
Info<< "domainAveSolidsVolFrac_DMLRegionTFM: " << alpha1TotalTFM/unityTotalTFM << endl;	         
	    

	    
