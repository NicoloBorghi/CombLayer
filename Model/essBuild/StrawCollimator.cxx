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

        StrawCollimator::StrawCollimator(const std::string& Key)        :       attachSystem::ContainedComp(),
                                                                                attachSystem::FixedComp(Key, 20),
                                                                                attachSystem::CellMap(),
                                                                                strawIndex( ModelSupport::objectRegister::Instance().cell(Key) ),
                                                                                cellIndex( strawIndex + 1 ) {

                /*!
                        Constructor
                        \param Key :: Name of construction key
                */

        }

        StrawCollimator::StrawCollimator(const StrawCollimator& A)      :       attachSystem::ContainedComp(A),
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

                distanceTargetSurfImagingPlane = Control.EvalVar<double>(keyName+"DistanceTargetSurfImagingPlane");
                zImagingPlane = distanceTargetSurfImagingPlane + targetTopSurfZ;

                ELog::EM << "=== zImagingPlane === " << zImagingPlane << ELog::endDiag;

                if ( (zImagingPlane <= floorSurfaceZ) || (zImagingPlane >= roofSurfaceZ) ) {

                        throw ColErr::RangeError<double>(distanceTargetSurfImagingPlane,
                                                        (floorSurfaceZ - targetTopSurfZ),
                                                        (roofSurfaceZ - targetTopSurfZ),
                                                        "DistanceTargetSurfImagingPlane must be set within the diagnostic plug hole.");

                }
        

                strawWidth = Control.EvalVar<double>(keyName+"StrawWidth");
                strawLength = Control.EvalVar<double>(keyName+"StrawLength");
                rowDistance = Control.EvalVar<double>(keyName+"RowDistance");

                if ( (strawWidth <= 0) || (strawWidth >= (width/2.0)) ) {

                        throw ColErr::RangeError<double>(strawWidth,0.0,(width/2.0),"Straw width must be positive and within the plug.");

                }

                if ( (strawLength <= 0) || (strawLength >= length) ) {

                        throw ColErr::RangeError<double>(strawLength,0.0,length,"Straw length must be positive and within the plug.");

                }

                if ( (rowDistance <= 0) || (rowDistance >= (width/2.0)) ) {

                        throw ColErr::RangeError<double>(rowDistance,0.0,(width/2.0),"Row distance must be positive and within the plug.");

                }

                double checkWidth = 2.0*strawWidth + rowDistance;

                if (checkWidth >= width) {

                        throw ColErr::RangeError<double>(checkWidth,0.0,width,"The width of the two straws plus the distance between the rows must not exceed the plug width.");

                }

		// Segmentation for activation calculations

		activationLayerThick1 = Control.EvalVar<double>(keyName+"ActivationLayerThick1");
		activationLayerThick2 = Control.EvalVar<double>(keyName+"ActivationLayerThick2") + activationLayerThick1;
		activationLayerThick3 = Control.EvalVar<double>(keyName+"ActivationLayerThick3") + activationLayerThick2;

                bulkMat = ModelSupport::EvalMat<int>(Control,keyName+"BulkMat");

                return;

        }

        void StrawCollimator::createUnitVector(const attachSystem::FixedComp& FC) {

                /*!
                        Create the unit vectors
                        \param FC :: Fixed Component
                */

                ELog::RegMethod RegA("StrawCollimator","createUnitVector");

                attachSystem::FixedComp::createUnitVector(FC); // The UnitVector is created with respect to FC, which is relative to the DiagnosticPlug, so it shouldn't be necessary to apply any shift or rotations.
                //applyShift(xStep,yStep,zStep);
                //applyAngleRotate(xyAngle,zAngle);

                return;

        }

        void StrawCollimator::createSurfaces(const attachSystem::FixedComp& FC,
                                             const attachSystem::FixedComp& floorFC,
                                             const size_t floorLP,
                                             const attachSystem::FixedComp& roofFC,
                                             const size_t roofLP) {

                /*!
                        Create the collimator surfaces
                */

                ELog::RegMethod RegA("StrawCollimator","createSurfaces");

                // Imaging plane
                ModelSupport::buildPlane(SMap,strawIndex+5,Origin+Z*zImagingPlane,Z);

                // Central straw walls
                ModelSupport::buildPlane(SMap,strawIndex+13,Origin-X*(rowDistance/2.0),X);
                ModelSupport::buildPlane(SMap,strawIndex+14,Origin+X*(rowDistance/2.0),X);

                // External straw walls
                ModelSupport::buildPlane(SMap,strawIndex+23,Origin-X*(rowDistance/2.0+strawWidth),X);
                ModelSupport::buildPlane(SMap,strawIndex+24,Origin+X*(rowDistance/2.0+strawWidth),X);

                Geometry::Vec3D backWallPos = Origin - Y*(length/2.0);
                Geometry::Vec3D frontWallPos = Origin + Y*(length/2.0);

                Geometry::Vec3D wallPos;
                nStraws = 0;

                // Transversal straw walls
                for (;;) {

                        wallPos = backWallPos + Y*((nStraws+1)*strawLength);

                        if (wallPos.abs() < frontWallPos.abs()) {

                                ModelSupport::buildPlane(SMap,strawIndex+((nStraws+1)*10 + 1),wallPos,Y);
                                nStraws++;

                        } else if (wallPos == frontWallPos) {

                                nStraws++;
                                break;

                        } else {

                                break;

                        }

                }

		// Horizontal planes for segmentation (activation)
		// Planes are positioned with reference to the Imaging plane defined above

		ModelSupport::buildPlane(SMap,strawIndex+6,Origin+Z*zImagingPlane-Z*activationLayerThick1,Z);
		ModelSupport::buildPlane(SMap,strawIndex+16,Origin+Z*zImagingPlane-Z*activationLayerThick2,Z);
		ModelSupport::buildPlane(SMap,strawIndex+26,Origin+Z*zImagingPlane-Z*activationLayerThick3,Z);

                ELog::EM << "=== nStraws = " << nStraws << " ===" << ELog::endDiag;

                return;

        }

        void StrawCollimator::createObjects(Simulation& System,
                                            attachSystem::FixedComp& FC,
                                            const attachSystem::FixedComp& floorFC,
                                            const size_t floorLP,
                                            const attachSystem::FixedComp& roofFC,
                                            const size_t roofLP) {

                /*!
                        Create the objects for the straw collimator
                */

                ELog::RegMethod RegA("StrawCollimator","createObjects");

                std::string strFloor = floorFC.getLinkString(floorLP);
                std::string strRoof  = roofFC.getLinkComplement(roofLP);
                std::string strBackWall = FC.getLinkString(0);
                std::string strFrontWall = FC.getLinkComplement(1);
                std::string strLeftWall = FC.getLinkString(2);
                std::string strRightWall = FC.getLinkComplement(3);

                attachSystem::CellMap* CM = dynamic_cast<attachSystem::CellMap*>(&FC);
                CM->deleteCellWithData(System, "main");

                std::string Out=ModelSupport::getComposite(SMap,strawIndex," 5") + strRoof + strBackWall + strFrontWall + strLeftWall + strRightWall;
                System.addCell(MonteCarlo::Qhull(cellIndex++, 0, 0.0, Out));

                // Central structure
                Out = ModelSupport::getComposite(SMap,strawIndex," -5 13 -14") + strFloor + strBackWall + strFrontWall;
                System.addCell(MonteCarlo::Qhull(cellIndex++,bulkMat, 300.0, Out));

                // Outer straw surfaces
                Out = ModelSupport::getComposite(SMap,strawIndex," -5 -23") + strLeftWall + strFloor + strBackWall + strFrontWall;
                System.addCell(MonteCarlo::Qhull(cellIndex++,bulkMat, 300.0, Out));

                Out = ModelSupport::getComposite(SMap,strawIndex," -5 24") + strRightWall + strFloor + strBackWall + strFrontWall;
                System.addCell(MonteCarlo::Qhull(cellIndex++,bulkMat, 300.0, Out));

                std::string OutLeft = ModelSupport::getComposite(SMap,strawIndex,"-5 23 -13") + strFloor;
                std::string OutRight = ModelSupport::getComposite(SMap,strawIndex,"-5 14 -24") + strFloor;

                int tmpMatLeft = 0;
                int tmpMatRight = bulkMat;

                for (int i = 1; i <= nStraws; i++) {

                        if (i == 1) {

                                Out = ModelSupport::getComposite(SMap,strawIndex + 10*i," -1") + strBackWall;

                        } else if (i == nStraws) {

                                Out = ModelSupport::getComposite(SMap,strawIndex + 10*(i-1)," 1") + strFrontWall;

                        } else {

                                Out = ModelSupport::getComposite(SMap,strawIndex + 10*(i-1)," 1 -11");

                        }

                        tmpMatLeft = ((i % 2) == 0) ? 0 : bulkMat;
                        tmpMatRight = ((i % 2) == 1) ? 0 : bulkMat;
                        
                        System.addCell(MonteCarlo::Qhull(cellIndex++,tmpMatLeft, 300.0, OutLeft + Out));
                        System.addCell(MonteCarlo::Qhull(cellIndex++,tmpMatRight, 300.0, OutRight + Out));

                }

                Out = strFloor + strRoof + FC.getLinkString(0) + FC.getLinkComplement(1) + strLeftWall + strRightWall;
                addOuterSurf(Out);

                return;

        }

        void StrawCollimator::createLinks() {

                /*!
                        Create the link points
                */

                ELog::RegMethod RegA("StrawCollimator","createLinks");

                return;

        }

        void StrawCollimator::createAll(Simulation& System,
                                        attachSystem::FixedComp& FC,
                                        const attachSystem::FixedComp& floorFC,
                                        const size_t floorLP,
                                        const attachSystem::FixedComp& roofFC,
                                        const size_t roofLP) {

                /*!
                        External build everything
                        \param System :: Simulation
                        \param FC :: FixedComponent for origin
                */

                ELog::RegMethod RegA("StrawCollimator","createAll");

                populate(System.getDataBase());

                createUnitVector(FC);
                createSurfaces(FC,floorFC,floorLP,roofFC,roofLP);
                createObjects(System,FC,floorFC,floorLP,roofFC,roofLP);
                createLinks();

                return;

        }

}






















