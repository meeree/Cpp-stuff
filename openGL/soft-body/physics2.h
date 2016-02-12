#include <iostream>
#include <cmath>
#include <cassert>
#include <vector>
#ifndef "physics2.h"
#include "physics2.h"
#endif
#ifndef "physics2.cpp"
#include "physics2.cpp"
#endif 

class jEng {
public:
   jEng (double const &cTimeStep, std::vector<pointMass> const &pointMasses);
   void setDownGravity (double const &cDownGrav);   
   void setSprings (std::vector<spring> &rSprings);
   void addPointMass (pointMass &rPointMass);
private:
   double mTimeStep;
   std::vector<pointMass> mPointMasses;
   bool mForceMethods [3];
   double mDownGrav;
};

class pointMass {
public:
   pointMass (double const &cMass, std::vector<double> const &cPosition, double const &cRadius = 0, std::vector<double> const &cVelocity = {0, 0, 0});
   void forces ();
   void updateAcceleration ();
   void integrate ();

private:
   double mMass;
   std::vector<double> mPosition;
   double mRadius;
   std::vector<double> mVelocity;
   std::vector<double> mAcceleration;
   std::vector<double> mForce;
};

struct spring {
   std::vector<unsigned> connection;
   double hookesConst;
   double restLength;
}
