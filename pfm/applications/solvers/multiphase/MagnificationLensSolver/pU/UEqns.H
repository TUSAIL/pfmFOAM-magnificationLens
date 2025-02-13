Info<< "Constructing momentum equations" << endl;

MRF.correctBoundaryVelocity(U1);
MRF.correctBoundaryVelocity(U2);
MRF.correctBoundaryVelocity(U);

fvVectorMatrix U1Eqn(U1, rho1.dimensions()*U1.dimensions()*dimVol/dimTime);
fvVectorMatrix U2Eqn(U2, rho2.dimensions()*U2.dimensions()*dimVol/dimTime);

// modification for periodic box simulations
volScalarField rhoMix = 0.0*rho1;
volVectorField totalMomentum = 0.0*U1;
dimensionedScalar deltaT = U1.mesh().time().deltaT();
if (periodicBox) {
    rhoMix = fvc::domainIntegrate(alpha1*rho1+alpha2*rho2)
                / fvc::domainIntegrate(unity);
    
    totalMomentum =
                 fvc::domainIntegrate(alpha1*rho1*U1 + alpha2*rho2*U2)
                /fvc::domainIntegrate(alpha1*rho1);
}

Kd = fluid.Kd();
volVectorField KdUdrift(fluid.KdUdrift());

if (magLensSwitch && (twoWayMagLensMC || twoWayMagLensMMC)) {
	if (U1.mesh().time().value() > U1.mesh().time().startTime().value() + magLensStart * U1.mesh().time().deltaTValue()) {
        // replace kd fields with ksl calculated in cfd-dem
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
	    forAll(MLCells, i) {
            //if (1.0 - voidfraction[MLCells[i]] >= solidsVolFracThresh) Kd[MLCells[i]] = Ksl[MLCells[i]];
            Kd[MLCells[i]] = Ksl[MLCells[i]];
	    }  
  	}
}
Kd.correctBoundaryConditions();

if (magLensSwitch && (twoWayMagLensMC || twoWayMagLensMMC)) {
    // activate the magnification lens switch in the defined cellset
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
    forAll(MLCells, i) {
        //if (1.0 - voidfraction[MLCells[i]] >= solidsVolFracThresh) switchMagLens[MLCells[i]] = 1.0;
        switchMagLens[MLCells[i]] = 1.0;
    }
}     

{

    surfaceScalarField alphaf1("alphaf1", fvc::interpolate(alpha1));
    surfaceScalarField alphaf2("alphaf2", scalar(1) - alphaf1);

    volScalarField Vm(fluid.Vm());
    volScalarField CpIPh(fluid.CpIPh());

    {
    if (magLensSwitch) {
        // start the momentum coupling after the specified number of CFD/TFM time steps.
        if(U1.mesh().time().value() > U1.mesh().time().startTime().value() + magLensStart * U1.mesh().time().deltaTValue()) {
            if (!oneWayMagLens) {
                U1Eqn =
                (
                    fvm::ddt(alpha1, rho1, U1) + fvm::div(alphaRhoPhi1, U1)
                  + fvm::SuSp(-contErr1, U1)
                  + MRF.DDt(alpha1*rho1 + Vm, U1)
                  + phase1.turbulence().divDevRhoReff(U1)
                  + fvm::Sp(Kd, U1)
                  // adding momentum coupling source based on discrete particles
                  + switchMagLens * alpha1 * ( -(rho1 * Us / U1.mesh().time().deltaT()) + fvm::Sp(rho1/U1.mesh().time().deltaT(), U1) )
                 ==
                  // Virtual mass force
                  - Vm
                   *(
                        fvm::ddt(U1)
                      + fvm::div(phi1, U1)
                      - fvm::SuSp(fvc::div(phi1), U1)
                      - DDtU2
                    )
                  // (gN = 0, if pimple switch "periodicBox" is false)
                  + alpha1*(rho1-rhoMix)*gN
                  // in case of periodic box, adjust CG-velocity
                  - alpha1*rho1*totalMomentum*dimensionedScalar("tau",dimensionSet(0,0,-1,0,0),9.81)
                  + fvOptions(alpha1, rho1, U1)
                );
            } else {
                U1Eqn =
                (
                    fvm::ddt(alpha1, rho1, U1) + fvm::div(alphaRhoPhi1, U1)
                  + fvm::SuSp(-contErr1, U1)
                  + MRF.DDt(alpha1*rho1 + Vm, U1)
                  + phase1.turbulence().divDevRhoReff(U1)
                  + fvm::Sp(Kd, U1)
                 ==
                  // Virtual mass force
                  - Vm
                   *(
                        fvm::ddt(U1)
                      + fvm::div(phi1, U1)
                      - fvm::SuSp(fvc::div(phi1), U1)
                      - DDtU2
                    )
                  // own gravity for periodic box
                  // (gN = 0, if pimple switch "periodicBox" is false)
                  + alpha1*(rho1-rhoMix)*gN
                  // in case of periodic box, adjust CG-velocity
                  - alpha1*rho1*totalMomentum*dimensionedScalar("tau",dimensionSet(0,0,-1,0,0),9.81)
                  + fvOptions(alpha1, rho1, U1)
                );
            }            
            U1Eqn.relax();
            fvOptions.constrain(U1Eqn);
     	    } else {
                U1Eqn =
                (
                    fvm::ddt(alpha1, rho1, U1) + fvm::div(alphaRhoPhi1, U1)
                  + fvm::SuSp(-contErr1, U1)
                  + MRF.DDt(alpha1*rho1 + Vm, U1)
                  + phase1.turbulence().divDevRhoReff(U1)
                  + fvm::Sp(Kd, U1)
                 ==
                  // Virtual mass force
                  - Vm
                   *(
                        fvm::ddt(U1)
                      + fvm::div(phi1, U1)
                      - fvm::SuSp(fvc::div(phi1), U1)
                      - DDtU2
                    )
                  // own gravity for periodic box
                  // (gN = 0, if pimple switch "periodicBox" is false)
                  + alpha1*(rho1-rhoMix)*gN
                  // in case of periodic box, adjust CG-velocity
                  - alpha1*rho1*totalMomentum*dimensionedScalar("tau",dimensionSet(0,0,-1,0,0),9.81)
                  + fvOptions(alpha1, rho1, U1)
                );
            U1Eqn.relax();
            fvOptions.constrain(U1Eqn);
            }
        } else {
            U1Eqn =
            (
                fvm::ddt(alpha1, rho1, U1) + fvm::div(alphaRhoPhi1, U1)
              + fvm::SuSp(-contErr1, U1)
              + MRF.DDt(alpha1*rho1 + Vm, U1)
              + phase1.turbulence().divDevRhoReff(U1)
              + fvm::Sp(Kd, U1)
             ==
              // Virtual mass force
              - Vm
               *(
                    fvm::ddt(U1)
                  + fvm::div(phi1, U1)
                  - fvm::SuSp(fvc::div(phi1), U1)
                  - DDtU2
                )
              // own gravity for periodic box
              // (gN = 0, if pimple switch "periodicBox" is false)
              + alpha1*(rho1-rhoMix)*gN
              // in case of periodic box, adjust CG-velocity
              - alpha1*rho1*totalMomentum*dimensionedScalar("tau",dimensionSet(0,0,-1,0,0),9.81)
              + fvOptions(alpha1, rho1, U1)
            );
            U1Eqn.relax();
            fvOptions.constrain(U1Eqn);
        }

        // improved momentum predictor
        // (is currently experimental, deactivate with momentumPredictor "no" in PIMPLE dict)
        if (pimple.momentumPredictor()) {
            volScalarField rAU1
            (
                IOobject::groupName("rAU", phase1.name()),
                1.0
               /(
                    U1Eqn.A()
                  + max(phase1.residualAlpha() - alpha1, scalar(0))
                   *rho1/runTime.deltaT()
                )
            );
            
            surfaceScalarField rAU1f("rAU1f", fvc::interpolate(rAU1));
            
            surfaceScalarField alphaCprAUf1
            (
                fvc::interpolate(max(alpha1 + CpIPh, phase1.residualAlpha())*rAU1)
            );
            
            solve
            (
                U1Eqn
             ==
                fvc::reconstruct
                (
                    (
                      - rAU1f*(fvc::snGrad(phase1.turbulence().normalStress()))*mesh.magSf()
                      - alphaCprAUf1*(fvc::snGrad(p_rgh) + ghf*fvc::snGrad(fluid.rho())) * mesh.magSf()
                    )/ rAU1f
                  // drag
                  + (fvc::interpolate(rAU1*Kd*U2)&mesh.Sf())/rAU1f
                  // gravity
                  + fvc::interpolate(alpha1*rho1*rAU1)*(g&mesh.Sf())/rAU1f
                  // drift velocity
                  + (fvc::interpolate(KdUdrift*rAU1)&mesh.Sf())/rAU1f
                )
            );
        }
        U1.correctBoundaryConditions();
        fvOptions.correct(U1);
    }

    {
        U2Eqn =
        (
            fvm::ddt(alpha2, rho2, U2) + fvm::div(alphaRhoPhi2, U2)
          + fvm::SuSp(-contErr2, U2)
          + MRF.DDt(alpha2*rho2 + Vm, U2)
          + phase2.turbulence().divDevRhoReff(U2)
          + fvm::Sp(Kd, U2)
        ==
          // Virtual mass force
          - Vm
           *(
                fvm::ddt(U2)
              + fvm::div(phi2, U2)
              - fvm::SuSp(fvc::div(phi2), U2)
              - DDtU1
            )
          // interphase force
          // + CpIPh*fvc::grad(p)
          // drift velocity
          // - KdUdrift
          // own gravity for periodic box
          // (gN = 0, if pimple switch "periodicBox" is false)
          + alpha2*(rho2-rhoMix)*gN
          // in case of periodic box, adjust CG-velocity
          //- alpha2*rho2*totalMomentum*dimensionedScalar("tau",dimensionSet(0,0,-1,0,0),9.81)
          + fvOptions(alpha2, rho2, U2)
        );
        U2Eqn.relax();
        //U2Eqn += fvm::Sp(Kd, U2);
        fvOptions.constrain(U2Eqn);
        
        // improved momentum predictor
        // (is currently experimental, deactivate with momentumPredictor "no" in PIMPLE dict)
        if (pimple.momentumPredictor()) {
            volScalarField rAU2
            (
                IOobject::groupName("rAU", phase2.name()),
                1.0
               /(
                    U2Eqn.A()
                  + max(phase2.residualAlpha() - alpha2, scalar(0))
                   *rho2/runTime.deltaT()
                )
            );
            
            surfaceScalarField rAU2f("rAU2f", fvc::interpolate(rAU2));

            surfaceScalarField alphaCprAUf2
            (
                fvc::interpolate(max(alpha2 - CpIPh, phase2.residualAlpha())*rAU2)
            );
            solve
            (
                U2Eqn
             ==
                fvc::reconstruct
                (
                    (
                      - rAU2f*(fvc::snGrad(phase2.turbulence().normalStress()))*mesh.magSf()
                      - alphaCprAUf2*(fvc::snGrad(p_rgh) + ghf*fvc::snGrad(fluid.rho()))* mesh.magSf()
                    ) / rAU2f
                 // drag
                 + (fvc::interpolate(rAU2*Kd*U1)&mesh.Sf())/rAU2f
                 // gravity
                 + fvc::interpolate(alpha2*rho2*rAU2)*(g&mesh.Sf())/rAU2f
                 // drift velocity
                 - (fvc::interpolate(KdUdrift*rAU2)&mesh.Sf())/rAU2f
                )
            );

        }
        
        U2.correctBoundaryConditions();
        fvOptions.correct(U2);
    }
}
