/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/targetVariables.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell/Konstantin Batkov
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

        void EssDiagnosticPlug(FuncDataBase& Control) {

                /*!
                        Variables that are used for the segmented wheel
                        \param Control :: Segment variables
                */

                // Diagnostic plug
                Control.addVariable("DiagnosticPlugXStep",-17.2);
                Control.addVariable("DiagnosticPlugYStep",211.0);
                Control.addVariable("DiagnosticPlugZStep",0.0);
                Control.addVariable("DiagnosticPlugXYangle",10.0);
                Control.addVariable("DiagnosticPlugZangle",0.0);
                Control.addVariable("DiagnosticPlugWidth",5.0);
                Control.addVariable("DiagnosticPlugLength",50.0);
                Control.addVariable("DiagnosticPlugActivationLayerThick1",1.0);
                Control.addVariable("DiagnosticPlugActivationLayerThick2",2.0);
                Control.addVariable("DiagnosticPlugActivationLayerThick3",5.0);
                Control.addVariable("DiagnosticPlugActivationLayerHeight",50.0);
                Control.addVariable("DiagnosticPlugActivationWallsMaterial","CastIron");
                /* |  */
                /* |  */
                /* +->*/Control.addVariable("RectangularPinholeRadialPinholeOffset",0.0);
                /* +->*/Control.addVariable("RectangularPinholeTransversalPinholeOffset",0.0);
                /* +->*/Control.addVariable("RectangularPinholeDistancePinholeImagingPlane",100.0);
                /* +->*/Control.addVariable("RectangularPinholeDistanceTargetSurfImagingPlane",200.0);
                /* |  */
                /* |  */
                /* +->*/Control.addVariable("RectangularPinholeRadialPinholeWidth",0.02);
                /* +->*/Control.addVariable("RectangularPinholeTransversalPinholeWidth",0.02);
                /* |  */
                /* |  */
                /* +->*/Control.addVariable("RectangularPinholeArrayRadialPinholeOffset",0.0);
                /* +->*/Control.addVariable("RectangularPinholeArrayTransversalPinholeOffset",0.0);
                /* +->*/Control.addVariable("RectangularPinholeArrayDistancePinholeImagingPlane",100.0);
                /* +->*/Control.addVariable("RectangularPinholeArrayDistanceTargetSurfImagingPlane",200.0);
                /* |  */
                /* |  */
                /* +->*/Control.addVariable("RectangularPinholeArrayRadialPinholeWidth",0.02);
                /* +->*/Control.addVariable("RectangularPinholeArrayTransversalPinholeWidth",0.02);
                /* |  */
                /* |  */
                /* +->*/Control.addVariable("RectangularPinholeArrayPinholeNumber",5);
                /* |  */
                /* |  */
//              /* |  */        /* --- NOT IMPLEMENTED YET --- */
//              /* +->*/Control.addVariable("CircularPinholeRadialPinholeOffset",0.0);
//              /* +->*/Control.addVariable("CircularPinholeTransversalPinholeOffset",0.0);
//              /* +->*/Control.addVariable("CircularPinholeDistancePinholeImagingPlane",100.0);
//              /* +->*/Control.addVariable("CircularPinholeDistanceTargetSurfImagingPlane",200.0);
                /* |  */
                /* |  */
//              /* +->*/Control.addVariable("CircularPinholeRadius",1.0);
                /* |  */
                /* |  */
                /* +->*/Control.addVariable("StrawCollimatorStrawWidth",0.1);
                /* +->*/Control.addVariable("StrawCollimatorStrawLength",0.1);
                /* +->*/Control.addVariable("StrawCollimatorRowDistance",0.1);
                /* +->*/Control.addVariable("StrawCollimatorDistanceTargetSurfImagingPlane",285.8);
                /* +->*/Control.addVariable("StrawCollimatorBulkMat","SS304");
                /* +->*/Control.addVariable("StrawCollimatorActivationLayerThick1",1.0);
                /* +->*/Control.addVariable("StrawCollimatorActivationLayerThick2",2.0);
                /* +->*/Control.addVariable("StrawCollimatorActivationLayerThick3",5.0);

                return;

        }
  
}  // NAMESPACE setVariable
