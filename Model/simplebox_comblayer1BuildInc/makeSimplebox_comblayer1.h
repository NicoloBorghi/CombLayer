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
#ifndef simplebox_comblayer1System_makeSimplebox_comblayer1_h
#define simplebox_comblayer1System_makeSimplebox_comblayer1_h

namespace constructSystem
{  
  class DiskChopper;
}


/*!
  \namespace simplebox_comblayer1System
  \brief simple simplebox_comblayer1modl
  \version 1.0
  \date May 2015
  \author S. Ansell
*/

namespace simplebox_comblayer1System
{
  class simplebox_comblayer1Tube;
  
  /*!
    \class makeSimplebox_comblayer
    \version 1.0
    \author S. Ansell
    \date May 2015
    \brief General simplebox_comblayer1 building system
  */


class makeSimplebox_comblayer1
{
 private:

  std::shared_ptr<simplebox_comblayer1System::simplebox_comblayer1Tube> TubeObj;   ///< Main tube
  std::shared_ptr<simplebox_comblayer1System::simplebox_comblayer1Tube> BoxObj; 
  std::shared_ptr<simplebox_comblayer1System::simplebox_comblayer1Tube> GreenObj;
  std::shared_ptr<constructSystem::DiskChopper> ChopObj;
 
 public:
  
  makeSimplebox_comblayer1();
  makeSimplebox_comblayer1(const makeSimplebox_comblayer1&);
  makeSimplebox_comblayer1& operator=(const makeSimplebox_comblayer1&);
  ~makeSimplebox_comblayer1();
  
  void build(Simulation*,const mainSystem::inputParam&);
  void build(Simulation*);
};
 
}

#endif
