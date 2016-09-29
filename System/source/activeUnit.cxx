/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   source/activeUnit.cxx
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
#include <boost/format.hpp>

#include "Exception.h"
#include "MersenneTwister.h"
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
#include "doubleErr.h"
#include "mathSupport.h"
#include "WorkData.h"
#include "activeUnit.h"

extern MTRand RNG;

namespace SDef
{

activeUnit::activeUnit(const std::vector<double>& E,
                       const std::vector<double>& G) :
  volume(0.0),integralFlux(0.0),
  energy(E),cellFlux(G)
  /*!
    Constructor 
    \param E :: Energy
    \param G :: gamma value
  */
{}

  
activeUnit::activeUnit(const activeUnit& A) :
  volume(A.volume),integralFlux(A.integralFlux),
  energy(A.energy),cellFlux(A.cellFlux)
  /*!
    Copy Constructor 
    \param A :: activeUnit to copy
  */
{}

activeUnit&
activeUnit::operator=(const activeUnit& A)
  /*!
    Assignement operator
    \param A :: activeUnit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      volume=A.volume;
      integralFlux=A.integralFlux;
      energy=A.energy;
      cellFlux=A.cellFlux;
    }
  return *this;
}  

activeUnit::~activeUnit() 
  /*!
    Destructor
  */
{}


void
activeUnit::normalize(const double V)
  /*!
    Normalize and integrate the volumes
    \param V :: Volume of object
   */
{
  ELog::RegMethod RegA("activeUnit","normalize");

  integralFlux=0.0;

  std::vector<double> YOut;
  for(const double& FV : cellFlux)
    integralFlux+=FV;

  if (integralFlux>Geometry::zeroTol)
    {
      double prevSum(0.0);
      for(double& FV : cellFlux)
	{
	  prevSum+=FV;
	  FV=prevSum/integralFlux;
	}
      volume=V;
      integralFlux/=V;  // need to normalize
    }
  return;
}

double
activeUnit::XInverse(const double R) const
  /*!
    Inverse the y data [as based]
    \param R :: Value to search for
  */
{

  long int index=indexPos(cellFlux,R);
  const size_t IX=static_cast<size_t>(index);
  if (index<0 || IX>=cellFlux.size())
    throw ColErr::RangeError<double>(R,cellFlux.front(),
                                     cellFlux.back(),"value out of range");
  
  const double yA=cellFlux[IX];
  const double yB=cellFlux[IX+1];

  const double frac=(R-yA)/(yB-yA);

  return energy[IX]+frac*(energy[IX+1]-energy[IX]);
}



  
void
activeUnit::writePhoton(std::ostream& OX,const Geometry::Vec3D& Pt) const
  /*!
    Calculate the energy based on RNG nubmer and 
    write a photon in a random direction
    \parma OX :: Output stream
    \param Pt :: Point for interaction
  */
{
  boost::format FMT("% 12.6e %|14t| % 12.6e %|28t| % 12.6e");
  boost::format FMTB("% 12.6e %|14t| % 12.6e");
  const double thetaAngle=2*M_PI*RNG.rand();
  const double z=2.0*(RNG.rand()-0.5);
  const double sinZ=sqrt(1-z*z);
  const Geometry::Vec3D uvw(sinZ*cos(thetaAngle),sinZ*sin(thetaAngle),z);

  const double R=RNG.rand();
  const double E=XInverse(R);

  //  OX<<(FMT % Pt.X() % Pt.Y() % Pt.Z())<<std::endl;
  OX<<"2 "<<(FMT % Pt.X() % Pt.Y() % Pt.Z());
  OX<<"  "<<(FMT % uvw.X() % uvw.Y() % uvw.Z());
  OX<<"  "<<(FMTB % E % integralFlux)<<std::endl;


  //  OX<<2<<" "<<Pt<<" "<<uvw<<" "<<E<<" "<<1.0<<std::endl;
  return;
}
  

  
} // NAMESPACE SDef