/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   sourceInc/ActivationSource.h
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
#ifndef SDef_ActivationSource_h
#define SDef_ActivationSource_h

namespace SDef
{
  class Source;
}

namespace SDef
{

/*!
  \class ActivationSource
  \version 1.0
  \author S. Ansell
  \date September 2016
  \brief Creates an active projection source
*/

class ActivationSource 
{
 private:

  size_t timeStep;                ///< Time step from cinder
  size_t nPoints;                 ///< Number of points
  size_t nTotal;                  ///< Total points
   
  Geometry::Vec3D ABoxPt;         ///< Bounding box corner
  Geometry::Vec3D BBoxPt;         ///< Bounding box corner

  std::map<int,double> volCorrection;   ///< cell / volume
  std::map<int,activeUnit> cellFlux;    ///< cell[active] : flux data
  std::vector<Geometry::Vec3D> fluxPt;  ///< Flux emmision points
  std::vector<int> cellID;              ///< cell ID for points

  

  void createVolumeCount();
  void createFluxVolumes(const Simulation&);
  void readFluxes(const std::string&);
  void processFluxFiles(const std::vector<std::string>&,
			const std::vector<int>&);


  void writePoints(const std::string&) const;
  
 public:

  ActivationSource();
  ActivationSource(const ActivationSource&);
  ActivationSource& operator=(const ActivationSource&);
  ~ActivationSource();

  /// Set number of output points
  void setNPoints(const size_t N) { nPoints=N; }
  /// set time segment number 
  void setTimeSegment(const size_t T) { timeStep=T+1; }
  void setBox(const Geometry::Vec3D&,const Geometry::Vec3D&);
  

  void createOutput(const std::string&);

  void createSource(Simulation&,const std::string&,const std::string&);
};

}

#endif
 
