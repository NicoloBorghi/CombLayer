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
#include <boost/shared_ptr.hpp>

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
#include "F5Calc.h"
#include "F5Collimator.h"

namespace essSystem
{

  F5Collimator::F5Collimator(const std::string& Key) :
    attachSystem::ContainedComp(),attachSystem::FixedComp(Key,6),
    colIndex(ModelSupport::objectRegister::Instance().cell(Key)),
    cellIndex(colIndex+1)
    /*!
      Constructor
      \param Key :: Name of construction key
    */
  {}

  /*  F5Collimator::F5Collimator(const F5Collimator& A) : 
  colIndex(A.colIndex), cellIndex(A.cellIndex)
  {
  }*/

  F5Collimator::~F5Collimator()
  /*!
    Destructor
  */
  {}

  void F5Collimator::populate(FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
  {
    ELog::RegMethod RegA("F5Collimator","populate");

    // Master values
    xStep=Control.EvalVar<double>(keyName+"X");
    yStep=Control.EvalVar<double>(keyName+"Y");
    zStep=Control.EvalVar<double>(keyName+"Z");
    //    xyAngle=getXYAngle(); //Control.EvalVar<double>(keyName+"XYangle");
    //    zAngle=Control.EvalVar<double>(keyName+"Zangle");

    length=Control.EvalVar<double>(keyName+"Length"); // along x
    wall=Control.EvalDefVar<double>(keyName+"WallThick", 0.5);

    GluePoint = Control.EvalDefVar<int>(keyName+"GluePoint", -1);

    tallySystem::point gC,gB,gB2;

    if (GluePoint>=0) {
      std::ifstream essdat; // currently used by collimators
      essdat.open(".ess.dat", std::ios::in);
      double F[12], L[13];
      while (!essdat.eof()) {
	std::string str;
	std::getline(essdat, str);
	std::stringstream ss(str);
	std::string header; // F: or L: point title
	ss >> header;
	int i=0;
	if (header == "F:")
	  while(ss >> F[i]) i++;
	else if (header == "L:")
	  while(ss >> L[i]) i++;
	
	int gpshift = GluePoint*3;
	if (F[2]>0) { // top moderator;
	  Control.setVariable<double>(keyName+"XB", F[gpshift+0]);
	  Control.setVariable<double>(keyName+"YB", F[gpshift+1]);
	  Control.setVariable<double>(keyName+"ZB", F[gpshift+2]);

	  Control.setVariable<double>(keyName+"XC", L[gpshift+0]);
	  Control.setVariable<double>(keyName+"YC", L[gpshift+1]);
	  Control.setVariable<double>(keyName+"ZC", L[gpshift+2]);

	  Control.setVariable<double>(keyName+"ZG", L[12]);
	}
      } 
      essdat.close();
    } 
    gB.x=Control.EvalDefVar<double>(keyName+"XB", 0);
    gB.y=Control.EvalDefVar<double>(keyName+"YB", 0);
    gB.z=Control.EvalDefVar<double>(keyName+"ZB", 0);

    gC.x=Control.EvalDefVar<double>(keyName+"XC", 0);
    gC.y=Control.EvalDefVar<double>(keyName+"YC", 0);
    gC.z=Control.EvalDefVar<double>(keyName+"ZC", 0);

    gB2.z = Control.EvalDefVar<double>(keyName+"ZG", 0);
    

    gB2.x = gB.x;
    gB2.y = gB.y;

    SetTally(xStep, yStep, zStep);
    SetPoints(gB, gC, gB2);
    SetLength(length);
    xyAngle = GetXYAngle();
    zAngle  = GetZAngle();
    width = 2*GetHalfSizeX();
    height = 2*GetHalfSizeZ();

    /*    std::cout << "F5 detector: " << xStep << " " << yStep << " " << zStep << std::endl;
    std::cout << "B: " << gB.x << " " << gB.y << " " << gB.z << std::endl;
    std::cout << "C: " << gC.x << " " << gC.y << " " << gC.z << std::endl;
    std::cout << "B2(G): " << gB2.x << " " << gB2.y << " " << gB2.z << std::endl;
    std::cout << "F5Col::dx: " << width << std::endl;
    std::cout << "F5Col::dz: " << height << std::endl;
    std::cout << "F5Col::xyAngle: " << xyAngle << std::endl;
    std::cout << "F5Col::zAngle: " << zAngle << std::endl;
    std::cout << "F5Col::Omega: " << GetOmega()*1E+6 << " mksr" << std::endl;
    */
    return;
  }

  void F5Collimator::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
  */
  {
    ELog::RegMethod RegA("F5Collimator","createUnitVector");
    attachSystem::FixedComp::createUnitVector(FC);
    applyShift(xStep,yStep,zStep);
    applyAngleRotate(xyAngle,zAngle);
  
    return;
  }

  void F5Collimator::createSurfaces()
  {
    /*!
      Create Surfaces for the F5 collimator
    */
    ELog::RegMethod RegA("F5Collimator","createSurfaces");
      
    //    SMap.addMatch(colIndex+1, FC.getLinkSurf(2)); // inner radius
    ModelSupport::buildPlane(SMap,colIndex+1, Origin-X*1.0, X);
    ModelSupport::buildPlane(SMap,colIndex+11, Origin-X*(1.0+wall), X);
    ModelSupport::buildPlane(SMap,colIndex+2, Origin+X*length, X);

    ModelSupport::buildPlane(SMap,colIndex+3, Origin-Y*(width/2.0), Y);
    ModelSupport::buildPlane(SMap,colIndex+13, Origin-Y*(width/2.0+wall), Y);
    ModelSupport::buildPlane(SMap,colIndex+4, Origin+Y*(width/2.0), Y);
    ModelSupport::buildPlane(SMap,colIndex+14, Origin+Y*(width/2.0+wall), Y);

    ModelSupport::buildPlane(SMap,colIndex+5, Origin-Z*(height/2.0), Z); 
    ModelSupport::buildPlane(SMap,colIndex+15, Origin-Z*(height/2.0+wall), Z);
    ModelSupport::buildPlane(SMap,colIndex+6, Origin+Z*(height/2.0), Z);
    ModelSupport::buildPlane(SMap,colIndex+16, Origin+Z*(height/2.0+wall), Z);

    return; 
  }

  void F5Collimator::addToInsertChain(attachSystem::ContainedComp& CC) const
  /*!
    Adds this object to the containedComp to be inserted.
    \param CC :: ContainedComp object to add to this
  */
  {
    for(int i=colIndex+1;i<cellIndex;i++)
      CC.addInsertCell(i);
    
    return;
  }


  void F5Collimator::createObjects(Simulation& System)
  /*!
    Create the F5 collimator
    \param System :: Simulation to add results
  */
  {
    ELog::RegMethod RegA("F5Collimator","createObjects");

    std::string Out;
  
    int voidMat = 0;//1001;
    Out=ModelSupport::getComposite(SMap,colIndex, " 11 -2 13 -14 15 -16");
    addOuterSurf(Out);

    // Internal region
    Out=ModelSupport::getComposite(SMap,colIndex," 1 -2 3 -4 5 -6");
    System.addCell(MonteCarlo::Qhull(cellIndex++, voidMat, 0.0, Out));

    // Wall
    Out=ModelSupport::getComposite(SMap, colIndex, " (11 -2 13 -14 15 -16) (-1:2:-3:4:-5:6) ");
    MonteCarlo::Qhull c = MonteCarlo::Qhull(cellIndex++, voidMat, 0.0, Out);
    c.setImp(0);
    System.addCell(c);

    return; 
  }

  void F5Collimator::createLinks()
  /*!
    Creates a full attachment set
    Links/directions going outwards true.
  */
  {
    ELog::RegMethod RegA("F5Collimator","createLinks");

    FixedComp::setConnect(0,   Origin-X*(1.0+wall), -X);   FixedComp::setLinkSurf(0, -SMap.realSurf(colIndex+1));
    FixedComp::setConnect(1,   Origin+X*length,  X);

    FixedComp::setConnect(2,   Origin-Y*(width/2+wall), -Y);
    FixedComp::setConnect(3,   Origin+Y*(width/2+wall),  Y);

    FixedComp::setConnect(4,   Origin-Z*(height/2+wall), -Z); 
    FixedComp::setConnect(5,   Origin+Z*(height/2+wall),  Z); 

    /*FixedComp::setLinkSurf(0,  -SMap.realSurf(colIndex+1));
    FixedComp::setLinkSurf(1, SMap.realSurf(colIndex+2));
    FixedComp::setLinkSurf(2,  -SMap.realSurf(colIndex+3));
    FixedComp::setLinkSurf(3, SMap.realSurf(colIndex+4));
    FixedComp::setLinkSurf(4,  SMap.realSurf(colIndex+6));
    FixedComp::setLinkSurf(5, -SMap.realSurf(colIndex+5));
    */

    for (size_t i=0; i<6; i++)
      FixedComp::setLinkSurf(i, SMap.realSurf(colIndex+static_cast<int>(i)));
    
    return;
  }


  void
  F5Collimator::createAll(Simulation& System, const attachSystem::FixedComp& FC)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
  */
  {
    ELog::RegMethod RegA("F5Collimator","createAll");
    populate(System.getDataBase());

    createUnitVector(FC);
    createSurfaces();
    createObjects(System);
    createLinks();
    insertObjects(System);       

    return;
  }

}  // namespace essSystem
