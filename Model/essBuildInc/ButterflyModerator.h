/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/ButterflyModerator.h
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
#ifndef essSystem_ButterflyModerator_h
#define essSystem_ButterflyModerator_h

class Simulation;

namespace essSystem
{
  class MidWaterDivider;
  class EdgeWater;

/*!
  \class ButterflyModerator
  \author Stuart Ansell 
  \version 1.0
  \date April 2015
  \brief Butterfly moderator object [composite for mutli-system]
     
  Implementation based on K. Bat concept
*/

class ButterflyModerator :
  public constructSystem::ModBase
{
 private:

  const int flyIndex;        ///< Index of surface offset
  int cellIndex;             ///< Cell index
  
  std::shared_ptr<H2Wing> LeftUnit;        ///< Left part of the moderator
  std::shared_ptr<H2Wing> RightUnit;       ///< Right part of the moderator
  std::shared_ptr<MidWaterDivider> MidWater;    ///< Water divider
  std::shared_ptr<EdgeWater> LeftWater;    ///< Water divider
  std::shared_ptr<EdgeWater> RightWater;    ///< Water divider

  double totalHeight;                     ///< Total height
  double outerRadius;                     ///< Main outer radius

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const attachSystem::FixedComp*,
			const long int);

  void createExternal();
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:
  
  ButterflyModerator(const std::string&);
  ButterflyModerator(const ButterflyModerator&);
  ButterflyModerator& operator=(const ButterflyModerator&);
  virtual ButterflyModerator* clone() const;
  virtual ~ButterflyModerator();
  
  virtual Geometry::Vec3D getSurfacePoint(const size_t,const long int) const;
  virtual int getLayerSurf(const size_t,const long int) const;
  virtual std::string getLayerString(const size_t,const long int) const;
  virtual int getCommonSurf(const size_t) const;

  /// Accessor to radius
  void setRadiusX(const double R) { outerRadius=R; }
  virtual const attachSystem::FixedComp&
    getComponent(const std::string&) const;

  std::string getSideRule() const;
  std::string getLeftRightWaterSideRule() const;
  Geometry::Vec3D getFocalPoint(const long int) const;
  std::vector<Geometry::Vec3D> getFocalPoints() const;

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const attachSystem::FixedComp*,
		 const long int);
};

}

#endif
 
