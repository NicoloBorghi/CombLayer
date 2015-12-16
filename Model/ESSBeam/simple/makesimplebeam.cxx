/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1Upgrade/makePipe.cxx
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
#include "FixedGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "LayerComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "GroupOrigin.h"
#include "World.h"
#include "AttachSupport.h"
#include "DiskChopper.h"
#include "simplebeamTube.h"

#include "makesimplebeam.h"

namespace simplebeamSystem
{

makesimplebeam::makesimplebeam() :
  TubeObj(new simplebeamSystem::simplebeamTube("TubeObj")),
  BoxObj(new simplebeamSystem::simplebeamTube("BoxObj")),
  GreenObj(new simplebeamSystem::simplebeamTube("GreenObj")),
  ChopObj(new constructSystem::DiskChopper("ChopObj"))


  /*!
    Constructor
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(TubeObj);
  OR.addObject(BoxObj);
  OR.addObject(GreenObj);
  OR.addObject(ChopObj);
}

makesimplebeam::makesimplebeam(const makesimplebeam& A) : 
  TubeObj(new simplebeamSystem::simplebeamTube(*(A.TubeObj))),
  BoxObj(new simplebeamSystem::simplebeamTube(*(A.BoxObj))),
  GreenObj(new simplebeamSystem::simplebeamTube(*(A.GreenObj)))

  /*!
    Copy constructor
    \param A :: makesimplebeam to copy
  */
{}

makesimplebeam&
makesimplebeam::operator=(const makesimplebeam& A)
  /*!
    Assignment operator
    \param A :: makesimplebeam to copy
    \return *this
  */
{
  if (this!=&A)
    {
      *TubeObj=*A.TubeObj;
      *BoxObj=*A.BoxObj;
      *GreenObj=*A.GreenObj;
    }
  return *this;
}

makesimplebeam::~makesimplebeam()
  /*!
    Destructor
   */
{}

void 
makesimplebeam::build(Simulation* SimPtr,
		       const mainSystem::inputParam& IParam)
/*!
  Carry out the full build
  \param SimPtr :: Simulation system
  \param IParam :: Input parameters
*/
{
  build(SimPtr); 

}

void 
makesimplebeam::build(Simulation* SimPtr)
/*!
  Carry out the full build
  \param SimPtr :: Simulation system
  \param IParam :: Input parameters
*/
{
  // For output stream
  ELog::RegMethod RControl("makesimplebeam","build");

  int voidCell(74123);

  TubeObj->addInsertCell(voidCell);
  TubeObj->createAll(*SimPtr,World::masterOrigin(),0);

  GreenObj->addInsertCell(voidCell);
  GreenObj->createAll(*SimPtr,World::masterOrigin(),0);

  ChopObj->addInsertCell(voidCell);
  ChopObj->createAll(*SimPtr,World::masterOrigin(),0);


  BoxObj->addInsertCell(voidCell);
  BoxObj->createAll(*SimPtr,*TubeObj,2);


  attachSystem::addToInsertSurfCtrl(*SimPtr,*TubeObj,*GreenObj);

  //Print my LinkPoints
  TubeObj->getSignedLinkPt(1);
  TubeObj->getLinkAxis(1);
  TubeObj->getLinkString(1);
  ELog::EM<< TubeObj->getSignedLinkPt(1) <<ELog::endDiag;

  ELog::EM<<"this is also a test"<<ELog::endDiag;

  return;
}


}   // NAMESPACE ts1System

