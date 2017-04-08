/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   tungstenBuildInc/makeTungsten.h
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
#ifndef tungstenSystem_makeTungsten_h
#define tungstenSystem_makeTungsten_h


/*!
  \namespace tungstenSystem
  \brief simple tungsten model
  \version 1.0
  \date April 2017
  \author N. Borghi
*/

namespace tungstenSystem {

        class tungstenGammaCell;
        class tungstenSample;

        /*!
                \class makeTungsten
                \version 1.0
                \author N. Borghi
                \date April 2017
                \brief Irradiation of ESS tungsten bricks at ISOLDE
        */

        class makeTungsten {

                private:

                        std::shared_ptr<tungstenSystem::tungstenGammaCell> GammaCell;   ///< DTU Gamma Cell
                        std::shared_ptr<tungstenSystem::tungstenSample>    Sample;      ///< Sample


                public:
  
                        makeTungsten();
                        makeTungsten(const makeTungsten&);
                        makeTungsten& operator=(const makeTungsten&);
                        ~makeTungsten();
  
                        void build(Simulation*,const mainSystem::inputParam&);

        };

}

#endif


