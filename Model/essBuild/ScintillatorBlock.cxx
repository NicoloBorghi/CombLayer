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
                claddingRadius = Control.EvalVar<double>(keyName+"CladdingRadius");
                claddingSeparation = Control.EvalVar<double>(keyName+"CladdingSeparation");
                claddingDepth = Control.EvalVar<double>(keyName+"CladdingDepth");

                bulkMat  = ModelSupport::EvalMat<int>(Control,keyName+"Material");
                scintMat = ModelSupport::EvalMat<int>(Control,keyName+"ScintillatorMaterial");

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
                                             const attachSystem::FixedComp& roofFC,
                                             const size_t roofLP) {

                /*!
                        Create the collimator surfaces
                */

                ELog::RegMethod RegA("ScintillatorBlock","createSurfaces");

                return;

        }

        void ScintillatorBlock::createObjects(Simulation& System,
                                            attachSystem::FixedComp& FC,
                                            const attachSystem::FixedComp& roofFC,
                                            const size_t roofLP) {

                /*!
                        Create the objects for the straw collimator
                */

                ELog::RegMethod RegA("ScintillatorBlock","createObjects");

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
                                        const attachSystem::FixedComp& roofFC,
                                        const size_t roofLP) {

                /*!
                        External build everything
                        \param System :: Simulation
                        \param FC :: FixedComponent for origin
                */

                ELog::RegMethod RegA("ScintillatorBlock","createAll");

                populate(System.getDataBase());

                createUnitVector(FC);
                createSurfaces(FC,roofFC,roofLP);
                createObjects(System,FC,roofFC,roofLP);
                createLinks();

                return;

        }

}























