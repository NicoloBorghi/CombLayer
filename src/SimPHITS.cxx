/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/SimPHITS.cxx
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
#include <cmath>
#include <complex> 
#include <vector>
#include <list>
#include <map> 
#include <set>
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
#include <memory>
#include <array>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "mathSupport.h"
#include "support.h"
#include "version.h"
#include "Element.h"
#include "MapSupport.h"
#include "MXcards.h"
#include "Material.h"
#include "DBMaterial.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Triple.h"
#include "NList.h"
#include "NRange.h"
#include "Tally.h"
#include "cellFluxTally.h"
#include "pointTally.h"
#include "heatTally.h"
#include "tallyFactory.h"
#include "Transform.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "Plane.h"
#include "ArbPoly.h"
#include "Cylinder.h"
#include "Cone.h"
#include "MBrect.h"
#include "NullSurface.h"
#include "Sphere.h"
#include "Torus.h"
#include "General.h"
#include "surfaceFactory.h"
#include "surfProg.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FItem.h"
#include "FuncDataBase.h"
#include "SurInter.h"
#include "Debug.h"
#include "BnId.h"
#include "Acomp.h"
#include "Algebra.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "weightManager.h"
#include "ModeCard.h"
#include "LSwitchCard.h"
#include "PhysCard.h"
#include "PhysImp.h"
#include "SrcData.h"
#include "SrcItem.h"
#include "Source.h"
#include "KCode.h"
#include "PhysicsCards.h"
#include "Simulation.h"
#include "SimPHITS.h"


SimPHITS::SimPHITS() : Simulation()
  /*!
    Constructor
  */
{}


SimPHITS::SimPHITS(const SimPHITS& A) : Simulation(A)
 /*! 
   Copy constructor
   \param A :: Simulation to copy
 */
{}

SimPHITS&
SimPHITS::operator=(const SimPHITS& A)
  /*!
    Assignment operator
    \param A :: SimPHITS to copy
    \return *this
   */
{
  if (this!=&A)
    {
      Simulation::operator=(A);
    }
  return *this;
}


void
SimPHITS::writeTally(std::ostream& OX) const
  /*!
    Writes out the tallies using a nice boost binding
    construction.
    \param OX :: Output stream
   */
{
  OX<<"c -----------------------------------------------------------"<<std::endl;
  OX<<"c ------------------- TALLY CARDS ---------------------------"<<std::endl;
  OX<<"c -----------------------------------------------------------"<<std::endl;
  // The totally insane line below does the following
  // It iterats over the Titems and since they are a map
  // uses the mathSupport:::PSecond
  // _1 refers back to the TItem pair<int,tally*>
  for(const TallyTYPE::value_type& TI : TItem)
    TI.second->write(OX);

  return;
}

void
SimPHITS::writeTransform(std::ostream& OX) const
  /*!
    Write all the transforms in standard MCNPX output 
    type [These should now not be used].
    \param OX :: Output stream
  */

{
  OX<<"[transform]"<<std::endl;

  TransTYPE::const_iterator vt;
  for(vt=TList.begin();vt!=TList.end();vt++)
    {
      vt->second.write(OX);
    }
  return;
}


void
SimPHITS::writeCells(std::ostream& OX) const
  /*!
    Write all the cells in standard MCNPX output 
    type.
    \param OX :: Output stream
  */
{
  boost::format FmtStr("  %1$d%|20t|%2$d\n");
  OX<<"[cell]"<<std::endl;
  OTYPE::const_iterator mp;
  for(mp=OList.begin();mp!=OList.end();mp++)
    mp->second->writePHITS(OX);

  OX<<std::endl;  // Empty line manditory for MCNPX

  OX<<"[temperature]"<<std::endl;
  for(mp=OList.begin();mp!=OList.end();mp++)
    {
      const double T=mp->second->getTemp();
      if (fabs(T-300.0)>1.0)
	{
	  OX<<FmtStr % mp->second->getName() % (T*8.6173422e11);
	}
    }

  return;
}

void
SimPHITS::writeSurfaces(std::ostream& OX) const
  /*!
    Write all the surfaces in standard MCNPX output 
    type.
    \param OX :: Output stream
  */

{
  OX<<"  [surface] " <<std::endl;

  const ModelSupport::surfIndex::STYPE& SurMap=
    ModelSupport::surfIndex::Instance().surMap();
  std::map<int,Geometry::Surface*>::const_iterator mp;
  for(mp=SurMap.begin();mp!=SurMap.end();mp++)
    {
      (mp->second)->write(OX);
    }
  return;
} 

void
SimPHITS::writeMaterial(std::ostream& OX) const
  /*!
    Write all the used Materials in standard MCNPX output 
    type.
    \param OX :: Output stream
  */
{
  OX<<"    [material]"<<std::endl;
  ModelSupport::DBMaterial::Instance().writeMCNPX(OX);
  return;
}


void
SimPHITS::writeWeights(std::ostream& OX) const
  /*!
    Write all the used Weight in standard MCNPX output 
    type.
    \param OX :: Output stream
  */

{
  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();
  
  OX<<"[weight]"<<std::endl;
  WM.write(OX);
  return;
}


void
SimPHITS::writePhysics(std::ostream& OX) const
  /*!
    Write all the used Weight in standard MCNPX output 
    type. Note that it also has to add the rdum cards
    to the physics
    \param OX :: Output stream
  */

{  
  ELog::RegMethod RegA("SimPHITS","writePhysics");
  // Processing for point tallies
  std::map<int,tallySystem::Tally*>::const_iterator mc;
  std::vector<int> Idum;
  std::vector<Geometry::Vec3D> Rdum;
  for(mc=TItem.begin();mc!=TItem.end();mc++)
    {
      const tallySystem::pointTally* Ptr=
	dynamic_cast<const tallySystem::pointTally*>(mc->second);
      if(Ptr && Ptr->hasRdum())
        {
	  Idum.push_back(Ptr->getKey());
	  for(size_t i=0;i<4;i++)
	    Rdum.push_back(Ptr->getWindowPt(i));
	}
    }
  if (!Idum.empty())
    {
      OX<<"idum "<<Idum.size()<<" ";
      copy(Idum.begin(),Idum.end(),std::ostream_iterator<int>(OX," "));
      OX<<std::endl;
      OX<<"rdum       "<<Rdum.front()<<std::endl;
      std::vector<Geometry::Vec3D>::const_iterator vc;
      for(vc=Rdum.begin()+1;vc!=Rdum.end();vc++)
	OX<<"           "<< *vc<<std::endl;
    }

  // Remaining Physics cards
  PhysPtr->write(OX,cellOutOrder,voidCells);
  OX<<"c ++++++++++++++++++++++ END ++++++++++++++++++++++++++++"<<std::endl;
  OX<<std::endl;  // MCNPX requires a blank line to terminate
  return;
}

void
SimPHITS::write(const std::string& Fname) const
  /*!
    Write out all the system (in PHITS output format)
    \param Fname :: Output file 
  */
{
  std::ofstream OX(Fname.c_str());
  Simulation::writeVariables(OX);
  writeCells(OX);
  writeSurfaces(OX);
  writeMaterial(OX);
  writeTransform(OX);
  writeWeights(OX);
  writeTally(OX);
  writePhysics(OX);
  OX.close();
  return;
}
