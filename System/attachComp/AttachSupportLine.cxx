/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/AttachSupport.cxx
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
#include <deque>
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "support.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "surfRegister.h"
#include "objectRegister.h"

#include "varList.h"
#include "Code.h"

#include "FuncDataBase.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "objectRegister.h"
#include "Qhull.h"
#include "Simulation.h"
#include "SurInter.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "AttachSupport.h"

#include "Debug.h"
#include "debugMethod.h"


namespace attachSystem
{

bool
checkLineIntersect(const FixedComp& InsertFC,
                   const MonteCarlo::Object& CellObj)
  /*!
    Calculate if a fixed component is within a cell object      
    \param InsertFC :: FixedComp to get intersect 
    \param CellObj :: Cell Object
   */
{
  ELog::RegMethod RegA("AttachSupportLine[F]","checkLineInsert");
  
  
  const size_t NPoint=InsertFC.NConnect();
  for(size_t j=0;j<NPoint;j++)
    {
      const Geometry::Vec3D& IP=InsertFC.getLinkPt(j);
      if (CellObj.isValid(IP))
        return 1;
    }


  // Check line intersection:
  const std::vector<const Geometry::Surface*>& SurList=
    CellObj.getSurList();

  for(size_t j=0;j<NPoint;j++)
    {
      const Geometry::Vec3D& IP=InsertFC.getLinkPt(j);
      for(size_t k=j+1;k<NPoint;k++)
        {
          Geometry::Vec3D UV=InsertFC.getLinkPt(k)-IP;
          const double LLen=UV.makeUnit();
          if (LLen>Geometry::zeroTol)
            {
              MonteCarlo::LineIntersectVisit LI(IP,UV);
              std::vector<const Geometry::Surface*>::const_iterator vc;
              for(vc=SurList.begin();vc!=SurList.end();vc++)
                (*vc)->acceptVisitor(LI);
              
              const std::vector<double>& distVec(LI.getDistance());
              const std::vector<Geometry::Vec3D>& dPts(LI.getPoints());
              const std::vector<const Geometry::Surface*>& 
                surfPts=LI.getSurfIndex();
              
              for(size_t dI=0;dI<dPts.size();dI++)
                {
                  if ((distVec[dI]>0.0 && distVec[dI]<LLen) &&
                      CellObj.isValid(dPts[dI],surfPts[dI]->getName()))
                    return 1;
                }
	    }
        }
    }
  return 0;
}

  
void
addToInsertLineCtrl(Simulation& System,
		    const attachSystem::FixedComp& OuterFC,
		    const attachSystem::FixedComp& InsertFC)
  /*!
    Adds this object to the containedComp to be inserted.
    CC is the fixed object that is to be inserted -- linkpoints
    must be set. 

    It is tested by 
      -- A link point is within the object
      -- A line between link points intersect the surface of CellObject
      
    It is tested by tracking lines from each link point
    to another link point and seeing if they intersect the 
    surfaces within the BaseFC object. Then the validity of
    the point is tested REGARDLESS of being in the CC, to 
    being in the BaseFC. If it is an insert is made
    \param System :: Simulation to use
    \param OuterFC :: FixedComp for name
    \param InsertFC :: FixedComp with a ContainedComp/containedGroup
    dynamics cast
  */
{
  ELog::RegMethod RegA("AttachSupport[F]","addtoInsectLineCtrl(FC,FC)");

  const attachSystem::ContainedComp* CCPtr=
    dynamic_cast<const attachSystem::ContainedComp*>(&InsertFC);
  if (!CCPtr)
    {
      ELog::EM<<InsertFC.getKeyName()<<" does not have CC component"
	      <<ELog::endErr;
      return;
    }
  addToInsertLineCtrl(System,OuterFC,InsertFC,*CCPtr);
  return;
}

void
addToInsertLineCtrl(Simulation& System,
                    const attachSystem::CellMap& BaseCell,
		    const std::string& cellName,
		    const attachSystem::FixedComp& InsertFC,
                    const attachSystem::ContainedComp& CC)
  /*!
    Adds this object to the containedComp to be inserted.
    CC is the fixed object that is to be inserted -- linkpoints
    must be set. It is tested by tracking lines from each link point
    to another link point and seeing if they intersect the 
    surfaces within the BaseFC object. Then the validity of
    the point is tested REGARDLESS of being in the CC, to 
    being in the BaseFC. If it is an insert is made
    \param System :: Simulation to use
    \param OuterFC :: FixedComp for name
    \param InsertFC :: FixedComp with a ContainedComp/containedGroup
    dynamics cast
  */
{
  ELog::RegMethod RegA("AttachSupport[F]","addtoInsectLineCtrl(FC,FC)");

  for(const int cn : BaseCell.getCells(cellName))
    {
      addToInsertLineCtrl(System,InsertFC,CC,cn);
    }
  return;
}

void
addToInsertLineCtrl(Simulation& System,
		    const attachSystem::FixedComp& OuterFC,
		    const attachSystem::FixedComp& InsertFC,
                    const attachSystem::ContainedComp& CC)
  /*!
    Adds this object to the containedComp to be inserted.
    CC is the fixed object that is to be inserted -- linkpoints
    must be set. It is tested by tracking lines from each link point
    to another link point and seeing if they intersect the 
    surfaces within the BaseFC object. Then the validity of
    the point is tested REGARDLESS of being in the CC, to 
    being in the BaseFC. If it is an insert is made
    \param System :: Simulation to use
    \param OuterFC :: FixedComp to get name for cells
    \param InsertFC :: FixedComp 
    dynamics cast
  */
{
  ELog::RegMethod RegA("AttachSupport[F]","addtoInsectLineCtrl(FC,FC)");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  // Determin cells to scan
  const int cellN=OR.getCell(OuterFC.getKeyName());
  const int cellL=OR.getLast(OuterFC.getKeyName());

  for(int i=cellN+1;i<=cellL;i++)
    addToInsertLineCtrl(System,InsertFC,CC,i);

  return;
}

void
addToInsertLineCtrl(Simulation& System,
		    const attachSystem::FixedComp& InsertFC,
                    const attachSystem::ContainedComp& CC,
                    const int cellN)
  /*!
    Adds this object to the containedComp to be inserted.

    CC is the ContainedComp that is to be inserted.
    It is tested by 
      -- A link point is within the object
      -- A line between link points intersect the surface of CellObject

    \param System :: Simulation to use
    \param InsertFC :: FixedComp to get linkpoint (inserting object)
    \param CC :: Contained comp of insertion (often InsertFC by cast)
    \param cellN :: Cell number of object to test
  */
{
  ELog::RegMethod RegA("AttachSupport[F]","addtoInsectLineCtrl(FC,FC)");

  MonteCarlo::Qhull* CRPtr=System.findQhull(cellN);
  if (CRPtr && checkLineIntersect(InsertFC,*CRPtr))
    {
      const std::string excludeStr=CC.getExclude();
      CRPtr->addSurfString(excludeStr);
    }
  return;
}
  
}  // NAMESPACE attachSystem
