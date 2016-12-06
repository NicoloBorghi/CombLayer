/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuildInc/ScintillatorBlock.h
 *
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef essSystem_ScintillatorBlock_h
#define essSystem_ScintillatorBlock_h

class Simulation;

namespace essSystem {

        /*!
                \class ScintillatorBlock
                \author Nicolò Borghi
                \version 1.0
                \date December 2016
                \brief Scintillator Block design for target diagnostic plug
        */

        class ScintillatorBlock  :  public attachSystem::ContainedComp,
                                    public attachSystem::FixedComp,
                                    public attachSystem::CellMap {

                public:

                                                    ScintillatorBlock(const std::string&);
                                                    ScintillatorBlock(const ScintillatorBlock&);
                                                    ScintillatorBlock& operator=(const ScintillatorBlock&);
                        virtual ScintillatorBlock  *clone() const;
                                          virtual  ~ScintillatorBlock();

                                             void   createAll(Simulation&,
                                                              attachSystem::FixedComp&,                   /*FC*/
                                                              const attachSystem::FixedComp&,             /*floorFC*/
                                                              const size_t,                               /*floorLP*/
                                                              const attachSystem::FixedComp&,             /*roofFC*/
                                                              const size_t);                              /*roofLP*/

                                             void   setDimensions(double, double, double);
                                             void   setBoundarySurfacesZ(double, double, double);

                protected:

                                      const int   scintIndex;                                           ///< Index of surface offset
                                            int   cellIndex;                                            ///< Cell index

                        // Variables relative to the DiagnosticPlug, necessary for building the straw collimator

                                         double   width;                                                ///< Width (obtained from DiagnosticPlug)
                                         double   length;                                               ///< Length (obtained from DiagnosticPlug)
                                         double   height;                                               ///< Height (obtained from DiagnosticPlug)
                                         double   floorSurfaceZ;                                        ///< Z coordinate of the floor surface
                                         double   roofSurfaceZ;                                         ///< Z coordinate of the roof surface
                                         double   targetTopSurfZ;                                       ///< Z coordinate of the target upper surface

                        // Variables relative to the imaging system

                                         double   heightImagingSystem;                                  ///< Height of the imaging system measured from the DiagnosticPlug floor to the imaging plane
                                         double   zImagingPlane;                                        ///< Elevation (z coordinate) of the imaging plane (relative to the target upper surface)
                                         double   distanceTargetSurfImagingPlane;                       ///< Distance between the target upper surface and the imaging plane

                        // Variables relative to the segmentation for activation calculations

                                         double   activationLayerThick1;
                                         double   activationLayerThick2;
                                         double   activationLayerThick3;

                        // Variables relative to the straw collimator

                                         double   strawWidth;                                           ///< Width of the straws in the transversal direction
                                         double   strawLength;                                          ///< Length of the straws in the radial direction
                                         double   rowDistance;                                          ///< Distance between straw rows
                                            int   nStraws;                                              ///< Number of straws that can be fitted inside the diagnostic plug

                                            int   bulkMat;                                              ///< Material for the collimator structure

                private:

                                           void   populate(const FuncDataBase&);

                                           void   createUnitVector(const attachSystem::FixedComp&);

                                           void   createSurfaces(const attachSystem::FixedComp&,        /*FC*/
                                                                 const attachSystem::FixedComp&,        /*floorFC*/
                                                                 const size_t,                          /*floorLP*/
                                                                 const attachSystem::FixedComp&,        /*roofFC*/
                                                                 const size_t);                         /*roofLP*/

                                           void   createObjects(Simulation&,
                                                                attachSystem::FixedComp&,               /*FC*/
                                                                const attachSystem::FixedComp&,         /*floorFC*/
                                                                const size_t,                           /*floorLP*/
                                                                const attachSystem::FixedComp&,         /*roofFC*/
                                                                const size_t);                          /*roofLP*/

                                           void   createLinks();

        };

}

#endif

