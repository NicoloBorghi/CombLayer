/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuildInc/IradCylinder.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef essSystem_IradCylinder_h
#define essSystem_IradCylinder_h

class Simulation;

namespace essSystem
{
  class BlockAddition;
/*!
  \class IradCylinder
  \author S. Ansell
  \version 1.0
  \date October 2012
  \brief Specialized for a cylinder moderator
*/

class IradCylinder : public attachSystem::ContainedGroup,
    public attachSystem::LayerComp,
    public attachSystem::FixedComp
{
 private:
  
  const int modIndex;             ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  /// Extension object
  std::shared_ptr<BlockAddition> ExtAObj; 
  std::shared_ptr<BlockAddition> ExtBObj; 
  int blockActiveA;
  int blockActiveB;
  size_t aSide;  
  size_t bSide;

  double innerRadius;             ///< Radius from inner cell
  double innerHeight;             ///< height from inner cell
  double innerDepth;              ///< Depth from inner cell

  std::vector<double> radius;         ///< cylinder radii
  std::vector<double> height;         ///< Full heights
  std::vector<double> depth;          ///< full depths
  std::vector<int> mat;               ///< Materials
  std::vector<double> temp;           ///< Temperatures

  std::vector<Geometry::Vec3D> viewY; ///< Direction from centre
  std::vector<double> viewAngle;      ///< Angle from centre
  std::vector<double> viewOpenAngle;  ///< Angle opening
  std::vector<double> viewHeight;     ///< Height from centre
  std::vector<double> viewWidth;      ///< Width at intercept

  // Now calculated
  std::vector<Geometry::Vec3D> FLpts;   ///< Flight line corner 
  std::vector<Geometry::Vec3D> FLunit;  ///< Flight line direction  [-x,x,-z,z]
  std::vector<int> layerCells;          ///< Layered cells
  // Functions:  
  void checkItems(const attachSystem::FixedComp&);
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&,const attachSystem::ContainedComp*);
  void createLinks();
  
  void updateLayers(Simulation&,const char,
		    const size_t,const size_t) const;

  Geometry::Vec3D calcViewIntercept(const size_t,const size_t) const;

 public:

  IradCylinder(const std::string&);
  IradCylinder(const IradCylinder&);
  IradCylinder& operator=(const IradCylinder&);
  virtual ~IradCylinder();

  void createAll(Simulation&,const attachSystem::FixedComp&);

    

};

}

#endif
 
