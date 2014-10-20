/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   processInc/mergeTemplate.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef ModelSupport_mergeTemplate_h
#define ModelSupport_mergeTemplate_h

namespace ModelSupport
{

/*!
  \class mergeTemplate
  \version 1.0
  \author S. Ansell
  \date June 2014
  \brief Fractionally splits several semi-parallel surface
  with full key templating for output control
*/

template<typename T,typename U>
class mergeTemplate : public surfDBase
{
 private:

  HeadRule InTemplate;           ///< Inner template
  HeadRule OutTemplate;          ///< Outer template

  HeadRule InRule;               ///< Inner Rule 
  HeadRule OutRule;              ///< Outer Rule

  HeadRule BaseUnit;             ///< Immutable base unit

  std::vector<int> pSurf;        ///< Primary surfaces 
  std::vector<int> sSurf;        ///< Secondary surfaces [signed]

  std::vector<const T*> primSPtr;       ///< Base surface
  std::vector<const U*> secSPtr;        ///< Second planes
  
  // Created as managed [No del required]
  std::vector<Geometry::Surface*> OSPtr;   ///< Output surfaces 

  void clearRules();
  void addRules();


 public:
  
  mergeTemplate();
  mergeTemplate(const mergeTemplate<T,U>&);
  mergeTemplate<T,U>& operator=(const mergeTemplate<T,U>&);
  virtual mergeTemplate<T,U>* clone() const;
  virtual ~mergeTemplate();

  // SETTING METHODS
  void setSurfPair(const int,const int);

  void setPrimarySurf(const int,const int);
  void addSecondarySurf(const int);

  void setDirection(const std::vector<int>&);

  virtual void populate();
  virtual int createSurf(const double,int&);
  virtual void processInnerOuter(const int,std::vector<Token>&) const;
  virtual void process(const HeadRule&) const;

  virtual void write(std::ostream&) const;

};

}

#endif
