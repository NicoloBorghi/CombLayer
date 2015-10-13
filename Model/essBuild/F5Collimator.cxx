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
  {
    theta = -1111;
  }

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
    Populate all the variables for collimators without glue points
    \param Control :: Variable table to use
  */
  {
    ELog::RegMethod RegA("F5Collimator","populate");

    xStep=Control.EvalVar<double>(keyName+"X");
    yStep=Control.EvalVar<double>(keyName+"Y");
    zStep=Control.EvalVar<double>(keyName+"Z");

    length=Control.EvalVar<double>(keyName+"Length"); // along x
    wall=Control.EvalDefVar<double>(keyName+"WallThick", 0.5);

    GluePoint = Control.EvalDefVar<int>(keyName+"GluePoint", -1);

    tallySystem::point gC,gB,gB2;

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

    return;
  }


  void F5Collimator::populateWithTheta(FuncDataBase& Control)
  /*!
    Populate all the variables in the case when \theta is defined
    (i.e. if theta is defined then glue points are calculated automatically)
    \param Control :: Variable table to use
  */
  {
    ELog::RegMethod RegA("F5Collimator","populate");

    if (theta<0) 
      {
	ELog::EM << "Theta must be defined when this method is used. Call F5Collimator::populate() if glue points are not needed"
		 << ELog::endErr;
	return;
      }

    radius=Control.EvalDefVar<double>("F5Radius", -1);  // used with theta. If set, this value is the same for all collimators. Must be positive to be used with theta.
    if (radius<0)
      {
	ELog::EM << "Radius must be positive if used with theta" << ELog::endErr;
	return;
      }


    length=Control.EvalVar<double>(keyName+"Length"); // along x
    wall=Control.EvalDefVar<double>(keyName+"WallThick", 0.5);
    viewWidth=Control.EvalVar<double>(keyName+"ViewWidth");

    // xyz coordinates of F5 tally
    Control.setVariable<double>(keyName+"X", radius*sin(theta*M_PI/180.0));
    Control.setVariable<double>(keyName+"Y", radius*cos(theta*M_PI/180.0));

    xStep=Control.EvalVar<double>(keyName+"X");
    yStep=Control.EvalVar<double>(keyName+"Y");

    const double zmin = vecFP[vecFP.size()-2].Z();
    const double zmax = vecFP[vecFP.size()-1].Z();
    Control.setVariable<double>(keyName+"Z", (zmin+zmax)/2.0);
    zStep=Control.EvalVar<double>(keyName+"Z");


    tallySystem::point gC,gB,gB2;
    size_t fpshift(0); // shift in the vecFP, depends on the range
    if (range=="cold")
      {
	// glue point (defined by theta)
	if (theta<90)
	  Control.setVariable<double>(keyName+"GluePoint", 0);
	else if (theta<180)
	  Control.setVariable<double>(keyName+"GluePoint", 3);
	else if (theta<270)
	  Control.setVariable<double>(keyName+"GluePoint", 2);
	else // if theta>270
	  Control.setVariable<double>(keyName+"GluePoint", 1);

	GluePoint = Control.EvalDefVar<int>(keyName+"GluePoint", -1);

	fpshift = 6+GluePoint;
      }
    else if (range=="thermal")
      {
	throw ColErr::AbsObjMethod("'thermal' range in F5Collimator not yet implemented");
      }
    else
      {
	ELog::EM << "Range must be either 'cold' or 'thermal'" << ELog::endErr;
      }

    Control.setVariable<double>(keyName+"XB", vecFP[fpshift].X());
    Control.setVariable<double>(keyName+"YB", vecFP[fpshift].Y());
    Control.setVariable<double>(keyName+"ZB", zmax);

    // Calculate the coordinate of L (the second point)
    /*
      F and L are two points where the collimator looks
      O is the F5 tally location

      C
      |
      |                              O(xStep, yStep, zStep)
      |
      B(fpshift+0, fpshift+1, zStep)

    */

    Geometry::Vec3D B(vecFP[fpshift].X(), vecFP[fpshift].Y(), zmax);
    Geometry::Vec3D OB(B[0]-xStep, B[1]-yStep, B[2]-zStep);

    // Calculate angle BOC by the law of cosines:
    double BOC = acos((2*pow(OB.abs(), 2) - pow(viewWidth, 2))/(2*pow(OB.abs(), 2)));
    if ((GluePoint==0) || (GluePoint==2))
      BOC *= -1;
    Geometry::Vec3D OC(OB);
    Geometry::Quaternion::calcQRotDeg(BOC*180/M_PI,Z).rotate(OC);

    Geometry::Vec3D BC(OC-OB);
    if (BC.abs()-viewWidth>Geometry::zeroTol)
      ELog::EM << "Problem with tally " << keyName << ": distance between B and C is " << BC.abs() << " --- not equal to F5ViewWidth = " << viewWidth << ELog::endErr;

    Control.setVariable<double>(keyName+"XC", B[0]+BC[0]);
    Control.setVariable<double>(keyName+"YC", B[1]+BC[1]);
    Control.setVariable<double>(keyName+"ZC", B[2]+BC[2]);

    Control.setVariable<double>(keyName+"ZG", zmin);


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

  void F5Collimator::setTheta(double t)
  {
    if ((t<0) || (t>360))
      throw ColErr::RangeError<double>(theta, 0, 360, "Theta must be set in range 0-360 deg");
    theta=t;
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
    if (theta>0)
      populateWithTheta(System.getDataBase()); // build collimators with glue points
    else
      populate(System.getDataBase()); // no glue points

    createUnitVector(FC);
    createSurfaces();
    createObjects(System);
    createLinks();
    insertObjects(System);       

    return;
  }

}  // namespace essSystem
