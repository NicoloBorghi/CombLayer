/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/makeESS.h
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
#ifndef essSystem_makeESS_h
#define essSystem_makeESS_h


namespace beamlineSystem
{
  class beamlineConstructor;
}

namespace constructSystem
{
  class ModBase;
  class SupplyPipe;

}

namespace moderatorSystem
{
  class BasicFlightLine;
  class FlightLine;
}

/*!
  \namespace essSystem
  \brief General ESS stuff
  \version 1.0
  \date January 2013
  \author S. Ansell
*/

namespace essSystem
{
  class WheelBase;
  class Wheel;
  class BilbaoWheel;
  class BeRef;
  class essMod;
  class ConicModerator;
  class CylPreMod;
  class IradCylinder;
  class BulkModule;
  class ShutterBay;
  class ProtonTube;
  class GuideBay;
  class BeamMonitor;
  class DiskPreMod;
  class Bunker;
  class RoofPillars;
  class Curtain;
  class F5Collimator;
  class ODIN;
  class LOKI;
  class VOR;
  class BunkerFeed;
  /*!
    \class makeESS
    \version 1.0
    \author S. Ansell
    \date January 2013
    \brief Main moderator system for ESS
  */
  
class makeESS
{
 private:
  
  std::shared_ptr<WheelBase> Target;   ///< target object
  std::shared_ptr<BeRef> Reflector;    ///< reflector object
  std::shared_ptr<ProtonTube> PBeam;   ///< Proton Void
  std::shared_ptr<BeamMonitor> BMon;   ///< Beam Monitor

  // Butterly
  /// Primary Lower Mod 
  std::shared_ptr<constructSystem::ModBase> LowMod;
  std::shared_ptr<DiskPreMod> LowPreMod;         ///< Lower mod 
  std::shared_ptr<DiskPreMod> LowCapMod;         ///< Upper mod

  std::shared_ptr<moderatorSystem::BasicFlightLine> LowAFL;  ///< Lower Mode FL
  std::shared_ptr<moderatorSystem::BasicFlightLine> LowBFL;  ///< Lower Mode FL
  std::shared_ptr<CylPreMod> LowPre;          ///< Lower Mod (Pre)

  /// Lower supply 
  std::shared_ptr<constructSystem::SupplyPipe> LowSupplyPipe; 
  std::shared_ptr<constructSystem::SupplyPipe> LowReturnPipe;  ///< Lower supply

  // Butterly
  /// Primary Upper Mod 
  std::shared_ptr<constructSystem::ModBase> TopMod;
  std::shared_ptr<DiskPreMod> TopPreMod;         ///< Top mod 
  std::shared_ptr<DiskPreMod> TopCapMod;         ///< Lower mod


  
  std::shared_ptr<moderatorSystem::BasicFlightLine> TopAFL;  ///< Top Mode FL
  std::shared_ptr<moderatorSystem::BasicFlightLine> TopBFL;  ///< Top Mode FL
  std::shared_ptr<CylPreMod> TopPre;          ///< Toper Mod (Pre)

  /// Top supply 

  std::shared_ptr<constructSystem::SupplyPipe> TopSupplyPipe; 
  std::shared_ptr<constructSystem::SupplyPipe> TopReturnPipe;  ///< Top supply

  std::shared_ptr<BulkModule> Bulk;      ///< Main bulk module
  std::shared_ptr<moderatorSystem::FlightLine> BulkLowAFL;  ///< Lower Mode FL

  /// Shutterbay objects
  std::shared_ptr<ShutterBay> ShutterBayObj;  
  /// Array of Guidies
  std::vector<std::shared_ptr<GuideBay> > GBArray;  

  std::shared_ptr<Bunker> ABunker;  ///< Right bunker [A unit]
  std::shared_ptr<Bunker> BBunker;  ///< Left bunker [B unit]
  std::shared_ptr<Bunker> CBunker;  ///< Opposite Right bunker [A unit]
  std::shared_ptr<Bunker> DBunker;  ///< Opposite bunker [B unit]
  /// A bunker freed thorugh
  std::vector<std::shared_ptr<BunkerFeed> > bFeedArray;

  ///< Right bunker Pillars [A]
  std::shared_ptr<RoofPillars> ABunkerPillars;
  ///< Right bunker Pillars [B]
  std::shared_ptr<RoofPillars> BBunkerPillars; 
  std::shared_ptr<Curtain> TopCurtain;  ///< Conc-curtain

  /// collimators for F5 tallies
  std::vector<std::shared_ptr<F5Collimator>> F5array; 
  
  void topFlightLines(Simulation&);
  void lowFlightLines(Simulation&);
  void createGuides(Simulation&);

  void buildLowButterfly(Simulation&);
  void buildLowPreMod(Simulation&);
  void buildIradComponent(Simulation&,const mainSystem::inputParam&);


  void buildTopButterfly(Simulation&);
  void buildLowerPipe(Simulation&,const std::string&);

  void buildTopPreMod(Simulation&);
  void buildToperPipe(Simulation&,const std::string&);

  void makeTarget(Simulation&,const std::string&);
  void makeBunker(Simulation&,const mainSystem::inputParam&);
  
  void makeBeamLine(Simulation&,
		    const mainSystem::inputParam&);

  void buildPillars(Simulation&);
  void buildBunkerFeedThrough(Simulation&,
			      const mainSystem::inputParam&);


  void buildF5Collimator(Simulation&, size_t);

  void optionSummary(Simulation&);

 public:
  
  makeESS();
  makeESS(const makeESS&);
  makeESS& operator=(const makeESS&);
  ~makeESS();
  
  void build(Simulation&,const mainSystem::inputParam&);

};

}

#endif
