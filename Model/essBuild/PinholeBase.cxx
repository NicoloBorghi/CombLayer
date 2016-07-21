  /////////////////////////////////////////////////////////////////////////////

  PHType = "rect"; // Control.EvalVar<double>(keyName+"PHType"); // For now we only build a rectangular pinhole

  zImagingPlane = Control.EvalVar<double>(keyName+"ZImagingPlane") + 5.80;

  /////////////////////////////////////////////////////////////////////////////

  radialPHOffset = -1.0*Control.EvalVar<double>(keyName+"RadialPHOffset");

  if (fabs(radialPHOffset) >= (length/2.0)) {

    ELog::EM << "Radial Offset out of range. Setting to 0" << ELog::endErr;
    radialPHOffset = 0.0;

  }

  transversalPHOffset = -1.0*Control.EvalVar<double>(keyName+"TransversalPHOffset");
  //transversalPHOffset = -1.0*Control.EvalDefVar<double>(keyName+"TransversalPHOffset",0);

  if (fabs(transversalPHOffset) >= (width/2.0)) {

    ELog::EM << "Transversal Offset out of range. Setting to 0" << ELog::endDiag;
    transversalPHOffset = 0.0;

  }

  radialPHPos      = radialPHOffset*sin(90.0 + xyAngle) + transversalPHOffset*cos(90.0 + xyAngle);
  transversalPHPos = radialPHOffset*cos(90.0 + xyAngle) + transversalPHOffset*sin(90.0 + xyAngle);

  radialPHWidth = Control.EvalVar<double>(keyName+"RadialPHWidth");
  transversalPHWidth = Control.EvalVar<double>(keyName+"TransversalPHWidth");

  /////////////////////////////////////////////////////////////////////////////

  const Geometry::Plane *floorPlane = SMap.realPtr<Geometry::Plane>(DPfloorSurfaceNumber);

  height = zImagingPlane - floorPlane->getDistance();

  zDistanceFromImage = Control.EvalVar<double>(keyName+"ZDistanceFromImage");

  if (fabs(zDistanceFromImage) > height) {

    ELog::EM << "Pinhole distance from imaging plane out of range. Setting to half height" << ELog::endDiag;
    zDistanceFromImage = zImagingPlane/2.0;

  }

  zPHPos = zImagingPlane - zDistanceFromImage;

  ELog::EM << "X:" << transversalPHPos << " - Y: " << radialPHPos << " - Z: " << zPHPos << ELog::endDiag;
  ELog::EM << "xStep:" << xStep << " - yStep: " << yStep << " - zStep: " << zStep << ELog::endDiag;
