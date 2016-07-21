  // Get three points for the first PH collimator face

  const Geometry::Plane *pl1 = SMap.realPtr<Geometry::Plane>(tIndex+1);
  const Geometry::Plane *pl2 = SMap.realPtr<Geometry::Plane>(tIndex+3);
  const Geometry::Plane *pl3 = SMap.realPtr<Geometry::Plane>(tIndex+5);

  Geometry::Vec3D p1 = SurInter::getPoint(pl1,pl2,pl3);

  ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

  pl1 = SMap.realPtr<Geometry::Plane>(tIndex+1);
  pl2 = SMap.realPtr<Geometry::Plane>(tIndex+4);
  pl3 = SMap.realPtr<Geometry::Plane>(tIndex+5);

  Geometry::Vec3D p2 = SurInter::getPoint(pl1,pl2,pl3);

  ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

  Geometry::Vec3D p3( xStep + transversalPHPos + fabs((radialPHWidth/2.0)*cos(90.0 + xyAngle)),
                      yStep + radialPHPos      - fabs((radialPHWidth/2.0)*sin(90.0 + xyAngle)),
                      zStep + zPHPos);

  ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

  Geometry::Vec3D norm(cos(90.0 + xyAngle),sin(90.0 + xyAngle),cos(atan(zDistanceFromImage/((length-radialPHWidth)/2.0))));

  ELog::EM << "CROSS PRODUCT ==> X: " << norm.X() << " - Y: " << norm.Y() << " - Z: " << norm.Z() << ELog::endDiag;

  Geometry::Vec3D A(p3.X() - p1.X(), p3.Y() - p1.Y(), p3.Z() - p1.Z());
  Geometry::Vec3D B(p3.X() - p2.X(), p3.Y() - p2.Y(), p3.Z() - p2.Z());
  //ELog::EM << "CROSS PRODUCT ==> X: " << calculateNorm(A/A.abs(),B/B.abs()).X() << " - Y: " << calculateNorm(A/A.abs(),B/B.abs()).Y() << " - Z: " << calculateNorm(A/A.abs(),B/B.abs()).Z() << ELog::endDiag;

  ModelSupport::buildPlane(SMap,tIndex+7,p1,p2,p3,norm);
  
  ////////////////////////////////////////////////////

  // Get three points for the second PH collimator face

  pl1 = SMap.realPtr<Geometry::Plane>(tIndex+1);
  pl2 = SMap.realPtr<Geometry::Plane>(tIndex+3);
  pl3 = SMap.realPtr<Geometry::Plane>(DPfloorSurfaceNumber);

  p1 = SurInter::getPoint(pl1,pl2,pl3);

  ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

  pl1 = SMap.realPtr<Geometry::Plane>(tIndex+1);
  pl2 = SMap.realPtr<Geometry::Plane>(tIndex+4);
  pl3 = SMap.realPtr<Geometry::Plane>(DPfloorSurfaceNumber);

  p2 = SurInter::getPoint(pl1,pl2,pl3);

  ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

  p3( xStep + transversalPHPos + fabs((radialPHWidth/2.0)*cos(90.0 + xyAngle)),
      yStep + radialPHPos      - fabs((radialPHWidth/2.0)*sin(90.0 + xyAngle)),
      zStep + zPHPos);

  ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

  norm(-sin(xyAngle),cos(xyAngle),cos(atan((height-zDistanceFromImage)/((length-radialPHWidth)/2.0))));

  ModelSupport::buildPlane(SMap,tIndex+8,p1,p2,p3,norm);
  
  ////////////////////////////////////////////////////

  // Get three points for the third PH collimator face

  pl1 = SMap.realPtr<Geometry::Plane>(tIndex+2);
  pl2 = SMap.realPtr<Geometry::Plane>(tIndex+3);
  pl3 = SMap.realPtr<Geometry::Plane>(tIndex+5);

  p1 = SurInter::getPoint(pl1,pl2,pl3);

  ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

  pl1 = SMap.realPtr<Geometry::Plane>(tIndex+2);
  pl2 = SMap.realPtr<Geometry::Plane>(tIndex+4);
  pl3 = SMap.realPtr<Geometry::Plane>(tIndex+5);

  p2 = SurInter::getPoint(pl1,pl2,pl3);

  ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

  p3( xStep + transversalPHPos - fabs((radialPHWidth/2.0)*cos(90.0 + xyAngle)),
      yStep + radialPHPos      + fabs((radialPHWidth/2.0)*sin(90.0 + xyAngle)),
      zStep + zPHPos);

  ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

  norm(-sin(xyAngle),cos(xyAngle),-cos(atan(zDistanceFromImage/((length+radialPHWidth)/2.0))));

  ModelSupport::buildPlane(SMap,tIndex+17,p1,p2,p3,norm);
  
  ////////////////////////////////////////////////////

  // Get three points for the fourth PH collimator face

  pl1 = SMap.realPtr<Geometry::Plane>(tIndex+2);
  pl2 = SMap.realPtr<Geometry::Plane>(tIndex+3);
  pl3 = SMap.realPtr<Geometry::Plane>(DPfloorSurfaceNumber);

  p1 = SurInter::getPoint(pl1,pl2,pl3);

  ELog::EM << "Point 1 ==> X: " << p1.X() << " - Y: " << p1.Y() << " - Z: " << p1.Z() << ELog::endDiag;

  pl1 = SMap.realPtr<Geometry::Plane>(tIndex+2);
  pl2 = SMap.realPtr<Geometry::Plane>(tIndex+4);
  pl3 = SMap.realPtr<Geometry::Plane>(DPfloorSurfaceNumber);

  p2 = SurInter::getPoint(pl1,pl2,pl3);

  ELog::EM << "Point 2 ==> X: " << p2.X() << " - Y: " << p2.Y() << " - Z: " << p2.Z() << ELog::endDiag;

  p3( xStep + transversalPHPos - fabs((radialPHWidth/2.0)*cos(90.0 + xyAngle)),
      yStep + radialPHPos      + fabs((radialPHWidth/2.0)*sin(90.0 + xyAngle)),
      zStep + zPHPos);

  ELog::EM << "Point 3 ==> X: " << p3.X() << " - Y: " << p3.Y() << " - Z: " << p3.Z() << ELog::endDiag;

  norm(-sin(xyAngle),cos(xyAngle),-cos(atan((height-zDistanceFromImage)/((length+radialPHWidth)/2.0))));

  ModelSupport::buildPlane(SMap,tIndex+18,p1,p2,p3,norm);
  
  ////////////////////////////////////////////////////
























  Out=ModelSupport::getComposite(SMap,tIndex," 1 3 -4 -5 7 8") + strFloor;
  System.addCell(MonteCarlo::Qhull(cellIndex++, 0, 0.0, Out));

  Out=ModelSupport::getComposite(SMap,tIndex," -2 3 -4 -5 -17 -18") + strFloor;
  System.addCell(MonteCarlo::Qhull(cellIndex++, 0, 0.0, Out));

  Out=ModelSupport::getComposite(SMap,tIndex," -2 3 -4 -5 (-7:-8) (17:18)") + strFloor;
  System.addCell(MonteCarlo::Qhull(cellIndex++, 0, 0.0, Out));
