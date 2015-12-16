/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   pipeInc/makeSimplebox_comblayer.h
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
#ifndef simplebeamSystem_makesimplebeam_h
#define simplebeamSystem_makesimplebeam_h

namespace constructSystem
{  
  class DiskChopper;
}


/*!
  \namespace simplebeamSystem
  \brief simple simplebeammodl
  \version 1.0
  \date May 2015
  \author S. Ansell
*/

namespace simplebeamSystem
{
  class simplebeamTube;
  
  /*!
    \class makeSimplebox_comblayer
    \version 1.0
    \author S. Ansell
    \date May 2015
    \brief General simplebeam building system
  */


class makesimplebeam
{
 private:

  std::shared_ptr<simplebeamSystem::simplebeamTube> TubeObj;   ///< Main tube
  std::shared_ptr<simplebeamSystem::simplebeamTube> BoxObj; 
  std::shared_ptr<simplebeamSystem::simplebeamTube> GreenObj;
  std::shared_ptr<constructSystem::DiskChopper> ChopObj;
 
 public:
  
  makesimplebeam();
  makesimplebeam(const makesimplebeam&);
  makesimplebeam& operator=(const makesimplebeam&);
  ~makesimplebeam();
  
  void build(Simulation*,const mainSystem::inputParam&);
  void build(Simulation*);
};
 
}

#endif
