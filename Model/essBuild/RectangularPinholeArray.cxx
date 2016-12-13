/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/RectangularPinholeArray.cxx
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
#include "RectangularPinholeArray.h"
#include "SurInter.h"


namespace essSystem {

        RectangularPinholeArray::RectangularPinholeArray(const std::string& Key) : PinholeBase(Key) {

                /*!
                        Constructor
                        \param Key :: Name of construction key
                */

        }

        RectangularPinholeArray::RectangularPinholeArray(const RectangularPinholeArray& A) : PinholeBase(A) {

                /*!
                        Copy constructor
                        \param A :: RectangularPinholeArray to copy
                */

        }

        RectangularPinholeArray& RectangularPinholeArray::operator=(const RectangularPinholeArray& A) {

                /*!
                        Assignment operator
                        \param A :: RectangularPinholeArray to copy
                        \return *this
                */


                if (this!=&A) {

                        PinholeBase::operator=(A);
                }

                return *this;

        }

        RectangularPinholeArray::~RectangularPinholeArray() {

                /*!
                        Destructor
                */

        }

        RectangularPinholeArray* RectangularPinholeArray::clone() const {

                /*!
                        Virtual copy constructor
                        \return new(this)
                */

                return new RectangularPinholeArray(*this);

        }

        void RectangularPinholeArray::populate(const FuncDataBase& Control) {

                /*!
                        Populate all the variables
                        \param Control :: Variable table to use
                */

                ELog::RegMethod RegA("RectangularPinholeArray","populate");

                populateBase(Control);

                radialPinholeWidth      = Control.EvalVar<double>(keyName+"RadialPinholeWidth");
                transversalPinholeWidth = Control.EvalVar<double>(keyName+"TransversalPinholeWidth");
                pinholeNumber           = Control.EvalVar<int>(keyName+"PinholeNumber");

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

                if ( pinholeNumber <= 0) {

                        throw ColErr::InContainerError<int>(pinholeNumber,"There must be at least one pinhole (" + keyName + "PinholeNumber > 0).");

                }

                return;

        }

        void RectangularPinholeArray::createUnitVector(const attachSystem::FixedComp& FC) {

                /*!
                        Create the unit vectors
                        \param FC :: Fixed Component
                */

                ELog::RegMethod RegA("RectangularPinholeArray","createUnitVector");
                attachSystem::FixedComp::createUnitVector(FC); // The UnitVector is created with respect to FC, which is relative to the DiagnosticPlug, so it shouldn't be necessary to apply any shift or rotations.
                //applyShift(xStep,yStep,zStep);
                //applyAngleRotate(xyAngle,zAngle);

                return;

        }

        void RectangularPinholeArray::createSurfaces(const attachSystem::FixedComp& FC,
                                                     const attachSystem::FixedComp& floorFC,
                                                     const size_t floorLP,
                                                     const attachSystem::FixedComp& roofFC,
                                                     const size_t roofLP) {

                /*!
                        Create the collimator surfaces
                */

                ELog::RegMethod RegA("RectangularPinholeArray","createSurfaces");

                // Calculate the absoulte pinhole position
                pinholePos = Origin + X*transversalPinholeOffset + Y*radialPinholeOffset + Z*zPinholePos;

                ELog::EM << "=== PINHOLE CENTER COORDINATES === X: " << pinholePos.X() << " - Y: " << pinholePos.Y() << " - Z: " << pinholePos.Z() << ELog::endDiag;

                // Imaging plane
                ModelSupport::buildPlane(SMap,pinholeIndex+5,Origin+Z*zImagingPlane,Z);

                createBoundaryWalls(FC);

                for (int i=0; i < pinholeNumber; i++) {

                        createTransversalSurfaces(FC,floorFC,floorLP,roofFC,roofLP,i);
                        createRadialSurfaces(FC,floorFC,floorLP,roofFC,roofLP,i);
                        createSideSurfaces(FC,floorFC,floorLP,roofFC,roofLP,i);

                }

                return;

        }

        void RectangularPinholeArray::createBoundaryWalls(const attachSystem::FixedComp& FC) {

                ELog::RegMethod RegA("RectangularPinholeArray","createBoundaryWalls");

                Geometry::Plane *backWall = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(0));
                Geometry::Plane *frontWall = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(1));
                Geometry::Plane *tmp;

                Geometry::Vec3D tmpPos;
                Geometry::Vec3D norm = backWall->getNormal();

                double pinholeElementLength = length/pinholeNumber;

                for (int i=0; i < pinholeNumber; i++) {

                        tmpPos = (Origin - Y*length/2.0) + Y*i*pinholeElementLength + Y*(pinholeElementLength/2.0) +
                                  X*transversalPinholeOffset + Y*radialPinholeOffset + Z*zPinholePos;
                        
                        ELog::EM << "=== PINHOLE No. " << i+1 << " CENTER COORDINATES === X: " << tmpPos.X() << " - Y: " << tmpPos.Y() << " - Z: " << tmpPos.Z() << ELog::endDiag;

                        pinholePositions.push_back(tmpPos);

                }

                // Put backWall as the first element of the array
                radialWalls.push_back(backWall);

                for (int i=1; i < pinholeNumber; i++) {

                        tmp = ModelSupport::buildPlane(SMap,pinholeIndex+i*10+1, (Origin - Y*length/2.0) + Y*i*pinholeElementLength, norm);
                        radialWalls.push_back(tmp);

                }

                // Put the frontWall as the last element of the array
                radialWalls.push_back(frontWall);

                return;

        }

        void RectangularPinholeArray::createTransversalSurfaces(const attachSystem::FixedComp& FC,
                                                                const attachSystem::FixedComp& floorFC,
                                                                const size_t floorLP,
                                                                const attachSystem::FixedComp& roofFC,
                                                                const size_t roofLP,
                                                                int elementIndex) {

                /*!
                        Create the collimator surfaces
                */

                ELog::RegMethod RegA("RectangularPinholeArray","createTransversalSurfaces");

                // Get three points for the first PH collimator face

                const Geometry::Plane *pl1 = (radialWalls.at(elementIndex))->clone();
                const Geometry::Plane *pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(2));
                const Geometry::Plane *pl3 = SMap.realPtr<Geometry::Plane>(pinholeIndex+5);

                Geometry::Vec3D p1 = SurInter::getPoint(pl1,pl2,pl3);

                //ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

                pl1 = (radialWalls.at(elementIndex))->clone();
                pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(3));
                pl3 = SMap.realPtr<Geometry::Plane>(pinholeIndex+5);

                Geometry::Vec3D p2 = SurInter::getPoint(pl1,pl2,pl3);

                //ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

                Geometry::Vec3D p3 = pinholePositions.at(elementIndex) - Y*fabs(radialPinholeWidth/2.0);

                //ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

                ModelSupport::buildPlane(SMap,pinholeIndex+(100*elementIndex+7),p1,p2,p3,(p3-p1)*(p3-p2));
  
                ////////////////////////////////////////////////////

                // Get three points for the second PH collimator face

                pl1 = (radialWalls.at(elementIndex))->clone();
                pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(2));
                pl3 = SMap.realPtr<Geometry::Plane>(floorFC.getLinkSurf(floorLP));

                p1 = SurInter::getPoint(pl1,pl2,pl3);

                //ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

                pl1 = (radialWalls.at(elementIndex))->clone();
                pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(3));
                pl3 = SMap.realPtr<Geometry::Plane>(floorFC.getLinkSurf(floorLP));

                p2 = SurInter::getPoint(pl1,pl2,pl3);

                //ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

                p3 = pinholePositions.at(elementIndex) - Y*fabs(radialPinholeWidth/2.0);

                //ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

                ModelSupport::buildPlane(SMap,pinholeIndex+(100*elementIndex+8),p1,p2,p3,(p3-p1)*(p3-p2));
  
                ////////////////////////////////////////////////////

                // Get three points for the third PH collimator face

                pl1 = (radialWalls.at(elementIndex+1))->clone();
                pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(2));
                pl3 = SMap.realPtr<Geometry::Plane>(pinholeIndex+5);

                p1 = SurInter::getPoint(pl1,pl2,pl3);

                //ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

                pl1 = (radialWalls.at(elementIndex+1))->clone();
                pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(3));
                pl3 = SMap.realPtr<Geometry::Plane>(pinholeIndex+5);

                p2 = SurInter::getPoint(pl1,pl2,pl3);

                //ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

                p3 = pinholePositions.at(elementIndex) + Y*fabs(radialPinholeWidth/2.0);

                //ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

                ModelSupport::buildPlane(SMap,pinholeIndex+(100*elementIndex+17),p1,p2,p3,(p3-p2)*(p3-p1));
  
                ////////////////////////////////////////////////////

                // Get three points for the fourth PH collimator face

                pl1 = (radialWalls.at(elementIndex+1))->clone();
                pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(2));
                pl3 = SMap.realPtr<Geometry::Plane>(floorFC.getLinkSurf(floorLP));

                p1 = SurInter::getPoint(pl1,pl2,pl3);

                //ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

                pl1 = (radialWalls.at(elementIndex+1))->clone();
                pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(3));
                pl3 = SMap.realPtr<Geometry::Plane>(floorFC.getLinkSurf(floorLP));

                p2 = SurInter::getPoint(pl1,pl2,pl3);

                //ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

                p3 = pinholePositions.at(elementIndex) + Y*fabs(radialPinholeWidth/2.0);

                //ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

                ModelSupport::buildPlane(SMap,pinholeIndex+(100*elementIndex+18),p1,p2,p3,(p3-p2)*(p3-p1));

                ////////////////////////////////////////////////////

                return;

        }

        void RectangularPinholeArray::createRadialSurfaces(const attachSystem::FixedComp& FC,
                                                           const attachSystem::FixedComp& floorFC,
                                                           const size_t floorLP,
                                                           const attachSystem::FixedComp& roofFC,
                                                           const size_t roofLP,
                                                           int elementIndex) {

                /*!
                        Create the collimator surfaces
                */

                ELog::RegMethod RegA("RectangularPinholeArray","createRadialSurfaces");

                // Get three points for the first PH collimator face

                const Geometry::Plane *pl1 = (radialWalls.at(elementIndex))->clone();
                const Geometry::Plane *pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(2));
                const Geometry::Plane *pl3 = SMap.realPtr<Geometry::Plane>(pinholeIndex+5);

                Geometry::Vec3D p1 = SurInter::getPoint(pl1,pl2,pl3);

                //ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

                pl1 = (radialWalls.at(elementIndex+1))->clone();
                pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(2));
                pl3 = SMap.realPtr<Geometry::Plane>(pinholeIndex+5);

                Geometry::Vec3D p2 = SurInter::getPoint(pl1,pl2,pl3);

                //ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

                Geometry::Vec3D p3 = pinholePositions.at(elementIndex) - X*fabs(transversalPinholeWidth/2.0);

                //ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

                ModelSupport::buildPlane(SMap,pinholeIndex+(100*elementIndex+27),p1,p2,p3,(p3-p2)*(p3-p1));
  
                ////////////////////////////////////////////////////

                // Get three points for the second PH collimator face

                pl1 = (radialWalls.at(elementIndex))->clone();
                pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(2));
                pl3 = SMap.realPtr<Geometry::Plane>(floorFC.getLinkSurf(floorLP));

                p1 = SurInter::getPoint(pl1,pl2,pl3);

                //ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

                pl1 = (radialWalls.at(elementIndex+1))->clone();
                pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(2));
                pl3 = SMap.realPtr<Geometry::Plane>(floorFC.getLinkSurf(floorLP));

                p2 = SurInter::getPoint(pl1,pl2,pl3);

                //ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

                p3 = pinholePositions.at(elementIndex) - X*fabs(transversalPinholeWidth/2.0);

                //ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

                ModelSupport::buildPlane(SMap,pinholeIndex+(100*elementIndex+28),p1,p2,p3,(p3-p2)*(p3-p1));
  
                ////////////////////////////////////////////////////

                // Get three points for the third PH collimator face

                pl1 = (radialWalls.at(elementIndex))->clone();
                pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(3));
                pl3 = SMap.realPtr<Geometry::Plane>(pinholeIndex+5);

                p1 = SurInter::getPoint(pl1,pl2,pl3);

                //ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

                pl1 = (radialWalls.at(elementIndex+1))->clone();
                pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(3));
                pl3 = SMap.realPtr<Geometry::Plane>(pinholeIndex+5);

                p2 = SurInter::getPoint(pl1,pl2,pl3);

                //ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

                p3 = pinholePositions.at(elementIndex) + X*fabs(transversalPinholeWidth/2.0);

                //ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

                ModelSupport::buildPlane(SMap,pinholeIndex+(100*elementIndex+37),p1,p2,p3,(p3-p1)*(p3-p2));
  
                ////////////////////////////////////////////////////

                // Get three points for the fourth PH collimator face

                pl1 = (radialWalls.at(elementIndex))->clone();
                pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(3));
                pl3 = SMap.realPtr<Geometry::Plane>(floorFC.getLinkSurf(floorLP));

                p1 = SurInter::getPoint(pl1,pl2,pl3);

                //ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

                pl1 = (radialWalls.at(elementIndex+1))->clone();
                pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(3));
                pl3 = SMap.realPtr<Geometry::Plane>(floorFC.getLinkSurf(floorLP));

                p2 = SurInter::getPoint(pl1,pl2,pl3);

                //ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

                p3 = pinholePositions.at(elementIndex) + X*fabs(transversalPinholeWidth/2.0);

                //ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

                ModelSupport::buildPlane(SMap,pinholeIndex+(100*elementIndex+38),p1,p2,p3,(p3-p1)*(p3-p2));
  
                ////////////////////////////////////////////////////

                return;

        }

        void RectangularPinholeArray::createSideSurfaces(const attachSystem::FixedComp& FC,
                                                         const attachSystem::FixedComp& floorFC,
                                                         const size_t floorLP,
                                                         const attachSystem::FixedComp& roofFC,
                                                         const size_t roofLP,
                                                         int elementIndex) {

                /*!
                        Create the collimator surfaces
                */

                ELog::RegMethod RegA("RectangularPinholeArray","createSideSurfaces");

                // Get three points for the first PH collimator face

                const Geometry::Plane *pl1 = (radialWalls.at(elementIndex))->clone();
                const Geometry::Plane *pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(2));
                const Geometry::Plane *pl3 = SMap.realPtr<Geometry::Plane>(pinholeIndex+5);

                Geometry::Vec3D p1 = SurInter::getPoint(pl1,pl2,pl3);

                //ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

                pl1 = (radialWalls.at(elementIndex))->clone();
                pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(2));
                pl3 = SMap.realPtr<Geometry::Plane>(floorFC.getLinkSurf(floorLP));

                Geometry::Vec3D p2 = SurInter::getPoint(pl1,pl2,pl3);

                //ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

                Geometry::Vec3D p3 = pinholePositions.at(elementIndex) - X*fabs(transversalPinholeWidth/2.0) - Y*fabs(radialPinholeWidth/2.0);

                //ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

                ModelSupport::buildPlane(SMap,pinholeIndex+(100*elementIndex+9),p1,p2,p3,(p3-p1)*(p3-p2));
  
                ////////////////////////////////////////////////////

                // Get three points for the second PH collimator face

                pl1 = (radialWalls.at(elementIndex))->clone();
                pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(3));
                pl3 = SMap.realPtr<Geometry::Plane>(pinholeIndex+5);

                p1 = SurInter::getPoint(pl1,pl2,pl3);

                //ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

                pl1 = (radialWalls.at(elementIndex))->clone();
                pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(3));
                pl3 = SMap.realPtr<Geometry::Plane>(floorFC.getLinkSurf(floorLP));

                p2 = SurInter::getPoint(pl1,pl2,pl3);

                //ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

                p3 = pinholePositions.at(elementIndex) + X*fabs(transversalPinholeWidth/2.0) - Y*fabs(radialPinholeWidth/2.0);

                //ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

                ModelSupport::buildPlane(SMap,pinholeIndex+(100*elementIndex+19),p1,p2,p3,(p3-p1)*(p3-p2));
  
                ////////////////////////////////////////////////////

                // Get three points for the third PH collimator face

                pl1 = (radialWalls.at(elementIndex+1))->clone();
                pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(2));
                pl3 = SMap.realPtr<Geometry::Plane>(pinholeIndex+5);

                p1 = SurInter::getPoint(pl1,pl2,pl3);

                //ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

                pl1 = (radialWalls.at(elementIndex+1))->clone();
                pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(2));
                pl3 = SMap.realPtr<Geometry::Plane>(floorFC.getLinkSurf(floorLP));

                p2 = SurInter::getPoint(pl1,pl2,pl3);

                //ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

                p3 = pinholePositions.at(elementIndex) - X*fabs(transversalPinholeWidth/2.0) + Y*fabs(radialPinholeWidth/2.0);

                //ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

                ModelSupport::buildPlane(SMap,pinholeIndex+(100*elementIndex+29),p1,p2,p3,(p3-p2)*(p3-p1));
  
                ////////////////////////////////////////////////////

                // Get three points for the third PH collimator face

                pl1 = (radialWalls.at(elementIndex+1))->clone();
                pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(3));
                pl3 = SMap.realPtr<Geometry::Plane>(pinholeIndex+5);

                p1 = SurInter::getPoint(pl1,pl2,pl3);

                //ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

                pl1 = (radialWalls.at(elementIndex+1))->clone();
                pl2 = SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(3));
                pl3 = SMap.realPtr<Geometry::Plane>(floorFC.getLinkSurf(floorLP));

                p2 = SurInter::getPoint(pl1,pl2,pl3);

                //ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

                p3 = pinholePositions.at(elementIndex) + X*fabs(transversalPinholeWidth/2.0) + Y*fabs(radialPinholeWidth/2.0);

                //ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

                ModelSupport::buildPlane(SMap,pinholeIndex+(100*elementIndex+39),p1,p2,p3,(p3-p2)*(p3-p1));
  
                ////////////////////////////////////////////////////

                return;

        }

        void RectangularPinholeArray::createObjects(Simulation& System,
                                                    attachSystem::FixedComp& FC,
                                                    const attachSystem::FixedComp& floorFC,
                                                    const size_t floorLP,
                                                    const attachSystem::FixedComp& roofFC,
                                                    const size_t roofLP) {

                /*!
                        Create the objects for the collimator
                */

                ELog::RegMethod RegA("RectangularPinholeArray","createObjects");

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

                for (int elementIndex = 0; elementIndex < pinholeNumber; elementIndex++) {

                        if (elementIndex > 0) {

                                strBackWall = ModelSupport::getComposite(SMap,pinholeIndex + elementIndex*10," 1");

                        }

                        if (elementIndex < (pinholeNumber - 1)) {

                                strFrontWall = ModelSupport::getComposite(SMap,pinholeIndex + elementIndex*10," -11");

                        } else {

                                strFrontWall = FC.getLinkComplement(1);

                        }

                        Out=ModelSupport::getComposite(SMap,pinholeIndex, pinholeIndex + 100*elementIndex, " -7M 8M 9M -19M") + strBackWall;
                        System.addCell(MonteCarlo::Qhull(cellIndex++, 7401, 300.0, Out));

                        Out=ModelSupport::getComposite(SMap,pinholeIndex, pinholeIndex + 100*elementIndex, " -17M 18M 29M -39M") + strFrontWall;
                        System.addCell(MonteCarlo::Qhull(cellIndex++, 7401, 300.0, Out));

                        Out=ModelSupport::getComposite(SMap,pinholeIndex, pinholeIndex + 100*elementIndex, " -27M 28M -9M -29M") + strLeftWall;
                        System.addCell(MonteCarlo::Qhull(cellIndex++, 7401, 300.0, Out));

                        Out=ModelSupport::getComposite(SMap,pinholeIndex, pinholeIndex + 100*elementIndex, " -37M 38M 19M 39M") + strRightWall;
                        System.addCell(MonteCarlo::Qhull(cellIndex++, 7401, 300.0, Out));

                        Out=ModelSupport::getComposite(SMap,pinholeIndex, pinholeIndex + 100*elementIndex, " -5 (7M:-8M:-9M:19M) (17M:-18M:-29M:39M) (27M:-28M:9M:29M) (37M:-38M:-19M:-39M)")  + strFloor;
                        System.addCell(MonteCarlo::Qhull(cellIndex++, 0, 0.0, Out));

                }

                Out = strFloor + strRoof + FC.getLinkString(0) + FC.getLinkComplement(1) + strLeftWall + strRightWall; 
                addOuterSurf(Out);

                return;

        }

        void RectangularPinholeArray::createLinks() {

                /*!
                        Create the link points
                */

                ELog::RegMethod RegA("RectangularPinholeArray","creatLinks");

                return;

        }

        void RectangularPinholeArray::createAll(Simulation& System,
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

                ELog::RegMethod RegA("RectangularPinholeArray","createAll");

                populate(System.getDataBase());

                createUnitVector(FC);
                createSurfaces(FC,floorFC,floorLP,roofFC,roofLP);
                createObjects(System,FC,floorFC,floorLP,roofFC,roofLP);
                createLinks();
                //insertObjects(System);       

                return;

        }

}