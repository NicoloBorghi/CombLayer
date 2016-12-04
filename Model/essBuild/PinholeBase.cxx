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

        PinholeBase::PinholeBase(const std::string& Key)   :    attachSystem::ContainedComp(),
                                                                attachSystem::FixedComp(Key, 20),
                                                                attachSystem::CellMap(),
                                                                pinholeIndex( ModelSupport::objectRegister::Instance().cell(Key) ),
                                                                cellIndex( pinholeIndex+1 ) {

                /*!
                        Constructor
                        \param Key :: Name of construction key
                */

        }

        PinholeBase::PinholeBase(const PinholeBase& A)    :     attachSystem::ContainedComp(A),
                                                                attachSystem::FixedComp(A),
                                                                attachSystem::CellMap(A),  
                                                                pinholeIndex(A.pinholeIndex),
                                                                cellIndex(A.cellIndex),
                                                                zImagingPlane(A.zImagingPlane),
                                                                radialPinholeOffset(A.radialPinholeOffset),
                                                                radialPinholeWidth(A.radialPinholeWidth),
                                                                transversalPinholeOffset(A.transversalPinholeOffset),
                                                                transversalPinholeWidth(A.transversalPinholeWidth),
                                                                distancePinholeImagingPlane(A.distancePinholeImagingPlane),
                                                                distanceTargetSurfImagingPlane(A.distanceTargetSurfImagingPlane),
                                                                zPinholePos(A.zPinholePos),
                                                                pinholePos(A.pinholePos) {
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
                        radialPinholeWidth=A.radialPinholeWidth;
                        transversalPinholeOffset=A.transversalPinholeOffset;
                        transversalPinholeWidth=A.transversalPinholeWidth;
                        distancePinholeImagingPlane=A.distancePinholeImagingPlane;
                        distanceTargetSurfImagingPlane=A.distanceTargetSurfImagingPlane;
                        zPinholePos=A.zPinholePos;
                        pinholePos=A.pinholePos;
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

                return;

        }

        void PinholeBase::setAngles(double xyA, double zA) {

                ELog::RegMethod RegA("PinholeBase","setAngles");

                xyAngle = xyA;
                zAngle = zA;

                return;

        }

        void PinholeBase::setDimensions(double l, double w, double h) {

                ELog::RegMethod RegA("PinholeBase","setDimensions");

                length = l;
                width = w;
                height = h;

                return;

        }

        void PinholeBase::setBoundarySurfacesZ(double fZ, double rZ, double tZ) {

                ELog::RegMethod RegA("PinholeBase","setTargetTopSurf");

                floorSurfaceZ = fZ;
                roofSurfaceZ = rZ;
                targetTopSurfZ = tZ;

                return;

        }

        void PinholeBase::populateBase(const FuncDataBase& Control) {

                ELog::RegMethod RegA("PinholeBase","populateBase");


                distancePinholeImagingPlane    = Control.EvalVar<double>(keyName+"DistancePinholeImagingPlane");
                distanceTargetSurfImagingPlane = Control.EvalVar<double>(keyName+"DistanceTargetSurfImagingPlane");

                radialPinholeOffset            = Control.EvalVar<double>(keyName+"RadialPinholeOffset");

                transversalPinholeOffset       = Control.EvalVar<double>(keyName+"TransversalPinholeOffset");

                zImagingPlane = distanceTargetSurfImagingPlane + targetTopSurfZ;

                ELog::EM << "=== zImagingPlane === " << zImagingPlane << ELog::endDiag;

                if ( (zImagingPlane <= floorSurfaceZ) || (zImagingPlane >= roofSurfaceZ) ) { 

                        throw ColErr::RangeError<double>(distanceTargetSurfImagingPlane, 
                                                        (floorSurfaceZ - targetTopSurfZ),
                                                        (roofSurfaceZ - targetTopSurfZ),
                                                        "DistanceTargetSurfImagingPlane must be set within the diagnostic plug hole.");

                }

                heightImagingSystem = zImagingPlane - floorSurfaceZ;

                if ( (distancePinholeImagingPlane <= 0) || (distancePinholeImagingPlane >= heightImagingSystem) ) {

                        throw ColErr::RangeError<double>(distancePinholeImagingPlane,
                                                        0.0,
                                                        heightImagingSystem,
                                                        "DistancePinholeImagingPlane must be larger than 0 and must be be within the allowed gap in the diagnostic plug.");

                }

                zPinholePos = zImagingPlane - distancePinholeImagingPlane;

                if ( fabs(radialPinholeOffset) >= (length/2.0) ) {

                        throw ColErr::RangeError<double>(radialPinholeOffset,
                                                        -1.0*(length/2.0),
                                                        (length/2.0),
                                                        "The radial offset must not exceed the diagnostic plug radial borders.");

                }

                if ( fabs(transversalPinholeOffset) >= (width/2.0) ) {

                        throw ColErr::RangeError<double>(transversalPinholeOffset,
                                                        -1.0*(width/2.0),
                                                        (width/2.0),
                                                        "The transversal offset must not exceed the diagnostic plug transversal borders.");

                }

                return;

        }

}
