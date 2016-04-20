/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/IradCylinder.cxx
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
#include "SurInter.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "IradCylinder.h"

namespace essSystem
{

IradCylinder::IradCylinder(const std::string& Key) :
  attachSystem::FixedOffset(Key,6),
  iradIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(iradIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
 
}


IradCylinder::~IradCylinder()
  /*!
    Destructor
  */
{}

void
IradCylinder::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("IradCylinder","populate");

  return;
}

void
IradCylinder::createUnitVector(const attachSystem::FixedComp& FC,
			       const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
  */
{
  ELog::RegMethod RegA("IradCylinder","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);  
  return;
}

void
IradCylinder::createSurfaces()
  /*!
    Create surfaces for the model
  */
{
  ELog::RegMethod RegA("IradCylinder","createSurfaces");

  // Divide plane
  ModelSupport::buildPlane(SMap,iradIndex+1,Origin,X);  
  ModelSupport::buildPlane(SMap,iradIndex+2,Origin,Y);  
  // First surface when needed:

  return; 
}

void
IradCylinder::createObjects(Simulation& System)
   /*!
    Create the vaned moderator
    \param System :: Simulation to add results
    \param CMod :: Moderator fixed unit
  */
{
  ELog::RegMethod RegA("IradCylinder","createObjects");

  std::string Out;
 
  return; 
}

void
IradCylinder::createLinks()
  /*!
    Creates a full attachment set
    First two are in the -/+Y direction and have a divider
    Last two are in the -/+X direction and have a divider
    The mid two are -/+Z direction
  */
{  
  ELog::RegMethod RegA("IradCylinder","createLinks");

  return;

}


void
IradCylinder::createAll(Simulation& System,
			const attachSystem::FixedComp& CylMod,
			const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param CylMod :: Cylindrical moderator [3 surfaces]
   */
{
  ELog::RegMethod RegA("IradCylinder","createAll");

  populate(System.getDataBase());
  createUnitVector(CylMod,sideIndex);

  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       
  return;
}

}  // NAMESPACE essSystem

