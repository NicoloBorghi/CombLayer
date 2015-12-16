/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   pipeBuild/pipeTube.cxx
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
#include "surfDIter.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
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
#include "LayerComp.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "surfDBase.h"
#include "mergeTemplate.h"
#include "simplebox_comblayer1Tube.h"

namespace simplebox_comblayer1System
{

simplebox_comblayer1Tube::simplebox_comblayer1Tube(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedComp(Key,6),
  tubeIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(tubeIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

simplebox_comblayer1Tube::simplebox_comblayer1Tube(const simplebox_comblayer1Tube& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedComp(A),attachSystem::CellMap(A),
  tubeIndex(A.tubeIndex),cellIndex(A.cellIndex),
  xStep(A.xStep),yStep(A.yStep),zStep(A.zStep),
  xyAngle(A.xyAngle),zAngle(A.zAngle),length(A.length),
  height(A.height),width(A.width),innerHeight(A.innerHeight),
  innerWidth(A.innerWidth),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: simplebox_comblayer1Tube to copy
  */
{}

simplebox_comblayer1Tube&
simplebox_comblayer1Tube::operator=(const simplebox_comblayer1Tube& A)
  /*!
    Assignment operator
    \param A :: simplebox_comblayer1Tube to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      length=A.length;
      height=A.height;
      width=A.width;
      innerHeight=A.innerHeight;
      innerWidth=A.innerWidth;
      wallMat=A.wallMat;
    }
  return *this;
}

simplebox_comblayer1Tube::~simplebox_comblayer1Tube()
  /*!
    Destructor
  */
{}
  

void
simplebox_comblayer1Tube::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("simplebox_comblayer1Tube","populate");

  xStep=Control.EvalDefVar<double>(keyName+"XStep",0.0);
  yStep=Control.EvalDefVar<double>(keyName+"YStep",0.0);
  zStep=Control.EvalDefVar<double>(keyName+"ZStep",0.0);
  xyAngle=Control.EvalDefVar<double>(keyName+"XYangle",0.0);
  zAngle=Control.EvalDefVar<double>(keyName+"Zangle",0.0);
  
  length=Control.EvalVar<double>(keyName+"Length");

  height=Control.EvalVar<double>(keyName+"Height");
  width=Control.EvalVar<double>(keyName+"Width");
  //innerHeight=Control.EvalVar<double>(keyName+"InnerHeight");
  //innerWidth=Control.EvalVar<double>(keyName+"InnerWidth");

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  nLayers=Control.EvalVar<size_t>(keyName+"nLayers");
  ModelSupport::populateDivideLen(Control,nLayers,keyName+"Len",
				  length,lenFrac);
 
    //length=5.0;


  return;
}

void
simplebox_comblayer1Tube::createUnitVector(const attachSystem::FixedComp& FC,const long sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
  */
{
  ELog::RegMethod RegA("simplebox_comblayer1Tube","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);

  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);

  return;
}

void
simplebox_comblayer1Tube::createSurfaces(const long sideIndex,const attachSystem::FixedComp& FC)
  /*!
    Create planes for system
  */
{
  ELog::RegMethod RegA("simplebox_comblayer1Tube","createSurfaces");

   //+++++++++++++++++
  //Add Rotation
  //++++++++++++++++++++
  
  // Geometry::Vec3D axis(Y);  //I created a copy of the Y axis so I did not screw the Y axis

  //const Geometry::Quaternion Qz= Geometry::Quaternion::calcQRotDeg(37.5,Z);
  //Qz.rotate(axis);

  ModelSupport::buildPlane(SMap,tubeIndex+2,Origin+Y*(length/2.0),Y);  
  ModelSupport::buildPlane(SMap,tubeIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,tubeIndex+4,Origin+X*(width/2.0),X);  
  ModelSupport::buildPlane(SMap,tubeIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,tubeIndex+6,Origin+Z*(height/2.0),Z);


  if (sideIndex!=0)
    SMap.addMatch(tubeIndex+1,FC.getSignedLinkSurf(sideIndex));
  else
    ModelSupport::buildPlane(SMap,tubeIndex+1,Origin-Y*(length/2.0),Y);
  


    return; 
}

void
simplebox_comblayer1Tube::createObjects(Simulation& System,const long sideIndex,const attachSystem::FixedComp& FC)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("simplebox_comblayer1Tube","createObjects");

  std::string Out;

  // Inner 
  //  Out=ModelSupport::getComposite(SMap,tubeIndex," 1 -2 3 -4 5 -6 ");
  if (sideIndex==0)
    {
      Out=ModelSupport::getComposite(SMap,tubeIndex," 1 -2 3 -4 5 -6 ");
    }
  else  {
    Out=ModelSupport::getComposite(SMap,tubeIndex," -2 3 -4 5 -6 "); //remove oe of the surphace
    Out+=FC.getSignedLinkString(sideIndex);
  }


  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  ELog::EM<<"Cell == "<<cellIndex-1<<ELog::endDiag;
  // CellMap::setCell("Inner",cellIndex-1);
  //Out=ModelSupport::getComposite(SMap,tubeIndex,
  //				 " 1 -2 3 -4 5 -6 (-13:14:-15:16) ");
  
  //System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  //CellMap::setCell("Outer",cellIndex-1);
  
  //Out=ModelSupport::getComposite(SMap,tubeIndex," 1 -2 3 -4 5 -6 ");
  addOuterSurf(Out);
  return; 
}

void
simplebox_comblayer1Tube::createLinks()
  /*!
    Creates a full attachment set
    First two are in the -/+Y direction and have a divider
    Last two are in the -/+X direction and have a divider
    The mid two are -/+Z direction
  */
{  
  ELog::RegMethod RegA("simplebox_comblayer1Tube","createLinks");

  FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(tubeIndex+1));

  FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(tubeIndex+2));
  
  FixedComp::setConnect(2,Origin-X*(length/2.0),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(tubeIndex+3));
  
  FixedComp::setConnect(3,Origin+X*(width/2.0),X);
  FixedComp::setLinkSurf(3,-SMap.realSurf(tubeIndex+4));
  
  FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(tubeIndex+5));
  
  FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);
  FixedComp::setLinkSurf(5,-SMap.realSurf(tubeIndex+6));

  return;
}

void 
simplebox_comblayer1Tube::layerProcess(Simulation& System)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    \param System :: Simulation to work on
  */
{
  ELog::RegMethod RegA("simplebox_comblayer1Tube","layerProcess");
  // Steel layers
  if (nLayers>1)
    {
      std::string OutA,OutB;
      ModelSupport::surfDivide DA;
            
      for(size_t i=1;i<nLayers;i++)
	{
	  DA.addFrac(lenFrac[i-1]);
	  DA.addMaterial(wallMat);
	}
      DA.addMaterial(wallMat);


      // Cell Specific:
      DA.setCellN(cellIndex-1);
      ELog::EM<<"Cell == "<<cellIndex-1<<ELog::endDiag;
      DA.setOutNum(cellIndex,tubeIndex+1001);

      ModelSupport::mergeTemplate<Geometry::Plane,
				  Geometry::Plane> surroundRule;

      surroundRule.setSurfPair(SMap.realSurf(tubeIndex+1),
			       SMap.realSurf(tubeIndex+2));
  
      OutA=ModelSupport::getComposite(SMap,tubeIndex," 1 ");
      OutB=ModelSupport::getComposite(SMap,tubeIndex," -2 ");

      surroundRule.setInnerRule(OutA);
      surroundRule.setOuterRule(OutB);
      DA.addRule(&surroundRule);
      DA.activeDivideTemplate(System);

      cellIndex=DA.getCellNum();
    }
  
  return;
}


  
void
simplebox_comblayer1Tube::createAll(Simulation& System,
				    const attachSystem::FixedComp& FC,const long sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: Attachment point
   */
{
  ELog::RegMethod RegA("simplebox_comblayer1Tube","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  createSurfaces(sideIndex,FC);
  createObjects(System,sideIndex,FC);
  createLinks();
  insertObjects(System);
  layerProcess(System);
  return;
}

}  // NAMESPACE simplebox_comblayer1System
