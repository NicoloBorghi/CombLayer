/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachCompInc/FixedComp.h
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#ifndef attachSystem_FixedComp_h
#define attachSystem_FixedComp_h

class localRotate;
class HeadRule;


namespace attachSystem
{
/*!
  \class FixedComp
  \version 1.0
  \author S. Ansell
  \date March 2010
  \brief Component that is at a fixed position
*/

class FixedComp  
{
 protected:
  
  const std::string keyName;       ///< Key Name
  ModelSupport::surfRegister SMap; ///< Surface register

  Geometry::Vec3D X;            ///< X-coordinate [shutter x]
  Geometry::Vec3D Y;            ///< Y-coordinate [shutter y]
  Geometry::Vec3D Z;            ///< Z-coordinate 
  Geometry::Vec3D Origin;       ///< Origin  

  Geometry::Vec3D beamOrigin;   ///< Neutron origin [if different]
  Geometry::Vec3D beamAxis;     ///< Neutron direction [if different]

  std::vector<LinkUnit> LU;     ///< Linked unit items
  static void computeZOffPlane(const Geometry::Vec3D&,
			       const Geometry::Vec3D&,
			       Geometry::Vec3D&);
  void makeOrthogonal();
  
 public:

  FixedComp(const std::string&,const size_t);
  FixedComp(const std::string&,const size_t,const Geometry::Vec3D&);
  FixedComp(const std::string&,const size_t,
	    const Geometry::Vec3D&,const Geometry::Vec3D&,
	    const Geometry::Vec3D&,const Geometry::Vec3D&);
  FixedComp(const FixedComp&);
  FixedComp& operator=(const FixedComp&);
  virtual ~FixedComp() {}     ///< Destructor

  const LinkUnit& operator[](const size_t) const; 

  // Operator Set:
  void createUnitVector();
  void createUnitVector(const FixedComp&);
  void createUnitVector(const FixedComp&,const Geometry::Vec3D&);
  void createUnitVector(const FixedComp&,const long int);
  void createUnitVector(const Geometry::Vec3D&,const Geometry::Vec3D&,
			const Geometry::Vec3D&,const Geometry::Vec3D&);

  void setCentre(const Geometry::Vec3D&);
  void applyShift(const double,const double,const double);
  void applyAngleRotate(const double,const double);
  void applyAngleRotate(const double,const double,const double);
  void linkAngleRotate(const long int,const double,const double);
  void applyFullRotate(const double,const double,
		       const Geometry::Vec3D&);
  void reverseZ();
  
  void setConnect(const size_t,const Geometry::Vec3D&,const Geometry::Vec3D&);
  void setBasicExtent(const double,const double,const double);

  void setLinkSurf(const size_t,const FixedComp&,const size_t);
  void setLinkSurf(const size_t,const int);
  void setLinkSurf(const size_t,const std::string&);
  void setLinkSurf(const size_t,const HeadRule&);
  void addLinkSurf(const size_t,const int);
  void addLinkSurf(const size_t,const std::string&);

  void setBridgeSurf(const size_t,const FixedComp&,const size_t);
  void setBridgeSurf(const size_t,const int);
  void setBridgeSurf(const size_t,const HeadRule&);
  void addBridgeSurf(const size_t,const int);
  void addBridgeSurf(const size_t,const std::string&);

  void setLinkComponent(const size_t,const FixedComp&,const size_t);
  void setLinkCopy(const size_t,const FixedComp&,const size_t);
  void setLinkSignedCopy(const size_t,const FixedComp&,const long int);


  /// Get keyname
  const std::string& getKeyName() const 
    { return keyName; }
  /// Access X
  const Geometry::Vec3D& getX() const 
    { return X; }  
  /// Access Y direction
  const Geometry::Vec3D& getY() const 
    { return Y; }  
  /// Access Z direction
  const Geometry::Vec3D& getZ() const 
    { return Z; }
  /// Access centre
  virtual const Geometry::Vec3D& getCentre() const 
    { return Origin; }  
  /// Access beamOrigin
  virtual const Geometry::Vec3D& getBeamOrigin() const 
    { return beamOrigin; }  

  virtual int getExitWindow(const size_t,std::vector<int>&) const;
  virtual int getMasterSurf(const size_t) const;
			 
  void copyLinkObjects(const FixedComp&);
  /// How many connections
  size_t NConnect() const { return LU.size(); }
  void setNConnect(const size_t);
  const LinkUnit& getLU(const size_t)  const;

  LinkUnit& getSignedLU(const long int);
  const LinkUnit& getSignedLU(const long int)  const; 


  virtual int getLinkSurf(const size_t) const;
  virtual const Geometry::Vec3D& getLinkPt(const size_t) const;
  virtual const Geometry::Vec3D& getLinkAxis(const size_t) const;

  virtual Geometry::Vec3D getSignedLinkPt(const long int) const;
  virtual Geometry::Vec3D getSignedLinkAxis(const long int) const;
  virtual std::string getSignedLinkString(const long int) const;
  virtual int getSignedLinkSurf(const long int) const;
  HeadRule getSignedFullRule(const long int) const;
  HeadRule getSignedMainRule(const long int) const;
  HeadRule getSignedCommonRule(const long int) const;
  
  
  const HeadRule& getMainRule(const size_t) const;
  const HeadRule& getCommonRule(const size_t) const;

  virtual std::string getLinkString(const size_t) const;
  virtual std::string getLinkComplement(const size_t) const;
  virtual std::string getBridgeComplement(const size_t) const;

  virtual std::string getCommonString(const size_t) const;
  virtual std::string getCommonComplement(const size_t) const;

  virtual std::string getMasterString(const size_t) const;
  virtual std::string getMasterComplement(const size_t) const;

  size_t findLinkAxis(const Geometry::Vec3D&) const;


  const Geometry::Vec3D& getExit() const;
  const Geometry::Vec3D& getExitNorm() const;
  void selectAltAxis(const long int,Geometry::Vec3D&,
		     Geometry::Vec3D&,Geometry::Vec3D&) const;
  void calcLinkAxis(const long int,Geometry::Vec3D&,
		    Geometry::Vec3D&,Geometry::Vec3D&) const;

  virtual void applyRotation(const localRotate&);
  virtual void applyRotation(const Geometry::Vec3D&,const double);
  void setExit(const Geometry::Vec3D&,const Geometry::Vec3D&);

};

}

#endif
 
