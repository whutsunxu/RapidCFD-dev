/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2012 OpenFOAM Foundation
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

Class
    Foam::absoluteInternalEnergy

Description
    Thermodynamics mapping class to expose the absolute internal energy function
    as the standard internal energy function e(T).

\*---------------------------------------------------------------------------*/

#ifndef absoluteInternalEnergy_H
#define absoluteInternalEnergy_H

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                       Class absoluteInternalEnergy Declaration
\*---------------------------------------------------------------------------*/

template<class Thermo>
class absoluteInternalEnergy
{

public:

    // Constructors

        //- Construct
        absoluteInternalEnergy()
        {}


    // Member Functions

        //- Return the instantiated type name
        static word typeName()
        {
            return "absoluteInternalEnergy";
        }

        // Fundamental properties

            static word name()
            {
                return "ea";
            }

            // Absolute internal energy [J/kmol]
            __HOST____DEVICE__
            scalar he
            (
                const Thermo& thermo,
                const scalar p,
                const scalar T
            ) const
            {
                return thermo.ea(p, T);
            }

            // Heat capacity at constant volume [J/(kmol K)]
            __HOST____DEVICE__
            scalar cpv
            (
                const Thermo& thermo,
                const scalar p,
                const scalar T
            ) const
            {
                return thermo.cv(p, T);
            }

            //- cp/cv []
            __HOST____DEVICE__
            scalar cpBycpv
            (
                const Thermo& thermo,
                const scalar p,
                const scalar T
            ) const
            {
                return thermo.gamma(p, T);
            }

            // Absolute internal energy [J/kg]
            __HOST____DEVICE__
            scalar HE
            (
                const Thermo& thermo,
                const scalar p,
                const scalar T
            ) const
            {
                return thermo.Ea(p, T);
            }

            //- Temperature from absolute internal energy
            //  given an initial temperature T0
            __HOST____DEVICE__
            scalar THE
            (
                const Thermo& thermo,
                const scalar e,
                const scalar p,
                const scalar T0
            ) const
            {
                return thermo.TEa(e, p, T0);
            }
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
