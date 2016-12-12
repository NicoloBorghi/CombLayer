/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuildInc/DiagnosticPlug.h
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
#ifndef essSystem_DiagnosticPlug_h
#define essSystem_DiagnosticPlug_h

class Simulation;

namespace essSystem
{
/*!
  \class DiagnosticPlug
  \author K. Batkov
  \version 1.0
  \date May 2016
  \brief Diagnostic plug
*/

class PinholeBase;
class StrawCollimator;
class ScintillatorBlock;

class DiagnosticPlug : public attachSystem::ContainedComp,
  public attachSystem::FixedComp,
  public attachSystem::CellMap
{
 private:

  const int tIndex;               ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  double xStep;                   ///< X step
  double yStep;                   ///< Y step
  double zStep;                   ///< Z step
  double xyAngle;                 ///< XY Angle
  double zAngle;                  ///< Z Angle

  double width;                   ///< Width
  double length;                  ///< Length
  double height;                  ///< Height

  double activationLayerThick1;   ///< Thickness of the first layer for activation calculation
  double activationLayerThick2;   ///< Thickness of the second layer for activation calculation
  double activationLayerThick3;   ///< Thickness of the third layer for activation calculation
  double activationLayerHeight;   ///< Height of the activation segments
  std::vector<double> activationZPlanes; ///< Z coordinate of the horizontal segmentation planes
  int activationWallsMaterial;    ///< Material for the wall segments

  int topShieldingMaterial;       ///< Material for the top shielding block of the DiagnosticPlug

  int floorSurfaceNumber;          ///< Diagnostic plug floor surface number
  int roofSurfaceNumber;          ///< Diagnostic plug roof surface number
  int targetTopSurfaceNumber;     //< Target top surface number
  
  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&,
                     const attachSystem::FixedComp& floorFC,
                     const size_t floorLP,
                     const attachSystem::FixedComp& roofFC,
                     const size_t roofLP);
  void createLinks();

  PinholeBase *Pinhole;   ///< Pinhole object
  StrawCollimator *Straws; ///< Straw collimator
  ScintillatorBlock *Scintillator; ///< Scintillator block

 public:

  DiagnosticPlug(const std::string&);
  DiagnosticPlug(const DiagnosticPlug&);
  DiagnosticPlug& operator=(const DiagnosticPlug&);
  virtual DiagnosticPlug* clone() const;
  virtual ~DiagnosticPlug();

  void setPinhole(PinholeBase *);
  void setStraws(StrawCollimator *);
  void setScintillators(ScintillatorBlock *);

  void createAll(Simulation&,
                 const attachSystem::FixedComp&,
                 const attachSystem::FixedComp& floorFC,
                 const size_t floorLP,
                 const attachSystem::FixedComp& roofFC,
                 const size_t roofLP);
};

}

#endif
 
