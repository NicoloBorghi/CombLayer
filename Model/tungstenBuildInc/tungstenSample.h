/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   tungstenBuildInc/tungstenSample.h
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
#ifndef tungstenSystem_tungstenSample_h
#define tungstenSystem_tungstenSample_h

class Simulation;

namespace tungstenSystem {

        /*!
                \class tungstenSample
                \author N. Borghi
                \version 1.0
                \date April 2017
                \brief Simple model of ESS tungsten bricks irradiated at ISOLDE and analyzed at DTU
        */

        class tungstenSample : public attachSystem::ContainedComp,
                               public attachSystem::FixedOffset,
                               public attachSystem::CellMap {

                private: // Variables
  
                        const int sampleIndex;            ///< Index of surface offset
                        int cellIndex;                    ///< Cell index

                        doulbe sampleMaterial1;
                        doulbe sampleMaterial2;
                        doulbe sampleMaterial3;

                private: // Methods

                        void populate(const FuncDataBase&);
                        void createUnitVector(const attachSystem::FixedComp&,const long int);
                        void createSurfaces();
                        void createObjects(Simulation&);
                        void createLinks();

                public: // Methods

                        tungstenSample(const std::string&);
                        tungstenSample(const tungstenSample&);
                        tungstenSample& operator=(const tungstenSample&);
                       ~tungstenSample();

                        void createAll(Simulation&,const attachSystem::FixedComp&, const long int);
    
        };

}

#endif
 
