/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/makeESS.h
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
  class TaperedFlightLine;
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
  class PreModWing;
  class BulkModule;
  class TwisterModule;
  class DiagnosticPlug;
  class ShutterBay;
  class ProtonTube;
  class GuideBay;
  class BeamMonitor;
  class TaperedDiskPreMod;
  class Bunker;
  class RoofPillars;
  class Curtain;
  class F5Collimator;
  class ODIN;
  class LOKI;
  class VOR;
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
  std::shared_ptr<TaperedDiskPreMod> LowPreMod;         ///< Lower mod 
  std::shared_ptr<TaperedDiskPreMod> LowCapMod;         ///< Upper mod

  std::shared_ptr<moderatorSystem::TaperedFlightLine> LowAFL;  ///< Lower Mode FL
  std::shared_ptr<moderatorSystem::TaperedFlightLine> LowBFL;  ///< Lower Mode FL
  std::shared_ptr<CylPreMod> LowPre;          ///< Lower Mod (Pre)

  // Butterly
  /// Primary Upper Mod 
  std::shared_ptr<constructSystem::ModBase> TopMod;
  std::shared_ptr<TaperedDiskPreMod> TopPreMod;         ///< Top mod 
  std::shared_ptr<TaperedDiskPreMod> TopCapMod;         ///< Lower mod

  std::shared_ptr<moderatorSystem::TaperedFlightLine> TopAFL;  ///< Top Mode FL
  std::shared_ptr<moderatorSystem::TaperedFlightLine> TopBFL;  ///< Top Mode FL
  std::shared_ptr<CylPreMod> TopPre;          ///< Toper Mod (Pre)

  std::shared_ptr<PreModWing> LowPreWing; ///< Low premoderator wing
  std::shared_ptr<PreModWing> TopPreWing; ///< Top premoderator wing
  std::shared_ptr<PreModWing> LowCapWing; ///< Low cap premoderator wing
  std::shared_ptr<PreModWing> TopCapWing; ///< Top cap premoderator wing

  /// Pipes
  std::shared_ptr<constructSystem::SupplyPipe> TopSupplyLeftAl,  TopSupplyLeftConnect,  TopSupplyLeftInvar;
  std::shared_ptr<constructSystem::SupplyPipe> TopReturnLeftAl,  TopReturnLeftConnect,  TopReturnLeftInvar;

  std::shared_ptr<constructSystem::SupplyPipe> TopSupplyRightAl, TopSupplyRightConnect, TopSupplyRightInvar;
  std::shared_ptr<constructSystem::SupplyPipe> TopReturnRightAl, TopReturnRightConnect, TopReturnRightInvar;

  std::shared_ptr<constructSystem::SupplyPipe> LowSupplyLeftAl,  LowSupplyLeftConnect,  LowSupplyLeftInvar;
  std::shared_ptr<constructSystem::SupplyPipe> LowReturnLeftAl,  LowReturnLeftConnect,  LowReturnLeftInvar;

  std::shared_ptr<constructSystem::SupplyPipe> LowSupplyRightAl, LowSupplyRightConnect, LowSupplyRightInvar;
  std::shared_ptr<constructSystem::SupplyPipe> LowReturnRightAl, LowReturnRightConnect, LowReturnRightInvar;

  std::shared_ptr<BulkModule> Bulk;      ///< Main bulk module
  std::shared_ptr<TwisterModule> Twister;
  std::shared_ptr<DiagnosticPlug> DPlug; ///< Diagnostic plug

  std::shared_ptr<moderatorSystem::FlightLine> BulkLowAFL;  ///< Lower Mode FL

  /// Shutterbay objects
  std::shared_ptr<ShutterBay> ShutterBayObj;  
  /// Array of Guidies
  std::vector<std::shared_ptr<GuideBay> > GBArray;  

  /// Array of beamlines constructors:
  std::vector<std::shared_ptr<beamlineSystem::beamlineConstructor> > 
    BLArray;  

  std::shared_ptr<Bunker> ABunker;  ///< Right bunker [A unit]
  std::shared_ptr<Bunker> BBunker;  ///< Right bunker [B unit]
  ///< Right bunker Pillars [A]
  std::shared_ptr<RoofPillars> ABunkerPillars;
  ///< Right bunker Pillars [B]
  std::shared_ptr<RoofPillars> BBunkerPillars; 
  std::shared_ptr<Curtain> TopCurtain;  ///< Conc-curtain

  std::vector<std::shared_ptr<F5Collimator>> F5array; ///< collimators for F5 tallies
  std::vector<Geometry::Vec3D> TopFocalPoints; ///< top moderator focal points
  std::vector<Geometry::Vec3D> LowFocalPoints; ///< low moderator focal points
 
  void topFlightLines(Simulation&);
  void lowFlightLines(Simulation&);
  void createGuides(Simulation&);

  void buildLowButterfly(Simulation&);
  void buildLowPreMod(Simulation&);

  void buildTopButterfly(Simulation&);
  void buildH2Pipe(Simulation&, std::string, std::string, std::string,
		   std::shared_ptr<constructSystem::SupplyPipe>,
		   std::shared_ptr<constructSystem::SupplyPipe>,
		   std::shared_ptr<constructSystem::SupplyPipe>);
  void buildTopPipes(Simulation&,const std::string&);
  void buildLowPipes(Simulation&,const std::string&);

  void makeTarget(Simulation&,const std::string&);
  void makeBunker(Simulation&,const std::string&);
  
  void makeBeamLine(Simulation&,
		    const mainSystem::inputParam&);


  void buildF5Collimator(Simulation&, size_t); // when -nF5 is used
  void buildF5Collimator(Simulation&, const mainSystem::inputParam&); // when -TopCC is used

  void buildPreWings(Simulation&);
  void buildTwister(Simulation&);
  void buildDiagnosticPlug(Simulation&, const std::string&);

  void buildPillars(Simulation&);

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
