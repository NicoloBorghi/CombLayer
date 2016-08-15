/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/StrawCollimator.cxx
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
#include "StrawCollimator.h"

namespace essSystem {

	StrawCollimator::StrawCollimator(const std::string& Key)	:	attachSystem::ContainedComp(),
										attachSystem::FixedComp(Key, 20),
										attachSystem::CellMap(),
										strawIndex( ModelSupport::objectRegister::Instance().cell(Key) ),
										cellIndex( strawIndex + 1 ) {

		/*!
			Constructor
			\param Key :: Name of construction key
		*/

	}

	StrawCollimator::StrawCollimator(const StrawCollimator& A)	:	attachSystem::ContainedComp(A),
										attachSystem::FixedComp(A),
										attachSystem::CellMap(A),
										strawIndex(A.strawIndex),
										cellIndex(A.cellIndex),
										zImagingPlane(A.zImagingPlane) {

		/*!
			Copy constructor
			\param A :: StrawCollimator to copy
		*/

	}

	StrawCollimator& StrawCollimator::operator=(const StrawCollimator& A) {

		if (this != &A) {

			attachSystem::ContainedComp::operator=(A);
			attachSystem::FixedComp::operator=(A);
			CellMap::operator=(A);
			cellIndex=A.cellIndex;
			zImagingPlane=A.zImagingPlane;

		}

		return *this;

	}

	StrawCollimator* StrawCollimator::clone() const {

		return new StrawCollimator(*this);

	}

	StrawCollimator::~StrawCollimator() {

		/*!
			Destructor
		*/

	}

	void StrawCollimator::setXYZSteps(double xS, double yS, double zS) {

		ELog::RegMethod RegA("StrawCollimator","setXYZSteps");

		xStep = xS;
		yStep = yS;
		zStep = zS;

		return;

	}

	void StrawCollimator::setAngles(double xyA, double zA) {

		ELog::RegMethod RegA("StrawCollimator","setAngles");

		xyAngle = xyA;
		zAngle = zA;

		return;

	}

	void StrawCollimator::setDimensions(double l, double w, double h) {

		ELog::RegMethod RegA("StrawCollimator","setDimensions");

		length = l;
		width = w;
		height = h;

		return;

	}

	void StrawCollimator::setBoundarySurfacesZ(double fZ, double rZ, double tZ) {

		ELog::RegMethod RegA("StrawCollimator","setTargetTopSurf");

		floorSurfaceZ = fZ;
		roofSurfaceZ = rZ;
		targetTopSurfZ = tZ;

		return;

	}

	void StrawCollimator::populate(const FuncDataBase& Control) {

		/*!
			Populate all the variables
			\param Control :: Variable table to use
		*/

		ELog::RegMethod RegA("StrawCollimator","populate");

		side = Control.EvalVar<double>(keyName+"Radius");

		if (side <= 0.0) {

			throw ColErr::RangeError<double>(side, 0.0, INFINITY, "The straw side must be positive");

		}

		radialSpacing = Control.EvalVar<double>(keyName+"RadialSpacing");
		transversalSpacing = Control.EvalVar<double>(keyName+"TransversalSpacing");

		oddRowOffset = Control.EvalVar<double>(keyName+"OddRowOffset");

		if ( radialSpacing <= side ) {

			throw ColErr::RangeError<double>(radialSpacing, 0.0, INFINITY,"Straws along the same row are overlapping (set " + keyName + "RadialSpacing > " + keyName + "Radius).");

		}

		if ( transversalSpacing <= 0.0 ) {

			throw ColErr::RangeError<double>(transversalSpacing, 0.0, INFINITY,"Distance between rows must be positive.");

		}


		

		return;

	}

}






















