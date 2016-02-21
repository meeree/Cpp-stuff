#include "turtle.h"
#include <iostream>
#include <cmath>

turtle::turtle(double const (&position)[3], double const (&orientation)[3]) {
   double magnitude = 0; /*normalize orientation with magnitude*/
   magnitude += pow(orientation[0], 2);
   magnitude += pow(orientation[1], 2);
   magnitude += pow(orientation[2], 2);
   magnitude = sqrt(magnitude);
   for (int iComponent = 0; iComponent < 3; iComponent++) {
      mOrientation[iComponent] = orientation[iComponent] / magnitude;
      mPosition[iComponent] = position[iComponent];
   }
}

double * turtle::Rx(double const &theta) {
   double const oldOri = mOrientation[1];
   mOrientation[1] = mOrientation[1] * cos(theta) - mOrientation[2] * sin(theta);
   mOrientation[2] = oldOri * sin(theta) + mOrientation[2] * cos(theta);
   double * oriPtr = mOrientation;
   return oriPtr;
}

double * turtle::Ry(double const &theta) {
   double const oldOri = mOrientation[0];
   mOrientation[0] = mOrientation[0] * cos(theta) + mOrientation[2] * sin(theta);
   mOrientation[2] = -oldOri * sin(theta) + mOrientation[2] * cos(theta);
   double * oriPtr = mOrientation;
   return oriPtr;
}

double * turtle::Rz(double const &theta) {
   double const oldOri = mOrientation[0];
   mOrientation[0] = mOrientation[0] * cos(theta) - mOrientation[1] * sin(theta);
   mOrientation[1] = oldOri * sin(theta) + mOrientation[1] * cos(theta);
   double * oriPtr = mOrientation;
   return oriPtr;
}
   
double * turtle::Mv(double const &distance) {
   for (int iComponent = 0; iComponent < 3; iComponent++) {
      mPosition[iComponent] += distance * mOrientation[iComponent];
   }
   double * posPtr = mPosition;
   return posPtr;
}
