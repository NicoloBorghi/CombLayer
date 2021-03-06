/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/shortNMX/shortNMX.h
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
#ifndef essSystem_shortNMX_h
#define essSystem_shortNMX_h

namespace attachSystem
{
  class FixedComp;
  class TwinComp;
  class CellMap;
}

namespace constructSystem
{  
  class Jaws;
  class DiskChopper;
  class ChopperPit;
  class RotaryCollimator;
  class VacuumBox;
  class ChopperHousing;
}

namespace essSystem
{  
  class GuideItem;
  /*!
    \class shortNMX
    \version 1.0
    \author S. Ansell
    \date July 2014
    \brief shortNMX beamline constructor for the ESS
  */
  
class shortNMX
{
 private:

  /// Main Beam Axis [for construction]
  std::shared_ptr<attachSystem::FixedOffset> nmxAxis;

  /// tapper in insert bay
  std::shared_ptr<beamlineSystem::GuideLine> GuideA;
    
  /// Pipe between bunker and the wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeA;
  /// Pipe between bunker and the wall
  std::shared_ptr<constructSystem::VacuumPipe> VPipeB;
  /// Bender in insert bay
  std::shared_ptr<beamlineSystem::GuideLine> BendA;
  /// Bunker insert
  std::shared_ptr<essSystem::BunkerInsert> BInsert;

  /// tapper in insert bay
  std::shared_ptr<beamlineSystem::GuideLine> FocusWall;

  // Piece after wall
  std::shared_ptr<constructSystem::LineShield> ShieldA;
  
  void setBeamAxis(const FuncDataBase&,
		   const GuideItem&,const bool);
  
 public:
  
  shortNMX();
  shortNMX(const shortNMX&);
  shortNMX& operator=(const shortNMX&);
  ~shortNMX();
  
  void build(Simulation&,const GuideItem&,
	     const Bunker&,const int);

};

}

#endif
