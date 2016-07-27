/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photon/makePhoton2.cxx
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
#include <iterator>
#include <memory>

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
#include "stringCombine.h"
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
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "insertPlate.h"
#include "insertSphere.h"
#include "World.h"
#include "AttachSupport.h"
#include "plateInfo.h"
#include "PlateMod.h"
#include "EQDetector.h"

#include "makePhoton2.h"

namespace photonSystem
{

makePhoton2::makePhoton2() :
  PModObj(new photonSystem::PlateMod("PMod")),
  DetPlate(new constructSystem::insertPlate("TPlate")),
  DetHold(new constructSystem::insertSphere("THold"))
  /*!
    Constructor
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(PModObj);
  OR.addObject(DetPlate);
  OR.addObject(DetHold);
}



makePhoton2::~makePhoton2()
  /*!
    Destructor
   */
{}

void
makePhoton2::buildWings(Simulation& System)
  /*!
    make the wings
   */
{
  ELog::RegMethod RegA("makePhoton2","buildWings");
    int voidCell(74123);
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const FuncDataBase& Control=System.getDataBase();
  const double H=Control.EvalDefVar<double>("SPlateHeight",1.5);
  const double W=Control.EvalDefVar<double>("SPlateWidth",7.5);

  const Geometry::Vec3D YA=PModObj->getSignedLinkPt(1);
  const Geometry::Vec3D YB=PModObj->getSignedLinkPt(2);
  const Geometry::Vec3D XA=PModObj->getSignedLinkPt(3);
  const Geometry::Vec3D XB=PModObj->getSignedLinkPt(4);
  const Geometry::Vec3D ZA=PModObj->getSignedLinkPt(5);
  const Geometry::Vec3D ZB=PModObj->getSignedLinkPt(6);
  
  const double DX=XA.Distance(XB)/2.0;
  const double DY=YA.Distance(YB);   // Not dividde
  const double DZ=ZA.Distance(ZB)/2.0;
  
  for(size_t i=0;i<4;i++)
    {
      SPlate.push_back
	(std::shared_ptr<constructSystem::insertPlate>
	 (new constructSystem::insertPlate
	  ("SPlate"+StrFunc::makeString(i))));

      if (i>1)
	SPlate[i]->setValues(5.0,H,DY,"Lead");
      else
	SPlate[i]->setValues(DY,H,5.0,"Lead");
      SPlate[i]->setStep(0,H/2.0,0);
      
      OR.addObject(SPlate[i]);
      SPlate[i]->addInsertCell(voidCell);
      SPlate[i]->createAll(System,*PModObj,static_cast<long int>(i+3));
    }
  return;
}
void 
makePhoton2::build(Simulation* SimPtr,
		  const mainSystem::inputParam& IParam)
/*!
    Carry out the full build
    \param SimPtr :: Simulation system
    \param IParam :: Input parameters
   */
{
  // For output stream
  ELog::RegMethod RControl("makePhoton2","build");


  int voidCell(74123);
  
  PModObj->addInsertCell(voidCell);
  PModObj->createAll(*SimPtr,World::masterOrigin(),0);
  buildWings(*SimPtr);
  DetHold->addInsertCell(voidCell);
  DetHold->createAll(*SimPtr,World::masterOrigin(),0);

  DetPlate->addInsertCell(DetHold->getCell("Main"));
  DetPlate->createAll(*SimPtr,World::masterOrigin(),0);


  return;
}


}   // NAMESPACE photonSystem
