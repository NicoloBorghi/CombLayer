/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/PinholeBase.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "BaseMap.h"
#include "CellMap.h"
#include "PinholeBase.h"

namespace essSystem {

	PinholeBase::PinholeBase(const std::string& Key)   :	attachSystem::ContainedComp(),
								attachSystem::FixedComp(Key, 20),
								attachSystem::CellMap(),
								pinholeIndex( ModelSupport::objectRegister::Instance().cell(Key) ),
								cellIndex( pinholeIndex+1 ) {

		/*!
			Constructor
			\param Key :: Name of construction key
		*/
	}

	PinholeBase::PinholeBase(const PinholeBase& A)    :	attachSystem::ContainedComp(A),
								attachSystem::FixedComp(A),
								attachSystem::CellMap(A),  
								pinholeIndex(A.pinholeIndex),
								cellIndex(A.cellIndex),
								zImagingPlane(A.zImagingPlane),
								radialPinholeOffset(A.radialPinholeOffset),
								radialPinholePos(A.radialPinholePos),
								radialPinholeWidth(A.radialPinholeWidth),
								transversalPinholeOffset(A.transversalPinholeOffset),
								transversalPinholePos(A.transversalPinholePos),
								transversalPinholeWidth(A.transversalPinholeWidth),
								distancePinholeImagingPlane(A.distancePinholeImagingPlane),
								distanceTargetSurfImagingPlane(A.distanceTargetSurfImagingPlane),
								zPinholePos(A.zPinholePos) {
		/*!
			Copy constructor
			\param A :: PinholeBase to copy
		*/
	}

	PinholeBase& PinholeBase::operator=(const PinholeBase& A) {

		/*!
			Assignment operator
			\param A :: PinholeBase to copy
			\return *this
		*/

		if (this!=&A)     {

			attachSystem::ContainedComp::operator=(A);
			attachSystem::FixedComp::operator=(A);
			CellMap::operator=(A);
			cellIndex=A.cellIndex;
			zImagingPlane=A.zImagingPlane;
			radialPinholeOffset=A.radialPinholeOffset;
			radialPinholePos=A.radialPinholePos;
			radialPinholeWidth=A.radialPinholeWidth;
			transversalPinholeOffset=A.transversalPinholeOffset;
			transversalPinholePos=A.transversalPinholePos;
			transversalPinholeWidth=A.transversalPinholeWidth;
			distancePinholeImagingPlane=A.distancePinholeImagingPlane;
			distanceTargetSurfImagingPlane=A.distanceTargetSurfImagingPlane;
			zPinholePos=A.zPinholePos;
		}

		return *this;
	}
  
	PinholeBase::~PinholeBase() {

		/*!
			Destructor
		*/

	}

	void PinholeBase::setXYZSteps(double xS, double yS, double zS) {

		ELog::RegMethod RegA("PinholeBase","setXYZSteps");

		xStep = xS;
		yStep = yS;
		zStep = zS;

	}

	void PinholeBase::setAngles(double xyA, double zA) {

		ELog::RegMethod RegA("PinholeBase","setAngles");

		xyAngle = xyA;
		zAngle = zA;

	}

	void PinholeBase::setDimensions(double l, double w, double h) {

		ELog::RegMethod RegA("PinholeBase","setDimensions");

		length = l;
		width = w;
		height = h;

	}

	void PinholeBase::setTargetTopSurfaceZ(double z) {

		ELog::RegMethod RegA("PinholeBase","setTargetTopSurf");

		targetTopSurfZ = z;

	}

	void PinholeBase::populateBase(const FuncDataBase& Control) {

		ELog::RegMethod RegA("PinholeBase","populateBase");

		distanceTargetSurfImagingPlane = Control.EvalVar<double>(keyName+"DistanceTargetSurfImagingPlane");
		distancePinholeImagingPlane    = Control.EvalVar<double>(keyName+"DistancePinholeImagingPlane");

		radialPinholeOffset            = Control.EvalVar<double>(keyName+"RadialPinholeOffset");
		radialPinholeWidth             = Control.EvalVar<double>(keyName+"RadialPinholeWidth");

		transversalPinholeOffset       = Control.EvalVar<double>(keyName+"TransversalPinholeOffset");
		transversalPinholeWidth        = Control.EvalVar<double>(keyName+"TransversalPinholeWidth");

		//throw ColErr::RangeError<double>(theta, 0, 360, "Theta must be set in range 0-360 deg");

	}

}
















/*
  /////////////////////////////////////////////////////////////////////////////

  PHType = "rect"; // Control.EvalVar<double>(keyName+"PHType"); // For now we only build a rectangular pinhole

  zImagingPlane = Control.EvalVar<double>(keyName+"ZImagingPlane") + 5.80;

  /////////////////////////////////////////////////////////////////////////////

  radialPHOffset = -1.0*Control.EvalVar<double>(keyName+"RadialPHOffset");

  if (fabs(radialPHOffset) >= (length/2.0)) {

    ELog::EM << "Radial Offset out of range. Setting to 0" << ELog::endErr;
    radialPHOffset = 0.0;

  }

  transversalPHOffset = -1.0*Control.EvalVar<double>(keyName+"TransversalPHOffset");
  //transversalPHOffset = -1.0*Control.EvalDefVar<double>(keyName+"TransversalPHOffset",0);

  if (fabs(transversalPHOffset) >= (width/2.0)) {

    ELog::EM << "Transversal Offset out of range. Setting to 0" << ELog::endDiag;
    transversalPHOffset = 0.0;

  }

  radialPHPos      = radialPHOffset*sin(90.0 + xyAngle) + transversalPHOffset*cos(90.0 + xyAngle);
  transversalPHPos = radialPHOffset*cos(90.0 + xyAngle) + transversalPHOffset*sin(90.0 + xyAngle);

  radialPHWidth = Control.EvalVar<double>(keyName+"RadialPHWidth");
  transversalPHWidth = Control.EvalVar<double>(keyName+"TransversalPHWidth");

  /////////////////////////////////////////////////////////////////////////////

  const Geometry::Plane *floorPlane = SMap.realPtr<Geometry::Plane>(DPfloorSurfaceNumber);

  height = zImagingPlane - floorPlane->getDistance();

  zDistanceFromImage = Control.EvalVar<double>(keyName+"ZDistanceFromImage");

  if (fabs(zDistanceFromImage) > height) {

    ELog::EM << "Pinhole distance from imaging plane out of range. Setting to half height" << ELog::endDiag;
    zDistanceFromImage = zImagingPlane/2.0;

  }

  zPHPos = zImagingPlane - zDistanceFromImage;

  ELog::EM << "X:" << transversalPHPos << " - Y: " << radialPHPos << " - Z: " << zPHPos << ELog::endDiag;
  ELog::EM << "xStep:" << xStep << " - yStep: " << yStep << " - zStep: " << zStep << ELog::endDiag;



*/
