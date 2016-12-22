/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/DefUnitsESS.cxx
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
#include <vector>
#include <set>
#include <list>
#include <map>
#include <string>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "InputControl.h"
#include "inputParam.h"
#include "support.h"
#include "stringCombine.h"
#include "defaultConfig.h"
#include "DefUnitsESS.h"

namespace mainSystem
{

void 
setDefUnits(FuncDataBase& Control,
            inputParam& IParam)
  /*!
    Based on the defaultConf set up the model
    \param Control :: FuncDataBase
    \param IParam :: input system
  */
{
  ELog::RegMethod RegA("DefUnitsESS[F]","setDefUnits");

  defaultConfig A("");
  if (IParam.flag("defaultConfig"))
    {
      const std::string Key=IParam.getValue<std::string>("defaultConfig");
      
      std::vector<std::string> LItems=
	IParam.getObjectItems("defaultConfig",0);

      const std::string sndItem=(LItems.size()>1) ? LItems[1] : "";
      const std::string extraItem=(LItems.size()>2) ? LItems[2] : "";

      if (Key=="Main")
        setESS(A);
      else if (Key=="Full")
        setESSFull(A);
      else if (Key=="PortsOnly")
	setESSPortsOnly(A,sndItem,extraItem);
      else if (Key=="Single")
	setESSSingle(A,LItems);
      else if (Key=="neutronics")
	setESSNeutronics(A,sndItem,extraItem);
      else if (Key=="help")
	{
	  ELog::EM<<"Options : "<<ELog::endDiag;
	  ELog::EM<<"  Main : Everything that works"<<ELog::endDiag;
	  ELog::EM<<"  Full : Beamline on every port"<<ELog::endDiag;
	  ELog::EM<<"  PortsOnly [lower/upper] : Nothing beyond beamport "
		  <<ELog::endDiag;
	  ELog::EM<<"  Single  beamLine : Single beamline [for BL devel] "
		  <<ELog::endDiag;
	  ELog::EM<<"  neutronics {BF1,BF2} [single]: configuration for neutronics calculations " << ELog::endDiag;
	  throw ColErr::ExitAbort("Iparam.defaultConfig");	  
	}
      else 
        {
          ELog::EM<<"Unknown Default Key ::"<<Key<<ELog::endDiag;
          throw ColErr::InContainerError<std::string>
            (Key,"Iparam.defaultConfig");
        }
      A.process(Control,IParam);
    }
  return;
}

void
setESSFull(defaultConfig& A)
  /*!
    Default configuration for ESS
    \param A :: Paramter for default config
   */
{
  ELog::RegMethod RegA("DefUnitsESS[F]","setESS");

  A.setOption("lowMod","Butterfly");
  A.setOption("topMod","Butterfly");

  const std::map<std::string,std::string> beamDef=
    {
      {"MAGIC","G4BLine6"},          // W6
      {"BIFROST","G4BLine4"},    // W4
      {"NMX","G4BLine1"},        // W1
      {"VOR","G3BLine10"},   // also 17
      {"LOKI","G4BLine17"},
      {"DREAM","G4BLine19"},
      // {"CSPEC","G4BLine3"},
       {"VESPA","G3BLine7"},
      {"FREIA","G4BLine15"}     // N5
      // {"ODIN","G2BLine2"}
    };
  
  const std::set<std::string> beamFilled=
    {"NMX","CSPEC","DREAM","FREIA","SHORTDREAM","SHORTDREAM2","LOKI",
     "MAGIC","VESPA","VOR"};

  size_t index(0);
  std::map<std::string,std::string>::const_iterator mc;
  for(mc=beamDef.begin();mc!=beamDef.end();mc++)
    {
      A.setMultiOption("beamlines",index,mc->second+" "+mc->first);
      A.setVar(mc->second+"Active",1);
      if (beamFilled.find(mc->first)!=beamFilled.end())
        A.setVar(mc->second+"Filled",1);
      index++;
    }
  return;
}

void
setESSPortsOnly(defaultConfig& A,const std::string& lvl,
                const std::string& lvlExtra)
  /*!
    Default configuration for ESS for beamports only
    \param A :: Paramter for default config
    \param lvl :: level value (lower/uppper)
    \param lvlExtra :: extra level value (lower/uppper)
   */
{
  ELog::RegMethod RegA("DefUnitsESS[F]","setESS");

  static const std::map<std::string,std::string>
    nameBay({
        {"lower","G1BLine"},
        {"upper","G4BLine"},
        {"farUpper","G3BLine"},
        {"farLower","G2BLine"}
      });
  
  A.setOption("lowMod","Butterfly");

  int defaultFlag(0);
  std::string GNum;
  std::map<std::string,std::string>::const_iterator mc=
    nameBay.find(lvlExtra);
  if (mc!=nameBay.end())
    {
      for(size_t i=0;i<21;i++)
        A.setVar(mc->second+StrFunc::makeString(i+1)+"Active",1);
      defaultFlag=1;
      GNum=mc->second+" ";
    }
  // and take default:
  mc=nameBay.find(lvl);
  if (mc!=nameBay.end())
    {
      for(size_t i=0;i<21;i++)
        A.setVar(mc->second+StrFunc::makeString(i+1)+"Active",1);
      defaultFlag=1;
      GNum+=mc->second;
    }
  if (!defaultFlag)
    {
      GNum="G4BLine";
      for(size_t i=0;i<21;i++)
        A.setVar("G4BLine"+StrFunc::makeString(i+1)+"Active",1);
    }
      
  ELog::EM<<"Port Only on sectors:"<<GNum<<ELog::endDiag;
  return;
}

void
setESSSingle(defaultConfig& A,
	     std::vector<std::string>& LItems)

  /*!
    Default configuration for ESS for testing single beamlines
    for building
    \param A :: Paramter for default config
    \param LItems :: single selection
   */
{
  ELog::RegMethod RegA("DefUnitsESS[F]","setESSSingle");

  
  A.setOption("lowMod","Butterfly");
  const std::map<std::string,std::string> beamDefNotSet=
    { 
     {"HEIMDAL","G4BLine8"},
     {"SLEIPNIR","G4BLine13"},   // N9
     {"ANNI","G3BLine3"},        // E3
     {"SURFSCATTER","G3BLine8"},        // E8
     {"SKADI","G3BLine5"}         // E5
    };     
  const std::map<std::string,std::string> beamDef=
    {{"NMX","G4BLine1"},        // W1
     {"BIFROST","G4BLine4"},    // W4
     {"MIRACLES","G4BLine5"},   // W5
     {"SHORTDREAM","G4BLine17"},
     {"SHORTODIN","G1BLine4"},
     {"TREX","G4BLine7"},       // W7
     {"MAGIC","G4BLine6"},      // W6
     {"DREAM","G3BLine19"},     // S3
     {"CSPEC","G4BLine3"},      // W3
     {"VESPA","G3BLine7"},      // E7
     {"VOR","G3BLine10"},       // E11  [CHANGED TO FIT]
     {"SIMPLE","G4BLine17"},
     {"LOKI","G4BLine17"},      // N7
     {"ODIN","G2BLine2"},       // Lower S2
     {"ESTIA","G3BLine2"},      // E2
     {"FREIA","G4BLine15"},     // N5
     {"BEER","G4BLine2"}        // W2
    };     
  const std::set<std::string> beamFilled=
    {"BEER","BIFROST","CSPEC","DREAM","FREIA","LOKI",
     "MAGIC","MIRACLES","NMX","VESPA","VOR","SHORTDREAM"};

  size_t beamLineIndex(0);
  while(!LItems.empty())
    {
      bool portFlag=0;
      if (LItems.front()!="Single")
        {
          const std::string beamItem=LItems.front();
          const std::string portItem=(LItems.size()>1) ? LItems[1] : "";
      
          std::map<std::string,std::string>::const_iterator mc=
            beamDef.find(beamItem);
	
          portFlag=beamDef.find(portItem)==beamDef.end();
      
          const int filled =
            (beamFilled.find(beamItem)==beamFilled.end()) ? 0 : 1;
          
          if (mc!=beamDef.end())
            {
	      ELog::EM<<"Beam Def  == "<<mc->first<<ELog::endDiag;
              if (!portFlag || portItem.empty())
                {
                  A.setMultiOption("beamlines",beamLineIndex,
				   mc->second+" "+beamItem);
                  A.setVar(mc->second+"Active",1);
                  if (filled)
                    A.setVar(mc->second+"Filled",1);
                }
              else
                {
                  A.setMultiOption("beamlines",beamLineIndex,
				   portItem+" "+beamItem);
                  A.setVar(portItem+"Active",1);
                  if (filled)
                    A.setVar(portItem+"Filled",1);
                }
	      beamLineIndex++;
            }
          else
            throw ColErr::InContainerError<std::string>(beamItem,"BeamItem");
        }
      if (portFlag)
        LItems.erase(LItems.begin(),LItems.begin()+1);
      else
        LItems.erase(LItems.begin());
    }
  return;
}

void
setESSNeutronics(defaultConfig& A, const std::string& modtype, const std::string& single)

  /*!
    Default configuration for ESS for testing single beamlines
    for building
    \param A :: Paramter for default config
    \param modtype :: Moderator type: either BF1 or BF2
    \param single :: if "single" then only top moderator is built;
                     if not specified then both moderators are built.
   */
{
  ELog::RegMethod RegA("DefUnitsESS[F]","setESSSingle");

  size_t bfType=0;
  
  if (modtype=="BF1")
    {
      bfType = 1;
      A.setVar("TopFlyLeftLobeXStep", -2.0);
      A.setVar("TopFlyRightLobeXStep", 2.0);
      A.setVar("TopFlyMidWaterMidYStep", 7.0);
      A.setOption("topPipe", "supply");

      A.setVar("LowFlyLeftLobeXStep", -2.0);
      A.setVar("LowFlyRightLobeXStep", 2.0);
      A.setVar("LowFlyMidWaterMidYStep", 7.0);
      A.setOption("lowPipe", "supply");

      // straighten the pipes
      A.setVar("TSupplyRightAlNSegIn", 1);
      A.setVar("TSupplyRightAlPPt0", Geometry::Vec3D(0,0,0));
      A.setVar("TSupplyRightAlPPt1", Geometry::Vec3D(0,15,0));
      A.setVar("TSupplyLeftAlNSegIn", 1);
      A.setVar("TSupplyLeftAlPPt0", Geometry::Vec3D(0,0,0));
      A.setVar("TSupplyLeftAlPPt1", Geometry::Vec3D(0,15,0));

      A.setVar("LSupplyRightAlNSegIn", 1);
      A.setVar("LSupplyRightAlPPt0", Geometry::Vec3D(0,0,0));
      A.setVar("LSupplyRightAlPPt1", Geometry::Vec3D(0,15,0));
      A.setVar("LSupplyLeftAlNSegIn", 1);
      A.setVar("LSupplyLeftAlPPt0", Geometry::Vec3D(0,0,0));
      A.setVar("LSupplyLeftAlPPt1", Geometry::Vec3D(0,15,0));
    } else if (modtype=="BF2")
    {
      bfType = 2;
      // variables are set in moderatorVariables
      // build pipes
      A.setOption("topPipe", "supply,return");
      A.setOption("lowPipe", "supply,return");
    } else
    throw ColErr::InvalidLine(modtype, "Either BF1 or BF2 are supported in defaultConfig");

  if (single=="")
    {
      A.setOption("lowMod", "Butterfly");
      A.setOption("topMod", "Butterfly");
    } else if (single=="single")
    {
      A.setOption("lowMod", "None");
      A.setOption("topMod", "Butterfly");
      A.setVar("BeRefLowVoidThick", 0);
      A.setVar("BeRefLowRefMat", "Iron_10H2O");
      A.setVar("BeRefLowWallMat", "Iron_10H2O");
      A.setVar("BeRefLowInnerStructureActive", 0);
      // Since there is no bunkers, we have to 
      // prolong collimators until they are emerged into ShutterBay, otherwise a neutron crosses imp=0 cell
      A.setVar("F5DefaultLength", 500.0);
    } else
    throw ColErr::InvalidLine(single,"Either 'single' or nothing are supported in defaultConfig");

  A.setVar("LowFlyType", bfType);
  A.setVar("TopFlyType", bfType);

  A.setOption("matDB", "neutronics");
  A.setOption("physModel", "BD");
  A.setOption("pinholeType","Straws");

  // simplify the bunkers
  A.setOption("bunker", "noPillar");
  A.setVar("ABunkerNRoofRadial", 1);
  A.setVar("ABunkerNRoofVert", 1);
  A.setVar("ABunkerNSectors", 1);
  A.setVar("ABunkerNSegment", 1);
  A.setVar("ABunkerNSideThick", 1);
  A.setVar("ABunkerNSideVert", 1);
  A.setVar("ABunkerRoofMat", "Void");
  A.setVar("ABunkerWallMat", "Void");

  A.setVar("BBunkerNRoofRadial", 1);
  A.setVar("BBunkerNRoofVert", 1);
  A.setVar("BBunkerNSectors", 1);
  A.setVar("BBunkerNSegment", 1);
  A.setVar("BBunkerNSideThick", 1);
  A.setVar("BBunkerNSideVert", 1);
  A.setVar("BBunkerRoofMat", "Void");
  A.setVar("BBunkerWallMat", "Void");

  A.setVar("CBunkerNRoofRadial", 1);
  A.setVar("CBunkerNRoofVert", 1);
  A.setVar("CBunkerNSectors", 1);
  A.setVar("CBunkerNSegment", 1);
  A.setVar("CBunkerNSideThick", 1);
  A.setVar("CBunkerNSideVert", 1);
  A.setVar("CBunkerRoofMat", "Void");
  A.setVar("CBunkerWallMat", "Void");

  A.setVar("DBunkerNRoofRadial", 1);
  A.setVar("DBunkerNRoofVert", 1);
  A.setVar("DBunkerNSectors", 1);
  A.setVar("DBunkerNSegment", 1);
  A.setVar("DBunkerNSideThick", 1);
  A.setVar("DBunkerNSideVert", 1);
  A.setVar("DBunkerRoofMat", "Void");
  A.setVar("DBunkerWallMat", "Void");

  // simplify the curtain
  A.setVar("CurtainNBaseLayers", 1);
  A.setVar("CurtainNMidLayers", 1);
  A.setVar("CurtainNTopLayers", 1);
  A.setVar("CurtainWallMat", "Void");

  // sdef
  A.setVar("sdefEnergy", 2000.0);
  A.setVar("sdefWidth", 7.0);
  A.setVar("sdefHeight", 1.6);
  A.setVar("sdefYPos", -500);
  A.setVar("sdefPDF", "uniform");
  
  return;
}

void
setESS(defaultConfig& A)
  /*!
    Default configuration for ESS
    \param A :: Paramter for default config
   */
{
  ELog::RegMethod RegA("DefUnitsESS[F]","setESS");


  A.setOption("lowMod","Butterfly");
  A.setOption("topMod","Butterfly");

  const std::map<std::string,std::string> beamDef=
    {
      {"NMX","G4BLine17"},
      {"DREAM","G4BLine14"},
      {"CSPEC","G4BLine13"},
      {"VOR","G4BLine7"},   // also 17  
      {"LOKI","G4BLine4"},
      {"ODIN","G4BLine19"}
    };     
  const std::set<std::string> beamFilled=
    {"NMX","CSPEC","DREAM","VOR","LOKI"};

  size_t index(0);
  std::map<std::string,std::string>::const_iterator mc;
  for(mc=beamDef.begin();mc!=beamDef.end();mc++)
    {
      A.setMultiOption("beamlines",index,mc->second+" "+mc->first);
      A.setVar(mc->second+"Active",1);
      if (beamFilled.find(mc->first)!=beamFilled.end())
        A.setVar(mc->second+"Filled",1);
      index++;
    }
   
  return;
}

  
} // NAMESPACE mainSystem
