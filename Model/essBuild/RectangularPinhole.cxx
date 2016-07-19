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


namespace essSystem {

	RectangularPinhole::RectangularPinhole(const std::string& Key) : PinholeBase("PinholeBase") {

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

	}

	void RectangularPinhole::createUnitVector(const attachSystem::FixedComp& FC) {

		/*!
			Create the unit vectors
			\param FC :: Fixed Component
		*/

		ELog::RegMethod RegA("RectangularPinhole","createUnitVector");
		attachSystem::FixedComp::createUnitVector(FC);
		//applyShift(xStep,yStep,zStep);
		//applyAngleRotate(xyAngle,zAngle);

		return;

	}

	void RectangularPinhole::createSurfaces() {

		/*!
			Create the collimator surfaces
		*/

		ELog::RegMethod RegA("RectangularPinhole","createSrfaces");

		return;

	}

	void RectangularPinhole::createObjects(Simulation&) {

		/*!
			Create the objects for the collimator
		*/

		ELog::RegMethod RegA("RectangularPinhole","createObjects");

		return;

	}

	void RectangularPinhole::createLinks() {

		/*!
			Create the link points
		*/

		ELog::RegMethod RegA("RectangularPinhole","creatLinks");

		return;

	}

	void RectangularPinhole::createAll(Simulation& System, const attachSystem::FixedComp& FC) {

		/*|
			External build everything
			\param System :: Simulation
			\param FC :: FixedComponent for origin
		*/

		ELog::RegMethod RegA("RectangularPinhole","createAll");

	}








}
/*

  // Get three points for the first PH collimator face

  const Geometry::Plane *pl1 = SMap.realPtr<Geometry::Plane>(tIndex+1);
  const Geometry::Plane *pl2 = SMap.realPtr<Geometry::Plane>(tIndex+3);
  const Geometry::Plane *pl3 = SMap.realPtr<Geometry::Plane>(tIndex+5);

  Geometry::Vec3D p1 = SurInter::getPoint(pl1,pl2,pl3);

  ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

  pl1 = SMap.realPtr<Geometry::Plane>(tIndex+1);
  pl2 = SMap.realPtr<Geometry::Plane>(tIndex+4);
  pl3 = SMap.realPtr<Geometry::Plane>(tIndex+5);

  Geometry::Vec3D p2 = SurInter::getPoint(pl1,pl2,pl3);

  ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

  Geometry::Vec3D p3( xStep + transversalPHPos + fabs((radialPHWidth/2.0)*cos(90.0 + xyAngle)),
                      yStep + radialPHPos      - fabs((radialPHWidth/2.0)*sin(90.0 + xyAngle)),
                      zStep + zPHPos);

  ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

  Geometry::Vec3D norm(cos(90.0 + xyAngle),sin(90.0 + xyAngle),cos(atan(zDistanceFromImage/((length-radialPHWidth)/2.0))));

  ELog::EM << "CROSS PRODUCT ==> X: " << norm.X() << " - Y: " << norm.Y() << " - Z: " << norm.Z() << ELog::endDiag;

  Geometry::Vec3D A(p3.X() - p1.X(), p3.Y() - p1.Y(), p3.Z() - p1.Z());
  Geometry::Vec3D B(p3.X() - p2.X(), p3.Y() - p2.Y(), p3.Z() - p2.Z());
  //ELog::EM << "CROSS PRODUCT ==> X: " << calculateNorm(A/A.abs(),B/B.abs()).X() << " - Y: " << calculateNorm(A/A.abs(),B/B.abs()).Y() << " - Z: " << calculateNorm(A/A.abs(),B/B.abs()).Z() << ELog::endDiag;

  ModelSupport::buildPlane(SMap,tIndex+7,p1,p2,p3,norm);
  
  ////////////////////////////////////////////////////

  // Get three points for the second PH collimator face

  pl1 = SMap.realPtr<Geometry::Plane>(tIndex+1);
  pl2 = SMap.realPtr<Geometry::Plane>(tIndex+3);
  pl3 = SMap.realPtr<Geometry::Plane>(DPfloorSurfaceNumber);

  p1 = SurInter::getPoint(pl1,pl2,pl3);

  ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

  pl1 = SMap.realPtr<Geometry::Plane>(tIndex+1);
  pl2 = SMap.realPtr<Geometry::Plane>(tIndex+4);
  pl3 = SMap.realPtr<Geometry::Plane>(DPfloorSurfaceNumber);

  p2 = SurInter::getPoint(pl1,pl2,pl3);

  ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

  p3( xStep + transversalPHPos + fabs((radialPHWidth/2.0)*cos(90.0 + xyAngle)),
      yStep + radialPHPos      - fabs((radialPHWidth/2.0)*sin(90.0 + xyAngle)),
      zStep + zPHPos);

  ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

  norm(-sin(xyAngle),cos(xyAngle),cos(atan((height-zDistanceFromImage)/((length-radialPHWidth)/2.0))));

  ModelSupport::buildPlane(SMap,tIndex+8,p1,p2,p3,norm);
  
  ////////////////////////////////////////////////////

  // Get three points for the third PH collimator face

  pl1 = SMap.realPtr<Geometry::Plane>(tIndex+2);
  pl2 = SMap.realPtr<Geometry::Plane>(tIndex+3);
  pl3 = SMap.realPtr<Geometry::Plane>(tIndex+5);

  p1 = SurInter::getPoint(pl1,pl2,pl3);

  ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

  pl1 = SMap.realPtr<Geometry::Plane>(tIndex+2);
  pl2 = SMap.realPtr<Geometry::Plane>(tIndex+4);
  pl3 = SMap.realPtr<Geometry::Plane>(tIndex+5);

  p2 = SurInter::getPoint(pl1,pl2,pl3);

  ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

  p3( xStep + transversalPHPos - fabs((radialPHWidth/2.0)*cos(90.0 + xyAngle)),
      yStep + radialPHPos      + fabs((radialPHWidth/2.0)*sin(90.0 + xyAngle)),
      zStep + zPHPos);

  ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

  norm(-sin(xyAngle),cos(xyAngle),-cos(atan(zDistanceFromImage/((length+radialPHWidth)/2.0))));

  ModelSupport::buildPlane(SMap,tIndex+17,p1,p2,p3,norm);
  
  ////////////////////////////////////////////////////

  // Get three points for the fourth PH collimator face

  pl1 = SMap.realPtr<Geometry::Plane>(tIndex+2);
  pl2 = SMap.realPtr<Geometry::Plane>(tIndex+3);
  pl3 = SMap.realPtr<Geometry::Plane>(DPfloorSurfaceNumber);

  p1 = SurInter::getPoint(pl1,pl2,pl3);

  ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

  pl1 = SMap.realPtr<Geometry::Plane>(tIndex+2);
  pl2 = SMap.realPtr<Geometry::Plane>(tIndex+4);
  pl3 = SMap.realPtr<Geometry::Plane>(DPfloorSurfaceNumber);

  p2 = SurInter::getPoint(pl1,pl2,pl3);

  ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

  p3( xStep + transversalPHPos - fabs((radialPHWidth/2.0)*cos(90.0 + xyAngle)),
      yStep + radialPHPos      + fabs((radialPHWidth/2.0)*sin(90.0 + xyAngle)),
      zStep + zPHPos);

  ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

  norm(-sin(xyAngle),cos(xyAngle),-cos(atan((height-zDistanceFromImage)/((length+radialPHWidth)/2.0))));

  ModelSupport::buildPlane(SMap,tIndex+18,p1,p2,p3,norm);
  
  ////////////////////////////////////////////////////
























  Out=ModelSupport::getComposite(SMap,tIndex," 1 3 -4 -5 7 8") + strFloor;
  System.addCell(MonteCarlo::Qhull(cellIndex++, 0, 0.0, Out));

  Out=ModelSupport::getComposite(SMap,tIndex," -2 3 -4 -5 -17 -18") + strFloor;
  System.addCell(MonteCarlo::Qhull(cellIndex++, 0, 0.0, Out));

  Out=ModelSupport::getComposite(SMap,tIndex," -2 3 -4 -5 (-7:-8) (17:18)") + strFloor;
  System.addCell(MonteCarlo::Qhull(cellIndex++, 0, 0.0, Out));



*/
