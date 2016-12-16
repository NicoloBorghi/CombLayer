/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/DREAM.h
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
#ifndef essSystem_DREAM_h
#define essSystem_DREAM_h

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
  class DreamHut;
  class DetectorTank;

  /*!
    \class DREAM
    \version 1.0
    \author S. Ansell
    \date September 2015
    \brief DREAM beamline constructor for the ESS
  */
  
class DREAM : public attachSystem::CopiedComp
{
 private:

  /// Stop at [0:Complete / 1:Mono Wall / 2:Inner Bunker / 3:Outer Bunker ]
  int stopPoint;  

  /// Main Beam Axis [for construction]
  std::shared_ptr<attachSystem::FixedOffset> dreamAxis;

  /// Elliptic focus in bulkshield [m5]
  std::shared_ptr<beamlineSystem::GuideLine> FocusA;

  /// Pipe between bunker and the wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;
  /// Elliptic guide from 5.5 to 6metre
  std::shared_ptr<beamlineSystem::GuideLine> FocusB;
  
  /// Vac box for first chopper
  std::shared_ptr<constructSystem::ChopperUnit> ChopperA;
  /// Double disk chopper
  std::shared_ptr<constructSystem::DiskChopper> DDisk;
  /// Double disk chopper
  std::shared_ptr<constructSystem::DiskChopper> SDisk;

  /// Pipe between bunker and the wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeC;
  /// Elliptic guide from 6m to band/t0 chopper
  std::shared_ptr<beamlineSystem::GuideLine> FocusC;

  /// Vac box for BandPass chopper
  std::shared_ptr<constructSystem::ChopperUnit> ChopperB;
  /// Band Chopper
  std::shared_ptr<constructSystem::DiskChopper> BandADisk;  
  /// Vac box for T0 chopper
  std::shared_ptr<constructSystem::ChopperUnit> ChopperC;
  /// TO disk chopper [part A]
  std::shared_ptr<constructSystem::DiskChopper> T0DiskA;
  /// Elliptic forcus in bulkshield [m5]
  std::shared_ptr<beamlineSystem::GuideLine> FocusT0Mid;
  /// TO disk chopper [part B]  
  std::shared_ptr<constructSystem::DiskChopper> T0DiskB;  
  /// Pipe between T0 chopper and BandA
  std::shared_ptr<constructSystem::VacuumPipe> VPipeD;
  /// Guide between diskchoppers  / T0
  std::shared_ptr<beamlineSystem::GuideLine> FocusD;
  /// Vac box for second band pass
  std::shared_ptr<constructSystem::ChopperUnit> ChopperD;
  /// Band Chopper 
  std::shared_ptr<constructSystem::DiskChopper> BandBDisk;  
  /// Vac box for second band chopper
  std::shared_ptr<constructSystem::ChopperUnit> ChopperE;
  /// TO disk chopper [part A]
  std::shared_ptr<constructSystem::DiskChopper> T1DiskA;
  /// Elliptic forcus in bulkshield [m5]
  std::shared_ptr<beamlineSystem::GuideLine> FocusT1Mid;
  /// TO disk chopper [part B]  
  std::shared_ptr<constructSystem::DiskChopper> T1DiskB;  
  /// Pipe between  T0 and bunker wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeE;
  /// Guide between T0 and bunker wall
  std::shared_ptr<beamlineSystem::GuideLine> FocusE;
  

    /// Pipe between  BandA and Band B 
  std::shared_ptr<constructSystem::VacuumPipe> VPipeF;
  /// Guide between BandA and Band B
  std::shared_ptr<beamlineSystem::GuideLine> FocusF;

  /// Bunker insert
  std::shared_ptr<essSystem::BunkerInsert> BInsert;
  /// Guide running to bunker wall
  std::shared_ptr<beamlineSystem::GuideLine> FocusWall;

  /// First outer shield section
  std::shared_ptr<constructSystem::LineShield> ShieldA;
  /// Pipe leaving bunker
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutA;
  /// Guide leaving the bunker wall
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutA;  

  /// First outer shield section
  std::shared_ptr<constructSystem::LineShield> ShieldB;
  /// Pipe leaving bunker
  std::shared_ptr<constructSystem::VacuumPipe> VPipeOutB;
  /// Guide leaving the bunker wall
  std::shared_ptr<beamlineSystem::GuideLine> FocusOutB;  

  /// Cave 
  std::shared_ptr<DreamHut> Cave;

  /// Pipe entering cave
  std::shared_ptr<constructSystem::VacuumPipe> VPipeCaveA;
  /// Guide in cave 
  std::shared_ptr<beamlineSystem::GuideLine> FocusCaveA;  

  void setBeamAxis(const GuideItem&,const bool);
  
 public:
  
  DREAM(const std::string&);
  DREAM(const DREAM&);
  DREAM& operator=(const DREAM&);
  ~DREAM();
  
  void build(Simulation&,const GuideItem&,
	     const Bunker&,const int);

};

}

#endif
