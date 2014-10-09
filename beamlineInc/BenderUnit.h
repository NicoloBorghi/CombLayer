/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   beamlineInc/BenderUnit.h
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
#ifndef beamlineSystem_BenderUnit_h
#define beamlineSystem_BenderUnit_h


namespace beamlineSystem
{

/*!
  \class BenderUnit
  \version 1.0
  \author S. Ansell
  \date September 2014
  \brief Points associated with tracked beamline bender
*/

class BenderUnit : public ShapeUnit
{
 private:

  Geometry::Vec3D RCent;      ///< Rotation centre
  Geometry::Vec3D RAxis;      ///< Rotation axis

  double Radius;              ///< Primary rotation ratius
  double aHeight;             ///< Height across rotation plane [start]
  double bHeight;             ///< Across rotation plane [end]
  double aWidth;              ///< In rotation plane [start]
  double bWidth;              ///< In rotation plane [end]
  double Length;              ///< Length of section
  double rotAng;              ///< Rotation of Z bend axis for bend

  Geometry::Vec3D AXVec;    ///< Current XVector
  Geometry::Vec3D AYVec;    ///< Current YVector
  Geometry::Vec3D AZVec;    ///< Current ZVector

  Geometry::Vec3D BXVec;    ///< Current XVector
  Geometry::Vec3D BYVec;    ///< Current YVector
  Geometry::Vec3D BZVec;    ///< Current ZVector

  
 public:

  BenderUnit(const int,const int);
  BenderUnit(const BenderUnit&);
  BenderUnit& operator=(const BenderUnit&);
  virtual BenderUnit* clone() const;
  virtual ~BenderUnit();

  void setValues(const double,const double,const double,
		 const double,const double);
  void setValues(const double,const double,const double,
		 const double,const double,const double,
		 const double);
  void setOriginAxis(const Geometry::Vec3D&,const Geometry::Vec3D&,
		     const Geometry::Vec3D&,const Geometry::Vec3D&);

  // Accessor to beg axis
  Geometry::Vec3D getBegAxis() const { return AYVec; }
  // Accessor to end axis
  Geometry::Vec3D getEndAxis() const { return BYVec; }

  std::string getString(const size_t) const;
  std::string getExclude(const size_t) const;

  void createSurfaces(ModelSupport::surfRegister&,const int,
		      const std::vector<double>&);
};

}

#endif
 
