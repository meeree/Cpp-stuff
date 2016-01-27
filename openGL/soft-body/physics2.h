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

class spring {
public:
   spring (std::vector<unsigned> const &cConnection, double const &cHookesConst);
   void updateRestLength ();
   void addConnection (unsigned const &cIndex);
   void deleteConnection (unsigned const &cIndex);
private:  
   std::vector<unsigned> mConnection;
   double mHookesConst; 
   double mRestLength;
};

class jEng {
public:
   jEng (double const &cTimeStep, std::vector<pointMass> const &pointMasses);
   void setDownGravity (double const &cDownGrav);   
   void setSprings (std::vector<spring> const &cSprings);
private:
   double mTimeStep;
   std::vector<pointMass> mPointMasses;
   bool mForces [3];

   double mDownGrav;

};
