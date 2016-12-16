/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   source/GammaSource.cxx
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "doubleErr.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "Source.h"
#include "SrcItem.h"
#include "SrcData.h"
#include "surfRegister.h"
#include "ModelSupport.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "WorkData.h"
#include "World.h"
#include "GammaSource.h"

namespace SDef
{

GammaSource::GammaSource(const std::string& keyName) : 
  attachSystem::FixedOffset(keyName,0),cutEnergy(0.0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param keyName :: main name
  */
{}

GammaSource::GammaSource(const GammaSource& A) : 
  attachSystem::FixedOffset(A),
  particleType(A.particleType),cutEnergy(A.cutEnergy),
  shape(A.shape),width(A.width),height(A.height),
  radius(A.radius),angleSpread(A.angleSpread),
  FocusPoint(A.FocusPoint),Direction(A.Direction),
  weight(A.weight),Energy(A.Energy),EWeight(A.EWeight)
  /*!
    Copy constructor
    \param A :: GammaSource to copy
  */
{}

GammaSource&
GammaSource::operator=(const GammaSource& A)
  /*!
    Assignment operator
    \param A :: GammaSource to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      particleType=A.particleType;
      cutEnergy=A.cutEnergy;
      shape=A.shape;
      width=A.width;
      height=A.height;
      radius=A.radius;
      angleSpread=A.angleSpread;
      FocusPoint=A.FocusPoint;
      Direction=A.Direction;
      weight=A.weight;
      Energy=A.Energy;
      EWeight=A.EWeight;
    }
  return *this;
}

GammaSource::~GammaSource() 
  /*!
    Destructor
  */
{}

int
GammaSource::populateEFile(const std::string& FName,
			   const int colE,const int colP)
  /*!
    Load a distribution table
    - Care is taken to add an extra energy with zero 
    weight onto the table since we are using a
    \param FName :: filename 
    return 0 on failure / 1 on success
  */
{
  ELog::RegMethod RegA("GammaSource","loadEnergy");

  const int eCol(colE);
  const int iCol(colP);
  
  Energy.clear();
  EWeight.clear();
  
  WorkData A;
  if (FName.empty() || A.load(FName,eCol,iCol,0))
    return 0;


  A.xScale(1e-6);   // convert to MeV
  A.binDivide(1.0);
  DError::doubleErr IV=A.integrate(cutEnergy,1e38);
  // Normalize A:
  A/=IV;

  Energy=A.getXdata();
  if (Energy.size()<2)
    {
      ELog::EM<<"Failed to read energy/data from file:"<<FName<<ELog::endErr;
      return 0;
    }
  Energy.push_back(2.0*Energy.back()-Energy[Energy.size()-2]);
  const std::vector<DError::doubleErr>& Yvec=A.getYdata();

  std::vector<DError::doubleErr>::const_iterator vc;
  EWeight.push_back(0.0);
  for(vc=Yvec.begin();vc!=Yvec.end();vc++)
    EWeight.push_back(vc->getVal());
  EWeight.push_back(0.0);
  return (EWeight.empty()) ? 0 : 1;
}

int
GammaSource::populateEnergy(std::string EPts,std::string EProb)
  /*!
    Read two strings that are the Energy points and the 
    \param EPts :: Energy Points string 
    \param EProb :: Energy Prob String
    \return 1 on success success
   */
{
  ELog::RegMethod RegA("GammaSource","populateEnergy");

  Energy.clear();
  EWeight.clear();

  double eB,eP;
  
  // if (!StrFunc::section(EPts,eA) || eA<0.0)
  //   return 0;
  while(StrFunc::section(EPts,eB) &&
	StrFunc::section(EProb,eP))
    {
      if (!Energy.empty() && eB<=Energy.back())
	throw ColErr::IndexError<double>(eB,Energy.back(),
					 "Energy point not in sequence");
      if (eP<0.0)
	throw ColErr::IndexError<double>(eP,0.0,"Probablity eP negative");
      Energy.push_back(eB);
      EWeight.push_back(eP);
    }
  if (!StrFunc::isEmpty(EPts) || !StrFunc::isEmpty(EProb))
    ELog::EM<<"Trailing line info \n"
	    <<"Energy : "<<EPts<<"\n"
  	    <<"Prob : "<<EProb<<ELog::endErr;

  // // Normalize 
  // for(double& prob : EWeight)
  //   prob/=sum;
  return (EWeight.empty()) ? 0 : 1;
}
  
void
GammaSource::populate(const FuncDataBase& Control)
  /*!
    Populate Varaibles
    \param Control :: Control variables
   */
{
  ELog::RegMethod RegA("GammaSource","populate");

  FixedOffset::populate(Control);

  // default photon
  particleType=Control.EvalDefVar<int>(keyName+"ParticleType",2);
  shape=Control.EvalDefVar<size_t>(keyName+"Shape",0);
  if (!shape)   // circle
    radius=Control.EvalVar<double>(keyName+"Radius");
  else
    {
      height=Control.EvalVar<double>(keyName+"Height");
      width=Control.EvalVar<double>(keyName+"Width");
    }    
  angleSpread=Control.EvalDefVar<double>(keyName+"ASpread",0.0); 
  
  const std::string EList=
    Control.EvalDefVar<std::string>(keyName+"Energy","");
  const std::string EPList=
    Control.EvalDefVar<std::string>(keyName+"EProb","");
  const std::string EFile=
    Control.EvalDefVar<std::string>(keyName+"EFile","");

  if (!populateEnergy(EList,EPList) &&
      !populateEFile(EFile,1,11))
    {
      double E=Control.EvalDefVar<double>(keyName+"EStart",-1.0); 
      const size_t nE=Control.EvalDefVar<size_t>(keyName+"NE",1);
      if (nE<2)
        {
          Energy.push_back(E);
        }
      else
        {
          const double EEnd=Control.EvalVar<double>(keyName+"EEnd"); 
          const double EStep((EEnd-E)/(nE+1));
          for(size_t i=0;i<nE;i++)
            {
              Energy.push_back(E);
              EWeight.push_back(1.0);
              E+=EStep;
            }
        }
    }
  return;
}

void
GammaSource::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int linkIndex)
  /*!
    Create the unit vector
    \param FC :: Fixed Componenet
    \param linkIndex :: Link index [signed for opposite side]
   */
{
  ELog::RegMethod RegA("GammaSource","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC,linkIndex);
  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);
  Direction=Y;

  return;
}
  
void
GammaSource::calcPosition()
  /*!
    Calcuate the focus position and other points
  */    
{
  ELog::RegMethod RegA("GammaSource","calcPosition");
  if (angleSpread>Geometry::zeroTol)
    FocusPoint=Origin-Direction*(radius/tan(M_PI*angleSpread/180.0));
  else
    FocusPoint=Origin;
  return;
}

void
GammaSource::createSource(SDef::Source& sourceCard) const
  /*!
    Creates a gamma bremstraual source
    \param sourceCard :: Source system
  */
{
  ELog::RegMethod RegA("GammaSource","createSource");

  ELog::EM<<"Source shape ::"<<shape<<ELog::endDiag;
  sourceCard.setComp("par",particleType);            /// photon (2)
    
  // Energy:
  if (Energy.size()>1)
    {
      SDef::SrcData D1(1);
      SDef::SrcInfo SI1('A');
      SDef::SrcProb SP1;
      SP1.setData(EWeight);
      SI1.setData(Energy);
      D1.addUnit(SI1);
      D1.addUnit(SP1);
      sourceCard.setData("erg",D1);
    }
  else if (!Energy.empty())
    sourceCard.setComp("erg",Energy.front());

  ELog::EM<<"AngleSPread == "<<angleSpread<<ELog::endDiag;
  if (angleSpread>Geometry::zeroTol)
    {
      ELog::EM<<"Adding ARI card"<<angleSpread<<ELog::endDiag;
      SDef::SrcData D2(2);
      SDef::SrcInfo SI2;
      SI2.addData(-1.0);
      SI2.addData(cos(M_PI*angleSpread/180.0));
      SI2.addData(1.0);
      SDef::SrcProb SP2;
      SP2.setData({0.0,0.0,1.0});
      D2.addUnit(SI2);
      D2.addUnit(SP2);
      sourceCard.setData("dir",D2);
    }
  else
    {
      ELog::EM<<"Adding dir card"<<ELog::endDiag;
      sourceCard.setComp("dir",1.0);
    }

  if (!shape)
    createRadialSource(sourceCard);
  else
    createRectangleSource(sourceCard);

  return;
}

void
GammaSource::createRadialSource(SDef::Source& sourceCard) const
  /*!
    Creates a gamma bremstraual source
    \param sourceCard :: Source system
  */
{
  ELog::RegMethod RegA("GammaSource","createRadialSource");
  
  sourceCard.setActive();
  sourceCard.setComp("vec",Direction);
  sourceCard.setComp("pos",FocusPoint);

  ELog::EM<<"Direction  "<<Direction<<ELog::endDiag;
  ELog::EM<<"FocusPoint "<<FocusPoint<<ELog::endDiag;

  return;
}  

void
GammaSource::createRectangleSource(SDef::Source& sourceCard) const
  /*!
    Creates a gamma bremstraual source
    \param sourceCard :: Source system
  */
{
  ELog::RegMethod RegA("GammaSource","createRectangleSource");

  ELog::EM<<"REC SOURCE"<<ELog::endDiag;
  sourceCard.setActive();
  sourceCard.setComp("vec",Direction);
  sourceCard.setComp("y",Origin.Y());
  sourceCard.setComp("ara",width*height);
  
  SDef::SrcData D3(3);
  SDef::SrcInfo SI3;
  SI3.addData(Origin[0]-width/2.0);
  SI3.addData(Origin[0]+width/2.0);

  SDef::SrcData D4(4);
  SDef::SrcInfo SI4;
  SI4.addData(Origin[2]-height/2.0);
  SI4.addData(Origin[2]+height/2.0);

  SDef::SrcProb SP3;
  SP3.addData(0.0);
  SP3.addData(1.0);
  D3.addUnit(SI3);  
  D3.addUnit(SP3);  
  sourceCard.setData("x",D3);

  SDef::SrcProb SP4;
  SP4.addData(0);
  SP4.addData(1.0);
  D4.addUnit(SI4);  
  D4.addUnit(SP4);  
  sourceCard.setData("z",D4);
  return;
}  

void
GammaSource::createAll(const FuncDataBase& Control,
		       SDef::Source& sourceCard)
  /*!
    Create all the source
    \param Control :: DataBase for variables
    \param souceCard :: Source Term
   */
{
  ELog::RegMethod RegA("GammaSource","createAll");
  populate(Control);
  createUnitVector(World::masterOrigin(),0);
  calcPosition();
  createSource(sourceCard);
  return;
}


void
GammaSource::createAll(const FuncDataBase& Control,
		       const attachSystem::FixedComp& FC,
		       const long int linkIndex,
		       SDef::Source& sourceCard)

  /*!
    Create all the source
    \param Control :: DataBase for variables
    \param souceCard :: Source Term
   */
{
  ELog::RegMethod RegA("GammaSource","createAll<FC,linkIndex>");
  populate(Control);
  createUnitVector(FC,linkIndex);
  calcPosition();
  createSource(sourceCard);
  return;
}


} // NAMESPACE SDef
