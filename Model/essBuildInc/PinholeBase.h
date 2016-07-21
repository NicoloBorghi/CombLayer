/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuildInc/PinholeBase.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef essSystem_PinholeBase_h
#define essSystem_PinholeBase_h

class Simulation;

namespace essSystem
{

/*!
  \class PinholeBase
  \author N. Borghi
  \version 1.0
  \date July 2016
  \brief Pinhole collimator base class
*/

class PinholeBase : public attachSystem::ContainedGroup,
    public attachSystem::ContainedComp,
    public attachSystem::FixedComp
    public attachSystem::CellMap
{
 protected:
  
  const int pnholeIndex;             ///< Index of surface offset
  int cellIndex;                    ///< Cell index

  public:

  PinholeBase(const std::string&);
  PinholeBase(const PinholeBase&);
  PinholeBase& operator=(const PinholeBase&);
  virtual PinholeBase* clone() const =0;
  virtual ~PinholeBase();

  virtual double wheelHeight() const =0;
  
  //  virtual int getCell() const =0;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&) =0;
  
};

}

#endif






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

  int DPfloorSurfaceNumber;
  int DProofSurfaceNumber;

