/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2014 OpenFOAM Foundation
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

\*---------------------------------------------------------------------------*/

#include "radiationCoupledBase.H"
#include "volFields.H"
#include "mappedPatchBase.H"
#include "fvPatchFieldMapper.H"
#include "radiationModel.H"
#include "absorptionEmissionModel.H"

// * * * * * * * * * * * * * Static Member Data  * * * * * * * * * * * * * * //


namespace Foam
{
    defineTypeNameAndDebug(radiationCoupledBase, 0);
}


namespace Foam
{
    template<>
    const char* Foam::NamedEnum
    <
        Foam::radiationCoupledBase::emissivityMethodType,
        2
    >::names[] =
    {
        "solidRadiation",
        "lookup"
    };
}


const Foam::NamedEnum<Foam::radiationCoupledBase::emissivityMethodType, 2>
    Foam::radiationCoupledBase::emissivityMethodTypeNames_;


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::radiationCoupledBase::radiationCoupledBase
(
    const fvPatch& patch,
    const word& calculationType,
    const scalargpuField& emissivity
)
:
    patch_(patch),
    method_(emissivityMethodTypeNames_[calculationType]),
    emissivity_(emissivity)
{}


Foam::radiationCoupledBase::radiationCoupledBase
(
    const fvPatch& patch,
    const word& calculationType,
    const scalarField& emissivity,
    const fvPatchFieldMapper& mapper
)
:
    patch_(patch),
    method_(emissivityMethodTypeNames_[calculationType]),
    emissivity_(emissivity, mapper)
{}


Foam::radiationCoupledBase::radiationCoupledBase
(
    const fvPatch& patch,
    const dictionary& dict
)
:
    patch_(patch),
    method_(emissivityMethodTypeNames_.read(dict.lookup("emissivityMode")))
{
    switch (method_)
    {
        case SOLIDRADIATION:
        {
            if (!isA<mappedPatchBase>(patch_.patch()))
            {
                FatalIOErrorIn
                (
                    "radiationCoupledBase::radiationCoupledBase\n"
                    "(\n"
                    "    const fvPatch& p,\n"
                    "    const dictionary& dict\n"
                    ")\n",
                    dict
                )   << "\n    patch type '" << patch_.type()
                    << "' not type '" << mappedPatchBase::typeName << "'"
                    << "\n    for patch " << patch_.name()
                    << exit(FatalIOError);
            }

            emissivity_ = scalargpuField(patch_.size(), 0.0);
        }
        break;

        case LOOKUP:
        {
            if (!dict.found("emissivity"))
            {
                FatalIOErrorIn
                (
                    "radiationCoupledBase::radiationCoupledBase\n"
                    "(\n"
                    "    const fvPatch& p,\n"
                    "    const dictionary& dict\n"
                    ")\n",
                    dict
                )   << "\n    emissivity key does not exist for patch "
                    << patch_.name()
                    << exit(FatalIOError);
            }
            else
            {
                emissivity_ = scalargpuField("emissivity", dict, patch_.size());
            }
        }
        break;
    }
}


// * * * * * * * * * * * * * * * * Destructor    * * * * * * * * * * * * * * //

Foam::radiationCoupledBase::~radiationCoupledBase()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::scalargpuField Foam::radiationCoupledBase::emissivity() const
{
    switch (method_)
    {
        case SOLIDRADIATION:
        {
            // Get the coupling information from the mappedPatchBase
            const mappedPatchBase& mpp =
                refCast<const mappedPatchBase>(patch_.patch());

            const polyMesh& nbrMesh = mpp.sampleMesh();

            const radiation::radiationModel& radiation =
                nbrMesh.lookupObject<radiation::radiationModel>
                (
                    "radiationProperties"
                );


            const fvMesh& nbrFvMesh = refCast<const fvMesh>(nbrMesh);

            const fvPatch& nbrPatch =
                nbrFvMesh.boundary()[mpp.samplePolyPatch().index()];


            scalargpuField emissivity
            (
                radiation.absorptionEmission().e()().boundaryField()
                [
                    nbrPatch.index()
                ]
            );
            mpp.distribute(emissivity);

            return emissivity;

        }
        break;

        case LOOKUP:
        {
            // return local value
            return emissivity_;
        }

        default:
        {
            FatalErrorIn
            (
                "radiationCoupledBase::emissivity(const scalarField&)"
            )   << "Unimplemented method " << method_ << endl
                << "Please set 'emissivity' to one of "
                << emissivityMethodTypeNames_.toc()
                << exit(FatalError);
        }
        break;
    }

    return scalargpuField(0);
}


void Foam::radiationCoupledBase::autoMap
(
    const fvPatchFieldMapper& m
)
{
    emissivity_.autoMap(m);
}


void Foam::radiationCoupledBase::rmap
(
    const fvPatchScalarField& ptf,
    const labelList& addr
)
{
    const radiationCoupledBase& mrptf =
        refCast<const radiationCoupledBase>(ptf);

    emissivity_.rmap(mrptf.emissivity_, addr);
}


void Foam::radiationCoupledBase::write(Ostream& os) const
{
    os.writeKeyword("emissivityMode") << emissivityMethodTypeNames_[method_]
        << token::END_STATEMENT << nl;
    emissivity_.writeEntry("emissivity", os);
}


// ************************************************************************* //
