/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   pipeBuild/pipeVariables.cxx
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

namespace setVariable
{

  
void
simplebox_comblayer1Variables(FuncDataBase& Control)
  /*!
    Create all the beamline variabes
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("simplebox_comblayer1Variables[F]","simplebox_comblayer1Variables");
  
  Control.addVariable("TubeObjLength",10.0);
  Control.addVariable("TubeObjWidth",10.0);
  Control.addVariable("TubeObjHeight",10.0);
  Control.addVariable("TubeObjXStep",0.0);
  Control.addVariable("TubeObjYStep",0.0);
  Control.addVariable("TubeObjZStep",0.0);

  Control.addVariable("TubeObjWallMat","Stainless304");

   Control.addVariable("BoxObjLength",10.0);
  Control.addVariable("BoxObjWidth",10.0);
  Control.addVariable("BoxObjHeight",10.0);
  Control.addVariable("BoxObjWallMat","H2O");

  Control.addVariable("BoxObjXStep",0.0);
  Control.addVariable("BoxObjYStep",0.0);
  Control.addVariable("BoxObjZStep",0.0);


  Control.addVariable("GreenObjLength",10.0);
  Control.addVariable("GreenObjWidth",10.0);
  Control.addVariable("GreenObjHeight",10.0);
  Control.addVariable("GreenObjXStep",10.0);
  Control.addVariable("GreenObjYStep",10.0);
  Control.addVariable("GreenObjZStep",10.0);

  Control.addVariable("GreenObjWallMat","Tungsten");

  Control.addVariable("ChopObjXStep",0.0);
  Control.addVariable("ChopObjYStep",11.0);
  Control.addVariable("ChopObjZStep",0.0);
  Control.addVariable("ChopObjXYangle",0.0);
  Control.addVariable("ChopObjZangle",0.0);

  Control.addVariable("ChopObjGap",1.0);
  Control.addVariable("ChopObjInnerRadius",30.0);
  Control.addVariable("ChopObjOuterRadius",37.5);
  Control.addVariable("ChopObjNDisk",2);

  Control.addVariable("ChopObj0Thick",0.2);
  Control.addVariable("ChopObj1Thick",0.2);
  Control.addVariable("ChopObjInnerMat","Inconnel");
  Control.addVariable("ChopObjOuterMat","B4C");
  
  Control.addVariable("ChopObjNBlades",2);
  Control.addVariable("ChopObjPhaseAngle0",95.0);
  Control.addVariable("ChopObjOpenAngle0",30.0);
  Control.addVariable("ChopObj1PhaseAngle0",95.0);
  Control.addVariable("ChopObj1OpenAngle0",30.0);

  Control.addVariable("ChopObj0PhaseAngle1",275.0);
  Control.addVariable("ChopObj0OpenAngle1",30.0);
  Control.addVariable("ChopObj1PhaseAngle1",275.0);
  Control.addVariable("ChopObj1OpenAngle1",30.0);

  
  Control.addVariable("pointSourceXStep",0.0);
  Control.addVariable("pointSourceYStep",1.0);
  Control.addVariable("pointSourceZStep",0.0);
  Control.addVariable("pointSourceRadius",0.2);
  Control.addVariable("pointSourceParticleType",1);  // neutron
  Control.addVariable("pointSourceASpread",90.0);
  Control.addVariable("pointSourceEnergy",3.0);
  Control.addVariable("pointSourceEProb",1.0);


  Control.addVariable("TubeObjnLayers",10);
  Control.addVariable("GreenObjnLayers",10);
  Control.addVariable("BoxObjnLayers",10);

  return;
}

}  // NAMESPACE setVariable
 
