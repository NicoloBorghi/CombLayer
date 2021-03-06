/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/insertSphere.h
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
#ifndef constructSystem_insertSphere_h
#define constructSystem_insertSphere_h

class Simulation;

namespace constructSystem
{
/*!
  \class insertSphere
  \version 1.0
  \author S. Ansell
  \date June 2016
  \brief Spherical insert object
  
  Designed to be a quick spher to put an object into a model
  for fluxes/tallies etc
*/

class insertSphere : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:
  
  const int ptIndex;             ///< Index of surface offset
  int cellIndex;                 ///< Cell index
  int populated;                 ///< externally set values

  double radius;             ///< Full Width

  int defMat;               ///< Material

  void populate(const FuncDataBase&);
  void createUnitVector(const Geometry::Vec3D&,
			const attachSystem::FixedComp&);

  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  void createUnitVector(const Geometry::Vec3D&,
			const Geometry::Vec3D&,
			const Geometry::Vec3D&,
			const Geometry::Vec3D&);


  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  void findObjects(const Simulation&);

  void mainAll(Simulation&);

 public:

  insertSphere(const std::string&);
  insertSphere(const insertSphere&);
  insertSphere& operator=(const insertSphere&);
  ~insertSphere();

  void setValues(const double,const int);
  void createAll(Simulation&,const Geometry::Vec3D&,
		 const attachSystem::FixedComp&);

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
