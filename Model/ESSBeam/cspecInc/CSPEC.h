/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/CSPEC.h
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
#ifndef essSystem_CSPEC_h
#define essSystem_CSPEC_h

namespace attachSystem
{
  class FixedComp;
  class TwinComp;
  class CellMap;
}

namespace instrumentSystem
{
  class CylSample;
}

namespace constructSystem
{  
  class ChopperPit;
  class DiskChopper;
  class Jaws;
  class LineShield;
  class RotaryCollimator;
  class VacuumBox;
  class VacuumPipe;
  class VacuumWindow;
  class ChopperHousing;
  class ChopperUnit;
}

namespace essSystem
{  
  class GuideItem;
  class CspecHut;
  class DetectorTank;

  /*!
    \class CSPEC
    \version 1.0
    \author S. Ansell
    \date September 2015
    \brief CSPEC beamline constructor for the ESS
  */
  
class CSPEC : public attachSystem::CopiedComp
{
 private:

  /// Stop at [0:Complete / 1:Mono Wall / 2:Inner Bunker / 3:Outer Bunker ]
  int stopPoint;  

  /// Main Beam Axis [for construction]
  std::shared_ptr<attachSystem::FixedComp> cspecAxis;

  /// Elliptic focus in bulkshield [m5]
  std::shared_ptr<beamlineSystem::GuideLine> FocusA;

  // Vac pipe in 
  std::shared_ptr<constructSystem::VacuumPipe> VPipeA;
  /// Elliptic guide from 5.5 to 6metre
  std::shared_ptr<beamlineSystem::GuideLine> FocusB;

  // Vac pipe in 
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;
  /// Elliptic guide from 5.5 to 6metre
  std::shared_ptr<beamlineSystem::GuideLine> BendB;

  void setBeamAxis(const GuideItem&,const bool);
  
 public:
  
  CSPEC(const std::string&);
  CSPEC(const CSPEC&);
  CSPEC& operator=(const CSPEC&);
  ~CSPEC();
  
  void build(Simulation&,const GuideItem&,
	     const Bunker&,const int);

};

}

#endif
