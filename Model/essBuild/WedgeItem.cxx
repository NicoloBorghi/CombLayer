/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/WedgeItem.cxx
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
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDIter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "SimProcess.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "World.h"
#include "WedgeItem.h"

namespace essSystem
{

WedgeItem::WedgeItem(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6),
  wedgeIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(wedgeIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

WedgeItem::WedgeItem(const WedgeItem& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  wedgeIndex(A.wedgeIndex),
  cellIndex(A.cellIndex),
  length(A.length),width(A.width),
  theta(A.theta),mat(A.mat)
  /*!
    Copy constructor
    \param A :: WedgeItem to copy
  */
{}

WedgeItem&
WedgeItem::operator=(const WedgeItem& A)
  /*!
    Assignment operator
    \param A :: WedgeItem to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      cellIndex=A.cellIndex;
      length=A.length;
      width=A.width;
      theta=A.theta;
      mat=A.mat;
    }
  return *this;
}

WedgeItem::~WedgeItem() 
  /*!
    Destructor
  */
{}

void
WedgeItem::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: FuncDataBase to use
 */
{
  ELog::RegMethod RegA("WedgeItem","populate");

  FixedOffset::populate(Control);

  ELog::EM<< "keyName: " << keyName << ELog::endDiag;
  
  length=Control.EvalVar<double>(keyName+"Length");
  width=Control.EvalVar<double>(keyName+"Width");
  theta=Control.EvalVar<double>(keyName+"Theta");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  return;
}
  
void
WedgeItem::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Linked object
  */
{
  ELog::RegMethod RegA("WedgeItem","createUnitVector");

  FixedComp::createUnitVector(FC);
  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);

  return;
}
  
void
WedgeItem::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("WedgeItem","createSurface");

  // divider:
  ModelSupport::buildPlane(SMap,wedgeIndex+3,Origin,X);

  ModelSupport::buildPlaneRotAxis(SMap,wedgeIndex+1,Origin-Y*width/2.0,Y,Z,theta);
  ModelSupport::buildPlaneRotAxis(SMap,wedgeIndex+2,Origin+Y*width/2.0,Y,Z,theta);

  ModelSupport::buildCylinder(SMap,wedgeIndex+7,Origin,Z,100);

  return;
}

void
WedgeItem::createObjects(Simulation& System,
			 const attachSystem::FixedComp& FC,
			 const int baseSurf,
			 const attachSystem::FixedComp& FL,
			 const int top, const int bottom)
  /*!
    Adds the all the components
    \param System :: Simulation item
    other parameters are the same as in WedgeItem::createAll
  */
{
  ELog::RegMethod RegA("WedgeItem","createObjects");

  std::string Out;

  Out = ModelSupport::getComposite(SMap, wedgeIndex, " 3 1 -2 7 ") +
    FC.getSignedLinkString(baseSurf) +
    FL.getSignedLinkString(top) + FL.getSignedLinkString(bottom);
  
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));

  addOuterSurf(Out);
  return;
}

void
WedgeItem::createLinks()
  /*!
    Create all the linkes [OutGoing]
  */
{
  ELog::RegMethod RegA("WedgeItem","createLinks");

  return;
}


void
WedgeItem::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC, const int baseSurf,
		     const attachSystem::FixedComp& FL,
		     const int top, const int bottom)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param baseSurf :: base surface of the wedge
    \param FL :: Flight line the wedge is inserted to
    \param top :: top link surface
    \param bottom :: bottom link surface
  */
{
  ELog::RegMethod RegA("WedgeItem","createAll");
  

  populate(System.getDataBase());
  createUnitVector(FC);
  createSurfaces();
  createLinks();
  createObjects(System,FC,baseSurf,FL,top,bottom);
  insertObjects(System);              

  return;
}

}  // NAMESPACE essSystem
