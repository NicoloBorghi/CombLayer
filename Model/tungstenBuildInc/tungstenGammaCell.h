/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   tungstenBuildInc/tungstenGammaCell.h
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
#ifndef tungstenSystem_tungstenGammaCell_h
#define tungstenSystem_tungstenGammaCell_h

class Simulation;

namespace tungstenSystem {

        /*!
                \class tungstenGammaCell
                \author N. Borghi
                \version 1.0
                \date April 2017
                \brief Simple model of DTU GammaCellfor active sample analysis
        */

        class tungstenGammaCell : public attachSystem::ContainedComp,
                                  public attachSystem::FixedOffset,
                                  public attachSystem::CellMap {

                private: // Variables
  
                        const int gammaIndex;            ///< Index of surface offset
                        int cellIndex;                   ///< Cell index

                        double leftWallThickness;
                        double rightWallThickness;
                        double frontWallThickness;
                        double backWallThickness;
                        double topWallThickness;
                        double bottomWallThickness;

                        double leftPadding;
                        double rightPadding;
                        double fronntPadding;
                        double backPadding;
                        double topPadding;
                        double bottomPadding;

                        double totalCellVolume;

                        double cellAtmosphereGas;
                        double cellAtmospherePressure;
                        double cellWallMaterial1;
                        double cellWallMaterial2;

                private: // Methods

                        void populate(const FuncDataBase&);
                        void createUnitVector(const attachSystem::FixedComp&,const long int);
                        void createSurfaces();
                        void createObjects(Simulation&);
                        void createLinks();

                public: // Methods

                        tungstenGammaCell(const std::string&);
                        tungstenGammaCell(const tungstenGammaCell&);
                        tungstenGammaCell& operator=(const tungstenGammaCell&);
                       ~tungstenGammaCell();

                        void createAll(Simulation&,const attachSystem::FixedComp&, const long int);
    
        };

}

#endif
 
