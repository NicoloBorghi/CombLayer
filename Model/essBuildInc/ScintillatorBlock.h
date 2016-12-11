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
                                                              const attachSystem::FixedComp&,             /*strawsFC*/
                                                              const size_t,                               /*strawsLP*/
                                                              const attachSystem::FixedComp&,             /*roofFC*/
                                                              const size_t);                              /*roofLP*/

                                             void   setDimensions(double, double);                        ///< XY dimensions of the DiagnosticPlug

                protected:

                                           double   length;                                               ///< Length (radial) of the DiagnosticPlug
                                           double   width;                                                ///< Width (transversal) of the DiagnosticPlug
                                           double   height;                                               ///< Height of the collimator block
                                           double   scintWidth;                                           ///< Trasversal width of the scintillators
                                           double   scintLength;                                          ///< Radial length of the scintillators
                                           double   scintHeight;                                          ///< Height of the scintillators
                                           double   scintSeparation;                                      ///< Distance between the rows of scintillators (must match the Straws Row Distance)
                                           double   claddingRadius;                                       ///< Radius of the cladding
                                           double   claddingSeparation;                                   ///< Separation between the cladding (radial)
                                           double   claddingDepth;                                        ///< Depth of the center for cladding housing (transversal)

                                        const int   scintIndex;                                           ///< Index of surface offset
                                              int   nScint;                                               ///< Number of scintillators to match the straw grid
                                              int   nCladLeft;                                            ///< Number of cladding housings on the left side
                                              int   nCladRight;                                           ///< Number of cladding housings on the right side
                                              int   cellIndex;                                            ///< Cell index

                                              int   bulkMat;                                              ///< Scintillator block material
                                              int   scintMat;                                             ///< Scintillator material
                                              int   fiberMat;                                             ///< Material for fibers
                                              int   epoxyMat;                                             ///< Material for the epoxy holding the fibers

                                           double   zImagingPlane;                                        ///< Z coordinate of the imaging plane (which is the bottom plane for the scintillator box)
                                           double   zScintillatorTop;                                     ///< Z coordinate of the top plane for scintillators
                                           double   zBlockTop;                                            ///< Z coordinate of the top plane for the entire scintillator block

                                              int   imagingPlaneSurfaceNumber;                            ///< Imaging plane surface number (top of straw collimator)
                                              int   roofSurfaceNumber;                                    ///< Diagnostic plug roof surface number

                private:

                                             void   populate(const FuncDataBase&);

                                             void   createUnitVector(const attachSystem::FixedComp&);

                                             void   createSurfaces(const attachSystem::FixedComp&,        /*FC*/
                                                                   const attachSystem::FixedComp&,        /*strawsFC*/
                                                                   const size_t,                          /*strawsLP*/
                                                                   const attachSystem::FixedComp&,        /*roofFC*/
                                                                   const size_t);                         /*roofLP*/

                                             void   createObjects(Simulation&,
                                                                  attachSystem::FixedComp&,               /*FC*/
                                                                  const attachSystem::FixedComp&,         /*strawsFC*/
                                                                  const size_t,                           /*strawsLP*/
                                                                  const attachSystem::FixedComp&,         /*roofFC*/
                                                                  const size_t);                          /*roofLP*/

                                             void   createLinks();

        };

}

#endif

