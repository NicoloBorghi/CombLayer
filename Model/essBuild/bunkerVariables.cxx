/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/bunkerVariables.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell/Konstantin Batkov
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
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "variableSetup.h"
#include "essVariables.h"

namespace setVariable
{


void
EssBunkerVariables(FuncDataBase& Control)
  /*!
    Create all the bunker variables
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("essVariables[F]","EssBunkerVariables");

  Control.addVariable("ABunkerLeftPhase",-65.0);
  Control.addVariable("ABunkerRightPhase",0.0);
  Control.addVariable("ABunkerLeftAngle",0.0);
  Control.addVariable("ABunkerRightAngle",0.0);
  Control.addVariable("ABunkerNSectors",10);
  Control.addVariable("ABunkerNVert",1);
  Control.addVariable("ABunkerNLayers",1);
  Control.addVariable("ABunkerNSegment",5);
  Control.addVariable("ABunkerActiveSegment",0);
  Control.addVariable("ABunkerActiveRoof",0);

  Control.addVariable("ABunkerNRoofVert",20);
  Control.addVariable("ABunkerNRoofSide",1);
  Control.addVariable("ABunkerNRoofRadial",12);

  Control.addVariable("ABunkerNSide",5);
  Control.addVariable("ABunkerNSideVert",5);
  Control.addVariable("ABunkerNSideThick",5);
  
  Control.addVariable("ABunkerWallRadius",2450.0);
  Control.addVariable("ABunkerFloorDepth",120.0);
  Control.addVariable("ABunkerRoofHeight",190.0);

  Control.addVariable("ABunkerWallThick",320.0);
  Control.addVariable("ABunkerSideThick",80.0);
  Control.addVariable("ABunkerRoofThick",195.0);
  Control.addVariable("ABunkerFloorThick",100.0);

  Control.addVariable("ABunkerVoidMat","Void");
  Control.addVariable("ABunkerWallMat","Steel71");
  Control.addVariable("ABunkerRoofMat","Aluminium");

  Control.addVariable("ABunkerRoofVert1",10.0);


  Control.addVariable("ABunkerRoofMat0","CarstonConc");
  Control.addVariable("ABunkerRoofMat3","CastIron");
  Control.addVariable("ABunkerRoofMat6","CarstonConc");
  Control.addVariable("ABunkerRoofMat14","CastIron");
  Control.addVariable("ABunkerRoofMat16","CarstonConc");

  Control.addVariable("ABunkerWallMat0","CarstenConc");
  Control.addVariable("ABunkerWallMat1","CastIron");
  Control.addVariable("ABunkerWallMat2","CastenConc");
  Control.addVariable("ABunkerWallMat3","CastIron");
  Control.addVariable("ABunkerWallMat4","CarstenConc");
  
  Control.addVariable("ABunkerWallLen1",0.5);
  Control.addVariable("ABunkerWallLen2",10.0);
 
  Control.addVariable("ABunkerLoadFile","ABunkerDef.xml");
  Control.addVariable("ABunkerOutFile","ABunker.xml");

  // PILLARS:
  
  Control.addVariable("ABunkerPillarsRadius",6.0);
  Control.addVariable("ABunkerPillarsMat","Stainless304");
  Control.addVariable("ABunkerPillarsNRadius",2);
  Control.addVariable("ABunkerPillarsNSector",11); // default
  Control.addVariable("ABunkerPillarsR0",600.0);
  Control.addVariable("ABunkerPillarsR1",900.0);
  Control.addVariable("ABunkerPillarsRS_0",-3.0);
  Control.addVariable("ABunkerPillarsRS_1",-9.0);
  Control.addVariable("ABunkerPillarsRS_2",-15.0);
  Control.addVariable("ABunkerPillarsRS_3",-21.0);
  Control.addVariable("ABunkerPillarsRS_4",-27.0);
  Control.addVariable("ABunkerPillarsRS_5",-34.0);
  Control.addVariable("ABunkerPillarsRS_6",-39.0);
  Control.addVariable("ABunkerPillarsRS_7",-46.0);
  Control.addVariable("ABunkerPillarsRS_8",-52.0);
  Control.addVariable("ABunkerPillarsRS_9",-57.0);
  Control.addVariable("ABunkerPillarsRS_10",-63.0);

  //
  // RIGHT BUNKER : B PART
  //
  Control.addVariable("BBunkerLeftPhase",0.0);
  Control.addVariable("BBunkerRightPhase",65.0);
  Control.addVariable("BBunkerLeftAngle",0.0);
  Control.addVariable("BBunkerRightAngle",0.0);
  Control.addVariable("BBunkerNSectors",9);
  Control.addVariable("BBunkerNVert",1);
  Control.addVariable("BBunkerNLayers",1);
  Control.addVariable("BBunkerActiveSegment",0);
  Control.addVariable("BBunkerActiveRoof",0);

  Control.addVariable("BBunkerNRoofVert",5);
  Control.addVariable("BBunkerNRoofSide",1);
  Control.addVariable("BBunkerNRoofRadial",4);

  Control.addVariable("BBunkerNSide",5);
  Control.addVariable("BBunkerNSideVert",5);
  Control.addVariable("BBunkerNSideThick",5);
  
  Control.addVariable("BBunkerWallLen1",0.5);
  Control.addVariable("BBunkerWallLen2",10.0);
  
  Control.addVariable("BBunkerWallRadius",1150.0);
  Control.addVariable("BBunkerFloorDepth",120.0);
  Control.addVariable("BBunkerRoofHeight",190.0);

  Control.addVariable("BBunkerWallThick",320.0);
  Control.addVariable("BBunkerSideThick",80.0);
  Control.addVariable("BBunkerRoofThick",195.0);
  Control.addVariable("BBunkerFloorThick",100.0);

  Control.addVariable("BBunkerVoidMat","Void");
  Control.addVariable("BBunkerWallMat","Steel71");
  Control.addVariable("BBunkerRoofMat","CarstonConc");

  Control.addVariable("BBunkerLoadFile","BBunkerDef.xml");
  Control.addVariable("BBunkerOutFile","BBunker.xml");

  
  Control.addVariable("CurtainLeftPhase",-65.0);
  Control.addVariable("CurtainRightPhase",65.0);
  Control.addVariable("CurtainWallThick",20.0);
  Control.addVariable("CurtainInnerStep",25.0);
  Control.addVariable("CurtainTopRaise",60.0);
  Control.addVariable("CurtainHeight",250.0);
  Control.addVariable("CurtainDepth",100.0);
  Control.addVariable("CurtainWallMat","Concrete");
  Control.addVariable("CurtainNTopLayers",7);
  Control.addVariable("CurtainNMidLayers",2);
  Control.addVariable("CurtainNBaseLayers",7);


  return;
}

}  // NAMESPACE setVariable
