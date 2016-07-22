/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuildInc/PinholeBase.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef essSystem_PinholeBase_h
#define essSystem_PinholeBase_h

class Simulation;

namespace essSystem {

	/*!
		\class PinholeBase
		\author Nicolò Borghi
		\version 1.0
		\date July 2016
		\brief Pinhole collimator base class
	*/

	class PinholeBase   :	public attachSystem::ContainedGroup,
				public attachSystem::ContainedComp,
				public attachSystem::FixedComp,
				public attachSystem::CellMap {

		public:

					 PinholeBase(const std::string&);
					 PinholeBase(const PinholeBase&);
					 PinholeBase& operator=(const PinholeBase&);
			       virtual   PinholeBase* clone() const=0;
			       virtual  ~PinholeBase();

			  virtual void   createAll(Simulation&, const attachSystem::FixedComp&) = 0;

  				  void   setXYZSteps(double, double, double);
				  void   setAngles(double, double);
				  void   setDimensions(double, double, double);
				  void   setBoundarySurfacesZ(double, double, double);
				  void   populateBase(const FuncDataBase&);

	 	protected:

			     const int   pinholeIndex;				///< Index of surface offset
				   int   cellIndex;				///< Cell index

			// Variables relative to the DiagnosticPlug, necessary for building the pinhole collimator

  				double   xStep;					///< X step (obtained from DiagnosticPlug)
				double   yStep;					///< Y step (obtained from DiagnosticPlug)
				double   zStep;					///< Z step (obtained from DiagnosticPlug)
				double   xyAngle;				///< XY Angle (obtained from DiagnosticPlug)
				double   zAngle;				///< Z Angle (obtained from DiagnosticPlug)
				double   width;					///< Width (obtained from DiagnosticPlug)
				double   length;				///< Length (obtained from DiagnosticPlug)
				double   height;				///< Height (obtained from DiagnosticPlug)
				double   floorSurfaceZ;				///< Z coordinate of the floor surface
				double   roofSurfaceZ;				///< Z coordinate of the roof surface
				double   targetTopSurfZ;			///< Z coordinate of the target upper surface

			// Variables relative to the Pinhole collimator common to all the possible pinhole geometries

				double   heightImagingSystem;			///< Height of the imaging system measured from the DiagnosticPlug floor to the imaging plane
				double   zImagingPlane;				///< Elevation (z coordinate) of the imaging plane (relative to the target upper surface)

				double   radialPinholeOffset;			///< Offset of the Pinhole center from the center of the DP
				double   radialPinholeWidth;			///< Radial width of the Pinhole


				double   transversalPinholeOffset;		///< Offset of the Pinhole center from the center of the DP
				double   transversalPinholeWidth;		///< Transversal width of the Pinhole

				double   distancePinholeImagingPlane;		///< Distance between the Pinhole and the imaging plane
				double   distanceTargetSurfImagingPlane;	///< Distance between the target upper surface and the imaging plane
				double   zPinholePos;				///< z position of the Pinhole

		       Geometry::Vec3D   pinholePos;				///< Vec3D to hold the absolute pinhole position

	};

}

#endif


/*


  // Pinhole (Pinhole) definition

  std::string PinholeType;             ///< The pinhole can be either "rect" (rectangular) or "circ" (circular)


  int DPfloorSurfaceNumber;
  int DProofSurfaceNumber;

*/
