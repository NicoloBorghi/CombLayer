/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tungstenBuild/tungstenGammaCell.cxx
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

#include "tungstenGammaCell.h"

namespace tungstenSystem {

        tungstenGammaCell::tungstenGammaCell(const std::string& Key) : attachSystem::ContainedComp(),
                                                                       attachSystem::FixedOffset(Key,6),
                                                                       gammaIndex(ModelSupport::objectRegister::Instance().cell(Key)),
                                                                       cellIndex(gammaIndex+1) {

        /*!
                Constructor
                \param Key :: Name of construction key
        */

        }

        tungstenGammaCell::tungstenGammaCell(const tungstenGammaCell& A) : attachSystem::ContainedComp(A),
                                                                           attachSystem::FixedOffset(A),
                                                                           attachSystem::CellMap(A),
                                                                           gammaIndex(A.gammaIndex),
                                                                           cellIndex(A.cellIndex) {

        /*!
                Copy constructor
                \param A :: tungstenGammaCell to copy
        */

        }

        tungstenGammaCell &tungstenGammaCell::operator=(const tungstenGammaCell& A) {

        /*!
                Assignment operator
                \param A :: tungstenGammaCell to copy
                \return *this
        */

                if (this != &A) {

                        attachSystem::ContainedComp::operator=(A);
                        attachSystem::FixedOffset::operator=(A);
                        attachSystem::CellMap::operator=(A);
                        //gammaIndex = A.gammaIndex;
                        cellIndex = A.cellIndex;

                }

                return *this;

        }

        tungstenGammaCell::~tungstenGammaCell() {

        /*!
                Destructor
        */

        }

        void tungstenGammaCell::populate(const FuncDataBase& Control) {

        /*!
                Populate all the variables
                \param Control :: Variable table to use
        */

                ELog::RegMethod RegA("tungstenGammaCell","populate");

                FixedOffset::populate(Control);

                leftWallThickness = Control.EvalVar<double>(keyName+"LeftWallThickness");
                rightWallThickness = Control.EvalVar<double>(keyName+"RightWallThickness");
                frontWallThickness = Control.EvalVar<double>(keyName+"FrontWallThickness");
                backWallThickness = Control.EvalVar<double>(keyName+"BackWallThickness");
                topWallThickness = Control.EvalVar<double>(keyName+"TopWallThickness");
                bottomWallThickness = Control.EvalVar<double>(keyName+"BottomWallThickness");

                innerWidth = Control.EvalVar<double>(keyName+"InnerWidth");
                innerHeight = Control.EvalVar<double>(keyName+"InnerHeight");
                innerDepth = Control.EvalVar<double>(keyName+"InnerDepth");

                totalVolume = Control.EvalVar<double>(keyName+"TotalVolume");

                atmosphereGas = ModelSupport::EvalMat<int>(Control,keyName+"AtmosphereGas");
                atmospherePressure = Control.EvalVar<double>(keyName+"AtmospherePressure");

                wallMaterial1 = ModelSupport::EvalMat<int>(Control,keyName+"WallMaterial1");
                wallMaterial2 = ModelSupport::EvalMat<int>(Control,keyName+"WallMaterial2");

                return;

        }

        void tungstenGammaCell::createUnitVector(const attachSystem::FixedComp& FC) {

        /*!
                Create the unit vectors
                \param FC :: Fixed Component
        */

                ELog::RegMethod RegA("tungstenGammaCell","createUnitVector");

                attachSystem::FixedComp::createUnitVector(FC);

                return;

        }

        void tungstenGammaCell::createSurfaces() {

        /*!
                Create planes for system
        */

                ELog::RegMethod RegA("tungstenGammaCell","createSurfaces");

                ModelSupport::buildPlane(SMap,gammaIndex+1,Origin-Y*(innerWidth/2.0),Y);
                ModelSupport::buildPlane(SMap,gammaIndex+2,Origin+Y*(innerWidth/2.0),Y);
                ModelSupport::buildPlane(SMap,gammaIndex+3,Origin-X*(innerDepth/2.0),X);
                ModelSupport::buildPlane(SMap,gammaIndex+4,Origin+X*(innerDepth/2.0),X);
                ModelSupport::buildPlane(SMap,gammaIndex+5,Origin-Z*(innerHeight/2.0),Z);
                ModelSupport::buildPlane(SMap,gammaIndex+6,Origin+Z*(innerHeight/2.0),Z);

                ModelSupport::buildPlane(SMap,gammaIndex+11,Origin-Y*(innerWidth/2.0 + leftWallThickness),Y);
                ModelSupport::buildPlane(SMap,gammaIndex+12,Origin+Y*(innerWidth/2.0 + leftWallThickness),Y);
                ModelSupport::buildPlane(SMap,gammaIndex+13,Origin-X*(innerDepth/2.0 + backWallThickness),X);
                ModelSupport::buildPlane(SMap,gammaIndex+14,Origin+X*(innerDepth/2.0 + frontWallThickness),X);
                ModelSupport::buildPlane(SMap,gammaIndex+15,Origin-Z*(innerHeight/2.0 + bottomWallThickness),Z);
                ModelSupport::buildPlane(SMap,gammaIndex+16,Origin+Z*(innerHeight/2.0 + topWallThickness),Z);

                return;

        }

        void tungstenGammaCell::createObjects(Simulation& System) {

        /*!
                Create the vaned moderator
                \param System :: Simulation to add results
        */

                ELog::RegMethod RegA("tungstenGammaCell","createObjects");

                std::string Out;

                Out = ModelSupport::getComposite(SMap,gammaIndex," 1 -2 3 -4 5 -6");
                System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
                CellMap::setCell("InnerCell",cellIndex-1);

                Out = ModelSupport::getComposite(SMap,gammaIndex," 11 -1 13 -4 15 -16");
                System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

                Out = ModelSupport::getComposite(SMap,gammaIndex," 2 -12 13 -4 15 -16");
                System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

                Out = ModelSupport::getComposite(SMap,gammaIndex," 1 -2 13 -3 15 -16");
                System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

                Out = ModelSupport::getComposite(SMap,gammaIndex," 1 -2 3 -4 6 -16");
                System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

                Out = ModelSupport::getComposite(SMap,gammaIndex," 1 -2 3 -4 15 -5");
                System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

                Out = ModelSupport::getComposite(SMap,gammaIndex," 11 -12 4 -14 15 -16");
                System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

                // Outer
                Out = ModelSupport::getComposite(SMap,gammaIndex," 11 -12 13 -14 15 -16");
                addOuterSurf(Out);

                return;

        }

        void tungstenGammaCell::createLinks() {

        /*!
                Creates a full attachment set
                First two are in the -/+Y direction and have a divider
                Last two are in the -/+X direction and have a divider
                The mid two are -/+Z direction
        */

                ELog::RegMethod RegA("tungstenGammaCell","createLinks");

                //FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
                //FixedComp::setLinkSurf(0,-SMap.realSurf(gammaIndex+1));

                return;

        }

        void tungstenGammaCell::createAll(Simulation& System, const attachSystem::FixedComp& FC) {

        /*!
                External build everything
                \param System :: Simulation
                \param FC :: Attachment point
        */

                ELog::RegMethod RegA("tungstenGammaCell","createAll");

                populate(System.getDataBase());
                createUnitVector(FC);

                createSurfaces();
                createObjects(System);
                createLinks();
                insertObjects(System);

                return;

        }

}  // NAMESPACE tungstenSystem


