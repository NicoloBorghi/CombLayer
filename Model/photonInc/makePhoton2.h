/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photonInc/makePhoton2.h
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
#ifndef photonSystem_makePhoton2_h
#define photonSystem_makePhoton2_h

namespace constructSystem
{
  class SupplyPipe;
  class insertPlate;
  class insertSphere;
}

/*!
  \namespace photonSystem
  \brief General Laser moderator system
  \version 1.0
  \date January 2015
  \author S. Ansell
*/

namespace photonSystem
{
  class CylLayer;
  class CylContainer;
  class TubeMod;
  class B4CCollimator;
  class PlateMod;
  class EQDetector;
   
  /*!
    \class makePhoton2
    \version 1.0
    \author S. Ansell
    \date June 2016
    \brief General constructor for the photon system
  */

class makePhoton2
{
 private:

  std::shared_ptr<PlateMod> PModObj;        ///< Initial moderator
  std::shared_ptr<constructSystem::insertPlate> DetPlate;
  std::shared_ptr<constructSystem::insertSphere> DetHold;

 public:
  
  makePhoton2();
  makePhoton2(const makePhoton2&);
  makePhoton2& operator=(const makePhoton2&);
  ~makePhoton2();
  
  void build(Simulation*,const mainSystem::inputParam&);

};

}

#endif
