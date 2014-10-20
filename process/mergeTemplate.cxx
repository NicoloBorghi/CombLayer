/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   process/mergeMulti.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <map> 
#include <list> 
#include <set>
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
#include <memory>
#include <boost/functional.hpp>
#include <boost/bind.hpp>
 
#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "mathSupport.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "Transform.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "surfExpand.h"
#include "surfEqual.h"
#include "Quaternion.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Token.h"
#include "AttachSupport.h"
#include "surfDBase.h"
#include "mergeTemplate.h"

namespace ModelSupport
{

template<typename T,typename U>
mergeTemplate<T,U>::mergeTemplate() : 
  surfDBase(),surfN(0)
  /*!
    Constructor
  */
{}


template<typename T,typename U>
mergeTemplate<T,U>*
mergeTemplate<T,U>::clone() const
  /*!
    Clone copy constructor
    \return new(*this);
  */
{
  return new mergeTemplate<T,U>(*this);
}


template<typename T,typename U>
mergeTemplate<T,U>::~mergeTemplate()
  /*!
    Delete operator
    \return new(*this);
  */
{}

template<typename T,typename U>
void
mergeTemplate<T,U>::clearRules()
  /*!
    Simple helper function to remove old rules
  */
{  
  return;
}

template<typename T,typename U>
void
mergeTemplate<T,U>::addRules() 
  /*!
    Adds the surface object to the rules in a simple way
  */
{
  ELog::RegMethod RegA("mergeTemplate","addRules");
  /*
  clearRules();
  std::vector<Geometry::Surface*>::const_iterator ac;  
  const Geometry::Surface* SPtr(0);
  for(size_t i=0;i<OSPtr.size();i++)
    {
      SPtr = OSPtr[i];
      const int dirSign=secDir[i];
      InRule.addUnion(SPtr->getName());
    }
  OutRule=InRule;
  OutRule.makeComplement();
  */
  return;
}

template<typename T,typename U>
int
mergeTemplate<T,U>::createSurfaces(const double fraction)
  /*!
    Create all the surfaces needed for this template 
    \param fraction :: Weight between the two surface
  */
{
  ELog::RegMethod RegA("mergeTemplate","createSurfaces");

  OSPtr.clear();
  for(size_t i=0;i<pSurf.size();i++)
    OSPtr.push_back
      (surfDBase::createSurf(primSPtr[i],secSPtr[i],fraction,surfN));
  
  return OSPtr.back()->getName();
}
  

template<typename T,typename U>
void 
mergeTemplate<T,U>::setSurfPair(const int pS,const int sS)
  /*!
    Set the primary surface number / inner direction
    \param pS :: Primary surface
    \param sS :: Secondary signed surface
   */
{
  pSurf.push_back(pS);
  sSurf.push_back(sS);
  return;
}

template<typename T,typename U>
void
mergeTemplate<T,U>::populate()
  /*!
    Populates all the surface
  */
{
  ELog::RegMethod RegA("mergeTemplate","populate");
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  
  primSPtr.clear();
  secSPtr.clear();
  for(int PN : pSurf)
    {
      const T* PPtr=dynamic_cast<const T*>
	(SurI.getSurf(abs(PN)));
      if (!PPtr)
	ELog::EM<<"Failed on primary surface "<<PN<<ELog::endErr;
      primSPtr.push_back(PPtr);
    }
  for(int SN : sSurf)
    {
      const U* SPtr=dynamic_cast<const U*>
	(SurI.getSurf(abs(SN)));
      if (!SPtr)
	ELog::EM<<"Failed on secondary surface "<<SN<<ELog::endErr;
      secSPtr.push_back(SPtr);
    }
  return;
}


template<typename T,typename U>
void
mergeTemplate<T,U>::processInnerOuter(const int ,
				      std::vector<Token>& ) const
 /*!
   Process the cells 
   \param outerFlag :: Decide if outer/inner process.
   \param Cell :: Cell to process
 */
{
  ELog::RegMethod RegA("mergeTemplate","processInnerOuter");
  return;
}


template<typename T,typename U>
void
mergeTemplate<T,U>::setInnerRule(const std::string& iStr) 
  /*!
    Process the cells 
    \param iSTr :: Inner Head Rule
   */
{
  ELog::RegMethod RegA("mergeTemplate","setInnerRule");
  if (!InTemplate.procString(iStr))
    throw ColErr::InvalidLine(iStr,"iStr value",0);
  return;
}

template<typename T,typename U>
void
mergeTemplate<T,U>::setOuterRule(const std::string& iStr) 
  /*!
    Process the cells 
    \param iSTr :: Inner Head Rule
   */
{
  ELog::RegMethod RegA("mergeTemplate","setInnerRule");
  if (!OutTemplate.procString(iStr))
    throw ColErr::InvalidLine(iStr,"iStr value",0);
  return;
}

template<typename T,typename U>
void
mergeTemplate<T,U>::process(const double fA,const double fB,
			    HeadRule& HR) 
  /*!
    Process the cells 
    \param HR :: Head Rule
   */
{
  ELog::RegMethod RegA("mergeTemplate","process");

  // transfer old to new and make surfaces
  PSPtr=OSPtr;
  if (fB<1.0)
    createSurfaces(fB);

  // Divide at level 1:
  std::vector<const Rule*> RefItems;
  const size_t NL=HR.countNLevel(0);
  for(size_t i=1;i<=NL;i++)
    RefItems.push_back(HR.findNode(0,i));

  HeadRule Result=HR;
  // Create Inner/Outer Units:

  bool replacedInner(0);
  bool replacedOuter(0);
  for(const Rule* PR : RefItems)
    {
      if (!replacedInner && fA>Geometry::zeroTol &&
	  containInnerRules(PR))
	{
	  HR.addIntersection(makeOuterComp());
	  replacedInner=1;
	}
      else
	HR.addIntersection(PR);

      if (!replacedOuter && (1.0-fB)>Geometry::zeroTol && 
	  containOuterRules(PR))
	{
	  HR.addIntersection(makeOuter());
	  replacedOuter=1;
	}
      else
	HR.addIntersection(PR);
    }

  return;
}



template<typename T,typename U>
bool
mergeTemplate<T,U>::containInnerRules(const Rule* PR) const
  /*!
    Determine in the PR rule is a containing rule for the
    inner system
    \param PR :: Rule to test 
  */
{
  const size_t NL=InTemplate.countNLevel(0);
  for(size_t i=1;i<=NL;i++)
    if (*PR== (*InTemplate.findNode(0,i)))
      return 1;
 
  return 0;
}

template<typename T,typename U>
bool
mergeTemplate<T,U>::containOuterRules(const Rule* PR) const
  /*!
    Determine in the PR rule is a containing rule for the
    outer System
    \param PR :: Rule to test 
  */
{
  const size_t NL=OutTemplate.countNLevel(0);
  for(size_t i=1;i<=NL;i++)
    if (*PR==OutTemplate.findNode(0,i))
      return 1;
 
  return 0;
}

template<typename T,typename U>
void
mergeTemplate<T,U>::write(std::ostream& OX) const
  /*!
    Write out stuff
    \param OX :: output stream
  */
{
  OX<<"In Rule == "<<InTemplate.display()<<std::endl;
  OX<<"Out Rule == "<<OutTemplate.display()<<std::endl;
  return;
}

///\cond TEMPLATE

template class mergeTemplate<Geometry::Plane,Geometry::Plane>;
  //template class mergeTemplate<Geometry::Cylinder,Geometry::Cylinder>;
  //template class mergeTemplate<Geometry::Cylinder,Geometry::Plane>;

///\endcond TEMPLATE




} // NAMESPACE ModelSupport

