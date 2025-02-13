//===============================
// particle fields
//===============================

Info<< "\nReading momentum exchange field Ksl\n" << endl;
volScalarField Ksl
(
    IOobject
    (
        "Ksl",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);

Info<< "\nReading voidfraction field voidfraction = (Vgas/Vparticle)\n" << endl;
volScalarField voidfraction
(
    IOobject
    (
        "voidfraction",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);

Info<< "Reading particle velocity field Us\n" << endl;
volVectorField Us
(
    IOobject
    (
        "Us",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);

volScalarField rho
(
    IOobject
    (
        "rho",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);

// Need to define U_dem and phi_dem dummy fields to create the turbulence model object
// required by cfdemCloud
volVectorField U_dem
(
    IOobject
    (
        "U_dem",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);

Info<< "Reading/calculating face flux field phi\n" << endl;
surfaceScalarField phi_dem
(
    IOobject
    (
        "phi_dem",
        runTime.timeName(),
        mesh,
        IOobject::READ_IF_PRESENT,
        IOobject::AUTO_WRITE
    ),
    linearInterpolate(U_dem*voidfraction) & mesh.Sf()
);



singlePhaseTransportModel laminarTransport(U_dem, phi_dem);

autoPtr<incompressible::turbulenceModel> turbulence
 (
    incompressible::turbulenceModel::New(U_dem, phi_dem, laminarTransport)
 );
