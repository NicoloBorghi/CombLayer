/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/DiagnosticPlug.cxx
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
#include "DiagnosticPlug.h"
#include "PinholeBase.h"
#include "RectangularPinhole.h"
#include "Plane.h"
#include "SurInter.h"

namespace essSystem
{

DiagnosticPlug::DiagnosticPlug(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,11),
  attachSystem::CellMap(),
  tIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(tIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
}

DiagnosticPlug::DiagnosticPlug(const DiagnosticPlug& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  attachSystem::CellMap(A),  
  tIndex(A.tIndex),cellIndex(A.cellIndex),xStep(A.xStep),
  yStep(A.yStep),zStep(A.zStep),xyAngle(A.xyAngle),zAngle(A.zAngle),
  width(A.width),length(A.length)
  /*!
    Copy constructor
    \param A :: DiagnosticPlug to copy
  */
{}

DiagnosticPlug&
DiagnosticPlug::operator=(const DiagnosticPlug& A)
  /*!
    Assignment operator
    \param A :: DiagnosticPlug to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      CellMap::operator=(A);
      cellIndex=A.cellIndex;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      width=A.width;
      length=A.length;
    }
  return *this;
}
  
  DiagnosticPlug*
  DiagnosticPlug::clone() const
  /*!
    Clone self
    \return new (this)
   */
  {
    return new DiagnosticPlug(*this);
  }
  

DiagnosticPlug::~DiagnosticPlug()
  /*!
    Destructor
   */
{}

void
DiagnosticPlug::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("DiagnosticPlug","populate");

    // Master values
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYangle");
  zAngle=Control.EvalVar<double>(keyName+"Zangle");
  
  width=Control.EvalVar<double>(keyName+"Width");
  length=Control.EvalVar<double>(keyName+"Length");

  const Geometry::Plane *floorPlane    = SMap.realPtr<Geometry::Plane>(floorSurfaceNumber);
  const Geometry::Plane *roofPlane     = SMap.realPtr<Geometry::Plane>(roofSurfaceNumber);
  const Geometry::Plane *targetTopSurf = SMap.realPtr<Geometry::Plane>(targetTopSurfaceNumber);

  height = fabs(roofPlane->getDistance() - floorPlane->getDistance());

  Pinhole->setXYZSteps(xStep,yStep,zStep);
  Pinhole->setAngles(xyAngle,zAngle);
  Pinhole->setDimensions(length,width, height);
  Pinhole->setBoundarySurfacesZ(floorPlane->getDistance(), roofPlane->getDistance(), targetTopSurf->getDistance());

  return;
}

void
DiagnosticPlug::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
  */
{
  ELog::RegMethod RegA("DiagnosticPlug","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);
  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);
  
  return;
}

void
DiagnosticPlug::setPinhole(PinholeBase *ph)
  /*!
    Load the pinhole base class to build the requested pinhole
  */
{

  ELog::RegMethod RegA("DiagnosticPlug","setPinhole");

  Pinhole = ph;

}

void
DiagnosticPlug::createSurfaces()
  /*!
    Create Surfaces for the Be
  */
{
  ELog::RegMethod RegA("DiagnosticPlug","createSurfaces");

  ModelSupport::buildPlane(SMap,tIndex+1,Origin-Y*length/2.0,Y);
  ModelSupport::buildPlane(SMap,tIndex+2,Origin+Y*length/2.0,Y);

  ModelSupport::buildPlane(SMap,tIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,tIndex+4,Origin+X*width/2.0,X);

  return;
}

void
DiagnosticPlug::createObjects(Simulation& System,
			      const attachSystem::FixedComp& floorFC,
			      const size_t floorLP,
			      const attachSystem::FixedComp& roofFC,
			      const size_t roofLP)

  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
    \param floorFC :: FixedComponent of the floor
    \param floorLP :: link point of floorFC
    \param roofFC :: FixedComponent of the roof
    \param roofLP :: link poin of roofFC
  */
{
  ELog::RegMethod RegA("DiagnosticPlug","createObjects");

  std::string strFloor = floorFC.getLinkString(floorLP);
  HeadRule HR;
  HR.procString(roofFC.getLinkString(roofLP));
  HR.makeComplement();
  std::string strRoof = HR.display();
  
  std::string Out;

  Out=ModelSupport::getComposite(SMap,tIndex," 1 -2 3 -4") + strFloor + strRoof; 
  System.addCell(MonteCarlo::Qhull(cellIndex++, 0, 0.0, Out));
  setCell("main",cellIndex-1);

  addOuterSurf(Out);

  return; 
}

void
DiagnosticPlug::createLinks()
  /*!
    Creates a full attachment set
    Links/directions going outwards true.
  */
{

  ELog::RegMethod RegA("DIagnosticPlug","createLinks");

  FixedComp::setConnect(0,Origin-Y*length/2.0,Y);
  FixedComp::setLinkSurf(0,SMap.realSurf(tIndex+1));

  FixedComp::setConnect(1,Origin+Y*length/2.0,-Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(tIndex+2));

  FixedComp::setConnect(2,Origin-X*width/2.0,X);
  FixedComp::setLinkSurf(2,SMap.realSurf(tIndex+3));

  FixedComp::setConnect(3,Origin-X*width/2.0,-X);
  FixedComp::setLinkSurf(3,SMap.realSurf(tIndex+4));

  return;
}


void
DiagnosticPlug::createAll(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const attachSystem::FixedComp& floorFC,
			  const size_t floorLP,
			  const attachSystem::FixedComp& roofFC,
			  const size_t roofLP)
/*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param floorFC :: FixedComponent of the floor
    \param floorLP :: link point of floorFC
    \param roofFC :: FixedComponent of the roof
    \param roofLP :: link poin of roofFC
  */
{
  ELog::RegMethod RegA("DiagnosticPlug","createAll");

  floorSurfaceNumber = floorFC.getLinkSurf(floorLP);
  roofSurfaceNumber = roofFC.getLinkSurf(roofLP);
  targetTopSurfaceNumber = floorFC.getLinkSurf(10);

  populate(System.getDataBase());

  createUnitVector(FC);
  createSurfaces();
  createObjects(System, floorFC, floorLP, roofFC, roofLP);
  createLinks();
  Pinhole->createAll(System, *this, floorFC, floorLP, roofFC, roofLP);
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE instrumentSystem