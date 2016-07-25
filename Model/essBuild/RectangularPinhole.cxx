/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/RectangularPinhole.cxx
 *
 * Copyright (c) 2015 by Konstantin Batkov
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Vec2D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "EllipticCyl.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "support.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "General.h"
#include "Plane.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "PinholeBase.h"
#include "RectangularPinhole.h"
#include "SurInter.h"


namespace essSystem {

	RectangularPinhole::RectangularPinhole(const std::string& Key) : PinholeBase(Key) {

		/*!
			Constructor
			\param Key :: Name of construction key
		*/

	}

	RectangularPinhole::RectangularPinhole(const RectangularPinhole& A) : PinholeBase(A) {

		/*!
			Copy constructor
			\param A :: RectangularPinhole to copy
		*/

	}

	RectangularPinhole& RectangularPinhole::operator=(const RectangularPinhole& A) {

		/*!
			Assignment operator
			\param A :: RectangularPinhole to copy
			\return *this
		*/


		if (this!=&A) {

			PinholeBase::operator=(A);
		}

		return *this;

	}

	RectangularPinhole::~RectangularPinhole() {

		/*!
			Destructor
		*/

	}

	RectangularPinhole* RectangularPinhole::clone() const {

		/*!
			Virtual copy constructor
			\return new(this)
		*/

		return new RectangularPinhole(*this);

	}

	void RectangularPinhole::populate(const FuncDataBase& Control) {

		/*!
			Populate all the variables
			\param Control :: Variable table to use
		*/

		ELog::RegMethod RegA("RectangularPinhole","populate");

		populateBase(Control);

		radialPinholeWidth      = Control.EvalVar<double>(keyName+"RadialPinholeWidth");
		transversalPinholeWidth = Control.EvalVar<double>(keyName+"TransversalPinholeWidth");

		if ( (radialPinholeWidth <= 0.0) || (radialPinholeWidth >= length) ) {

			throw ColErr::RangeError<double>(radialPinholeWidth, 0.0, length, "The radial size of the pinhole must be larger than 0 and must not exceed the radial dimension of the diagnostic plug.");

		}

		double overallSize = (radialPinholeWidth/2.0) + fabs(radialPinholeOffset);

		if ( overallSize >= (length/2.0) ) {

			throw ColErr::RangeError<double>(overallSize, 0.0, (length/2.0), "Pinhole radial size and the relative offset must not exceed the diagnostic plug size.");

		}

		if ( (transversalPinholeWidth <= 0.0) || (transversalPinholeWidth >= width) ) {

			throw ColErr::RangeError<double>(transversalPinholeWidth, 0.0, width, "The transversal size of the pinhole must be larger than 0 and must not exceed the transversal dimension of the diagnostic plug.");

		}

		overallSize = (transversalPinholeWidth/2.0) + fabs(transversalPinholeOffset);

		if ( overallSize >= (width/2.0) ) {

			throw ColErr::RangeError<double>(overallSize, 0.0, (width/2.0), "Pinhole transversal size and the relative offset must not exceed the diagnostic plug size.");

		}

		return;

	}

	void RectangularPinhole::createUnitVector(const attachSystem::FixedComp& FC) {

		/*!
			Create the unit vectors
			\param FC :: Fixed Component
		*/

		ELog::RegMethod RegA("RectangularPinhole","createUnitVector");
		attachSystem::FixedComp::createUnitVector(FC); // The UnitVector is created with respect to FC, which is relative to the DiagnosticPlug, so it shouldn't be necessary to apply any shift or rotations.
		//applyShift(xStep,yStep,zStep);
		//applyAngleRotate(xyAngle,zAngle);

		return;

	}

	void RectangularPinhole::createSurfaces(const attachSystem::FixedComp& FC,
						const attachSystem::FixedComp& floorFC,
						const size_t floorLP,
						const attachSystem::FixedComp& roofFC,
						const size_t roofLP) {

		/*!
			Create the collimator surfaces
		*/

		ELog::RegMethod RegA("RectangularPinhole","createSurfaces");

		// Calculate the absoulte pinhole position
		pinholePos = Origin + X*transversalPinholeOffset + Y*radialPinholeOffset + Z*zPinholePos;

		ELog::EM << "=== PINHOLE CENTER COORDINATES === X: " << pinholePos.X() << " - Y: " << pinholePos.Y() << " - Z: " << pinholePos.Z() << ELog::endDiag;

		// Imaging plane
		ModelSupport::buildPlane(SMap,pinholeIndex+5,Origin+Z*zImagingPlane,Z);

		createRadialSurfaces(FC,floorFC,floorLP,roofFC,roofLP);
		createTransversalSurfaces(FC,floorFC,floorLP,roofFC,roofLP);
		createSideSurfaces(FC,floorFC,floorLP,roofFC,roofLP);

		return;

	}

	void RectangularPinhole::createRadialSurfaces(const attachSystem::FixedComp& FC,
						      const attachSystem::FixedComp& floorFC,
						      const size_t floorLP,
						      const attachSystem::FixedComp& roofFC,
						      const size_t roofLP) {

		/*!
			Create the collimator surfaces
		*/

		ELog::RegMethod RegA("RectangularPinhole","createRadialSurfaces");

		// Get three points for the first PH collimator face

		const Geometry::Plane *pl1 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(0));
		const Geometry::Plane *pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(2));
		const Geometry::Plane *pl3 = SMap.realPtr<Geometry::Plane>(pinholeIndex+5);

		Geometry::Vec3D p1 = SurInter::getPoint(pl1,pl2,pl3);

		ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

		pl1 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(0));
		pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(3));
		pl3 = SMap.realPtr<Geometry::Plane>(pinholeIndex+5);

		Geometry::Vec3D p2 = SurInter::getPoint(pl1,pl2,pl3);

		ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

		Geometry::Vec3D p3 = pinholePos - Y*fabs(radialPinholeWidth/2.0);

		ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

		ModelSupport::buildPlane(SMap,pinholeIndex+7,p1,p2,p3,(p3-p1)*(p3-p2));
  
		////////////////////////////////////////////////////

		// Get three points for the second PH collimator face

		pl1 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(0));
		pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(2));
		pl3 = SMap.realPtr<Geometry::Plane>(floorFC.getLinkSurf(floorLP));

		p1 = SurInter::getPoint(pl1,pl2,pl3);

		ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

		pl1 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(0));
		pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(3));
		pl3 = SMap.realPtr<Geometry::Plane>(floorFC.getLinkSurf(floorLP));

		p2 = SurInter::getPoint(pl1,pl2,pl3);

		ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

		p3 = pinholePos - Y*fabs(radialPinholeWidth/2.0);

		ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

		ModelSupport::buildPlane(SMap,pinholeIndex+8,p1,p2,p3,(p3-p1)*(p3-p2));
  
		////////////////////////////////////////////////////

		// Get three points for the third PH collimator face

		pl1 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(1));
		pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(2));
		pl3 = SMap.realPtr<Geometry::Plane>(pinholeIndex+5);

		p1 = SurInter::getPoint(pl1,pl2,pl3);

		ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

		pl1 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(1));
		pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(3));
		pl3 = SMap.realPtr<Geometry::Plane>(pinholeIndex+5);

		p2 = SurInter::getPoint(pl1,pl2,pl3);

		ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

		p3 = pinholePos + Y*fabs(radialPinholeWidth/2.0);

		ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

		ModelSupport::buildPlane(SMap,pinholeIndex+17,p1,p2,p3,(p3-p2)*(p3-p1));
  
		////////////////////////////////////////////////////

		// Get three points for the fourth PH collimator face

		pl1 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(1));
		pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(2));
		pl3 = SMap.realPtr<Geometry::Plane>(floorFC.getLinkSurf(floorLP));

		p1 = SurInter::getPoint(pl1,pl2,pl3);

		ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

		pl1 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(1));
		pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(3));
		pl3 = SMap.realPtr<Geometry::Plane>(floorFC.getLinkSurf(floorLP));

		p2 = SurInter::getPoint(pl1,pl2,pl3);

		ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

		p3 = pinholePos + Y*fabs(radialPinholeWidth/2.0);

		ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

		ModelSupport::buildPlane(SMap,pinholeIndex+18,p1,p2,p3,(p3-p2)*(p3-p1));

		////////////////////////////////////////////////////

		return;

	}

	void RectangularPinhole::createTransversalSurfaces(const attachSystem::FixedComp& FC,
							   const attachSystem::FixedComp& floorFC,
							   const size_t floorLP,
							   const attachSystem::FixedComp& roofFC,
							   const size_t roofLP) {

		/*!
			Create the collimator surfaces
		*/

		ELog::RegMethod RegA("RectangularPinhole","createTransversalSurfaces");

		// Get three points for the first PH collimator face

		const Geometry::Plane *pl1 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(0));
		const Geometry::Plane *pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(2));
		const Geometry::Plane *pl3 = SMap.realPtr<Geometry::Plane>(pinholeIndex+5);

		Geometry::Vec3D p1 = SurInter::getPoint(pl1,pl2,pl3);

		ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

		pl1 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(1));
		pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(2));
		pl3 = SMap.realPtr<Geometry::Plane>(pinholeIndex+5);

		Geometry::Vec3D p2 = SurInter::getPoint(pl1,pl2,pl3);

		ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

		Geometry::Vec3D p3 = pinholePos - X*fabs(transversalPinholeWidth/2.0);

		ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

		ModelSupport::buildPlane(SMap,pinholeIndex+27,p1,p2,p3,(p3-p2)*(p3-p1));
  
		////////////////////////////////////////////////////

		// Get three points for the second PH collimator face

		pl1 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(0));
		pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(2));
		pl3 = SMap.realPtr<Geometry::Plane>(floorFC.getLinkSurf(floorLP));

		p1 = SurInter::getPoint(pl1,pl2,pl3);

		ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

		pl1 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(1));
		pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(2));
		pl3 = SMap.realPtr<Geometry::Plane>(floorFC.getLinkSurf(floorLP));

		p2 = SurInter::getPoint(pl1,pl2,pl3);

		ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

		p3 = pinholePos - X*fabs(transversalPinholeWidth/2.0);

		ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

		ModelSupport::buildPlane(SMap,pinholeIndex+28,p1,p2,p3,(p3-p2)*(p3-p1));
  
		////////////////////////////////////////////////////

		// Get three points for the third PH collimator face

		pl1 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(0));
		pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(3));
		pl3 = SMap.realPtr<Geometry::Plane>(pinholeIndex+5);

		p1 = SurInter::getPoint(pl1,pl2,pl3);

		ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

		pl1 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(1));
		pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(3));
		pl3 = SMap.realPtr<Geometry::Plane>(pinholeIndex+5);

		p2 = SurInter::getPoint(pl1,pl2,pl3);

		ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

		p3 = pinholePos + X*fabs(transversalPinholeWidth/2.0);

		ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

		ModelSupport::buildPlane(SMap,pinholeIndex+37,p1,p2,p3,(p3-p1)*(p3-p2));
  
		////////////////////////////////////////////////////

		// Get three points for the fourth PH collimator face

		pl1 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(0));
		pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(3));
		pl3 = SMap.realPtr<Geometry::Plane>(floorFC.getLinkSurf(floorLP));

		p1 = SurInter::getPoint(pl1,pl2,pl3);

		ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

		pl1 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(1));
		pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(3));
		pl3 = SMap.realPtr<Geometry::Plane>(floorFC.getLinkSurf(floorLP));

		p2 = SurInter::getPoint(pl1,pl2,pl3);

		ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

		p3 = pinholePos + X*fabs(transversalPinholeWidth/2.0);

		ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

		ModelSupport::buildPlane(SMap,pinholeIndex+38,p1,p2,p3,(p3-p1)*(p3-p2));
  
		////////////////////////////////////////////////////

		return;

	}

	void RectangularPinhole::createSideSurfaces(const attachSystem::FixedComp& FC,
						    const attachSystem::FixedComp& floorFC,
						    const size_t floorLP,
						    const attachSystem::FixedComp& roofFC,
						    const size_t roofLP) {

		/*!
			Create the collimator surfaces
		*/

		ELog::RegMethod RegA("RectangularPinhole","createSideSurfaces");

		// Get three points for the first PH collimator face

		const Geometry::Plane *pl1 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(0));
		const Geometry::Plane *pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(2));
		const Geometry::Plane *pl3 = SMap.realPtr<Geometry::Plane>(pinholeIndex+5);

		Geometry::Vec3D p1 = SurInter::getPoint(pl1,pl2,pl3);

		ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

		pl1 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(0));
		pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(2));
		pl3 = SMap.realPtr<Geometry::Plane>(floorFC.getLinkSurf(floorLP));

		Geometry::Vec3D p2 = SurInter::getPoint(pl1,pl2,pl3);

		ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

		Geometry::Vec3D p3 = pinholePos - X*fabs(transversalPinholeWidth/2.0) - Y*fabs(radialPinholeWidth/2.0);

		ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

		ModelSupport::buildPlane(SMap,pinholeIndex+9,p1,p2,p3,(p3-p1)*(p3-p2));
  
		////////////////////////////////////////////////////

		// Get three points for the second PH collimator face

		pl1 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(0));
		pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(3));
		pl3 = SMap.realPtr<Geometry::Plane>(pinholeIndex+5);

		p1 = SurInter::getPoint(pl1,pl2,pl3);

		ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

		pl1 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(0));
		pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(3));
		pl3 = SMap.realPtr<Geometry::Plane>(floorFC.getLinkSurf(floorLP));

		p2 = SurInter::getPoint(pl1,pl2,pl3);

		ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

		p3 = pinholePos + X*fabs(transversalPinholeWidth/2.0) - Y*fabs(radialPinholeWidth/2.0);

		ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

		ModelSupport::buildPlane(SMap,pinholeIndex+19,p1,p2,p3,(p3-p1)*(p3-p2));
  
		////////////////////////////////////////////////////

		// Get three points for the third PH collimator face

		pl1 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(1));
		pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(2));
		pl3 = SMap.realPtr<Geometry::Plane>(pinholeIndex+5);

		p1 = SurInter::getPoint(pl1,pl2,pl3);

		ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

		pl1 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(1));
		pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(2));
		pl3 = SMap.realPtr<Geometry::Plane>(floorFC.getLinkSurf(floorLP));

		p2 = SurInter::getPoint(pl1,pl2,pl3);

		ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

		p3 = pinholePos - X*fabs(transversalPinholeWidth/2.0) + Y*fabs(radialPinholeWidth/2.0);

		ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

		ModelSupport::buildPlane(SMap,pinholeIndex+29,p1,p2,p3,(p3-p2)*(p3-p1));
  
		////////////////////////////////////////////////////

		// Get three points for the third PH collimator face

		pl1 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(1));
		pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(3));
		pl3 = SMap.realPtr<Geometry::Plane>(pinholeIndex+5);

		p1 = SurInter::getPoint(pl1,pl2,pl3);

		ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

		pl1 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(1));
		pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(3));
		pl3 = SMap.realPtr<Geometry::Plane>(floorFC.getLinkSurf(floorLP));

		p2 = SurInter::getPoint(pl1,pl2,pl3);

		ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

		p3 = pinholePos + X*fabs(transversalPinholeWidth/2.0) + Y*fabs(radialPinholeWidth/2.0);

		ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

		ModelSupport::buildPlane(SMap,pinholeIndex+39,p1,p2,p3,(p3-p2)*(p3-p1));
  
		////////////////////////////////////////////////////

		return;

	}

	void RectangularPinhole::createObjects( Simulation& System,
						attachSystem::FixedComp& FC,
						const attachSystem::FixedComp& floorFC,
						const size_t floorLP,
						const attachSystem::FixedComp& roofFC,
						const size_t roofLP) {

		/*!
			Create the objects for the collimator
		*/

		ELog::RegMethod RegA("RectangularPinhole","createObjects");

		std::string strFloor = floorFC.getLinkString(floorLP);
		std::string strRoof  = roofFC.getLinkComplement(roofLP);
		std::string strBackWall = FC.getLinkString(0);
		std::string strFrontWall = FC.getLinkComplement(1);
		std::string strLeftWall = FC.getLinkString(2);
		std::string strRightWall = FC.getLinkComplement(3);


		attachSystem::CellMap* CM = dynamic_cast<attachSystem::CellMap*>(&FC);
		CM->deleteCellWithData(System, "main");

		
		std::string Out=ModelSupport::getComposite(SMap,pinholeIndex," 5") + strRoof + strBackWall + strFrontWall + strLeftWall + strRightWall; 
		System.addCell(MonteCarlo::Qhull(cellIndex++, 0, 0.0, Out));

		Out=ModelSupport::getComposite(SMap,pinholeIndex," -7 8 9 -19") + strBackWall;
		System.addCell(MonteCarlo::Qhull(cellIndex++, 7401, 300.0, Out));

		Out=ModelSupport::getComposite(SMap,pinholeIndex," -17 18 29 -39") + strFrontWall;
		System.addCell(MonteCarlo::Qhull(cellIndex++, 7401, 300.0, Out));

		Out=ModelSupport::getComposite(SMap,pinholeIndex," -27 28 -9 -29") + strLeftWall;
		System.addCell(MonteCarlo::Qhull(cellIndex++, 7401, 300.0, Out));

		Out=ModelSupport::getComposite(SMap,pinholeIndex," -37 38 19 39") + strRightWall;
		System.addCell(MonteCarlo::Qhull(cellIndex++, 7401, 300.0, Out));

		Out=ModelSupport::getComposite(SMap,pinholeIndex," -5 (7:-8:-9:19) (17:-18:-29:39) (27:-28:9:29) (37:-38:-19:-39)")  + strFloor;
		System.addCell(MonteCarlo::Qhull(cellIndex++, 0, 0.0, Out));

		//Out=ModelSupport::getComposite(SMap,pinholeIndex," -17 18") + strFrontWall + strLeftWall + strRightWall;
		//System.addCell(MonteCarlo::Qhull(cellIndex++, 0, 0.0, Out));

		//Out=ModelSupport::getComposite(SMap,pinholeIndex," -5 (7:-8:-9:19)") + strFrontWall + strLeftWall + strRightWall + strFloor + strRoof;
		//System.addCell(MonteCarlo::Qhull(cellIndex++, 0, 0.0, Out));

		/*Out=ModelSupport::getComposite(SMap,pinholeIndex," -27 28") + strLeftWall + strBackWall + strFrontWall;
		System.addCell(MonteCarlo::Qhull(cellIndex++, 0, 0.0, Out));

		Out=ModelSupport::getComposite(SMap,pinholeIndex," -37 38") + strRightWall + strBackWall + strFrontWall;
		System.addCell(MonteCarlo::Qhull(cellIndex++, 0, 0.0, Out));

		Out=ModelSupport::getComposite(SMap,pinholeIndex," -5 (27:-28) (37:-38)") + strBackWall + strFrontWall + strFloor + strRoof;
		System.addCell(MonteCarlo::Qhull(cellIndex++, 0, 0.0, Out));*/

		Out = strFloor + strRoof + strBackWall + strFrontWall + strLeftWall + strRightWall; 
		addOuterSurf(Out);

		return;

	}

	void RectangularPinhole::createLinks() {

		/*!
			Create the link points
		*/

		ELog::RegMethod RegA("RectangularPinhole","creatLinks");

		return;

	}

	void RectangularPinhole::createAll(Simulation& System,
					   attachSystem::FixedComp& FC,
					   const attachSystem::FixedComp& floorFC,
					   const size_t floorLP,
					   const attachSystem::FixedComp& roofFC,
					   const size_t roofLP) {

		/*|
			External build everything
			\param System :: Simulation
			\param FC :: FixedComponent for origin
		*/

		ELog::RegMethod RegA("RectangularPinhole","createAll");

		populate(System.getDataBase());

		createUnitVector(FC);
		createSurfaces(FC,floorFC,floorLP,roofFC,roofLP);
		createObjects(System,FC,floorFC,floorLP,roofFC,roofLP);
		createLinks();
		//insertObjects(System);       

		return;

	}

}
