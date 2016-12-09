/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/ScintillatorBlock.cxx
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
#include "ScintillatorBlock.h"
#include "Plane.h"

namespace essSystem {

        ScintillatorBlock::ScintillatorBlock(const std::string& Key)     :      attachSystem::ContainedComp(),
                                                                                attachSystem::FixedComp(Key, 20),
                                                                                attachSystem::CellMap(),
                                                                                scintIndex( ModelSupport::objectRegister::Instance().cell(Key) ),
                                                                                cellIndex( scintIndex + 1 ) {

                /*!
                        Constructor
                        \param Key :: Name of construction key
                */

        }

        ScintillatorBlock::ScintillatorBlock(const ScintillatorBlock& A)   :    attachSystem::ContainedComp(A),
                                                                                attachSystem::FixedComp(A),
                                                                                attachSystem::CellMap(A),
                                                                                scintIndex(A.scintIndex),
                                                                                cellIndex(A.cellIndex) {

                /*!
                        Copy constructor
                        \param A :: ScintillatorBlock to copy
                */

        }

        ScintillatorBlock& ScintillatorBlock::operator=(const ScintillatorBlock& A) {

                if (this != &A) {

                        attachSystem::ContainedComp::operator=(A);
                        attachSystem::FixedComp::operator=(A);
                        CellMap::operator=(A);
                        cellIndex=A.cellIndex;

                }

                return *this;

        }

        ScintillatorBlock* ScintillatorBlock::clone() const {

                return new ScintillatorBlock(*this);

        }

        ScintillatorBlock::~ScintillatorBlock() {

                /*!
                        Destructor
                */

        }

        void ScintillatorBlock::setDimensions(double l, double w) {

                /*!
                        Set dimensions of the ScintillatorBlock
                */

                ELog::RegMethod RegA("ScintillatorBlock","setDimensions");

                length = l;
                width = w;

        }

        void ScintillatorBlock::populate(const FuncDataBase& Control) {

                /*!
                        Populate all the variables
                        \param Control :: Variable table to use
                */

                ELog::RegMethod RegA("ScintillatorBlock","populate");

                height = Control.EvalVar<double>(keyName+"Height");
                scintHeight  = Control.EvalVar<double>(keyName+"ScintillatorHeight");
                scintWidth = Control.EvalVar<double>(keyName+"ScintillatorWidth");
                scintLength = Control.EvalVar<double>(keyName+"ScintillatorLength");
                scintSeparation = Control.EvalVar<double>(keyName+"ScintillatorSeparation");
                claddingRadius = Control.EvalVar<double>(keyName+"CladdingRadius");
                claddingSeparation = Control.EvalVar<double>(keyName+"CladdingSeparation");
                claddingDepth = Control.EvalVar<double>(keyName+"CladdingDepth");

                const Geometry::Plane *imagingPlane = SMap.realPtr<Geometry::Plane>(imagingPlaneSurfaceNumber);

                zImagingPlane = imagingPlane->getDistance();
                zScintillatorTop = zImagingPlane + scintHeight;
                zBlockTop = zImagingPlane + height;

                bulkMat  = ModelSupport::EvalMat<int>(Control,keyName+"Material");
                scintMat = ModelSupport::EvalMat<int>(Control,keyName+"ScintillatorMaterial");
                //fiberMat  = ModelSupport::EvalMat<int>(Control,keyName+"FiberMaterial");
                //epoxyMat = ModelSupport::EvalMat<int>(Control,keyName+"EpoxyMaterial");

                return;

        }

        void ScintillatorBlock::createUnitVector(const attachSystem::FixedComp& FC) {

                /*!
                        Create the unit vectors
                        \param FC :: Fixed Component
                */

                ELog::RegMethod RegA("ScintillatorBlock","createUnitVector");

                attachSystem::FixedComp::createUnitVector(FC);

                return;

        }

        void ScintillatorBlock::createSurfaces(const attachSystem::FixedComp& FC,
                                               const attachSystem::FixedComp& strawsFC,
                                               const size_t strawsLP,
                                               const attachSystem::FixedComp& roofFC,
                                               const size_t roofLP) {

                /*!
                        Create the collimator surfaces
                */

                ELog::RegMethod RegA("ScintillatorBlock","createSurfaces");

                // Top of scintillator block
                ModelSupport::buildPlane(SMap,scintIndex+5,Origin+Z*zBlockTop,Z);

                // Top of scintillators
                ModelSupport::buildPlane(SMap,scintIndex+15,Origin+Z*zScintillatorTop,Z);

                // Central straw walls
                ModelSupport::buildPlane(SMap,scintIndex+13,Origin-X*(scintSeparation/2.0),X);
                ModelSupport::buildPlane(SMap,scintIndex+14,Origin+X*(scintSeparation/2.0),X);

                // External straw walls
                ModelSupport::buildPlane(SMap,scintIndex+23,Origin-X*(scintSeparation/2.0+scintWidth),X);
                ModelSupport::buildPlane(SMap,scintIndex+24,Origin+X*(scintSeparation/2.0+scintWidth),X);

                Geometry::Vec3D backWallPos = Origin - Y*(length/2.0);
                Geometry::Vec3D frontWallPos = Origin + Y*(length/2.0);

                Geometry::Vec3D wallPos;
                nScint = 0;

                // Transversal straw walls
                for (;;) {

                        wallPos = backWallPos + Y*((nScint+1)*scintLength);

                        if (wallPos.abs() < frontWallPos.abs()) {

                                ModelSupport::buildPlane(SMap,scintIndex+((nScint+1)*10 + 1),wallPos,Y);
                                nScint++;

                        } else if (wallPos == frontWallPos) {

                                nScint++;
                                break;

                        } else {

                                break;

                        }

                }
                return;

                ELog::EM << "=== nScint = " << nScint << " ===" << ELog::endDiag;

        }

        void ScintillatorBlock::createObjects(Simulation& System,
                                              attachSystem::FixedComp& FC,
                                              const attachSystem::FixedComp& strawsFC,
                                              const size_t strawsLP,
                                              const attachSystem::FixedComp& roofFC,
                                              const size_t roofLP) {

                /*!
                        Create the objects for the straw collimator
                */

                ELog::RegMethod RegA("ScintillatorBlock","createObjects");

                std::string strImage = strawsFC.getLinkString(strawsLP);
                std::string strRoof  = roofFC.getLinkComplement(roofLP);
                std::string strBackWall = FC.getLinkString(0);
                std::string strFrontWall = FC.getLinkComplement(1);
                std::string strLeftWall = FC.getLinkString(2);
                std::string strRightWall = FC.getLinkComplement(3);

                std::string Out;

                Out = strRoof + ModelSupport::getComposite(SMap,scintIndex," 5") + strBackWall + strFrontWall + strLeftWall + strRightWall;
                System.addCell(MonteCarlo::Qhull(cellIndex++, 0, 0.0, Out));

                // Central structure
                Out = ModelSupport::getComposite(SMap,scintIndex," -15 13 -14") + strImage + strBackWall + strFrontWall;
                System.addCell(MonteCarlo::Qhull(cellIndex++,bulkMat, 300.0, Out));

                // Outer straw surfaces
                Out = ModelSupport::getComposite(SMap,scintIndex," -15 -23") + strLeftWall + strImage + strBackWall + strFrontWall;
                System.addCell(MonteCarlo::Qhull(cellIndex++,bulkMat, 300.0, Out));

                Out = ModelSupport::getComposite(SMap,scintIndex," -15 24") + strRightWall + strImage + strBackWall + strFrontWall;
                System.addCell(MonteCarlo::Qhull(cellIndex++,bulkMat, 300.0, Out));

                std::string OutLeft = ModelSupport::getComposite(SMap,scintIndex,"-15 23 -13") + strImage;
                std::string OutRight = ModelSupport::getComposite(SMap,scintIndex,"-15 14 -24") + strImage;

                int tmpMatLeft = scintMat;
                int tmpMatRight = bulkMat;

                for (int i = 1; i <= nScint; i++) {

                        if (i == 1) {

                                Out = ModelSupport::getComposite(SMap,scintIndex + 10*i," -1") + strBackWall;

                        } else if (i == nScint) {

                                Out = ModelSupport::getComposite(SMap,scintIndex + 10*(i-1)," 1") + strFrontWall;

                        } else {

                                Out = ModelSupport::getComposite(SMap,scintIndex + 10*(i-1)," 1 -11");

                        }

                        tmpMatLeft = ((i % 2) == 0) ? scintMat : bulkMat;
                        tmpMatRight = ((i % 2) == 1) ? scintMat : bulkMat;

                        System.addCell(MonteCarlo::Qhull(cellIndex++,tmpMatLeft, 300.0, OutLeft + Out));
                        System.addCell(MonteCarlo::Qhull(cellIndex++,tmpMatRight, 300.0, OutRight + Out));

                }

                Out = ModelSupport::getComposite(SMap,scintIndex," -5 15") + strRightWall + strLeftWall + strBackWall + strFrontWall;
                System.addCell(MonteCarlo::Qhull(cellIndex++,bulkMat, 300.0, Out));

                Out = strImage + strRoof + strFrontWall + strBackWall + strLeftWall + strRightWall;
                addOuterSurf(Out);

                return;

        }

        void ScintillatorBlock::createLinks() {

                /*!
                        Create the link points
                */

                ELog::RegMethod RegA("ScintillatorBlock","createLinks");

                return;

        }

        void ScintillatorBlock::createAll(Simulation& System,
                                        attachSystem::FixedComp& FC,
                                        const attachSystem::FixedComp& strawsFC,
                                        const size_t strawsLP,
                                        const attachSystem::FixedComp& roofFC,
                                        const size_t roofLP) {

                /*!
                        External build everything
                        \param System :: Simulation
                        \param FC :: FixedComponent for origin
                */

                ELog::RegMethod RegA("ScintillatorBlock","createAll");

                imagingPlaneSurfaceNumber = strawsFC.getLinkSurf(strawsLP);
                roofSurfaceNumber = roofFC.getLinkSurf(roofLP);

                populate(System.getDataBase());

                createUnitVector(FC);
                createSurfaces(FC,strawsFC,strawsLP,roofFC,roofLP);
                createObjects(System,FC,strawsFC,strawsLP,roofFC,roofLP);
                createLinks();

                return;

        }

}























