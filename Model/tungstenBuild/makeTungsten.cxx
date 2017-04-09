/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tungstenBuild/makeTungsten.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include <iterator>
#include <memory>

#include <boost/format.hpp>


#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "insertInfo.h"
#include "insertBaseInfo.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "LayerComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "GroupOrigin.h"
#include "World.h"
#include "AttachSupport.h"

#include "tungstenGammaCell.h"
#include "tungstenSample.h"

#include "makeTungsten.h"

namespace tungstenSystem {

        makeTungsten::makeTungsten() : GammaCell(new tungstenSystem::tungstenGammaCell("GammaCell")),
                                       Sample(new tungstenSystem::tungstenSample("Sample")) {

        /*!
               Constructor
        */

                ModelSupport::objectRegister& OR = ModelSupport::objectRegister::Instance();

                OR.addObject(GammaCell);
                OR.addObject(Sample);

        }

        makeTungsten::makeTungsten(const makeTungsten &A) : GammaCell(new tungstenSystem::tungstenGammaCell(*(A.GammaCell))),
                                                            Sample(new tungstenSystem::tungstenSample(*(A.Sample))) {

        /*!
                Copy constructor
                \param A :: makeTungsten to copy
        */

        }

        makeTungsten &makeTungsten::operator=(const makeTungsten& A) {

        /*!
                Assignment operator
                \param A :: makeTungsten to copy
                \return *this
        */

                if (this!=&A) {

                        *GammaCell = *A.GammaCell;
                        *Sample = *A.Sample;

                }

                return *this;

        }

        makeTungsten::~makeTungsten() {

        /*!
                Destructor
        */

        }

        void makeTungsten::build(Simulation* SimPtr, const mainSystem::inputParam&) {

        /*!
                Carry out the full build
                \param SimPtr :: Simulation system
                \param :: Input parameters
        */

                // For output stream
                ELog::RegMethod RControl("makeTungsten","build");

                int voidCell(74123);

                GammaCell->addInsertCell(voidCell);
                GammaCell->createAll(*SimPtr,World::masterOrigin());

                //Sample->addInsertCell(voidCell);
                //Sample->createAll(*SimPtr,*GammaCell,2);

                return;

        }

}   // NAMESPACE tungstenSystem

