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

namespace simplebeamSystem
{
  

class simplebeamTube : public attachSystem::ContainedComp,
   public attachSystem::FixedComp,public attachSystem::CellMap
{
 private:
 
   const int tubeIndex;
   int cellIndex;

   double xStep;
   double yStep;
   double zStep;

   double xyAngle;
   double zAngle;

   double length;
   double height; 
   double width;

   double innerHeight;
   double innerWidth;

   int wallMat;
   size_t nLayers;

   std::vector<double> lenFrac;

 
   void populate(const FuncDataBase&);
   void createUnitVector(const attachSystem::FixedComp&,const long);
   void createSurfaces(const long,const attachSystem::FixedComp&);
   void createObjects(Simulation&,const long,const attachSystem::FixedComp&);
   void createLinks();
   void layerProcess(Simulation&);


 public:

  simplebeamTube(const std::string&);
  simplebeamTube(const simplebeamTube&);
  simplebeamTube& operator=(const simplebeamTube&);
  ~simplebeamTube();

  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);
 
};
 
}

#endif
