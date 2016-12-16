/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   sourceInc/PointSource.h
 *
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
#ifndef SDef_PointSource_h
#define SDef_PointSource_h

namespace SDef
{
  class Source;
}

namespace SDef
{

/*!
  \class PointSource
  \version 1.0
  \author S. Ansell
  \date September 2015
  \brief Ponit source
*/

class PointSource : 
  public attachSystem::FixedOffset
{
 private:
  
  
  int particleType;             ///< Particle Type
  double cutEnergy;             ///< Energy cut point
  double angleSpread;           ///< Angle spread
  double weight;                ///< Particle weight

  std::vector<double> Energy;   ///< Energies [MeV]
  std::vector<double> EWeight;  ///< Weights
  
  void populate(const FuncDataBase& Control);
  int populateEnergy(std::string,std::string);
  int populateEFile(const std::string&,const int,const int);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSource(SDef::Source&) const;

 public:

  PointSource(const std::string&);
  PointSource(const PointSource&);
  PointSource& operator=(const PointSource&);
  ~PointSource();

  /// set default distance
  void setDefaultStep(const Geometry::Vec3D& D)
  { xStep=D[0]; yStep=D[1], zStep=D[2]; }
  /// Set cut energy
  void setCutEnergy(const double E) { cutEnergy=E; }


  void createAll(const FuncDataBase&,SDef::Source&);
  void createAll(const FuncDataBase&,const attachSystem::FixedComp&,
		 const long int,SDef::Source&);
  
};

}

#endif
 
