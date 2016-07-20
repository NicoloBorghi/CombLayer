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

class DiagnosticPlug : public attachSystem::ContainedComp,
  public attachSystem::FixedComp,
  public attachSystem::CellMap
{
 private:

  const int tIndex;               ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  int engActive;                  ///< Engineering active flag

  double xStep;                   ///< X step
  double yStep;                   ///< Y step
  double zStep;                   ///< Z step
  double xyAngle;                 ///< XY Angle
  double zAngle;                  ///< Z Angle

  double width;                   ///< Width
  double length;                  ///< Length
  double height;                  ///< Height
  
  double shaftWallThick;          ///< Shaft wall thickness
  double shaftBearingRadius;              ///< shaft bearing radius
  double shaftBearingHeight;              ///< shaft bearing height
  double shaftBearingWallThick;      ///< shaft bearing side wall thick
  
  double plugFrameRadius;         ///< plug frame outer radius
  double plugFrameHeight;         ///< plug frame height
  double plugFrameDepth;          ///< plug frame depth
  double plugFrameAngle;          ///< plug frame opening angle
  double plugFrameWallThick;      ///< plug frame wall thickness

  int plugFrameMat;               ///< plug frame material
  int plugFrameWallMat;           ///< plug frame wall material
  int shaftMat;                   ///< shaft material
  int shaftWallMat;               ///< shaft wall material

  // Pinhole (PH) definition

  std::string PHType;             ///< The pinhole can be either "rect" (rectangular) or "circ" (circular)

  double zImagingPlane;            ///< Elevation (z coordinate) of the imaging plane (relative to the target upper surface)

  double radialPHOffset;          ///< Offset of the PH center from the center of the DP
  double radialPHPos;             ///< Radial position of the PH (Y coord.)
  double radialPHWidth;           ///< Radial width of the PH


  double transversalPHOffset;     ///< Offset of the PH center from the center of the DP
  double transversalPHPos;        ///< Transversal position of the PH (Y coord.)
  double transversalPHWidth;      ///< Transversal width of the PH

  double zDistanceFromImage;      ///< Distance of the PH from the imaging plane
  double zPHPos;                  ///< z position of the PH

  int DPfloorLinkNumber;
  int DProofLinkNumber;

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

 public:

  DiagnosticPlug(const std::string&);
  DiagnosticPlug(const DiagnosticPlug&);
  DiagnosticPlug& operator=(const DiagnosticPlug&);
  virtual DiagnosticPlug* clone() const;
  virtual ~DiagnosticPlug();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const attachSystem::FixedComp& floorFC,
		 const size_t floorLP,
		 const attachSystem::FixedComp& roofFC,
		 const size_t roofLP);
};

}

#endif
 
