/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/GuideBay.h
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
#ifndef essSystem_GuideBay_h
#define essSystem_GuideBay_h

class Simulation;

namespace essSystem
{
  class GuideItem;

/*!
  \class GuideBay
  \version 1.0
  \author S. Ansell
  \date November 2013
  \brief Groupd of guide objects sharing a common sector
*/


class GuideBay : public attachSystem::ContainedGroup,
  public attachSystem::FixedComp,public attachSystem::CellMap
{
 private:
  
  const std::string baseKey;    ///< Base key
  const size_t bayNumber;       ///< Bay number [id]
  const int bayIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index
  
  double xStep;                 ///< Offset on X to Target
  double yStep;                 ///< Offset on Y to Target [+ve forward]
  double zStep;                 ///< Offset on Z top Target
  double xyAngle;               ///< Angle of master XY rotation
  double zAngle;                ///< Angle of master Z rotation

  double viewAngle;     ///< Angle of guide
  double innerHeight;   ///< height
  double innerDepth;    ///< Depth 
  double height;        ///< height
  double depth;         ///< Depth 
  double midRadius;     ///< Mid divide
  int mat;              ///< Material
  size_t nItems;        ///< Number of guide items
  
  int innerCyl;        ///< Inner Cylinder
  int outerCyl;        ///< Outer Cylinder 

  /// Guide units
  std::vector<std::shared_ptr<GuideItem> > GUnit;

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&);

  
 public:

  GuideBay(const std::string&,const size_t);
  GuideBay(const GuideBay&);
  GuideBay& operator=(const GuideBay&);
  virtual ~GuideBay();

  /// access to cylinder
  void setCylBoundary(const int A,const int B) 
      { innerCyl=abs(A); outerCyl=abs(B); } 

  void outerMerge(Simulation&,GuideBay&);
  void createGuideItems(Simulation&,const attachSystem::FixedComp&,
			const long int,const long int);
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 

