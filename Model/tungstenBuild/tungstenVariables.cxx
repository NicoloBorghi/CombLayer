/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   tungstenBuild/tungstenVariables.cxx
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "variableSetup.h"

namespace setVariable {

        void TungstenVariables(FuncDataBase& Control) {
        /*!
                Create all the beamline variabes
                \param Control :: DataBase
        */

                ELog::RegMethod RegA("tungstenVariables[F]","TungstenVariables");

                // GammaCell

                Control.addVariable("GammaCellLeftWallThickness",50.0);       ///< Left wall thickness
                Control.addVariable("GammaCellRightWallThickness",50.0);      ///< Right wall thickness
                Control.addVariable("GammaCellFrontWallThickness",50.0);      ///< Front wall thickness
                Control.addVariable("GammaCellBackWallThickness",50.0);       ///< Back wall thickness
                Control.addVariable("GammaCellTopWallThickness",50.0);        ///< Top wall thickness
                Control.addVariable("GammaCellBottomWallThickness",50.0);     ///< Bottom wall thickness
                Control.addVariable("GammaCellInnerWidth",18.0);              ///< Cell width (Y)
                Control.addVariable("GammaCellInnerHeight",13.0);             ///< Cell height (Z)
                Control.addVariable("GammaCellInnerDepth",8.55);              ///< Cell depth (X)
                Control.addVariable("GammaCellTotalVolume",2.0);              ///< Total cell inner volume
                Control.addVariable("GammaCellAtmosphereGas","Void");         ///< Gas for internal atmosphere
                Control.addVariable("GammaCellAtmospherePressure",2.0E-3);    ///< Pressure in bar
                Control.addVariable("GammaCellWallMaterial1","Void");         ///< Material for the walls
                Control.addVariable("GammaCellWallMaterial2","Void");         ///< Material for front lid (may be different or absent)

                // Tungsten sample

                Control.addVariable("SampleBrickWidth",8.0);                  ///< Sample width (Y)
                Control.addVariable("SampleBrickHeight",3.0);                 ///< Sample height (Z)
                Control.addVariable("SampleBrickDepth",1.0);                  ///< Sample depth (X)

                Control.addVariable("SampleLayerWidth",8.0);                  ///< Sample width (Y)
                Control.addVariable("SampleLayerHeight",3.0);                 ///< Sample height (Z)
                Control.addVariable("SampleLayerDepth",0.05);                 ///< Sample depth (X)

                Control.addVariable("SampleLeftPadding",5.0);                 ///< Left padding
                Control.addVariable("SampleRightPadding",5.0);                ///< Right padding
                Control.addVariable("SampleFrontPadding",5.0);                ///< Front padding
                Control.addVariable("SampleBackPadding",5.0);                 ///< Back padding
                Control.addVariable("SampleTopPadding",5.0);                  ///< Top padding
                Control.addVariable("SampleBottomPadding",5.0);               ///< Bottom padding
                Control.addVariable("SampleMaterial1","Tungsten600K");        ///< Brick material (ESS Tungsten)
                Control.addVariable("SampleMaterial2",0.0);
                Control.addVariable("SampleMaterial3",0.0);

                // General parameters

                Control.addVariable("ProtonEnergy",0.0);

                return;
        }

}  // NAMESPACE setVariable










