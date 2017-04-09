/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tungstenBuild/tungstenSample.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfDIter.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
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
#include "SurInter.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "LayerComp.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "surfDBase.h"
#include "mergeTemplate.h"

#include "tungstenSample.h"

namespace tungstenSystem {

        tungstenSample::tungstenSample(const std::string& Key) : attachSystem::ContainedComp(),
                                                                 attachSystem::FixedOffset(Key,6),
                                                                 sampleIndex(ModelSupport::objectRegister::Instance().cell(Key)),
                                                                 cellIndex(sampleIndex+1) {

        /*!
                Constructor
                \param Key :: Name of construction key
        */

        }

        tungstenSample::tungstenSample(const tungstenSample& A) : attachSystem::ContainedComp(A),
                                                                  attachSystem::FixedOffset(A),
                                                                  attachSystem::CellMap(A),
                                                                  sampleIndex(A.sampleIndex),
                                                                  cellIndex(A.cellIndex) {

        /*!
                Copy constructor
                \param A :: tungstenSample to copy
        */

        }

        tungstenSample &tungstenSample::operator=(const tungstenSample& A) {

        /*!
                Assignment operator
                \param A :: tungstenSample to copy
                \return *this
        */

                if (this != &A) {

                        attachSystem::ContainedComp::operator=(A);
                        attachSystem::FixedOffset::operator=(A);
                        attachSystem::CellMap::operator=(A);
                        //sampleIndex = A.sampleIndex;
                        cellIndex = A.cellIndex;

                }

                return *this;

        }

        tungstenSample::~tungstenSample() {

        /*!
                Destructor
        */

        }

        void tungstenSample::populate(const FuncDataBase& Control) {

        /*!
                Populate all the variables
                \param Control :: Variable table to use
        */

                ELog::RegMethod RegA("tungstenSample","populate");

                FixedOffset::populate(Control);

                brickWidth = Control.EvalVar<double>(keyName + "BrickWidth");

                //length=Control.EvalVar<double>(keyName+"Length");
                //wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

                //ModelSupport::populateDivide(Control, nWallLayers, keyName+"WLayerMat", wallMat,wallMatList);
                //ModelSupport::populateDivideLen(Control, nWallLayers, keyName+"WLayerThick", (height-innerHeight)/2.0, wallFracList);

                return;

        }

        void tungstenSample::createUnitVector(const attachSystem::FixedComp& FC) {

        /*!
                Create the unit vectors
                \param FC :: Fixed Component
                \param sideIndex :: link point
        */

                ELog::RegMethod RegA("tungstenSample","createUnitVector");

                attachSystem::FixedComp::createUnitVector(FC);

                return;

        }

        void tungstenSample::createSurfaces() {

        /*!
                Create planes for system
        */

                ELog::RegMethod RegA("tungstenSample","createSurfaces");

                //ModelSupport::buildPlane(SMap,sampleIndex+1,Origin-Y*(length/2.0),Y);

                return;

        }

        void tungstenSample::createObjects(Simulation& System) {

        /*!
                Create the vaned moderator
                \param System :: Simulation to add results
        */

                ELog::RegMethod RegA("tungstenSample","createObjects");

                std::string Out;

                // Inner 
                //Out=ModelSupport::getComposite(SMap,sampleIndex," 1 -2 13 -14 15 -16 ");
                //System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

                //CellMap::setCell("Inner",cellIndex-1);
                //Out=ModelSupport::getComposite(SMap,sampleIndex," 1 -2 3 -4 5 -6 (-13:14:-15:16) ");
  
                //System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
                //CellMap::setCell("Outer",cellIndex-1);
  
                //Out=ModelSupport::getComposite(SMap,sampleIndex," 1 -2 3 -4 5 -6 ");
                //addOuterSurf(Out);

                return;

        }

        void tungstenSample::createLinks() {

        /*!
                Creates a full attachment set
                First two are in the -/+Y direction and have a divider
                Last two are in the -/+X direction and have a divider
                The mid two are -/+Z direction
        */

                ELog::RegMethod RegA("tungstenSample","createLinks");

                //FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
                //FixedComp::setLinkSurf(0,-SMap.realSurf(sampleIndex+1));

                return;

        }

        void tungstenSample::createAll(Simulation& System, const attachSystem::FixedComp& FC) {

        /*!
                External build everything
                \param System :: Simulation
                \param FC :: Attachment point
                \param sideIndex :: sideIndex for link point
        */

                ELog::RegMethod RegA("tungstenSample","createAll");

                populate(System.getDataBase());
                createUnitVector(FC);

                createSurfaces();
                createObjects(System);
                createLinks();
                insertObjects(System);

                return;

        }

}  // NAMESPACE tungstenSystem
