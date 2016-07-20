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
#include "BaseMap.h"
#include "CellMap.h"
#include "DiagnosticPlug.h"
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
  width(A.width),length(A.length),shaftWallThick(A.shaftWallThick),
  shaftBearingRadius(A.shaftBearingRadius),shaftBearingHeight(A.shaftBearingHeight),shaftBearingWallThick(A.shaftBearingWallThick),
  plugFrameRadius(A.plugFrameRadius),plugFrameHeight(A.plugFrameHeight),
  plugFrameDepth(A.plugFrameDepth),plugFrameAngle(A.plugFrameAngle),
  plugFrameWallThick(A.plugFrameWallThick),
  plugFrameMat(A.plugFrameMat),plugFrameWallMat(A.plugFrameWallMat),
  shaftMat(A.shaftMat),shaftWallMat(A.shaftWallMat)
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
      shaftWallThick=A.shaftWallThick;
      shaftBearingRadius=A.shaftBearingRadius;
      shaftBearingHeight=A.shaftBearingHeight;
      shaftBearingWallThick=A.shaftBearingWallThick;

      plugFrameRadius=A.plugFrameRadius;
      plugFrameHeight=A.plugFrameHeight;
      plugFrameDepth=A.plugFrameDepth;
      plugFrameAngle=A.plugFrameAngle;
      plugFrameWallThick=A.plugFrameWallThick;

      plugFrameMat=A.plugFrameMat;
      plugFrameWallMat=A.plugFrameWallMat;
      shaftMat=A.shaftMat;
      shaftWallMat=A.shaftWallMat;
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

  engActive=Control.EvalPair<int>(keyName,"","EngineeringActive");

    // Master values
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYangle");
  zAngle=Control.EvalVar<double>(keyName+"Zangle");
  
  width=Control.EvalVar<double>(keyName+"Width");
  length=Control.EvalVar<double>(keyName+"Length");

  /////////////////////////////////////////////////////////////////////////////

  PHType = "rect"; // Control.EvalVar<double>(keyName+"PHType"); // For now we only build a rectangular pinhole

  zImagingPlane = Control.EvalVar<double>(keyName+"ZImagingPlane") + 5.80;

  /////////////////////////////////////////////////////////////////////////////

  radialPHOffset = -1.0*Control.EvalVar<double>(keyName+"RadialPHOffset");

  if (fabs(radialPHOffset) >= (length/2.0)) {

    ELog::EM << "Radial Offset out of range. Setting to 0" << ELog::endDiag;
    radialPHOffset = 0.0;

  }

  transversalPHOffset = -1.0*Control.EvalVar<double>(keyName+"TransversalPHOffset");

  if (fabs(transversalPHOffset) >= (width/2.0)) {

    ELog::EM << "Transversal Offset out of range. Setting to 0" << ELog::endDiag;
    transversalPHOffset = 0.0;

  }

  radialPHPos      = radialPHOffset*sin(90.0 + xyAngle) + transversalPHOffset*cos(90.0 + xyAngle);
  transversalPHPos = radialPHOffset*cos(90.0 + xyAngle) + transversalPHOffset*sin(90.0 + xyAngle);

  radialPHWidth = Control.EvalVar<double>(keyName+"RadialPHWidth");
  transversalPHWidth = Control.EvalVar<double>(keyName+"TransversalPHWidth");

  /////////////////////////////////////////////////////////////////////////////

  const Geometry::Plane *floorPlane = SMap.realPtr<Geometry::Plane>(DPfloorLinkNumber);

  height = zImagingPlane - floorPlane->getDistance();

  zDistanceFromImage = Control.EvalVar<double>(keyName+"ZDistanceFromImage");

  if (fabs(zDistanceFromImage) > height) {

    ELog::EM << "Pinhole distance from imaging plane out of range. Setting to half height" << ELog::endDiag;
    zDistanceFromImage = zImagingPlane/2.0;

  }

  zPHPos = zImagingPlane - zDistanceFromImage;

  ELog::EM << "X:" << transversalPHPos << " - Y: " << radialPHPos << " - Z: " << zPHPos << ELog::endDiag;
  ELog::EM << "xStep:" << xStep << " - yStep: " << yStep << " - zStep: " << zStep << ELog::endDiag;

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

  // Imaging plane
  ModelSupport::buildPlane(SMap,tIndex+5,Origin+Z*zImagingPlane,Z);

  // Get three points for the first PH collimator face

  const Geometry::Plane *pl1 = SMap.realPtr<Geometry::Plane>(tIndex+1);
  const Geometry::Plane *pl2 = SMap.realPtr<Geometry::Plane>(tIndex+3);
  const Geometry::Plane *pl3 = SMap.realPtr<Geometry::Plane>(tIndex+5);

  Geometry::Vec3D p1 = SurInter::getPoint(pl1,pl2,pl3);

  ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

  pl1 = SMap.realPtr<Geometry::Plane>(tIndex+1);
  pl2 = SMap.realPtr<Geometry::Plane>(tIndex+4);
  pl3 = SMap.realPtr<Geometry::Plane>(tIndex+5);

  Geometry::Vec3D p2 = SurInter::getPoint(pl1,pl2,pl3);

  ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

  Geometry::Vec3D p3( xStep + transversalPHPos + fabs((radialPHWidth/2.0)*cos(90.0 + xyAngle)),
                      yStep + radialPHPos      - fabs((radialPHWidth/2.0)*sin(90.0 + xyAngle)),
                      zStep + zPHPos);

  ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

  Geometry::Vec3D norm(-sin(xyAngle),cos(xyAngle),cos(atan(zDistanceFromImage/((length-radialPHWidth)/2.0))));

  ModelSupport::buildPlane(SMap,tIndex+7,p1,p2,p3,norm);
  
  ////////////////////////////////////////////////////

  // Get three points for the second PH collimator face

  pl1 = SMap.realPtr<Geometry::Plane>(tIndex+1);
  pl2 = SMap.realPtr<Geometry::Plane>(tIndex+3);
  pl3 = SMap.realPtr<Geometry::Plane>(DPfloorLinkNumber);

  p1 = SurInter::getPoint(pl1,pl2,pl3);

  ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

  pl1 = SMap.realPtr<Geometry::Plane>(tIndex+1);
  pl2 = SMap.realPtr<Geometry::Plane>(tIndex+4);
  pl3 = SMap.realPtr<Geometry::Plane>(DPfloorLinkNumber);

  p2 = SurInter::getPoint(pl1,pl2,pl3);

  ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

  p3( xStep + transversalPHPos + fabs((radialPHWidth/2.0)*cos(90.0 + xyAngle)),
      yStep + radialPHPos      - fabs((radialPHWidth/2.0)*sin(90.0 + xyAngle)),
      zStep + zPHPos);

  ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

  norm(-sin(xyAngle),cos(xyAngle),cos(atan((height-zDistanceFromImage)/((length-radialPHWidth)/2.0))));

  ModelSupport::buildPlane(SMap,tIndex+8,p1,p2,p3,norm);
  
  ////////////////////////////////////////////////////

  // Get three points for the third PH collimator face

  pl1 = SMap.realPtr<Geometry::Plane>(tIndex+2);
  pl2 = SMap.realPtr<Geometry::Plane>(tIndex+3);
  pl3 = SMap.realPtr<Geometry::Plane>(tIndex+5);

  p1 = SurInter::getPoint(pl1,pl2,pl3);

  ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

  pl1 = SMap.realPtr<Geometry::Plane>(tIndex+2);
  pl2 = SMap.realPtr<Geometry::Plane>(tIndex+4);
  pl3 = SMap.realPtr<Geometry::Plane>(tIndex+5);

  p2 = SurInter::getPoint(pl1,pl2,pl3);

  ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

  p3( xStep + transversalPHPos - fabs((radialPHWidth/2.0)*cos(90.0 + xyAngle)),
      yStep + radialPHPos      + fabs((radialPHWidth/2.0)*sin(90.0 + xyAngle)),
      zStep + zPHPos);

  ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

  norm(-sin(xyAngle),cos(xyAngle),-cos(atan(zDistanceFromImage/((length+radialPHWidth)/2.0))));

  ModelSupport::buildPlane(SMap,tIndex+17,p1,p2,p3,norm);
  
  ////////////////////////////////////////////////////

  // Get three points for the fourth PH collimator face

  pl1 = SMap.realPtr<Geometry::Plane>(tIndex+2);
  pl2 = SMap.realPtr<Geometry::Plane>(tIndex+3);
  pl3 = SMap.realPtr<Geometry::Plane>(DPfloorLinkNumber);

  p1 = SurInter::getPoint(pl1,pl2,pl3);

  ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

  pl1 = SMap.realPtr<Geometry::Plane>(tIndex+2);
  pl2 = SMap.realPtr<Geometry::Plane>(tIndex+4);
  pl3 = SMap.realPtr<Geometry::Plane>(DPfloorLinkNumber);

  p2 = SurInter::getPoint(pl1,pl2,pl3);

  ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

  p3( xStep + transversalPHPos - fabs((radialPHWidth/2.0)*cos(90.0 + xyAngle)),
      yStep + radialPHPos      + fabs((radialPHWidth/2.0)*sin(90.0 + xyAngle)),
      zStep + zPHPos);

  ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

  norm(-sin(xyAngle),cos(xyAngle),-cos(atan((height-zDistanceFromImage)/((length+radialPHWidth)/2.0))));

  ModelSupport::buildPlane(SMap,tIndex+18,p1,p2,p3,norm);
  
  ////////////////////////////////////////////////////

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

  Out=ModelSupport::getComposite(SMap,tIndex," 1 3 -4 -5 7 8") + strFloor;
  System.addCell(MonteCarlo::Qhull(cellIndex++, 0, 0.0, Out));

  Out=ModelSupport::getComposite(SMap,tIndex," -2 3 -4 -5 -17 -18") + strFloor;
  System.addCell(MonteCarlo::Qhull(cellIndex++, 0, 0.0, Out));

  Out=ModelSupport::getComposite(SMap,tIndex," -2 3 -4 -5 (-7:-8) (17:18)") + strFloor;
  System.addCell(MonteCarlo::Qhull(cellIndex++, 0, 0.0, Out));

  Out=ModelSupport::getComposite(SMap,tIndex," 1 -2 3 -4 5") + strRoof;
  System.addCell(MonteCarlo::Qhull(cellIndex++, 0, 0.0, Out));

  Out=ModelSupport::getComposite(SMap,tIndex," 1 -2 3 -4") + strFloor + strRoof;
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

  DPfloorLinkNumber = floorFC.getLinkSurf(floorLP);
  DProofLinkNumber = roofFC.getLinkSurf(roofLP);

  populate(System.getDataBase());

  createUnitVector(FC);
  createSurfaces();
  createObjects(System, floorFC, floorLP, roofFC, roofLP);
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE instrumentSystem