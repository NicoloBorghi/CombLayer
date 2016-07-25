/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/RectangularPinhole.h
 *
 * Copyright (c) 2015 Konstantin Batkov
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#ifndef essSystem_RectangularPinhole_h
#define essSystem_RectangularPinhole_h

class Simulation;

namespace essSystem {

	/*!
		\class RectangularPinhole
		\author Nicolò Borghi
		\version 1.0
		\date July 2016
		\brief Rectangular pinhole for the ESS diagnostic plug
	*/


	class RectangularPinhole : public PinholeBase {

		public:

						     RectangularPinhole(const std::string&);
						     RectangularPinhole(const RectangularPinhole&);

				RectangularPinhole  &operator=(const RectangularPinhole&);

			virtual RectangularPinhole  *clone() const;

					   virtual  ~RectangularPinhole();

				      virtual void   createAll( Simulation&,
								attachSystem::FixedComp&,			/*FC*/
								const attachSystem::FixedComp&,			/*floorFC*/
								const size_t, 					/*floorLP*/
								const attachSystem::FixedComp&,			/*roofFC*/
								const size_t);					/*roofLP*/

		private:

					    double   radialPinholeWidth;					///< Radial width of the Pinhole
					    double   transversalPinholeWidth;					///< Transversal width of the Pinhole

		private:

					      void   populate(const FuncDataBase&);

					      void   createUnitVector(const attachSystem::FixedComp&);

					      void   createSurfaces(const attachSystem::FixedComp&,		/*FC*/
								    const attachSystem::FixedComp&,		/*floorFC*/
								    const size_t,				/*floorLP*/
								    const attachSystem::FixedComp&,		/*roofFC*/
								    const size_t);				/*roofLP*/

					      void   createRadialSurfaces(const attachSystem::FixedComp&,	/*FC*/
								    const attachSystem::FixedComp&,		/*floorFC*/
								    const size_t,				/*floorLP*/
								    const attachSystem::FixedComp&,		/*roofFC*/
								    const size_t);				/*roofLP*/

					      void   createTransversalSurfaces(const attachSystem::FixedComp&,	/*FC*/
								    const attachSystem::FixedComp&,		/*floorFC*/
								    const size_t,				/*floorLP*/
								    const attachSystem::FixedComp&,		/*roofFC*/
								    const size_t);				/*roofLP*/

					      void   createSideSurfaces(const attachSystem::FixedComp&,		/*FC*/
								    const attachSystem::FixedComp&,		/*floorFC*/
								    const size_t,				/*floorLP*/
								    const attachSystem::FixedComp&,		/*roofFC*/
								    const size_t);				/*roofLP*/

					      void   createObjects( Simulation&,
								    attachSystem::FixedComp&,			/*FC*/
								    const attachSystem::FixedComp&,		/*floorFC*/
								    const size_t,				/*floorLP*/
								    const attachSystem::FixedComp&,		/*roofFC*/
								    const size_t);				/*roofLP*/

					      void   createLinks();

	};

}

#endif
 
