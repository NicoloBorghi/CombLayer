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
#include "StrawCollimator.h"
#include "ScintillatorBlock.h"
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

  activationLayerThick1=Control.EvalVar<double>(keyName+"ActivationLayerThick1");
  activationLayerThick2=Control.EvalVar<double>(keyName+"ActivationLayerThick2") + activationLayerThick1;
  activationLayerThick3=Control.EvalVar<double>(keyName+"ActivationLayerThick3") + activationLayerThick2;
  activationLayerHeight=Control.EvalVar<double>(keyName+"ActivationLayerHeight");

  const Geometry::Plane *floorPlane    = SMap.realPtr<Geometry::Plane>(floorSurfaceNumber);
  const Geometry::Plane *roofPlane     = SMap.realPtr<Geometry::Plane>(roofSurfaceNumber);
  const Geometry::Plane *targetTopSurf = SMap.realPtr<Geometry::Plane>(targetTopSurfaceNumber);

  height = fabs(roofPlane->getDistance() - floorPlane->getDistance());

  int i = 0;
  double zPlane;

  for (;;) {

     zPlane = floorPlane->getDistance() + i*activationLayerHeight;

     if (zPlane <= roofPlane->getDistance()) {

        std::cout << "=============== zPlane ==================" << zPlane << std::endl;
        activationZPlanes.push_back(zPlane);
        i++;

     } else {

        break;

     }

  }

  if (Pinhole) {

     Pinhole->setXYZSteps(xStep,yStep,zStep);
     Pinhole->setAngles(xyAngle,zAngle);
     Pinhole->setDimensions(length,width, height);
     Pinhole->setBoundarySurfacesZ(floorPlane->getDistance(), roofPlane->getDistance(), targetTopSurf->getDistance());

  } else if (Straws) {

    Straws->setXYZSteps(xStep,yStep,zStep);
    Straws->setAngles(xyAngle,zAngle);
    Straws->setDimensions(length,width, height);
    Straws->setBoundarySurfacesZ(floorPlane->getDistance(), roofPlane->getDistance(), targetTopSurf->getDistance());

    Scintillator->setDimensions(length,width);

  }

  activationWallsMaterial = ModelSupport::EvalMat<int>(Control,keyName+"ActivationWallsMaterial");

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
  Straws = 0;

}

void
DiagnosticPlug::setStraws(StrawCollimator *sc)
  /*!
    Load the straw collimator class to build the corresponding collimator
  */
{

  ELog::RegMethod RegA("DiagnosticPlug","setStraws");

  Straws = sc;
  Pinhole = 0;

}

void
DiagnosticPlug::setScintillators(ScintillatorBlock *sb)
  /*!
    Load the scintillator block class to build the corresponding detector
  */
{

  ELog::RegMethod RegA("DiagnosticPlug","setScintillators");

  Scintillator = sb;

}

void
DiagnosticPlug::createSurfaces()
  /*!
    Create Surfaces for the Be
  */
{
  ELog::RegMethod RegA("DiagnosticPlug","createSurfaces");

  ModelSupport::buildPlane(SMap,tIndex+1,Origin-Y*length/2.0,Y);
  ModelSupport::buildPlane(SMap,tIndex+11,Origin-Y*length/2.0-Y*activationLayerThick1,Y);
  ModelSupport::buildPlane(SMap,tIndex+21,Origin-Y*length/2.0-Y*activationLayerThick2,Y);
  ModelSupport::buildPlane(SMap,tIndex+31,Origin-Y*length/2.0-Y*activationLayerThick3,Y);

  ModelSupport::buildPlane(SMap,tIndex+2,Origin+Y*length/2.0,Y);
  ModelSupport::buildPlane(SMap,tIndex+12,Origin+Y*length/2.0+Y*activationLayerThick1,Y);
  ModelSupport::buildPlane(SMap,tIndex+22,Origin+Y*length/2.0+Y*activationLayerThick2,Y);
  ModelSupport::buildPlane(SMap,tIndex+32,Origin+Y*length/2.0+Y*activationLayerThick3,Y);

  ModelSupport::buildPlane(SMap,tIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,tIndex+13,Origin-X*width/2.0-X*activationLayerThick1,X);
  ModelSupport::buildPlane(SMap,tIndex+23,Origin-X*width/2.0-X*activationLayerThick2,X);
  ModelSupport::buildPlane(SMap,tIndex+33,Origin-X*width/2.0-X*activationLayerThick3,X);

  ModelSupport::buildPlane(SMap,tIndex+4,Origin+X*width/2.0,X);
  ModelSupport::buildPlane(SMap,tIndex+14,Origin+X*width/2.0+X*activationLayerThick1,X);
  ModelSupport::buildPlane(SMap,tIndex+24,Origin+X*width/2.0+X*activationLayerThick2,X);
  ModelSupport::buildPlane(SMap,tIndex+34,Origin+X*width/2.0+X*activationLayerThick3,X);

  for (size_t i = 0; i < activationZPlanes.size(); i++) {

        ModelSupport::buildPlane(SMap,tIndex+(10*(const int)i+5),Z*activationZPlanes.at(i),Z);

  }

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

  std::string top,bottom;

  for (size_t i = 0; i < (activationZPlanes.size()+1); i++) {

        if (i == 0) {

                bottom = strFloor;
                top = ModelSupport::getComposite(SMap,tIndex+10*(const int)i," -5");

        } else if (i == (activationZPlanes.size())) {

                bottom = ModelSupport::getComposite(SMap,tIndex+10*(const int)(i-1)," 5");
                top = strRoof;

        } else {

                bottom = ModelSupport::getComposite(SMap,tIndex+10*(const int)(i-1)," 5");
                top = ModelSupport::getComposite(SMap,tIndex+10*(const int)(i)," -5");

        }

  Out=ModelSupport::getComposite(SMap,tIndex," 31 -32 33 -34 (-21:22:-23:24)") + bottom + top; 
  System.addCell(MonteCarlo::Qhull(cellIndex++, activationWallsMaterial, 0.0, Out));

  Out=ModelSupport::getComposite(SMap,tIndex," 21 -22 23 -24 (-11:12:-13:14)") + bottom + top; 
  System.addCell(MonteCarlo::Qhull(cellIndex++, activationWallsMaterial, 0.0, Out));

  Out=ModelSupport::getComposite(SMap,tIndex," 11 -12 13 -14 (-1:2:-3:4)") + bottom + top;
  System.addCell(MonteCarlo::Qhull(cellIndex++, activationWallsMaterial, 0.0, Out));

  }

  Out=ModelSupport::getComposite(SMap,tIndex," 31 -32 33 -34") + strFloor + strRoof; 
  addOuterSurf(Out);

/*
  Out=ModelSupport::getComposite(SMap,tIndex," 31 -32 33 -34 (-21:22:-23:24)") + strFloor + strRoof; 
  System.addCell(MonteCarlo::Qhull(cellIndex++, 0, 0.0, Out));

  Out=ModelSupport::getComposite(SMap,tIndex," 21 -22 23 -24 (-11:12:-13:14)") + strFloor + strRoof; 
  System.addCell(MonteCarlo::Qhull(cellIndex++, 0, 0.0, Out));

  Out=ModelSupport::getComposite(SMap,tIndex," 11 -12 13 -14 (-1:2:-3:4)") + strFloor + strRoof; 
  System.addCell(MonteCarlo::Qhull(cellIndex++, 0, 0.0, Out));
*/

  Out=ModelSupport::getComposite(SMap,tIndex," 1 -2 3 -4") + strFloor + strRoof; 
  System.addCell(MonteCarlo::Qhull(cellIndex++, 0, 0.0, Out));
  setCell("main",cellIndex-1);

  //addOuterSurf(Out);

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

  if (Pinhole) {

     Pinhole->createAll(System, *this, floorFC, floorLP, roofFC, roofLP);

  } else if (Straws) {

     Straws->createAll(System, *this, floorFC, floorLP, roofFC, roofLP);
     Scintillator->createAll(System, *this, *Straws, 0, roofFC, roofLP);

  }

  insertObjects(System);       

  return;
}

  
}  // NAMESPACE instrumentSystem
