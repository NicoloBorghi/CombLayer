/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tungstenBuild/tungstenGammaCell.cxx
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
#include "FixedOffset.h"
#include "LayerComp.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "surfDBase.h"
#include "mergeTemplate.h"
#include "tungstenGammaCell.h"

namespace tungstenSystem
{

tungstenGammaCell::tungstenGammaCell(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6),
  gammaCellIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(gammaCellIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

tungstenGammaCell::tungstenGammaCell(const tungstenGammaCell& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),attachSystem::CellMap(A),
  gammaCellIndex(A.gammaCellIndex),cellIndex(A.cellIndex),
  length(A.length),height(A.height),width(A.width),
  innerHeight(A.innerHeight),innerWidth(A.innerWidth),
  wallMat(A.wallMat),nWallLayers(A.nWallLayers),
  wallFracList(A.wallFracList),wallMatList(A.wallMatList)
  /*!
    Copy constructor
    \param A :: tungstenGammaCell to copy
  */
{}

tungstenGammaCell&
tungstenGammaCell::operator=(const tungstenGammaCell& A)
  /*!
    Assignment operator
    \param A :: tungstenGammaCell to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      length=A.length;
      height=A.height;
      width=A.width;
      innerHeight=A.innerHeight;
      innerWidth=A.innerWidth;
      wallMat=A.wallMat;
      nWallLayers=A.nWallLayers;
      wallFracList=A.wallFracList;
      wallMatList=A.wallMatList;
    }
  return *this;
}

tungstenGammaCell::~tungstenGammaCell()
  /*!
    Destructor
  */
{}
  

void
tungstenGammaCell::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("tungstenGammaCell","populate");

  FixedOffset::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  height=Control.EvalVar<double>(keyName+"Height");
  width=Control.EvalVar<double>(keyName+"Width");
  innerHeight=Control.EvalVar<double>(keyName+"InnerHeight");
  innerWidth=Control.EvalVar<double>(keyName+"InnerWidth");

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  nWallLayers=Control.EvalVar<size_t>(keyName+"NWallLayers");
  ModelSupport::populateDivide(Control,nWallLayers,
			       keyName+"WLayerMat",
			       wallMat,wallMatList);
  ModelSupport::populateDivideLen(Control,nWallLayers,
				  keyName+"WLayerThick",
				  (height-innerHeight)/2.0,
				  wallFracList);

  return;
}

void
tungstenGammaCell::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("tungstenGammaCell","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  yStep+=length/2.0;
  FixedOffset::applyOffset();

  return;
}

void
tungstenGammaCell::createSurfaces()
  /*!
    Create planes for system
  */
{
  ELog::RegMethod RegA("tungstenGammaCell","createSurfaces");

  ModelSupport::buildPlane(SMap,gammaCellIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,gammaCellIndex+2,Origin+Y*(length/2.0),Y);  
  ModelSupport::buildPlane(SMap,gammaCellIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,gammaCellIndex+4,Origin+X*(width/2.0),X);  
  ModelSupport::buildPlane(SMap,gammaCellIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,gammaCellIndex+6,Origin+Z*(height/2.0),Z);  

  ModelSupport::buildPlane(SMap,gammaCellIndex+13,Origin-X*(innerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,gammaCellIndex+14,Origin+X*(innerWidth/2.0),X);  
  ModelSupport::buildPlane(SMap,gammaCellIndex+15,Origin-Z*(innerHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,gammaCellIndex+16,Origin+Z*(innerHeight/2.0),Z);  

  return; 
}

void
tungstenGammaCell::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("tungstenGammaCell","createObjects");

  std::string Out;

  // Inner 
  Out=ModelSupport::getComposite(SMap,gammaCellIndex," 1 -2 13 -14 15 -16 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  CellMap::setCell("Inner",cellIndex-1);
  Out=ModelSupport::getComposite(SMap,gammaCellIndex,
				 " 1 -2 3 -4 5 -6 (-13:14:-15:16) ");
  
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  CellMap::setCell("Outer",cellIndex-1);
  
  Out=ModelSupport::getComposite(SMap,gammaCellIndex," 1 -2 3 -4 5 -6 ");
  addOuterSurf(Out);
  return; 
}

void
tungstenGammaCell::createLinks()
  /*!
    Creates a full attachment set
    First two are in the -/+Y direction and have a divider
    Last two are in the -/+X direction and have a divider
    The mid two are -/+Z direction
  */
{  
  ELog::RegMethod RegA("tungstenGammaCell","createLinks");

  FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(gammaCellIndex+1));

  FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(gammaCellIndex+2));
  
  FixedComp::setConnect(2,Origin-X*(length/2.0),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(gammaCellIndex+3));
  
  FixedComp::setConnect(3,Origin+X*(width/2.0),X);
  FixedComp::setLinkSurf(3,-SMap.realSurf(gammaCellIndex+4));
  
  FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(gammaCellIndex+5));
  
  FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);
  FixedComp::setLinkSurf(5,-SMap.realSurf(gammaCellIndex+6));

  return;
}

void 
tungstenGammaCell::layerProcess(Simulation& System)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    \param System :: Simulation to work on
  */
{
  ELog::RegMethod RegA("tungstenGammaCell","layerProcess");
  // Steel layers
  //  layerSpecial(System);

  if (nWallLayers>1)
    {
      std::string OutA,OutB;
      ModelSupport::surfDivide DA;
            
      for(size_t i=1;i<nWallLayers;i++)
	{
	  DA.addFrac(wallFracList[i-1]);
	  DA.addMaterial(wallMatList[i-1]);
	}
      DA.addMaterial(wallMatList.back());
      // Cell Specific:
      DA.setCellN(CellMap::getCell("Outer"));
      DA.setOutNum(cellIndex,gammaCellIndex+1001);

      ModelSupport::mergeTemplate<Geometry::Plane,
				  Geometry::Plane> surroundRule;

      surroundRule.setSurfPair(SMap.realSurf(gammaCellIndex+13),
			       SMap.realSurf(gammaCellIndex+3));
      surroundRule.setSurfPair(SMap.realSurf(gammaCellIndex+14),
			       SMap.realSurf(gammaCellIndex+4));
      surroundRule.setSurfPair(SMap.realSurf(gammaCellIndex+15),
			       SMap.realSurf(gammaCellIndex+5));
      surroundRule.setSurfPair(SMap.realSurf(gammaCellIndex+16),
			       SMap.realSurf(gammaCellIndex+6));
      OutA=ModelSupport::getComposite(SMap,gammaCellIndex," (-13:14:-15:16) ");
      OutB=ModelSupport::getComposite(SMap,gammaCellIndex," 3 -4 5 -6 ");

      surroundRule.setInnerRule(OutA);
      surroundRule.setOuterRule(OutB);
      DA.addRule(&surroundRule);
      DA.activeDivideTemplate(System);

      cellIndex=DA.getCellNum();
    }
  return;
}


  
void
tungstenGammaCell::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: Attachment point
    \param sideIndex :: sideIndex for link point
   */
{
  ELog::RegMethod RegA("tungstenGammaCell","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  layerProcess(System);
  return;
}

}  // NAMESPACE tungstenSystem
